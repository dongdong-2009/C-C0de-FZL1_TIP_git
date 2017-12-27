// security_simulator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "security_simulator.h"
#include "security_simulatorDlg.h"

#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/process_management/src/UtilityInitialiser.h"


#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"

#include <iostream>


using TA_Base_Core::RunParams;
using TA_Base_Core::CorbaUtil;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecurity_simulatorApp

BEGIN_MESSAGE_MAP(CSecurity_simulatorApp, CWinApp)
	//{{AFX_MSG_MAP(CSecurity_simulatorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecurity_simulatorApp construction

CSecurity_simulatorApp::CSecurity_simulatorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSecurity_simulatorApp object

CSecurity_simulatorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSecurity_simulatorApp initialization

BOOL CSecurity_simulatorApp::InitInstance()
{
	TA_Base_Core::UtilityInitialiser::initialiseUtilities(m_lpCmdLine);

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CSecurity_simulatorDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
