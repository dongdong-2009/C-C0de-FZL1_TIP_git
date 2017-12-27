/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/SCADA_development/TA_BASE/transactive/core/threads/src/LFThreadPoolManager.cpp $
  * @author:	HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  *

  */

#include <sstream>
#include "./LFThreadPoolManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
	LFThreadPoolManager::LFThreadPoolManager(unsigned long numThreads, bool autoStart)
        : m_semaphore(0),
          m_currentSize(numThreads),
		  m_threadRunning(false),
		  m_isLeaderSet(false)
	{
		m_threadWorkerList.clear();
	
		for ( unsigned long i = 1; i <= numThreads; i++ )
		{
			LFThreadPoolWorker * newWorker = 0;
			newWorker = new LFThreadPoolWorker(*this);

			// start up background thread of the workers
			newWorker->start();

			// save to internal list for later usage
			m_threadWorkerList.push_back ( newWorker );
			m_workerList.InsertItem(newWorker);
		}
		if (autoStart)
		{
			start();
		}
	}

	LFThreadPoolManager::~LFThreadPoolManager()
	{
		m_threadRunning = false;
        cleanUp();
	}

	void LFThreadPoolManager::start()
	{
		if (true == m_threadRunning)
		{
			return ;
		}
		m_threadRunning = true;
		setNewLeader();
	}

	void LFThreadPoolManager::cleanUp()
	{
		{
			TA_THREADGUARD(m_workerListLock);
			m_workerList.cleanUp();
			m_semaphore.post();
		}
		std::vector < LFThreadPoolWorker * >::iterator threadedWorkerItr;
		for ( threadedWorkerItr = m_threadWorkerList.begin();
			  threadedWorkerItr != m_threadWorkerList.end();
			  threadedWorkerItr++ )
		{
			m_semaphore.post();
			(*threadedWorkerItr)->terminateAndWait();

			delete (*threadedWorkerItr);
		}
		
		TA_THREADGUARD( m_workItemListLock );
		m_itemList.cleanUp();
	}

	void LFThreadPoolManager::queueWorkItem ( IThreadPoolExecuteItem* workItem )
	{
		TA_THREADGUARD( m_workItemListLock );
		m_itemList.InsertItem(workItem);
		m_semaphore.post();
	}
	
	void LFThreadPoolManager::removeWorkItem ( IThreadPoolExecuteItem* workItem )
	{
		// stop multiple threads accessing the internal list
		TA_THREADGUARD( m_workItemListLock );

		m_itemList.removeItem(workItem);
		// else do nothing
	}

	void LFThreadPoolManager::queueWorkItemList(std::vector<IThreadPoolExecuteItem*>& WorkItemList)
	{
		int i = 0;
		int cnt = WorkItemList.size();
		TA_THREADGUARD( m_workItemListLock );
		for (i = 0; i < cnt; ++i)
		{
			IThreadPoolExecuteItem* item = WorkItemList[i];
			m_itemList.InsertItem(item);
			m_semaphore.post();
		}
	}

	IThreadPoolExecuteItem* LFThreadPoolManager::leaderExecuteItem()
	{
		if ( false == m_threadRunning )
		{
			return 0;
		}
		m_semaphore.wait();

		IThreadPoolExecuteItem* item = 0;
		{
			TA_THREADGUARD( m_workItemListLock );
			item = static_cast<IThreadPoolExecuteItem*>(m_itemList.pop());
		}
		setNewLeader();

		return item;
	}

	void LFThreadPoolManager::setNewLeader()
	{
		LFThreadPoolWorker * threadedWorker = 0;

		TA_THREADGUARD(m_workerListLock);
		threadedWorker = static_cast<LFThreadPoolWorker *>(m_workerList.pop());

		// if there is one
		if ( 0 != threadedWorker )
		{
			m_isLeaderSet = true;
			threadedWorker->changeToLeader();
		}
		else
		{
			m_isLeaderSet = false;
			std::stringstream ss;
			ss << "There are no threaded workers available to perform any of the queued work items.  Total number of queued work items = " << m_itemList.size();
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str()  );
		}
	}

    void LFThreadPoolManager::workerFree(IListItem* worker)
    {
		TA_THREADGUARD(m_workerListLock);
		if ( false == m_threadRunning )
		{
			return ;
		}
		if (m_isLeaderSet)
		{
			m_workerList.InsertItem(worker);
		}
		else
		{
			m_isLeaderSet = true;
			LFThreadPoolWorker* threadedWorker = static_cast<LFThreadPoolWorker *>(worker);
			threadedWorker->changeToLeader();
		}
    }

    long LFThreadPoolManager::getQueueSize()
    {
        TA_THREADGUARD( m_workItemListLock );
        return (unsigned long)m_itemList.size();
    }
    
    unsigned long LFThreadPoolManager::getNumberOfThreads()
    {
        return m_currentSize;
    }
}
