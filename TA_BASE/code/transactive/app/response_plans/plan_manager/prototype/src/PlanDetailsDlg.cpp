/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanDetailsDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanDetailsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "PlanDetailsDlg.h"
#include "Globals.h"
#include "PvData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanDetailsDlg dialog

CPlanDetailsDlg::CPlanDetailsDlg(CMenu *menu, CToolBar *editToolBar, CToolBar *ctrlToolBar)
    : CDialog(CPlanDetailsDlg::IDD, NULL), m_plan(NULL), m_menu(menu), m_editToolBar(editToolBar), m_ctrlToolBar(ctrlToolBar),
      m_readOnly(true), m_controlMode(false), m_splitRatio(0.75)
{
    FUNCTION_ENTRY( "CPlanDetailsDlg" );

    m_notes.AddItem("Double-clicking on the plan in the tree brings up a plan edit/control window.");
    m_notes.AddItem("Active Instances list could be hidden and the Details section stretched all the "
                    "way to the bottom if the user does not have plan control rights.");
    m_notes.AddItem("The horizontal splitter bar can be moved up and down to resize the Active Instances list.");

    FUNCTION_EXIT;
}


void CPlanDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanDetailsDlg)
    DDX_Control(pDX, IDC_PLANAPPSTATUS_EDIT, m_planAppStatusEdit);
    DDX_Control(pDX, IDC_NOTES_PICT, m_notes);
    DDX_Control(pDX, IDC_PLANEXECMODE_EDIT, m_planExecModeEdit);
    DDX_Control(pDX, IDC_PLAN_NAME_EDIT, m_planNameEdit);
    DDX_Control(pDX, IDC_PLAN_DESCRIPTION_EDIT, m_planDescEdit);
    DDX_Control(pDX, IDC_ACTIVEPLANS_BOX, m_activePlansBox);
    DDX_Control(pDX, IDC_ACTIVEPLAN_LIST, m_activePlansListCtrl);
    DDX_Control(pDX, IDC_PLANDETAILS_BOX, m_planDetailsBox);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanDetailsDlg, CDialog)
    // {{AFX_MSG_MAP(CPlanDetailsDlg)
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_ACTIVEPLAN_LIST, OnDblclkActiveplanList)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_ACTIVEPLAN_LIST, OnItemchangedActiveplanList)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_UPDATE_PLAN_DETAILS, OnUpdatePlanDetails)
    ON_MESSAGE(WM_ENABLE_EDITING, OnEnableEditing)
    ON_MESSAGE(WM_ENABLE_CONTROL, OnEnableControl)
    ON_MESSAGE(WM_SPLITTER_MOVED, OnSplitterMoved)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanDetailsDlg message handlers

BOOL CPlanDetailsDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    m_activePlansListCtrl.SetExtendedStyle(m_activePlansListCtrl.GetExtendedStyle() |
                                           LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    m_activePlansListCtrl.SetBkColor(::GetSysColor(COLOR_3DFACE));
    m_activePlansListCtrl.SetTextBkColor(m_activePlansListCtrl.GetBkColor());


    int columnIndex = 0;
    m_activePlansListCtrl.InsertColumn(columnIndex++, "Instance", LVCFMT_RIGHT, 55);
    m_activePlansListCtrl.InsertColumn(columnIndex++, "Owner", LVCFMT_LEFT, 100);
    m_activePlansListCtrl.InsertColumn(columnIndex++, "Activation", LVCFMT_LEFT, 120);
    m_activePlansListCtrl.InsertColumn(columnIndex++, "State", LVCFMT_LEFT, 100);
    m_activePlansListCtrl.InsertColumn(columnIndex++, "Step", LVCFMT_LEFT, 200);
    m_activePlansListCtrl.InsertColumn(columnIndex++, "Remarks", LVCFMT_LEFT, 200);

    // Separate step and parameter lists with a movable splitter bar
    m_splitter.BindWithControl(this, IDC_SPLITBAR);
    m_splitter.SetMinHeight(180, 0);
    m_splitter.AttachAbove(m_planDetailsBox.GetDlgCtrlID(), 12);
    m_splitter.AttachAbove(m_planDescEdit.GetDlgCtrlID(), 27);
    m_splitter.AttachBelow(m_activePlansBox.GetDlgCtrlID(), 10);
    m_splitter.AttachBelow(m_activePlansListCtrl.GetDlgCtrlID(), 35);
    m_splitter.RecalcLayout();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CPlanDetailsDlg::OnUpdatePlanDetails(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnUpdatePlanDetails" );

    m_plan = (PV::Plan *) wParam;

    m_planNameEdit.SetWindowText(m_plan->m_name.c_str());
    m_planDescEdit.SetWindowText(m_plan->m_description.c_str());

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
    m_planAppStatusEdit.SetWindowText(m_plan->m_isApproved? "Approved" : "Unapproved");

    // Remove current entries from the active instances list
    m_activePlansListCtrl.DeleteAllItems();

    // Unapproved plans cannot become active.
    m_activePlansListCtrl.EnableWindow(m_plan->m_isApproved);
    if (m_plan->m_isApproved)
    {
        // Populate the active instance list with the plan's instances
        for (PV::PlanInstanceList::iterator iter = m_plan->m_instances.begin(); iter != m_plan->m_instances.end(); iter++)
        {
            PV::PlanInstance *instance = *iter;
            PV::Step *curStep = CPvData::instance().getStep(instance->m_currentStepPkey);

            CTime activationTime = CTime(time(NULL) - (rand() % 300));

            CString instanceNumStr, actTimeStr, execStateStr, currentStepStr;
            instanceNumStr.Format("%d", instance->m_instanceNumber);
            actTimeStr = activationTime.Format("%Y/%m/%d %H:%M:%S");
            execStateStr = instance->m_execState == PV::PlanInstance::ExecState::ES_RUNNING? "Running" : "Paused";
            currentStepStr.Format("%d - %s", curStep->m_executionOrder, curStep->m_name.c_str());

            int rowIndex = m_activePlansListCtrl.GetItemCount();
            int columnIndex = 1;
            m_activePlansListCtrl.InsertItem(rowIndex, instanceNumStr);
            m_activePlansListCtrl.SetItemText(rowIndex, columnIndex++, instance->m_owner.c_str());
            m_activePlansListCtrl.SetItemText(rowIndex, columnIndex++, actTimeStr);
            m_activePlansListCtrl.SetItemText(rowIndex, columnIndex++, execStateStr);
            m_activePlansListCtrl.SetItemText(rowIndex, columnIndex++, currentStepStr);
            m_activePlansListCtrl.SetItemText(rowIndex, columnIndex++, "Plan loaded"); // remarks
        }

        // Set the active instance selection
        if (m_plan->m_selectedInstance >= 0 && m_plan->m_selectedInstance < m_activePlansListCtrl.GetItemCount())
        {
            m_activePlansListCtrl.SetItemState(m_plan->m_selectedInstance, 0xFF, LVIS_SELECTED | LVIS_FOCUSED);
            m_activePlansListCtrl.EnsureVisible(m_plan->m_selectedInstance, FALSE);
        }
    }

    if (m_readOnly)
    {
        m_activePlansBox.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_activePlansListCtrl.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
        m_splitter.ShowWindow(m_plan->m_isApproved? SW_SHOW : SW_HIDE);
    }

    ResetMenusAndToolbars();

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlanDetailsDlg::OnEnableEditing(WPARAM /* wParam */, LPARAM /* lParam */)
{
    FUNCTION_ENTRY( "OnEnableEditing" );

    m_readOnly = false;
    m_controlMode = false;

    m_notes.AddItem("Edit functionality will be available only on unapproved plans. To edit an approved "
                    "plan, the plan must be unapproved or a copy of it made.");
    m_notes.AddItem("Plan control rules:\n"
                    "- user must have plan execution/control rights to access the functionality\n"
                    "- new instances of the plan can be started at any time\n"
                    "- existing instances can be stopped/paused/resumed only by their owner\n"
                    "- ownership is obtained by either starting a new instance or taking control "
                      "of an existing one\n"
                    "- control of instances can be taken only given appropriate privileges\n"
                    "- control of instances can be released only if the plan has automatic "
                      "execution mode -OR- when another user requests to take the control.");

    // Approved plans cannot be edited
    if (m_plan->m_isApproved == false)
    {
        m_planNameEdit.SetReadOnly(FALSE);
        m_planDescEdit.SetReadOnly(FALSE);
    }

    m_activePlansBox.ShowWindow(SW_HIDE);
    m_activePlansListCtrl.ShowWindow(SW_HIDE);

    m_splitter.ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlanDetailsDlg::OnEnableControl(WPARAM /* wParam */, LPARAM /* lParam */)
{
    FUNCTION_ENTRY( "OnEnableControl" );

    m_controlMode = true;

    CListCtrl listCtrl; listCtrl.Create(0, CRect(0, 0, 0, 0), this, 0);
    COLORREF listCtrlDefBkColor = listCtrl.GetBkColor();

    m_activePlansListCtrl.SetBkColor(listCtrlDefBkColor);
    m_activePlansListCtrl.SetTextBkColor(listCtrlDefBkColor);

    m_activePlansBox.ShowWindow(SW_SHOW);
    m_activePlansListCtrl.ShowWindow(SW_SHOW);

    m_splitter.ShowWindow(SW_SHOW);

    FUNCTION_EXIT;
    return 0;
}


LRESULT CPlanDetailsDlg::OnSplitterMoved(WPARAM wParam, LPARAM lParam)
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


void CPlanDetailsDlg::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (::IsWindow(m_planDetailsBox.m_hWnd) && m_plan)
    {
        // Plan details group box occupies 75% of the vertical space available, with the
        // remaining 25% taken up by the active instances group box. Of all plan detail
        // controls, only the description edit is resized.
        // Note that this code is good enough for prototype purposes, however, in a
        // commercial product it would require more work to ensure that no control overlap
        // occurs when the window is sized down. (The same goes for the annoying flicker.)

        CRect tmpRect;

        m_planDetailsBox.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 10;
        if (!(m_controlMode || m_readOnly && m_plan->m_isApproved))
        {
            tmpRect.bottom = cy - 10;
        }

        m_planDetailsBox.MoveWindow(tmpRect);
        int topLimit = tmpRect.top;

        m_planNameEdit.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 25;
        m_planNameEdit.MoveWindow(tmpRect);

        m_planDescEdit.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 25;
        if (!(m_controlMode || m_readOnly && m_plan->m_isApproved))
        {
            tmpRect.bottom = cy - 25;
        }

        m_planDescEdit.MoveWindow(tmpRect);

        if (m_controlMode || m_readOnly && m_plan->m_isApproved)
        {
            m_activePlansBox.GetWindowRect(tmpRect);
            ScreenToClient(tmpRect);
            tmpRect.right = cx - 10;
            tmpRect.bottom = cy - 10;
            m_activePlansBox.MoveWindow(tmpRect);
            int bottomLimit = tmpRect.bottom;

            m_activePlansListCtrl.GetWindowRect(tmpRect);
            ScreenToClient(tmpRect);
            tmpRect.right = cx - 25;
            tmpRect.bottom = cy - 25;
            m_activePlansListCtrl.MoveWindow(tmpRect);

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
    }

    FUNCTION_EXIT;
}


void CPlanDetailsDlg::OnDblclkActiveplanList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclkActiveplanList" );

    // TODO: Add your control notification handler code here
    POSITION pos = m_activePlansListCtrl.GetFirstSelectedItemPosition();
    while (pos != NULL)
    {
        int item = m_activePlansListCtrl.GetNextSelectedItem(pos);

        // Display plan edit/execution window for the selected instance???
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanDetailsDlg::OnItemchangedActiveplanList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemchangedActiveplanList" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    int selectedInstance = -1;
    POSITION pos = m_activePlansListCtrl.GetFirstSelectedItemPosition();

    if (pos != NULL)
    {
        selectedInstance = m_activePlansListCtrl.GetNextSelectedItem(pos);
    }

    m_plan->m_selectedInstance = selectedInstance;

    ResetMenusAndToolbars();

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanDetailsDlg::ResetMenusAndToolbars()
{
    FUNCTION_ENTRY( "ResetMenusAndToolbars" );

    // The appearance of plan editor/controller menus and toolbars should be dictated
    // by the tab that's currently visible.
    if (this->IsWindowVisible() == FALSE)
    {
        FUNCTION_EXIT;
        return;
    }


    PV::PlanInstance *planInstance = NULL;
    if (m_plan->m_instances.size() > 0 && m_plan->m_selectedInstance >= 0)
    {
        planInstance = m_plan->m_instances[m_plan->m_selectedInstance];
    }


    bool approved = m_plan->m_isApproved;
    bool automatic = m_plan->m_execMode == PV::Plan::EM_AUTOMATIC;
    bool idle = planInstance && planInstance->m_execState == PV::PlanInstance::ES_IDLE;
    bool running = planInstance && planInstance->m_execState == PV::PlanInstance::ES_RUNNING;
    bool paused = planInstance && planInstance->m_execState == PV::PlanInstance::ES_PAUSED;
    bool mine = planInstance && planInstance->m_owner == "Me !!!";

    if (m_menu)
    {
        m_menu->EnableMenuItem(ID_PLAN_SAVE, MF_BYCOMMAND | (approved? MF_GRAYED : MF_ENABLED));
        m_menu->EnableMenuItem(ID_PLAN_APPROVE, MF_BYCOMMAND | (approved? MF_GRAYED : MF_ENABLED));
        m_menu->EnableMenuItem(ID_PLAN_UNAPPROVE, MF_BYCOMMAND | (approved? MF_ENABLED : MF_GRAYED));
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
        m_menu->EnableMenuItem(ID_STEP_CUSTOMISE, MF_BYCOMMAND | MF_GRAYED);
    }

    if (m_editToolBar)
    {
        CToolBarCtrl &toolBarCtrl = m_editToolBar->GetToolBarCtrl();
        toolBarCtrl.EnableButton(ID_PLAN_SAVE, !approved);
        toolBarCtrl.EnableButton(ID_PLAN_APPROVE, !approved);
        toolBarCtrl.EnableButton(ID_PLAN_UNAPPROVE, approved);
        toolBarCtrl.EnableButton(ID_PLAN_VERIFY, TRUE);
        toolBarCtrl.EnableButton(ID_STEP_NEW, FALSE);
        toolBarCtrl.EnableButton(ID_STEP_DELETE, FALSE);
        toolBarCtrl.EnableButton(ID_STEP_CUT, FALSE);
        toolBarCtrl.EnableButton(ID_STEP_COPY, FALSE);
        toolBarCtrl.EnableButton(ID_STEP_PASTE, FALSE);
        toolBarCtrl.EnableButton(ID_STEP_EDIT, FALSE);

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

        toolBarCtrl.HideButton(ID_STEP_CUSTOMISE, TRUE);
    }

    FUNCTION_EXIT;
}


