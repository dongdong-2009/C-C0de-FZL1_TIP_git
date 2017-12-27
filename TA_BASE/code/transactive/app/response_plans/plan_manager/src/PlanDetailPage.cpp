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

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/PlanDetailPage.h"
#include "app/response_plans/plan_manager/src/PlanBrowserDoc.h"
#include "app/response_plans/plan_manager/src/PlanNode.h"
#include "app/response_plans/plan_manager/src/PlanInstanceFilter.h"
#include "app/response_plans/plan_manager/src/OperatorRights.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanDetailPage dialog

CPlanDetailPage::CPlanDetailPage(UINT nIDTemplate, UINT nIDCaption /*= 0*/) :
    CPropertyPage(nIDTemplate, nIDCaption),
    m_isRefreshPending(true)
{
    FUNCTION_ENTRY( "CPlanDetailPage" );
    FUNCTION_EXIT;
}


CPlanDetailPage::~CPlanDetailPage()
{
    FUNCTION_ENTRY( "~CPlanDetailPage" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanDetailPage, CPropertyPage)
    // {{AFX_MSG_MAP(CPlanDetailPage)
    ON_BN_CLICKED(IDC_ALL_OWNERS_CHECK, OnAllOwnersCheck)
    ON_BN_CLICKED(IDC_ALL_LOCATIONS_CHECK, OnAllLocationsCheck)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_RESET_TREE_NODE_VIEW, OnResetTreeNodeView)
    ON_MESSAGE_VOID(WM_ADJUST_CONTROL_POSITIONS, AdjustControlPositions)
    ON_MESSAGE_VOID(WM_ADJUST_PLAN_FILTER_BUTTON_STATES, SetPlanFilterButtonStates)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanDetailPage message handlers

void CPlanDetailPage::OnAllOwnersCheck()
{
    FUNCTION_ENTRY( "OnAllOwnersCheck" );

    PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();
    planInstanceFilter->setAllOwnersNeeded(m_allOwnersCheck.GetCheck() == BST_CHECKED);

    if (!m_isRefreshPending)
    {
        CWaitCursor waitCursor;

        if (planInstanceFilter->allOwnersNeeded())
        {
            RefreshActiveInstances();
        }
        else
        {
            PurgeActiveInstances();
        }
    }

    FUNCTION_EXIT;
}


void CPlanDetailPage::OnAllLocationsCheck()
{
    FUNCTION_ENTRY( "OnAllLocationsCheck" );

    PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();
    planInstanceFilter->setAllLocationsNeeded(m_allLocationsCheck.GetCheck() == BST_CHECKED);

    if (!m_isRefreshPending)
    {
        CWaitCursor waitCursor;

        if (planInstanceFilter->allLocationsNeeded())
        {
            RefreshActiveInstances();
        }
        else
        {
            PurgeActiveInstances();
        }
    }

    FUNCTION_EXIT;
}


LRESULT CPlanDetailPage::OnResetTreeNodeView(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    FUNCTION_ENTRY( "OnResetTreeNodeView" );

    // Reset active instance filtering rules
    GetDocument()->GetPlanInstanceFilter()->setDefaults();

    // Reset active instance seletion
    PlanInstanceSharedPtr instance;
    GetDocument()->SetCurrentInstance(instance);

    // For efficiency update only the active page. Other page(s) will update once activated.
    m_isRefreshPending = true;

    if (IsActive())
    {
        UpdatePlanDetail();
    }

    FUNCTION_EXIT;
    return 0;
}


BOOL CPlanDetailPage::OnSetActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    if (CPropertyPage::OnSetActive())
    {
        GetParentFrame()->SendMessage(WM_PLAN_DETAIL_PAGE_SELECTION_CHANGED, (WPARAM) this);

        UpdatePlanDetail();

        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}


bool CPlanDetailPage::IsActive()
{
    FUNCTION_ENTRY( "IsActive" );
    FUNCTION_EXIT;
    return (this == GetPropertySheet()->GetActivePage());
}


void CPlanDetailPage::UpdatePlanDetail()
{
    FUNCTION_ENTRY( "UpdatePlanDetail" );

    AdjustControlPositions();
    SetPlanFilterButtonStates();

    if (m_isRefreshPending)
    {
        ShowRelevantControls();
        RefreshControlContents();

        m_isRefreshPending = false;
    }

    FUNCTION_EXIT;
}


void CPlanDetailPage::AdjustControlPositions()
{
    FUNCTION_ENTRY( "AdjustControlPositions" );

    CTabCtrl *tabCtrl = GetPropertySheet()->GetTabControl();
    CRect tabCtrlRect;

    tabCtrl->GetClientRect(tabCtrlRect);
    tabCtrl->AdjustRect(FALSE, tabCtrlRect);
    tabCtrlRect.OffsetRect(CPoint(5, 9));   // BGTODO: Shouldn't have to offset at all. Investigate ...

    // Reposition the dialog and tell it to resize. This will adjust the sizes and positions of individual controls.
    MoveWindow(tabCtrlRect);
    SendMessage(WM_SIZE, SIZE_RESTORED, MAKEWPARAM(tabCtrlRect.Width(), tabCtrlRect.Height()));

    FUNCTION_EXIT;
}


void CPlanDetailPage::SetPlanFilterButtonStates()
{
    FUNCTION_ENTRY( "SetPlanFilterButtonStates" );

    PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();

    m_allLocationsCheck.EnableWindow(OperatorRights::getInstance().canViewNonLocalActivePlans());
    if (!OperatorRights::getInstance().canViewNonLocalActivePlans())
    {
        planInstanceFilter->setAllLocationsNeeded(false);
    }

    if (planInstanceFilter->allOwnersNeeded() != (m_allOwnersCheck.GetCheck() == BST_CHECKED))
    {
        m_allOwnersCheck.SetCheck(planInstanceFilter->allOwnersNeeded()? BST_CHECKED : BST_UNCHECKED);
        SendMessage(WM_COMMAND, MAKEWPARAM(m_allOwnersCheck.GetDlgCtrlID(), BN_CLICKED), (LPARAM) m_allOwnersCheck.GetSafeHwnd());
    }

    if (planInstanceFilter->allLocationsNeeded() != (m_allLocationsCheck.GetCheck() == BST_CHECKED))
    {
        m_allLocationsCheck.SetCheck(planInstanceFilter->allLocationsNeeded()? BST_CHECKED : BST_UNCHECKED);
        SendMessage(WM_COMMAND, MAKEWPARAM(m_allLocationsCheck.GetDlgCtrlID(), BN_CLICKED), (LPARAM) m_allLocationsCheck.GetSafeHwnd());
    }

    FUNCTION_EXIT;
}


BOOL CPlanDetailPage::PreTranslateMessage(MSG* pMsg)
{
    FUNCTION_ENTRY( "PreTranslateMessage" );

    // TODO: Add your specialized code here and/or call the base class
    CFrameWnd *parentFrame = GetParentFrame();
    if (parentFrame)
    {
        // Give the frame a chance to process accelerator key presses.
        if (parentFrame->PreTranslateMessage(pMsg))
        {
            FUNCTION_EXIT;
            return TRUE;
        }
    }

    FUNCTION_EXIT;
    return CPropertyPage::PreTranslateMessage(pMsg);
}


CPropertySheet *CPlanDetailPage::GetPropertySheet()
{
    FUNCTION_ENTRY( "GetPropertySheet" );

    CPropertySheet *propertySheet = static_cast<CPropertySheet *>(GetParent());
    TA_ASSERT(propertySheet != NULL, "Parent property sheet is null");

    FUNCTION_EXIT;
    return propertySheet;
}


CPlanBrowserDoc *CPlanDetailPage::GetDocument()
{
    FUNCTION_ENTRY( "GetDocument" );

    CFrameWnd *parentFrame = GetParentFrame();
    TA_ASSERT(parentFrame != NULL, "Parent frame is null");

    CDocument *document = parentFrame->GetActiveDocument();
    TA_ASSERT(document != NULL, "Parent frame's active document is null");

    CPlanBrowserDoc *planBrowserDoc = dynamic_cast<CPlanBrowserDoc *>(document);
    TA_ASSERT(planBrowserDoc != NULL, "Parent frame's active document is not a CPlanBrowserDoc");

    FUNCTION_EXIT;
    return planBrowserDoc;
}


PlanNode *CPlanDetailPage::GetPlanNode()
{
    FUNCTION_ENTRY( "GetPlanNode" );

    PlanNode *planNode = dynamic_cast<PlanNode *>(GetDocument()->GetCurrentNode());
    TA_ASSERT(planNode != NULL, "Not a plan node");

    FUNCTION_EXIT;
    return planNode;
}


PlanInstanceFilter *CPlanDetailPage::GetPlanInstanceFilter()
{
    FUNCTION_ENTRY( "GetPlanInstanceFilter" );

    PlanInstanceFilter *planInstanceFilter = GetDocument()->GetPlanInstanceFilter();
    TA_ASSERT(planInstanceFilter != NULL, "Plan instance filter is null");

    FUNCTION_EXIT;
    return planInstanceFilter;
}


BOOL CPlanDetailPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    return TRUE;
}