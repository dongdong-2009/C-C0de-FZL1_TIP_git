#include "StdAfx.h"
#include "ThreadWorker.h"
#include "CommandHandler.h"
#include "DataGenerator.h"
#include "ThreadWorkerMgr.h"

#include <windows.h>
CThreadWorker::CThreadWorker(CThreadWorkerMgr* pManager, int nSubmitInterval)
			: m_bTerminate(false), m_nSubmitInterval(nSubmitInterval), m_pManager(0)
{
	m_pManager = pManager;
}

CThreadWorker::~CThreadWorker(void) 
{
}

void CThreadWorker::run()
{
	DWORD dwTimeOut;
	m_hExitEv = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventClose = CreateEvent(NULL, TRUE, FALSE, NULL);
	dwTimeOut = WaitForSingleObject(m_hEventClose, m_nSubmitInterval);
	while (dwTimeOut == WAIT_TIMEOUT)
	{
		if (m_bTerminate)
			break;

 		if (m_pManager != NULL)
 		{
			bool bCanSubmit = m_pManager->decrementAlarmCount();
			if (bCanSubmit)
			{
				I_ALARM_PROVIDER::AlarmDetail * pAlarmDetail = NULL;
				pAlarmDetail = CDataGenerator::getInstance().createRandomAlarm();
				if (NULL != pAlarmDetail)
				{
					CCommandHandler::getInstance().submitAlarm(pAlarmDetail);
				}
			}
			else
				m_pManager->notifySubmitFinish();
		}
				
		dwTimeOut = WaitForSingleObject(m_hEventClose, m_nSubmitInterval);
	}

	SetEvent(m_hExitEv);
}

void CThreadWorker::terminate()
{
	m_bTerminate = true;
	SetEvent(m_hEventClose);

	WaitForSingleObject(m_hExitEv, INFINITE); // wait for exit
	if (m_hEventClose)
	{
		CloseHandle(m_hEventClose);
		m_hEventClose = 0;
	}

	if (m_hExitEv)
	{
		CloseHandle(m_hExitEv);
		m_hExitEv = 0;
	}
}

