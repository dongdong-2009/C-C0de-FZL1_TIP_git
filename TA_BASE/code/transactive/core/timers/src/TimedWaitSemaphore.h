/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/TimedWaitSemaphore.h $
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

#ifndef TIMEDWAITSEMAPHORE_H
#define TIMEDWAITSEMAPHORE_H

#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include <list> 
  
namespace TA_Base_Core
{
	class TimedWaitSemaphore
	{
	public:
		/** 
		  * TimedWaitSemaphore
		  *
		  * Construct a new semaphore object
		  *
		  * @param initial	the initial count. Set to zero if you want the semaphore to
		  *						block on the first call to wait
		  */
		TimedWaitSemaphore( unsigned int initial = 1 );

		/** 
		  * ~TimedWaitSemaphore
		  * 
		  * semaphore desctructor
		  */
		virtual ~TimedWaitSemaphore();

		/** 
		  * wait
		  *
		  * wait (and block) for the semaphore to be signalled, or continue if already signalled
		  *
		  * ie: If semaphore value is > 0 then decrement it and carry on. If it's
		  * already 0 then block.
		  */
		void wait(void);

		/** 
		  * timedWait
		  *
		  * Blocks for a set period of time, or until the semaphore is posted.
		  *
		  * ie: if semaphore value is > 0 within the timeout period then decrement it and return true.
		  * If it's still 0 at the timeout then return false.
		  *
		  * @param	timeoutMsec		timeout period in msec
		  *
		  * @return bool			false if the semaphore times out
		  */
		bool timedWait(unsigned long timeoutMsec);

		/** 
		  * trywait
		  *
		  * test to see if the semaphore will block on wait. This allows your thread to do something
		  * else while it waits for a signal.
		  *
		  * ie: if semaphore value is > 0 then decrement it and return true.
		  * If it's already 0 then return false.
		  *
		  * @return bool	false if the semaphore will block
		  */
		bool tryWait(void);

		/** 
		  * post
		  *
		  * if any threads are blocked in wait(), wake one of them up. Otherwise
		  * increment the value of the semaphore.
		  *
		  * @param void
		  */
		virtual void post(void);
		
	private:

        //
        // Disable the default copy and assignment constructors as shouldn't used.
        //
        TimedWaitSemaphore( const TimedWaitSemaphore& );
        TimedWaitSemaphore& operator=( const TimedWaitSemaphore& );
		
        int m_count;
        std::list<omni_condition*> m_conditions;
        omni_mutex m_mutex;
        volatile bool m_destroyed;
	};
}

#endif // SEMAPHORE_H_INCLUDED
