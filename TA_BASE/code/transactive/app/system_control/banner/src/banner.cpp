/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// banner.cpp : Defines the class behaviors for the application.
//

#include "app/system_control/banner/src/stdafx.h"
#include "app/system_control/banner/src/banner.h"
#include "app/system_control/banner/src/bannerGUI.h"
#include "app/system_control/banner/src/BannerInterface.h"
#include "app/system_control/banner/src/IscsBanner.h"
#include "app/system_control/banner/src/BannerInterface_i.c"
#include "bus/banner_framework/src/Constants.h"
#include <initguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

/////////////////////////////////////////////////////////////////////////////
// BannerApp

BEGIN_MESSAGE_MAP(BannerApp, TransActiveWinApp)
	//{{AFX_MSG_MAP(BannerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only BannerApp object

BannerApp theApp;


/////////////////////////////////////////////////////////////////////////////
// BannerApp construction

BannerApp::BannerApp()
: TransActiveWinApp( new TA_Base_App::BannerGUI(), "Control Station Banner" )
{
	// Place all significant initialization in InitInstance
}


BOOL BannerApp::InitInstance()
{
 	if (!InitATL())
	{
	//if ( !AfxOleInit() )	// ly++, vs2005 init OLE object
		return FALSE;
	}
    return TransActiveWinApp::InitInstance();
}


CBannerInterfaceModule _Module;


BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_IscsBanner, CIscsBanner)
END_OBJECT_MAP()


LONG CBannerInterfaceModule::Unlock()
{
	AfxOleUnlockApp();
	return 0;
}


LONG CBannerInterfaceModule::Lock()
{
	AfxOleLockApp();
	return 1;
}


BOOL BannerApp::InitATL()
{
	m_bATLInited = TRUE;

#if _WIN32_WINNT >= 0x0400
	// changed from multithread inorder to avoid application stop responding based on article
	// http://support.microsoft.com/default.aspx?scid=kb;en-us;828643

	HRESULT hRes = CoInitializeEx(NULL,COINIT_APARTMENTTHREADED); 
#else
	HRESULT hRes = CoInitialize(NULL);
#endif

	if (FAILED(hRes))
	{
		m_bATLInited = FALSE;
		return FALSE;
	}

	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	_Module.dwThreadID = GetCurrentThreadId();

    std::string lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT

	BOOL bRun = TRUE;
    if (lpCmdLine.find(BANNER_CONST::UNREGISTER_CMD) != std::string::npos)
	{
		_Module.UpdateRegistryFromResource(IDR_BANNERLIB, FALSE);
		_Module.UnregisterServer(TRUE); //TRUE means typelib is unreg'd
		bRun = FALSE;
	}
    if (lpCmdLine.find(BANNER_CONST::REGISTER_CMD) != std::string::npos)
	{
		_Module.UpdateRegistryFromResource(IDR_BANNERLIB, TRUE);
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
