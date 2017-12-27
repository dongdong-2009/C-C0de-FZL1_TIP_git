/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/SessionQueue.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements SessionQueue.
 */


#include "SessionQueue.h"

#include "core/data_access_interface/src/ISession.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_App
{
	SessionQueue::SessionQueue( QueueStateUpdater& queueStateUpdater, bool isInControl ) :
		m_queueStateUpdater(queueStateUpdater),
		m_isInControl(isInControl)
	{
        FUNCTION_ENTRY("SessionQueue::SessionQueue() constructor");
        FUNCTION_EXIT;
	}

    SessionQueue::~SessionQueue()
    {
        clear();
    }

	void SessionQueue::setMonitor()
    {
        FUNCTION_ENTRY("setMonitor");
        m_isInControl = false;  // Set the flag to say the agent is in monitor mode.
        FUNCTION_EXIT;
    }


    void SessionQueue::setControl()
    {
        FUNCTION_ENTRY("setControl");
        m_isInControl = true;           // Set the flag to say the agent is in control mode
        FUNCTION_EXIT;
    }

	TA_Base_Core::ISession* SessionQueue::pop()
    {
        FUNCTION_ENTRY("pop");

		TA_Base_Core::ISession* rval = NULL;

		TA_Base_Core::ThreadGuard guard(m_lock);

		if( m_queue.size() == 0 )
		{
			FUNCTION_EXIT;
			return rval;
		}

		FUNCTION_EXIT;

		rval = m_queue.front();
		m_queue.pop_front();
		return( rval );
    }

    void SessionQueue::push(TA_Base_Core::ISession* session)
    {
        FUNCTION_ENTRY("push");

        // Need to ensure the session object is not null. It is a pre-condition on this method, so it 
        // can be asserted on.
        TA_ASSERT(NULL != session, "New queue item is null at push()");

        // Additionally, the working ofthe SessionAccessFactory/Session classes should mean that the session
        // id can never be empty. So assert on it.
        TA_ASSERT(0 != session->getSessionId().size(),"Session ID is empty");

        // Take a thread guard. This lock is to prevent multi-access to the deque object
        TA_Base_Core::ThreadGuard guard(m_lock);    
        
        // A session should only be on the queue once, so check to ensure the new session is not already on the queue
        for( TheQueue::iterator iter = m_queue.begin();iter != m_queue.end();iter++ )
        {
            if (0 == session->getSessionId().compare((*iter)->getSessionId()) )
            {
                // The session is on the queue, so can exit now
                return;
            }

        }

        // Push the passed item onto the back of the queue.
        m_queue.push_back( session );

		if( m_isInControl && !session->getHasEnded() )
		{
			m_queueStateUpdater.sendStateUpdateMessage(session,true);
		}
        FUNCTION_EXIT;
    }


    std::vector<TA_Base_Core::ISession*> SessionQueue::getCurrentQueue()
    {
        FUNCTION_ENTRY("getCurrentQueue");

        // Take a thread guard. This lock prevents multi-access on the dequeue.
        // In this instance, it prevents the objects being sent across the wire 
        // from being deleted until they are actually sent, as well as preventing
        // errors in accessing the objects in the queue.
        TA_Base_Core::ThreadGuard guard(m_lock);            

        // Create a new vector into which the objects currently in the queue will be placed
        std::vector<TA_Base_Core::ISession*> sessions;

        
        // Loop through the queue, adding each item to the vector.
        for( TheQueue::iterator iter = m_queue.begin();iter != m_queue.end();iter++ )
        {
            sessions.push_back(*iter);
        }

        FUNCTION_EXIT;
        return sessions;
    }


    void SessionQueue::clear()
    {
        FUNCTION_ENTRY("clear");

        // Take a thread guard. This lock prevents multi-access to the queue.
        TA_Base_Core::ThreadGuard guard(m_lock);
        
        // Iterate through the queue. Delete each object.
        for( TheQueue::iterator iter = m_queue.begin();iter != m_queue.end();iter++ )
        {
            if( *iter != NULL )
            {
                delete *iter;
				*iter = NULL;
                // Note that if the queue has been "abused" by having the same pointer
                // added twice, it will crash on this cleanup, as it will attempt to 
                // delete the same pointer twice.
            }
        }

        // Now it is safe to clear the queue.
        m_queue.clear();
        FUNCTION_EXIT;
    }


    void SessionQueue::removeQueueItem(std::string sessionId)
    {
        FUNCTION_ENTRY("removeQueueItem");

        // Take a thread guard. This lock prevents multi-access to the queue.
        TA_Base_Core::ThreadGuard guard(m_lock);

        TheQueue::iterator iter;
        TheQueue::iterator begin = m_queue.begin();
        TheQueue::iterator end = m_queue.end();

        //Iterate through the queue.
        for( iter = begin ;iter != end;/*iter++*/ )
        {
            // If the sessionId matches the one for the queue item pointed to by the
            // iterator, then a match has been found.
            if( 0 == sessionId.compare((*iter)->getSessionId()))
            {
                
                // This is it - delete the object...
                delete *iter;
				*iter = NULL;
                // ... and remove it from the queue
                iter = m_queue.erase(iter);	// must get return iterator

				//push guarantee that there is only one sessionId in the queue, and the loop can breaks here
				break;
            }
			else
			{
				++iter;
			}
        }

        FUNCTION_EXIT; 
    }


    bool SessionQueue::isEmpty()
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
        return m_queue.empty();
    }
}
