/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/StepListDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// StepListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "StepListDlg.h"
#include "Globals.h"
#include "PvData.h"
#include "PlanStepEditDoc.h"
#include "PlanStepEditMainFrm.h"
#include "PlanStepForm.h"
#include "StepParameterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static enum
{
    IMG_STEP_OK = 0,
    IMG_STEP_FAIL,
    IMG_STEP_SKIP,
    IMG_NEXT_STEP,
    IMG_PAUSED_NEXT_STEP,
    IMG_BLANK
};

static UINT STEP_ICONS[] =
{
    IDI_STEP_OK_ICON,
    IDI_STEP_FAIL_ICON,
    IDI_STEP_SKIP_ICON,
    IDI_NEXT_STEP_ICON,
    IDI_PAUSED_NEXT_STEP_ICON,
    IDI_BLANK_ICON
};

// Allow in-table editing of steps for customisation purposes prior to execution
static ICellManager::CellType STEP_LIST_COLUMN_TYPES[] =
{
    ICellManager::CT_NOTEDITABLE,   // execution progress column
    ICellManager::CT_NOTEDITABLE,   // "Step" column
    ICellManager::CT_NOTEDITABLE,   // "Name" column
    ICellManager::CT_NOTEDITABLE,   // "Type" column
    ICellManager::CT_NOTEDITABLE,   // "Description" column
    ICellManager::CT_CHECKBOX,      // "Skip" column
    ICellManager::CT_NUMBER         // "Delay" column
};

static PV::PlanInstance IDLE_INSTANCE;


/////////////////////////////////////////////////////////////////////////////
// CStepListDlg dialog

CStepListDlg::CStepListDlg(CMenu *menu, CToolBar *editToolBar, CToolBar *ctrlToolBar)
    : CDialog(CStepListDlg::IDD, NULL), m_plan(NULL), m_menu(menu), m_editToolBar(editToolBar), m_ctrlToolBar(ctrlToolBar),
      m_readOnly(true), m_controlMode(false), m_splitRatio(0.7), m_planStepListCtrl(this), m_stepParamListCtrl(this)
{
    FUNCTION_ENTRY( "CStepListDlg" );

    m_notes.AddItem("Double-clicking on an entry in the Plan Steps list brings up the step edit window.");
    m_notes.AddItem("The horizontal splitter bar can be moved up and down to resize the Parameters list.");

    FUNCTION_EXIT;
}


void CStepListDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CStepListDlg)
    DDX_Control(pDX, IDC_PLANOWNER_LABEL, m_planOwnerLabel);
    DDX_Control(pDX, IDC_PLANINST_LABEL, m_planInstLabel);
    DDX_Control(pDX, IDC_PLANEXECSTATE_LABEL, m_planExecStateLabel);
    DDX_Control(pDX, IDC_PLANEXECMODE_LABEL, m_planExecModeLabel);
    DDX_Control(pDX, IDC_STEPPARAM_BOX, m_stepParamBox);
    DDX_Control(pDX, IDC_STEPPARAM_LIST, m_stepParamListCtrl);
    DDX_Control(pDX, IDC_NOTES_PICT, m_notes);
    DDX_Control(pDX, IDC_PLANSTATUS_BOX, m_planStatusBox);
    DDX_Control(pDX, IDC_PLANSTEPS_BOX, m_planStepsBox);
    DDX_Control(pDX, IDC_PLANSTEP_LIST, m_planStepListCtrl);
    DDX_Control(pDX, IDC_PLANOWNER_EDIT, m_planOwnerEdit);
    DDX_Control(pDX, IDC_PLANINST_COMBO, m_planInstCombo);
    DDX_Control(pDX, IDC_PLANEXECSTATE_EDIT, m_planExecStateEdit);
    DDX_Control(pDX, IDC_PLANEXECMODE_EDIT, m_planExecModeEdit);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CStepListDlg, CDialog)
    // {{AFX_MSG_MAP(CStepListDlg)
    ON_WM_SIZE()
    ON_CBN_SELCHANGE(IDC_PLANINST_COMBO, OnSelchangePlaninstCombo)
    ON_NOTIFY(NM_DBLCLK, IDC_PLANSTEP_LIST, OnDblclkPlanstepList)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_PLANSTEP_LIST, OnItemchangedPlanstepList)
    ON_NOTIFY(NM_DBLCLK, IDC_STEPPARAM_LIST, OnDblclkStepparamList)
    ON_COMMAND(ID_STEP_EDIT, OnStepEdit)
    ON_COMMAND(ID_STEP_CUSTOMISE, OnStepCustomise)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_UPDATE_PLAN_DETAILS, OnUpdatePlanDetails)
    ON_MESSAGE(WM_ENABLE_EDITING, OnEnableEditing)
    ON_MESSAGE(WM_ENABLE_CONTROL, OnEnableControl)
    ON_MESSAGE(WM_SPLITTER_MOVED, OnSplitterMoved)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStepListDlg message handlers

BOOL CStepListDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    // Create image list for steps list control
    m_stepListImages.Create(12, 12, ILC_COLOR4 | ILC_MASK, 4, 4);
    for (int i = 0; i < sizeof(STEP_ICONS) / sizeof(UINT); i++)
    {
        m_stepListImages.Add(AfxGetApp()->LoadIcon(STEP_ICONS[i]));
    }

    // Attach the image list to the steps list control
    m_planStepListCtrl.SetImageList(&m_stepListImages, LVSIL_SMALL);
    m_planStepListCtrl.SetExtendedStyle(m_planStepListCtrl.GetExtendedStyle() |
                                        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);

    // Set background color for read-only appearance
    m_planStepListCtrl.SetBkColor(::GetSysColor(COLOR_3DFACE));
    m_planStepListCtrl.SetTextBkColor(m_planStepListCtrl.GetBkColor());

    // Create step list control columns
    // NOTE: changing the order may require changes to UpdateCellValue()
    int columnIndex = 0;
    m_planStepListCtrl.InsertColumn(columnIndex++, "", LVCFMT_LEFT, 16); // for bitmaps
    m_planStepListCtrl.InsertColumn(columnIndex++, "Step", LVCFMT_RIGHT, 35);
    m_planStepListCtrl.InsertColumn(columnIndex++, "Name", LVCFMT_LEFT, 180);
    m_planStepListCtrl.InsertColumn(columnIndex++, "Type", LVCFMT_LEFT, 100);
    m_planStepListCtrl.InsertColumn(columnIndex++, "Description", LVCFMT_LEFT, 300);
    m_planStepListCtrl.InsertColumn(columnIndex++, "Skip", LVCFMT_LEFT, 40);
    m_planStepListCtrl.InsertColumn(columnIndex++, "Delay", LVCFMT_RIGHT, 40);

    // Set step parameter list style
    m_stepParamListCtrl.SetExtendedStyle(m_stepParamListCtrl.GetExtendedStyle() |
                                         LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // Set background color for read-only appearance
    m_stepParamListCtrl.SetBkColor(m_planStepListCtrl.GetBkColor());
    m_stepParamListCtrl.SetTextBkColor(m_planStepListCtrl.GetBkColor());

    // Create step parameter list control columns
    // NOTE: changing the order may require changes to UpdateCellValue()
    columnIndex = 0;
    m_stepParamListCtrl.InsertColumn(columnIndex++, "Parameter Name", LVCFMT_LEFT, 200);
    m_stepParamListCtrl.InsertColumn(columnIndex++, "Parameter Value", LVCFMT_LEFT, 200);

    // Separate step and parameter lists with a movable splitter bar
    m_splitter.BindWithControl(this, IDC_SPLITBAR);
    m_splitter.SetMinHeight(110, 0);
    m_splitter.AttachAbove(m_planStepsBox.GetDlgCtrlID(), 12);
    m_splitter.AttachAbove(m_planStepListCtrl.GetDlgCtrlID(), 27);
    m_splitter.AttachBelow(m_stepParamBox.GetDlgCtrlID(), 10);
    m_splitter.AttachBelow(m_stepParamListCtrl.GetDlgCtrlID(), 35);
    m_splitter.RecalcLayout();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CStepListDlg::OnUpdatePlanDetails(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnUpdatePlanDetails" );

    m_plan = (PV::Plan *) wParam;

    PV::StepList steps;
    CPvData::instance().getStepsForPlan(steps, m_plan->m_pkey);

    IDLE_INSTANCE.m_plan = m_plan;
    if (steps.size() > 0)
    {
        IDLE_INSTANCE.m_currentStepPkey = steps[0]->m_pkey;
    }

    if (m_controlMode)
    {
        m_planInstance = &IDLE_INSTANCE;

        if (m_plan->m_instances.size() > 0)
        {
            m_planInstance = m_plan->m_selectedInstance < 0? &IDLE_INSTANCE : m_plan->m_instances[m_plan->m_selectedInstance];
        }
    }
    else if (m_readOnly && m_plan->m_isApproved)
    {
        m_planInstance = NULL;

        if (m_plan->m_instances.size() > 0)
        {
            m_planInstance = m_plan->m_selectedInstance < 0? m_plan->m_instances[0] : m_plan->m_instances[m_plan->m_selectedInstance];
        }
    }
    else
    {
        m_planInstance = NULL;
    }

    PopulateData();
    ResetMenusAndToolbars();

    FUNCTION_EXIT;
    return 0;
}


LRESULT CStepListDlg::OnEnableEditing(WPARAM /* wParam */, LPARAM /* lParam */)
{
    FUNCTION_ENTRY( "OnEnableEditing" );

    m_readOnly = false;
    m_controlMode = false;

    m_notes.AddItem("Edit functionality will be available only to users with plan editing rights.");
    m_notes.AddItem("Edit functionality will be available only on unapproved plans. To edit an approved "
                    "plan, the plan must be unapproved or a copy of it made.");
    m_notes.AddItem("Select instance 'None' to edit steps or to customise them prior to starting "
                    "a new instance");
    m_notes.AddItem("To customise a running instance, it must first be paused. Only those steps "
                    "which have not yet been executed can be customised.");
    m_notes.AddItem("In-table customisation of steps can be performed by single-clicking on the "
                    "appropriate cell within the currently selected Plan Steps list entry. Only "
                    "the Skip and Delay cell values can be modified.");

    if (m_plan->m_isApproved == false)
    {
        CListCtrl listCtrl; listCtrl.Create(0, CRect(0, 0, 0, 0), this, 0);
        COLORREF listCtrlDefBkColor = listCtrl.GetBkColor();

        m_planStepListCtrl.SetBkColor(listCtrlDefBkColor);
        m_planStepListCtrl.SetTextBkColor(listCtrlDefBkColor);
        m_planStepListCtrl.ModifyStyle(0, LVS_EDITLABELS);

        m_stepParamListCtrl.SetBkColor(listCtrlDefBkColor);
        m_stepParamListCtrl.SetTextBkColor(listCtrlDefBkColor);
        m_stepParamListCtrl.ModifyStyle(0, LVS_EDITLABELS);
    }

    m_planStatusBox.ShowWindow(SW_HIDE);
    m_planInstCombo.ShowWindow(SW_HIDE);
    m_planOwnerEdit.ShowWindow(SW_HIDE);
    m_planExecStateEdit.ShowWindow(SW_HIDE);
    m_planExecModeEdit.ShowWindow(SW_HIDE);
    m_planInstLabel.ShowWindow(SW_HIDE);
    m_planOwnerLabel.ShowWindow(SW_HIDE);
    m_planExecStateLabel.ShowWindow(SW_HIDE);
    m_planExecModeLabel.ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
    return 0;
}


LRESULT CStepListDlg::OnEnableControl(WPARAM /* wParam */, LPARAM /* lParam */)
{
    FUNCTION_ENTRY( "OnEnableControl" );

    m_controlMode = true;

    CListCtrl listCtrl; listCtrl.Create(0, CRect(0, 0, 0, 0), this, 0);
    COLORREF listCtrlDefBkColor = listCtrl.GetBkColor();

    m_planStepListCtrl.SetBkColor(listCtrlDefBkColor);
    m_planStepListCtrl.SetTextBkColor(listCtrlDefBkColor);
    m_planStepListCtrl.ModifyStyle(0, LVS_EDITLABELS);

    m_stepParamListCtrl.SetBkColor(listCtrlDefBkColor);
    m_stepParamListCtrl.SetTextBkColor(listCtrlDefBkColor);
    m_stepParamListCtrl.ModifyStyle(0, LVS_EDITLABELS);

    m_planStatusBox.ShowWindow(SW_SHOW);
    m_planInstCombo.ShowWindow(SW_SHOW);
    m_planOwnerEdit.ShowWindow(SW_SHOW);
    m_planExecStateEdit.ShowWindow(SW_SHOW);
    m_planExecModeEdit.ShowWindow(SW_SHOW);
    m_planInstLabel.ShowWindow(SW_HIDE);
    m_planOwnerLabel.ShowWindow(SW_HIDE);
    m_planExecStateLabel.ShowWindow(SW_HIDE);
    m_planExecModeLabel.ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
    return 0;
}


LRESULT CStepListDlg::OnSplitterMoved(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnSplitterMoved" );

    if (lParam == m_splitter.GetDlgCtrlID())
    {
        WORD above = HIWORD(wParam);
        WORD below = LOWORD(wParam);

        m_splitRatio = (double)above / (double)(above + below);
    }

    FUNCTION_EXIT;
    return 0;
}


void CStepListDlg::PopulateData()
{
    FUNCTION_ENTRY( "PopulateData" );

    ASSERT(m_plan != NULL);

    // Remove current contents of the step and step parameter list controls.
    m_planStepListCtrl.DeleteAllItems();
    m_stepParamListCtrl.DeleteAllItems();

    // Populate the step list control with plan's step data.
    PV::StepList steps;
    CPvData::instance().getStepsForPlan(steps, m_plan->m_pkey);

    PV::Step *curStep = NULL;
    if (m_planInstance && (m_controlMode || m_readOnly && m_plan->m_isApproved))
    {
        curStep = CPvData::instance().getStep(m_planInstance->m_currentStepPkey);
    }


    for (PV::StepList::iterator siter = steps.begin(); siter != steps.end(); siter++)
    {
        PV::Step *step = *siter;
        CString execOrderStr, delayStr;

        execOrderStr.Format("%d", step->m_executionOrder);
        delayStr.Format("%02d:%02d", step->m_delay / 60, step->m_delay % 60);

        // Select an icon for this step:
        // - current step gets [>] or [|>] depending on whether plan is running or paused
        // - anything after the current step gets [ ] (i.e. blank)
        // - steps before the current step marked as "skipped" get [-]
        // - all other steps are marked OK [v]
        int stepImage;
        if (curStep)
        {
            if (step->m_executionOrder == curStep->m_executionOrder)
            {
                if (m_planInstance->m_execState == PV::PlanInstance::ExecState::ES_RUNNING)
                {
                    stepImage = IMG_NEXT_STEP;
                }

                else if (m_planInstance->m_execState == PV::PlanInstance::ExecState::ES_PAUSED)
                    stepImage = IMG_PAUSED_NEXT_STEP;
                else
                    stepImage = IMG_BLANK;
            }
            else if (step->m_executionOrder > curStep->m_executionOrder)
            {
                stepImage = IMG_BLANK;
            }
            else
            {
                stepImage = step->m_skip? IMG_STEP_SKIP : IMG_STEP_OK;
            }
        }
        else
        {
            stepImage = IMG_BLANK;
        }

        int rowIndex = m_planStepListCtrl.GetItemCount();
        m_planStepListCtrl.InsertItem(rowIndex, "", stepImage);
        m_planStepListCtrl.SetItemData(rowIndex, (DWORD) step->m_pkey);

        int columnIndex = 1;
        m_planStepListCtrl.SetItemText(rowIndex, columnIndex++, execOrderStr);
        m_planStepListCtrl.SetItemText(rowIndex, columnIndex++, step->m_name.c_str());
        m_planStepListCtrl.SetItemText(rowIndex, columnIndex++, step->m_type.c_str());
        m_planStepListCtrl.SetItemText(rowIndex, columnIndex++, step->m_description.c_str());
        m_planStepListCtrl.showCheckIcon(rowIndex, columnIndex++, step->m_skip, true);
        m_planStepListCtrl.SetItemText(rowIndex, columnIndex++, delayStr);
    }

    if (m_readOnly)
    {
        m_planStatusBox.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planInstCombo.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planOwnerEdit.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planExecStateEdit.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planExecModeEdit.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planInstLabel.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planOwnerLabel.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planExecStateLabel.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planExecModeLabel.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
    }

    if (m_controlMode || m_readOnly && m_plan->m_isApproved)
    {
        if (m_planInstance)
        {
            // Populate instance combo and select the appropriate entry.
            m_planInstCombo.ResetContent();

            for (PV::PlanInstanceList::iterator iter = m_plan->m_instances.begin(); iter != m_plan->m_instances.end(); iter++)
            {
                PV::PlanInstance *instance = *iter;

                CString instNumStr;
                instNumStr.Format("%d", instance->m_instanceNumber);

                m_planInstCombo.AddString(instNumStr);
            }

            CString selectedInstNumStr;
            selectedInstNumStr.Format("%d", m_planInstance->m_instanceNumber);

            if (m_controlMode)
            {
                m_planInstCombo.InsertString(0, "None");

                if (m_planInstance->m_instanceNumber < 0)
                {
                    selectedInstNumStr = "None";
                }

            }

            m_planInstCombo.SelectString(0, selectedInstNumStr);
            m_planInstCombo.EnableWindow(TRUE);

            // Set instance owner edit
            m_planOwnerEdit.SetWindowText(m_planInstance->m_owner.c_str());

            // Set execution state edit
            CString execState;
            if (m_planInstance->m_execState == PV::PlanInstance::ExecState::ES_RUNNING)
            {
                execState = "Running";
            }

            else if (m_planInstance->m_execState == PV::PlanInstance::ExecState::ES_PAUSED)
                execState = "Paused";
            else if (m_planInstance->m_execState == PV::PlanInstance::ExecState::ES_IDLE)
                execState = "";
            else
                execState = "Undefined";
            m_planExecStateEdit.SetWindowText(execState);
        }
        else
        {
            // Set edits.
            m_planInstCombo.ResetContent();
            m_planInstCombo.InsertString(0, "None");
            m_planInstCombo.SelectString(0, "None");
            m_planOwnerEdit.SetWindowText("");
            m_planExecStateEdit.SetWindowText("");
        }

        // Set execution mode edit
        CString execMode;
        if (m_plan->m_execMode == PV::Plan::ExecMode::EM_AUTOMATIC)
        {
            execMode = "Batch";
        }

        else if (m_plan->m_execMode == PV::Plan::ExecMode::EM_INTERACTIVE)
            execMode = "Interactive";
        else
            execMode = "Undefined";
        m_planExecModeEdit.SetWindowText(execMode);
    }

    FUNCTION_EXIT;
}


void CStepListDlg::ResetMenusAndToolbars()
{
    FUNCTION_ENTRY( "ResetMenusAndToolbars" );

    // The appearance of plan editor/controller menus and toolbars should be dictated
    // by the tab that's currently visible.
    if (this->IsWindowVisible() == FALSE)
    {
        FUNCTION_EXIT;
        return;
    }


    PV::PlanInstance *planInstance = m_planInstance;

    bool approved = m_plan->m_isApproved;
    bool automatic = m_plan->m_execMode == PV::Plan::EM_AUTOMATIC;
    bool idle = !planInstance || planInstance && planInstance->m_execState == PV::PlanInstance::ES_IDLE;
    bool running = planInstance && planInstance->m_execState == PV::PlanInstance::ES_RUNNING;
    bool paused = planInstance && planInstance->m_execState == PV::PlanInstance::ES_PAUSED;
    bool mine = planInstance && planInstance->m_owner == "Me !!!";

    bool stepSelected = false;
    bool stepNotProcessed = false;

    POSITION pos = m_planStepListCtrl.GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        stepSelected = true;

        if (!idle)
        {
            PV::StepList steps;
            CPvData::instance().getStepsForPlan(steps, m_plan->m_pkey);
            PV::Step *curStep = CPvData::instance().getStep(planInstance->m_currentStepPkey);
            ASSERT(curStep != NULL);

            if (curStep->m_executionOrder <= steps[m_planStepListCtrl.GetNextSelectedItem(pos)]->m_executionOrder)
            {
                stepNotProcessed = true;
            }

        }
    }

    if (m_menu)
    {
        m_menu->EnableMenuItem(ID_PLAN_SAVE, MF_BYCOMMAND | (approved? MF_GRAYED : MF_ENABLED));
        m_menu->EnableMenuItem(ID_PLAN_APPROVE, MF_BYCOMMAND | (approved? MF_GRAYED : MF_ENABLED));
        m_menu->EnableMenuItem(ID_PLAN_UNAPPROVE, MF_BYCOMMAND | (approved? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_PLAN_VERIFY, MF_BYCOMMAND | MF_ENABLED);
        m_menu->EnableMenuItem(ID_PLAN_EXECUTE, MF_BYCOMMAND | (approved? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_PLAN_STOP, MF_BYCOMMAND | ((running || paused) && mine? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_PLAN_PAUSE, MF_BYCOMMAND | (running && mine? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_PLAN_RESUME, MF_BYCOMMAND | (paused && mine? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_PLAN_TAKECONTROL, MF_BYCOMMAND | ((running || paused) && !mine? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_PLAN_RELEASECONTROL, MF_BYCOMMAND | (automatic && mine? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_PLAN_SCHEDULE, MF_BYCOMMAND | (approved? MF_ENABLED : MF_GRAYED));

        m_menu->EnableMenuItem(ID_STEP_NEW, MF_BYCOMMAND | (idle && !approved? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_STEP_DELETE, MF_BYCOMMAND | (stepSelected && idle && !approved? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_STEP_CUT, MF_BYCOMMAND | (stepSelected && idle && !approved? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_STEP_COPY, MF_BYCOMMAND | (stepSelected && idle && !approved? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_STEP_PASTE, MF_BYCOMMAND | (stepSelected && idle && !approved? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_STEP_EDIT, MF_BYCOMMAND | (stepSelected && idle && !approved? MF_ENABLED : MF_GRAYED));
        m_menu->EnableMenuItem(ID_STEP_CUSTOMISE, MF_BYCOMMAND | (stepSelected && approved && (idle || stepNotProcessed && paused && mine)? MF_ENABLED : MF_GRAYED));
    }

    if (m_editToolBar)
    {
        CToolBarCtrl &toolBarCtrl = m_editToolBar->GetToolBarCtrl();
        toolBarCtrl.EnableButton(ID_PLAN_SAVE, !approved);
        toolBarCtrl.EnableButton(ID_PLAN_APPROVE, !approved);
        toolBarCtrl.EnableButton(ID_PLAN_UNAPPROVE, approved);
        toolBarCtrl.EnableButton(ID_PLAN_VERIFY, TRUE);
        toolBarCtrl.EnableButton(ID_STEP_NEW, idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_DELETE, stepSelected && idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_CUT, stepSelected && idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_COPY, stepSelected && idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_PASTE, stepSelected && idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_EDIT, stepSelected && idle && !approved);

        toolBarCtrl.HideButton(ID_PLAN_APPROVE, approved);
        toolBarCtrl.HideButton(ID_PLAN_UNAPPROVE, !approved);
        toolBarCtrl.HideButton(ID_STEP_NEW, FALSE);
        toolBarCtrl.HideButton(ID_STEP_DELETE, FALSE);
        toolBarCtrl.HideButton(ID_STEP_CUT, FALSE);
        toolBarCtrl.HideButton(ID_STEP_COPY, FALSE);
        toolBarCtrl.HideButton(ID_STEP_PASTE, FALSE);
        toolBarCtrl.HideButton(ID_STEP_EDIT, FALSE);
    }

    if (m_ctrlToolBar)
    {
        CToolBarCtrl &toolBarCtrl = m_ctrlToolBar->GetToolBarCtrl();
        toolBarCtrl.EnableButton(ID_PLAN_EXECUTE, approved);
        toolBarCtrl.EnableButton(ID_PLAN_STOP, (running || paused) && mine);
        toolBarCtrl.EnableButton(ID_PLAN_PAUSE, running && mine);
        toolBarCtrl.EnableButton(ID_PLAN_RESUME, paused && mine);
        toolBarCtrl.EnableButton(ID_PLAN_TAKECONTROL, (running || paused) && !mine);
        toolBarCtrl.EnableButton(ID_PLAN_RELEASECONTROL, automatic && mine);
        toolBarCtrl.EnableButton(ID_PLAN_SCHEDULE, approved);
        toolBarCtrl.EnableButton(ID_STEP_CUSTOMISE, stepSelected && approved && (idle || stepNotProcessed && paused && mine));

        toolBarCtrl.HideButton(ID_STEP_CUSTOMISE, FALSE);
    }

    FUNCTION_EXIT;
}


void CStepListDlg::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (::IsWindow(m_planStatusBox.m_hWnd) && m_plan)
    {
        CRect tmpRect;
        int statusBoxBottom = 0;

        if (m_controlMode || m_readOnly && m_plan->m_isApproved)
        {
            m_planStatusBox.GetWindowRect(tmpRect);
            ScreenToClient(tmpRect);
            tmpRect.right = cx - 10;
            m_planStatusBox.MoveWindow(tmpRect);
            statusBoxBottom = tmpRect.bottom;
        }

        m_planStepsBox.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.top = statusBoxBottom + 10;
        tmpRect.right = cx - 10;
        m_planStepsBox.MoveWindow(tmpRect);
        int topLimit = tmpRect.top;

        m_planStepListCtrl.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.top = statusBoxBottom + 35;
        tmpRect.right = cx - 25;
        m_planStepListCtrl.MoveWindow(tmpRect);

        m_stepParamBox.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 10;
        tmpRect.bottom = cy - 10;
        m_stepParamBox.MoveWindow(tmpRect);
        int bottomLimit = tmpRect.bottom;

        m_stepParamListCtrl.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 25;
        tmpRect.bottom = cy - 25;
        m_stepParamListCtrl.MoveWindow(tmpRect);

        m_splitter.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        int splitterHeight = tmpRect.Height();
        tmpRect.left = 0;
        tmpRect.right = cx;
        tmpRect.top = topLimit + m_splitRatio * (bottomLimit - topLimit) - splitterHeight / 2;
        tmpRect.bottom = tmpRect.top + splitterHeight;
        m_splitter.MoveWindow(tmpRect);
        m_splitter.RecalcLayout();
    }

    FUNCTION_EXIT;
}


void CStepListDlg::OnSelchangePlaninstCombo()
{
    FUNCTION_ENTRY( "OnSelchangePlaninstCombo" );

    // TODO: Add your control notification handler code here
    int selectedInstance = m_planInstCombo.GetCurSel();
    ASSERT(selectedInstance <= m_plan->m_instances.size());

    if (m_controlMode)
    {
        --selectedInstance;
    }
  // First entry is instance "None"

    m_plan->m_selectedInstance = selectedInstance;
    m_planInstance = selectedInstance < 0? &IDLE_INSTANCE : m_plan->m_instances[selectedInstance];

    PopulateData();
    ResetMenusAndToolbars();

    FUNCTION_EXIT;
}


void CStepListDlg::OnDblclkPlanstepList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclkPlanstepList" );

    *pResult = 0;

    // TODO: Add your control notification handler code here
    if (m_menu)
    {
        UINT menuState = m_menu->GetMenuState(ID_STEP_EDIT, MF_BYCOMMAND);

        if (menuState & MF_GRAYED || menuState & MF_DISABLED)
        {
            FUNCTION_EXIT;
            return;
        }

    }

    OnStepEdit();

    FUNCTION_EXIT;
}


void CStepListDlg::OnStepEdit()
{
    FUNCTION_ENTRY( "OnStepEdit" );

    // TODO: Add your command handler code here
    POSITION pos = m_planStepListCtrl.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        FUNCTION_EXIT;
        return;
    }


    int selectedStep = m_planStepListCtrl.GetNextSelectedItem(pos);

    PV::StepList steps;
    CPvData::instance().getStepsForPlan(steps, m_plan->m_pkey);
    ASSERT(selectedStep < steps.size());

    CString stepFileName;
    stepFileName.Format("%u.step", steps[selectedStep]->m_pkey);

    CPlanStepEditDoc *planStepEditDoc = NULL;

    // Q: Is there a simpler way to create/open a document based on document type???
    pos = AfxGetApp()->GetFirstDocTemplatePosition();
    while (pos && !planStepEditDoc)
    {
        CDocTemplate* docTemplate = AfxGetApp()->GetNextDocTemplate(pos);

        CDocTemplate::Confidence conf = docTemplate->MatchDocType(stepFileName, (CDocument *&) planStepEditDoc);
        if (conf == CDocTemplate::Confidence::yesAttemptNative)
        {
            POSITION docPos = docTemplate->GetFirstDocPosition();
            if (docPos)
            {
                planStepEditDoc = (CPlanStepEditDoc *) docTemplate->GetNextDoc(docPos);
            }
            else
            {
                planStepEditDoc = (CPlanStepEditDoc *) docTemplate->OpenDocumentFile(NULL);
            }

            planStepEditDoc->SetPathName(stepFileName, FALSE);
            planStepEditDoc->SetTitle((LPCTSTR) steps[selectedStep]->m_name.c_str());
        }
    }
    ASSERT(planStepEditDoc != NULL);

    pos = planStepEditDoc->GetFirstViewPosition();
    if (pos != NULL)
    {
        CPlanStepForm* planStepEditForm = (CPlanStepForm *) planStepEditDoc->GetNextView(pos);
        if (planStepEditForm != NULL)
        {
            planStepEditForm->PopulateData(steps[selectedStep]);
            planStepEditForm->SetFocus();

            if (m_readOnly == false)
            {
                planStepEditForm->EnableEditing();
            }
        }
    }

    FUNCTION_EXIT;
}


void CStepListDlg::OnStepCustomise()
{
    FUNCTION_ENTRY( "OnStepCustomise" );

    // TODO: Add your command handler code here
    POSITION pos = m_planStepListCtrl.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        FUNCTION_EXIT;
        return;
    }


    int selectedStep = m_planStepListCtrl.GetNextSelectedItem(pos);

    PV::StepList steps;
    CPvData::instance().getStepsForPlan(steps, m_plan->m_pkey);
    ASSERT(selectedStep < steps.size());

    CString stepFileName;
    stepFileName.Format("%u.step", steps[selectedStep]->m_pkey);

    CPlanStepEditDoc *planStepEditDoc = NULL;

    // Q: Is there a simpler way to create/open a document based on document type???
    pos = AfxGetApp()->GetFirstDocTemplatePosition();
    while (pos && !planStepEditDoc)
    {
        CDocTemplate* docTemplate = AfxGetApp()->GetNextDocTemplate(pos);

        CDocTemplate::Confidence conf = docTemplate->MatchDocType(stepFileName, (CDocument *&) planStepEditDoc);
        if (conf == CDocTemplate::Confidence::yesAttemptNative)
        {
            POSITION docPos = docTemplate->GetFirstDocPosition();
            if (docPos)
            {
                planStepEditDoc = (CPlanStepEditDoc *) docTemplate->GetNextDoc(docPos);
            }
            else
            {
                planStepEditDoc = (CPlanStepEditDoc *) docTemplate->OpenDocumentFile(NULL);
            }

            planStepEditDoc->SetPathName(stepFileName, FALSE);
            planStepEditDoc->SetTitle((LPCTSTR) steps[selectedStep]->m_name.c_str());
        }
    }
    ASSERT(planStepEditDoc != NULL);

    pos = planStepEditDoc->GetFirstViewPosition();
    if (pos != NULL)
    {
        CPlanStepForm* planStepEditForm = (CPlanStepForm *) planStepEditDoc->GetNextView(pos);
        if (planStepEditForm != NULL)
        {
            planStepEditForm->PopulateData(steps[selectedStep]);
            planStepEditForm->SetFocus();

            if (m_controlMode == true)
            {
                planStepEditForm->EnableCustomisation();
            }
        }
    }

    FUNCTION_EXIT;
}


void CStepListDlg::OnItemchangedPlanstepList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemchangedPlanstepList" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    POSITION pos = m_planStepListCtrl.GetFirstSelectedItemPosition();

    if (pos != NULL)
    {
        PV::Step *step = CPvData::instance().getStep(m_planStepListCtrl.GetItemData(m_planStepListCtrl.GetNextSelectedItem(pos)));
        ASSERT(step != NULL);

        // Populate the parameter list with parameters relevant to the selected step
        m_stepParamListCtrl.DeleteAllItems();
        for (PV::StepParameterList::iterator iter = step->m_parameters.begin(); iter != step->m_parameters.end(); iter++)
        {
            PV::StepParameter *parameter = *iter;

            int rowIndex = m_stepParamListCtrl.GetItemCount();
            int columnIndex = 1;
            m_stepParamListCtrl.InsertItem(rowIndex, parameter->m_name.c_str());
            m_stepParamListCtrl.SetItemText(rowIndex, columnIndex++, parameter->m_value.c_str());
        }
    }
    else
    {
        m_stepParamListCtrl.DeleteAllItems();
    }

    ResetMenusAndToolbars();

    *pResult = 0;

    FUNCTION_EXIT;
}


void CStepListDlg::OnDblclkStepparamList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclkStepparamList" );

    // TODO: Add your control notification handler code here
/*
    if (m_readOnly)
    {
        FUNCTION_EXIT;
        return;
    }


    POSITION pos = m_stepParamListCtrl.GetFirstSelectedItemPosition();
    while (pos != NULL)
    {
        int paramItem = m_stepParamListCtrl.GetNextSelectedItem(pos);

        pos = m_planStepListCtrl.GetFirstSelectedItemPosition();
        if (pos != NULL)
        {
            int stepItem = m_planStepListCtrl.GetNextSelectedItem(pos);

            PV::Step *step = CPvData::instance().getStep(m_planStepListCtrl.GetItemData(stepItem));
            ASSERT(step != NULL);

            PV::StepParameter *param = step->m_parameters[paramItem];

            CStepParameterDlg parameterDlg(param, &m_stepParamListCtrl);
            if (parameterDlg.DoModal() == IDOK)
            {
                m_stepParamListCtrl.SetItemText(paramItem, 1, param->m_value.c_str());
            }
        }
    }
*/

    *pResult = 0;

    FUNCTION_EXIT;
}


ICellManager::CellType CStepListDlg::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
{
    FUNCTION_ENTRY( "GetCellType" );

    if (listCtrl == &m_planStepListCtrl)
    {
        // Step list: cell type is dictated by the column it's in, regardless of the row.
        if (subItem >= sizeof(STEP_LIST_COLUMN_TYPES) / sizeof(ICellManager::CellType))
        {
            CString message;
            message.Format("No column type available for step list column %d.", subItem);
            MessageBox(message, "Error");

            FUNCTION_EXIT;
            return ICellManager::CT_NOTEDITABLE;
        }
        else
        {
            // In-table editing for customisation purposes only!
            if (m_menu)
            {
                UINT menuState = m_menu->GetMenuState(ID_STEP_CUSTOMISE, MF_BYCOMMAND);

                if (menuState == MF_ENABLED)
                {
                    FUNCTION_EXIT;
                    return STEP_LIST_COLUMN_TYPES[subItem];
                }

            }

            FUNCTION_EXIT;
            return (STEP_LIST_COLUMN_TYPES[subItem] == ICellManager::CT_CHECKBOX? ICellManager::CT_CHECKBOX_DISABLED : ICellManager::CT_NOTEDITABLE);
        }
    }
    else if (listCtrl == &m_stepParamListCtrl)
    {
        FUNCTION_EXIT;
        return ICellManager::CT_NOTEDITABLE;
/*
        // Step parameter list: cell type depends on row (parameter) as well as column.
        if (subItem < 1)
        {
            FUNCTION_EXIT;
            return CellType::CT_NOTEDITABLE;
        }


        POSITION pos = m_planStepListCtrl.GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            FUNCTION_EXIT;
            return CellType::CT_NOTEDITABLE;
        }


        PV::Step *step = CPvData::instance().getStep(m_planStepListCtrl.GetItemData(m_planStepListCtrl.GetNextSelectedItem(pos)));
        ASSERT(step != NULL);

        PV::StepParameter *parameter = step->m_parameters[item];

        if (parameter->m_isSelection)
        {
            FUNCTION_EXIT;
            return CellType::CT_SELECTION;
        }


        if (parameter->m_isNumeric)
        {
            FUNCTION_EXIT;
            return CellType::CT_NUMBER;
        }

        FUNCTION_EXIT;
        return CellType::CT_TEXT;
*/
    }

    CString message;
    message.Format("No cell type information available for list ctrl");
    MessageBox(message, "Error");

    FUNCTION_EXIT;
    return ICellManager::CT_NOTEDITABLE;
}


ICellManager::CellData CStepListDlg::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
{
    FUNCTION_ENTRY( "GetCellData" );

    ICellManager::CellData data;

    // Step parameter list: return valid data values for selection type parameters
    if (listCtrl == &m_stepParamListCtrl && subItem >= 1)
    {
        POSITION pos = m_planStepListCtrl.GetFirstSelectedItemPosition();
        if (pos != NULL)
        {
            PV::Step *step = CPvData::instance().getStep(m_planStepListCtrl.GetItemData(m_planStepListCtrl.GetNextSelectedItem(pos)));
            ASSERT(step != NULL);

            PV::StepParameter *parameter = step->m_parameters[item];

            if (parameter->m_isSelection)
            {
                data = parameter->m_valueList;
            }
        }
    }

    FUNCTION_EXIT;
    return data;
}


bool CStepListDlg::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
{
    FUNCTION_ENTRY( "UpdateCellValue" );
    FUNCTION_EXIT;
    return true;

    // In-table editing is for customisation purposes only and should not
    // affect the general definition of the plan's steps.

/*
    POSITION pos = m_planStepListCtrl.GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        PV::Step *step = CPvData::instance().getStep(m_planStepListCtrl.GetItemData(m_planStepListCtrl.GetNextSelectedItem(pos)));
        ASSERT(step != NULL);

        if (listCtrlId == m_planStepListCtrl.GetDlgCtrlID())
        {
            switch (subItem)
            {
                case 2: // name
                    step->m_name = (LPCTSTR) value;
                    break;

                case 4: // description
                    step->m_description = (LPCTSTR) value;
                    break;

                case 5: // skip
                    step->m_skip = (value == "Yes");
                    break;

                case 6: // delay
                    step->m_delay = atoi((LPCTSTR) value);
                    break;

                default:
                    ;
            }
        }
        else if (listCtrlId == m_stepParamListCtrl.GetDlgCtrlID())
        {
            if (subItem == 1)
            {
                PV::StepParameter *parameter = step->m_parameters[item];

                parameter->m_value = (LPCTSTR) value;
            }
        }
    }
*/
}


