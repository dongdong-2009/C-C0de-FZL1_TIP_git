#include "StdAfx.h"
#include "ThreadGuard.h"
#include "ThreadAutoAckClose.h"
#include "CommandHandler.h"

CThreadAutoAckClose::CThreadAutoAckClose(int nAckCloseInterval, HWND hWndTarget) 
					: m_bTerminate(false)	 
					, m_nAckCloseInterval(nAckCloseInterval)
					, m_bfinishAutoAck(false)
{
	m_hWndTarget = hWndTarget;
}

CThreadAutoAckClose::~CThreadAutoAckClose(void)
{
	if (TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState())
	{
		terminateAndWait();
	}
}

void CThreadAutoAckClose::run()
{
	m_hExitEv = CreateEvent(NULL, TRUE, FALSE, NULL);
	while (false == m_bTerminate || true == m_bfinishAutoAck)
	{
		AlarmQueueItem* pQueueItem = NULL;
		{
			TA_Base_Bus::ThreadGuard guard(m_threadLock);
			if (m_pAlarmQueue.size() != 0)
			{
				ALARMDETAIL_QUEUE::iterator itr = m_pAlarmQueue.begin();
				if (difftime(time(0), (*itr)->t_submit) > m_nAckCloseInterval)
				{
					pQueueItem = *itr;
					m_pAlarmQueue.erase(itr);
				}
			}
			else if (m_bfinishAutoAck)
			{
				PostMessage(m_hWndTarget, WMU_AUTO_ACKCLOSE_FINISH, NULL, NULL);
				m_bfinishAutoAck = false; // finish Auto ack/close
			}
		}
		if (NULL != pQueueItem)
		{
			pQueueItem->pAlarmDetail->ackTime = time(0);
			pQueueItem->pAlarmDetail->alarmAcknowledgeBy = "C952-OPT";
			if (pQueueItem->pAlarmDetail->state == I_ALARM_PROVIDER::AlarmReturned)
			{
				CCommandHandler::getInstance().acknowledgeAlarm(pQueueItem->pAlarmDetail);
			}
			else
			{
				CCommandHandler::getInstance().acknowledgeAlarm(pQueueItem->pAlarmDetail);
				Sleep(10); // 
				CCommandHandler::getInstance().closeAlarm(pQueueItem->pAlarmDetail);
			}
		}
		else
			Sleep(m_nAckCloseInterval);

		if (m_bTerminate && false == m_bfinishAutoAck)
			break;
	}
	SetEvent(m_hExitEv);
}

void CThreadAutoAckClose::terminate()
{
	m_bTerminate = true;
	WaitForSingleObject(m_hExitEv, INFINITE);

	if (m_hExitEv)
	{
		CloseHandle(m_hExitEv);
	}
}

void CThreadAutoAckClose::insert(AlarmQueueItem* pQueueItem)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	m_pAlarmQueue.push_back(pQueueItem);
}

void CThreadAutoAckClose::remove(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	ALARMDETAIL_QUEUE::iterator itr = m_pAlarmQueue.begin();
	while(itr != m_pAlarmQueue.end())
	{
		AlarmQueueItem* ptemp = *itr;
		if ((DWORD)pAlarmDetail == (DWORD)ptemp->pAlarmDetail)
		{
			m_pAlarmQueue.erase(itr);
			break;
		}
		itr++;
	}
}

void CThreadAutoAckClose::finishedAutoAck()
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	m_bfinishAutoAck = true;
	m_bTerminate = true;
	
	//terminateAndWait();
}