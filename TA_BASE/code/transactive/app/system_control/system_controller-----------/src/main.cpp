/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif

#include "ace/OS.h"

#ifdef WIN32
    #include "WindowsService.h"
#endif

#include "SystemController.h"
#include "SignalEventHandler.h"
#include "ProcessManager.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include <string>

using TA_Base_Core::DebugUtil;

TA_Base_App::SystemController* systemController;

#ifdef WIN32

/**
  * windowsServiceCtrlHandler()
  *
  * This the external entry point for Windows Service commands, such 
  * as START, STOP, etc
  */
VOID windowsServiceCtrlHandler (DWORD Opcode) 
{ 
   	SERVICE_STATUS* windowsServiceStatus=
		WindowsService::getInstance().getMonitorServiceStatus();

	SERVICE_STATUS_HANDLE* windowsServiceStatusHandle=
		WindowsService::getInstance().getMonitorServiceStatusHandle();

    switch(Opcode) 
    { 
        case SERVICE_CONTROL_PAUSE: 
            // Do whatever it takes to pause here. 
            windowsServiceStatus->dwCurrentState = SERVICE_PAUSED; 
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
            // Do whatever it takes to continue here. 
            windowsServiceStatus->dwCurrentState = SERVICE_RUNNING; 
            break; 
 
        case SERVICE_CONTROL_STOP: 
            // Do whatever it takes to stop here. 
            systemController->shutdown();
            return; 
    } 
 
    // Send current status. 
    if (!SetServiceStatus (*windowsServiceStatusHandle,  windowsServiceStatus)) 
		WindowsService::getInstance().serviceErrorOut(" SetServiceStatus error."); 
    return; 
}


/**
  * service_main()
  * 
  * The console entry point, this is where any command line args given to the SCM
  * are passed into the application, hence the call to ParseCmdLine
  * Once all the service startup stuff has been done the call to monitorRun() will 
  * be made to start off the system
  */
void service_main (DWORD argc, char  **argv) 
{
    FUNCTION_ENTRY("service_main");

    std::string commandLine(WindowsService::getInstance().readCommandLineFromRegistry());
    
    // Parse the command line to get the entity name and the dbConnection string
    if (!TA_Base_Core::RunParams::getInstance().parseCmdLine(commandLine.c_str()))
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
            "Could not parse the command line.");
        return;
    }

	SERVICE_STATUS* windowsServiceStatus =
		WindowsService::getInstance().getMonitorServiceStatus();
	SERVICE_STATUS_HANDLE* windowsServiceStatusHandle = 
		WindowsService::getInstance().getMonitorServiceStatusHandle();
	
	windowsServiceStatus->dwServiceType				= SERVICE_WIN32; 
	windowsServiceStatus->dwCurrentState			= SERVICE_START_PENDING; 
	windowsServiceStatus->dwControlsAccepted		= SERVICE_ACCEPT_STOP;
	windowsServiceStatus->dwWin32ExitCode			= 0; 
	windowsServiceStatus->dwServiceSpecificExitCode	= 0; 
	windowsServiceStatus->dwCheckPoint				= 0; 
	windowsServiceStatus->dwWaitHint				= 0;  

	// Install handler
    *windowsServiceStatusHandle = RegisterServiceCtrlHandler( 
        SERVICE_NAME,         (LPHANDLER_FUNCTION ) windowsServiceCtrlHandler);  

    if (*windowsServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
        WindowsService::getInstance().serviceErrorOut("RegisterServiceCtrlHandler failed.");
	
	// Initialisation complete - report running status. 
    windowsServiceStatus->dwCurrentState       = SERVICE_RUNNING; 
    windowsServiceStatus->dwCheckPoint         = 0; 
    windowsServiceStatus->dwWaitHint           = 0;  
    if (!SetServiceStatus (*windowsServiceStatusHandle, windowsServiceStatus))
		WindowsService::getInstance().serviceErrorOut(" SetServiceStatus error.");

	////////////////////////////////////////////////////////////////////////////
	// This is where the service does its work.
	////////////////////////////////////////////////////////////////////////////
	
    systemController = new TA_Base_App::SystemController();
    TA_Base_Core::CorbaUtil::getInstance().run();
    
	// The end - cleanup
	windowsServiceStatus->dwCurrentState       = SERVICE_STOPPED; 
    windowsServiceStatus->dwCheckPoint         = 0; 
    windowsServiceStatus->dwWaitHint           = 0;  
	if (!SetServiceStatus (*windowsServiceStatusHandle, windowsServiceStatus))     
		WindowsService::getInstance().serviceErrorOut(" SetServiceStatus error.");

    FUNCTION_EXIT;
}

#endif //WIN32

/**
  * main()
  *
  * Entry point for the console application and the Windows service application.
  *
  * WIN32:
  *     The console app, if called with args, is used to setup the windows service
  *         then exit, or run with the standard command line args
  *     The console app, if called without args (ie: by the Service Control manager)
  *         will register with the SCM and service_main is the service entry point.
  */
int main(int argc, char* argv[])
{

    // Parse the command line to get the entity name and the dbConnection string
    if (!TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv))
    {
        if (!TA_Base_Core::RunParams::getInstance().isSet(RPARAM_VERSION))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, 
                "Command line arguments in invalid format.");
            return 1;
        }
        else
        {
            return 0;
        }
    }


#ifdef WIN32
    // Give the System Controller a default debug file and debug level.
    TA_Base_Core::DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);

    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGFILE))
    {
        TA_Base_Core::DebugUtil::getInstance().setFile(TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE).c_str());
    }
    else
    {
        TA_Base_Core::DebugUtil::getInstance().setFile("C:\\SystemController.log");
    }
#endif
    


#ifdef WIN32

    // Create the Windows NT service.
    // If there are no command line args, start the service straight away.

    // Create the dispatch table, which will give the location of the service_main 
    // function.
	SERVICE_TABLE_ENTRY dispatchTable[] = 
    {
        { SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) service_main },
        { NULL, NULL }
    };

    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "argc = %d", argc);
    // This has been started by the SCM!
    if (argc==1)
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "SCM started service");

        // This must be called as soon as the service is started!
		if (!StartServiceCtrlDispatcher(dispatchTable))
		{
            DWORD error = GetLastError(); 
            switch (error)
            {
            case ERROR_INVALID_DATA:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The specified dispatch table contains entries that are not in the proper format.");
                break;
            case ERROR_SERVICE_ALREADY_RUNNING:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The process has already called StartServiceCtrlDispatcher. Each process can call StartServiceCtrlDispatcher only one time.");
                break;
            }

   			WindowsService::getInstance().serviceErrorOut("StartServiceCtrlDispatcher failed.");	
		} 
		return 0;
	}

    // Install the service!
    if (TA_Base_Core::RunParams::getInstance().isSet("InstallService"))
    {
        WindowsService::getInstance().installMonitorService();
        WindowsService::getInstance().writeCommandLineIntoRegistry(argc, argv);
		exit(0);
    }
    // Uninstall the service
    else if (TA_Base_Core::RunParams::getInstance().isSet("UninstallService"))
    {
        WindowsService::getInstance().uninstallMonitorService();
		exit(0);
    }
    // Start the service
    else if (TA_Base_Core::RunParams::getInstance().isSet("StartService"))
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Starting service");
        WindowsService::getInstance().startMonitorService();
        exit(0);
    }
    // Stop the service
    else if (TA_Base_Core::RunParams::getInstance().isSet("StopService"))
    {
        SERVICE_STATUS* status=WindowsService::getInstance().getMonitorServiceStatus();
			
		WindowsService::getInstance().sendControl(SERVICE_CONTROL_STOP);
						
	    exit(0);
    }

#endif //WIN32
 
    // Start the System Controller.
	systemController = new TA_Base_App::SystemController();
    
	// Wait until we're meant to shut it down.
    systemController->waitUntilShutdown();

	// Shut it down.
	systemController->shutdown();

    TA_Base_Core::CorbaUtil::getInstance().shutdown();

    return 0;
}
