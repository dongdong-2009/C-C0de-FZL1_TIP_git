/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_store_service/src/AlarmStoreService.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// AlarmStoreService.cpp : Defines the entry point for the console application.
//

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif // defined _MSC_VER

#include "bus/generic_service/src/ServiceMain.h"
#include "AlarmStoreAgentUser.h"
#include <tlhelp32.h>
#include <windows.h>
template<> TA_Base_Bus::ServiceMain<TA_Base_App::AlarmStoreAgentUser>* TA_Base_Bus::ServiceMain<TA_Base_App::AlarmStoreAgentUser>::m_singleton = NULL;
template<> TA_Base_Core::ReEntrantThreadLockable TA_Base_Bus::ServiceMain<TA_Base_App::AlarmStoreAgentUser>::m_singletonLock;
int GetThreadCnt( DWORD id)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		return( 0 );
	}
	
	// Set the size of the structure before using it.
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	
	// Retrieve information about the first process,
	// and exit if unsuccessful
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		CloseHandle( hProcessSnap );     // Must clean up the snapshot object!
		return( 0 );
	}
	
	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		if(id == pe32.th32ProcessID)
		{
			CloseHandle( hProcessSnap );
			return pe32.cntThreads;
		}
		
		// List the modules and threads associated with this process
	} while( Process32Next( hProcessSnap, &pe32 ) );
	
	CloseHandle( hProcessSnap );
	return( 0 );
}

int die( int reportType, char* message, int* returnValue)
{
	ExitProcess(1);
	return 1;
}
DWORD WINAPI Demon(LPVOID id)
{
	DWORD proc_id = *(reinterpret_cast<DWORD *>(id));
	while(1)
	{
		if(GetThreadCnt(proc_id) > 500)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MAX THREADS REACHED");
			ExitProcess(1);
		}
		Sleep(600000);
	}
	return 1;
}
int main(int argc, char* argv[])
{
	//_CrtSetReportHook(die);
	//DWORD proc_id = GetCurrentProcessId();
	//CreateThread(NULL, NULL, Demon, &proc_id,NULL,NULL);
	//DeleteFile("C:\\Shared.map");
	//ACE::init();
	//
	TA_Base_Core::DebugUtil::getInstance().setFile("c:\\AlarmStoreService_main.log");

	LOGBEFORECOTS("ACE::init" );
	ACE::init();
	LOGAFTERCOTS("ACE::init");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "=============== main function entry ===============");
	TA_Base_Bus::ServiceMain<TA_Base_App::AlarmStoreAgentUser>::getInstance().initialise(std::string("Alarm Store Service"),
        TA_Base_Core::IAlarmStoreCorbaDef::DEFAULT_ALARM_STORE_PORT);
	TA_Base_Bus::ServiceMain<TA_Base_App::AlarmStoreAgentUser>::getInstance().run(argc, argv);
	TA_Base_Bus::ServiceMain<TA_Base_App::AlarmStoreAgentUser>::removeInstance();

	return 0;
}
	
