/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/authentication_library/src/KeepSessionAliveThread.cpp $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#include <time.h>

#include "bus/security/authentication_library/src/KeepSessionAliveThread.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "bus/security/authentication_library/src/KeepSessionAliveThreadManager.h"

namespace
{
    const unsigned int TIME_IN_MILLISECS = 1000;
    const unsigned int MAX_ERRORS = 7;
}

namespace TA_Base_Bus
{
    KeepSessionAliveThread::KeepSessionAliveThread() : m_terminated(false),m_runStarted(false)
    {
    }


    KeepSessionAliveThread::~KeepSessionAliveThread()
    {
    }


    void KeepSessionAliveThread::run()
    {
        FUNCTION_ENTRY("run");
		
        // The thread is now running, so its not terminated.
        m_terminated = false;
		//indicate that run has been called
		m_runStarted = true;

		 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "start the keep alive thread" );

		//unsigned long runTimes = 1;

        while(!m_terminated)
        {
			//outputDebugInfomation(runTimes, "thread begin to keep alive operation", 2);

			{
				// Lock the list of sessions.
				TA_Base_Core::ThreadGuard guard( m_keepAliveListLock );

				bool shouldRemoveDeadSession = false;
				// Now keep each session alive.
				KeepAliveList::iterator keepAliveIt;
				for ( keepAliveIt=m_keepAliveList.begin(); keepAliveIt!=m_keepAliveList.end(); keepAliveIt++ )
				{
					const std::string sessionId = keepAliveIt->sessionInfo.SessionId;
					
					bool hasDeadSession = doHeartBeat(keepAliveIt, sessionId);
					if ( hasDeadSession )
					{
						shouldRemoveDeadSession = true;
					}
				}

				//	void KeepSessionAliveThread::removeDeadSession(KeepAliveList & keepAliveList )
				if ( shouldRemoveDeadSession )
				{
					removeDeadSession( m_keepAliveList );
				}

 			}
            {
                // lock out the terminate method 
                TA_Base_Core::ThreadGuard guard( m_condition.getLinkedThreadLockable() );
                if ( !m_terminated )
                {
                    if ( m_keepAliveList.size() > 0 )
                    {
                        // Wait for the heart beat interval.
                        if ( m_condition.timedWait( m_heartbeat*TIME_IN_MILLISECS ) )
                        {
                             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "keep alive thread, Signal recieved - time to work!" );
                        }
                        else
                        {
                             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "keep alive thread, Timer went off - same old stuff." );
                        }
                    }
                    else  // Wait for something to do
                    {
						//xinsong ++
						//note: if m_keepAliveList=0 and add session successful, then the siganl could not received
						//then will deadlock, but the remove session means that the MFT log off, so we don't care this case
                        m_condition.wait();
                         LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "keep alive thread, Signal recieved - do not wait?" );
                    }
                }
                else // m_terminted must be true - time to die
                {
                     LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "thread was asked to end" );
                }
            }
        }

        FUNCTION_EXIT;
    }


    void KeepSessionAliveThread::terminate()
    {
        FUNCTION_ENTRY("terminate");
		while(true)
		{
			if(m_runStarted)
			{
				TA_Base_Core::ThreadGuard guard( m_condition.getLinkedThreadLockable() );

				// Tell the thread to terminate.
				m_terminated = true;
				// Signal the thread to wake up if it's sleeping.
				m_condition.signal();
				break;
			}
			TA_Base_Core::Thread::sleep(10);
		}
		m_runStarted = false;
               
        FUNCTION_EXIT;
    }


    void KeepSessionAliveThread::setHeartbeat( const unsigned long& heartbeat )
    {
        FUNCTION_ENTRY("setHeartBeat");

        m_heartbeat = heartbeat;

        FUNCTION_EXIT;
    }


    void KeepSessionAliveThread::addSession( const SessionInfo& session )
    {
        FUNCTION_ENTRY("addSession");

        TA_Base_Core::ThreadGuard guard( m_keepAliveListLock );

        // Search for any sessions with the specified session ID.
        KeepAliveList::iterator foundSession = findSessionWithId( session.SessionId );
        if ( m_keepAliveList.end() == foundSession )
        {
             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Adding session '%s' to the keep alive list.", session.SessionId.c_str() );

            // Initialise the keep alive information.
            KeepAliveInfo keepAliveInfo;
            keepAliveInfo.errorCount = 0;
            keepAliveInfo.sessionInfo = session;

            // Add it to the list.
            m_keepAliveList.push_back( keepAliveInfo );

            // wake up the thread
            m_condition.signal();
        }
        else
        {
             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Session '%s' was already in the keep alive list, not adding.", session.SessionId.c_str() );
        }

        FUNCTION_EXIT;
    }


    void KeepSessionAliveThread::removeSession( const std::string& sessionId )
    {
        FUNCTION_ENTRY("removeSession");

        TA_Base_Core::ThreadGuard guard( m_keepAliveListLock );

        // Search for any sessions with the specified session ID.
        KeepAliveList::iterator foundSession = findSessionWithId( sessionId );
        if ( m_keepAliveList.end() != foundSession )
        {
            // One was found so remove it.
             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Removing session '%s' from the keep alive list.", sessionId.c_str() );
            m_keepAliveList.erase( foundSession );
        }
        else
        {
             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Session '%s' was not found in the keep alive list, not removing.", sessionId.c_str() );
        }

        FUNCTION_EXIT;
    }


    void KeepSessionAliveThread::removeAllSessions()
    {
        FUNCTION_ENTRY("removeAllSessions");

        TA_Base_Core::ThreadGuard guard( m_keepAliveListLock );

        // Clear the list.
        m_keepAliveList.clear();

        FUNCTION_EXIT;
    }


    bool KeepSessionAliveThread::canFindMatchingSession( const std::string & sessionId, SessionInfo& sessionFound )
    {
        FUNCTION_ENTRY("canFindMatchingSession");

        TA_Base_Core::ThreadGuard guard( m_keepAliveListLock );

        // Search the list for the first match.
        KeepAliveList::iterator foundSession = findSessionWithId( sessionId );
        if ( m_keepAliveList.end() != foundSession )
        {
            // A match was found so copy the session information into the 'out' parameter.
            sessionFound = foundSession->sessionInfo;

            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;
        return false;
    }


    KeepSessionAliveThread::KeepAliveList::iterator KeepSessionAliveThread::findSessionWithId( const std::string & sessionId )
    {	
		TA_Assert( !sessionId.empty() )
        // This method will only be called from methods inside this class that have already obtained the
        // lock.
        KeepAliveList::iterator keepAliveIt;
        for ( keepAliveIt=m_keepAliveList.begin(); keepAliveIt!=m_keepAliveList.end(); keepAliveIt++ )
        {
            if ( ( sessionId == keepAliveIt->sessionInfo.SessionId ) )
            {
				 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Session '%s' matched provided session details.", keepAliveIt->sessionInfo.SessionId.c_str() );
				return keepAliveIt;
            }
		}
		return m_keepAliveList.end();
    }
	

    KeepSessionAliveThread::EHeartbeatResult KeepSessionAliveThread::sendHeartbeatForSession( const std::string& sessionId )
    {
        FUNCTION_ENTRY("sendHeartbeatForSession");

        // Assume the heartbeat failed, but the failure should be ignored.
        EHeartbeatResult heartbeatResult = E_HB_FAILED_IGNORE;

        // Get the current time.
        time_t timeNow;
        time(&timeNow);

         LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "keepSessionAlive : %lu : %s", timeNow, sessionId.c_str() );

        try
        {
			if(! m_terminated) //
			{
				AuthenticationAgentNamedObj & authAgent = KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject(sessionId);
				CORBA_CALL( authAgent,
					keepSessionAlive,
					( timeNow, sessionId.c_str() ) );
				// The heartbeat was successful.
				heartbeatResult = E_HB_SUCCESSFUL;

			}
			else  //terminating
			{
				heartbeatResult = E_HB_FAILED_REMOVE;
			}
		
        }
        catch(const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationSecurityException", ex.description.in());

            if ( TA_Base_Bus::IAuthenticationAgentCorbaDef::INVALID_SESSION == ex.reason )
            {
                // If the exception indicates that the session is invalid there is no point in retrying.
                heartbeatResult = E_HB_FAILED_REMOVE;
            }
            else
            {
                // Otherwise it is an ordinary failure.
                heartbeatResult = E_HB_FAILED;
            }
        }
        catch(const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", ex.reason.in());

            // Normal failure.
            heartbeatResult = E_HB_FAILED;
        }
        catch(const CORBA::Exception& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
        }
        catch(const TA_Base_Core::ObjectResolutionException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Authentication agent threw an exception when trying to keepSessionAlive");
        }

        FUNCTION_EXIT;
        return heartbeatResult;
    }

	bool KeepSessionAliveThread::doHeartBeat(KeepAliveList::iterator & keepAliveIt, const std::string & sessionId )
	{
        // Send the heartbeat.
		bool exitFor = false;

		KeepSessionAliveThread::EHeartbeatResult heartBeatResult = sendHeartbeatForSession( sessionId );
        switch ( heartBeatResult )
        {
        case E_HB_SUCCESSFUL:
            keepAliveIt->errorCount = 0;
             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Heartbeat successful for session '%s'.", sessionId.c_str() );
            break;

        case E_HB_FAILED:
//            if ( ++keepAliveIt->errorCount >= MAX_ERRORS )
//            {
//                 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed heartbeat for session '%s'. Error count exceeded to removing session from keep alive list.", sessionId.c_str() );
//                // KeepAliveIt may now be invalid so...
//                exitFor = true;
//            }
//            else
            {
                 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed heartbeat for session '%s'. %d attempts remaining.", sessionId.c_str(), MAX_ERRORS-keepAliveIt->errorCount );
            }
            break;

        case E_HB_FAILED_IGNORE:
             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed heartbeat for session '%s'. Ignoring failure.", sessionId.c_str() );
            break;

        case E_HB_FAILED_REMOVE:
             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed heartbeat for session '%s'. Removing session from keep alive list.", sessionId.c_str() );
            keepAliveIt->errorCount++;
            // KeepAliveIt may now be invalid so...
            exitFor = true;
            break;

        default:
             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown enun value returned for session '%s'.", sessionId.c_str() );
            break;
        }
		
		return exitFor;
	}

	void KeepSessionAliveThread::removeDeadSession( KeepAliveList & keepAliveList )
	{
		KeepAliveList tempList;
        KeepAliveList::iterator keepAliveIt;
        for ( keepAliveIt=m_keepAliveList.begin(); keepAliveIt!=m_keepAliveList.end(); keepAliveIt++ )
        {
            if ( ( keepAliveIt->errorCount < MAX_ERRORS ) )
            {
				tempList.push_back(*keepAliveIt);
            }
			else
			{
				std::string  sessionId = keepAliveIt->sessionInfo.SessionId;
				 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "remove dead session <%s> from keep alive list.", sessionId.c_str() );
			}
		}

		keepAliveList.clear();
		keepAliveList.assign(tempList.begin(), tempList.end());

	}

	void KeepSessionAliveThread::outputDebugInfomation( unsigned long & runTimes , const char * debugInfo, unsigned long baseValue, unsigned long largestValue)
	{
		if ( (runTimes % baseValue) == 0 )
		{
			 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, debugInfo );
			if ( runTimes > largestValue)
			{
				runTimes = 1;
			}
		}
	}
};
