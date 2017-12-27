/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/CommandScheduler.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class uses a priority queue in order to execute commands. It also
  * supports delayed execution for commands that need to execute at some
  * point in the future.
  *
  */

#if !defined(_COMMAND_SCHEDULER_H_)
#define _COMMAND_SCHEDULER_H_

#include <queue>

#include "core/event_processing/src/AbstractCommand.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Core
{
    class CommandScheduler: public Thread
    {
    public:
        /**
         * Constructor
         */
        CommandScheduler();


        /**
         * Destructor
         */
        virtual ~CommandScheduler();


        /** 
          * push
          *
          * Pushes the command object into the queue. Its location
          * will depend on its priority.
          *
          * @param command The command to push.
          */
        virtual void push( AbstractCommand* command );


        /** 
          * run
          *
          * This is the main thread of the queue and handles all dequeuing and
          * command execution.
          */
        virtual void run();


        /** 
          * terminate
          *
          * Called when the thread should terminate.
          */
        virtual void terminate();

    private:
        /**
         * expire
         *
         * Expires any commands that should now be executed. If no commands are scheduled to
         * be executed this will essentially be a no-op.
         */
        void expire();

        /**
         * wait
         *
         * Waits until one of the following two conditions are met:
         * 1. A new command is added to the scheduler.
         * 2. The timer representing the top command has expired.
         */
        void wait();

        /**
         * flush
         *
         * Flushes all remaining commands out of the event queue.
         */
        void flush();


        // The flag to specify when the thread should terminate.
        bool m_terminate;

        // The condition used to freeze thread execution until command arrive.
        Condition m_condition;

        // The lock used to guard against priority queue corruption.
        WriteReadThreadLockable m_priorityQueueLock;

        // The priority queue that stores pending commands.
        std::priority_queue<AbstractCommand*> m_priorityQueue;
    };
};

#endif // !defined(_COMMAND_SCHEDULER_H_)
