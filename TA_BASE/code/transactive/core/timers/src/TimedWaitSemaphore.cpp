/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/TimedWaitSemaphore.cpp $
  * @author:  Darren Sampson
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 11:21:23 $
  * Last modified by:  $Author: Ouyang $
  * 
  * This class provides a timed wait semaphore implementation with a list of Conditions
  * 
  * NOTE: Only works with pthreads
  */
#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/TimedWaitSemaphore.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"


namespace
{
    struct LockGuard
    {
        LockGuard( omni_mutex& mutex ) : m_mutex( mutex ) { m_mutex.lock(); }
        ~LockGuard() { m_mutex.unlock(); }
        omni_mutex& m_mutex;
    };
}

#define LOCK_GUARD(mutex) LockGuard __guard__(mutex)


namespace TA_Base_Core
{
    TimedWaitSemaphore::TimedWaitSemaphore( unsigned int initial )
        : m_count( initial ),
          m_destroyed( false )
    {}

    TimedWaitSemaphore::~TimedWaitSemaphore()
    {
        FUNCTION_ENTRY("~TimedWaitSemaphore");

        m_destroyed = true;

        while ( false == m_conditions.empty() )
        {
            {
                LOCK_GUARD( m_mutex );

                for ( std::list<omni_condition*>::iterator it = m_conditions.begin(); it != m_conditions.end(); ++it )
                {
                    (*it)->signal();
                }

                m_conditions.clear();
            }

            Thread::sleep(20);
        }

        FUNCTION_EXIT;
    }

    void TimedWaitSemaphore::wait(void)
    {
        FUNCTION_ENTRY("wait");

        if ( m_destroyed )
        {
            FUNCTION_EXIT;
            return;
        }

        LOCK_GUARD( m_mutex );

        if ( 0 < m_count )
        {
            --m_count;
            FUNCTION_EXIT;
            return;
        }

        omni_condition condition( &m_mutex );
        m_conditions.push_back( &condition );
        condition.wait();
        //m_conditions.remove( &condition );

        if ( 0 < m_count )
        {
            --m_count;
        }

        FUNCTION_EXIT;
    }

    bool TimedWaitSemaphore::tryWait(void)
    {
        FUNCTION_ENTRY("tryWait");

        if ( m_destroyed )
        {
            FUNCTION_EXIT;
            return false;
        }

        LOCK_GUARD( m_mutex );

        if ( 0 < m_count )
        {
            --m_count;
            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;
        return false;
    }

    void TimedWaitSemaphore::post(void)
    {
        FUNCTION_ENTRY("post");

        if ( m_destroyed )
        {
            FUNCTION_EXIT;
            return;
        }

        LOCK_GUARD( m_mutex );

        ++m_count;

        if ( false == m_conditions.empty() )
        {
            m_conditions.front()->signal();
            m_conditions.pop_front(); // ensure post once
        }

        FUNCTION_EXIT;
    }

    bool TimedWaitSemaphore::timedWait(unsigned long timeoutMsec)
    {
        FUNCTION_ENTRY("timedWait");

        if ( m_destroyed )
        {
            FUNCTION_EXIT;
            return false;
        }

        LOCK_GUARD( m_mutex );

        if ( 0 < m_count )
        {
            --m_count;
            FUNCTION_EXIT;
            return true;
        }

        unsigned long secs = 0, nanosecs = 0;
        omni_thread::get_time( &secs, &nanosecs, timeoutMsec / 1000, (timeoutMsec % 1000) * 1000000 );

        omni_condition condition( &m_mutex );
        m_conditions.push_back( &condition );
        int result = condition.timedwait( secs, nanosecs );
        m_conditions.remove( &condition );

        if ( 1 == result && 0 < m_count ) // 0: time expired
        {
            --m_count;
        }

        FUNCTION_EXIT;
        return result;
    }
}
