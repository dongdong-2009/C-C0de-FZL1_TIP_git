/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanFlowDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanFlowDlg.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "PlanFlowDlg.h"
#include "PvData.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static PV::PlanInstance IDLE_INSTANCE;


/////////////////////////////////////////////////////////////////////////////
// CPlanFlowDlg dialog


CPlanFlowDlg::CPlanFlowDlg(CMenu *menu, CToolBar *editToolBar, CToolBar *ctrlToolBar)
    : CDialog(CPlanFlowDlg::IDD, NULL), m_plan(NULL), m_readOnly(true), m_controlMode(false), m_menu(menu), m_editToolBar(editToolBar), m_ctrlToolBar(ctrlToolBar)
{
    FUNCTION_ENTRY( "CPlanFlowDlg" );

    m_notes.AddItem("Plan Status section is duplicated between the 'Steps' and 'Flow' tabs. "
                    "Would it be better to have only one tab where the user toggles between "
                    "a list view and a flow chart view of the steps?");
    m_notes.AddItem("Flow chart layout will be saved with the plan.");
    m_notes.AddItem("Colour codes can be used to show successful/failed steps, skippable/skipped steps "
                    "as well as the currently executing step");
    m_notes.AddItem("If flow chart does not fit in its entirety within the viewing area, "
                    "veritical and/or horizontal scroll bars will be shown to allow the "
                    "user to view the hidden parts. (What about pan/zoom?)");

    FUNCTION_EXIT;
}


void CPlanFlowDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanFlowDlg)
    DDX_Control(pDX, IDC_PLANOWNER_LABEL, m_planOwnerLabel);
    DDX_Control(pDX, IDC_PLANINST_LABEL, m_planInstLabel);
    DDX_Control(pDX, IDC_PLANEXECSTATE_LABEL, m_planExecStateLabel);
    DDX_Control(pDX, IDC_PLANEXECMODE_LABEL, m_planExecModeLabel);
    DDX_Control(pDX, IDC_FLOW_PICT, m_flowImage);
    DDX_Control(pDX, IDC_PLANINST_EDIT, m_planInstEdit);
    DDX_Control(pDX, IDC_PLANSTATUS_BOX, m_planStatusBox);
    DDX_Control(pDX, IDC_PLANFLOW_BOX, m_planFlowBox);
    DDX_Control(pDX, IDC_PLANOWNER_EDIT, m_planOwnerEdit);
    DDX_Control(pDX, IDC_PLANINST_COMBO, m_planInstCombo);
    DDX_Control(pDX, IDC_PLANEXECSTATE_EDIT, m_planExecStateEdit);
    DDX_Control(pDX, IDC_PLANEXECMODE_EDIT, m_planExecModeEdit);
    DDX_Control(pDX, IDC_FLOW_VER_SBAR, m_flowVSBar);
    DDX_Control(pDX, IDC_FLOW_HOR_SBAR, m_flowHSBar);
    DDX_Control(pDX, IDC_NOTES_PICT, m_notes);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanFlowDlg, CDialog)
    // {{AFX_MSG_MAP(CPlanFlowDlg)
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_CBN_SELCHANGE(IDC_PLANINST_COMBO, OnSelchangePlaninstCombo)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_UPDATE_PLAN_DETAILS, OnUpdatePlanDetails)
    ON_MESSAGE(WM_ENABLE_EDITING, OnEnableEditing)
    ON_MESSAGE(WM_ENABLE_CONTROL, OnEnableControl)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanFlowDlg message handlers

BOOL CPlanFlowDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanFlowDlg::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (::IsWindow(m_planStatusBox.m_hWnd) && m_plan)
    {
        CRect boxRect, vSBarRect, hSBarRect, imgRect;
        int statusBoxBottom = 0;

        if (m_controlMode || m_readOnly && m_plan->m_isApproved)
        {
            m_planStatusBox.GetWindowRect(boxRect);
            ScreenToClient(boxRect);
            boxRect.right = cx - 10;
            m_planStatusBox.MoveWindow(boxRect);
            statusBoxBottom = boxRect.bottom;
        }

        m_planFlowBox.GetWindowRect(boxRect);
        ScreenToClient(boxRect);
        boxRect.top = statusBoxBottom + 10;
        boxRect.right = cx - 10;
        boxRect.bottom = cy - 10;
        m_planFlowBox.MoveWindow(boxRect);
        int topLimit = boxRect.top + 7;
        int rightLimit = boxRect.right - 2;
        int bottomLimit = boxRect.bottom - 2;

        m_flowVSBar.GetWindowRect(vSBarRect);
        ScreenToClient(vSBarRect);
        int vSBarWidth = vSBarRect.Width();

        m_flowHSBar.GetWindowRect(hSBarRect);
        ScreenToClient(hSBarRect);
        int hSBarHeight = hSBarRect.Height();

        m_flowImage.GetWindowRect(imgRect);
        ScreenToClient(imgRect);
        int imgHeight = imgRect.Height();
        imgRect.top = statusBoxBottom + 50;
        imgRect.bottom = imgRect.top + imgHeight;
        m_flowImage.MoveWindow(imgRect);
        int hiddenOnRight = imgRect.right - (m_flowVSBar.IsWindowVisible()? rightLimit - vSBarWidth : rightLimit);
        int hiddenBelow = imgRect.bottom - (m_flowHSBar.IsWindowVisible()? bottomLimit - hSBarHeight : bottomLimit);

        vSBarRect.right = rightLimit;
        vSBarRect.left = vSBarRect.right - vSBarWidth;
        vSBarRect.top = topLimit;
        vSBarRect.bottom = (hiddenOnRight > 0)? bottomLimit - hSBarHeight : bottomLimit;
        m_flowVSBar.MoveWindow(vSBarRect);
        m_flowVSBar.ShowScrollBar(hiddenBelow > 0);

        hSBarRect.bottom = bottomLimit;
        hSBarRect.top = hSBarRect.bottom - hSBarHeight;
        hSBarRect.right = (hiddenBelow > 0)? rightLimit - vSBarWidth : rightLimit;
        m_flowHSBar.MoveWindow(hSBarRect);
        m_flowHSBar.ShowScrollBar(hiddenOnRight > 0);

        CRect imgClipRect;
        imgClipRect.left = imgClipRect.top = 0;
        imgClipRect.right = imgRect.Width() - hiddenOnRight;
        imgClipRect.bottom = imgRect.Height() - hiddenBelow;
        m_flowImage.SetPaintRect(imgClipRect);
        m_flowImage.Draw();
    }

    FUNCTION_EXIT;
}


void CPlanFlowDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    FUNCTION_ENTRY( "OnVScroll" );

    // TODO: Add your message handler code here and/or call default
    if (pScrollBar == &m_flowVSBar)
    {
        // TODO
    }

    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

    FUNCTION_EXIT;
}


void CPlanFlowDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    FUNCTION_ENTRY( "OnHScroll" );

    // TODO: Add your message handler code here and/or call default
    if (pScrollBar == &m_flowHSBar)
    {
        // TODO
    }

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

    FUNCTION_EXIT;
}


void CPlanFlowDlg::OnSelchangePlaninstCombo()
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


LRESULT CPlanFlowDlg::OnUpdatePlanDetails(WPARAM wParam, LPARAM lParam)
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


LRESULT CPlanFlowDlg::OnEnableEditing(WPARAM /* wParam */, LPARAM /* lParam */)
{
    FUNCTION_ENTRY( "OnEnableEditing" );

    m_readOnly = false;
    m_controlMode = false;

    m_notes.AddItem("Double-clicking on a step blob should bring up the step edit window.");
    m_notes.AddItem("Step edit/customisation rules documented on the 'Steps' tab should also apply here.");

    m_planStatusBox.ShowWindow(SW_HIDE);
    m_planInstEdit.ShowWindow(SW_HIDE);
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


LRESULT CPlanFlowDlg::OnEnableControl(WPARAM /* wParam */, LPARAM /* lParam */)
{
    FUNCTION_ENTRY( "OnEnableControl" );

    m_controlMode = true;

    m_planStatusBox.ShowWindow(SW_SHOW);
    m_planInstEdit.ShowWindow(m_planInstCombo.GetCount() == 0);
    m_planInstCombo.ShowWindow(m_planInstCombo.GetCount() > 0);
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


void CPlanFlowDlg::PopulateData()
{
    FUNCTION_ENTRY( "PopulateData" );

    ASSERT(m_plan != NULL);

    if (m_readOnly)
    {
        m_planStatusBox.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planInstEdit.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planInstCombo.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planOwnerEdit.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planExecStateEdit.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planExecModeEdit.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planInstLabel.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planOwnerLabel.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planExecStateLabel.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_planExecModeLabel.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
    }

    // Drop the current flow chart
    m_flowImage.UnLoad();

    if (m_controlMode || m_readOnly && m_plan->m_isApproved)
    {
        if (m_planInstance)
        {
            // Hide the plan instance read-only edit and show a combo in its place
            m_planInstEdit.ShowWindow(SW_HIDE);
            m_planInstCombo.ShowWindow(SW_SHOW);

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
                execState = "Idle";
            else
                execState = "Undefined";
            m_planExecStateEdit.SetWindowText(execState);

            // Load the flow chart
            if (m_planInstance->m_execState == PV::PlanInstance::ExecState::ES_IDLE)
            {
                VERIFY(m_flowImage.Load(MAKEINTRESOURCE(IDR_STEPFLOW_INACTIVE_JPG),_T("JPG")));
            }

            else
                VERIFY(m_flowImage.Load(MAKEINTRESOURCE(IDR_STEPFLOW_ACTIVE_JPG),_T("JPG")));
        }
        else
        {
            // Hide the plan instance combo and show a read-only edit in its place
            m_planInstCombo.ShowWindow(SW_HIDE);
            m_planInstEdit.ShowWindow(SW_SHOW);

            // Set edits.
            m_planInstEdit.SetWindowText("None");
            m_planOwnerEdit.SetWindowText("None");
            m_planExecStateEdit.SetWindowText("Idle");

            // Load the flow chart
            VERIFY(m_flowImage.Load(MAKEINTRESOURCE(IDR_STEPFLOW_INACTIVE_JPG),_T("JPG")));
        }

        // Set execution mode edit
        CString execMode;
        if (m_plan->m_execMode == PV::Plan::ExecMode::EM_AUTOMATIC)
        {
            execMode = "Automatic";
        }

        else if (m_plan->m_execMode == PV::Plan::ExecMode::EM_INTERACTIVE)
            execMode = "Interactive";
        else
            execMode = "Undefined";
        m_planExecModeEdit.SetWindowText(execMode);
    }
    else
    {
        // Load the flow chart
        VERIFY(m_flowImage.Load(MAKEINTRESOURCE(IDR_STEPFLOW_INACTIVE_JPG),_T("JPG")));
    }

    // Redraw the loaded flow chart
    VERIFY(m_flowImage.Draw());

    FUNCTION_EXIT;
}


void CPlanFlowDlg::ResetMenusAndToolbars()
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

    if (m_menu)
    {
        CString suffixStr;

        if (planInstance && planInstance->m_instanceNumber > 0)
        {
            suffixStr.Format(" Instance %d", planInstance->m_instanceNumber);
        }

        else
            suffixStr = " Instance";

        m_menu->ModifyMenu(ID_PLAN_STOP, MF_BYCOMMAND | MF_STRING, ID_PLAN_STOP, "S&top" + suffixStr + "\t    Ctrl+S");
        m_menu->ModifyMenu(ID_PLAN_PAUSE, MF_BYCOMMAND | MF_STRING, ID_PLAN_PAUSE, "&Pause" + suffixStr + "\t    Ctrl+P");
        m_menu->ModifyMenu(ID_PLAN_RESUME, MF_BYCOMMAND | MF_STRING, ID_PLAN_RESUME, "&Resume" + suffixStr + "\t    Ctrl+R");
        m_menu->ModifyMenu(ID_PLAN_TAKECONTROL, MF_BYCOMMAND | MF_STRING, ID_PLAN_TAKECONTROL, "Ta&ke Control of" + suffixStr);
        m_menu->ModifyMenu(ID_PLAN_RELEASECONTROL, MF_BYCOMMAND | MF_STRING, ID_PLAN_RELEASECONTROL, "Re&lease Control of" + suffixStr);

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

        m_menu->EnableMenuItem(ID_STEP_NEW, MF_BYCOMMAND | MF_GRAYED);
        m_menu->EnableMenuItem(ID_STEP_DELETE, MF_BYCOMMAND | MF_GRAYED);
        m_menu->EnableMenuItem(ID_STEP_CUT, MF_BYCOMMAND | MF_GRAYED);
        m_menu->EnableMenuItem(ID_STEP_COPY, MF_BYCOMMAND | MF_GRAYED);
        m_menu->EnableMenuItem(ID_STEP_PASTE, MF_BYCOMMAND | MF_GRAYED);
        m_menu->EnableMenuItem(ID_STEP_EDIT, MF_BYCOMMAND | MF_GRAYED);
        m_menu->EnableMenuItem(ID_STEP_CUSTOMISE, MF_BYCOMMAND | (approved && (idle || paused && mine)? MF_ENABLED : MF_GRAYED));
    }

    if (m_editToolBar)
    {
        CToolBarCtrl &toolBarCtrl = m_editToolBar->GetToolBarCtrl();
        toolBarCtrl.EnableButton(ID_PLAN_SAVE, !approved);
        toolBarCtrl.EnableButton(ID_PLAN_APPROVE, !approved);
        toolBarCtrl.EnableButton(ID_PLAN_UNAPPROVE, approved);
        toolBarCtrl.EnableButton(ID_PLAN_VERIFY, TRUE);
        toolBarCtrl.EnableButton(ID_STEP_NEW, idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_DELETE, idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_CUT, idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_COPY, idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_PASTE, idle && !approved);
        toolBarCtrl.EnableButton(ID_STEP_EDIT, idle && !approved);

        toolBarCtrl.HideButton(ID_PLAN_APPROVE, approved);
        toolBarCtrl.HideButton(ID_PLAN_UNAPPROVE, !approved);
        toolBarCtrl.HideButton(ID_STEP_NEW, TRUE);
        toolBarCtrl.HideButton(ID_STEP_DELETE, TRUE);
        toolBarCtrl.HideButton(ID_STEP_CUT, TRUE);
        toolBarCtrl.HideButton(ID_STEP_COPY, TRUE);
        toolBarCtrl.HideButton(ID_STEP_PASTE, TRUE);
        toolBarCtrl.HideButton(ID_STEP_EDIT, TRUE);
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

        toolBarCtrl.HideButton(ID_PLAN_TAKECONTROL, idle || mine);
        toolBarCtrl.HideButton(ID_PLAN_RELEASECONTROL, idle || !mine);
        toolBarCtrl.HideButton(ID_STEP_CUSTOMISE, FALSE);

    }

    FUNCTION_EXIT;
}


