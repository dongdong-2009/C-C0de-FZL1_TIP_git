/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/SessionQueueProcessor.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class is a threaded class that performs "background" database writes,
 * leaving the agent to carry on processing registration requests. It is
 * sychronised in monitor/agent pairs, where any item added to the queue is
 * updated in the monitor agent, and any item removed likewise.
 */

#if defined(_MSC_VER)
#pragma warning(disable:4018)   // TODO: Delete this warning disable
#pragma warning(disable:4146)   // TODO: Delete this warning disable
#pragma warning(disable:4786)   // Decorated name too long
#endif // defined(_MSC_VER)

#include "SessionQueueProcessor.h"
#include "SessionQueue.h"
#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"

#include "core/data_access_interface/src/ISession.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/AuthenticationAgentStateUpdate_MessageTypes.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DbUniqueConstraintViolated.h"
#include "core/exceptions/src/DataConfigurationException.h"

using TA_Base_Core::ISession;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DbUniqueConstraintViolated;
using TA_Base_Core::DataConfigurationException;

namespace TA_Base_App
{

    const int SessionQueueProcessor::LOG_LOOPS     = 10;
    const int SessionQueueProcessor::SLEEP_TIME_MS = 2000;

    SessionQueueProcessor::SessionQueueProcessor(SessionQueue& sessionQueue, QueueStateUpdater &queueStateUpdater,
		bool isInControl) :
		m_queue(sessionQueue),
		m_isInControl(isInControl),
		m_isTerminated(false),
		m_queueStateUpdater(queueStateUpdater)
    {
        FUNCTION_ENTRY("SessionQueueProcessor::SessionQueueProcessor() constructor");
        FUNCTION_EXIT;
    }

	void SessionQueueProcessor::setMonitor()
    {
        FUNCTION_ENTRY("setMonitor");
        m_isInControl = false;  // Set the flag to say the agent is in monitor mode.
        FUNCTION_EXIT;
    }


    void SessionQueueProcessor::setControl()
    {
        FUNCTION_ENTRY("setControl");
        m_isInControl = true;           // Set the flag to say the agent is in control mode
        FUNCTION_EXIT;
    }

    void SessionQueueProcessor::run()
    {
        FUNCTION_ENTRY("run");


        try
        {
            int i = 0;
            while( !m_isTerminated )
            {
                // Only want to do something if the queue is not empty, AND we're in control mode
                if( m_isInControl && !m_queue.isEmpty() )
                {                   
                    // process the queue item - it is deleted (or not) in here.
					// Note: because we are popping here and pushing if something goes
					// wrong, a new item could be pushed onto the queue before our
					// push. This will result in older items being processed AFTER
					// newer items.
                    handleSessionUpdate(m_queue.pop(), true);
                }
                Thread::sleep(SLEEP_TIME_MS);
                if (LOG_LOOPS-1 == i++) 
                {
                    i = 0;
                    LOG_GENERIC (SourceInfo, DebugUtil::DebugDebug,
                        "Queue at 0 size. Waiting for events.");
                }
            }
        }
        catch(...)
        {
            // Catch to ensure nothing leaks from run().
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[TD15901] An exception was prevented from leaking out of the run method"); //TD15901
        }
        FUNCTION_EXIT;
    }


    void SessionQueueProcessor::terminate()
    {
        FUNCTION_ENTRY("terminate");
        // set the flag to make the run() method finish
        m_isTerminated = true;
        FUNCTION_EXIT;
    }


    void SessionQueueProcessor::writeNewSession(ISession* session)
    {
        handleSessionUpdate(session, false);
    }


    void SessionQueueProcessor::handleSessionUpdate(ISession* session, bool isQueueItem)
    {
        FUNCTION_ENTRY("handleSessionUpdate");

        TA_ASSERT(session != NULL,"The session was null");

        // Some of the methods used here can throw database exceptions, hence the try/catch block.
        try
        {
            // Attempt to write the session to the database
            session->applyChanges();

			LOG_GENERIC (SourceInfo, DebugUtil::DebugInfo,
				"[TD15901] Session (%s) has been written.", session->getSessionId().c_str() ); //TD15901

            // Now the event is in the database, the monitor agent can safely remove it
            // from the queue. So send a state update event to remove it from the monitor agent if this
            // session came from the queue.
			if( isQueueItem && !session->getHasEnded() )
			{
				m_queueStateUpdater.sendStateUpdateMessage(session, false);
			}

            // The has been taken care of, so it should be deleted
            delete session;
			session = NULL;
        }
		catch(const DbUniqueConstraintViolated& ex)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DbUniqueConstraintViolated", ex.what() );

			LOG_GENERIC (SourceInfo, DebugUtil::DebugInfo,
				"[TD15901] Assuming session (%s) has been written by previous Control mode agent, deleting from queue...", session->getSessionId().c_str() );	//TD14337 yezh++ //TD15901
			delete session;
			session = NULL;
		}
        catch(const DatabaseException& ex)
        {
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", ex.what() );

            // There is a database error. Don't delete.
            // Push the passed item onto the back of the queue.
            m_queue.push( session );

			LOG_GENERIC (SourceInfo, DebugUtil::DebugInfo,
				"[TD15901] Push back session (%s) in queue.", session->getSessionId().c_str() ); //TD15901
        }
        catch(const DataException&)
        {
            LOG_EXCEPTION_CATCH (SourceInfo, "DataException", 
                "[TD15901] DataException thrown while trying to insert item into database."); //TD15901

			// The data in the this session object is corrupt, and cannot be written to the
            // database. Remove it.
            delete session;
			session = NULL;
        }
        catch(const DataConfigurationException&)
        {
            LOG_EXCEPTION_CATCH (SourceInfo, "DataConfigurationException", 
                "[TD15901] DataConfigurationException thrown while trying to insert item into database."); //TD15901

            // The data in this session object is not able to be written to the database.
            delete session;
			session = NULL;
        }
        catch(...)
        {
			// Need to catch... for now (in case the data stuff throws other exceptions)
            LOG_EXCEPTION_CATCH (SourceInfo, "unknown", 
                "[TD15901] Unknown exception thrown while trying to insert item into database."); //TD15901
            
            delete session;
			session = NULL;

        }
        FUNCTION_EXIT;
    }
}
