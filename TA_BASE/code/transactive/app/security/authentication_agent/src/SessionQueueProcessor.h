/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/SessionQueueProcessor.h $
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


#if !defined(SessionQueueProcessor_A42B4837_7B8B_4300_B832_99A2A3FFA774__INCLUDED_)
#define SessionQueueProcessor_A42B4837_7B8B_4300_B832_99A2A3FFA774__INCLUDED_

#include "core/threads/src/Thread.h"
#include "SessionQueue.h"
#include "QueueStateUpdater.h"

namespace TA_Base_App
{

    class SessionQueueProcessor : public TA_Base_Core::Thread
    {

    public:  
        
        /** 
         * Constructor
         *
         * Consturcts the monitored queue, with an initial mode of control or monitor
         *
         * @param isInControl true if the queue is to be in control mode when constructed.
         */
	    SessionQueueProcessor(SessionQueue& sessionQueue, QueueStateUpdater &queueStateUpdater, bool isInControl);

        /**
         * run
         *
         * Starts the thread running.
         */
	    void run();

        /**
         * terminate
         *
         * Stops the thread running.
         */
	    void terminate();

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

        /** 
          * writeNewSession
          *
          * Attempts to write a new session to the database. If it fails due to the database being
          * unavailable, the session is added to the queue so that it can be written at a later date.
          *
          * @param session  The new session to write to the database.
          */
        void writeNewSession(TA_Base_Core::ISession* session);

    private:
        /**
         * Copy constructor
         *
         * Hidden to prevent accidental use
         */
	    SessionQueueProcessor( const SessionQueueProcessor& theSessionQueueProcessor);

        /**
         * operator=
         *
         * Hidden to prevent accidental use
         */
        SessionQueueProcessor& operator=(const SessionQueueProcessor& rhs);

       /**
         * handleSessionUpdate
         *
         * Deals with a new or updated session. This method is responsible for handling any 
         * database exceptions that may occur from attempting a database write.
         *
         * @param session      The new session to write to the database.
         * @param isQueueItem  True if the item came from the session queue.
         */
        void handleSessionUpdate(TA_Base_Core::ISession* session, bool isQueueItem);

        /**
         * m_queue
         *
         * The actualy queue object.
         */
	    SessionQueue &m_queue;

        /**
         * m_isInControl
         *
         * Boolean flag that holds the control state of the class. True if the class is in control.
         */
        bool m_isInControl;

        /**
         * m_isTerminated
         *
         * Boolean flag that is set to false at construction and changed to true when the terminate 
         * method is called
         */
        bool m_isTerminated;

        /**
         * LOG_LOOPS
         *
         * The total number of loops the queue will sleep for between logging a "queue at 0" message
         */
        static const int LOG_LOOPS;

        /**
         * SLEEP_TIME_MS
         *
         * The length of each sleep cycle
         */
        static const int SLEEP_TIME_MS;

		QueueStateUpdater& m_queueStateUpdater;

    };
}

#endif // !defined(SessionQueueProcessor_A42B4837_7B8B_4300_B832_99A2A3FFA774__INCLUDED_)
