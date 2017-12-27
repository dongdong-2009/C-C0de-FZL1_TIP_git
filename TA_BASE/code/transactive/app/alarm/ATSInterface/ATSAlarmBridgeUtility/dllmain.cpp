// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ATSAlarmBridgeUtility_i.h"
#include "dllmain.h"
#include "DebugUtil.h"

CATSAlarmBridgeUtilityModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			// initialize logger //
			char modelName[200+1];
			char logName[200];
			DWORD dwSize = GetModuleFileName(NULL, modelName, 200);
			modelName[dwSize] = 0;
			std::string strAppName=modelName;
			strAppName = strAppName.substr(strAppName.rfind("\\") + 1);
			if(strAppName.length()>0)
			{
				sprintf(logName,"c:\\transActive\\logs\\ATSAlarmBridgeUtility_%s.log",strAppName.c_str());			 			
			}
			else
			{
				sprintf(logName, "c:\\transActive\\logs\\ATSAlarmBridgeUtility_%p.log", _AtlModule);
			}				

			// Set Log File name
			TA_Base_Core::DebugUtil::getInstance().setFile(logName);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, " --------------------============= start =============-------------------------");	 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ATSAlarmBridgeUtility - DLL_PROCESS_ATTACH] sdwReason= %ld , lpReserved= %p", dwReason ,lpReserved); 
		}
		break;
	case DLL_PROCESS_DETACH:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[ATSAlarmBridgeUtility - DLL_PROCESS_DETACH] sdwReason= %ld , lpReserved= %p", dwReason ,lpReserved); 

		break;
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
