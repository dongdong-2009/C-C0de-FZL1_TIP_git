// ReceiveThread.cpp: implementation of the CReceiveThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReceiveThread.h"
#include "windows.h"
#include <process.h> 
#include <iostream>
#include <list>
#include "..\ShareMemoryDLL\AlarmApi.h"
using namespace I_ATS_ISCS_ALARM;

 
//extern IISCSAlarm *g_pISCSAlarm;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
unsigned int __stdcall threadFunc(void* pParam )
{
	CReceiveThread* pThis = static_cast<CReceiveThread*>(pParam);
	IISCSAlarm *g_pISCSAlarm=getISCSInterface();
	while(pThis)
	{
		std::cout << "---begin recv ATS Alar 1 \n";
		g_pISCSAlarm=getISCSInterface();
		std::cout << "---begin recv ATS Alar 2 \n";
		if(g_pISCSAlarm)
		{
			//retrieve ack
		 
			std::list<ATSAlarmQueueItem*> AckupdateList;
			
			g_pISCSAlarm->retrieveATSAlarm(AckupdateList);
			std::cout << "---get recv ATS Alar \n";
			Sleep(2000);
			for(std::list<ATSAlarmQueueItem*>::iterator itr = AckupdateList.begin();
			itr != AckupdateList.end(); 
			++itr)
			{

				std::cout << "---recv ATS Alarm:" 					 
					<< "\n";	
 				std::cout << "---recv ATS Alarm:" 
					<<" " <<(*itr)->detail.alarmID
					<<" " <<(*itr)->detail.ackTime
					<< "\n";	
			 
				 std::cout << "---delete recv ATS Alarm:" 					 
					 << "\n";	
				 Sleep(2000);
			}
			//AckupdateList.clear();

			

		}

		Sleep(500);
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

