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
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/PlanNodeView.h"
#include "app/response_plans/plan_manager/src/PlanNode.h"
#include "app/response_plans/plan_manager/src/PlanBrowserDoc.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/MessageBox.h"
#include "app/response_plans/plan_manager/src/LabelMaker.h"
#include "app/response_plans/plan_manager/src/ViewUpdate.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanNodeView

IMPLEMENT_DYNCREATE(CPlanNodeView, CScrollView)

CPlanNodeView::CPlanNodeView()
{
    FUNCTION_ENTRY( "CPlanNodeView" );
    FUNCTION_EXIT;
}


CPlanNodeView::~CPlanNodeView()
{
    FUNCTION_ENTRY( "~CPlanNodeView" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanNodeView, CScrollView)
    // {{AFX_MSG_MAP(CPlanNodeView)
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_SELECT_ACTIVE_PLAN_INSTANCE, OnSelectActivePlanInstance)
    ON_MESSAGE(WM_RESET_TREE_NODE_VIEW, OnResetTreeNodeView)
    ON_MESSAGE(WM_REFRESH_PLAN_STEP_LIST, OnRefreshPlanStepList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanNodeView diagnostics

#ifdef _DEBUG
void CPlanNodeView::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CScrollView::AssertValid();

    FUNCTION_EXIT;
}


void CPlanNodeView::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CScrollView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanNodeView message handlers

void CPlanNodeView::OnDraw(CDC* pDC)
{
    FUNCTION_ENTRY( "OnDraw" );
    FUNCTION_EXIT;
}


BOOL CPlanNodeView::OnEraseBkgnd(CDC* pDC)
{
    FUNCTION_ENTRY( "OnEraseBkgnd" );

    // TODO: Add your message handler code here and/or call default

    CBrush backBrush;
    backBrush.CreateSysColorBrush(COLOR_BTNFACE);

    CBrush* oldBrush = pDC->SelectObject(&backBrush);

    CRect clipBox;
    pDC->GetClipBox(clipBox);
    pDC->PatBlt(clipBox.left, clipBox.top, clipBox.Width(), clipBox.Height(), PATCOPY);
    pDC->SelectObject(oldBrush);

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanNodeView::OnInitialUpdate()
{
    FUNCTION_ENTRY( "OnInitialUpdate" );

    SetScrollSizes(MM_TEXT, CSize(10, 10));
    CScrollView::OnInitialUpdate();

    m_planPropertySheet.AddPage(&m_stepDetailPage);
    m_planPropertySheet.AddPage(&m_generalDetailPage);
    //m_planPropertySheet.AddPage(&m_flowchartPage);

    // Creating a modeless property sheet within the plan view causes the mouse cursor to jump
    // to a random position (???). Work around this by saving the cursor before creating the
    // property sheet and restoring it afterwards.
    CPoint cursorPos;
    ::GetCursorPos(&cursorPos);

    m_planPropertySheet.Create(this, WS_CHILD | WS_VISIBLE, WS_EX_RIGHTSCROLLBAR);

    ApplicationFont::getInstance().adjustChildrenFont( m_planPropertySheet.GetTabControl()->GetSafeHwnd() );

    ::SetCursorPos(cursorPos.x, cursorPos.y);

    FUNCTION_EXIT;
}


void CPlanNodeView::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CScrollView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (::IsWindow(m_planPropertySheet.GetSafeHwnd()) && m_planPropertySheet.GetActivePage() != NULL)
    {
        CSize totalSize = GetTotalSize();
        int rightLimit = (cx > totalSize.cx)? cx : totalSize.cx;
        int bottomLimit = (cy > totalSize.cy)? cy : totalSize.cy;

        CRect propSheetRect;
        m_planPropertySheet.GetWindowRect(propSheetRect);
        ScreenToClient(propSheetRect);
        propSheetRect.left = propSheetRect.top = 0;
        propSheetRect.right = rightLimit;
        propSheetRect.bottom = bottomLimit;
        m_planPropertySheet.MoveWindow(propSheetRect);

        CRect tabCtrlRect;
        m_planPropertySheet.GetTabControl()->GetWindowRect(tabCtrlRect);
        ScreenToClient(tabCtrlRect);
        tabCtrlRect.right = propSheetRect.right - 5;
        tabCtrlRect.bottom = propSheetRect.bottom - 5;
        m_planPropertySheet.GetTabControl()->MoveWindow(tabCtrlRect);

        m_planPropertySheet.GetActivePage()->SendMessage(WM_ADJUST_CONTROL_POSITIONS);
    }

    FUNCTION_EXIT;
}


void CPlanNodeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    FUNCTION_ENTRY( "OnUpdate" );

    // TODO: Add your specialized code here and/or call the base class
    if (lHint == ID_RPARAM_SESSIONID_UPDATE)
    {
        // A change of session ID may involve a change of access rights.
        // Adjust plan filter buttons accordingly.
        CPropertyPage *activePage = m_planPropertySheet.GetActivePage();
        if (activePage)
        {
            activePage->SendMessage(WM_ADJUST_PLAN_FILTER_BUTTON_STATES);
        }

        FUNCTION_EXIT;
        return;
    }

    CViewUpdate<TA_Base_Core::ActivePlanDetail> *apdUpdate = dynamic_cast<CViewUpdate<TA_Base_Core::ActivePlanDetail> *>(pHint);
    if (apdUpdate)
    {
        TA_Base_Core::ActivePlanDetail activePlanDetail = apdUpdate->GetUpdateData();

        if (activePlanDetail.plan.plan == GetPlanNode()->getNodeId())
        {
            SendMessageToDescendants(WM_ACTIVE_PLAN_DETAIL_UPDATE, (WPARAM) &activePlanDetail);
        }

        FUNCTION_EXIT;
        return;
    }

    CViewUpdate<TA_Base_Core::PlanConfigUpdateData> *pcdUpdate = dynamic_cast<CViewUpdate<TA_Base_Core::PlanConfigUpdateData> *>(pHint);
    if (pcdUpdate)
    {
        TA_Base_Core::PlanConfigUpdateData planConfigData = pcdUpdate->GetUpdateData();

        switch (planConfigData.configUpdate._d())
        {
            case TA_Base_Core::PCUT_APPROVAL_STATE:
            {
                if (planConfigData.configUpdate.approvalState().plan == GetPlanNode()->getNodeId())
                {
                    SendMessage(WM_RESET_TREE_NODE_VIEW);
                }

                break;
            }

            case TA_Base_Core::PCUT_UPDATE_PLAN:
            {
                if (planConfigData.configUpdate.updatePlan().node == GetPlanNode()->getNodeId())
                {
                    SendMessage(WM_RESET_TREE_NODE_VIEW);
                }

                break;
            }
        }

        FUNCTION_EXIT;
        return;
    }

    CViewUpdate<TA_Base_Core::PlanCustomisationUpdate> *pcuUpdate = dynamic_cast<CViewUpdate<TA_Base_Core::PlanCustomisationUpdate> *>(pHint);
    if (pcuUpdate)
    {
        TA_Base_Core::PlanCustomisationUpdate planCustomDetail = pcuUpdate->GetUpdateData();

        if (planCustomDetail.plan.plan == GetPlanNode()->getNodeId())
        {
            SendMessageToDescendants(WM_PLAN_CUSTOMISATION_UPDATE, (WPARAM) &planCustomDetail);
        }

        FUNCTION_EXIT;
        return;
    }

    FUNCTION_EXIT;
}


LRESULT CPlanNodeView::OnSelectActivePlanInstance(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnSelectActivePlanInstance" );

    if (m_planPropertySheet.GetActivePage() != &m_stepDetailPage)
    {
        m_planPropertySheet.SetActivePage(&m_stepDetailPage);
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlanNodeView::OnResetTreeNodeView(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnResetTreeNodeView" );

    SendMessageToDescendants(WM_RESET_TREE_NODE_VIEW);

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlanNodeView::OnRefreshPlanStepList(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnRefreshPlanStepList" );

    SendMessageToDescendants(WM_REFRESH_PLAN_STEP_LIST, wParam, lParam);

    FUNCTION_EXIT;
    return 0;
}


CPlanBrowserDoc *CPlanNodeView::GetDocument()
{
    FUNCTION_ENTRY( "GetDocument" );

    CPlanBrowserDoc *planBrowserDoc = dynamic_cast<CPlanBrowserDoc *>(CScrollView::GetDocument());
    TA_ASSERT(planBrowserDoc != NULL, "Active document is not a CPlanBrowserDoc");

    FUNCTION_EXIT;
    return planBrowserDoc;
}


PlanNode *CPlanNodeView::GetPlanNode()
{
    FUNCTION_ENTRY( "GetPlanNode" );

    PlanNode *planNode = dynamic_cast<PlanNode *>(GetDocument()->GetCurrentNode());
    TA_ASSERT(planNode != NULL, "Plan node pointer is null");

    FUNCTION_EXIT;
    return planNode;
}


