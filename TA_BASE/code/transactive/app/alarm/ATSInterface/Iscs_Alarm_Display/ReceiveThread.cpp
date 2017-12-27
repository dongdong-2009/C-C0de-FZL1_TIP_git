// ReceiveThread.cpp: implementation of the CReceiveThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReceiveThread.h"
#include "./AlarmAPI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
unsigned int __stdcall threadFunc(void* pParam )
{
	CReceiveThread* pThis = static_cast<CReceiveThread*>(pParam);
	
	while(pThis)
	{
		CMD_TO_ATS recvData;
		if(g_recvQueue.pop_front(recvData) == 0)
		{
			std::cout << "---recv:" << recvData.content.ID << recvData.content.alarmMsg << "\n";	
		}

		Sleep(100);
	}
	
    return 0;
} 

CReceiveThread::CReceiveThread()
{
	unsigned long hHandle = _beginthreadex
		(NULL,
		0,
		threadFunc, 
		(void*)this,
		0, 
		NULL);
	
	if(0 == hHandle)
	{
		std::cout << "create thread error! \n";		
	}

}

CReceiveThread::~CReceiveThread()
{

}

