#include "StdAfx.h"
#include "CommonDefine.h"
#include "RequestHandler.h"
#include "CommandHandler.h"

using namespace I_ALARM_PROVIDER;

extern IAlarmProvider *g_pAtsInterface;

CRequestHandler::CRequestHandler(CATSListCtrl* pListCtrl) 
				: m_bTerminate(false)
				, m_pAtsListCtrl(0)
{
	m_pAtsListCtrl = pListCtrl;
}

CRequestHandler::~CRequestHandler(void)
{
}

void CRequestHandler::run()
{
	ProfileInfo profileOld;
	CCommandHandler::getInstance().getProfileInfo(profileOld);
	while(!m_bTerminate)
	{
		if(g_pAtsInterface)
		{
			//retrieve ack
			std::list<AlarmRequest*> RequestUpdateList;
			g_pAtsInterface->retrieveAlarmRequestUpdate(RequestUpdateList);
			for(std::list<AlarmRequest*>::iterator itr = RequestUpdateList.begin();
				itr != RequestUpdateList.end(); 
				++itr)
			{
				if (m_pAtsListCtrl)
				{
					AlarmRequest* pReq = *itr;
					I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail = m_pAtsListCtrl->getAlarmDetails(pReq->alarmId);
					if (pAlarmDetail)
					{
						if (pReq->state == I_ALARM_PROVIDER::AlarmAcknowledged)
						{
							pAlarmDetail->alarmAcknowledgeBy = pReq->operatorID;
							pAlarmDetail->ackTime = pReq->requestTime;
							CCommandHandler::getInstance().acknowledgeAlarm(pAlarmDetail);
						}
						else if (pReq->state == I_ALARM_PROVIDER::AlarmClosed)
						{
							if (pAlarmDetail->state != I_ALARM_PROVIDER::AlarmAcknowledged)
							{
								pAlarmDetail->alarmAcknowledgeBy = pReq->operatorID;
								pAlarmDetail->ackTime = pReq->requestTime;
								CCommandHandler::getInstance().acknowledgeAlarm(pAlarmDetail);
							}
							
							if (pAlarmDetail->state != I_ALARM_PROVIDER::AlarmClosed)
							{
								pAlarmDetail->closeTime = pReq->requestTime;
								CCommandHandler::getInstance().closeAlarm(pAlarmDetail);
							}
						}
						
					}
					delete pReq;
					pReq = NULL;
				}
			}

			//retrieve mms
			std::list<MmsJobRequest*> MmsupdateList;
			g_pAtsInterface->retrieveMmsJobRequest(MmsupdateList);
			for(std::list<MmsJobRequest*>::iterator itr1 = MmsupdateList.begin();
				itr1 != MmsupdateList.end(); 
				++itr1)
			{
				if (m_pAtsListCtrl)
				{
					MmsJobRequest* mmsJobReq = *itr1;
					I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail = m_pAtsListCtrl->getAlarmDetails(mmsJobReq->AlarmId);
					if (pAlarmDetail)
					{
						pAlarmDetail->mmsState = I_ALARM_PROVIDER::MMS_JR_SENT;
						CCommandHandler::getInstance().updateMMSState(pAlarmDetail);
					}
					delete mmsJobReq;
					mmsJobReq = NULL;
				}
			}

			ProfileInfo profile;
			g_pAtsInterface->getProfileInfo(profile);
			if ( profileOld.updateTime != profile.updateTime )
			{
				profileOld.operatorId = profile.operatorId;
				profileOld.profileId = profile.profileId;
				profileOld.loginLocation = profile.loginLocation;
				profileOld.displayOnly = profile.displayOnly;
				profileOld.updateTime = profile.updateTime;

				CCommandHandler::getInstance().setProfileInfo(profile);

				// Resend all the existing alarms
				for (int nIndex = 0; nIndex < m_pAtsListCtrl->GetItemCount(); nIndex++)
				{
					I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail = NULL;
					pAlarmDetail = (I_ALARM_PROVIDER::AlarmDetail*)m_pAtsListCtrl->GetItemData(nIndex);

					CCommandHandler::getInstance().submitAlarm(pAlarmDetail, false);
				}
			}
		}
		Sleep(1000);
	}
}

void CRequestHandler::terminate()
{
	m_bTerminate = true;
}
