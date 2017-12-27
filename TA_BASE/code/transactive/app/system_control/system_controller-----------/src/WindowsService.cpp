/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/WindowsService.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "WindowsService.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

WindowsService::WindowsService()
{
	m_monitorServiceStopEvent = false;
	
	m_SCManagerHandle=OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
}

WindowsService::~WindowsService()
{
	CloseServiceHandle(m_SCManagerHandle); 
}


/**
  * getInstance()
  * 
  * Return the one and only instance of this singleton class
  * 
  * @return WindowsService& - reference to the singleton instance
  */
WindowsService& WindowsService::getInstance()
{
    static WindowsService theService;
	
    return theService;  
}


/**
  * getMonitorServiceStatus()
  * 
  * Returns the service status structure that describes the service
  * 
  * @return SERVICE_STATUS* - pointer to a service status structure
  */
SERVICE_STATUS* WindowsService::getMonitorServiceStatus()
{
	return &m_myServiceStatus; 
}


/**
  * getMonitorServiceStatusHandle()
  * 
  * Returns the service status handle for the controller service
  * 
  * @return SERVICE_STATUS_HANDLE* - pointer to the service status handle
  */
SERVICE_STATUS_HANDLE* WindowsService::getMonitorServiceStatusHandle()
{
	return &m_myServiceStatusHandle;
}


/**
  * setMonitorServiceStopEvent()
  * 
  * Used to notify the service that the process should be stopping
  * 
  * @param bool stopStatus - true if the process is due to be stopping
  */
bool WindowsService::getMonitorServiceStopEvent()
{
	return m_monitorServiceStopEvent;
}


/**
  * getMonitorServiceStopEvent()
  * 
  * Returns true if the Service is due to be stopping
  * 
  * @return bool - true if the service is due to be stopping
  */
void WindowsService::setMonitorServiceStopEvent(bool stopStatus)
{
	 m_monitorServiceStopEvent = stopStatus;
}

								
/**
  * installMonitorService()
  * 
  * Installs this process as a WinNT service, it can then be started and stopped from the
  * Control Panel Services dialog.
  */							
VOID WindowsService::installMonitorService() 
{
    char fullPath[512];
	SC_HANDLE	schService;

	GetModuleFileName(NULL,fullPath,512);

	schService = CreateService( 
        m_SCManagerHandle,              // SCManager database 
        SERVICE_NAME,		       // name of service 
        SERVICE_DISPLAY_NAME,      // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_AUTO_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        fullPath,				   // service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password      

    // Set the service's description.
    SERVICE_DESCRIPTION sdBuf;
    sdBuf.lpDescription = SERVICE_DESC;
    if( !ChangeServiceConfig2(
        schService,                 // handle to service
        SERVICE_CONFIG_DESCRIPTION, // change: description
        &sdBuf) )                   // value: new description
    {
        serviceErrorOut("ChangeServiceConfig2 failed.");
    }

	if (schService == NULL)
    {
		if (GetLastError()==ERROR_SERVICE_EXISTS)
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                "Monitor service already installed. ");
		}
		else
        {
			serviceErrorOut("InstallService failed.");
        }
    }
	else 
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "InstallService successful.");
    }
	
	CloseServiceHandle(schService); 

    // The service has been installed, so now we can create the command line
    // list in the Registry.

}


/**
  * uninstallMonitorService()
  * 
  * Removes this process from the Control Panel services dialog
  */
VOID WindowsService::uninstallMonitorService() 
{     
	SC_HANDLE	schService;
	
	// Note: Used 0x00010000L instead of DELETE because windows.h defines it as such
	//       but object.h defines it as a different macro.  
	schService = OpenService(m_SCManagerHandle,			// SCManager database 
                                        SERVICE_NAME,			// name of service 
                                        0x00010000L);			// only need DELETE access  
	
    if (schService == NULL) 
    {
        serviceErrorOut("Error opening service for deleting.");
    }
	
    if (! DeleteService(schService) ) 
    {
        serviceErrorOut("Error deleting service.");
    }
    else      
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "UninstallService successful.");  
    }

    CloseServiceHandle(schService); 
}
 

/**
  * startMonitorService()
  * 
  * Provides the ability to start this application as a service from within the
  * application (as opposed to using the Control Panel).
  */
VOID WindowsService::startMonitorService() 
{     
	SERVICE_STATUS	ssStatus; 
	SC_HANDLE		schService;
    DWORD			dwOldCheckPoint;      

	schService = OpenService(m_SCManagerHandle,			// SCM database 
                                        SERVICE_NAME,		         // service name
                                        SERVICE_ALL_ACCESS);     // Full control access
    if (schService == NULL)
    {
        serviceErrorOut("OpenService error.");
    }
	
    if (!StartService(schService,		// handle to service 
                           0,					 // number of arguments 
                           NULL))              // commandline args
    {
        DWORD error = GetLastError();
        switch (error)
        {
            case ERROR_ACCESS_DENIED:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The user does not have the SERVICE_START access right.");
                break;
            case ERROR_INVALID_HANDLE:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The service handle is invalid.");
                break;
            case ERROR_PATH_NOT_FOUND:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The service binary file could not be found.");
                break;
            case ERROR_SERVICE_ALREADY_RUNNING:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "An instance of the service is already running.");
                break;
            case ERROR_SERVICE_DATABASE_LOCKED:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The service database is locked.");
                break;
            case ERROR_SERVICE_DEPENDENCY_DELETED:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The service depends on a service that does not exist or has been marked for deletion.");
                break;
            case ERROR_SERVICE_DEPENDENCY_FAIL:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The service depends on another service that has failed to start.");
                break;
            case ERROR_SERVICE_DISABLED:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The service has been disabled.");
                break;
            case ERROR_SERVICE_LOGON_FAILED:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The service could not be logged on. This error occurs if the service was started from an account that does not have the 'Log on as a service' right.");
                break;
            case ERROR_SERVICE_MARKED_FOR_DELETE:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The service has been marked for deletion.");
                break;
            case ERROR_SERVICE_NO_THREAD:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "A thread could not be created for the service.");
                break;
            case ERROR_SERVICE_REQUEST_TIMEOUT:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The process for the service was started, but it did not call StartServiceCtrlDispatcher, or the thread that called StartServiceCtrlDispatcher may be blocked in a control handler function.");
                break;
            default:
                LOG(SourceInfo,TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Unknown error code");
        }
        serviceErrorOut("Error starting service.");
    }
    else
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Service start pending");  
    }
  
	// Check the status until the service is no longer start pending.  
    if (!QueryServiceStatus(schService,			// handle to service 
			                        &ssStatus))         // address of status information
    {
		 serviceErrorOut("QueryServiceStatus error.");  
    }
    
	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)     
	{ 
        // Save the current checkpoint.  
        dwOldCheckPoint = ssStatus.dwCheckPoint;  
        
		// Wait for the specified interval.  
        Sleep(ssStatus.dwWaitHint);          
		
		// Check the status again.  
        if (!QueryServiceStatus(schService,		// handle to service 
                                        &ssStatus))		// address of status information
        {
            break;			
        }
		
		// Break if the checkpoint has not been incremented.  
        if (dwOldCheckPoint >= ssStatus.dwCheckPoint)
        {
            break;     
        }
	}  
   
    if (ssStatus.dwCurrentState == SERVICE_RUNNING) 
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "StartService successful.");     
    }
	else 
    { 
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "StartService failed in context: ");
	}  
    CloseServiceHandle(schService); 
}  


/**
  * stopMonitorService()
  * 
  * Provides the ability to stop this application as a service from within the
  * application (as opposed to using the Control Panel).
  */
VOID WindowsService::stopMonitorService()
{   
	
	SC_HANDLE		schService;
    
	DWORD fdwControl=SERVICE_CONTROL_STOP;

	// Open a handle to the service.      
	schService = OpenService(m_SCManagerHandle,			// SCManager database 
                                        SERVICE_NAME,			// name of service 
                                        SERVICE_STOP);			// specify access     
	
	if (schService == NULL) 
    {
        serviceErrorOut("OpenService for stopping error.");        
    }

	m_myServiceStatus.dwWin32ExitCode = 0; 
    m_myServiceStatus.dwCurrentState  = SERVICE_STOP_PENDING; 
    m_myServiceStatus.dwCheckPoint    = 0; 
    m_myServiceStatus.dwWaitHint      = 0;
	
	setMonitorServiceStopEvent(TRUE);
 
    if (!SetServiceStatus (m_myServiceStatusHandle, 
		                          &m_myServiceStatus))
    {
		serviceErrorOut(" SetServiceStatus error.");
    }
	
	CloseServiceHandle(schService); 	
	return; 
}


/**
  * serviceErrorOut(char *message)
  *
  * Provides a formatted error message to accompany the message.  It is expected
  * that GetLastError() will return the most recent error number.
  * 
  * @param char* message - string containing context information.
  */
VOID WindowsService::serviceErrorOut(char* message)
{
	LPTSTR errMesg;									

    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, message);									
	FormatMessage(									
		FORMAT_MESSAGE_ALLOCATE_BUFFER |			
		FORMAT_MESSAGE_FROM_SYSTEM,					
		NULL,    									
		GetLastError(),    							
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 	
		(LPTSTR) &errMesg,							
		0,											
		NULL);											

	LocalFree(errMesg);								
	exit(-1);
}


/**
  * sendControl(DWORD fdwControl) 
  *
  * Sends a service event signal to the running instance of the service.  The
  * input parameter is one of the following:
  *
  *		SERVICE_CONTROL_STOP
  *		SERVICE_CONTROL_PAUSE
  *		SERVICE_CONTROL_CONTINUE
  *		SERVICE_CONTROL_INTERROGATE
  *
  * @param DWORD fdwControl - one of the above messages
  */
VOID WindowsService::sendControl(DWORD fdwControl) 
{ 
    SC_HANDLE		schService;
	SERVICE_STATUS	ssStatus; 
    DWORD			fdwAccess;  

    // The required service object access depends on the control.  
    switch (fdwControl)     
	{         
		case SERVICE_CONTROL_STOP: 
            fdwAccess = SERVICE_STOP;             
			break;  
        case SERVICE_CONTROL_PAUSE:         
		case SERVICE_CONTROL_CONTINUE: 
            fdwAccess = SERVICE_PAUSE_CONTINUE;             
			break;  
        case SERVICE_CONTROL_INTERROGATE: 
            fdwAccess = SERVICE_INTERROGATE;             
			break;  
        default:             
			fdwAccess = SERVICE_INTERROGATE;     
	}  
    
	// Open a handle to the service.      
	schService = OpenService(m_SCManagerHandle,        // SCManager database 
		                                SERVICE_NAME,		 // name of service 
		                                fdwAccess);		// specify access     
		         
	if (schService == NULL)
    {
        serviceErrorOut("OpenService");  
    }
    
	// Send a control value to the service.      
	if (! ControlService(schService,   // handle of service 
                              fdwControl,   // control value to send 
                              &ssStatus)) 	  // address of status info     
    {
		serviceErrorOut("Error sending control to service");
    }
	
	CloseServiceHandle(schService); 	
	return; 
}


/**
  * writeCommandLineIntoRegistry()
  *
  * This method will write the command line args taken after the --install-service option
  * into the registry in the location
  *
  * HKEY_LOCAL_MACHINE\\SYSTEM\CurrentControlSet\Services\SERVICE_NAME\CommandLine
  *
  * All parameters are taken from RunParams, hence no arguments
  */
void WindowsService::writeCommandLineIntoRegistry(int argc, char** argv)
{
    HKEY hKey;
    
    // Create the location of the Registry Key
    std::string key("SYSTEM\\CurrentControlSet\\Services\\");
    key += SERVICE_NAME;

    //Open the registry key
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    TEXT(key.c_str()),
                    0,
                    KEY_ALL_ACCESS,
                    &hKey) != ERROR_SUCCESS) 
    {
        serviceErrorOut("Could not open");
    }

    // Create the command line that is to written into the registry,
    // Loop through the list of arguments and generate a commanline
    std::string commandLine;
    for (int i = 1; i < argc; i++)
    {
        std::string argument(argv[i]);
        if (argument != "--install-service" && argument != "--start-service"
            && argument != "--stop-service" && argument != "--uninstall-service")
        {
            commandLine += argument;
            commandLine += " ";
        }
    }
 
    DWORD len = commandLine.length();

    // Write the value into the Registry under the CommandLine value
    if(RegSetValueEx(hKey,
                    TEXT(KEY_NAME),
                    0,
                    REG_SZ,
                    (LPBYTE)commandLine.c_str(),
                    len) != ERROR_SUCCESS)
    {
        serviceErrorOut("Could not write to registry");
    }
}


/**
  * readCommandLineFromRegistry()
  * 
  * This method will read the commandLine from the registry under the value
  *
  * HKEY_LOCAL_MACHINE\\SYSTEM\CurrentControlSet\Services\SERVICE_NAME\CommandLine
  *
  * @return const char* - the commandline taken from the registry
  */
std::string WindowsService::readCommandLineFromRegistry()
{
    HKEY hKey;
    
    // Create the location of the Registry Key
    std::string key("SYSTEM\\CurrentControlSet\\Services\\");
    key += SERVICE_NAME;

    //Open the registry key
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    TEXT(key.c_str()),
                    0,
                    KEY_ALL_ACCESS,
                    &hKey) != ERROR_SUCCESS) 
    {
        serviceErrorOut("Could not open the registry");
        return NULL;
    }

    char buff[256];
    DWORD buffLength = 256;

    if (RegQueryValueEx(hKey,
                        TEXT(KEY_NAME),
                        NULL,
                        NULL,
                        (LPBYTE)buff,
                        &buffLength) != ERROR_SUCCESS)
    {
        serviceErrorOut("Could not read from the registry");
        return NULL;
    }
    std::string out(buff);
    return out.c_str();
}
																				
///////////////////////////////////////////////////////////////////////////////
// WindowsService Class Usage
//
// This class contains most of the tools necessary for converting an 
// application into a WinNT service.  The code can be easily reused for other 
// applications if required.  
//
// The following is a rough guide detailing the usage of the ClMonitorClass.  
// This class is not standalone as it requires some certain functions to be
// created in the main.cpp file.  To create a service:
// 
//	1.	Modify SERVICE_NAME and SERVICE_DISPLAY_NAME constants, methods and 
//		variable names to suit the application.
//
//	2.	If it is desired to contain all functionality in a single application, it
//		is suggested that the command line interface should be:
//
//			<application> -install | -uninstall | -start | -stop
//
//		When no parameter is supplied it is assumed that the service is being
//		started externally.
//
//		The application can be split so that one application installs the other
//		which is purely a service.
//
//	3.	To install the service call installMonitorService() method.  This updates
//		the registry entries to hold the details of this application so that the 
//		Control Panel Service Manager recognises its existence.  
// 
//	4.	Start the service by calling the startMonitorService() method, using
//		the Control Panel Services or the "net start" command in dos.  Using the
//		last two options, it is possible to pass command-line parameters to the
//		service from main(argc,argv) to service_main(argc,argv).  
//
//	5.	Ensure that the service application can link the Service Manager to the
//		service_main function to pass execution from main() to service_main() by
//		using a SERVICE_TABLE_ENTRY (see main.cpp)
//		
//	6.	service_main(argc,argv) should be created to initialise the service, 
//		run the application and cleanup. (see main.cpp)
//
//		Involves: 
//
//		*	Registering the monitorServiceCtrlHandler() function with the 
//			RegisterServiceCtrlHandler() function.
//
//		*	Setting the current state of the service to SERVICE_RUNNING using
//			SetServiceStatus.
//		
//		*	Running the bulk of the execution.
//
//		*	Cleaning up by setting the current state to SERVICE_STOPPED.		
//		
//	7.	The monitorServiceCtrlHandler() should contain code to react to 
//		permitted events such as SERVICE_STOP.  This event should be processed 
//		to shutdown the application gracefully and then call 
//		stopMonitorService().  (see main.cpp)
//		
//	8.	Command line parameters should be linked to the methods 
//
//		*	installMonitorService()
//		*	uninstallMonitorService()
//		*	startMonitorService()
//		*	stopMonitorService()
//
//	9.	Additional notes:
//			
//		*	The service is run from the winnt\system32 directory and not from 
//			the executable, yet the service image is taken from its original
//			location.
//
//		*	When using the application to start or stop itself using command-
//			line parameters, it does so indirectly through the Service Manager.
//			Once its request has been processed, it exits and an another 
//			identical image starts/stops.  This way, parameters or variables
//			cannot be passed between the two instances.
//
///////////////////////////////////////////////////////////////////////////////

