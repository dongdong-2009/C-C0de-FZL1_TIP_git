//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanGeneralDetailPage.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanManager.h"
#include "PlanGeneralDetailPage.h"
#include "PlanBrowserDoc.h"
#include "PlanNode.h"
#include "MessageBox.h"
#include "core/exceptions/src/TransactiveException.h"
#include "PlanInstanceFilter.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Bus;
using namespace TA_Base_Core;

//as specified in GDN
//note that DB allows for an extra 5 characters
static const UINT NAME_TXT_LIMIT    = 80;

/////////////////////////////////////////////////////////////////////////////
// CPlanGeneralDetailPage dialog


CPlanGeneralDetailPage::CPlanGeneralDetailPage() :
    CPlanDetailPage(CPlanGeneralDetailPage::IDD), m_isLoadingControls(false)
{
    FUNCTION_ENTRY( "CPlanGeneralDetailPage" );

    // {{AFX_DATA_INIT(CPlanGeneralDetailPage)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanGeneralDetailPage::~CPlanGeneralDetailPage()
{
    FUNCTION_ENTRY( "~CPlanGeneralDetailPage" );
    FUNCTION_EXIT;
}


void CPlanGeneralDetailPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanDetailPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanGeneralDetailPage)
    DDX_Control(pDX, IDC_PLAN_MODIFIED_BY_EDIT, m_planModByEdit);
    DDX_Control(pDX, IDC_PLAN_MODIFICATION_TIME_EDIT, m_planModTimeEdit);
    DDX_Control(pDX, IDC_APPROVAL_MODIFIED_BY_EDIT, m_approvalModByEdit);
    DDX_Control(pDX, IDC_APPROVAL_MODIFICATION_TIME_EDIT, m_approvalModTimeEdit);
    DDX_Control(pDX, IDC_NAME_EDIT, m_nameEdit);
    DDX_Control(pDX, IDC_DESCRIPTION_EDIT, m_descriptionEdit);
    DDX_Control(pDX, IDC_APPROVAL_STATUS_EDIT, m_approvalStatusEdit);
    DDX_Control(pDX, IDC_ACTIVE_INSTANCES_GRPBOX, m_activeInstancesGroupBox);
    DDX_Control(pDX, IDC_DETAILS_GRPBOX, m_detailsGroupBox);
    DDX_Control(pDX, IDC_ACTIVE_INSTANCES_LIST, m_planInstanceListCtrl);
    DDX_Control(pDX, IDC_ALL_OWNERS_CHECK, m_allOwnersCheck);
    DDX_Control(pDX, IDC_ALL_LOCATIONS_CHECK, m_allLocationsCheck);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanGeneralDetailPage, CPlanDetailPage)
    // {{AFX_MSG_MAP(CPlanGeneralDetailPage)
    ON_WM_SIZE()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_ACTIVE_INSTANCES_LIST, OnPlanInstanceListItemChanged)
    ON_EN_CHANGE(IDC_NAME_EDIT, OnChangeNameEdit)
    ON_EN_CHANGE(IDC_DESCRIPTION_EDIT, OnChangeDescriptionEdit)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_REFRESH_PLAN_GENERAL_DETAIL, OnRefreshPlanGeneralDetail)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanGeneralDetailPage message handlers

BOOL CPlanGeneralDetailPage::PreTranslateMessage(MSG* pMsg)
{
    FUNCTION_ENTRY( "PreTranslateMessage" );

    // TD#1070/2616
    // This is relevant only to plan editing. It enables Del/Ctrl+X/Ctrl+C/Ctrl+V key presses
    // to be processed within the General tab. These keys are normally used as accelerators
    // for the ID_STEP_DELETE/ID_STEP_CUT/ID_STEP_COPY/ID_STEP_PASTE commands, which the General
    // tab disables.
    if (pMsg->message == WM_KEYDOWN)
    {
        WPARAM key = pMsg->wParam;
        bool isCtrlPressed = (GetAsyncKeyState(VK_CONTROL) < 0);

        if ((key == VK_DELETE) || (isCtrlPressed && (key == 'C' || key == 'X' || key == 'V')))
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);

            FUNCTION_EXIT;
            return TRUE;
        }
    }

    // TD#1739/1740/1741: Route the message via CPlanDetailPage to ensure the parent
    // frame gets a go at it.
    FUNCTION_EXIT;
    return CPlanDetailPage::PreTranslateMessage(pMsg);
}


BOOL CPlanGeneralDetailPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanDetailPage::OnInitDialog();

    // Enable the plan instance filter on the active plan instance list control.
    m_planInstanceListCtrl.SetPlanInstanceFilter(GetPlanInstanceFilter());

    // Separate step and parameter lists with a movable splitter bar
    m_splitterBar.BindWithControl(this, IDC_SPLITTER_BAR);
    m_splitterBar.SetMinHeight(235, 0);
    m_splitterBar.SetSplitRatio(0.5);
    m_splitterBar.AttachAbove(m_detailsGroupBox.GetDlgCtrlID(), 10);
    m_splitterBar.AttachAbove(m_descriptionEdit.GetDlgCtrlID(), 25);
    m_splitterBar.AttachBelow(m_activeInstancesGroupBox.GetDlgCtrlID(), 6);
    m_splitterBar.AttachBelow(m_allOwnersCheck.GetDlgCtrlID(), 28, TRUE);
    m_splitterBar.AttachBelow(m_allLocationsCheck.GetDlgCtrlID(), 54, TRUE);
    m_splitterBar.AttachBelow(m_planInstanceListCtrl.GetDlgCtrlID(), 82);
    m_splitterBar.RecalcLayout();

     // as per TD 11659
     m_nameEdit.SetLimitText (NAME_TXT_LIMIT);

	 //CL-14921++ Item1
	 const unsigned int MAX_DESCRIPTION_BYTES = 950;
	 m_descriptionEdit.SetLimitText(MAX_DESCRIPTION_BYTES);
	 //++CL-14921

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CPlanGeneralDetailPage::OnSetActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    if (CPlanDetailPage::OnSetActive())
    {
        PlanInstanceSharedPtr currentInstance = GetDocument()->GetCurrentInstance();
        m_planInstanceListCtrl.SetSelectedInstance(currentInstance);

        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}


void CPlanGeneralDetailPage::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CPlanDetailPage::OnSize(nType, cx, cy);

    if (::IsWindow(m_detailsGroupBox.GetSafeHwnd()))
    {
        PlanNode *planNode = GetPlanNode();

        CRect detailsBoxRect;
        m_detailsGroupBox.GetWindowRect(detailsBoxRect);
        ScreenToClient(detailsBoxRect);
        detailsBoxRect.right = cx - 8;
        if (!planNode->isApproved())
        {
            detailsBoxRect.bottom = cy - 8;
        }
        m_detailsGroupBox.MoveWindow(detailsBoxRect);

        CRect nameEditRect;
        m_nameEdit.GetWindowRect(nameEditRect);
        ScreenToClient(nameEditRect);
        nameEditRect.right = detailsBoxRect.right - 13;
        m_nameEdit.MoveWindow(nameEditRect);

        CRect descriptionEditRect;
        m_descriptionEdit.GetWindowRect(descriptionEditRect);
        ScreenToClient(descriptionEditRect);
        descriptionEditRect.right = detailsBoxRect.right - 13;
        descriptionEditRect.bottom = detailsBoxRect.bottom - 13;
        m_descriptionEdit.MoveWindow(descriptionEditRect);

        if (planNode->isApproved())
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

            m_splitterBar.RecalcLayout();
        }
    }

    FUNCTION_EXIT;
}


void CPlanGeneralDetailPage::OnPlanInstanceListItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnPlanInstanceListItemChanged" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED &&
        (pNMListView->uOldState & LVIS_SELECTED) != LVIS_SELECTED)
    {
        PlanInstanceSharedPtr planInstance = m_planInstanceListCtrl.GetSelectedInstance();
        GetDocument()->SetCurrentInstance(planInstance);
    }
    else if ((pNMListView->uNewState & LVIS_SELECTED) != LVIS_SELECTED &&
             (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        GetDocument()->SetCurrentInstance(PlanInstanceSharedPtr());
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


LRESULT CPlanGeneralDetailPage::OnRefreshPlanGeneralDetail(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnRefreshPlanGeneralDetail" );

    RefreshControlContents();

    FUNCTION_EXIT;
    return 0;
}


void CPlanGeneralDetailPage::OnChangeNameEdit()
{
    FUNCTION_ENTRY( "OnChangeNameEdit" );

    PlanNode* planNode = GetPlanNode();

    if (planNode->isCopiedPlan() && !m_isLoadingControls)
    {
        CString text;
        m_nameEdit.GetWindowText(text);

        planNode->setPlanName((LPCTSTR)text);
        GetDocument()->SetModifiedFlag();
    }

    FUNCTION_EXIT;
}


void CPlanGeneralDetailPage::OnChangeDescriptionEdit()
{
    FUNCTION_ENTRY( "OnChangeDescriptionEdit" );

    PlanNode* planNode = GetPlanNode();

    if (planNode->isCopiedPlan() && !m_isLoadingControls)
    {
        CString text;
        m_descriptionEdit.GetWindowText(text);

        planNode->setPlanDescription((LPCTSTR)text);
        GetDocument()->SetModifiedFlag();
    }

    FUNCTION_EXIT;
}


void CPlanGeneralDetailPage::ShowRelevantControls()
{
    FUNCTION_ENTRY( "ShowRelevantControls" );

    int showActivePlanControls = GetPlanNode()->isApproved()? SW_SHOW : SW_HIDE;

    m_splitterBar.ShowWindow(showActivePlanControls);
    m_activeInstancesGroupBox.ShowWindow(showActivePlanControls);
    m_allOwnersCheck.ShowWindow(showActivePlanControls);
    m_allLocationsCheck.ShowWindow(showActivePlanControls);
    m_planInstanceListCtrl.ShowWindow(showActivePlanControls);

    m_nameEdit.SetReadOnly(!GetDocument()->EditingEnabled());
    m_descriptionEdit.SetReadOnly(!GetDocument()->EditingEnabled());
    m_planInstanceListCtrl.SetReadOnly(!GetDocument()->ControlEnabled());

    FUNCTION_EXIT;
}


void CPlanGeneralDetailPage::RefreshControlContents()
{
    FUNCTION_ENTRY( "RefreshControlContents" );

    m_isLoadingControls = true;
    try
    {
        CWaitCursor waitCursor;

        PlanNode *planNode = GetPlanNode();
        PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();
        planNode->updatePlanView(*this, *planInstanceFilter);

        PlanInstanceSharedPtr currentInstance = GetDocument()->GetCurrentInstance();
        m_planInstanceListCtrl.SetSelectedInstance(currentInstance);

    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_REFRESH_CONTENTS, ex);
    }
    m_isLoadingControls = false;

    FUNCTION_EXIT;
}


void CPlanGeneralDetailPage::RefreshActiveInstances()
{
    FUNCTION_ENTRY( "RefreshActiveInstances" );

    PlanNode *planNode = GetPlanNode();
    PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();

    planNode->updatePlanInstanceList(m_planInstanceListCtrl, *planInstanceFilter);

    FUNCTION_EXIT;
}


void CPlanGeneralDetailPage::PurgeActiveInstances()
{
    FUNCTION_ENTRY( "PurgeActiveInstances" );

    // The purge may occur as the operator activates the page. The instance that may have
    // been selected on the previous page should also be selected in the active instances
    // list to make sure that current instance selection is not reset during the purge.
    m_planInstanceListCtrl.SetSelectedInstance(GetDocument()->GetCurrentInstance());
    m_planInstanceListCtrl.Purge();

    FUNCTION_EXIT;
}


