/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/CommandScheduler.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class uses a priority queue in order to execute event processing
  * commands.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <list>
#include <memory>

#include "ace/ACE.h"
#include "ace/OS_NS_sys_time.h"

#include "core/event_processing/src/CommandScheduler.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace
{
    // TD12003: ACE_Time_Value uses seconds and microseconds internally, so the millisecond value
    //          obtained is up to 999 microseconds short of the required sleep time. Theoretically
    //          a value of 1 msec should be sufficient, but give it a few to make sure. 
    unsigned long ADDITIONAL_SLEEP_TIME_MSEC = 10;
}

namespace TA_Base_Core
{
    CommandScheduler::CommandScheduler() : m_terminate(false)
    {
        FUNCTION_ENTRY( "Constructor" );

        // Start the thread.
        start();

        FUNCTION_EXIT;
    }


    CommandScheduler::~CommandScheduler()
    {
        FUNCTION_ENTRY( "Destructor" );

        // Make sure the thread is terminated.
        terminateAndWait();

        FUNCTION_EXIT;
    }


    void CommandScheduler::push( AbstractCommand* command )
    {
        FUNCTION_ENTRY( "push" );

        TA_ASSERT( NULL!=command, "NULL command pushed to scheduler." );

        // Get a write lock.
        ThreadGuard guard( m_priorityQueueLock );

        // Add the command to the queue.
        m_priorityQueue.push( command );

        // Signal the processing thread.
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Signalling scheduler thread." );
        m_condition.signal();

        FUNCTION_EXIT;
    }


    void CommandScheduler::run()
    {
        FUNCTION_ENTRY( "run" );

        // Set the priority to low.
        setPriority( 0 );

        // Don't reset terminate flag here as it can cause a race condition when a CommandScheduler
        // is created and destroyed very quickly. As a result a precondition for this run method is
        // that terminate is not set!

        while ( !m_terminate )
        {
            // Now wait until it is time to process a new command.
            wait();

            // Expire any commands that are now ready to be executed.
            expire();
        }

        // Thread has been terminated.
        m_terminate = false;

        // Flush the queue.
        flush();

        FUNCTION_EXIT;
    }


    void CommandScheduler::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        // The thread is now terminating.
        m_terminate = true;

        // Signal the processing thread.
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Signalling scheduler thread." );
        m_condition.signal();

        FUNCTION_EXIT;
    }


    void CommandScheduler::expire()
    {
        FUNCTION_ENTRY( "expire" );

        // This list stores all of the commands that are ready to execute.
        std::list<AbstractCommand*> cmdsToExecute;
        {
            // Get a write lock while we access the queue.
            ThreadGuard guard( m_priorityQueueLock );

            // Extract all commands from the queue that are scheduled to be executed.
            ACE_Time_Value currentTime = ACE_OS::gettimeofday();
            while ( !m_priorityQueue.empty() && ( m_priorityQueue.top()->getScheduledTime() <= currentTime ) )
            {
                cmdsToExecute.push_back( m_priorityQueue.top() );
                m_priorityQueue.pop();
            }
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "%d commands are ready to execute.", cmdsToExecute.size() );

        // We can now process all commands without locking.
        std::list<AbstractCommand*>::iterator cmdIt;
        for ( cmdIt=cmdsToExecute.begin(); cmdIt!=cmdsToExecute.end(); cmdIt++ )
        {
            try
            {
                (*cmdIt)->execute( *this );
            }
            catch ( ... )
            {
                // Make sure stray exceptions don't destroy the thread.
                LOG_EXCEPTION_CATCH( SourceInfo, "unknown", "Unknown exception received in event processing thread." );
            }

            // The command is owned by the scheduler, so we must delete it now it is no longer needed.
            delete *cmdIt;
        }

        FUNCTION_EXIT;
    }


    void CommandScheduler::wait()
    {
        FUNCTION_ENTRY( "wait" );

        // If we're terminating there is no need to wait.
        if ( m_terminate )
        {
            return;
        }

        // Get the time at which the next command needs to be executed.
        ACE_Time_Value scheduledTime;
        scheduledTime.msec( 0 );
        // TD12003
        bool isQueueEmpty = true;
        {
            // Get a read lock while we access the queue.
            ThreadReadGuard guard( m_priorityQueueLock );
            isQueueEmpty = m_priorityQueue.empty();
            if (isQueueEmpty == true)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Nothing to schedule, waiting indefinitely." );
            }
        }

        if (isQueueEmpty == true)
        {
            // since queue is empty, wait indefinitely until signaled.
            m_condition.wait();
        }
        
        {
            // Get a read lock while we access the queue.
            ThreadReadGuard guard( m_priorityQueueLock );

            if ( !m_priorityQueue.empty() )
            {
                scheduledTime = m_priorityQueue.top()->getScheduledTime();
            }
        }

        // Now wait.
        ACE_Time_Value currentTime = ACE_OS::gettimeofday();
        // TD12003
        ACE_Time_Value zero;
        zero.msec(0);
        if (zero < scheduledTime)
        {
            // If the time is non-zero we need a timed wait.
            if ( scheduledTime > currentTime )
            {
                // Only wait if the scheduled time hasn't already passed.
                unsigned long waitTimeMs = (scheduledTime-currentTime).msec() + ADDITIONAL_SLEEP_TIME_MSEC;

                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Scheduled time (%d) in the future, waiting %d ms.", scheduledTime.msec(), waitTimeMs );

                m_condition.timedWait( waitTimeMs );
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Scheduled time (%d) has already passed, not waiting.", scheduledTime.msec() );
            }
        }
        else
        {
            // TD12003
            // should not get here
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Nothing to schedule. Terminating thread." );

            // TODO LPP: In what sense are we terminating the thread?  Or does the comment mean that 
            // somehow the thread is in the process of terminating?
            // If we really shouldn't get here, why don't we assert?
        }

        FUNCTION_EXIT;
    }

    void CommandScheduler::flush()
    {
        FUNCTION_ENTRY( "flush" );

        // Get a lock while we flush the queue.
        ThreadGuard guard( m_priorityQueueLock );

        // Delete all remaining commands and remove them from the queue.
        while ( !m_priorityQueue.empty() )
        {
            delete m_priorityQueue.top();
            m_priorityQueue.pop();
        }

        FUNCTION_EXIT;
    }
};
