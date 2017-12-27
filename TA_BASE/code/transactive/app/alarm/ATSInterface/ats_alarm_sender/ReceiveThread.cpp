// ReceiveThread.cpp: implementation of the CReceiveThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReceiveThread.h"
#include "windows.h"
#include <process.h> 
#include <iostream>
#include "..\ShareMemoryDLL\AlarmApi.h"
using namespace I_ATS_ISCS_ALARM;

extern IATSAlarm *g_pAtsInterface;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
unsigned int __stdcall threadFunc(void* pParam )
{
	CReceiveThread* pThis = static_cast<CReceiveThread*>(pParam);
	ProfileInfo profileOld;
	while(pThis)
	{
		if(g_pAtsInterface)
		{
			//retrieve ack
			std::list<AlarmAckUpdate*> AckupdateList;
			g_pAtsInterface->retrieveATSAlarmAckUpdate(AckupdateList);
			for(std::list<AlarmAckUpdate*>::iterator itr = AckupdateList.begin();
			itr != AckupdateList.end(); 
			++itr)
			{

 				std::cout << "---recv Ack:" 
					<<" " <<(*itr)->alarmId
					<<" " <<(*itr)->alarmAcknowledgeBy
					<<" " <<(*itr)->ackTime
					<< "\n";	
			}

			//retrieve mms
			std::list<MmsJobRequest*> MmsupdateList;
			g_pAtsInterface->retrieveATSMmsJobRequest(MmsupdateList);
			for(std::list<MmsJobRequest*>::iterator itr1 = MmsupdateList.begin();
			itr1 != MmsupdateList.end(); 
			++itr1)
			{
				
				std::cout << "---recv Mms:" 
					<<" " << (*itr1)->AlarmId
					<<" "<<(*itr1)->AssetName 
					<<" "<<(*itr1)->OperatorId 
					<<" "<<(*itr1)->targetDate 
					<<" "<<(*itr1)->problemReport 
					<< "\n";	
			}

            ProfileInfo profile;
			g_pAtsInterface->getProfileInfo(profile);
		    if ( profileOld.updateTime != profile.updateTime )
			{
				//profileOld.profileId != profile.profileId  ||  profileOld.operatorId != profile.operatorId  ||
				std::string displayOnly="false";
				if (profile.displayOnly)
				{	
					 displayOnly="true";
				}
				std::cout << "---recv Profile:" 
					<<"operatorId=" << profile.operatorId
					<<" profileId="<<profile.profileId
					<<" loginLocation="<<profile.loginLocation 
					<<" updateTime="<<profile.updateTime << " displayOnly="<<displayOnly				 
					<< "\n";	

				profileOld.profileId=profile.profileId;
				profileOld.operatorId=profile.operatorId;
				profileOld.updateTime=profile.updateTime;

			}

		}

		Sleep(1000);
	}
	
    return 0;
} 

CReceiveThread::CReceiveThread() : m_hHandle(0)

{
	m_hHandle = _beginthreadex
		(NULL,
		0,
		threadFunc, 
		(void*)this,
		0, 
		NULL);
	
	if(0 == m_hHandle)
	{
		std::cout << "create thread error! \n";		
	}

}

CReceiveThread::~CReceiveThread()
{
}

