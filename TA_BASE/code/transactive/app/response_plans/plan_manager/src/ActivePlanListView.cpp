//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanListView.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Used for viewing all currently active plans.
//

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "PlanManager.h"
#include "ActivePlanListView.h"
#include "ActivePlanDoc.h"
#include "MessageBox.h"
#include "ViewUpdate.h"
#include "PlanNode.h"
#include "PlanAgentAccess.h"
#include "PlanManagerCommonDefs.h"
#include "OperatorRights.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CActivePlanListView

IMPLEMENT_DYNCREATE(CActivePlanListView, CFormView)

CActivePlanListView::CActivePlanListView() :
    CFormView(CActivePlanListView::IDD),
    m_planInstanceListCtrl(true /*plan path visible*/),
    m_initialised(false)
{
    FUNCTION_ENTRY( "CActivePlanListView" );

    // {{AFX_DATA_INIT(CActivePlanListView)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CActivePlanListView::~CActivePlanListView()
{
    FUNCTION_ENTRY( "~CActivePlanListView" );
    FUNCTION_EXIT;
}


void CActivePlanListView::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CFormView::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CActivePlanListView)
    DDX_Control(pDX, IDC_ALL_OWNERS_CHECK, m_allOwnersCheck);
    DDX_Control(pDX, IDC_ALL_LOCATIONS_CHECK, m_allLocationsCheck);
    DDX_Control(pDX, IDC_ACTIVE_INSTANCES_GRPBOX, m_activeInstancesGroupBox);
    DDX_Control(pDX, IDC_ACTIVE_INSTANCES_LIST, m_planInstanceListCtrl);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CActivePlanListView, CFormView)
    // {{AFX_MSG_MAP(CActivePlanListView)
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_ACTIVE_INSTANCES_LIST, OnDblclkActiveInstanceList)
    ON_BN_CLICKED(IDC_ALL_OWNERS_CHECK, OnAllOwnersCheck)
    ON_BN_CLICKED(IDC_ALL_LOCATIONS_CHECK, OnAllLocationsCheck)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_WINDOW_HAS_BECOME_VISIBLE, OnViewShown)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CActivePlanListView diagnostics

#ifdef _DEBUG
CActivePlanDoc* CActivePlanListView::GetDocument() // non-debug version is inline
{
    FUNCTION_ENTRY( "GetDocument" );

    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CActivePlanDoc)));

    FUNCTION_EXIT;
    return (CActivePlanDoc*)m_pDocument;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CActivePlanListView message handlers

void CActivePlanListView::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CFormView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (::IsWindow(m_activeInstancesGroupBox.GetSafeHwnd()))
    {
        CRect activeInstancesBoxRect;
        m_activeInstancesGroupBox.GetWindowRect(activeInstancesBoxRect);
        ScreenToClient(activeInstancesBoxRect);
        activeInstancesBoxRect.right = cx - 8;
        activeInstancesBoxRect.bottom = cy - 8;
        m_activeInstancesGroupBox.MoveWindow(activeInstancesBoxRect);

        CRect activeInstancesListRect;
        m_planInstanceListCtrl.GetWindowRect(activeInstancesListRect);
        ScreenToClient(activeInstancesListRect);
        activeInstancesListRect.right = activeInstancesBoxRect.right - 13;
        activeInstancesListRect.bottom = activeInstancesBoxRect.bottom - 13;
        m_planInstanceListCtrl.MoveWindow(activeInstancesListRect);
    }

    FUNCTION_EXIT;
}


void CActivePlanListView::OnAllOwnersCheck()
{
    FUNCTION_ENTRY( "OnAllOwnersCheck" );

    // TODO: Add your control notification handler code here
    PlanInstanceFilter *planInstanceFilter = GetDocument()->GetPlanInstanceFilter();
    planInstanceFilter->setAllOwnersNeeded(m_allOwnersCheck.GetCheck() == BST_CHECKED);

    if (planInstanceFilter->allOwnersNeeded())
    {
        m_planInstanceListCtrl.Refresh();
    }
    else
    {
        m_planInstanceListCtrl.Purge();
    }

    FUNCTION_EXIT;
}


void CActivePlanListView::OnAllLocationsCheck()
{
    FUNCTION_ENTRY( "OnAllLocationsCheck" );

    // TODO: Add your control notification handler code here
    PlanInstanceFilter *planInstanceFilter = GetDocument()->GetPlanInstanceFilter();
    planInstanceFilter->setAllLocationsNeeded(m_allLocationsCheck.GetCheck() == BST_CHECKED);

    if (planInstanceFilter->allLocationsNeeded())
    {
        m_planInstanceListCtrl.Refresh();
    }
    else
    {
        m_planInstanceListCtrl.Purge();
    }

    FUNCTION_EXIT;
}


void CActivePlanListView::OnDblclkActiveInstanceList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclkActiveInstanceList" );

    PlanNode *planNode = m_planInstanceListCtrl.GetSelectedPlan();
    if (planNode)
    {
        // The user may be locked to a specific category and should not be allowed to launch
        // the plan controller for a plan that does not live under that category.
        if (GetDocument()->IsNodeAccessible(planNode->getNodeId()))
        {
            PlanInstanceSharedPtr planInstance = m_planInstanceListCtrl.GetSelectedInstance();
            TA_ASSERT(NULL != planInstance.get(), "Plan instance is null");

            planNode->controlPlan(planInstance->getInstanceId());
        }
        else
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCKCATEGORY);
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_210013);
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


LRESULT CActivePlanListView::OnViewShown(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnViewShown" );

    if (!m_initialised)
    {
        PlanInstanceFilter *planInstanceFilter = GetDocument()->GetPlanInstanceFilter();

        if (!OperatorRights::getInstance().canViewNonLocalActivePlans())
        {
            planInstanceFilter->setAllLocationsNeeded(false);
            m_allLocationsCheck.EnableWindow(FALSE);
        }

        m_allOwnersCheck.SetCheck(planInstanceFilter->allOwnersNeeded()? BST_CHECKED : BST_UNCHECKED);
        m_allLocationsCheck.SetCheck(planInstanceFilter->allLocationsNeeded()? BST_CHECKED : BST_UNCHECKED);

        m_planInstanceListCtrl.SetPlanInstanceFilter(planInstanceFilter);
        m_planInstanceListCtrl.SetActivePlanAccess(&PlanAgentAccess::getInstance());

        m_planInstanceListCtrl.Refresh();

        m_initialised = true;
    }

    FUNCTION_EXIT;
    return 0;
}


void CActivePlanListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    FUNCTION_ENTRY( "OnUpdate" );

    // TODO: Add your specialized code here and/or call the base class
    if (!m_initialised)
    {
        FUNCTION_EXIT;
        return;
    }

    if (lHint == ID_RPARAM_SESSIONID_UPDATE)
    {
        m_allLocationsCheck.EnableWindow(OperatorRights::getInstance().canViewNonLocalActivePlans());

        PlanInstanceFilter *planInstanceFilter = GetDocument()->GetPlanInstanceFilter();

        if (planInstanceFilter->allLocationsNeeded() && !OperatorRights::getInstance().canViewNonLocalActivePlans())
        {
            planInstanceFilter->setAllLocationsNeeded(false);
            m_allLocationsCheck.SetCheck(BST_UNCHECKED);
            SendMessage(WM_COMMAND, MAKEWPARAM(m_allLocationsCheck.GetDlgCtrlID(), BN_CLICKED), (LPARAM) m_allLocationsCheck.GetSafeHwnd());
        }

        FUNCTION_EXIT;
        return;
    }

    CViewUpdate<TA_Base_Core::ActivePlanDetail> *apdUpdate = dynamic_cast<CViewUpdate<TA_Base_Core::ActivePlanDetail> *>(pHint);
    if (apdUpdate)
    {
        SendMessageToDescendants(WM_ACTIVE_PLAN_DETAIL_UPDATE, (WPARAM) &apdUpdate->GetUpdateData(), 0);
    }

    FUNCTION_EXIT;
}


