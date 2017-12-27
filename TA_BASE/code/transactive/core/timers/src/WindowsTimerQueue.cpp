/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/WindowsTimerQueue.cpp $
  * @author:	Adam Radics
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  * A more reliable windows implementation of an ACE timer queue
  */

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"
#include <mmsystem.h>
#ifdef WIN32
    #pragma comment( lib, "Winmm" )
#endif
#include "core/timers/src/WindowsTimerQueue.h"

namespace
{
    //this timer need not to achieve 1 ms resolution, 
    //adjust timer resolution to lower 10 ms
    const UINT resolution = 10;
}

namespace TA_Base_Core
{
	//
	// Constructor
	//
    WindowsTimerQueue::WindowsTimerQueue()
    : m_timerId (0),
	  m_timerRes (0),
      m_timerMaxPeriod (0),
      m_nextTimeout (ACE_Time_Value::zero),
	  m_resolutionValue (ACE_Time_Value::zero),
	  m_threadId (0),
	  m_processTimersSemaphore (0),
	  m_keepRunning (true)
    {
		FUNCTION_ENTRY("WindowsTimerQueue()");

		// Populate the timer IDs
		for (long i = 1; i <= MAX_NUM_TIMERS; i++)
		{
			m_timerIds.push(i);
		}

		FUNCTION_EXIT;
    }

	//
	// Destructor
	//
    WindowsTimerQueue::~WindowsTimerQueue()
    {
		FUNCTION_ENTRY("~WindowsTimerQueue()");

		while (m_timerIds.size() > 0)
		{
			m_timerIds.pop();
		}

		FUNCTION_EXIT;
    }

	//
	// activate
	//
	void WindowsTimerQueue::activate()
	{
		FUNCTION_ENTRY("activate()");

		// start the timer, 10ms resolution
        TIMECAPS	tc;

        // fix release version problem
        // TA_ASSERT(TIMERR_NOERROR == timeGetDevCaps(&tc,sizeof(TIMECAPS)),
		//		  "Failed to set up internal timer");
        MMRESULT result = timeGetDevCaps(&tc,sizeof(TIMECAPS));
        TA_ASSERT(TIMERR_NOERROR == result, "Failed to set up internal timer");

		m_timerRes = min(max(tc.wPeriodMin,resolution),tc.wPeriodMax);
        m_timerMaxPeriod = max(resolution, tc.wPeriodMax);

		timeBeginPeriod(m_timerRes);

		m_resolutionValue.msec(static_cast<long>(m_timerRes));

		start();

		FUNCTION_EXIT;
	}

	//
	// deactivate
	//
	void WindowsTimerQueue::deactivate()
	{
		FUNCTION_ENTRY("deactivate()");

		TA_ASSERT(m_threadId != getCurrentThreadId(), "Using the callback function to call this function could have undesirable results");

		terminateAndWait();

		 // stop the timer
        if (0 != m_timerRes)
		{
			timeEndPeriod(m_timerRes);
			m_timerRes = 0;
			m_resolutionValue = ACE_Time_Value::zero;
			m_nextTimeout = ACE_Time_Value::zero;
		}

		FUNCTION_EXIT;
	}

	//
	// schedule
	//
    long WindowsTimerQueue::schedule ( ACE_Event_Handler *handler,
                                       const void *act,
                                       const ACE_Time_Value &future_time,
                                       const ACE_Time_Value &interval )
    {  
		FUNCTION_ENTRY("schedule()");

		TA_ASSERT(m_threadId != getCurrentThreadId(), "Using the callback function to call this function could have undesirable results");
		
		long timerId = -1;

        // smallest lock possible to prevent blocking too long
        {
            TA_THREADGUARD(m_timerlistLock);

			if (m_timerIds.empty())
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
				     "Failed to schedule timer - no timer Ids available");

				FUNCTION_EXIT;
				return -1;
			}
			
			// Make sure the timer interval isn't less than the resolution
			// but only if it isn't zero (non periodic).
			ACE_Time_Value checkedInterval = interval;
			if (ACE_Time_Value::zero != checkedInterval && 
				checkedInterval < m_resolutionValue)
			{
				checkedInterval = m_resolutionValue;
			}

			timerId = m_timerIds.front();

			TimerEntry newTimer( timerId,
								 future_time,
								 checkedInterval,
								 act,
								 handler);

			m_timerIds.pop();

            // add it to the list
            m_timerList.insert(newTimer);

			// If this timer is sorted on the beginning of the list
			// need to reschedule
			if (timerId == m_timerList.begin()->timerId)
			{
				processTimers();
			}
        }

		FUNCTION_EXIT;
		return timerId;
    }

	//
	// reset_interval
	//
	int WindowsTimerQueue::reset_interval (long timer_id, const ACE_Time_Value &interval)
    {
		FUNCTION_ENTRY("reset_interval()");

		TA_ASSERT(m_threadId != getCurrentThreadId(), "Using the callback function to call this function could have undesirable results");

		bool found = false;
		
        {
			TA_THREADGUARD(m_timerlistLock);

			std::set<TimerEntry>::iterator timerIter = m_timerList.begin();

			// find the item in the list
			while (timerIter != m_timerList.end())
			{
				if (timerIter->timerId == timer_id)
				{
					TimerEntry entry = (*timerIter);
					// remove it
					timerIter = m_timerList.erase(timerIter);

					entry.interval = interval;
					entry.nextTimeout =  ACE_OS::gettimeofday() + interval;

					// add it to the list
					m_timerList.insert(entry);

					found = true;
					break;
				}
				
				timerIter++;
			}
		}

		if (!found)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
				 "Failed to reschedule timer %d", timer_id);
		}
		else
		{
			processTimers();
		}

		FUNCTION_EXIT;
		return found?0:-1;
    }

	//
	// cancel
	//
	int WindowsTimerQueue::cancel ( long timer_id,
                                    const void **act )
    {
		FUNCTION_ENTRY("cancel()");

		TA_ASSERT(m_threadId != getCurrentThreadId(), "Using the callback function to call this function could have undesirable results");

        TA_THREADGUARD(m_timerlistLock);

		bool found = false;
		std::set<TimerEntry>::iterator timerIter = m_timerList.begin();

        // find the item in the list
        while (timerIter != m_timerList.end())
        {
            if (timerIter->timerId == timer_id)
            {
				// Return the act pointed
				(*act) = timerIter->act;
                // remove the timer
                timerIter = m_timerList.erase(timerIter);
				// Add the timer Id to the queue
				m_timerIds.push(timer_id);
				found = true;
				break;
            }
			
			timerIter++;
        }

		if (!found)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
				 "Failed to cancel timer %d", timer_id);
		}

        // dont bother messing with timers,
        // if this was the next to go off there will just be no
        // expire timers on the next time out, and the timer will be rescheduled

		FUNCTION_EXIT;
		return found?1:0;
    }

	//
	// thr_count
	//
	int WindowsTimerQueue::thr_count()
	{
		FUNCTION_ENTRY("thr_count()");

		FUNCTION_EXIT;
		return (m_timerRes != 0)?1:0;
	}

	//
	// timerCallback
	//
	void WindowsTimerQueue::timerCallback(unsigned int id)
	{
		{
			TA_THREADGUARD(m_timerlistLock);
		
			if (m_timerId == id)
			{
				m_timerId = 0;
			}
			else
			{
				return;
			}
		}

		processTimers();
	}

	/*
	** Protected functions
	*/

	//
	// processTimers
	//
    void WindowsTimerQueue::processTimers()
    {
		FUNCTION_ENTRY("processTimers");

		m_processTimersSemaphore.post();

		FUNCTION_EXIT;
    }

	//
	// stopInternalTimer
	//
	void WindowsTimerQueue::stopInternalTimer()
	{
		FUNCTION_ENTRY("stopInternalTimer");

		TA_THREADGUARD(m_timerlistLock);

		// cancel the old timer
		if (m_timerId != 0)
		{
			MMRESULT result = timeKillEvent(m_timerId);
			if (TIMERR_NOERROR != result)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
					 "Failed to cancel internal timer id %d", m_timerId);
			}

			m_timerId = 0;
		}

		FUNCTION_EXIT;
	}

	//
	// expireTimers
	//
	void WindowsTimerQueue::expireTimers()
	{
		FUNCTION_ENTRY("expireTimers");

		// check for all expired timers and notify observers
        ACE_Time_Value currentTime = ACE_OS::gettimeofday();

		bool complete = false;

		while (!complete)
		{
            TA_THREADGUARD(m_timerlistLock);

            // Has the first timer on the list expired?
			std::set<TimerEntry>::iterator it = m_timerList.begin();
			if (it != m_timerList.end() && it->nextTimeout <= currentTime )
            {
                TimerEntry entry = (*it);

                // remove it
				m_timerList.erase(it);

				// Process the timeout
                try
                {
    				entry.handler->handle_timeout( entry.nextTimeout, entry.act );
                }
                catch(...)
                {
                    // suspect that when a windows timer is created and deleted fast
                    // the timer isnt actually deleted, and it expires and the user defined data
                    // points to deleted memory

                    LOG_EXCEPTION_CATCH(SourceInfo, "...", "Leaked out of handle_timeout");
                }

                // if its a periodic timer, reschedule it
                if (entry.interval != ACE_Time_Value::zero)
                {
					// If this is still before the current time,
					// skip the interval and go on to the next
					int count = -1;
					do
					{
						entry.nextTimeout += entry.interval;
						count++;
					}
					while (entry.nextTimeout < currentTime + m_resolutionValue);

					if (count > 0)
					{
						LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
							"Skipped %d intervals of timer %d interval %d", count, entry.timerId, entry.interval.msec());
					}

					m_timerList.insert(entry);
				}
				else
				{
					// Finished with this timer ID - recycle it
					m_timerIds.push(entry.timerId);
				}
            }
			else
			{
				// No more timers to expire
				complete = true;
			}
        }

		FUNCTION_EXIT;
	}


    //
	//  internalTimerProc
	//	C-style function used as a callback by Windows
	//
    extern "C" void CALLBACK internalTimerProc(UINT id,UINT msg,DWORD dwUser,DWORD dw1,DWORD dw2)
	{	
		WindowsTimerQueue* timerQueue = (WindowsTimerQueue*)dwUser;
		timerQueue->timerCallback(id);
	}

	//
	// restartInternalTimer
	//
	void WindowsTimerQueue::restartInternalTimer()
	{
		FUNCTION_ENTRY("restartInternalTimer");

        {
		    TA_THREADGUARD(m_timerlistLock);

		    if (m_timerList.size() > 0)
		    {
			    // re-evaluate the next timeout
			    if (m_timerList.begin()->nextTimeout != m_nextTimeout || 
				    m_timerId == 0)
			    {
				    m_nextTimeout = m_timerList.begin()->nextTimeout;
			    }

		    }
		    else
		    {
			    m_nextTimeout = ACE_Time_Value::zero;

			    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				     "No timers to reschedule");
		    }
        }

		if (m_nextTimeout != ACE_Time_Value::zero)
		{
			// schedule the new timer
            //
			// if the new timer is in the past or shorter than the resolution,
			// schedule to timeout in m_timerRes after the current time
			long nextTimeout = m_timerRes;

			ACE_Time_Value currentTime = ACE_OS::gettimeofday();

			nextTimeout = max(nextTimeout, (long)(m_nextTimeout - currentTime).msec());
            nextTimeout = min((UINT)nextTimeout, m_timerMaxPeriod);

			MMRESULT	result = NULL;

            {
                TA_THREADGUARD( m_timerlistLock ); // CL-21916
                
                result = timeSetEvent(nextTimeout, m_timerRes, internalTimerProc,
                    (DWORD) this, TIME_ONESHOT);
                
                if ( NULL != result )
                {
                    m_timerId = (long)result;
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace, 
                        "Set internal timer for %d ms", nextTimeout);
                }
            }

            if ( NULL == result )
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
					 "Failed to set internal timer, use sleep instead.");

                TA_Base_Core::Thread::sleep(m_timerRes);

                //signal the semaphone to trigger the thread to go on to retry
                m_processTimersSemaphore.post();
			}
		}

		FUNCTION_EXIT;
	}

	//
	// run
	//
	void WindowsTimerQueue::run()
	{
		FUNCTION_ENTRY("run()");

		m_threadId = getCurrentThreadId();

		while (m_keepRunning)
		{
			m_processTimersSemaphore.wait();

            try
            {
                stopInternalTimer();
            }
            catch (...) 
            {
                //don't know why these functions are possible to cause exceptions
                //just log and retry...
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                    "stopInternalTimer: Unknown exception caught in run function.");
                
                //don't know how to clear the error, just sleep a while, hope something goes right
                //TA_Base_Core::Thread::sleep(m_timerRes);
                
                //signal the semaphone to trigger the thread to go on to retry
                //m_processTimersSemaphore.post();
            }
                
            try
            {
                expireTimers();
            }
            catch (...) 
            {
                //don't know why these functions are possible to cause exceptions
                //just log and retry...
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                    "expireTimers: Unknown exception caught in run function.");
                
                //don't know how to clear the error, just sleep a while, hope something goes right
                //TA_Base_Core::Thread::sleep(m_timerRes);
                
                //signal the semaphone to trigger the thread to go on to retry
                //m_processTimersSemaphore.post();
            }
                
            try
            {
                restartInternalTimer();
            }
            catch (...) 
            {
                //don't know why these functions are possible to cause exceptions
                //just log and retry...
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                    "restartInternalTimer: Unknown exception caught in run function.");
                
                //don't know how to clear the error, just sleep a while, hope something goes right
                TA_Base_Core::Thread::sleep(m_timerRes);
                
                //signal the semaphone to trigger the thread to go on to retry
                m_processTimersSemaphore.post();
            }
		}
		
		m_threadId = 0;
		m_keepRunning = true;

		FUNCTION_EXIT;
	}

	//
	// terminate
	//
	void WindowsTimerQueue::terminate()
	{
		FUNCTION_ENTRY("terminate()");

		m_keepRunning = false;
		m_processTimersSemaphore.post();

		FUNCTION_EXIT;
	}
}

