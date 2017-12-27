/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/SessionQueue.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Maintains a thread-safe queue of TA_Base_Core::ISession*.
 */


#if !defined(SESSIONQUEUE_H_INCLUDED)
#define SESSIONQUEUE_H_INCLUDED

#include "QueueStateUpdater.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <deque>
#include <vector>

namespace TA_Base_Core
{
	class ISession;
}

namespace TA_Base_App
{

    class SessionQueue
    {

    public:  
        

		SessionQueue(QueueStateUpdater &queueStateUpdater, bool isInControl);

        /**
         * Destructor
         *
         * Destructs the monitored queue object
         */
		~SessionQueue();


        /**
         * push
         *
         * Adds a new item to the queue.
         *
         * @param session The new session to add to the queue. It is a pointer, and the SessionQueue
         * will take responsibility for the destruction of the pointer once passed in.
         *
         * pre: session is NOT NULL.
         */
        void push(TA_Base_Core::ISession* session);

        /**
         * pop
         *
         * Pops the item at the top of the queue.
         *
         * @param session The new session to add to the queue. It is a pointer, and the SessionQueue
		 *
         */
        TA_Base_Core::ISession* pop();

        /**
         * getCurrentQueue
         *
         * Retrieves the current queue. This is for state update purposes.
         *
         * @return The current state of the session database queue as a vector.
         */
        std::vector<TA_Base_Core::ISession*> getCurrentQueue();

        /**
         * clear
         *
         * Removes all items from the queue.
         */
	    void clear();

        /**
         * removeQueueItem
         *
         * Removes the session with the speicifed sessionId from the queue. This is used in state updates.
         *
         * @param sessionId The sessionId of the session to remove from the database queue.
         */
	    void removeQueueItem(std::string sessionId);

        /**
         * isEmpty
         *
         * Determines if the queue is or is not empty.
         *
         * @return true if the queue is empty
         */
	    bool isEmpty();

		/**
         * setMonitor
         *
         * Puts the queue into monitor mode - there will be no database writes.
         */
	    void setMonitor();


        /**
         * setControl
         *
         * Puts the queue into control mode - it will write to the database.
         */
	    void setControl();

    private:
        /**
         * Copy constructor
         *
         * Hidden to prevent accidental use
         */
	    SessionQueue( const SessionQueue& theSessionQueue);

        /**
         * operator=
         *
         * Hidden to prevent accidental use
         */
        SessionQueue& operator=(const SessionQueue& rhs);

        /** 
         * SessionQueue
         *
         * Define the deque of ISession objects as a SessionQueue
         */
        typedef std::deque<TA_Base_Core::ISession*> TheQueue;

        /**
         * m_queue
         *
         * The actualy queue object.
         */
	    TheQueue m_queue;

        /**
         * m_lock
         *
         * The thread lock. 
         */
        TA_Base_Core::NonReEntrantThreadLockable m_lock;


		QueueStateUpdater& m_queueStateUpdater;
		bool m_isInControl;
    };
}

#endif // !defined(SESSIONQUEUE_H_INCLUDED)
