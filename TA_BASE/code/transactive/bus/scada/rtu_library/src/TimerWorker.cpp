/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PMSAgent/src/RTUPollingWorker.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The TimerWorker object is running timely.
  *
  */

#include <sys/timeb.h>

#include "bus/scada/rtu_library/src/TimerWorker.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

//TD15792, remove AvalancheHeaderDataPointTable processing codes
namespace TA_Base_Bus
{
	TimerWorker::TimerWorker( unsigned int TimeOut)
	:
    m_timeOut( TimeOut ),
    m_inService( false ),
    m_threadTerminated( false ),
	m_timerUtility( TA_Base_Core::SingletonTimerUtil::getInstance() )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Constuct TimerWorker with TimeOut = %d", TimeOut);
 	}


	TimerWorker::~TimerWorker()
	{
        //TD13398, should stop this thread first, then stop the timers.
		// Ensure that TimerWorker thread is stopped
        setInServiceState( false );

		// Stop the timers
		m_timerUtility.stopPeriodicTimeOutClock(this);
		TA_Base_Core::SingletonTimerUtil::removeInstance();

	}


	void TimerWorker::setInServiceState( bool inService )
	{
        // prevent setting the same state
        if (m_inService == inService)
        {
            return;
        }

		m_inService = inService;

		if ( true == inService )
		{
			start();
		}
		else
		{
            terminateAndWait();
		}
	}

	bool TimerWorker::isInService() const
	{
		return m_inService;
	}


	void TimerWorker::terminate()
	{
		// terminate polling thread
		m_threadTerminated = true;

        // signalling wont do any good if its not waiting
        // worst case is wait for the timer
        // it will terminate on the next poll cycle
		m_timerCondition.signal();
	}


	void TimerWorker::run()
	{
		m_timerUtility.startPeriodicTimeOutClock( this, m_timeOut, false );

		while ( false == m_threadTerminated )
		{
			// if the worker thread is in service
			if ( true == m_inService )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "The worker thread is in service, start the process.");
				process();
            }   

			m_timerCondition.wait();
		}

		m_timerUtility.stopPeriodicTimeOutClock(this);

		// exit thread loop --> reset the flag
		m_threadTerminated = false;
	}

	void TimerWorker::process()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Loop test for TimerWorker");
	}
	

	void TimerWorker::timerExpired(long timerId, void* userData)
	{
		m_timerCondition.signal();
	}

	bool TimerWorker::isThreadRunning() const
	{
		return ( THREAD_STATE_RUNNING == getCurrentState() );
	}
}





















