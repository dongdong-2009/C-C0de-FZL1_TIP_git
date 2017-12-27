/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/AbstractThreadedTimeoutUser.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *	The class TimeoutUser associates a timer user, implemented as a callback interface,
  * with the TimerUtil to provide a callback mechanism when the timeout period that is
  * specified by the timer user has expired.
  *
  */

#ifndef ABSTRACTTHREADEDTIMEOUTUSER_H
#define ABSTRACTTHREADEDTIMEOUTUSER_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/IWorkItem.h"
#include "core/timers/src/ITimeoutCallback.h"

namespace TA_Base_Core
{
	class SingletonTimerUtil;
};

namespace TA_Base_Core
{
	class AbstractThreadedTimeoutUser : public ITimeoutCallback
	{
	public:

		/**
		*
		* AbstractThreadedTimeoutUser
		*
		* Parameterised contructor
		*
		* @param    user					The registered user associated with a TimerUtil
		* @param    timeoutPeriodMSeconds	The specified timeout period in mseconds
		*
		*/
		AbstractThreadedTimeoutUser ();


		/**
		*
		* ~AbstractThreadedTimeoutUser
		*
		* Destructor
		*
		*/
		virtual ~AbstractThreadedTimeoutUser();

		/**
		*
		* timerExpired
		*
		* This operation will be called when the registered timeout period has expired.
		*
		*/
		virtual void timerExpired(long timerId, void* userData);

		/**
		*
		* threadedTimerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired by a worker thread.
		*
		*/
		virtual void threadedTimerExpired(long timerId, void* userData) = 0;

	protected:	

		SingletonTimerUtil* m_singletonTimerUtil;
		ReEntrantThreadLockable m_lock;

	};

	class ThreadedTimeoutWorkItem : public IWorkItem
	{

	public:

		ThreadedTimeoutWorkItem(AbstractThreadedTimeoutUser& parent, long timerId, void* userData);

		virtual ~ThreadedTimeoutWorkItem();

		//
		// Operations specific to IWorkItem
		//
		virtual void executeWorkItem();

	protected:

		AbstractThreadedTimeoutUser& m_parent;
        long m_timerId;
        void* m_userData;
	};
}

#endif
