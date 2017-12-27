/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This holds all information about a running application. The Control Station
  * will maintain a collection of these so it knows exactly what is running on
  * a workstation.
  */

#include "app\system_control\control_station\src\RunningApplication.h"
#include "app\system_control\control_station\src\ApplicationMover.h"
#include "app\system_control\control_station\src\IProcessManager.h"

#include <sys/stat.h>
#include <map>

#include "core\synchronisation\src\ThreadGuard.h"
#include "core\data_access_interface\src\IGui.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\TAAssert.h"
#include "core\exceptions\src\ApplicationException.h"
#include "core\exceptions\src\DataException.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\corba\src\CorbaUtil.h"

using TA_Base_Core::DebugUtil;
using TA_Base_App::TA_ControlStation::STRINGS_EQUIVALENT;


RunningApplication::RunningApplication(TA_Base_Core::IGui& entity,
                                       bool isVisibleToUserAsRunning /*=true*/)
                                       : m_guiEntity(entity),
                                         m_managedProcess(NULL),
                                         m_managedGUI(NULL),
                                         m_isVisibleToUserAsRunning(isVisibleToUserAsRunning),
                                         m_isRunning(false),
                                         m_entityName(""),
                                         isRelaunchAvailable(false)
{
}


RunningApplication::RunningApplication(TA_Base_Core::IGui& entity,
                                       unsigned long processId,
                                       TA_Base_Core::IManagedProcessCorbaDef_var managedProcess,
                                       bool isVisibleToUserAsRunning /*=true*/)
                                       : m_guiEntity(entity),
                                         m_managedGUI(NULL),
                                         m_isVisibleToUserAsRunning(isVisibleToUserAsRunning),
                                         m_isRunning(true),
                                         m_entityName(""),
                                         m_isPositioned(false),
                                         isRelaunchAvailable(false)
{
    m_managedProcess = managedProcess;
    m_processInformation.dwProcessId = processId;
    m_processInformation.dwThreadId = -1;
    m_processInformation.hProcess = NULL;
    m_processInformation.hThread = NULL;
}


RunningApplication::~RunningApplication()
{
    try
    {
		if (m_applicationMover.getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING)
		{
			m_applicationMover.terminateAndWait();
		}
    }
    catch ( ... )
    {
        // Catch all exceptions so we don't get errors filtering out
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of RunningApplication");
    }
}


void RunningApplication::launch(const std::string& additionalCommandLine, 
                               const unsigned long posFlag,
                               const unsigned long alignFlag,
                               const RECT& objectDim,
                               const RECT& boundaryDim)
{
    // For relaunch
    m_additionalCommandLine = additionalCommandLine;
    m_posFlag = posFlag;
    m_alignFlag = alignFlag;
    m_objectDim = objectDim;
    m_boundaryDim = boundaryDim;
    isRelaunchAvailable = true;

    LOG( SourceInfo, DebugUtil::FunctionEntry, "launch");

    TA_ASSERT( m_isRunning == false, "This application was already running" );

    // First get the executable as stored in the database
    std::string startInstruction = m_guiEntity.getExecutable();

    // Split up the start instruction into the executable and list of command line arguments
    std::string executable;
	std::string arglist;
	parseProcessAddress(executable,arglist,startInstruction);

 	// Get the current directory for the application. This is everything in the pathname up to the last backslash
 	std::string currentDir = executable.substr(0, executable.find_last_of("\\"));

    // Add the extra arguments passed in.
    arglist += " ";
    arglist += additionalCommandLine;

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "About to launch application: %s",executable.c_str());
    LOGMORE( SourceInfo, "With command line arguments: %s",arglist.c_str() );

    // Make sure the executable exists, if not then throw an exception!
    struct stat statbuf;
    if (stat(executable.c_str(), &statbuf) != 0)
    {
        using TA_Base_Core::ApplicationException;
        TA_THROW( ApplicationException(executable.c_str(), "Executable not found - Unable to launch it.") );
    }

    // Construct the WIN32 start process information
    STARTUPINFO sInfo;
    sInfo.cb              = sizeof(STARTUPINFO);
    sInfo.lpReserved      = NULL;
    sInfo.lpReserved2     = NULL;
    sInfo.cbReserved2     = 0;
    sInfo.lpDesktop       = NULL;
    sInfo.lpTitle         = NULL;
    sInfo.dwFillAttribute = 0;
    sInfo.dwFlags         = NULL;

    // Launch the application using CreateProcess
    if( 0 == CreateProcess( executable.c_str(),
                            const_cast<char*>(arglist.c_str()),
                            NULL,
                            NULL,
                            TRUE,
                            DETACHED_PROCESS,
                            NULL,
                            currentDir.c_str(),
                            &sInfo,
                            &m_processInformation)
       )
    {
        // If CreateProcess returns 0 then it means it failed. Get the error message
        // and throw an exception
        char msg[1000];
        FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                        FORMAT_MESSAGE_FROM_SYSTEM | 
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        msg,
                        0,
                        NULL 
                    );

        using TA_Base_Core::ApplicationException;
        TA_THROW( TA_Base_Core::ApplicationException(executable.c_str(),msg) );
    }

    m_isRunning = true;

    m_applicationMover.setProcessDetails(m_processInformation.dwProcessId, posFlag, alignFlag, objectDim, boundaryDim);
    m_applicationMover.start();

    LOG( SourceInfo, DebugUtil::FunctionExit, "launch");
}


void RunningApplication::relaunch(IProcessManager& processManager)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "relaunch");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "RunningApplication::relaunch - shouldTerminateOnLogout=%d, isRelaunchAvailable=%d, entityKey=%d, commandline=%s",
        shouldTerminateOnLogout(), isRelaunchAvailable, m_guiEntity.getKey(), m_additionalCommandLine.c_str() );

    if ( false == shouldTerminateOnLogout() && true == isRelaunchAvailable )
    {
        processManager.launchApplication( m_guiEntity.getKey(), m_additionalCommandLine, m_posFlag, m_alignFlag, m_objectDim, m_boundaryDim );
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "relaunch");
}


void RunningApplication::move( const unsigned long posFlag,
                               const unsigned long alignFlag,
                               const RECT& objectDim,
                               const RECT& boundaryDim)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "move");

    // m_applicationMover.setProcessDetails(m_processInformation.dwProcessId, posFlag, alignFlag, objectDim, boundaryDim);
    if ( !CORBA::is_nil(m_managedGUI))
    {
        try
        {
            // convert normal windows rect to corba RECT 
            TA_Base_Core::IManagedGUICorbaDef::RECT obj, bound;
            obj.bottom = objectDim.bottom;
            obj.top = objectDim.top;
            obj.left = objectDim.left;
            obj.right = objectDim.right;

            bound.bottom = boundaryDim.bottom;
            bound.top = boundaryDim.top;
            bound.left = boundaryDim.left;
            bound.right = boundaryDim.right;

            m_managedGUI->setWindowPosition(posFlag, alignFlag, obj, bound);
        }
	    catch (const CORBA::Exception& ex)
	    {
            using TA_Base_Core::ApplicationException;
		    LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			    "Caught while trying to move application.");
            TA_THROW( ApplicationException("Could not move application. It may not be running") );
	    }
    }
    else
    {
        m_applicationMover.terminateAndWait();
        m_applicationMover.resetThread();       // reset thread
        m_applicationMover.start();
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "move");
}


void RunningApplication::terminate()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "terminate");

    // Retrieve the application name for logging
    std::string applicationName = "Unknown";
    try
    {
        applicationName = m_guiEntity.getName();
    }
    catch ( TA_Base_Core::DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve application name so we will use 'Unknown'.");
    }
    catch ( TA_Base_Core::DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve application name so we will use 'Unknown'.");
    }

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Terminating application - %s", applicationName.c_str() );

    // Check if the application is running. If it has already been stopped then there is no need to terminate again.
    if (!m_isRunning)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Application has already been terminated");
        return;
    }

    // Being told to terminate so no point trying to launch it
    m_applicationMover.terminateAndWait();

    try
    {
        // First we attempt to close the application if it is a managed process
        if ( !CORBA::is_nil(m_managedProcess) )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Terminating as a Managed Process");
            m_managedProcess->terminate();
            m_isRunning = false;
            LOG( SourceInfo, DebugUtil::FunctionExit, "terminate");
            return;
        }
    }
	catch (const CORBA::Exception& ex)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Exception caught from Corba actions when trying to terminate. Will now terminate like a normal Windows application.");
        // Catch any Corba Exceptions that may have been encountered. Since the application
        // probably didn't terminate correctly we will now use the generic terminate procedure.
	}


    // If it is not a Managed Process then we attempt to close the application differently.
    
    //TODO: How long do we wait. Do we wait forever ot ensure that it closes nicley? What about
    //      message boxes being displayed to the user.
    // First try nicely.
    //TODO: The nice method is not working
    DWORD returnCode = WAIT_TIMEOUT;
    DWORD dwExitCode = 0;

    // -1 indicates we don't have the thread and process information.
    if ( m_processInformation.dwThreadId != -1 )
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Terminating as a normal process");
        PostThreadMessage(m_processInformation.dwThreadId, WM_CLOSE, 0, 0);

        returnCode = WaitForSingleObject(m_processInformation.hProcess, 1000);
        // Check exit code
        GetExitCodeProcess(m_processInformation.hProcess, &dwExitCode);
    }
    
    if( (returnCode == WAIT_TIMEOUT) || (dwExitCode == WAIT_FAILED) )
    {
        // Process did not terminate -> force it
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Close failed when trying to terminate the application so now we have to kill it");
        TerminateProcess( m_processInformation.hProcess, 0);  // Zero is the exit code
    }

    // TD 1329
    // Close all handles
    ::CloseHandle(m_processInformation.hProcess);
    ::CloseHandle(m_processInformation.hThread);

    m_isRunning = false;

    LOG( SourceInfo, DebugUtil::FunctionExit, "terminate");
}


void RunningApplication::kill()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "kill");

    // Retrieve the application name for logging
    std::string applicationName = "Unknown";
    try
    {
        applicationName = m_guiEntity.getName();
    }
    catch ( TA_Base_Core::DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve application name so we will use 'Unknown'.");
    }
    catch ( TA_Base_Core::DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve application name so we will use 'Unknown'.");
    }

   LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Application %s would not close nicely. We will now kill it.", applicationName.c_str());
   
   TerminateProcess( m_processInformation.hProcess, 0);  // Zero is the exit code

   // TD 1329 - Release all handles
   ::CloseHandle(m_processInformation.hProcess);
   ::CloseHandle(m_processInformation.hThread);

   LOG( SourceInfo, DebugUtil::FunctionExit, "kill");
}


bool RunningApplication::shouldTerminateOnLogout()
{
    try
    {
        return m_guiEntity.shouldTerminateOnLogout();
    }
    catch ( TA_Base_Core::DataException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not retrieve application name. Returning Unknown");
    }
    catch ( TA_Base_Core::DatabaseException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not retrieve application name. Returning Unknown");
    }

    // If cannot find the entry, then just assume it's supposed to terminate
    return true;
}


void RunningApplication::changeApplicationFocus(const TA_Base_Core::EFocusType focus)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "changeApplicationFocus");

    try
    {
        if ( !CORBA::is_nil(m_managedGUI) )
        {
            m_managedGUI->changeFocus(focus);
        }
        else
        {
            m_focusInfo.processId = getProcessId();
            m_focusInfo.focus = focus;
            // if it is not a managed gui, then find the window and change the focus
            ::EnumWindows( focusNonManagedGui, reinterpret_cast<long>(&m_focusInfo) );
        }
    }
	catch (const CORBA::Exception& ex)
	{
        using TA_Base_Core::ApplicationException;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Caught while trying to change the application focus.");
        TA_THROW( ApplicationException("Could not give the application focus. It may not be running") );
	}

    LOG( SourceInfo, DebugUtil::FunctionExit, "changeApplicationFocus");
}


void RunningApplication::changeServerState(const bool isServerUp)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "changeServerState");

    try
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Going to change the status of %s", getApplicationName().c_str());
        
        if ( !CORBA::is_nil(m_managedGUI) )
        {
            m_managedGUI->serverStateChange( isServerUp );
        }
    }
	catch (const CORBA::Exception& ex)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Caught while trying to change the server state.");
        LOGMORE(SourceInfo, "There is no point alerting the user so we'll log and carry on. The request will just have to fail");
	}

    LOG( SourceInfo, DebugUtil::FunctionExit, "changeServerState");
}


void RunningApplication::passNewParameters(const std::string& additionalCommands)
{
    using TA_Base_Core::RunParams;

    LOG( SourceInfo, DebugUtil::FunctionEntry, "passNewParameters" );
    
    if ( CORBA::is_nil(m_managedProcess) )
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Could not pass new parameters as m_managedProcess was NULL");
        LOG( SourceInfo, DebugUtil::FunctionExit, "passNewParameters" );
        return;
    }

    std::map<std::string,std::string> parameters;
    if (!RunParams::getInstance().parseCmdLineAndReturnIt(additionalCommands, parameters))
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Additional commands passed to the application could not be parsed correctly");
        LOGMORE( SourceInfo, additionalCommands.c_str() );
        LOG( SourceInfo, DebugUtil::FunctionExit, "passNewParameters" );
        return;
    }

    // parameters will now hold the parsed command line
    // Now finally we can pass all these parameters on to the application

    try
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting parameters in application: ");
        for (std::map<std::string,std::string>::iterator iter = parameters.begin();
             iter != parameters.end();
             ++iter)
        {
            if ( !CORBA::is_nil(m_managedProcess) )
            {
                // Do not pass the parameter if it is empty.
                // Do not pass the parameter if it is the entity name. This just holds configuration for the
                // GUI and should not change.
                if ( (!iter->second.empty()) && (iter->first.compare(RPARAM_ENTITYNAME) != STRINGS_EQUIVALENT) )
                {
                    LOGMORE( SourceInfo, "%s=%s", iter->first.c_str(), iter->second.c_str() );
                    TA_Base_Core::RunParam parameter;
                    parameter.name = CORBA::string_dup( iter->first.c_str() );
                    parameter.value = CORBA::string_dup( iter->second.c_str() );
                    m_managedProcess->setParam(parameter);
                }
            }
        }
    }
	catch (const CORBA::Exception& ex)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Caught while trying to setParam() on the managed process.");
        LOGMORE( SourceInfo, " There is no point alerting the user so we'll log and carry on. The request will just have to fail");
	}

    LOG( SourceInfo, DebugUtil::FunctionExit, "passNewParameters" );
}


void RunningApplication::passNewParameters(TA_Base_Core::RunParamSeq_var additionalParameters)
{
    using TA_Base_Core::RunParams;

    LOG( SourceInfo, DebugUtil::FunctionEntry, "passNewParameters" );
    
    try
    {
        if ( CORBA::is_nil(m_managedProcess) )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Could not pass new parameters as m_managedProcess was NULL");
            LOG( SourceInfo, DebugUtil::FunctionExit, "passNewParameters" );
            return;
        }

        // Now finally we can pass all these parameters on to the application
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Repassing parameters to application: ");
        int size = additionalParameters->length();
        for (int i = 0; i < size; ++i)
        {
            if ( !CORBA::is_nil(m_managedProcess) )
            {
                std::string name(additionalParameters[i].name);
                std::string value(additionalParameters[i].value);
                // Do not pass the parameter if it is empty.
                // Do not pass the parameter if it is the entity name. This just holds configuration for the
                // GUI and should not change.
                if ( (!value.empty()) && (name.compare(RPARAM_ENTITYNAME) != STRINGS_EQUIVALENT) )
                {
                    LOGMORE( SourceInfo, "%s=%s", additionalParameters[i].name, additionalParameters[i].value );
                    TA_Base_Core::RunParam parameter;
                    parameter.name =  CORBA::string_dup( name.c_str() );
                    parameter.value = CORBA::string_dup( value.c_str() );
                    m_managedProcess->setParam(parameter);
                }
            }
        }
    }
	catch (const CORBA::Exception& ex)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Caught while trying to setParam() on the managed process.");
        LOGMORE( SourceInfo, " There is no point alerting the user so we'll log and carry on. The request will just have to fail");
	}

    LOG( SourceInfo, DebugUtil::FunctionExit, "passNewParameters" );
}


std::string RunningApplication::getApplicationName()
{
    try
    {
        return m_guiEntity.getName();
    }
    catch ( TA_Base_Core::DataException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not retrieve application name. Returning Unknown");
    }
    catch ( TA_Base_Core::DatabaseException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not retrieve application name. Returning Unknown");
    }

    return "Unknown";
}


std::string RunningApplication::getApplicationFullName()
{
    try
    {
        return m_guiEntity.getFullName();
    }
    catch ( TA_Base_Core::DataException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not retrieve application name. Returning Unknown");
    }
    catch ( TA_Base_Core::DatabaseException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not retrieve application name. Returning Unknown");
    }

    return "Unknown";
}


std::string RunningApplication::getHelpFile()
{
    try
    {
        return m_guiEntity.getHelpFilePath();
    }
    catch ( TA_Base_Core::DataException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not retrieve help file. Returning Unknown");
    }
    catch ( TA_Base_Core::DatabaseException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not retrieve help file. Returning Unknown");
    }

    return "Unknown";
}


TA_Base_Core::RunParamSeq* RunningApplication::getParameters()
{
    try
    {
        if ( !CORBA::is_nil(m_managedProcess) )
        {
            TA_Base_Core::RunParamSeq* toReturn = m_managedProcess->getParams();
            return toReturn;
        }
    }
	catch (const CORBA::Exception& ex)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Caught while trying to getParams() on the managed process.");
	}

    return NULL;
}


bool RunningApplication::isApplicationTypeEqualTo(const unsigned long appType)
{
    try
    {
        return m_guiEntity.getKey() == appType;
    }
    catch ( TA_Base_Core::DataException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not retrieve application type. Returning that this application types does not match.");
    }
    catch ( TA_Base_Core::DatabaseException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not retrieve application type. Returning that this application types does not match.");
    }

    return false;
}


bool RunningApplication::areInstancesLimited()
{
    try
    {
        return m_guiEntity.areInstancesLimited();
    }
    catch ( TA_Base_Core::DataException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not determine if the application is limited to 1 running instance.  Assume it is.");
    }
    catch ( TA_Base_Core::DatabaseException& )
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not determine if the application is limited to 1 running instance.  Assume it is.");
    }

    return true;
}


void RunningApplication::setManagedProcess(const TA_Base_Core::IManagedProcessCorbaDef_var managedProcess)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "setManagedProcess");
    m_managedProcess = managedProcess;
    LOG( SourceInfo, DebugUtil::FunctionExit, "setManagedProcess");
}


void RunningApplication::setManagedGui(const TA_Base_Core::IManagedGUICorbaDef_var managedGUI)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "setManagedGui");
    m_managedGUI = managedGUI;
    try
    {
        if ( !CORBA::is_nil(m_managedGUI))
        {
            // This has registered as a ManagedGUI so we can set the position ourselves.
            m_applicationMover.terminateAndWait();
            ApplicationMover::WindowMoveInformation det = m_applicationMover.getProcessDetails();
            // convert normal windows rect to corba RECT 
            TA_Base_Core::IManagedGUICorbaDef::RECT obj, bound;
            obj.bottom = det.objectDim.bottom;
            obj.top = det.objectDim.top;
            obj.left = det.objectDim.left;
            obj.right = det.objectDim.right;

            bound.bottom = det.boundaryDim.bottom;
            bound.top = det.boundaryDim.top;
            bound.left = det.boundaryDim.left;
            bound.right = det.boundaryDim.right;

            m_managedGUI->setWindowPosition(det.posFlag, det.alignFlag, obj, bound);
        }
    }
	catch (const CORBA::Exception& ex)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Could not tell the window to position.");
	}
    LOG( SourceInfo, DebugUtil::FunctionExit, "setManagedGui");
}


void RunningApplication::parseProcessAddress(std::string& startPath,
                                             std::string& argList,
                                             const std::string& processAddress)
{
    // Eg processAddress is "c:\transactive\alarm.exe --param=blah"

    // find the absolute path by searching for the .exe
    std::string::size_type pos = processAddress.find(".exe");
	if (pos == std::string::npos)
    {
        // Then the path we were given is not valid. Throw an exception.
        TA_THROW( TA_Base_Core::ApplicationException(processAddress.c_str(), "The string '.exe' was not found in the executable field.") );
    }
    pos = pos + 4;  // Add 4 for the string '.exe'.

    // The start path
    // Eg this willl give us "c:\transactive\alarm.exe"
    startPath = processAddress.substr(0, pos);  

    // The list of arguments
    // Eg this will give us " --param=blah"
    argList = processAddress.substr(pos, processAddress.size() - pos);
}


BOOL CALLBACK RunningApplication::focusNonManagedGui(HWND hWnd, LPARAM lParam)
{
    WindowFocusInfo* info = reinterpret_cast<WindowFocusInfo*>(lParam);
	if (info == NULL)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "focus is NULL");
        return FALSE;
    }

	// find out which process & thread created this window
	DWORD dwThisWindowProcessID;
	DWORD dwThisWindowThreadID;
	dwThisWindowThreadID = ::GetWindowThreadProcessId(hWnd, &dwThisWindowProcessID);

    //Raymond Pau++ TD16246 Each process can have more than 1 window without a parent.
    //                      Need to ensure that all of them get focus.

	// if it's the process we are interested in then we want to move it
	if (info->processId == dwThisWindowProcessID)
	{
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "focusNonManagedGui::Found a window with matching ID");

		
        // Get some information about it to help determine if it is the correct one
        if (NULL != ::GetParent(hWnd))
        {
            // This child is not visible or it is not the parent window
            // Therefore we are not interested.
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "It's a subwindow. Ignore.");
            return TRUE;
        }

        switch(info->focus)
        {
        case(TA_Base_Core::Minimise):
            // If window is not already minimised then minimise it
            if(FALSE == ::IsIconic(hWnd))
            {
				::ShowWindow(hWnd, SW_MINIMIZE);
            }
            break;

        case(TA_Base_Core::Maximise):
           	::ShowWindow(hWnd, SW_MAXIMIZE);
            ::SetForegroundWindow(hWnd);
            // Set the window to be the top most so it is given focus and then move it back again.
            ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
            ::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
            break;

        case(TA_Base_Core::GetFocus):
            if(TRUE == ::IsIconic(hWnd))
            {
				::ShowWindow(hWnd, SW_RESTORE);
            }
            ::SetForegroundWindow(hWnd);
            // Set the window to be the top most so it is given focus and then move it back again.
            ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
            ::SetWindowPos(hWnd , HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
            break;

        default:
            TA_ASSERT(false, "Invalid enumerated type passed for window focus change");
        }

        //return FALSE;
	}
    //++Raymond Pau TD16246
	return TRUE;
}


void RunningApplication::updateSessionId(std::string& sessionId)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "updateSessionId" );
    try
    {
        if ( !CORBA::is_nil(m_managedProcess) )
        {
            TA_Base_Core::RunParam parameter;
            parameter.name = CORBA::string_dup(RPARAM_SESSIONID);
            parameter.value = CORBA::string_dup( sessionId.c_str() );
            m_managedProcess->setParam(parameter);
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Successfully set Session ID for %s, thread ID = %d, process ID = %d", 
                getApplicationFullName().c_str(), 
                static_cast<int>(m_processInformation.dwThreadId), 
                static_cast<int>(m_processInformation.dwProcessId));
        }
    }
	catch (const CORBA::Exception& ex)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Caught while trying to update session ID on the managed process %s, thread ID = %d, process ID = %d", 
            getApplicationFullName().c_str(), 
            static_cast<int>(m_processInformation.dwThreadId), 
            static_cast<int>(m_processInformation.dwProcessId));
        LOGMORE( SourceInfo, " There is no point alerting the user so we'll log and carry on. The request will just have to fail");
	}

    LOG( SourceInfo, DebugUtil::FunctionExit, "updateSessionId" );
}

void RunningApplication::dutyChanged()
{
    FUNCTION_ENTRY("dutyChanged");
    try
    {
        if ( !CORBA::is_nil(m_managedGUI) )
        {
            m_managedGUI->dutyChanged();
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Notified process %s, thread ID = %d, process ID = %d of duty changes", 
                getApplicationFullName().c_str(), 
                static_cast<int>(m_processInformation.dwThreadId), 
                static_cast<int>(m_processInformation.dwProcessId));
        }
    }
	catch (const CORBA::Exception& ex)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			"Caught while trying to call dutyChanged() on the managed process %s, thread ID = %d, process ID = %d", 
            getApplicationFullName().c_str(), 
            static_cast<int>(m_processInformation.dwThreadId), 
            static_cast<int>(m_processInformation.dwProcessId));
        LOGMORE( SourceInfo, " There is no point alerting the user so we'll log and carry on. The request will just have to fail");
	}

    FUNCTION_EXIT;
}


void RunningApplication::getPositionInformation(long& leftPosition, long& topPosition)
{
    if ( !CORBA::is_nil(m_managedGUI))
    {
        try
        {
            TA_Base_Core::IManagedGUICorbaDef::RECT rect = m_managedGUI->getWindowPosition();
            leftPosition = rect.left;
            topPosition = rect.right;
        }
	    catch (const CORBA::Exception& ex)
	    {
		    LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
			    "Could get window position.");
	    }
    }
    else
    {
        RECT rect = m_applicationMover.getProcessDetails().appDim;
        leftPosition = rect.left;
        topPosition = rect.right;

    }
}
