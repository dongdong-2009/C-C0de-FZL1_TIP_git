//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepDetailPage.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include <sstream>
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanStepDetailPage.h"
#include "PlanBrowserDoc.h"
#include "PlanNode.h"
#include "PlanStep.h"
#include "EndStep.h"
#include "PlanInstance.h"
#include "PlanInstanceNone.h"
#include "Utilities.h"
#include "MessageBox.h"
#include "PlanManagerCommonDefs.h"
#include "PlanInstanceFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepDetailPage dialog


CPlanStepDetailPage::CPlanStepDetailPage() :
    CPlanDetailPage(CPlanStepDetailPage::IDD)
{
    FUNCTION_ENTRY( "CPlanStepDetailPage" );

    // {{AFX_DATA_INIT(CPlanStepDetailPage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepDetailPage::~CPlanStepDetailPage()
{
    FUNCTION_ENTRY( "~CPlanStepDetailPage" );
    FUNCTION_EXIT;
}


void CPlanStepDetailPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanDetailPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepDetailPage)
    DDX_Control(pDX, IDC_ACTIVE_INSTANCE_COMBO, m_planInstanceCombo);
    DDX_Control(pDX, IDC_ACTIVE_INSTANCE_LABEL, m_planInstanceComboLabel);
    DDX_Control(pDX, IDC_ACTIVE_INSTANCE_LIST, m_planInstanceListCtrl);
    DDX_Control(pDX, IDC_STEP_PARAMETERS_LIST, m_stepParametersListCtrl);
    DDX_Control(pDX, IDC_STEP_PARAMETERS_GRPBOX, m_stepParametersGroupBox);
    DDX_Control(pDX, IDC_PLAN_STEPS_LIST, m_planStepsListCtrl);
    DDX_Control(pDX, IDC_PLAN_STEPS_GRPBOX, m_planStepsGroupBox);
    DDX_Control(pDX, IDC_PLAN_STATUS_GRPBOX, m_planStatusGroupBox);
    DDX_Control(pDX, IDC_ALL_OWNERS_CHECK, m_allOwnersCheck);
    DDX_Control(pDX, IDC_ALL_LOCATIONS_CHECK, m_allLocationsCheck);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepDetailPage, CPlanDetailPage)
    // {{AFX_MSG_MAP(CPlanStepDetailPage)
    ON_WM_SIZE()
    ON_CBN_SELCHANGE(IDC_ACTIVE_INSTANCE_COMBO, OnPlanInstanceComboItemSelected)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_PLAN_STEPS_LIST, OnPlanStepsListItemChanged)
    ON_NOTIFY(NM_DBLCLK, IDC_PLAN_STEPS_LIST, OnDblclkPlanStepsList)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_SELECT_ACTIVE_PLAN_INSTANCE, OnSelectActivePlanInstance)
    ON_MESSAGE(WM_ACTIVE_PLAN_DETAIL_UPDATE, OnActivePlanDetailUpdate)
    ON_MESSAGE(WM_PLAN_CUSTOMISATION_UPDATE, OnPlanCustomisationUpdate)
    ON_MESSAGE(WM_REFRESH_PLAN_STEP_LIST, OnRefreshPlanStepList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepDetailPage message handlers

BOOL CPlanStepDetailPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanDetailPage::OnInitDialog();

    // TODO: Add extra initialization here
    m_planInstanceCombo.SetPlanInstanceFilter(GetPlanInstanceFilter());

    m_planStepsListCtrl.SetManager(this); // for in-table edit management

    if (GetDocument()->EditingEnabled())
    {
        m_planStepsListCtrl.ModifyStyle(LVS_SINGLESEL, 0);
    }
    else
    {
        m_planStepsListCtrl.ModifyStyle(0, LVS_SINGLESEL);
    }

    m_splitterBar.BindWithControl(this, IDC_SPLITTER_BAR);
    m_splitterBar.SetMinHeight(110, 0);
    m_splitterBar.SetSplitRatio(0.65);
    m_splitterBar.AttachAbove(m_planStepsGroupBox.GetDlgCtrlID(), 10);
    m_splitterBar.AttachAbove(m_planStepsListCtrl.GetDlgCtrlID(), 25);
    m_splitterBar.AttachBelow(m_stepParametersGroupBox.GetDlgCtrlID(), 6);
    m_splitterBar.AttachBelow(m_stepParametersListCtrl.GetDlgCtrlID(), 27);
    m_splitterBar.RecalcLayout();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CPlanStepDetailPage::OnSetActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    if (CPlanDetailPage::OnSetActive())
    {
        PlanInstanceSharedPtr currentInstance = GetDocument()->GetCurrentInstance();
        m_planInstanceCombo.SetSelectedInstance(currentInstance);

        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}


void CPlanStepDetailPage::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CPlanDetailPage::OnSize(nType, cx, cy);

    if (::IsWindow(m_planStatusGroupBox.GetSafeHwnd()))
    {
        PlanNode *planNode = GetPlanNode();

        CRect statusBoxRect;
        statusBoxRect.bottom = 0;

        if (planNode->isApproved())
        {
            m_planStatusGroupBox.GetWindowRect(statusBoxRect);
            ScreenToClient(statusBoxRect);
            statusBoxRect.right = cx - 8;
            m_planStatusGroupBox.MoveWindow(statusBoxRect);

            CRect instanceListRect;
            m_planInstanceListCtrl.GetWindowRect(instanceListRect);
            ScreenToClient(instanceListRect);
            instanceListRect.right = statusBoxRect.right - 13;
            m_planInstanceListCtrl.MoveWindow(instanceListRect);
        }

        CRect stepsBoxRect;
        m_planStepsGroupBox.GetWindowRect(stepsBoxRect);
        ScreenToClient(stepsBoxRect);
        stepsBoxRect.top = statusBoxRect.bottom + 7;
        stepsBoxRect.right = cx - 8;
        m_planStepsGroupBox.MoveWindow(stepsBoxRect);

        CRect stepsListRect;
        m_planStepsListCtrl.GetWindowRect(stepsListRect);
        ScreenToClient(stepsListRect);
        stepsListRect.top = stepsBoxRect.top + 22;
        stepsListRect.right = stepsBoxRect.right - 13;
        stepsListRect.bottom = stepsBoxRect.bottom - 13;
        m_planStepsListCtrl.MoveWindow(stepsListRect);

        CRect stepParamsBoxRect;
        m_stepParametersGroupBox.GetWindowRect(stepParamsBoxRect);
        ScreenToClient(stepParamsBoxRect);
        stepParamsBoxRect.right = cx - 8;
        stepParamsBoxRect.bottom = cy - 8;
        m_stepParametersGroupBox.MoveWindow(stepParamsBoxRect);

        CRect stepParamsListRect;
        m_stepParametersListCtrl.GetWindowRect(stepParamsListRect);
        ScreenToClient(stepParamsListRect);
        stepParamsListRect.right = stepParamsBoxRect.right - 13;
        stepParamsListRect.bottom = stepParamsBoxRect.bottom - 13;
        m_stepParametersListCtrl.MoveWindow(stepParamsListRect);

        m_splitterBar.RecalcLayout();
    }

    FUNCTION_EXIT;
}


void CPlanStepDetailPage::OnPlanInstanceComboItemSelected()
{
    FUNCTION_ENTRY( "OnPlanInstanceComboItemSelected" );

    PlanInstanceId planInstanceId = m_planInstanceCombo.GetSelectedInstance();
    bool isActiveInstance = (planInstanceId.m_instance != 0 && planInstanceId.m_location != 0);
    
    PlanInstanceSharedPtr planInstance;
    if (isActiveInstance)
    {
        planInstance = GetPlanNode()->getPlanInstance(planInstanceId);
    }
    else
    {
        planInstance = PlanInstanceSharedPtr(new PlanInstanceNone());
    }

    GetDocument()->SetCurrentInstance(isActiveInstance? planInstance : PlanInstanceSharedPtr());

    try
    {
        planInstance->setSinglePlanInstanceList(m_planInstanceListCtrl);
        planInstance->setCustomStepDetails(m_planStepsListCtrl);
        planInstance->setProgressDetails(m_planStepsListCtrl);
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        // When an instance is selected for the first time, its execution and customisation
        // history is requested from the Plan Agent. If the instance is not known to the
        // agent (e.g. it happened to expire just before we asked for its execution status)
        // an exception will be thrown. The exception will indicate the cause of the error.

        MessageBox::error(IDS_SHOW_EXECUTION_STATUS_FOR_INSTANCE, ex);

        if (isActiveInstance)
        {
            m_planInstanceCombo.SetSelectedInstance(PlanInstanceSharedPtr());
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepDetailPage::OnPlanStepsListItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnPlanStepsListItemChanged" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    bool stepSelected = (pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED &&
                        (pNMListView->uOldState & LVIS_SELECTED) != LVIS_SELECTED;
    bool stepDeselected = (pNMListView->uNewState & LVIS_SELECTED) != LVIS_SELECTED &&
                          (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED;

    // When a step is selected or deselected, update the current step list as well as parameter display.
    if (stepSelected || stepDeselected)
    {
        PlanStep *planStep = m_planStepsListCtrl.GetStep(pNMListView->iItem);
        TA_ASSERT(planStep, "Plan step is null");

        if (stepSelected)
        {
            GetDocument()->AddToCurrentSteps(planStep);
        }
        else
        {
            GetDocument()->RemoveFromCurrentSteps(planStep);
        }

        try
        {
            // We can show parameters for a single step. Clear the parameter list if multiple steps are selected.
            if (GetDocument()->GetCurrentSteps().size() == 1)
            {
                GetDocument()->GetCurrentStep()->addParametersToParameterList(m_stepParametersListCtrl);
            }
            else
            {
                m_stepParametersListCtrl.DeleteAllItems();
            }
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_UPDATE_STATE_PARAMETER_LIST, ex);
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepDetailPage::OnDblclkPlanStepsList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclkPlanStepsList" );

    // Run the edit function - handled by PlanEditorDoc
    PostMessage(WM_COMMAND, ID_STEP_EDIT);

    // Run the customise function - handled by PlanControllerDoc
    PostMessage(WM_COMMAND, ID_STEP_CUSTOMISE);

    *pResult = 0;

    FUNCTION_EXIT;
}


LRESULT CPlanStepDetailPage::OnSelectActivePlanInstance(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnSelectActivePlanInstance" );

    PlanInstanceId planInstanceId(LOWORD(lParam), HIWORD(lParam));
    PlanInstanceSharedPtr planInstance = GetPlanNode()->getPlanInstance(planInstanceId);

    if (NULL != planInstance.get())
    {
        // The instance to be selected may not be visible under the current filter. If that is the case,
        // the filter needs to be reset to enable the instance of interest to appear in the instance combo.
        if (!planInstance->ownedByCurrentSession())
        {
            GetPlanInstanceFilter()->setAllOwnersNeeded(true);
        }
        if (!planInstance->isLocalInstance())
        {
            GetPlanInstanceFilter()->setAllLocationsNeeded(true);
        }

        // Now check the instance filter buttons as appropriate. This will cause the instance combo
        // to be populated with any missing instances.
        SetPlanFilterButtonStates();

        // Finally select the specified instance.
        m_planInstanceCombo.SetSelectedInstance(planInstance);
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlanStepDetailPage::OnActivePlanDetailUpdate(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnActivePlanDetailUpdate" );

    TA_Base_Core::ActivePlanDetail *activePlanDetail = reinterpret_cast<TA_Base_Core::ActivePlanDetail *>(wParam);
    TA_ASSERT(activePlanDetail, "Active plan detail pointer is null");

    PlanInstanceId planInstanceId(activePlanDetail->plan.instance, activePlanDetail->plan.location);
    PlanInstanceSharedPtr planInstance = GetPlanNode()->getPlanInstance(planInstanceId);

    if (NULL != planInstance)
    {
        if (m_planInstanceCombo.IsNewInstance(planInstance->getInstanceId()))
        {
            m_planInstanceCombo.AddInstance(planInstance->getActivePlanState(), planInstance->getActivePlanId().plan, planInstance->getInstanceId());

            // Automatically select every new instance activated by this session
            if (planInstance->ownedByCurrentSession())
            {
                m_planInstanceCombo.SetSelectedInstance(planInstance);
            }
        }
        else
        {
            if (planInstance->getInstanceId() == m_planInstanceCombo.GetSelectedInstance())
            {
                m_planInstanceListCtrl.UpdateInstance(*activePlanDetail);
                m_planStepsListCtrl.UpdatePlanExecutionProgress(*activePlanDetail);
            }

            // If there's been a change in execution state, the instance may need repositioning
            // within the instance combo.
            m_planInstanceCombo.RepositionInstance(planInstance);
        }
    }
    else // if planInstance == NULL (i.e. the instance has expired)
    {
        m_planInstanceCombo.RemoveInstance(planInstanceId);
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlanStepDetailPage::OnPlanCustomisationUpdate(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnPlanCustomisationUpdate" );

    TA_Base_Core::PlanCustomisationUpdate *planCustomDetail = reinterpret_cast<TA_Base_Core::PlanCustomisationUpdate *>(wParam);
    TA_ASSERT(planCustomDetail, "Plan custom detail pointer is null");

    PlanInstanceId planInstanceId(planCustomDetail->plan.instance, planCustomDetail->plan.location);
    PlanInstanceSharedPtr planInstance = GetPlanNode()->getPlanInstance(planInstanceId);

    if (NULL != planInstance.get() && planInstance->getInstanceId() == m_planInstanceCombo.GetSelectedInstance())
    {
        m_planStepsListCtrl.UpdateCustomStepSettings(*planCustomDetail);
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlanStepDetailPage::OnRefreshPlanStepList(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnRefreshPlanStepList" );

    try
    {
        CWaitCursor waitCursor;

        PlanStepNumber firstStepToBeSelected = (PlanStepNumber) wParam;
        unsigned long stepCount = (unsigned long) lParam;

        GetDocument()->PurgeCurrentSteps();

        PlanNode *planNode = GetPlanNode();
        planNode->addStepsToStepList(m_planStepsListCtrl);

        m_planStepsListCtrl.SetSelectedSteps(firstStepToBeSelected, stepCount, true);
        m_planStepsListCtrl.SetFocus();
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_UPDATE_PLAN_STEP_LIST, ex);
    }

   FUNCTION_EXIT;
   return 0;
}


void CPlanStepDetailPage::ShowRelevantControls()
{
    FUNCTION_ENTRY( "ShowRelevantControls" );

    int showActivePlanControls = GetPlanNode()->isApproved()? SW_SHOW : SW_HIDE;

    m_planStatusGroupBox.ShowWindow(showActivePlanControls);
    m_allOwnersCheck.ShowWindow(showActivePlanControls);
    m_allLocationsCheck.ShowWindow(showActivePlanControls);
    m_planInstanceComboLabel.ShowWindow(showActivePlanControls);
    m_planInstanceCombo.ShowWindow(showActivePlanControls);
    m_planInstanceListCtrl.ShowWindow(showActivePlanControls);

    m_planInstanceListCtrl.SetReadOnly(!GetDocument()->EditingEnabled() && !GetDocument()->ControlEnabled());
    m_planStepsListCtrl.SetReadOnly(!GetDocument()->EditingEnabled() && !GetDocument()->ControlEnabled());
    m_stepParametersListCtrl.SetReadOnly(!GetDocument()->EditingEnabled() && !GetDocument()->ControlEnabled());

    FUNCTION_EXIT;
}


void CPlanStepDetailPage::RefreshControlContents()
{
    FUNCTION_ENTRY( "RefreshControlContents" );

    try
    {
        CWaitCursor waitCursor;

        GetDocument()->PurgeCurrentSteps();

        PlanNode *planNode = GetPlanNode();
        PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();
        planNode->updatePlanView(*this, *planInstanceFilter);

        PlanInstanceSharedPtr currentInstance = GetDocument()->GetCurrentInstance();
        m_planInstanceCombo.SetSelectedInstance(currentInstance);
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_REFRESH_CONTENTS, ex);
    }

    FUNCTION_EXIT;
}


void CPlanStepDetailPage::RefreshActiveInstances()
{
    FUNCTION_ENTRY( "RefreshActiveInstances" );

    PlanNode *planNode = GetPlanNode();
    PlanInstanceFilter *planInstanceFilter = GetPlanInstanceFilter();

    planNode->updatePlanInstanceCombo(m_planInstanceCombo, *planInstanceFilter);

    FUNCTION_EXIT;
}


void CPlanStepDetailPage::PurgeActiveInstances()
{
    FUNCTION_ENTRY( "PurgeActiveInstances" );

    // The purge may occur as the operator activates the page. The instance that may have
    // been selected on the previous page should also be selected in the active instances
    // combo to make sure that current instance selection is not reset during the purge.
    m_planInstanceCombo.SetSelectedInstance(GetDocument()->GetCurrentInstance());
    m_planInstanceCombo.Purge();

    FUNCTION_EXIT;
}


// ICellManagerInterface
ICellManager::CellType CPlanStepDetailPage::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
{
    FUNCTION_ENTRY( "GetCellType" );

    // Only the plan steps list ctrl allows any in-table editing.
    if (listCtrl != &m_planStepsListCtrl)
    {
        FUNCTION_EXIT;
        return ICellManager::CT_NOTEDITABLE;
    }

    // Only the PlanController makes use of in-table editing for step customisation purposes.
    if (!GetDocument()->ControlEnabled())
    {
        FUNCTION_EXIT;
        return ICellManager::CT_NOTEDITABLE;
    }

    // The plan instance must be customisable.
    PlanInstanceSharedPtr planInstance = GetDocument()->GetCurrentInstance();
    if (!planInstance || !planInstance->customisationEnabled())
    {
        FUNCTION_EXIT;
        return ICellManager::CT_NOTEDITABLE;
    }

    // The "End Step" cannot be customised
    PlanStep *planStep = reinterpret_cast<PlanStep *>(m_planStepsListCtrl.GetItemData(item));
    TA_ASSERT(planStep, "Plan step pointer is null");
    if (dynamic_cast<EndStep *>(planStep) != NULL)
    {
        FUNCTION_EXIT;
        return ICellManager::CT_NOTEDITABLE;
    }

    // For the "Skip" column use an enabled checkbox if the step is skippable or a disabled one if not skippable.
    if (subItem == CPlanStepListCtrl::SLC_STEP_SKIP)
    {
        FUNCTION_EXIT;
        return (planStep->isStepSkippable()? ICellManager::CT_CHECKBOX : ICellManager::CT_CHECKBOX_DISABLED);
    }

    // For the "Ignore failure" column use an enabled checkbox if the step is supports ignore failure and is not skipped,
    // otherwise use a disabled checkbox.
    if (subItem == CPlanStepListCtrl::SLC_STEP_IGNORE_FAILURE)
    {
        TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);

        FUNCTION_EXIT;
        return ((planStep->canIgnoreFailure() && !customStepDetail.skip)? ICellManager::CT_CHECKBOX : ICellManager::CT_CHECKBOX_DISABLED);
    }

    if (subItem == CPlanStepListCtrl::SLC_STEP_NOWAIT)
    {
        TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);

        return ((planStep->canEnableNoWait() && planStep->canIgnoreFailure() && !customStepDetail.skip)? ICellManager::CT_CHECKBOX : ICellManager::CT_CHECKBOX_DISABLED);
    }
    // For the "Delay" column use a masked edit box if the step is not skipped. If skipped then it makes no sense to edit delay.
    if (subItem == CPlanStepListCtrl::SLC_STEP_DELAY)
    {
        TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);

        FUNCTION_EXIT;
        return (customStepDetail.skip? ICellManager::CT_NOTEDITABLE : ICellManager::CT_MASK);
    }

    FUNCTION_EXIT;
    return ICellManager::CT_NOTEDITABLE;
}


unsigned long CPlanStepDetailPage::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
{
    FUNCTION_ENTRY( "GetCellMaximumCharacters" );

    TA_ASSERT(false, "There are no cells of type CT_TEXT or CT_NUMBER");

    FUNCTION_EXIT;
    return 0;
}


ICellManager::CellData CPlanStepDetailPage::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
{
    FUNCTION_ENTRY( "GetCellData" );

    TA_ASSERT(false, "There are no cells of type CT_SELECTION");

    FUNCTION_EXIT;
    return ICellManager::CellData();
}


CString CPlanStepDetailPage::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
{
    FUNCTION_ENTRY( "GetCellMask" );

    if (listCtrl == &m_planStepsListCtrl && subItem == CPlanStepListCtrl::SLC_STEP_DELAY)
    {
        // For step delay use a mask of the format "mm:ss" where "mm" is minutes between 00 and 59,
        // and "ss" is seconds between 00 and 59.
        FUNCTION_EXIT;
        return STEP_DELAY_EDIT_MASK;
    }

    FUNCTION_EXIT;
    return "";
}


char CPlanStepDetailPage::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
{
    FUNCTION_ENTRY( "GetCellMaskPromptChar" );

    if (listCtrl == &m_planStepsListCtrl && subItem == CPlanStepListCtrl::SLC_STEP_DELAY)
    {
        FUNCTION_EXIT;
        return '0';
    }

    FUNCTION_EXIT;
    return ' ';
}


bool CPlanStepDetailPage::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
{
    FUNCTION_ENTRY( "UpdateCellValue" );

    if (listCtrl == &m_planStepsListCtrl)
    {
        try
        {
            PlanInstanceSharedPtr planInstance = GetDocument()->GetCurrentInstance();
            TA_ASSERT(planInstance, "Plan instance pointer is null");

            PlanStep *planStep = reinterpret_cast<PlanStep *>(m_planStepsListCtrl.GetItemData(item));
            TA_ASSERT(planStep, "Plan step pointer is null");

            // "Skip" setting has been changed
            if (subItem == CPlanStepListCtrl::SLC_STEP_SKIP)
            {
                TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);
                customStepDetail.skip = (value.CompareNoCase("yes") == 0);

                if (customStepDetail.skip)
                {
                    // Reset ignore failure and delay on a skipped step
                    customStepDetail.ignoreFailure = false;
					customStepDetail.nowait = false;
                    customStepDetail.delay = 0;
                }

                planInstance->customiseStep(customStepDetail);
            }

            // "Ignore failure" setting has been changed
            if (subItem == CPlanStepListCtrl::SLC_STEP_IGNORE_FAILURE)
            {
                TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);
                customStepDetail.ignoreFailure = (value.CompareNoCase("yes") == 0);

				if ( ! customStepDetail.ignoreFailure )
				{
					customStepDetail.nowait = false;
				}

                planInstance->customiseStep(customStepDetail);
            }

            // "no wait" setting has been changed
            if (subItem == CPlanStepListCtrl::SLC_STEP_NOWAIT)
            {
                TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);
                customStepDetail.nowait = (value.CompareNoCase("yes") == 0);
                planInstance->customiseStep(customStepDetail);
            }

            // "Delay" setting has been changed
            if (subItem == CPlanStepListCtrl::SLC_STEP_DELAY)
            {
                // The value is in the format "mm:ss"
                std::istringstream delay((LPCTSTR) value);
                time_t minutes = -1, seconds = -1;
                char colon;

                delay >> minutes >> colon >> seconds;

                if (minutes >= MIN_STEP_DELAY_MINUTE && minutes <= MAX_STEP_DELAY_MINUTE &&
                    seconds >= MIN_STEP_DELAY_SECOND && seconds <= MAX_STEP_DELAY_SECOND)
                {
                    TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);
                    customStepDetail.delay = minutes * 60 + seconds;

                    planInstance->customiseStep(customStepDetail);
                }
                else
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_210074);
                }
            }
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_CUSTOMISE_STEP, ex);
        }
    }

    FUNCTION_EXIT;
    return false;
}


