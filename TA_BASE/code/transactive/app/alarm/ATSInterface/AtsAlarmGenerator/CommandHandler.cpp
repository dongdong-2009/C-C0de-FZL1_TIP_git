#include "StdAfx.h"
#include "CommonDefine.h"
#include "CommandHandler.h"
#include "ThreadGuard.h"

using namespace I_ALARM_PROVIDER;

IAlarmProvider *g_pAtsInterface = NULL;

//Initialize
CCommandHandler* CCommandHandler::m_pInstance = NULL;
TA_Base_Bus::NonReEntrantThreadLockable CCommandHandler::m_singletonLock;

CCommandHandler& CCommandHandler::getInstance()
{
	TA_Base_Bus::ThreadGuard guard(m_singletonLock);
	if (m_pInstance == NULL)
	{
		m_pInstance = new CCommandHandler();
	}
	return *m_pInstance;
}

void CCommandHandler::freeInstance()
{
	TA_Base_Bus::ThreadGuard guard(m_singletonLock);
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CCommandHandler::CCommandHandler(void) 
				: m_pProfileInfo(0)
				, m_pUpdateObserver(0)
				, m_bTerminate(false)
				, m_semaphore(0)
				, m_ulProviderID(VA_SUBSYSTEM_ID)
{
	g_pAtsInterface = getAlarmProvider(m_ulProviderID);
	m_semaphore = new CSemaphoreLock(0, 500, "CCommandHandleSemaphore");
	start();
}

CCommandHandler::~CCommandHandler(void)
{
	terminateAndWait();
	if (NULL != m_pProfileInfo)
	{
		delete m_pProfileInfo;
		m_pProfileInfo = NULL;
	}
	if (NULL != m_semaphore)
	{
		delete m_semaphore;
		m_semaphore = NULL;
	}

	freeAlarmProvider(m_ulProviderID);
}

bool CCommandHandler::submitAlarm(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail, bool bUpdateList/* = true*/)
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
	bool bResult = false;

	if (NULL != g_pAtsInterface && NULL != m_pProfileInfo)
	{
		RequestMgs* pRequest = new RequestMgs;
		pRequest->mgsType = NEW_ALARM;
		pRequest->pAlarmDetail = pAlarmDetail;
		pRequest->bUpdateList = bUpdateList;
		m_requestQue.push(pRequest);
		m_semaphore->release();
		bResult = true;
	}

	return bResult;
}
bool CCommandHandler::acknowledgeAlarm(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail)
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
	bool bResult = false;

	if (NULL != g_pAtsInterface && NULL != m_pProfileInfo)
	{
		RequestMgs* pRequest = new RequestMgs;

		if (pAlarmDetail->state == I_ALARM_PROVIDER::AlarmReturned)
		{	
			pRequest->mgsType = CLOSE_ALARM;
			pAlarmDetail->state = I_ALARM_PROVIDER::AlarmClosed;
		}
		else
		{
			pRequest->mgsType = UPDATE_ALARM;
			pAlarmDetail->state = I_ALARM_PROVIDER::AlarmAcknowledged;
		}

		pRequest->pAlarmDetail = pAlarmDetail;
		m_requestQue.push(pRequest);
		m_semaphore->release();
		
		bResult = true;
	}
	
	return bResult;
}

bool CCommandHandler::closeAlarm(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail)
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
	bool bResult = false;

	if (NULL != g_pAtsInterface && NULL != m_pProfileInfo)
	{
		pAlarmDetail->closeTime = time(0);
		pAlarmDetail->state = I_ALARM_PROVIDER::AlarmClosed;
		
		RequestMgs* pRequest = new RequestMgs;
		pRequest->mgsType = CLOSE_ALARM;
		pRequest->pAlarmDetail = pAlarmDetail;
		m_requestQue.push(pRequest);
		m_semaphore->release();
		bResult = true;
	}

	return bResult;
}

bool CCommandHandler::normalizeAlarm(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail)
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
	bool bResult = false;

	if (NULL != g_pAtsInterface && NULL != m_pProfileInfo)
	{
		RequestMgs* pRequest = new RequestMgs;
		pAlarmDetail->sourceTime.time = time(0);
		pAlarmDetail->closeTime = time(0);
		if (pAlarmDetail->state == I_ALARM_PROVIDER::AlarmAcknowledged)
		{			
			pAlarmDetail->state = I_ALARM_PROVIDER::AlarmClosed;
			pRequest->mgsType = CLOSE_ALARM;
		}
		else
		{
			pAlarmDetail->state = I_ALARM_PROVIDER::AlarmReturned;
			pRequest->mgsType = UPDATE_ALARM;
		}
		pRequest->pAlarmDetail = pAlarmDetail;
		m_requestQue.push(pRequest);	
		m_semaphore->release();

		bResult = true;
	}

	return bResult;
}

bool CCommandHandler::updateMMSState(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail)
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
	bool bResult = false;

	RequestMgs* pRequest = new RequestMgs;
	pRequest->mgsType = UPDATE_ALARM;
	pRequest->pAlarmDetail = pAlarmDetail;
	m_requestQue.push(pRequest);
	m_semaphore->release();
	return bResult;
}

void CCommandHandler::setProfileInfo(I_ALARM_PROVIDER::ProfileInfo profileInfo)
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);

	// Remove the old profile
	if (m_pProfileInfo != NULL)
	{
		delete m_pProfileInfo;
		m_pProfileInfo = NULL;
	}

	// Set new profile
	m_pProfileInfo = new I_ALARM_PROVIDER::ProfileInfo();
	m_pProfileInfo->operatorId = profileInfo.operatorId;
	m_pProfileInfo->profileId = profileInfo.profileId;
	m_pProfileInfo->loginLocation = profileInfo.loginLocation;
	m_pProfileInfo->displayOnly = profileInfo.displayOnly;
	m_pProfileInfo->updateTime = profileInfo.updateTime;

	RequestMgs* pRequest = new RequestMgs;
	pRequest->mgsType = REMOVE_ALL;
	pRequest->pAlarmDetail = NULL;
	pRequest->bUpdateList = false;
	m_requestQue.push(pRequest);
	m_semaphore->release();
}

void CCommandHandler::getProfileInfo(I_ALARM_PROVIDER::ProfileInfo& profileInfo)
{
	if (NULL != m_pProfileInfo)
	{
		// get profile information
		profileInfo.operatorId = m_pProfileInfo->operatorId;
		profileInfo.profileId = m_pProfileInfo->profileId;
		profileInfo.loginLocation = m_pProfileInfo->loginLocation;
		profileInfo.displayOnly = m_pProfileInfo->displayOnly;
		profileInfo.updateTime = m_pProfileInfo->updateTime;
	}
}

void CCommandHandler::setUpdateTime()
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);

	RequestMgs* pRequest = new RequestMgs;
	pRequest->mgsType = REMOVE_ALL;
	pRequest->pAlarmDetail = NULL;
	m_requestQue.push(pRequest);
	m_semaphore->release();
}

void CCommandHandler::registerObserver(IAlarmUpdate* pObserver)
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
	//Todo: check if the observer has already been registered
	m_pUpdateObserver.push_back(pObserver);
}

void CCommandHandler::unRegisterObserver(IAlarmUpdate* pObserver)
{
	TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
	AlarmUpdateObserver::iterator itr = m_pUpdateObserver.begin();
	for (itr; itr < m_pUpdateObserver.end(); itr++)
	{
		if ((DWORD)(*itr) == (DWORD)pObserver)
		{
			m_pUpdateObserver.erase(itr);
			break;
		}
	}	
}

void CCommandHandler::notifyObserver(UPDATE_TYPE tUpdate, I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail)
{
	switch (tUpdate)
	{
	case NEW_ALARM:
		{
			::PostMessage(m_hTargetWnd, WMU_INSERT_ALARM, WPARAM(time(0)), LPARAM(pAlarmDetail));
// 			for (int nIdx = 0; nIdx < m_pUpdateObserver.size(); nIdx++)
// 			{
// 				m_pUpdateObserver[nIdx]->insertAlarm(time(0), pAlarmDetail);
// 			}	
		}
		break;
	case UPDATE_ALARM:
		{
			::PostMessage(m_hTargetWnd, WMU_UPDATE_ALARM, 0, LPARAM(pAlarmDetail));
// 			for (int nIdx = 0; nIdx < m_pUpdateObserver.size(); nIdx++)
// 			{
// 				//m_pUpdateObserver[nIdx]->updateAlarm(pAlarmDetail);
// 			}	
		}
		break;
	case CLOSE_ALARM:
		{
			::PostMessage(m_hTargetWnd, WMU_REMOVE_ALARM, 0, LPARAM(pAlarmDetail));
// 			for (int nIdx = 0; nIdx < m_pUpdateObserver.size(); nIdx++)
// 			{
// 				m_pUpdateObserver[nIdx]->removeAlarm(pAlarmDetail->alarmID.c_str());
// 			}	
		}
		break;
	case REMOVE_ALL:
		{
			::PostMessage(m_hTargetWnd, WMU_CLEAR_ALARMS, 0, 0);
// 			for (int nIdx = 0; nIdx < m_pUpdateObserver.size(); nIdx++)
// 			{
// 				m_pUpdateObserver[nIdx]->removeAllAlarms();
// 			}	
		}
	}	
}

void CCommandHandler::run()
{
	RequestMgs* pMgs = NULL;
	int nRetryCount = 0;
	while (!m_bTerminate)
	{
		m_semaphore->acquire();

		if (m_bTerminate)
		{
			break;
		}

		try
		{
			if(pMgs == NULL)
			{
				TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
				if (m_requestQue.size() != 0)
				{
					pMgs = m_requestQue.front();
					m_requestQue.pop();
				}
			}

			if (pMgs != NULL && g_pAtsInterface != NULL)
			{
				switch(pMgs->mgsType)
				{
				case NEW_ALARM:
				case UPDATE_ALARM:
				case CLOSE_ALARM:
					g_pAtsInterface->submitAlarm(*(pMgs->pAlarmDetail), *m_pProfileInfo);
					if (pMgs->bUpdateList)
					{
						notifyObserver(pMgs->mgsType, pMgs->pAlarmDetail);
					}
					break;
				case REMOVE_ALL:
					{
						TA_Base_Bus::ThreadGuard guard(m_ThreadLock);
						while(!m_requestQue.empty())
						{
							RequestMgs* ptempMgs = m_requestQue.front();
							m_requestQue.pop();

							delete ptempMgs;
							ptempMgs = NULL;
						}
						g_pAtsInterface->setUpdateTime(time(NULL));
						if (pMgs->bUpdateList)
						{
							notifyObserver(REMOVE_ALL, NULL);
						}
					}
					break;
				}
			
				nRetryCount = 0;
				delete pMgs;
				pMgs = NULL;
				continue;
			}			
		}
		catch(...)
		{
			MessageBox(NULL, "Exception occured on the commandHandler Thread. Some alarms may not be proccess correctly by this simulator", "Fatar Error", MB_ICONERROR | MB_OK);
			if (pMgs != NULL)
			{
				if (nRetryCount < 3) // max retry per alarm
				{
					Sleep(10); // sleep for a while and retry again.
					m_semaphore->release();
					nRetryCount++;
				}
				else
				{
	 				delete pMgs;
	 				pMgs = NULL;
					nRetryCount = 0;
				}				
			}
		}
		
	}
}

void CCommandHandler::terminate()
{
	m_bTerminate = true;
	m_semaphore->release();
}