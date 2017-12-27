/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/WindowsService.h $
  * @author:  Sean Liew & Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * WindowsService is a class that is used to create and
  * manage the Windows Service side of the System Controller.
  * 
  */

#ifndef WINDOWSSERVICE_H
#define WINDOWSSERVICE_H

#include <windows.h>
#include <string>

#define SERVICE_NAME			"SystemController"
#define SERVICE_DISPLAY_NAME	"System Controller"
#define SERVICE_DESC            "Starts, stops, and manages Transactive processes."
#define KEY_NAME                "CommandLine"

class WindowsService
{
public:
	
    /**
      * getInstance()
      * 
      * Return the one and only instance of this singleton class
      * 
      * @return WindowsService& - reference to the singleton instance
      */
    static WindowsService& getInstance();

	~WindowsService();

    /**
      * installMonitorService()
      * 
      * Installs this process as a WinNT service, it can then be started and stopped from the
      * Control Panel Services dialog
      */
	void installMonitorService();			

    /**
      * uninstallMonitorService()
      * 
      * Removes this process from the Control Panel services dialog
      */
	void uninstallMonitorService();			

    /**
      * startMonitorService()
      * 
      * Provides the ability to start this application as a service from within the
      * application (as opposed to using the Control Panel).
      */
	void startMonitorService(); 			

    /**
      * stopMonitorService()
      * 
      * Provides the ability to stop this application as a service from within the
      * application (as opposed to using the Control Panel).
      */
	void stopMonitorService();				

    /**
      * serviceErrorOut(char *message)
      *
      * Provides a formatted error message to accompany the message.  It is expected
      * that GetLastError() will return the most recent error number.
      * 
      * @param char* message - string containing context information.
      */
	void serviceErrorOut(char* message);

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
	void sendControl(DWORD fdwControl);

    /**
      * setMonitorServiceStopEvent()
      * 
      * Used to notify the service that the process should be stopping
      * 
      * @param bool stopStatus - true if the process is due to be stopping
      */
	void setMonitorServiceStopEvent(bool stopStatus);

    /**
      * getMonitorServiceStopEvent()
      * 
      * Returns true if the Service is due to be stopping
      * 
      * @return bool - true if the service is due to be stopping
      */
	bool getMonitorServiceStopEvent();	

    /**
      * getMonitorServiceStatus()
      * 
      * Returns the service status structure that describes the service
      * 
      * @return SERVICE_STATUS* - pointer to a service status structure
      */
	SERVICE_STATUS*			getMonitorServiceStatus();

    /**
      * getMonitorServiceStatusHandle()
      * 
      * Returns the service status handle for the controller service
      * 
      * @return SERVICE_STATUS_HANDLE* - pointer to the service status handle
      */
	SERVICE_STATUS_HANDLE*	getMonitorServiceStatusHandle();

    /**
      * writeCommandLineIntoRegistry()
      *
      * This method will write the command line args taken after the --install-service option
      * into the registry in the location
      *
      * HKEY_LOCAL_MACHINE\\SYSTEM\CurrentControlSet\Services\SERVICE_NAME\CommandLine
      *
      * @param int argc - the number of arguments
      * @param char** argv - the commandline arguments
      */
    void writeCommandLineIntoRegistry(int argc, char** argv);

    /**
      * readCommandLineFromRegistry()
      * 
      * This method will read the commandLine from the registry under the value
      *
      * HKEY_LOCAL_MACHINE\\SYSTEM\CurrentControlSet\Services\SERVICE_NAME\CommandLine
      *
      * @return const char* - the commandline taken from the registry
      */
    std::string readCommandLineFromRegistry();

private:

	// Class constructor	
	WindowsService();

    // Declare the copy constructor private so the object can't be cloned
    WindowsService(const WindowsService& pms);
    
    // Declare the operator= function so the compiler doesn't create a default
    // implementation.
    WindowsService& operator=(const WindowsService& pms);

	// Handle to the Service Control Manager
	SC_HANDLE					m_SCManagerHandle;

	// Service status struct
	SERVICE_STATUS				m_myServiceStatus; 

	// Handle to the service status
	SERVICE_STATUS_HANDLE		m_myServiceStatusHandle;
	
	// Variable holding the stop event status
	bool m_monitorServiceStopEvent;

};

#endif //!defined

