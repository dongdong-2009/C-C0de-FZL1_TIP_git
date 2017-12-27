
#include "stdafx.h"
#include "app/event/EventGenerator/ThreadSubmitMgr.h"
#include "app/event/EventGenerator/ThreadSubmitWorker.h"

#include "core/synchronisation/src/ThreadGuard.h"

using namespace TA_Base_Core;

ThreadSubmitMgr::ThreadSubmitMgr(int nSubmitInterval, 
				int nBatchSubmitSize,
				int nNumberOfThreads,
				unsigned long ulNumberOfEvents,
				int nSubmitLocation)
				: m_nSubmitInterval(nSubmitInterval)
				, m_nBatchSubmitSize(nBatchSubmitSize)
				, m_nNumberOfThreads(nNumberOfThreads)
				, m_ulNumberOfEvents(ulNumberOfEvents)
				, m_nSubmitLocation(nSubmitLocation)
{

}

ThreadSubmitMgr::~ThreadSubmitMgr()
{
	//Stop delete all threads
	T_vecThreadWorkers::iterator itr = m_thrdSubmitWorkers.begin();
	for (itr; itr != m_thrdSubmitWorkers.end(); itr++)
	{
		ThreadSubmitWorker* pTemp = *itr;
		pTemp->terminateAndWait();

		delete pTemp;
		pTemp = NULL;
	}
	m_thrdSubmitWorkers.clear();
}

void ThreadSubmitMgr::startAutoSubmit()
{
	for (int idx=0; idx<m_nNumberOfThreads; idx++)
	{
		ThreadSubmitWorker* thrdWorker = new ThreadSubmitWorker(this, 
			m_nSubmitInterval, m_nSubmitLocation);

		thrdWorker->start();
		m_thrdSubmitWorkers.push_back(thrdWorker);
	}
}

void ThreadSubmitMgr::stopAutoSubmit()
{
	T_vecThreadWorkers::iterator itr = m_thrdSubmitWorkers.begin();
	for (itr; itr != m_thrdSubmitWorkers.end(); itr++)
	{
		ThreadSubmitWorker* pTemp = *itr;
		pTemp->terminateAndWait();
	}
}

int ThreadSubmitMgr::getEventSize()
{
	TA_Base_Core::ThreadGuard guard(m_threadLock);
	int nRetVal =0;

	if (m_ulNumberOfEvents > m_nBatchSubmitSize)
	{
		nRetVal = m_nBatchSubmitSize;
		m_ulNumberOfEvents -= m_nBatchSubmitSize;
	}
	else
	{
		nRetVal = m_ulNumberOfEvents;
		m_ulNumberOfEvents = 0;
	}

	return nRetVal;
}

unsigned long ThreadSubmitMgr::getUnsubmittedSize()
{
	TA_Base_Core::ThreadGuard guard(m_threadLock);
	return m_ulNumberOfEvents;
}
