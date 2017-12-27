/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/MainFrm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PlanViewer.h"

#include "MainFrm.h"
#include "PlansTreeView.h"
#include "BlankForm.h"
#include "PlanViewerDlg.h"
#include "PvCommonDefs.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    // {{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    // }}AFX_MSG_MAP
    // ON_MESSAGE(MF_START_BUSINESS, OnStartBusiness)
END_MESSAGE_MAP()

static UINT BASED_CODE indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    // ID_INDICATOR_CAPS,
    // ID_INDICATOR_NUM,
    // ID_INDICATOR_SCRL
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    FUNCTION_ENTRY( "CMainFrame" );

    // TODO: add member initialization code here
    m_bAutoMenuEnable = false;

    FUNCTION_EXIT;
}


CMainFrame::~CMainFrame()
{
    FUNCTION_ENTRY( "~CMainFrame" );
    FUNCTION_EXIT;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }


    if(!CreateToolBars())
    {
        FUNCTION_EXIT;
        return -1;
    }


    if(!PopulateToolBars())
    {
        FUNCTION_EXIT;
        return -1;
    }


    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");

        FUNCTION_EXIT;
        return -1;      // fail to create
    }

    UINT paneId, paneStyle;
    int paneWidth;
    m_wndStatusBar.GetPaneInfo (0, paneId, paneStyle, paneWidth);
    m_wndStatusBar.SetPaneInfo(0, paneId, SBPS_STRETCH, 250);

    m_pSplashScreen = (CPlanViewerDlg*) AfxGetApp()->m_pMainWnd;

    FUNCTION_EXIT;
    return 0;
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
    CCreateContext* pContext)
{
    FUNCTION_ENTRY( "OnCreateClient" );

    // create splitter window
    if (!m_wndSplitter.CreateStatic(this, 1, 2))
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CPlansTreeView), CSize(250, 250), pContext) ||
        !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CBlankForm), CSize(250, 250), pContext))
    {
        m_wndSplitter.DestroyWindow();

        FUNCTION_EXIT;
        return FALSE;
    }

    AfxGetApp()->m_pMainWnd = this;

    FUNCTION_EXIT;
    return TRUE;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    if( !CFrameWnd::PreCreateWindow(cs) )
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    RECT rect;
    if (::GetWindowRect(::GetDesktopWindow(), &rect))
    {
        cs.cx = rect.right - rect.left - 100;
        cs.cy = rect.bottom - rect.top - 100;
    }
    else
    {
        cs.cx = 800;
        cs.cy = 600;
    }

    cs.style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SIZEBOX | WS_BORDER;

    CDialog* pWnd = (CDialog*) AfxGetApp()->m_pMainWnd;

    FUNCTION_EXIT;
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFrameWnd::AssertValid();

    FUNCTION_EXIT;
}


void CMainFrame::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFrameWnd::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

CWnd* CMainFrame::GetRightPane()
{
    FUNCTION_ENTRY( "GetRightPane" );

    CWnd* pWnd = m_wndSplitter.GetPane(0, 1);

    FUNCTION_EXIT;
    return pWnd;
}


void CMainFrame::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    CFrameWnd::OnDestroy();

    AfxGetApp()->m_pMainWnd = NULL;

    if (m_pSplashScreen != NULL)
    {
        m_pSplashScreen->EndDialog(IDCANCEL);
    }

    FUNCTION_EXIT;
}


BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    FUNCTION_ENTRY( "OnCmdMsg" );

    // Pass to splitter
    CWnd* pWnd = (CWnd*)m_wndSplitter.GetPane(0, 0);
    if(pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
    {
        FUNCTION_EXIT;
        return true;
    }

    pWnd = (CWnd*)m_wndSplitter.GetPane(0, 1);
    if(pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
    {
        FUNCTION_EXIT;
        return true;
    }


    // Call default routing--very important!
    FUNCTION_EXIT;
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


bool CMainFrame::CreateToolBars()
{
    FUNCTION_ENTRY( "CreateToolBars" );

    UINT        tbarStyle1, tbarStyle2 = 0;

    tbarStyle1 = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
    tbarStyle2 =  WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_TOP | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

    if (!m_wndToolBar.CreateEx(this, tbarStyle1, tbarStyle2))
    {
        TRACE0("Failed to create toolbar\n");

        FUNCTION_EXIT;
        return false;      // fail to create
    }

    m_wndToolBar.SetSizes(CSize(100, 44), CSize(24,24));

    m_toolBarImageListHot.Create(24,24,ILC_MASK, 5, 5);
    m_wndToolBar.GetToolBarCtrl().SetImageList(&m_toolBarImageListHot);

    FUNCTION_EXIT;
    return true;
}


bool CMainFrame::PopulateToolBars()
{
    FUNCTION_ENTRY( "PopulateToolBars" );

    CWinApp* pApp = AfxGetApp();
    TBBUTTON tbButton;

    // Add New Category Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("New Category")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_NEWCAT_ICON));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_CAT_NEW;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Delete Category Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Delete Category")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_DELCAT_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_CAT_DELETE;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Edit Category Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Edit Category")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_EDITCAT_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_CAT_EDIT;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Space
    tbButton.iBitmap = NULL;
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_SEP;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_SEP_CAT;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add New Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("New Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_NEWPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_NEW;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Delete Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Delete Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_DELPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_DELETE;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Cut Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Cut Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_CUTPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_CUT;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Copy Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Copy Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_COPYPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_COPY;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Paste Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Paste Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_PASTEPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_PASTE;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Edit Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Edit Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_EDITPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_EDIT;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Space
    tbButton.iBitmap = NULL;
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_SEP;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_SEP_PLAN1;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Approve Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Approve Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_APPPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_APPROVE;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Unapprove Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Unapprove Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_UNAPPPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_UNAPPROVE;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Space
    tbButton.iBitmap = NULL;
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_SEP;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_SEP_PLAN2;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Schedule Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Schedule Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_SCHEDULEPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_SCHEDULE;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    // Add Control Plan Button
    VERIFY((tbButton.iString = m_wndToolBar.GetToolBarCtrl().AddStrings("Control Plan")) != -1);
    tbButton.iBitmap = m_toolBarImageListHot.Add(pApp->LoadIcon(IDI_EXECPLAN_ICON));
    tbButton.fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = ID_PLAN_CONTROL;
    VERIFY(m_wndToolBar.GetToolBarCtrl().AddButtons(1, &tbButton));

    FUNCTION_EXIT;
    return true;
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CFrameWnd::OnSize(nType, cx, cy);

    FUNCTION_EXIT;
}


CPlansTreeView* CMainFrame::GetTree()
{
    FUNCTION_ENTRY( "GetTree" );
    FUNCTION_EXIT;
    return (CPlansTreeView*) m_wndSplitter.GetPane(0, 0);
}


