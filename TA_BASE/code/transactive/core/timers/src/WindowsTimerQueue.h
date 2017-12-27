/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/WindowsTimerQueue.h $
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

#ifndef WINDOWS_TIMER_QUEUE_H
#define WINDOWS_TIMER_QUEUE_H

#include <set>
#include <queue>

#include "ace/ACE.h"
#include "ace/Thread_Manager.h"
#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue.h"
#include "ace/Timer_Queue_Adapters.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

#define MAX_NUM_TIMERS		1000

using namespace std;

namespace TA_Base_Core
{
    class WindowsTimerQueue : public Thread
    {
        public:

	    // Construction / destruction:
	    WindowsTimerQueue();
	    virtual ~WindowsTimerQueue();

		/**
		 *
		 * activate
		 * 
		 * Sets up the internal timer and starts the internal thread
		 *
		 */
		void activate();

		/**
		 *
		 * deactivate
		 * 
		 * Cleans up the internal timer and stops the internal thread
		 *
		 */
	    void deactivate();
		
		/**
		 *
		 * schedule
		 *
		 * Schedule a timer that will expire at <future_time>,
		 * which is specified in absolute time.  If it expires then <act> is
		 * passed in as the value to the <functor>.  If <interval> is != to
		 * <ACE_Time_Value::zero> then it is used to reschedule the <type>
		 * automatically, using relative time to the current <gettimeofday>.
		 * This method returns a <timer_id> that uniquely identifies the the
		 * timer entry in an internal list.  This <timer_id> can be used to
		 * cancel the timer before it expires. Returns -1 on failure (which 
		 * is guaranteed never to be a valid <timer_id>).
		 *
		 */
	    long schedule ( ACE_Event_Handler *handler,
                        const void *act,
                        const ACE_Time_Value &future_time,
                        const ACE_Time_Value &interval = ACE_Time_Value::zero );

		/**
	     * Resets the interval of the timer represented by <timer_id> to
	     * <interval>, which is specified in relative time to the current
	     * <gettimeofday>.  If <interval> is equal to
	     * <ACE_Time_Value::zero>, the timer will become a non-rescheduling
	     * timer.  Returns 0 if successful, -1 if not.
	     */
		int reset_interval (long timer_id, const ACE_Time_Value &interval);

		/**
		 *
		 * cancel
		 *
	     * Cancel the single timer that matches the <timer_id> value (which
	     * was returned from the <schedule> method).  If act is non-NULL
	     * then it will be set to point to the ``magic cookie'' argument
	     * passed in when the timer was registered.  This makes it possible
	     * to free up the memory and avoid memory leaks. Returns 1 if
	     * cancellation succeeded and 0 if the <timer_id> wasn't found.
	     *
		 */
	    int cancel ( long timer_id,
                     const void **act = 0 );

		/*
		 *	thr_count
		 *
		 *  Returns the number of currently running threads
		 *
		 */
		int thr_count();

        /**
	 	 *
		 * timer_queue
		 *
		 * This function is here just to make this look like ACE
         * m_activeQueue.timer_queue().reset_interval(timerId, interval); // ACE_5.5 and before
         * m_activeQueue.timer_queue()->reset_interval(timerId, interval); // ACE_5.6 and later
		 *
		 */
        //WindowsTimerQueue& timer_queue() { return *this; }; 
        WindowsTimerQueue* timer_queue() { return this; };

		/*
		 * timerCallback
		 *
		 * Internal timer callback - this is public so the C Style 
		 * Windows timer can call the function
		 *
		 */
		void timerCallback(unsigned int id);

	protected:

		//
		// Internal timer functions
		//
		void processTimers();
		
		void stopInternalTimer();
		void expireTimers();
		void restartInternalTimer();

		//
		// Functions required by Thread base class
		//
		void run();
		void terminate();

    private:

		//
		// Structure representing a timer
		//
        struct TimerEntry
        {
            long timerId;
            ACE_Time_Value nextTimeout;
            ACE_Time_Value interval;
            const void* act;
            ACE_Event_Handler* handler;

			TimerEntry( long id,
                        const ACE_Time_Value& nextTimeoutTV,
                        const ACE_Time_Value& intervalTV,
                        const void *data,
                        ACE_Event_Handler *callback)
            {
                timerId = id;
                nextTimeout = nextTimeoutTV;
                interval = intervalTV;
                act = data;
                handler = callback;
            }

			// Why do we have == 1 here?
			// Because ACE returns 1 for true explicitly
			// and the return type is int
            inline bool operator<(const TimerEntry& rhs) const
            {
                return (nextTimeout < rhs.nextTimeout) == 1 ||
                    (nextTimeout == rhs.nextTimeout && timerId < rhs.timerId);
            }
        };
        
		//
		// Internal timer list
		//
        ReEntrantThreadLockable					m_timerlistLock;
        std::set<TimerEntry>				    m_timerList;
		
        //
		// State for internal timer
		//
		unsigned int							m_timerId;
        UINT									m_timerRes;
        UINT                                    m_timerMaxPeriod;
		ACE_Time_Value							m_nextTimeout;
		ACE_Time_Value							m_resolutionValue;

		//
		// Pool of timer Ids used to ensure we don't get conficts
		// ACE doesn't seem to care about this - this may be
		// a reason to stop using ACE on solaris and use this class
		//
		std::queue<long>						m_timerIds;

		//
		// State for management of the internal thread
		//
		unsigned int							m_threadId;
		Semaphore								m_processTimersSemaphore;
		volatile bool							m_keepRunning;
    };
}

#endif