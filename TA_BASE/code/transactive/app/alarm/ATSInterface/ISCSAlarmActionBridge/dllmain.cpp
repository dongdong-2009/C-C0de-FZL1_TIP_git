// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "DebugUtil.h"

using namespace std;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{	
			char modelName[200+1];
			char logName[200];
			DWORD dwSize = GetModuleFileName(NULL, modelName, 200);
			modelName[dwSize] = 0;
			std::string strAppName=modelName;
			strAppName = strAppName.substr(strAppName.rfind("\\") + 1);
			if(strAppName.length()>0)
			{
				sprintf_s(logName,"c:\\transActive\\logs\\ISCSAlarmActionBridge_%s.log",strAppName.c_str());			 			
			}
			else
			{
				sprintf_s(logName, "c:\\transActive\\logs\\ISCSAlarmActionBridge.log");
			}				   

			TA_Base_Core::DebugUtil::getInstance().setFile(logName);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, " --------------------start-----------------------------");	 
			// Set Log File name		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ISCSAlarmActionBridge - DLL_PROCESS_ATTACH] dwReason=%ld, lpReserved=%p", ul_reason_for_call, lpReserved); 

			break;
		}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ISCSAlarmActionBridge - DLL_PROCESS_DETACH] dwReason=%ld, lpReserved=%p", ul_reason_for_call, lpReserved);

		break;
	}
	return TRUE;
}

