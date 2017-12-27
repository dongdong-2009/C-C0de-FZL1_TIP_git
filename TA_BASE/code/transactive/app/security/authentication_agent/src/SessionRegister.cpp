/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/SessionRegister.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class is contains the list of current sessions. It provides thread-safe
 * access to that list (as the list can be updated from more than one place), and
 * stores the profiles associated with a given session with the session key.
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)   // Decorated name too long
#endif // defined(_MSC_VER)

#include <time.h>

#include "app/security/authentication_agent/src/SessionRegister.h"

#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
using TA_Base_Bus::Authentication::BASE_OPERATOR;
using TA_Base_Bus::Authentication::BASE_PROFILE;
using TA_Base_Bus::SessionInfo;

using TA_Base_Core::AuthenticationAgentException;
using TA_Base_Core::AuthenticationSecurityException;

using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    SessionRegister::SessionRegister( bool isControl, unsigned long entityKey )
    {
        FUNCTION_ENTRY("SessionRegister::SessionRegister() constructor");

        // If the session register is being constructed while the agent is already in control mode
        // then this agent is the only one and cannot rely on state updates to determine which currently
        // active sessions should have active times stored. This information can instead be obtained from
        // the database.
        if( isControl )
        {
            std::vector<TA_Base_Core::ISession*> ownedSessions;
			std::vector<TA_Base_Core::ISession*>::const_iterator it;

			try
			{
				ownedSessions = TA_Base_Core::SessionAccessFactory::getInstance().getOwnedActiveSessions( entityKey );
			}
			catch(TA_Base_Core::DataException& ex)
			{
				// If this is of type NO_VALUE, throw a security exception, otherwise throw an agent exception
				if (TA_Base_Core::DataException::NO_VALUE == ex.getFailType())
				{
					LOG_EXCEPTION_CATCH (SourceInfo, "TA_Base_Core::DataException", "There is no active session in the database.");
				}
				else
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", ex.what() );
					TA_THROW(AuthenticationAgentException("An error occurred while getting active session from database") );
				}
			}

            for( it=ownedSessions.begin(); it!=ownedSessions.end(); it++ )
            {
                //m_preOwnedSessions.insert((*it)->getSessionId());

				// ly++ for CL-20784, date:20090420
				// init m_ghostSessionMap
				if ( !(*it)->getHasEnded() )
				{
					TA_Base_Bus::SessionInfo sessionInfo;
					sessionInfo.UserId = (*it)->getOperatorKey();
					sessionInfo.OperatorId.push_back( (*it)->getOperatorKey() );
					sessionInfo.ProfileId.push_back( (*it)->getProfileKey() );
					sessionInfo.SessionId = (*it)->getSessionId();
					sessionInfo.LocationId = (*it)->getLocationKey();
					sessionInfo.WorkstationId = (*it)->getConsoleKey();
					sessionInfo.isDisplayOnly = (*it)->getDisplayMode();
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"[CL-20784] SessionRegister: adding sesssion that maybe a ghost:%s",
						sessionInfo.SessionId.c_str() );

					TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
					m_ghostSessionMap.insert( TheRegister::value_type( sessionInfo.SessionId, sessionInfo ) );
					
					// when Authentication setControl() called, updateAllSessionsActiveTime() will update all
					// time to current system time, so we just initialize active time to zero
					// in DeadSessionCheckerThread run(), those session will keep alive in first m_timeout period,
					// then endSession() condition will be triggered.
					TA_Base_Core::ThreadGuard guardB( m_activeTimesAccessLock );
					m_sessionActiveTimes.insert( std::make_pair<std::string, time_t>( sessionInfo.SessionId, time(0) ) );
				}
				// ++ly

                delete *it;
            }
        }

        FUNCTION_EXIT;
    }


    SessionRegister::~SessionRegister()
    {
    }


    void SessionRegister::addSession( const TA_Base_Bus::SessionInfo & session, bool addActiveTime /* = false */)
    {
        FUNCTION_ENTRY("addSession");

        TA_Base_Core::ThreadGuard guard(m_registerAccessLock);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Adding session '%s' to the register.",session.SessionId.c_str());

		/*
        if( !addActiveTime && existInPreOwnedSession(session.SessionId) )
        {
            addActiveTime = true;
            m_preOwnedSessions.erase(session.SessionId);

			// ly++ for CL-20784, date:20090420
			// removeGhostSession( session.SessionId );
			// ++ly
        }
		*/
		removeGhostSession(session.SessionId);

        // find the session in the map
        TheRegister::iterator iter (m_sessionRegister.find(session.SessionId));
        
        // If the session already exists on the map, or the sessionId is of 0 length, it is bad.
        if (iter != m_sessionRegister.end() || 0 == session.SessionId.length())
        {
            // TD14332 -- make sure the session has an entry in m_sessionActiveTimes for session update
            if(addActiveTime)
            {
                addSessionActiveTime( session.SessionId );
            }
            // The session is already present so throw a meaningful exception.
            std::stringstream exMsg;
            exMsg << "Session '" << session.SessionId << "' was already found in the register when adding.";
            TA_THROW( AuthenticationAgentException( exMsg.str().c_str() ) );
        }
        else
        {
            // Item is not present yet so add it
            m_sessionRegister.insert(TheRegister::value_type(session.SessionId, session) );

			//TD15069 azenitha++
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"[TD15069] m_sessionRegister.insert() called, adding session %s to register", session.SessionId.c_str());
			//TD15069 azenitha++

            // AnitaL : Fix for TD 1826 : Don't want to add the session active time if this came from a broadcast
            if(addActiveTime)
            {
                addSessionActiveTime( session.SessionId );
            }
        }

        FUNCTION_EXIT;
    }


    void SessionRegister::removeSession( const std::string& sessionId )
    {
        FUNCTION_ENTRY("removeSession");

        TA_Base_Core::ThreadGuard guard( m_registerAccessLock );

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Removing session '%s' from the register.",sessionId.c_str());

        // find the session in the map
        TheRegister::iterator registerIter( m_sessionRegister.find(sessionId) );
        if ( m_sessionRegister.end() == registerIter )
        {
            // The session wasn't found so throw a meaningful exception.
            std::stringstream exMsg;
            exMsg << "Could not find session '" << sessionId << "' in the register while removing.";

            TA_THROW( AuthenticationSecurityException( exMsg.str().c_str(),
                AuthenticationSecurityException::INVALID_SESSION ) );
        }

        // The session is in the register so remove it.
        m_sessionRegister.erase( registerIter );

		//TD15069 azenitha++
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"[TD15069] m_sessionRegister.erase() called, remove session: %s from register", sessionId.c_str());
		//TD15069 azenitha++

        // If the session was removed from the session register it should also be removed from the session
        // active times map if it exists.
        removeSessionActiveTime( sessionId );

        FUNCTION_EXIT;
    }


    void SessionRegister::addOverride(const std::string& sessionId,unsigned long operatorId,unsigned long profileId)
    {
        FUNCTION_ENTRY("addOverride");

        TA_Base_Core::ThreadGuard guard( m_registerAccessLock );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Adding override for session '%s' (O:%d, P:%d).", sessionId.c_str(),
            operatorId, profileId );

        // Find the session in the map
        TheRegister::iterator iter (m_sessionRegister.find(sessionId));
        if (iter == m_sessionRegister.end())
        {
            // The session wasn't found so throw a meaningful exception.
            std::stringstream exMsg;
            exMsg << "Could not find session '" << sessionId << "' in the register while overriding.";

            TA_THROW( AuthenticationSecurityException( exMsg.str().c_str(),
                AuthenticationSecurityException::INVALID_SESSION ) );
        }

        // The session is on the map - add the operator and profile to it
        iter->second.OperatorId.push_back(operatorId);
        iter->second.ProfileId.push_back(profileId);

        FUNCTION_EXIT;
    }


    void SessionRegister::clearOverrides(const std::string& sessionId)
    {
        FUNCTION_ENTRY("clearOverrides");

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Clearing overrides for session '%s'.", sessionId.c_str() );

        // Find the session in the map
        TheRegister::iterator iter (m_sessionRegister.find(sessionId));
        if ( m_sessionRegister.end() == iter )
        {
            // The session wasn't found so throw a meaningful exception.
            std::stringstream exMsg;
            exMsg << "Could not find session '" << sessionId << "' in the register while clearing overrides.";

            TA_THROW( AuthenticationSecurityException( exMsg.str().c_str(),
                AuthenticationSecurityException::INVALID_SESSION ) );
        }

        // check that the profile vector is not empty - it is theoretically an "impossible" situation,
        // so it is guarded by an assert (which will prevent a memory error if it ever DOES occurr).
        TA_ASSERT(iter->second.ProfileId.size() > 0,"The ProfileId Vector is empty");

        // Save the base operator and profile from the lists of operators and profiles.
        unsigned long baseOperator = iter->second.OperatorId[BASE_OPERATOR];
        unsigned long baseProfile = iter->second.ProfileId[BASE_PROFILE];
        
        // Clear the operator and profile lists.
        iter->second.OperatorId.clear();
        iter->second.ProfileId.clear();

        // Add the base operator and profile back again.
        iter->second.OperatorId.push_back(baseOperator);
        iter->second.ProfileId.push_back(baseProfile);

        FUNCTION_EXIT;
    }


    bool SessionRegister::isSessionInRegister( const std::string& sessionId ) const
    {
        FUNCTION_ENTRY("isSessionInRegister");

        TA_Base_Core::ThreadGuard guard( m_registerAccessLock );

        TheRegister::const_iterator iter = m_sessionRegister.find( sessionId );

        FUNCTION_EXIT;
        return m_sessionRegister.end() != iter;
    }


    const TA_Base_Bus::SessionInfo& SessionRegister::getSession(const std::string& sessionId)
    {
        FUNCTION_ENTRY("getSession");

		bool exists = false;

		{
			TA_Base_Core::ThreadGuard guard( m_registerAccessLock );

			// Find the session in the map
			TheRegister::const_iterator iter (m_sessionRegister.find(sessionId));
			exists = (iter != m_sessionRegister.end());
			if(exists) 
			{
				FUNCTION_EXIT;
				return iter->second;
			}
		}

        if (!exists)
        {
			//xinsong ++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not find the session<%s> in session Register, we will retrieve info from DB", 
				sessionId.c_str());
			/*
			if ( ! existInPreOwnedSession(sessionId) )
			{
				// The session wasn't found so throw a meaningful exception.
				std::stringstream exMsg;
				exMsg << "Could not find session '" << sessionId << "' in the register while getting session details.";

				TA_THROW( AuthenticationSecurityException( exMsg.str().c_str(),
					AuthenticationSecurityException::INVALID_SESSION ) );
			}
			else //load info from db
			{
				// ly++ for CL-20784
				if ( isGhostSession(sessionId) )
				{
					TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
					iter = m_ghostSessionMap.find( sessionId );

					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
						"[CL-20784] getSession return from ghost sessionlist, session:%s", 
						sessionId.c_str());
					
					TA_Assert( iter != m_ghostSessionMap.end() );				
				}
				else
				{
					TA_Base_Bus::SessionInfo tmpSessionInfo = getSessionInfoFromDb(sessionId);
					addSession( tmpSessionInfo); //no use to add active time explicitly
					iter = m_sessionRegister.find(sessionId);
					TA_Assert( iter != m_sessionRegister.end() );
				}
			}
			*/
			if ( isGhostSession(sessionId) )
			{
				TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
				TheRegister::const_iterator iter = m_ghostSessionMap.find( sessionId );
				
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
					"[CL-20784] getSession return from ghost sessionlist, session:%s", 
					sessionId.c_str());
				
				TA_Assert( iter != m_ghostSessionMap.end() );

				FUNCTION_EXIT;
				return  iter->second;
			}
			else
			{
				TA_Base_Bus::SessionInfo tmpSession;
				bool hasEnded;
				if ( getOwnedSessionInfoFromDb(sessionId, tmpSession,hasEnded) )
				{
					/*
					addSession( tmpSession );
					iter = m_sessionRegister.find( sessionId );
					TA_Assert( iter != m_sessionRegister.end() );
					*/	
				if(!hasEnded)
				{
					{
						TA_Base_Core::ThreadGuard guardB( m_activeTimesAccessLock );
						m_sessionActiveTimes.insert( std::make_pair<std::string, time_t>( tmpSession.SessionId, time(0) ) );
					}
				
					{
						TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
						m_ghostSessionMap.insert( TheRegister::value_type( tmpSession.SessionId, tmpSession ) );
						TheRegister::const_iterator itr = m_ghostSessionMap.find( sessionId );
						TA_Assert( itr != m_ghostSessionMap.end() );

						FUNCTION_EXIT;
						return itr->second;
					}			
				}		
				//if the loaded session is ended, then ,just return the session info
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, 
						"Ended session from database: %s", 
						tmpSession.SessionId.c_str());
					FUNCTION_EXIT;
					return tmpSession;
				}

				}else{
					// The session wasn't found so throw a meaningful exception.
					std::stringstream exMsg;
					exMsg << "Could not find session '" << sessionId << "' in the register while getting session details.";
					
					TA_THROW( AuthenticationSecurityException( exMsg.str().c_str(),
						AuthenticationSecurityException::INVALID_SESSION ) );
				}
			}
			//+-chenlei CL-21183
        }

        // The session is on the map - return it

    }


    const std::vector<unsigned long>& SessionRegister::getProfiles(const std::string& sessionId) const
    {
        FUNCTION_ENTRY("getProfiles");

        TA_Base_Core::ThreadGuard guard( m_registerAccessLock );

        // Find the session in the map
        TheRegister::const_iterator iter (m_sessionRegister.find(sessionId));
        if ( m_sessionRegister.end() == iter )
        {
            // The session wasn't found so throw a meaningful exception.
            std::stringstream exMsg;
            exMsg << "Could not find session '" << sessionId << "' in the register while getting session profiles.";

            TA_THROW( AuthenticationSecurityException( exMsg.str().c_str(),
                AuthenticationSecurityException::INVALID_SESSION ) );
        }

        FUNCTION_EXIT;
        return iter->second.ProfileId;
    }

	//not return reference,copy whole content, afraid the other thread will modify the content
    void SessionRegister::getRegister(std::vector<TA_Base_Bus::SessionInfo> & vtSessinInfo ) const
    {
        FUNCTION_ENTRY("getRegister");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,"[2011-11-09] SessionRegister::getRegister!");
        TA_Base_Core::ThreadGuard guard(m_registerAccessLock);

		vtSessinInfo.clear();

        // Create the begin and end iterators for the map
        TheRegister::const_iterator iter (m_sessionRegister.begin());
        TheRegister::const_iterator iterEnd (m_sessionRegister.end());


        // Now loop through the entires on the map
        while (iter != iterEnd)
        {
            // Add each one to the vector
            vtSessinInfo.push_back(iter->second);
            ++iter;
        }
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"[2011-11-09] SessionRegister::getRegister leaving!");

        FUNCTION_EXIT;
    }


    void SessionRegister::addSessionActiveTime( const std::string& sessionId )
    {
        TA_Base_Core::ThreadGuard guard( m_activeTimesAccessLock );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Adding active time for session '%s'.",
            sessionId.c_str() );

        // Set the current time as the last known time the session was active
        std::pair<std::map<std::string, time_t>::iterator,bool> result;
        result = m_sessionActiveTimes.insert( std::map<std::string, time_t>::value_type( sessionId, time(0) ) );

        // For debugging purposes, log if the an active time already existed for this session.
        if ( !result.second )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Active time already exists for session '%s'.",
                sessionId.c_str() );
        }

        LOG1( SourceInfo, DebugUtil::DebugInfo, "[TD15069] addSessionActiveTime for session '%s'.",
                sessionId.c_str() );
    }


    void SessionRegister::updateSessionActiveTime(time_t timestamp, const std::string& sessionId)
    {
		bool exists = false;
		{
			TA_Base_Core::ThreadGuard guard( m_activeTimesAccessLock );

			// Find the session in the map
			std::map<std::string, time_t>::iterator iter (m_sessionActiveTimes.find(sessionId));
			exists = (iter !=m_sessionActiveTimes.end() );
			if(exists) iter->second = timestamp;
		}
        if ( !exists )
        {
            // TD14388
            try
            {
                getSession(sessionId);
                // add sessionActiveTime entry if it's a valid session but not inside m_sessionActiveTimes.
                LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Adding active time for session '%s'.",
                    sessionId.c_str() );

				TA_Base_Core::ThreadGuard guard( m_activeTimesAccessLock );
                // Set the current time as the last known time the session was active
                std::pair<std::map<std::string, time_t>::iterator,bool> result;
                result = m_sessionActiveTimes.insert( std::map<std::string, time_t>::value_type( sessionId, time(0) ) );
            }
            catch (AuthenticationSecurityException&)
            {
                // The session wasn't found so throw a meaningful exception.
                std::stringstream exMsg;
                exMsg << "Couldn't find session '" << sessionId << "' in the map of session times while updating.";

                TA_THROW( AuthenticationSecurityException( exMsg.str().c_str(),
                    AuthenticationSecurityException::INVALID_SESSION ) );
            }
        }
		
    }


    // TD14332
    void SessionRegister::updateAllSessionsActiveTime()
    {
        FUNCTION_ENTRY("updateAllSessionsActiveTime");

        time_t serverTimestamp;
        time(&serverTimestamp);
		unsigned long sessionActiveTimes = 0;
		{
			// Now loop through the entires on the map
			TA_Base_Core::ThreadGuard guard( m_activeTimesAccessLock );
			std::map<std::string, time_t>::iterator iter;
			for( iter=m_sessionActiveTimes.begin(); iter!=m_sessionActiveTimes.end(); iter++ )
			{
				iter->second = serverTimestamp;
			}
			sessionActiveTimes = m_sessionActiveTimes.size();
		}
		{
			TA_Base_Core::ThreadGuard guard(m_registerAccessLock);
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "updateAllSessionsActiveTime total session time in m_sessionActiveTimes is %ul, total sessions in m_sessionRegister is %ul.",
				sessionActiveTimes,  m_sessionRegister.size());
		}

        FUNCTION_EXIT;
    }


    void SessionRegister::removeSessionActiveTime( const std::string& sessionId )
    {
        TA_Base_Core::ThreadGuard guard( m_activeTimesAccessLock );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Removing active time for session '%s'.",
            sessionId.c_str() );

        std::map<std::string, time_t>::iterator timeIter(m_sessionActiveTimes.find(sessionId));
        if ( timeIter != m_sessionActiveTimes.end())
        {
            // The session was found in the map so remove it.
            m_sessionActiveTimes.erase(timeIter);
            LOG1( SourceInfo, DebugUtil::DebugInfo, "[TD15069] removeSessionActiveTime for session '%s'.",
                sessionId.c_str() );
        }
        else
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Active time doesn't exist for session '%s'.",
                sessionId.c_str() );
        }
    }


    bool SessionRegister::getLastSessionActiveTime(const std::string& sessionId, time_t& activeTime, time_t agentStartUpTime)
    {
        // find the session in the map
        std::map<std::string, time_t>::iterator iter (m_sessionActiveTimes.find(sessionId));

        if (iter != m_sessionActiveTimes.end())
        {
            activeTime = iter->second;
            return true;
        }

        LOG1( SourceInfo, DebugUtil::DebugError, "[TD15069] could not getLastSessionActiveTime for session '%s', use agent start up time",
                sessionId.c_str() );

		//xinsong++, use agent start up time
		activeTime = agentStartUpTime;	
        return false;
    }

	SessionInfo SessionRegister::getSessionInfoFromDb( const std::string & sessionId ) const
	{
        // Wrap the lot in a try-catch - database code can throw exceptions
        try
        {
            std::auto_ptr<TA_Base_Core::ISession> session( TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId) );

			// Create the session object to put the information in
			SessionInfo sessionInfo;

            // Copy the details into the session object
            sessionInfo.UserId = session->getOperatorKey();
            sessionInfo.OperatorId.push_back( session->getOperatorKey() );
            sessionInfo.ProfileId.push_back(session->getProfileKey());
            sessionInfo.SessionId = sessionId;
            sessionInfo.LocationId = session->getLocationKey();
            sessionInfo.WorkstationId = session->getConsoleKey();
			sessionInfo.isDisplayOnly = session->getDisplayMode();
			return sessionInfo;
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            // Log the catch, and re-throw as an AuthenticationAgentException
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException",
                "Database exception thrown while retrieving session information from the database, with the following error: "
                + std::string(ex.what()));

            std::stringstream exMsg;
            exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
                  << ex.what() << ").";
            TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
        }
        catch(TA_Base_Core::DataException& ex)
        {
            // Log the catch, and re-throw as an AuthenticationAgentException
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException",
                "Data exception thrown while retrieving session information from the database, with the following error: "
                + std::string(ex.what()));

            std::stringstream exMsg;
            exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
                  << ex.what() << ").";
            TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
        }
        catch(TA_Base_Core::DataConfigurationException& ex)
        {
            // Log the catch, and re-throw as an AuthenticationAgentException
            LOG_EXCEPTION_CATCH(SourceInfo, "DataConfigurationException",
                "Data Configuration exception thrown while retrieving session information from the database, with the following error: "
                + std::string(ex.what()));

            std::stringstream exMsg;
            exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
                  << ex.what() << ").";
            TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
        }
        catch(...)
        {
            // Log the catch, and re-throw as an AuthenticationAgentException
            LOG_EXCEPTION_CATCH(SourceInfo, "...",
                "Unknown exception thrown while retrieving session information from the database");

            std::stringstream exMsg;
            exMsg << "An unknown error occurred while loading session information for session " << sessionId << " from the database.";
            TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
        }
	}

	bool SessionRegister::getOwnedSessionInfoFromDb( const std::string& sessionId, TA_Base_Bus::SessionInfo& outputSession)
	{
		// Wrap the lot in a try-catch - database code can throw exceptions
        try
        {
            std::auto_ptr<TA_Base_Core::ISession> session( TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId) );
			if (session->getEntityKey() == m_entityKey)
			{
				// Copy the details into the session object
				outputSession.UserId = session->getOperatorKey();
				outputSession.OperatorId.push_back( session->getOperatorKey() );
				outputSession.ProfileId.push_back(session->getProfileKey());
				outputSession.SessionId = sessionId;
				outputSession.LocationId = session->getLocationKey();
				outputSession.WorkstationId = session->getConsoleKey();
				outputSession.isDisplayOnly = session->getDisplayMode();

				//CL-21875
				//if ( sessionId.length() != 0 )
				//{
				//	loadOvrrideInfomation(sessionId,outputSession);
				//}

				//if ( (m_operatorOverrideKey >=0 ) || (m_seprofOverrideKey >= 0) )
				//{
				//	outputSession.OperatorId.push_back( static_cast<unsigned long>(m_operatorOverrideKey) );
				//	outputSession.ProfileId.push_back( static_cast<unsigned long>(m_seprofOverrideKey) );
				//}

				return true;
			}
			else
			{
				return false;
			}
			
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            // Log the catch, and re-throw as an AuthenticationAgentException
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException",
                "Database exception thrown while retrieving session information from the database, with the following error: "
                + std::string(ex.what()));
			
            std::stringstream exMsg;
            exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
            TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
        }
        catch(TA_Base_Core::DataException& ex)
        {
            // Log the catch, and re-throw as an AuthenticationAgentException
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException",
                "Data exception thrown while retrieving session information from the database, with the following error: "
                + std::string(ex.what()));
			
            std::stringstream exMsg;
            exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
            TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
        }
        catch(TA_Base_Core::DataConfigurationException& ex)
        {
            // Log the catch, and re-throw as an AuthenticationAgentException
            LOG_EXCEPTION_CATCH(SourceInfo, "DataConfigurationException",
                "Data Configuration exception thrown while retrieving session information from the database, with the following error: "
                + std::string(ex.what()));
			
            std::stringstream exMsg;
            exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
            TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
        }
        catch(...)
        {
            // Log the catch, and re-throw as an AuthenticationAgentException
            LOG_EXCEPTION_CATCH(SourceInfo, "...",
                "Unknown exception thrown while retrieving session information from the database");
			
            std::stringstream exMsg;
            exMsg << "An unknown error occurred while loading session information for session " << sessionId << " from the database.";
            TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
        }
	
	}

	bool SessionRegister::getOwnedSessionInfoFromDb( const std::string& sessionId, TA_Base_Bus::SessionInfo& outputSession,bool& hasEnded)
	{
		// Wrap the lot in a try-catch - database code can throw exceptions
		try
		{
			std::auto_ptr<TA_Base_Core::ISession> session( TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId) );
			if (session->getEntityKey() == m_entityKey)
			{
				//session->resetHasEnded();
				//session->applyChanges();

				// Copy the details into the session object
				outputSession.UserId = session->getOperatorKey();
				outputSession.OperatorId.push_back( session->getOperatorKey() );
				outputSession.ProfileId.push_back(session->getProfileKey());
				outputSession.SessionId = sessionId;
				outputSession.LocationId = session->getLocationKey();
				outputSession.WorkstationId = session->getConsoleKey();
				outputSession.isDisplayOnly = session->getDisplayMode();

				hasEnded = session->getHasEnded();
				//CL-21875
				if ( sessionId.length() != 0 )
				{
					loadOvrrideInfomation(sessionId,outputSession);
				}

				//if ( (m_operatorOverrideKey >=0 ) || (m_seprofOverrideKey >= 0) )
				//{
				//	outputSession.OperatorId.push_back( static_cast<unsigned long>(m_operatorOverrideKey) );
				//	outputSession.ProfileId.push_back( static_cast<unsigned long>(m_seprofOverrideKey) );
				//}

				return true;
			}
			else
			{
				return false;
			}

		}
		catch(TA_Base_Core::DatabaseException& ex)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException",
				"Database exception thrown while retrieving session information from the database, with the following error: "
				+ std::string(ex.what()));

			std::stringstream exMsg;
			exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
			TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
		}
		catch(TA_Base_Core::DataException& ex)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException",
				"Data exception thrown while retrieving session information from the database, with the following error: "
				+ std::string(ex.what()));

			std::stringstream exMsg;
			exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
			TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
		}
		catch(TA_Base_Core::DataConfigurationException& ex)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "DataConfigurationException",
				"Data Configuration exception thrown while retrieving session information from the database, with the following error: "
				+ std::string(ex.what()));

			std::stringstream exMsg;
			exMsg << "An error occurred while loading session information for session " << sessionId << " from the database ("
				<< ex.what() << ").";
			TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
		}
		catch(...)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "...",
				"Unknown exception thrown while retrieving session information from the database");

			std::stringstream exMsg;
			exMsg << "An unknown error occurred while loading session information for session " << sessionId << " from the database.";
			TA_THROW(TA_Base_Core::AuthenticationAgentException( exMsg.str().c_str() ));
		}

	}
	// ly++ for CL-2078, date:20090420
	void SessionRegister::appendGhostSession( std::vector< TA_Base_Bus::SessionInfo > &sessionList ) const
	{
		FUNCTION_ENTRY( "appendGhostSession" );
		TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"[CL-20784] appendGhostSession: current session list size:%d, append list size:%d",
			sessionList.size(), m_ghostSessionMap.size() );
		for( TheRegister::const_iterator it = m_ghostSessionMap.begin(); it != m_ghostSessionMap.end(); ++it )
		{
			sessionList.push_back( it->second );
		}

		FUNCTION_EXIT;
	}

	bool SessionRegister::getGhostSessionInfo( const std::string &sessionId, TA_Base_Bus::SessionInfo &sessionInfo ) const
	{
		TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
		TheRegister::const_iterator it = m_ghostSessionMap.find( sessionId );
		if ( it != m_ghostSessionMap.end() )
		{
			sessionInfo = it->second;
			return true;
		}
		else
		{
			return false;
		}
	}

	void SessionRegister::removeGhostSession( const std::string &sessionId )
	{
		TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
		TheRegister::iterator it = m_ghostSessionMap.find( sessionId );
		if ( it != m_ghostSessionMap.end() )
		{
			m_ghostSessionMap.erase( it );
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"[CL-20784] removeGhostSession, sessionId:%s", sessionId.c_str() );
		}
	}
	
	void SessionRegister::updateGhostSessionAliveTime( unsigned long timeout, unsigned long aliveSecond /*= 60 */ )
	{
		TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
		time_t aliveTime = timeout - aliveSecond;
		if ( aliveTime < 0 )
		{
			aliveTime = 0;
		}
		TheRegister::iterator it = m_ghostSessionMap.begin();
		std::map< std::string, time_t >::iterator itAcitveTime;
		for( ; it != m_ghostSessionMap.end(); ++it )
		{
			TA_Base_Core::ThreadGuard guard( m_activeTimesAccessLock );
			if ( (itAcitveTime = m_sessionActiveTimes.find( it->first )) != m_sessionActiveTimes.end() )
			{
				m_sessionActiveTimes[ it->first ] = time(0) - aliveTime;
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"[CL-20784] update ghost session %s alive time to: %lu, timeout:%lu seconds, aliveTime:%lu",
					it->first.c_str(), m_sessionActiveTimes[ it->first ], timeout, aliveSecond);
			}
		}
	}
	// ++ly
	void SessionRegister::moveGhostSessionToRegister( const std::string &sessionId )
	{
		TA_Base_Bus::SessionInfo tmpSession;
		if (getGhostSessionInfo(sessionId, tmpSession))
		{
			removeGhostSession(sessionId);
			addSession(tmpSession, true);
		}
	}

	void SessionRegister::loadOvrrideInfomation( const std::string &sessionId,TA_Base_Bus::SessionInfo& session )
	{
		//LOG ( SourceInfo, DebugUtil::FunctionEntry, "SessionRegister::loadOvrrideInfomation" );
		//std::auto_ptr<TA_Base_Core::ISession> theSession( TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId) );

		//unsigned long tmpOperatorKey = theSession->getOverrideOperatorKey();
		//
		//if(tmpOperatorKey != ULONG_MAX )
		//{
		//	session.OperatorId.push_back( tmpOperatorKey );
		//	unsigned long tmpProfileKey =  theSession->getOverrideProfileKey();
		//	session.ProfileId.push_back( tmpProfileKey );
		//	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "loadOvrrideInfomation for session '%s', override oper: %ul, override profileId: %ul", session.SessionId.c_str(),tmpOperatorKey,tmpProfileKey);
		//}
		//

		//LOG ( SourceInfo, DebugUtil::FunctionExit, "SessionRegister::loadOvrrideInfomation" );
	}

	//++chenlei
}
