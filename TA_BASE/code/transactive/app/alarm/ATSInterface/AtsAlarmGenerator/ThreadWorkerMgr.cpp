#include "StdAfx.h"
#include "ThreadWorkerMgr.h"
#include "ThreadGuard.h"

CThreadWorkerMgr::CThreadWorkerMgr(HWND hwndDlg, int nThreadCount, int nSubmitInterval, ULONG ulAlarmCount)
				: m_nThreadCount(nThreadCount)
				, m_nSubmitInterval(nSubmitInterval)
				, m_ulAlarmCount(ulAlarmCount)
				, m_bSubmitFinish(false)
				, m_bThreadWorkerStop(true)
				, m_bTerminating(false)
{
	m_hwndDlg = hwndDlg;
	
	if (m_nThreadCount > (int)m_ulAlarmCount)
	{
		MessageBox(this->m_hwndDlg, "Thread workers exceed the total alarms. threads will create according to the number of alarms.", "In correct fields", MB_OK | MB_ICONINFORMATION);
		m_nThreadCount = m_ulAlarmCount;
	}
	// Create ThreadWorkers
	for (int nIdx = 0; nIdx < m_nThreadCount; nIdx++)
	{
		CThreadWorker * pThreadWorker = NULL;
		pThreadWorker = new CThreadWorker(this, m_nSubmitInterval);
		if (NULL != pThreadWorker)
		{
			m_threadWorkers.push_back(pThreadWorker);
		}
	}
}

CThreadWorkerMgr::~CThreadWorkerMgr(void)
{
	if (false == m_bThreadWorkerStop)
	{
		stopAutoSubmit();
	}
	
	// deallocate
	for (int nIdx = 0; nIdx < m_nThreadCount; nIdx++)
	{
		if (NULL != m_threadWorkers[nIdx])
		{
			delete m_threadWorkers[nIdx];
			m_threadWorkers[nIdx] = NULL;
		}
	}

	m_threadWorkers.clear();
}

void CThreadWorkerMgr::startAutoSubmit()
{
	//TA_Base_Bus::ThreadGuard guard(m_threadLock);
	// Start
	m_bSubmitFinish = false;
	for (int nIdx = 0; nIdx < m_nThreadCount; nIdx++)
	{
		if (NULL != m_threadWorkers[nIdx])
		{
			m_threadWorkers[nIdx]->start();
		}
	}

	m_bThreadWorkerStop = false;
}

void CThreadWorkerMgr::stopAutoSubmit()
{
	//TA_Base_Bus::ThreadGuard guard(m_threadLock);
	m_bTerminating = true;
	// Start
	for (int nIdx = 0; nIdx < m_nThreadCount; nIdx++)
	{
		if (NULL != m_threadWorkers[nIdx] && TA_Base_Core::Thread::THREAD_STATE_RUNNING == m_threadWorkers[nIdx]->getCurrentState())
		{
			m_threadWorkers[nIdx]->terminateAndWait();
		}
	}
	m_bTerminating = false;
	m_bThreadWorkerStop = true;
}

bool CThreadWorkerMgr::decrementAlarmCount()
{
	if (m_bTerminating) return false;
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	bool bRetVal = true;
	if (m_ulAlarmCount != 0)
		--m_ulAlarmCount;
	else
		bRetVal = false;
	
	return bRetVal;
}

void CThreadWorkerMgr::notifySubmitFinish()
{
	if (m_bTerminating) return;
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	if (false == m_bSubmitFinish)
	{
		m_bSubmitFinish = true;
		PostMessage(m_hwndDlg, WMU_AUTO_SUBMIT_FINISH, NULL, NULL);
	}
}