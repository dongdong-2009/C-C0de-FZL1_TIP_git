// Iscs_Alarm_Display.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Iscs_Alarm_Display.h"
#include "Iscs_Alarm_DisplayDlg.h"
 
#include <comutil.h>
#include <comdef.h>
 
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIscs_Alarm_DisplayApp

BEGIN_MESSAGE_MAP(CIscs_Alarm_DisplayApp, CWinApp)
	//{{AFX_MSG_MAP(CIscs_Alarm_DisplayApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIscs_Alarm_DisplayApp construction

CIscs_Alarm_DisplayApp::CIscs_Alarm_DisplayApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIscs_Alarm_DisplayApp object

CIscs_Alarm_DisplayApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIscs_Alarm_DisplayApp initialization

BOOL CIscs_Alarm_DisplayApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

   
//  	HRESULT hRes = CoInitialize(NULL);
//  	 
//  	CComPtr< IAlarmDetailInfo > ptrAlarm;
// 	HRESULT hr;
// 	// This time use CoCreateInstance
// 	hr = CoCreateInstance(__uuidof(AlarmDetailInfo),
// 		NULL,
// 		CLSCTX_LOCAL_SERVER,
// 		__uuidof(IAlarmDetailInfo),
// 		(void**) &ptrAlarm );
// 
//    BSTR b=_com_util::ConvertStringToBSTR("aaaab"); 
// 
//     ptrAlarm->put_AlarmValue(b);
// 
//    CComBSTR  bs;  
//    ptrAlarm->get_AlarmValue(&bs);

  


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CIscs_Alarm_DisplayDlg dlg;
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
