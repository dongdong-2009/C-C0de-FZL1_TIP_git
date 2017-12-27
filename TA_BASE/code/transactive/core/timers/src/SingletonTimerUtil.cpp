/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/timers/src/SingletonTimerUtil.cpp $
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


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER


#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "core/threads/src/ThreadPoolManager.h"

#include "core/timers/src/SingletonTimerUtil.h"

namespace
{
    const unsigned long DEACTIVATE_DELAY( 100 );
    const unsigned long MAX_DEACTIVATE_ATTEMPT( 10 );
}

namespace TA_Base_Core
{

	//
	// Initialisation of statics
	//
	SingletonTimerUtil *					        SingletonTimerUtil::m_singletonTimerUtil = 0;
	unsigned int							        SingletonTimerUtil::m_singletonRefCounter = 0;
	ReEntrantThreadLockable							SingletonTimerUtil::m_singletonLock;

	//
	// SingletonTimerUtil
	//
	SingletonTimerUtil::SingletonTimerUtil()
	{
		// create ThreadedWorker objects, based on the runtime parameter
		// "ThreadedWorkers". Use the default number if not defined
		int maxNumber = 30;
		if ( true == RunParams::getInstance().isSet ( "TimerThreadedWorkers" ) )
		{
			maxNumber = atoi ( RunParams::getInstance().get( "TimerThreadedWorkers" ).c_str() );
		}

		m_threadPoolManager = new ThreadPoolManager(maxNumber);
        m_invalidated = false;

		TA_ASSERT (m_threadPoolManager != NULL, "Thread Pool Manager not created");
	};

	//
	// ~SingletonTimerUtil
	//
	SingletonTimerUtil::~SingletonTimerUtil()
	{
		// clean up internal objects and the background task running
		m_threadPoolManager->cleanUp();
		delete m_threadPoolManager;
		m_threadPoolManager = NULL;
	}

	//
	// getInstance
	//
	SingletonTimerUtil & SingletonTimerUtil::getInstance()
	{
		// prevent multiple threads creating multiple instances
		// simultaneously
		ThreadGuard guard( m_singletonLock );

		// if SingletonTimerUtil has not been created yet
		if ( 0 == m_singletonTimerUtil )
		{
			// create one and only one instance
			m_singletonTimerUtil = new SingletonTimerUtil();
		}

		// increment reference counter and return the single instance of
		// SingletonTimerUtil
		m_singletonRefCounter++;
		return *m_singletonTimerUtil;
	}

	//
	// removeInstance
	//
	void SingletonTimerUtil::removeInstance()
	{
		// prevent multiple threads creating multiple instances
		// simultaneously
		ThreadGuard guard( m_singletonLock );

		m_singletonRefCounter--;

		// if not referenced anywhere
		if ( 0 == m_singletonRefCounter)
		{
			delete m_singletonTimerUtil;
			m_singletonTimerUtil = 0;
		}
	}

	//
	// startPeriodicTimeOutClock
	//
	long SingletonTimerUtil::startPeriodicTimeOutClock ( ITimeoutCallback * user,
														 unsigned long timeoutPeriodMSeconds,
														 bool singleShot,
                                                         void* userData)
	{
        if ( true == m_invalidated )
        {
            return -1;
        }

        return m_timerUtil.scheduleTimer(user, timeoutPeriodMSeconds, singleShot, userData);
	}

	//
	// stopPeriodicTimeOutClock
	//
	std::vector<void*> SingletonTimerUtil::stopPeriodicTimeOutClock ( ITimeoutCallback * user )
	{
        if ( true == m_invalidated )
        {
            return std::vector<void*>();
        }

        return m_timerUtil.cancelTimer(user);
	}

    //
	// stopPeriodicTimeOutClock
	//
	void* SingletonTimerUtil::stopPeriodicTimeOutClock ( ITimeoutCallback * user, long timerId )
	{
        if ( true == m_invalidated )
        {
            return NULL;
        }

        return m_timerUtil.cancelTimer(user, timerId);
	}


	void SingletonTimerUtil::queueWorkItem ( IWorkItemPtr workItem )
	{
        if ( false == m_invalidated )
        {
            m_threadPoolManager->queueWorkItem(workItem);
        }
	}

	void SingletonTimerUtil::removeWorkItem ( IWorkItemPtr workItem )
	{
        if ( false == m_invalidated )
        {
            m_threadPoolManager->removeWorkItem(workItem);
        }
	}

    void SingletonTimerUtil::invalidate()
    {
        if ( false == m_invalidated )
        {
            m_invalidated = true;
            m_threadPoolManager->cleanUp();
            m_timerUtil.invalidate();
        }
    }
}
