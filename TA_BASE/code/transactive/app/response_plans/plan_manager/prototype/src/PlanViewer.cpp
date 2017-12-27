/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanViewer.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PlanViewer.h"
#include "PlanViewerDlg.h"
#include "Notes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerApp

BEGIN_MESSAGE_MAP(CPlanViewerApp, CWinApp)
    // {{AFX_MSG_MAP(CPlanViewerApp)
    // }}AFX_MSG_MAP
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerApp construction

CPlanViewerApp::CPlanViewerApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPlanViewerApp object

CPlanViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerApp initialization

BOOL CPlanViewerApp::InitInstance()
{
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();            // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();    // Call this when linking to MFC statically
#endif

    int resp = IDNO; //AfxMessageBox("Do you want design notes to be visible?", MB_ICONQUESTION | MB_YESNO);
    CNotes::MakeVisible(resp == IDYES);

    CPlanViewerDlg dlg;
    m_pMainWnd = &dlg;

    dlg.DoModal();

    // Since the dialog has been closed, return FALSE so that we exit the
    // application, rather than start the application's message pump.
    return FALSE;
}

BOOL CPlanViewerApp::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class

    return CWinApp::PreTranslateMessage(pMsg);
}

void CPlanViewerApp::WinHelp(DWORD dwData, UINT nCmd)
{
    // TODO: Add your specialized code here and/or call the base class
}

int CPlanViewerApp::Run()
{
    // TODO: Add your specialized code here and/or call the base class

    return CWinApp::Run();
}

