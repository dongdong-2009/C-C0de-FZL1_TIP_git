//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/HelpLauncher.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/MainFrame.h"
#include "app/response_plans/plan_manager/src/AllPlanTreeFrame.h"
#include "app/response_plans/plan_manager/src/ApprovedPlanTreeFrame.h"
#include "app/response_plans/plan_manager/src/UnapprovedPlanTreeFrame.h"
#include "app/response_plans/plan_manager/src/ActivePlanFrame.h"
#include "app/response_plans/plan_manager/src/PlanTreeDoc.h"
#include "app/response_plans/plan_manager/src/TreeNode.h"
#include "app/response_plans/plan_manager/src/PlanNode.h"
#include "app/response_plans/plan_manager/src/CategoryNode.h"
#include "app/response_plans/plan_manager/src/TreeNodeFactory.h"
#include "app/response_plans/plan_manager/src/PlanManagerCorbaDefImpl.h"
#include "app/response_plans/plan_manager/src/InteractiveStep.h"
#include "app/response_plans/plan_manager/src/MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
#define TA_MSG_INIT_CORBA (WM_USER + 1001)

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, TransActiveMDIFrame)

BEGIN_MESSAGE_MAP(CMainFrame, TransActiveMDIFrame)
    // {{AFX_MSG_MAP(CMainFrame)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateToolbarView)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_ALLPLANS, ID_VIEW_ACTIVEPLANS, OnUpdatePlanView)
    ON_COMMAND_RANGE(ID_VIEW_ALLPLANS, ID_VIEW_ACTIVEPLANS, OnViewPlan)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SHOWWINDOW()
    ON_MESSAGE(WM_TREE_NODE_SELECTION_CHANGED, OnTreeNodeSelectionChanged)
    ON_MESSAGE(WM_CATEGORY_VIEW_SELECTION_CHANGED, OnCategoryViewSelectionChanged)
    ON_MESSAGE(WM_SESSION_ID_CHANGED, OnSessionIdChanged)
    ON_MESSAGE(TA_MSG_INIT_CORBA, onInitCorba)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame(TA_Base_Bus::AbstractGUIApplication* appGUI) :TransActiveMDIFrame(), m_planManagerServant(NULL)
{
    FUNCTION_ENTRY( "CMainFrame" );

    TA_ASSERT(appGUI != NULL, "the IGUIApplication is NULL!");
    // TODO: add member initialization code here
    m_appGUI = appGUI;

    FUNCTION_EXIT;
}


CMainFrame::CMainFrame() : m_planManagerServant(NULL)
{
    FUNCTION_ENTRY( "CMainFrame" );
    FUNCTION_EXIT;
}


CMainFrame::~CMainFrame()
{
    FUNCTION_ENTRY( "~CMainFrame" );

    m_planManagerServant->deactivateAndDeleteServant();
    m_planManagerServant = NULL;

    m_planBrowserFrameMap.clear();

    exit(0);

    FUNCTION_EXIT;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (TransActiveMDIFrame::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }

    if (!m_statusBar.Create(this) ||
        !m_statusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");

        FUNCTION_EXIT;
        return -1;      // fail to create
    }

    m_toolBarManager.initialise(this);

    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = 0;
    properties.minWidth = 0;
    setResizingProperties(properties);

    CWaitCursor cus;
    if(m_progress.m_hWnd == NULL)
    {
        if (!m_progress.Create(
            WS_CHILD|WS_VISIBLE|WS_TABSTOP|PBS_SMOOTH, //
            CRect(0,0,0,0),
            &m_statusBar,
            0))
        {
            TRACE(_T("Failed to create progress control.\n"));
            cus.Restore();

            FUNCTION_EXIT;
            return -1;
        }

        m_progress.ModifyStyleEx(WS_EX_STATICEDGE,0,SWP_FRAMECHANGED);

        m_progress.SetRange(0,100);
        m_progress.SetPos(0);
        m_progress.SetStep(1);

        BOOL bRet = m_statusBar.AddPane(ID_INDICATOR_PROGRESS,0);
        if(!bRet)
        {
            AfxMessageBox(_T("Pane index out of range\nor pane with same ID already exists in the status bar"), MB_ICONERROR);
            cus.Restore();

            FUNCTION_EXIT;
            return -1;
        }
        int nIndex = m_statusBar.CommandToIndex(ID_INDICATOR_PROGRESS);
        m_statusBar.SetPaneWidth(nIndex, 800);
        m_statusBar.AddPaneControl(&m_progress, ID_INDICATOR_PROGRESS, true);
        m_statusBar.SetPaneText(2, "Initializing Paramerters...");
    }
    cus.Restore();

    FUNCTION_EXIT;
    return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    if( !TransActiveMDIFrame::PreCreateWindow(cs) )
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    // Append active document name to the window title.
    cs.style &= ~FWS_PREFIXTITLE;
    cs.style &= ~FWS_ADDTOTITLE;

    // The initial size should eventually be set by the control station...
    /*
    cs.cx = 1920;
    cs.cy = 839;
    */

    FUNCTION_EXIT;
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    TransActiveMDIFrame::AssertValid();

    FUNCTION_EXIT;
}


void CMainFrame::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    TransActiveMDIFrame::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

LRESULT CMainFrame::OnTreeNodeSelectionChanged(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnTreeNodeSelectionChanged" );

    CPlanBrowserFrame *planBrowserFrame = static_cast<CPlanBrowserFrame *>(GetActiveFrame());
    CPlanTreeDoc *planTreeDoc = static_cast<CPlanTreeDoc *>(planBrowserFrame->GetActiveDocument());

    TreeNode *treeNode = planTreeDoc->GetCurrentNode();

    if (dynamic_cast<PlanNode *>(treeNode))
    {
        m_toolBarManager.selectToolBar(MainToolBarManager::TBT_PLAN);
    }
    else if (dynamic_cast<CategoryNode *>(treeNode))
    {
        m_toolBarManager.selectToolBar(MainToolBarManager::TBT_CATEGORY);
    }
    else
    {
        m_toolBarManager.selectToolBar(MainToolBarManager::TBT_NONE);
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT CMainFrame::OnCategoryViewSelectionChanged(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnCategoryViewSelectionChanged" );

    CPlanBrowserFrame *planBrowserFrame = static_cast<CPlanBrowserFrame *>(GetActiveFrame());
    CPlanTreeDoc *planTreeDoc = static_cast<CPlanTreeDoc *>(planBrowserFrame->GetActiveDocument());

    if (planTreeDoc->ArePlanNodesSelected())
    {
        m_toolBarManager.selectToolBar(MainToolBarManager::TBT_PLAN);
    }
    else
    {
        m_toolBarManager.selectToolBar(MainToolBarManager::TBT_CATEGORY);
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT CMainFrame::OnSessionIdChanged(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnSessionIdChanged" );

    // TD10849 Re-register for interactive steps
    // Bring the main window (this) to the front because there may be
    // messageboxes being displayed when PlanManagerCorbaDefImpl is
    // constructed, and the messageboxes are children of the main window.
    HWND oldForegroundWnd = ::GetForegroundWindow();
    ::SetForegroundWindow(GetSafeHwnd());

    delete m_planManagerServant;
    m_planManagerServant = new PlanManagerCorbaDefImpl();
    int iSize = m_planManagerServant->toActivate();
    CString strError;
    for (int i = 0; i < iSize; i++)
    {
        std::string strLocation;
        m_planManagerServant->init(i, strLocation);
        strError += strLocation.c_str();
        strError += " ";
    }
    strError.TrimRight(" ");
    if (!strError.IsEmpty())
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not get the following plan agents: [%s]", (LPCTSTR)strError);
    }
    // TD11365: Make sure the old window still exists before bringing it
    // to the foreground again.
    if (NULL != oldForegroundWnd && ::IsWindow(oldForegroundWnd))
    {
        ::SetForegroundWindow(oldForegroundWnd);
    }

    FUNCTION_EXIT;
    return 0;
}


void CMainFrame::OnAppAbout()
{
    FUNCTION_ENTRY( "OnAppAbout" );

    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

    FUNCTION_EXIT;
}


void CMainFrame::OnViewToolbar()
{
    FUNCTION_ENTRY( "OnViewToolbar" );

    // TODO: Add your command handler code here
    m_toolBarManager.toggleToolBarDisplay();

    FUNCTION_EXIT;
}


void CMainFrame::OnViewPlan(UINT nCommandID)
{
    FUNCTION_ENTRY( "OnViewPlan" );

    ASSERT(nCommandID >= ID_VIEW_ALLPLANS && nCommandID <= ID_VIEW_ACTIVEPLANS);

    CFrameWnd *activePlanBrowserFrame = GetActiveFrame();
    CPlanBrowserFrame *requiredPlanBrowserFrame = GetPlanBrowserFrame(nCommandID);
    TA_ASSERT(requiredPlanBrowserFrame, "No plan browser frame to be activated");

    if (requiredPlanBrowserFrame->GetSafeHwnd() != activePlanBrowserFrame->GetSafeHwnd())
    {
        if (nCommandID == ID_VIEW_ACTIVEPLANS)
        {
            m_toolBarManager.hideToolBar();
        }

        requiredPlanBrowserFrame->ActivateFrame(0);

        if (activePlanBrowserFrame && activePlanBrowserFrame->GetSafeHwnd() != GetSafeHwnd()) // don't hide the main frame
        {
            activePlanBrowserFrame->ShowWindow(SW_HIDE);
        }
    }

    std::string strNewTitle = m_strBaseTitle;

    switch (nCommandID)
    {
    case ID_VIEW_ALLPLANS:
        strNewTitle += " - 所有预案";
        break;

    case ID_VIEW_APPROVEDPLANS:
        strNewTitle += " - 批准预案";
        break;

    case ID_VIEW_UNAPPROVEDPLANS:
        strNewTitle += " - 未批准预案";
        break;

    case ID_VIEW_ACTIVEPLANS:
        strNewTitle += " - 激活预案";
        break;

    default:
        break;
    }

    SetMainTitle( strNewTitle.c_str() );

    FUNCTION_EXIT;
}


void CMainFrame::OnUpdateToolbarView(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdateToolbarView" );

    // TODO: Add your command update UI handler code here
    m_toolBarManager.updateCmdUI(*pCmdUI);

    FUNCTION_EXIT;
}


void CMainFrame::OnUpdatePlanView(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdatePlanView" );

    // TODO: customize or extend this code to handle choices on the
    // View menu.

    // Use a dot to indicate the currently selected plan view

    switch (pCmdUI->m_nID)
    {
        case ID_VIEW_ALLPLANS:
            pCmdUI->SetRadio(dynamic_cast<CAllPlanTreeFrame *>(GetActiveFrame()) != NULL);
            break;

        case ID_VIEW_APPROVEDPLANS:
            pCmdUI->SetRadio(dynamic_cast<CApprovedPlanTreeFrame *>(GetActiveFrame()) != NULL);
            break;

        case ID_VIEW_UNAPPROVEDPLANS:
            pCmdUI->SetRadio(dynamic_cast<CUnapprovedPlanTreeFrame *>(GetActiveFrame()) != NULL);
            break;

        case ID_VIEW_ACTIVEPLANS:
            pCmdUI->SetRadio(dynamic_cast<CActivePlanFrame *>(GetActiveFrame()) != NULL);
            break;

        default:
            pCmdUI->SetRadio(FALSE);
    }

    FUNCTION_EXIT;
}


void CMainFrame::AddPlanBrowserFrame(int planViewCommandID, CPlanBrowserFrame *planBrowserFrame)
{
    FUNCTION_ENTRY( "AddPlanBrowserFrame" );

    m_planBrowserFrameMap.insert(std::make_pair(planViewCommandID, planBrowserFrame));

    FUNCTION_EXIT;
}


CPlanBrowserFrame *CMainFrame::GetPlanBrowserFrame(int planViewCommandID)
{
    FUNCTION_ENTRY( "GetPlanBrowserFrame" );

    PlanBrowserFrameMap::iterator iter = m_planBrowserFrameMap.find(planViewCommandID);

    if (iter != m_planBrowserFrameMap.end())
    {
        FUNCTION_EXIT;
        return (CPlanBrowserFrame *) iter->second;
    }

    FUNCTION_EXIT;
    return NULL;
}


LRESULT CMainFrame::onInitCorba(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "onInitCorba" );

    CWaitCursor cus;

    // Activate the interactive step servant
    if (!m_planManagerServant)
    {
        m_planManagerServant = new PlanManagerCorbaDefImpl();
        int iSize = m_planManagerServant->toActivate();
        m_progress.SetPos(5);
        int interval = 95/iSize;
        CString strError;
        for (int i = 0; i < iSize; i++)
        {
            std::string strLocation;
            m_planManagerServant->init(i, strLocation);
            strError += strLocation.c_str();
            strError += " ";
            m_progress.SetPos(5 + interval*i);
            CString strPercent;
            strPercent.Format("Initializing Plans... %d", (5 + interval*i));
            strPercent += "%";
            m_statusBar.SetPaneText(1, strPercent);
        }
        m_progress.SetPos(99);
        m_statusBar.SetPaneText(1, "Initializing Plans... 99%");
        strError.TrimRight(" ");
        if (!strError.IsEmpty())
        {
            m_progress.SetPos(100);
            m_statusBar.SetPaneText(1, "Done");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not get the following plan agents: [%s]", (LPCTSTR)strError);
        }
    }
    cus.Restore();
    m_progress.SetPos(0);
	m_statusBar.SetPaneText(1, "准备");
    m_statusBar.RemovePane(m_statusBar.GetItemID(0));

    FUNCTION_EXIT;
    return 0;
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    TransActiveMDIFrame::OnSize(nType, cx, cy);

    FUNCTION_EXIT;
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    FUNCTION_ENTRY( "OnShowWindow" );

    TransActiveMDIFrame::OnShowWindow(bShow, nStatus);

    FUNCTION_EXIT;
}


void CMainFrame::initCorbaObj()
{
    FUNCTION_ENTRY( "initCorbaObj" );

    if (m_appGUI->getGenericGUIStartCompleted())
    {
        PostMessage(TA_MSG_INIT_CORBA);
    }

    FUNCTION_EXIT;
}


void CMainFrame::SetMainTitle( const std::string& strTitle )
{
    SetWindowText( strTitle.c_str() );
    m_strTitle = strTitle.c_str();
}


void TA_Base_App::CMainFrame::SetBaseTitle( const std::string& strTitle )
{
    m_strBaseTitle = strTitle.c_str();
    SetMainTitle( strTitle );
}
