/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanViewerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlanViewer.h"
#include "PlanViewerDlg.h"
#include "MainFrm.h"
#include "PlansTreeView.h"
#include "PlanViewerDoc.h"
#include "PlanCatEditMainFrm.h"
#include "PlanCatEditDoc.h"
#include "PlanCatForm.h"
#include "PlanEditMainFrm.h"
#include "PlanEditDoc.h"
#include "PlanControlMainFrm.h"
#include "PlanControlDoc.h"
#include "PlanForm.h"
#include "PlanStepEditMainFrm.h"
#include "PlanStepEditDoc.h"
#include "PlanStepForm.h"
#include "PvCommonDefs.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPlanViewerDlg dialog

CPlanViewerDlg::CPlanViewerDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CPlanViewerDlg::IDD, pParent)
{
    FUNCTION_ENTRY( "CPlanViewerDlg" );

    // {{AFX_DATA_INIT(CPlanViewerDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    FUNCTION_EXIT;
}


void CPlanViewerDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanViewerDlg)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanViewerDlg, CDialog)
    // {{AFX_MSG_MAP(CPlanViewerDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    // }}AFX_MSG_MAP
    ON_MESSAGE(DLG_START_BUSINESS, OnStartBusiness)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerDlg message handlers

BOOL CPlanViewerDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    // when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here
    PostMessage(DLG_START_BUSINESS);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlanViewerDlg::OnPaint()
{
    FUNCTION_ENTRY( "OnPaint" );

    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        ShowWindow(SW_HIDE);
        CDialog::OnPaint();
    }

    FUNCTION_EXIT;
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPlanViewerDlg::OnQueryDragIcon()
{
    FUNCTION_ENTRY( "OnQueryDragIcon" );
    FUNCTION_EXIT;
    return (HCURSOR) m_hIcon;
}


LRESULT CPlanViewerDlg::OnStartBusiness(WPARAM, LPARAM)
{
    FUNCTION_ENTRY( "OnStartBusiness" );

    LRESULT result = 0;

    // Main document
    AfxGetApp()->AddDocTemplate(new CSingleDocTemplate(IDR_MAINFRAME,
                                                       RUNTIME_CLASS(CPlanViewerDoc),
                                                       RUNTIME_CLASS(CMainFrame),
                                                       RUNTIME_CLASS(CPlansTreeView)));

    // For editing categories
    AfxGetApp()->AddDocTemplate(new CMultiDocTemplate(IDR_CATEDIT,
                                                      RUNTIME_CLASS(CPlanCatEditDoc),
                                                      RUNTIME_CLASS(CPlanCatEditMainFrame),
                                                      RUNTIME_CLASS(CPlanCatForm)));

    // For editing plans
    AfxGetApp()->AddDocTemplate(new CMultiDocTemplate(IDR_PLANEDIT,
                                                      RUNTIME_CLASS(CPlanEditDoc),
                                                      RUNTIME_CLASS(CPlanEditMainFrame),
                                                      RUNTIME_CLASS(CPlanForm)));

    // For executing plans
    AfxGetApp()->AddDocTemplate(new CMultiDocTemplate(IDR_PLANCTRL,
                                                      RUNTIME_CLASS(CPlanControlDoc),
                                                      RUNTIME_CLASS(CPlanControlMainFrame),
                                                      RUNTIME_CLASS(CPlanForm)));

    // For editing steps
    AfxGetApp()->AddDocTemplate(new CSingleDocTemplate(IDR_STEPEDIT,
                                                       RUNTIME_CLASS(CPlanStepEditDoc),
                                                       RUNTIME_CLASS(CPlanStepEditMainFrame),
                                                       RUNTIME_CLASS(CPlanStepForm)));

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    AfxGetApp()->ParseCommandLine(cmdInfo);

    // Dispatch commands specified on the command line
    if (!AfxGetApp()->ProcessShellCommand(cmdInfo))
    {
        FUNCTION_EXIT;
        return result;
    }

    FUNCTION_EXIT;
    return result;

}


void CPlanViewerDlg::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    CDialog::OnDestroy();

    CMainFrame* pWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;

    if(pWnd != NULL)
    {
        pWnd->m_pSplashScreen = NULL;
        ((CMainFrame*) pWnd)->DestroyWindow();
    }

    FUNCTION_EXIT;
}


// we override DoModal() (it is called by generic gui) to get a chance to
// handle all Plan Editor Manager exceptions at a single point...
int CPlanViewerDlg::DoModal()
{
    FUNCTION_ENTRY( "DoModal" );

    int ret = IDOK;
    try
    {
        // call base class
        ret = CDialog::DoModal();
    }
    catch (...)
    {
        ::MessageBox(AfxGetMainWnd()->GetSafeHwnd(),
                    "A fatal error occured. No additional information is available.",
                    "Plan Viewer - Fatal Error",
                    IDOK | MB_ICONERROR);
    }

    ::PostQuitMessage(-1);

    FUNCTION_EXIT;
    return ret;
}


