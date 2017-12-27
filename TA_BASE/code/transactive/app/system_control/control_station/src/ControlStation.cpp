/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Davide Calabro
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * 
  * Defines the class behaviors for the application.
  */

#include "app\system_control\control_station\src\stdafx.h"
#include "app\system_control\control_station\src\ControlStation.h"
#include "app\system_control\control_station\src\EnumProcess.h"
#include "app\system_control\control_station\src\ControlStationRootDialog.h"
#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "app\system_control\control_station\src\ControlStation_i.c"
#include "app\system_control\control_station\src\ControlStation2.h"
#include "app\system_control\control_station\src\SchematicDisplay.h"
#include "app\system_control\control_station\src\RippleControlStation_i.c"
//#include "app\system_control\control_station\src\sinstance.h"

#include <initguid.h>

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "core\exceptions\src\ProcessAlreadyRunningException.h"

#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\corba\src\CorbaUtil.h"
#include "core\exceptions\src\UtilitiesException.h"
#include "core\exceptions\src\InvalidCommandLineException.h"
#include "core\exceptions\src\SessionException.h"
#include "core\exceptions\src\MessageResourceException.h"

#include "ace/ACE.h"

// TD 12436 ++wenguang
#pragma warning(disable:4819)
#include "ZipArchive.h"
#include <vector>
// TD 12436 wenguang++

#ifdef DEMO_VERSION
#include "DemoKeygen.h"
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::SessionException;
using TA_Base_Core::MessageResourceException;
using TA_Base_Core::ProcessAlreadyRunningException;
using TA_Base_App::TA_ControlStation::APPLICATION_NAME;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ControlStationApp

BEGIN_MESSAGE_MAP(ControlStationApp, CWinApp)
	//{{AFX_MSG_MAP(ControlStationApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ControlStationApp construction

ControlStationApp::ControlStationApp()
    : m_mutex(NULL),
      m_logsThread(NULL)
{
	// Place all significant initialization in InitInstance
}

ControlStationApp::~ControlStationApp()
{
	//TD17838,39 haipeng
	if (m_mutex != NULL)
	{
		m_mutex->Unlock();
		delete m_mutex;
		m_mutex = NULL;
	}

	if (m_logsThread != NULL)
	{
		m_logsThread->terminateAndWait();
		delete m_logsThread;
		m_logsThread = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// The one and only ControlStationApp object

ControlStationApp theApp;

/////////////////////////////////////////////////////////////////////////////
// ControlStationApp initialization

namespace
{
    void checkDemoKey()
    {
#ifdef DEMO_VERSION
        try
        {
            DemoKeygen keygen;

            if ( keygen.getKeyFromFile() != keygen.generateKey() )
            {
                throw std::runtime_error( "key not match" );
            }
        }
        catch ( std::exception& )
        {
            ::MessageBox( NULL, "This computer is not authorized to run the demo version.\nPlease contact the administrator.", "ControlStation", MB_OK | MB_ICONSTOP );
            ::exit(0);
        }
#endif
    }
}


BOOL ControlStationApp::InitInstance()
{
	m_mutex = new CMutex(true, APPLICATION_NAME);

	//TD17838,39 haipeng 
	if (! m_mutex->Lock(100)) 
	{
		// Another instance is running, quit
        CString appName;
        TA_VERIFY(appName.LoadString(IDS_APPNAME)!=0, "Cannot load IDS_APPNAME");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << appName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010009);
		return FALSE;
	}
	
    // Initialise ACE
    ACE::init();

    // Initialise Active Template Libraries
	if (!initATL())
    {
		return FALSE;
    }

    {
        MSG Msg;
        while(GetMessage(&Msg, NULL, 0, 0) > 0) 
        { 
            TranslateMessage(&Msg); 
            DispatchMessage(&Msg); 
        } 

        while (true)
        {
            Sleep(1000);
        }
    }


	AfxEnableControlContainer();

    // do this so that we can use rich edit control
    AfxInitRichEdit();

    /*Haipeng commented those code for that those below will not work properly.TD17838,39 haipeng
    CEnumProcess enumeration;
    CEnumProcess::CProcessEntry entry;

    // Get the name of this executable to see if one is already running.
    std::string controlStationName(m_pszExeName);
	
	AfxMessageBox(controlStationName.c_str());

    // Cut the name to 15 characters if necessary as it seems to get truncated.
    if (controlStationName.size() > 15)
    {
        controlStationName = controlStationName.substr(0,15);
    }


    // Check that there isn't already an instance of the Control Station running by stepping through all
    // processes. If we have two Control Station's then one was already running when we started.
    int controlStationCount = 0;
    BOOL isMoreProcesses = enumeration.GetProcessFirst(&entry);
    while(isMoreProcesses)
    {
        std::string filename(entry.lpFilename);
        if( std::string::npos != filename.find(controlStationName,0) )
        {
            controlStationCount++;
        }
        isMoreProcesses = enumeration.GetProcessNext(&entry);
    }

    if (controlStationCount > 1)
    {
        CString appName;
        TA_VERIFY(appName.LoadString(IDS_APPNAME)!=0, "Cannot load IDS_APPNAME");
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << appName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010009);
		return false; 
    }*///Haipeng //TD17838,39 haipeng


    // Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	_Module.Lock();


	// TBC: will have to decide what to do if started as a result of a 
	// COM client instantiating the control station class.
	// For the time being, don't display the login dialog, just return true 
	// and start servicing requests.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}

    TA_Base_Bus::TransActiveMessage userMsg;
    try
    {
        try
        {
            initialiseUtilities(m_lpCmdLine);
        }
        catch( TA_Base_Core::UtilitiesException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "InvalidCommandLineException", "Must exit as the utilities did not intialise correctly");
				std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
				userMsg << appName;
            userMsg.showMsgBox(IDS_UE_010012);
            return FALSE;
        }
		catch( TA_Base_Core::ProcessAlreadyRunningException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "ProcessAlreadyRunningException", "For display version info only.");
			return FALSE;
		}
        catch( TA_Base_Core::InvalidCommandLineException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "InvalidCommandLineException", m_lpCmdLine );
            userMsg.showMsgBox(IDS_UE_010008);
            return FALSE;
        }
        
        if ( TA_Base_Core::RunParams::getInstance().get(TA_Base_App::TA_ControlStation::HOST_NAMES.c_str()).empty() )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal, "--host-names was not specified on the command line");
            userMsg.showMsgBox(IDS_UE_010008);
            return FALSE;
        }
        else if ( TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).empty() )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal, "--entity-name was not specified on the command line");
            userMsg.showMsgBox(IDS_UE_010008);
            return FALSE;
        }

        // Save the current system settings so we can restore them later
	    SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &m_focusTimeout, 0);

        // Ensure that windows are allowed to take the focus and come to the front
        if (!SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE))
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not set the system parametesr so that windows are launched to the top");
        }

        m_pMainWnd = &(ControlStationRootDialog::getInstance());

        ControlStationRootDialog::getInstance().DoModal();
    }
    catch ( SessionException&)
    {
        userMsg.showMsgBox(IDS_UE_020043);
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "SessionException","The session could not be set up. We must alert the user and exit");
        m_pMainWnd = NULL;
        return FALSE;
    }
    catch ( const MessageResourceException& ex)
    {
        // The problem was a message box that failed to display. Need to notify the user what is happening,
        // but WITHOUT using a TransActiveMessage
        
        // First, log the catch
        LOG_EXCEPTION_CATCH(SourceInfo, "MessageResourceException", "Caught exception at top level, so application will have to shutdown.");

        // Then the FATAL log
        CString errorMessage;
        switch (ex.getFailType())
        {
        case (MessageResourceException::DLL_NOT_FOUND):
            {
                errorMessage = "The MessageResource DLL could not be found. Message ID: %d.";
                break;
            };
        case (MessageResourceException::FORMATTING_ERROR):
            {
                errorMessage = "The message retrieved for ID %d contained invalid formatting details.";
                break;
            };
        case (MessageResourceException::LANGUAGE_ERROR):
            {
                errorMessage = "The message ID %d could not be found within the MessageResource for this systems language context.";
                break;
            };
        case (MessageResourceException::RESOURCE_NOT_FOUND_ERROR):
            {
                errorMessage = "The message ID %d could not be found within the MessageResource DLL.";
                break;
            };
        case (MessageResourceException::GENERAL_ERROR):
            {
                errorMessage = "An unspecified error occured while attempting to display the message with ID %d.";
                break;
            };
        default:
            {
                TA_ASSERT(false,"An invalid MessageResourceException type was found");
            }
        }            
        errorMessage += " Error description reported from library follows:";
        LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, errorMessage, ex.getMessageID());
        LOGMORE(SourceInfo, ex.what());
        LOGMORE(SourceInfo, "As this exception was not caught within the application, Control Station must now exit.");

		// TD14164 azenitha++
		CString errMsg;
		errMsg.LoadString(IDS_ERROR_MESSAGE);
        /*::MessageBox(NULL,"An error occured while attempting to display an application message box.\nThis is an unrecoverable error. Check log file for further details.\nControl Station will now exit.",
            "Control Station",MB_OK|MB_ICONSTOP);*/
		::MessageBox(NULL, errMsg, "Control Station",MB_OK|MB_ICONSTOP);
		// TD14164 ++azenitha

        return FALSE;
    }
    catch ( TA_Base_Core::TransactiveException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransActiveException","This is caught at the top level so we exit nicely.");
        m_pMainWnd = NULL;
        userMsg.showMsgBox(IDS_UE_020043);
        return FALSE;
    }
    catch ( ... )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown","This is caught at the top level so we exit nicely.");
        m_pMainWnd = NULL;
		//TD17838,39 haipeng disabled the pop msg, no need to scare the customers!
        //userMsg.showMsgBox(IDS_UE_020043);
		//TD17838,39 haipeng
        return FALSE;
    }
	
    return TRUE;
}


CControlStationModule _Module; // our ATL Module

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ControlStation2, CControlStation2)
OBJECT_ENTRY(CLSID_SchematicDisplay, CSchematicDisplay)
END_OBJECT_MAP()


int ControlStationApp::ExitInstance() 
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "ExitInstance");

	//TD18084
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "start to clean up Session Manager");
    SessionManager::cleanUp();
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "finish clean up Session Manager");

	SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, &m_focusTimeout, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE); 

	if (m_bATLInited)
	{
		_Module.RevokeClassObjects();
		_Module.Term();
		CoUninitialize();
	}

    ACE::fini();

	std::string cmd = GetCommandLine(); 
    BOOL bRun = TRUE;

    // if uninstall or install, no need to restart windows

    if ( cmd.find(TA_Base_App::TA_ControlStation::UNREGISTER_CMD) == std::string::npos
		&& cmd.find(TA_Base_App::TA_ControlStation::REGISTER_CMD) == std::string::npos 
		&& SessionManager::exitNeedsRestart())
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Needs to restart the workstation");
        restartWorkstation();
    }
    else
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Exiting to windows");
        exitFromWindows();
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "ExitInstance");

	//TD18084
	if (m_mutex != NULL)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "release the mutex for controlstation");
		m_mutex->Unlock();
		delete m_mutex;
		m_mutex = NULL;
	}
	
	return CWinApp::ExitInstance();
}


void ControlStationApp::initialiseUtilities(const std::string& commandLine)
{
    using TA_Base_Core::RunParams;
    using TA_Base_Core::CorbaUtil;
  
    //
    // Parse the command line
    //
    if( false == RunParams::getInstance().parseCmdLine(commandLine.c_str()) )
    {
		// If the version information is requested, open the about box.
		if (RunParams::getInstance().isSet(RPARAM_VERSION))
		{
			TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

			LOG( SourceInfo, DebugUtil::ExceptionCatch,"InvalidCommandLineException", "Will change this exception into a ProcessAlreadyRunningException shut down quietly after version info" );
			TA_THROW ( ProcessAlreadyRunningException( "Version info only" ) );	
		}
		else
		{
			TA_THROW (TA_Base_Core::InvalidCommandLineException("Command line arguments are in invalid format"));
		}
    }

    //
    // Set up DebugUtil
    //

    std::string param;

    // Set the debug level
    param = RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
    if (!param.empty())
    {
        DebugUtil::getInstance().setLevel(DebugUtil::getDebugLevelFromString(param.c_str()));
    }

    // Set the maximum size of a debug file in bytes
    param = RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);
    if (!param.empty())
    {
        DebugUtil::getInstance().setMaxSize(atoi(param.c_str()));
    }

    // Set the maximum number of log files
    param = RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);
    if (!param.empty())
    {
        DebugUtil::getInstance().setMaxFiles(atoi(param.c_str()));
    }

	// Use Pid encoding in filenames
	param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGPIDFILENAMES);
	if (!param.empty())
	{
		TA_Base_Core::DebugUtil::getInstance().encryptPidInFilenames(param);
	}
	
	// Use debug level specific log files
	param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVELFILE);
	if (!param.empty())
	{
		TA_Base_Core::DebugUtil::getInstance().decodeLevelFilenames(param);
	}
	
	// Turn off logging for debug levels
	param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVELOFF);
	if (!param.empty())
	{
		TA_Base_Core::DebugUtil::getInstance().decodeDisabledLevels(param);
	}

    // Set the debug file name
    param = RunParams::getInstance().get(RPARAM_DEBUGFILE);
    if (!param.empty())
    {
        // TD12436 ++wenguang
        archiveLogFiles( param);
        // TD12436 wenguang++
        DebugUtil::getInstance().setFile(param.c_str());
    }

    //
    // Set up Corbautil
    //
    unsigned long mgrPort = TA_Base_Core::DEFAULT_MGR_PORT;

    std::string mgrPortString = RunParams::getInstance().get(RPARAM_MGRPORT);
    if( !mgrPortString.empty() )
    {
		if (std::string::npos == mgrPortString.find_first_not_of("0123456789") )
        {
            // String contains valid characters so convert it
            mgrPort = atol(mgrPortString.c_str() );
            if (mgrPort == 0)
            {
                mgrPort = TA_Base_Core::DEFAULT_MGR_PORT;
            }
        }
    }

    if ( false == CorbaUtil::getInstance().initialise(mgrPort) )
    {
        TA_THROW( TA_Base_Core::UtilitiesException("Failed to initialise corba") );
    }

    try
    {
        CorbaUtil::getInstance().activate();
    }
    catch( ... )
    {
        TA_THROW( TA_Base_Core::UtilitiesException("Failed to initialise corba") );
    }
}


void ControlStationApp::restartWorkstation()
{
    FUNCTION_ENTRY("restartWorkstation");

    HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 

    // Get a token for this process. 
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "OpenProcessToken"); 
        return;
    }

    // Get the LUID for the shutdown privilege. 
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 

    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

    // Get the shutdown privilege for this process. 
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 

    // Cannot test the return value of AdjustTokenPrivileges. 
    if (GetLastError() != ERROR_SUCCESS) 
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "AdjustTokenPrivileges"); 
        FUNCTION_EXIT;
        return;
    }

    if (InitiateSystemShutdown(NULL, NULL, 0, TRUE, TRUE) != 0)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Successfully initiate system reboot first time round");
        LOG( SourceInfo, DebugUtil::FunctionExit, "restart");
        FUNCTION_EXIT;
        return;
    }

    // Shut down the system and force all applications to close. 
    if (ExitWindowsEx(EWX_REBOOT, NULL) != 0)
    {
        // Force a restart if ExitWindowsEx fails. (?)
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Successfully initiated system reboot second time round.");
        LOG( SourceInfo, DebugUtil::FunctionExit, "restart");
        FUNCTION_EXIT;
        return;
    }

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Failed to reboot nicely, forcing reboot now.");
    ExitWindowsEx(EWX_REBOOT | EWX_FORCE, NULL);

    FUNCTION_EXIT;
}


void ControlStationApp::exitFromWindows()
{
    FUNCTION_ENTRY("exitFromWindows");

    // Log off only if it is not already running explorer.exe (that means it's not running as a shell substitute).  
    CEnumProcess enumeration;
    CEnumProcess::CProcessEntry entry;    

	bool shouldLogoffWindow = true;
    CString explorerStr = "explorer.exe";
    enumeration.GetProcessFirst(&entry);

    do
    {
        if(explorerStr.CompareNoCase(entry.lpFilename) == 0)
        {
			shouldLogoffWindow = false;
            break;
        }
    }
    while (enumeration.GetProcessNext(&entry));

    if (shouldLogoffWindow)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Logging out of windows");
        // Force all applications to close and log off
        if (ExitWindowsEx(EWX_LOGOFF, NULL) == 0)
        {
            // Force a window log off if ExitWindowsEx fails. (?)
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not log off windows normally: forcing log off.");
            ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, NULL);
        }
    }
    else
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Not required to log out of windows as windows explorer is running");
    }
    FUNCTION_EXIT;
}


BOOL ControlStationApp::initATL()
{
	m_bATLInited = TRUE;

    HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(hRes))
	{
		m_bATLInited = FALSE;
		return FALSE;
	}

	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	_Module.dwThreadID = GetCurrentThreadId();


	std::string cmd = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
    BOOL bRun = TRUE;

    // uninstall it
    if (cmd.find(TA_Base_App::TA_ControlStation::UNREGISTER_CMD) != std::string::npos)
    {
		_Module.UpdateRegistryFromResource(IDR_CONTROLSTATION2, FALSE);
		_Module.UpdateRegistryFromResource(IDR_SCHEMATICDISPLAY, FALSE);
		_Module.UnregisterServer(TRUE); //TRUE means typelib is unreg'd
		bRun = FALSE;
    }
    else if (cmd.find(TA_Base_App::TA_ControlStation::REGISTER_CMD) != std::string::npos)
    {
	    _Module.UpdateRegistryFromResource(IDR_SCHEMATICDISPLAY, TRUE);
        _Module.UpdateRegistryFromResource(IDR_CONTROLSTATION2, TRUE);
	    //_Module.UpdateRegistryFromResource(IDR_SCHEMATICDISPLAY, TRUE);
	    _Module.RegisterServer(TRUE);
		bRun = FALSE;
    }

	if (!bRun)
	{
		m_bATLInited = FALSE;
		_Module.Term();
		CoUninitialize();
		return FALSE;
	}

    checkDemoKey();

	hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
		REGCLS_MULTIPLEUSE);
	if (FAILED(hRes))
	{
		m_bATLInited = FALSE;
		CoUninitialize();
		return FALSE;
	}	

	return TRUE;

}

//TD12436 wenguang++
void ControlStationApp::archiveLogFiles(const std::string& debugFileName)
{
    if ( debugFileName.empty() )
    {
        return;
    }

    unsigned int pos = debugFileName.rfind("\\");

    if ( pos != std::string::npos )
    {
        std::string dirStr;
        dirStr.assign(debugFileName, 0, pos + 1);

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, dirStr.c_str());

        CTime currentTime = CTime::GetCurrentTime();
        CTime time_to_clear_file = CTime::GetCurrentTime() - CTimeSpan(0, 0, 1, 0);

        std::string timeStr = currentTime.Format("%d%m%Y-%H%M");
        std::string folderName = "ArchivedLogs" + timeStr;

        {
            std::string createFolder = "cmd.exe /c mkdir " + dirStr + folderName;
            //	HANDLE CreateeHandle =  (HANDLE)WinExec(createFolder.c_str(), SW_SHOWNORMAL);
            //	WaitForSingleObject(CreateeHandle,INFINITE);
            //	std::string moveFileToFolder = "cmd.exe /c move /Y "+dirStr+"*.log " + dirStr+folderName;
            //	HANDLE MoveHandle =  (HANDLE)WinExec(moveFileToFolder.c_str(), SW_SHOWNORMAL);	
            //	WaitForSingleObject(MoveHandle,INFINITE);
            char *createName = (char*) createFolder.c_str();
            PROCESS_INFORMATION ProcessInfo0;
            STARTUPINFOA StartupInfo0; //This is an [in] parameter
            ZeroMemory(&StartupInfo0, sizeof(StartupInfo0));
            StartupInfo0.wShowWindow = SW_HIDE;
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Move file");

            if ( CreateProcessA(NULL, createName, NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &StartupInfo0, &ProcessInfo0) )
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Move file Done");
                WaitForSingleObject(ProcessInfo0.hProcess, INFINITE);
                CloseHandle(ProcessInfo0.hThread);
                CloseHandle(ProcessInfo0.hProcess);
            }
            else
            {
                //MessageBoxA("The process could not be started...");
            }
        }

        {
            std::string moveFileToFolder = "cmd.exe /c move /Y " + dirStr + "*.log " + dirStr + folderName;
            //	HANDLE MoveHandle =  (HANDLE)WinExec(moveFileToFolder.c_str(), SW_SHOWNORMAL);	
            //	WaitForSingleObject(MoveHandle,INFINITE);
            char *moveName = (char*) moveFileToFolder.c_str();
            PROCESS_INFORMATION ProcessInfo;
            STARTUPINFOA StartupInfo; //This is an [in] parameter
            ZeroMemory(&StartupInfo, sizeof(StartupInfo));
            StartupInfo.wShowWindow = SW_HIDE;

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Move file");

            if ( CreateProcessA(NULL, moveName, NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &StartupInfo, &ProcessInfo) )
            {

                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Move file Done");
                WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
                CloseHandle(ProcessInfo.hThread);
                CloseHandle(ProcessInfo.hProcess);
            }
            else
            {
                //MessageBoxA("The process could not be started...");
            }
        }

        {
            std::string copyFileToFolder = "cmd.exe /c copy " + dirStr + "*.log " + dirStr + folderName;
            //	HANDLE CopyHandle =  (HANDLE)WinExec(copyFileToFolder.c_str(), SW_SHOWNORMAL);		
            //	WaitForSingleObject(CopyHandle,INFINITE);
            char *copyName = (char*) copyFileToFolder.c_str();
            PROCESS_INFORMATION ProcessInfo1;
            STARTUPINFOA StartupInfo1; //This is an [in] parameter
            ZeroMemory(&StartupInfo1, sizeof(StartupInfo1));
            StartupInfo1.wShowWindow = SW_HIDE;

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Copy File");

            if ( CreateProcessA(NULL, copyName, NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &StartupInfo1, &ProcessInfo1) )
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Copy File Done");

                WaitForSingleObject(ProcessInfo1.hProcess, INFINITE);
                CloseHandle(ProcessInfo1.hThread);
                CloseHandle(ProcessInfo1.hProcess);
            }
            else
            {
                //MessageBoxA("The process could not be started...");
            }
        }

        m_logsThread = new CLogsThread(dirStr, timeStr, time_to_clear_file);
        m_logsThread->start();
    }
}
//TD12436 ++wenguang


LONG CControlStationModule::Unlock()
{
	AfxOleUnlockApp();
	return 0;
}


LONG CControlStationModule::Lock()
{
	AfxOleLockApp();
	return 1;
}
