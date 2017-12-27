/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event_processing/src/RealTimeEventSender.cpp $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by:  $Author: 
 * 
 * RealTimeEventSender is for sending event to real-time agent. 
 *
 */

#include "RealTimeEventSender.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
	//RealTimeEventSender* RealTimeEventSender::m_instance = NULL;
	//TA_Base_Core::NonReEntrantThreadLockable RealTimeEventSender::m_singletonLock;

	/*RealTimeEventSender& RealTimeEventSender::getInstance()
	{
		ThreadGuard guard(m_singletonLock);
		// Create a new instance if it doesn't exist.
		if ( 0 == m_instance )
		{
			m_instance = new RealTimeEventSender();

		}

		return *m_instance;
	}*/

	/*void RealTimeEventSender::releaseInstance()
	{		
		ThreadGuard guard(m_singletonLock);
		// Create a new instance if it doesn't exist.
		if ( 0 != m_instance )
		{
			delete m_instance;
			m_instance = NULL;

		}

	}*/

	RealTimeEventSender::RealTimeEventSender(void)
	{
		ThreadGuard guard(m_workerLock);
		m_worker = new RealTimeEventQueue();
	}

	RealTimeEventSender::~RealTimeEventSender(void)
	{
		/*ThreadGuard guard(m_workerLock);
		if( m_worker != NULL )
		{
			m_worker->terminateAndWait();
			delete m_worker;
			m_worker = 0;
		}*/
		cleanUp();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RealTimeEventSender::~RealTimeEventSender");
	}

	void RealTimeEventSender::processEvent(EventItemInfo *event)
	{
		ThreadGuard guard(m_workerLock);
		m_worker->push(event);
	}

	void RealTimeEventSender::cleanUp()
	{
		//terminates the worker thread 
		//clears the event before Agent shutdown
		ThreadGuard guard(m_workerLock);
		if( m_worker != NULL )
		{
			m_worker->terminateAndWait();
			delete m_worker;
			m_worker = NULL;
		}		
	}

	void RealTimeEventSender::setSessionHelper(ISessionHelper* sessionHelper)
	{
		ThreadGuard guard(m_workerLock);
		if( m_worker != NULL )
		{
			m_worker->setSessionHelper(sessionHelper);
		}

	}
}

