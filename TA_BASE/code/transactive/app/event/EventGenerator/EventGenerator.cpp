// EventGenerator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EventGenerator.h"
#include "EventGeneratorDlg.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEventGeneratorApp

BEGIN_MESSAGE_MAP(CEventGeneratorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEventGeneratorApp construction

CEventGeneratorApp::CEventGeneratorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CEventGeneratorApp object

CEventGeneratorApp theApp;


// CEventGeneratorApp initialization

BOOL CEventGeneratorApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	TA_Base_Core::RunParams::getInstance().parseCmdLine(m_lpCmdLine);
	TA_Base_Core::RunParams::getInstance().set( RPARAM_LOCATIONKEY, "1" );

	CEventGeneratorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		TA_Base_Core::CorbaUtil::getInstance().shutdown();
	}
	else if (nResponse == IDCANCEL)
	{
		TA_Base_Core::CorbaUtil::getInstance().shutdown();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
