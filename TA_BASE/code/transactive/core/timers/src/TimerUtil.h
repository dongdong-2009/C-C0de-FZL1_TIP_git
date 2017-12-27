/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/TimerUtil.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2015/02/11 16:09:39 $
  * Last modified by:	$Author: Ouyang $
  *
  * Description:
  *
  *
  */

#ifndef TIMER_UTIL_H
#define TIMER_UTIL_H

#ifndef WIN32
	#include "ace/ACE.h"
	#include "ace/Thread_Manager.h"
	#include "ace/Timer_Heap.h"
	#include "ace/Timer_Queue.h"
	#include "ace/Timer_Queue_Adapters.h"
#else
	#include "core/timers/src/WindowsTimerQueue.h"
#endif
#include <vector>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class ITimeoutCallback;
}

namespace TA_Base_Core
{

	class TimerUtil 
		: public ACE_Event_Handler
	{

	public:

		TimerUtil();
		virtual ~TimerUtil();

	

		/**
		*
		* scheduleTimer
		*
		* Schedules an ACE timer
		*
		* @param    timeoutUser					The registered user
		* @param    timeoutPeriodMSeconds		The timeout period in msec
		* @param	singleShot					True if a single shot timer
        * @param    userData	                Optional pointer to user defined data, owned by user, never deleted here.
		*
        * @return   the timer ID
		*/
		long scheduleTimer(ITimeoutCallback * user, unsigned long timeoutPeriodMSeconds, bool singleShot, void* userData = NULL);

		/**
		*
		* cancelTimer
		*
		* Cancels all timers for a user
		*
		* @param    timeoutUser					The registered user
		*
		*/
        std::vector<void*> cancelTimer(ITimeoutCallback * user);

        /**
		*
		* cancelTimer
		*
		* Cancels an ACE timer
		*
        * @param    timeoutUser					The registered user
		* @param    timerId					    The timer id returned by scheduleTimer
		*
		*/
		void* cancelTimer(ITimeoutCallback * user, long timerId);

		/**
		*
		* handle_timeout
		*
		* Handles a timeout from ACE
		*
		*/
		int handle_timeout ( const ACE_Time_Value &tv, const void *constArg );

        void invalidate();

	private:

        struct TimerItem
        {
            ITimeoutCallback * user;
            bool singleShot;
            long timerId;
            void* userData;
        };

        typedef std::multimap<ITimeoutCallback*, long>          TimerMap;

#ifndef WIN32
		typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>	ActiveQueue;
		ActiveQueue												m_activeQueue;
#else
		WindowsTimerQueue										m_activeQueue;
#endif

        ReEntrantThreadLockable									m_mapLock;
        TimerMap                                                m_timerMap;
        volatile bool                                           m_invalidated;
	};

}

#endif
