/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/ThreadWorker.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "ThreadWorker.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	ThreadWorker::ThreadWorker(IWorkerPool* workPool)
	:
	m_workPool(workPool),
	m_threadRunning ( true ),
	m_task(0),
	m_semaphore(0)
	{
	}

	ThreadWorker::~ThreadWorker()
	{
		if ( THREAD_STATE_RUNNING == getCurrentState() )
		{
			terminateAndWait();
		}
		m_task = 0;
	}

	void ThreadWorker::terminate()
	{
		m_threadRunning = false;
		m_semaphore.post();
	}

	void ThreadWorker::run()
	{
        while (true == m_threadRunning)
        {
			m_semaphore.wait();
            executeTask();
        }
	}

	bool ThreadWorker::assignTask(IWorkerTask* task)
	{
		if ( true == m_threadRunning )
		{
			if ( 0 != task )
			{
				m_task = task;
				m_semaphore.post();
				return true;
			}
		}
		return false;
	}

	void ThreadWorker::executeTask()
	{
		if ( 0 != m_task )
		{
			try
			{
				m_task->executeTask();
			}
			catch ( ... )
			{
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "unknow exception when execute task");
			}
		}
		if (0 != m_workPool)
		{
			IWorkerTask* tTask = m_task;
			m_task = 0;
			m_workPool->finishedTask(this, tTask);
		}
	}

}
