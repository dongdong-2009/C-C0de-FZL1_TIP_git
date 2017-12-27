/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/WorkerPool.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "WorkerPool.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

const int ADD_WORKER_WAITING_SECONDS = 10;

namespace TA_Base_Bus
{
	WorkerPool::WorkerPool(unsigned long numThreads, unsigned long upperLimit)
        : m_originalSize(numThreads),
          m_currentSize(numThreads),
          m_upperLimit(upperLimit),
		  m_bInService(false)
	{
		m_idleWorkers.clear();
		m_allWorkers.clear();
		m_idleTasks.clear();
		m_toExecuteTasks.clear();
		m_allTasks.clear();
		ftime( &m_lastAllTaskFinished );
	
		for ( unsigned long i = 1; i <= numThreads; i++ )
		{
			ThreadWorker * newWorker = new ThreadWorker(this);
			newWorker->start();
			m_allWorkers.push_back(newWorker);
			m_idleWorkers.push_back (newWorker);
		}
		m_threadRunning = true;
		start();
	}


	WorkerPool::~WorkerPool()
	{
		m_bInService = false;
		terminateAndWait();
        cleanUp();
	}


	void WorkerPool::cleanUp()
	{
		if ( THREAD_STATE_RUNNING == getCurrentState() )
		{
			terminateAndWait();
		}

		std::list<ThreadWorker*>::const_iterator it = m_allWorkers.begin();
		while (m_allWorkers.end() != it)
		{
			ThreadWorker* worker = *it;
			worker->terminateAndWait();
			delete worker;
			++it;
		}
		m_allWorkers.clear();
        m_currentSize = 0;
	}

	void WorkerPool::terminate()
	{
		m_threadRunning = false;
		m_semaphore.post();
	}


	void WorkerPool::run()
	{
		unsigned long runTimes = 0;
        while (true == m_threadRunning)
        {
            try
            {
				if (m_bInService)
				{
					scanIdleTasks();
					processTasks();
				}
            }
            catch( ... )
            {
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "unknow exception when WorkerPool::run");
            }
			++runTimes;
			if (0 == (runTimes % 10000))
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "worker pool is running, service state: %d, having %d workers, having %d tasks, todo %d tasks",
					m_bInService, m_allWorkers.size(), m_allTasks.size(), m_toExecuteTasks.size());
			}
			TA_Base_Core::Thread::sleep(60);
        }
	}


	void WorkerPool::queueTask(IWorkerTask* task)
	{
		if (0 == task)
		{
			return;
		}
		ThreadGuard guard(m_tasksLock);
		std::map<IWorkerTask*, bool>::const_iterator it = m_allTasks.find(task);
		if (m_allTasks.end() != it)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "task have existed.");
			return;
		}
		m_allTasks[task] = true;
		m_idleTasks.push_back(task);
	}

	void WorkerPool::scanIdleTasks()
	{
		ThreadGuard guard(m_tasksLock );
		std::list<IWorkerTask*>::iterator it = m_idleTasks.begin();
		while (m_idleTasks.end() != it)
		{
			if ((*it)->taskTimeExpired())
			{
				m_toExecuteTasks.push_back(*it);
				it = m_idleTasks.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	
	void WorkerPool::processTasks()
	{
		bool allFinished = true;
		ThreadGuard guard(m_tasksLock );
		std::list<IWorkerTask*>::iterator it = m_toExecuteTasks.begin();
		while (m_toExecuteTasks.end() != it)
		{
			ThreadWorker* worker = getIdleWorker();
			if (worker)
			{
				worker->assignTask(*it);
				it = m_toExecuteTasks.erase(it);
			}
			else
			{
				allFinished = false;
				timeb currentTimeStamp;
				ftime( &currentTimeStamp );
				if (currentTimeStamp.time - ADD_WORKER_WAITING_SECONDS > m_lastAllTaskFinished.time)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "there is no more idle worker, there are %d tasks, current worker size:%d",
						m_toExecuteTasks.size(), m_allWorkers.size());
				}
				break;
			}
		}
		if (allFinished)
		{
			ftime( &m_lastAllTaskFinished );
		}
	}

    void WorkerPool::finishedTask(IThreadWorker* worker, IWorkerTask* task)
    {
		ThreadWorker* realWorker = dynamic_cast<ThreadWorker*>(worker);
		if (0 == realWorker)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "dynamic cast worker error");
			return ;
		}
		{
			ThreadGuard guard(m_tasksLock );
			m_idleTasks.push_back(task);
		}
		{
			ThreadGuard guard(m_workersLock );
			m_idleWorkers.push_back(realWorker);
		}
    }

	ThreadWorker * WorkerPool::getIdleWorker()
	{
		ThreadWorker * worker = 0;
		{
			ThreadGuard guard(m_workersLock );
			if (m_idleWorkers.size())
			{
				worker = m_idleWorkers.front();
				m_idleWorkers.pop_front();
			}
		}

        if (0 == worker && m_currentSize < m_upperLimit)
        {
			timeb currentTimeStamp;
			ftime( &currentTimeStamp );
			if (currentTimeStamp.time - ADD_WORKER_WAITING_SECONDS > m_lastAllTaskFinished.time)
			{
				worker = new ThreadWorker(this);
				worker->start();

				m_allWorkers.push_back ( worker );
				++m_currentSize;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "All threads are busy, new thread created, current pool size is %d.", m_currentSize);

				// refresh m_lastAllTaskFinished
				m_lastAllTaskFinished = currentTimeStamp;
			}

        }

		return worker;
	}

}
