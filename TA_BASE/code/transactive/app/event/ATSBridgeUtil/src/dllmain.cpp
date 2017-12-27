// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ATSBridgeUtil_i.h"
#include "dllmain.h"
#include <xstring>

// include log4cxx header files.
#ifdef ENABLE_LOG4CXX
	#include "log4cxx/logger.h"
	#include "log4cxx/xml/domconfigurator.h"

	using namespace log4cxx;  
	using namespace log4cxx::xml;  
	using namespace log4cxx::helpers;  
#else
	#include "logger.h"  
	using namespace log4cxx;  
#endif

CATSBridgeUtilModule _AtlModule;

LoggerPtr logDLLMain(Logger::getLogger( "DllMain"));

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
				sprintf(logName,"c:\\transActive\\logs\\ATSBridgeUtil_%s.log",strAppName.c_str());			 			
			}
			else
			{
				sprintf(logName, "c:\\transActive\\logs\\ATSBridgeUtil_%p.log", _AtlModule);
			}				
			
			// Load configuration file  
#ifdef ENABLE_LOG4CXX			
			DOMConfigurator::configure("C:\\Transactive\\config\\ATSBridgeUtil\\LOG4cxxConfig.xml");  
#endif
			// Set Log File name		
			LOG4CXX_INFO (logDLLMain, "[ATSBridgeUtil - DLL_PROCESS_ATTACH] sdwReason= " << dwReason << ", lpReserved= " << lpReserved); 
		}
		break;
	case DLL_PROCESS_DETACH:
			LOG4CXX_INFO (logDLLMain, "[ATSBridgeUtil - DLL_PROCESS_DETACH] sdwReason= " << dwReason << ", lpReserved= " << lpReserved); 

		break;
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
