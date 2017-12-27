// ShareMemoryDLL .cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
 
#include "ShareMemoryDLL.h"
#include "core/utilities/src/DebugUtil.h"
 
#include "ISCSAlarmQueueItem.h"
#include "AckQueueItem.h"
#include "MmsQueueItem.h"
#include "AlarmProvider.h"
#include "ISCSAlarm.h"
#include <vector>

using namespace std;

#include "ATSAlarmInfoManager.h"
 

// const char *ISCS_QUEUE_ID = "iscs queue id";
// const char *ATS_MMS_QUEUE_ID = "ats mms queue id";
// const char *ATS_ACK_QUEUE_ID = "ats ack queue id";

// #define FIND_QUEUE_RETURN(QUEUE, ALLOCATOR, QUEUE_TYPE, QUEUE_ID) \
// 	if (NULL != ALLOCATOR) { \
// 		if (0 != ALLOCATOR->find(QUEUE_ID, QUEUE)) \
// 			QUEUE = QueryFactory<QUEUE_TYPE>::CreateQueue(pAlloc, QUEUE_ID);} 

typedef std::map<unsigned long, IAlarmProvider* > T_AlarmProviders;
T_AlarmProviders g_alarmProviders;
IISCSAlarm *g_pISCSAlarm = NULL;
 

// ISCS_ALARM_QUEUE* g_pIscsAlarmQueue = NULL;
// ATS_MMS_QUEUE* g_pAtsMmsQueue = NULL;
// ATS_ACK_QUEUE* g_pAtsAckQueue = NULL;

// ISCS_ALARM_QUEUE* CShareMemoryDLL::m_pIscsAlarmQueue = NULL;
// ATS_MMS_QUEUE* CShareMemoryDLL::m_pAtsMmsQueue = NULL;
// ATS_ACK_QUEUE* CShareMemoryDLL::m_pAtsAckQueue = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			 //
			{
 
		 
				char modelName[200+1];
				char logName[200];
				DWORD dwSize = GetModuleFileName(NULL, modelName, 200);
				modelName[dwSize] = 0;
				std::string strAppName=modelName;
				strAppName = strAppName.substr(strAppName.rfind("\\") + 1);
				if(strAppName.length()>0)
				{
					sprintf(logName,"c:\\transActive\\logs\\ShareMemory_%s.log",strAppName.c_str());			 			
				}
				else
				{
					sprintf(logName, "c:\\transActive\\logs\\ShareMemory_%p.log", hModule);
				}									
				//TA_Base_Core::DebugUtil::getInstance().setFile("c:\\SharedAlarm.log");
				TA_Base_Core::DebugUtil::getInstance().setFile(logName);
				TA_Base_Core::DebugUtil::getInstance().setMaxSize(2000000);

				TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugTrace);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ShareMemory - DLL_PROCESS_ATTACH] hModule=%p,ul_reason_for_call=%d, lpReserved=%p",
					hModule, ul_reason_for_call, lpReserved);

			  break;
			}
			
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ShareMemory - DLL_PROCESS_DETACH] hModule=%p,ul_reason_for_call=%d, lpReserved=%p",
				hModule, ul_reason_for_call, lpReserved);
			break;
    }
    return TRUE;
}

// This is the constructor of a class that has been exported.
// see ShareMemoryDLL.h for the class definition
SHAREMEMORYDLL_API I_ALARM_PROVIDER::IAlarmProvider* getAlarmProvider(unsigned long ulAlarmProviderID)
{
	I_ALARM_PROVIDER::IAlarmProvider* pRetVal=NULL;

	T_AlarmProviders::iterator itFind = g_alarmProviders.find(ulAlarmProviderID);
	if(itFind == g_alarmProviders.end())
	{
		pRetVal = new CAlarmProvider(ulAlarmProviderID);
		g_alarmProviders[ulAlarmProviderID] = pRetVal;
	}

	return pRetVal;
}

SHAREMEMORYDLL_API void freeAlarmProvider(unsigned long ulAlarmProviderID)
{
	T_AlarmProviders::iterator itFind = g_alarmProviders.find(ulAlarmProviderID);
	if(itFind != g_alarmProviders.end())
	{
		delete itFind->second;
		itFind->second = NULL;

		g_alarmProviders.erase(itFind);
	}
}

SHAREMEMORYDLL_API IISCSAlarm*  getISCSInterface()
{
	if(!g_pISCSAlarm)
	{
		g_pISCSAlarm = new CISCSAlarm();
	}

	return g_pISCSAlarm;
}

//std::string CShareMemoryDLL::m_appName("");
CShareMemoryDLL::CShareMemoryDLL() 
{
	// init();
}

CShareMemoryDLL::~CShareMemoryDLL()
{
	// fini();	
}

void CShareMemoryDLL::init()
{
	/*
	HANDLE ShareMem = CreateMutex(NULL, FALSE, SHARE_MEM_MUTEX);
	WaitForSingleObject(ShareMem, INFINITE);
	
	TA_Base_Core::ALLOCATOR *pAlloc = TA_Base_Core::Shmem::getAllocator();

	FIND_QUEUE_RETURN( ((void *&)g_pIscsAlarmQueue), pAlloc, ISCSAlarmQueueItem, ISCS_QUEUE_ID);
 	FIND_QUEUE_RETURN( ((void *&)g_pAtsMmsQueue), pAlloc, ATSMmsQueueItem, ATS_MMS_QUEUE_ID);
 	FIND_QUEUE_RETURN( ((void *&)g_pAtsAckQueue), pAlloc, ATSAckQueueItem, ATS_ACK_QUEUE_ID);
	g_pIscsAlarmQueue->setACE_Allocator(pAlloc);
	g_pAtsMmsQueue->setACE_Allocator(pAlloc);
	g_pAtsAckQueue->setACE_Allocator(pAlloc);


 //	void *pBuf = TA_Base_Core::Shmem::alloc(sizeof(CATSAlarm));
 //	g_pIATSAlarm = new (pBuf) CATSAlarm(*m_pIscsAlarmQueue, *m_pAtsMmsQueue, *m_pAtsAckQueue);

 //	pBuf = TA_Base_Core::Shmem::alloc(sizeof(CISCSAlarm));
 //	g_pISCSAlarm = new (pBuf) CISCSAlarm(*m_pIscsAlarmQueue, *m_pAtsMmsQueue, *m_pAtsAckQueue);

	ReleaseMutex(ShareMem);
	CloseHandle(ShareMem);
	*/
}
// void CShareMemoryDLL::initNew()
// {
// 	/*
// 	HANDLE ShareMem = CreateMutex(NULL, FALSE, SHARE_MEM_MUTEX);	
// 	WaitForSingleObject(ShareMem, INFINITE);
// 
// 	TA_Base_Core::ALLOCATOR *pAlloc = TA_Base_Core::Shmem::getAllocator();
// 
// 	FIND_QUEUE_RETURN( ((void *&)g_pIscsAlarmQueue), pAlloc, ISCSAlarmQueueItem, ISCS_QUEUE_ID);
//  	FIND_QUEUE_RETURN( ((void *&)g_pAtsMmsQueue), pAlloc, ATSMmsQueueItem, ATS_MMS_QUEUE_ID);
//  	FIND_QUEUE_RETURN( ((void *&)g_pAtsAckQueue), pAlloc, ATSAckQueueItem, ATS_ACK_QUEUE_ID);
// 
// 
// // 	g_pIscsAlarmQueue->setACE_Allocator(pAlloc);
// // 	g_pAtsMmsQueue->setACE_Allocator(pAlloc);
// // 	g_pAtsAckQueue->setACE_Allocator(pAlloc);
// 
//  
// 
// 	ReleaseMutex(ShareMem);
// 	CloseHandle(ShareMem);
// 	*/
// }

// void CShareMemoryDLL::setAppName(std::string appName)
// {
// 	m_appName=appName.c_str();
// }
// 
// std::string CShareMemoryDLL::getAppName( )
// {
// 	return m_appName.c_str();
// }
void CShareMemoryDLL::fini()
{
	/*
	HANDLE ShareMem = CreateMutex(NULL, FALSE, SHARE_MEM_MUTEX);
	WaitForSingleObject(ShareMem, INFINITE);

	TA_Base_Core::Shmem::dealloc(g_pIATSAlarm);
	TA_Base_Core::Shmem::dealloc(g_pISCSAlarm);
	TA_Base_Core::Shmem::dealloc(m_pIscsAlarmQueue);
	TA_Base_Core::Shmem::dealloc(m_pAtsMmsQueue);
	TA_Base_Core::Shmem::dealloc(m_pAtsAckQueue);

	ReleaseMutex(ShareMem);
	CloseHandle(ShareMem);
	*/
}
