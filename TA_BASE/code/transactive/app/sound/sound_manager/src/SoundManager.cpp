/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/src/SoundManager.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * The starting point of the Sound Manager. Either
  * registers the app as a COM server, or begins
  * taking requests from clients, depending on command
  * line arguments. Most of this code was generated
  * by the Visual C++ wizard.
  *
  */


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f SoundManagerps.mk in the project directory.

#ifdef _MSC_VER
	#pragma warning(disable:4786)
	#pragma warning(disable:4042)
#endif

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "bus/sound/sound_client/src/SoundManager.h"
#include "bus/sound/sound_client/src/SoundManager_i.c"

#include "SoundInterface.h"
#include "RunParamFetcher.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/ManagedProcessException.h"

#include <boost/tokenizer.hpp>
#include <string>

using TA_Base_Core::DebugUtil;

CServiceModule _Module;
std::string hostNames;  // The hostnames listed on the command line.
const std::string REGISTRY_KEY = "AppID\\TA_SoundManager.exe";


BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_SoundInterface, SoundInterface)
END_OBJECT_MAP()


// Although some of these functions are big they are declared inline since they are only used once

inline HRESULT CServiceModule::RegisterServer(BOOL bRegTypeLib, BOOL bService)
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
	{
        return hr;
	}

    // Remove any previous service since it may point to
    // the incorrect file
    Uninstall();

    // Add service entries
    UpdateRegistryFromResource(IDR_SoundManager, TRUE);

    // Adjust the AppID for Local Server or Service
    CRegKey keyAppID;
    LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
    if (lRes != ERROR_SUCCESS)
	{
        return lRes;
	}

    CRegKey key;
    lRes = key.Open(keyAppID, _T("{833DEE1C-D5FC-4868-9C8B-07C22F54A216}"), KEY_WRITE);
    if (lRes != ERROR_SUCCESS)
	{
        return lRes;
	}
    key.DeleteValue(_T("LocalService"));
    
    if (bService)
    {
        key.SetValue(_T("SoundManager"), _T("LocalService"));
        key.SetValue(_T("-Service"), _T("ServiceParameters"));
        // Create service
        Install();
    }

    // Add object entries
    hr = CComModule::RegisterServer(bRegTypeLib);

	// Write the hostnames to the registry.
	char hostNamesCstr[80];
	strcpy(hostNamesCstr, hostNames.c_str());
	BSTR bs = A2BSTR(hostNamesCstr);

	CRegKey hostNamesKey;
	LONG returnValue = hostNamesKey.Open(HKEY_CLASSES_ROOT, REGISTRY_KEY.c_str(), KEY_WRITE);
	TA_ASSERT(returnValue == ERROR_SUCCESS, "CRegKey::Open failed");
	returnValue = hostNamesKey.SetValue(hostNames.c_str(), "Hostnames");
	TA_ASSERT(returnValue == ERROR_SUCCESS, "CRegKey::SetValue failed");

	if (lRes != ERROR_SUCCESS)
	{
		return lRes;
	}

    CoUninitialize();
    return hr;
}

inline HRESULT CServiceModule::UnregisterServer()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return hr;

    // Remove service entries
    UpdateRegistryFromResource(IDR_SoundManager, FALSE);
    // Remove service
    Uninstall();
    // Remove object entries
    CComModule::UnregisterServer(TRUE);
    CoUninitialize();
    return S_OK;
}

inline void CServiceModule::Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid)
{
    CComModule::Init(p, h, plibid);

    m_bService = TRUE;

    LoadString(h, nServiceNameID, m_szServiceName, sizeof(m_szServiceName) / sizeof(TCHAR));

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_STOPPED;
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_status.dwWin32ExitCode = 0;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;
}

LONG CServiceModule::Unlock()
{
    LONG l = CComModule::Unlock();
    if (l == 0 && !m_bService)
        PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
    return l;
}

BOOL CServiceModule::IsInstalled()
{
    BOOL bResult = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
        SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

inline BOOL CServiceModule::Install()
{
    if (IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        CString actionName;
        actionName.LoadString(IDS_INSTALL);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_220001);
        return FALSE;
    }

    // Get the executable file path
    TCHAR szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

    SC_HANDLE hService = ::CreateService(
        hSCM, m_szServiceName, m_szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_220003);
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

inline BOOL CServiceModule::Uninstall()
{
    if (!IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        CString actionName;
        actionName.LoadString(IDS_UNINSTALL);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_220001);
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_220004);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    TA_Base_Bus::TransActiveMessage userMsg;
    UINT selectedButton = userMsg.showMsgBox(IDS_UE_220004);
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
// Logging functions
void CServiceModule::LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;

    va_start(pArg, pFormat);
    _vstprintf(chMsg, pFormat, pArg);
    va_end(pArg);

    lpszStrings[0] = chMsg;

    if (m_bService)
    {
        /* Get a handle to use with ReportEvent(). */
        hEventSource = RegisterEventSource(NULL, m_szServiceName);
        if (hEventSource != NULL)
        {
            /* Write to event log. */
            ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
            DeregisterEventSource(hEventSource);
        }
    }
    else
    {
        // As we are not running as a service, just write the error to the console.
        _putts(chMsg);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Service startup and registration
inline void CServiceModule::Start()
{
    SERVICE_TABLE_ENTRY st[] =
    {
        { m_szServiceName, _ServiceMain },
        { NULL, NULL }
    };
    if (m_bService && !::StartServiceCtrlDispatcher(st))
    {
        m_bService = FALSE;
    }
    if (m_bService == FALSE)
        Run();
}

inline void CServiceModule::ServiceMain(DWORD /* dwArgc */, LPTSTR* /* lpszArgv */)
{
    // Register the control request handler
    m_status.dwCurrentState = SERVICE_START_PENDING;
    m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);
    if (m_hServiceStatus == NULL)
    {
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(SERVICE_START_PENDING);

    m_status.dwWin32ExitCode = S_OK;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    // When the Run function returns, the service has stopped.
    Run();

    SetServiceStatus(SERVICE_STOPPED);
    LogEvent(_T("Service stopped"));
}

inline void CServiceModule::Handler(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        SetServiceStatus(SERVICE_STOP_PENDING);
        PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        break;
    default:
        LogEvent(_T("Bad service request"));
    }
}

void WINAPI CServiceModule::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    _Module.ServiceMain(dwArgc, lpszArgv);
}
void WINAPI CServiceModule::_Handler(DWORD dwOpcode)
{
    _Module.Handler(dwOpcode); 
}

void CServiceModule::SetServiceStatus(DWORD dwState)
{
    m_status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_status);
}

void CServiceModule::Run()
{
	TA_Base_Core::CorbaUtil::getInstance().initialise();
	TA_Base_Core::CorbaUtil::getInstance().activate();

	unsigned long length = 80;
    char hostNamesCstr[80];

    // Get the Hostnames
	CRegKey hostnamesKey;
	LONG returnValue = hostnamesKey.Open(HKEY_CLASSES_ROOT, REGISTRY_KEY.c_str(), KEY_READ);
	TA_ASSERT(returnValue == ERROR_SUCCESS, "CRegKey::Open failed");
	hostnamesKey.QueryValue(hostNamesCstr, "Hostnames", &length);

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Retrieved the following list of hostnames from the registry: %s",
		hostNamesCstr);

	try
	{
		TA_Base_App::RunParamFetcher runParamFetcher(hostNamesCstr);
	}
	catch(TA_Base_Core::ManagedProcessException&)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, 
			"Error fetching run params from Process Manager");
		exit(1);
	}

    // Set the debug file.
    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGFILE))
    {
        TA_Base_Core::DebugUtil::getInstance().setFile(TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE).c_str());
    }

    // Set the debug level
    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGLEVEL))
    {
        TA_Base_Core::DebugUtil::getInstance().setLevel(
            DebugUtil::getDebugLevelFromString(
                TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL).c_str()));
    }
    else
    {
        // Use the default
        TA_Base_Core::DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);
    }

    _Module.dwThreadID = GetCurrentThreadId();

    // HRESULT hr = CoInitialize(NULL);

//  If you are running on NT 4.0 or higher you can use the following call
//  instead to make the EXE free threaded.
//  This means that calls come in on a random RPC thread
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    _ASSERTE(SUCCEEDED(hr));

    // This provides a NULL DACL which will allow access to everyone.
    CSecurityDescriptor sd;
    sd.InitializeFromThreadToken();
    hr = CoInitializeSecurity(sd, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    _ASSERTE(SUCCEEDED(hr));

    hr = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER, REGCLS_MULTIPLEUSE);
    _ASSERTE(SUCCEEDED(hr));

    LogEvent(_T("Service started"));
    if (m_bService)
        SetServiceStatus(SERVICE_RUNNING);

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
        DispatchMessage(&msg);

    _Module.RevokeClassObjects();

    CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
//
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, 
    HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
    DebugUtil::getInstance().setFile("c:\\transactive\\logs\\Log_SoundManager.log");
    DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugDebug);

    lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
    _Module.Init(ObjectMap, hInstance, IDS_SERVICENAME, &LIBID_SOUNDMANAGERLib);
    _Module.m_bService = TRUE;

	std::string cmdLine = lpCmdLine;
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launched with a command line of: %s", cmdLine.c_str());

	// Find the hostnames on the command line.
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(" ");
	tokenizer tokens(cmdLine, sep);
	tokenizer::iterator iter;

	for (iter = tokens.begin(); iter != tokens.end(); ++iter)
	{
		// Find the position of the = sign.
		std::string::size_type position = iter->find("=");

		if (position != std::string::npos)
		{
			std::string argName = iter->substr(0, position);
			std::string argValue = iter->substr(position + 1, iter->length() - position);
			if (lstrcmpi(argName.c_str(), _T("--host-names")) == 0)
			{
				hostNames = argValue;
			}
		}
	}

	// Parse the command line.
	for (iter = tokens.begin(); iter != tokens.end(); ++iter)
	{
		// Check if we should unregister the server.
        if (lstrcmpi((*iter).c_str(), _T("--UnregServer")) == 0)
		{
            return _Module.UnregisterServer();
		}

	    // Check if we should register the server.
        if (lstrcmpi((*iter).c_str(), _T("--RegServer")) == 0)
		{
			if (hostNames.empty())
			{
				// Hostnames command line argument not found. Log and exit.
				LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "--host-names command line argument not found");
				exit(1);
			}

            HRESULT hr = _Module.RegisterServer(TRUE, FALSE);
            if (hr == 0)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Registered successfully.");
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error registering server. Error code: %lu", hr);
            }
           
            return hr;
        }
	}

    // Are we a Service or a Local Server
    CRegKey keyAppID;
    LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_READ);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    CRegKey key;
    lRes = key.Open(keyAppID, _T("{833DEE1C-D5FC-4868-9C8B-07C22F54A216}"), KEY_READ);
    if (lRes != ERROR_SUCCESS)
        return lRes;

    TCHAR szValue[_MAX_PATH];
    DWORD dwLen = _MAX_PATH;
    lRes = key.QueryValue(szValue, _T("LocalService"), &dwLen);

    _Module.m_bService = FALSE;
    if (lRes == ERROR_SUCCESS)
        _Module.m_bService = TRUE;

    _Module.Start();

    // When we get here, the service has been stopped
    return _Module.m_status.dwWin32ExitCode;
}

