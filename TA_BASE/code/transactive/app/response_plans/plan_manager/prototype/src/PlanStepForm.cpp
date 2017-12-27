/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanStepForm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanStepForm.cpp : implementation file
//

#include "stdafx.h"
#include "PlanViewer.h"
#include "PlanStepForm.h"
#include "MainFrm.h"
#include "StepParameterDlg.h"
#include "PlanStepEditMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanStepForm

IMPLEMENT_DYNCREATE(CPlanStepForm, CFormView)

CPlanStepForm::CPlanStepForm()
    : CFormView(CPlanStepForm::IDD), m_readOnly(true), m_customising(false)
{
    FUNCTION_ENTRY( "CPlanStepForm" );

    // {{AFX_DATA_INIT(CPlanStepForm)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepForm::~CPlanStepForm()
{
    FUNCTION_ENTRY( "~CPlanStepForm" );
    FUNCTION_EXIT;
}


void CPlanStepForm::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CFormView::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepForm)
    DDX_Control(pDX, IDC_NOTES_PICT, m_notes);
    DDX_Control(pDX, IDC_STEPEXEC_BOX, m_stepExecBox);
    DDX_Control(pDX, IDC_STEPSKIPPABLE_CHECK, m_stepSkippableBtn);
    DDX_Control(pDX, IDC_STEPSKIP_CHECK, m_stepSkipBtn);
    DDX_Control(pDX, IDC_STEPPARAM_BOX, m_stepParamBox);
    DDX_Control(pDX, IDC_STEPPARAM_LIST, m_stepParamListCtrl);
    DDX_Control(pDX, IDC_STEPDETAILS_BOX, m_stepDetailsBox);
    DDX_Control(pDX, IDC_STEPTYPE_EDIT, m_stepTypeEdit);
    DDX_Control(pDX, IDC_STEP_DESCRIPTION_EDIT, m_stepDescEdit);
    DDX_Control(pDX, IDC_STEP_NAME_EDIT, m_stepNameEdit);
    DDX_Control(pDX, IDC_SECOND_SPIN, m_secondSpin);
    DDX_Control(pDX, IDC_SECOND_EDIT, m_secondEdit);
    DDX_Control(pDX, IDC_MINUTE_SPIN, m_minuteSpin);
    DDX_Control(pDX, IDC_MINUTE_EDIT, m_minuteEdit);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepForm, CFormView)
    // {{AFX_MSG_MAP(CPlanStepForm)
    ON_WM_SIZE()
    ON_EN_CHANGE(IDC_HOUR_EDIT, OnChangeHourEdit)
    ON_EN_CHANGE(IDC_MINUTE_EDIT, OnChangeMinuteEdit)
    ON_EN_CHANGE(IDC_SECOND_EDIT, OnChangeSecondEdit)
    ON_EN_CHANGE(IDC_STEP_DESCRIPTION_EDIT, OnChangeStepDescriptionEdit)
    ON_EN_CHANGE(IDC_STEP_NAME_EDIT, OnChangeStepNameEdit)
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_BN_CLICKED(IDC_STEPSKIP_CHECK, OnStepskipCheck)
    ON_NOTIFY(NM_DBLCLK, IDC_STEPPARAM_LIST, OnDblclkStepparamList)
    ON_BN_CLICKED(IDC_STEPSKIPPABLE_CHECK, OnStepskippableCheck)
    ON_COMMAND(ID_STEP_EXIT, OnStepExit)
    ON_COMMAND(ID_STEP_CUSTOMOK, OnStepExit)
    ON_COMMAND(ID_STEP_EDITPARAM, OnStepEditparam)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_STEPPARAM_LIST, OnItemchangedStepparamList)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepForm diagnostics

#ifdef _DEBUG
void CPlanStepForm::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFormView::AssertValid();

    FUNCTION_EXIT;
}


void CPlanStepForm::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFormView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanStepForm message handlers

void CPlanStepForm::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CFormView::OnSize(nType, cx, cy);

    if (::IsWindow(m_stepDetailsBox.m_hWnd))
    {
        CRect tmpRect;

        m_stepDetailsBox.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 6;
        m_stepDetailsBox.MoveWindow(tmpRect);

        m_stepNameEdit.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 20;
        m_stepNameEdit.MoveWindow(tmpRect);

        m_stepDescEdit.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 20;
        m_stepDescEdit.MoveWindow(tmpRect);

        m_stepExecBox.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 6;
        m_stepExecBox.MoveWindow(tmpRect);

        m_stepParamBox.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 6;
        tmpRect.bottom = cy - 6;
        m_stepParamBox.MoveWindow(tmpRect);

        m_stepParamListCtrl.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 20;
        tmpRect.bottom = cy - 20;
        m_stepParamListCtrl.MoveWindow(tmpRect);
    }

    FUNCTION_EXIT;
}


void CPlanStepForm::PopulateData(PV::Step *step)
{
    FUNCTION_ENTRY( "PopulateData" );

    m_step = step;

    m_stepSkipBtn.SetCheck(m_step->m_skip? 1 : 0);
    m_stepNameEdit.SetWindowText(m_step->m_name.c_str());
    m_stepDescEdit.SetWindowText(m_step->m_description.c_str());
    m_stepTypeEdit.SetWindowText(m_step->m_type.c_str());

    CString hoursStr, minsStr, secsStr;
    hoursStr.Format("%d", m_step->m_delay / 3600);
    minsStr.Format("%d", (m_step->m_delay % 3600) / 60);
    secsStr.Format("%d", (m_step->m_delay % 3600) % 60);

//    m_hourEdit.SetWindowText(hoursStr);
    m_minuteEdit.SetWindowText(minsStr);
    m_secondEdit.SetWindowText(secsStr);

    OnStepskipCheck();

    // Populate the parameter list with parameters relevant to this step
    m_stepParamListCtrl.DeleteAllItems();
    for (PV::StepParameterList::iterator iter = m_step->m_parameters.begin(); iter != m_step->m_parameters.end(); iter++)
    {
        PV::StepParameter *parameter = *iter;

        int rowIndex = m_stepParamListCtrl.GetItemCount();
        int columnIndex = 1;
        m_stepParamListCtrl.InsertItem(rowIndex, parameter->m_name.c_str());
        m_stepParamListCtrl.SetItemText(rowIndex, columnIndex++, parameter->m_value.c_str());
    }

    ResetMenusAndToolbars();

    FUNCTION_EXIT;
}


void CPlanStepForm::EnableEditing()
{
    FUNCTION_ENTRY( "EnableEditing" );

    m_readOnly = false;

//    m_notes.ShowWindow(SW_SHOW);
    m_notes.AddItem("Double-clicking on an entry in the Parameters list brings up the parameter edit dialog.");
    m_notes.AddItem("In-table editing of parameter values can be performed by single-clicking on the "
                    "appropriate cell within the selected Parameters list entry.");

    CListCtrl listCtrl; listCtrl.Create(0, CRect(0, 0, 0, 0), this, 0);
    COLORREF listCtrlDefBkColor = listCtrl.GetBkColor();

    m_stepParamListCtrl.SetBkColor(listCtrlDefBkColor);
    m_stepParamListCtrl.SetTextBkColor(listCtrlDefBkColor);
    m_stepParamListCtrl.ModifyStyle(0, LVS_EDITLABELS);

    m_stepNameEdit.SetReadOnly(FALSE);
    m_stepDescEdit.SetReadOnly(FALSE);

    m_minuteEdit.SetReadOnly(FALSE);
    m_secondEdit.SetReadOnly(FALSE);

    m_stepSkippableBtn.EnableWindow(TRUE);
    OnStepskippableCheck();

    ResetMenusAndToolbars();

    FUNCTION_EXIT;
}


void CPlanStepForm::EnableCustomisation()
{
    FUNCTION_ENTRY( "EnableCustomisation" );

    m_readOnly = false;
    m_customising = true;

    m_minuteEdit.SetReadOnly(FALSE);
    m_secondEdit.SetReadOnly(FALSE);

    OnStepskippableCheck();

    ResetMenusAndToolbars();

    FUNCTION_EXIT;
}


void CPlanStepForm::ResetMenusAndToolbars()
{
    FUNCTION_ENTRY( "ResetMenusAndToolbars" );

    CPlanStepEditMainFrame *seFrame = dynamic_cast<CPlanStepEditMainFrame *>(GetParentFrame());
    if (seFrame && !m_readOnly)
    {
        if (m_customising)
        {
            seFrame->m_toolBar.GetToolBarCtrl().EnableButton(ID_STEP_CUSTOMOK, TRUE);
            seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_CUSTOMOK, FALSE);
            seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_SAVE, TRUE);
            seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_EDITPARAM, TRUE);
            seFrame->GetMenu()->RemoveMenu(1, MF_BYPOSITION);
            seFrame->DrawMenuBar();
        }
        else
        {
            bool paramSelected = m_stepParamListCtrl.GetFirstSelectedItemPosition() != NULL;

            seFrame->m_toolBar.GetToolBarCtrl().EnableButton(ID_STEP_SAVE, TRUE);
            seFrame->m_toolBar.GetToolBarCtrl().EnableButton(ID_STEP_EDITPARAM, paramSelected);
            seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_SAVE, FALSE);
            seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_EDITPARAM, FALSE);
            seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_CUSTOMOK, TRUE);
            seFrame->GetMenu()->EnableMenuItem(ID_STEP_SAVE, MF_BYCOMMAND | MF_ENABLED);
            seFrame->GetMenu()->EnableMenuItem(ID_STEP_EDITPARAM, MF_BYCOMMAND | (paramSelected? MF_ENABLED : MF_GRAYED));
        }
    }
    else
    {
        seFrame->m_toolBar.GetToolBarCtrl().EnableButton(ID_STEP_SAVE, FALSE);
        seFrame->m_toolBar.GetToolBarCtrl().EnableButton(ID_STEP_EDITPARAM, FALSE);
        seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_SAVE, FALSE);
        seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_EDITPARAM, FALSE);
        seFrame->m_toolBar.GetToolBarCtrl().HideButton(ID_STEP_CUSTOMOK, TRUE);
        seFrame->GetMenu()->EnableMenuItem(ID_STEP_SAVE, MF_BYCOMMAND | MF_GRAYED);
        seFrame->GetMenu()->EnableMenuItem(ID_STEP_EDITPARAM, MF_BYCOMMAND | MF_GRAYED);
    }

    FUNCTION_EXIT;
}


void CPlanStepForm::OnInitialUpdate()
{
    FUNCTION_ENTRY( "OnInitialUpdate" );

    CFormView::OnInitialUpdate();

    // TODO: Add your specialized code here and/or call the base class
    m_notes.ShowWindow(SW_HIDE);
    m_stepSkippableBtn.SetCheck(1);

//    m_hourSpin.SetRange(0,99);
    m_minuteSpin.SetRange(0,59);
    m_secondSpin.SetRange(0,59);

    m_stepParamListCtrl.SetExtendedStyle(m_stepParamListCtrl.GetExtendedStyle() |
                                         LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    m_stepParamListCtrl.SetBkColor(::GetSysColor(COLOR_3DFACE));
    m_stepParamListCtrl.SetTextBkColor(m_stepParamListCtrl.GetBkColor());

    int columnIndex = 0;
    m_stepParamListCtrl.InsertColumn(columnIndex++, "Name", LVCFMT_LEFT, 200);
    m_stepParamListCtrl.InsertColumn(columnIndex++, "Value", LVCFMT_LEFT, 300);

    FUNCTION_EXIT;
}


void CPlanStepForm::OnChangeHourEdit()
{
    FUNCTION_ENTRY( "OnChangeHourEdit" );

    // if(::IsWindow(m_hourEdit.m_hWnd))
//        SetStepDelay();

    FUNCTION_EXIT;
}


void CPlanStepForm::OnChangeMinuteEdit()
{
    FUNCTION_ENTRY( "OnChangeMinuteEdit" );

    if(::IsWindow(m_minuteEdit.m_hWnd))
    {
        SetStepDelay();
    }

    FUNCTION_EXIT;
}


void CPlanStepForm::OnChangeSecondEdit()
{
    FUNCTION_ENTRY( "OnChangeSecondEdit" );

    if(::IsWindow(m_secondEdit.m_hWnd))
    {
        SetStepDelay();
    }

    FUNCTION_EXIT;
}


void CPlanStepForm::OnChangeStepDescriptionEdit()
{
    FUNCTION_ENTRY( "OnChangeStepDescriptionEdit" );

    /*
    CString stepDesc;
    m_stepDescEdit.GetWindowText(stepDesc);
    ((CMainFrame*) AfxGetApp()->m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STEP_SAVE, TRUE);
    ((CMainFrame*) AfxGetApp()->m_pMainWnd)->GetMenu()->EnableMenuItem(ID_STEP_SAVE, MF_ENABLED | MF_BYCOMMAND);
*/

    FUNCTION_EXIT;
}


void CPlanStepForm::OnChangeStepNameEdit()
{
    FUNCTION_ENTRY( "OnChangeStepNameEdit" );

    /*
    CString stepName;
    m_stepNameEdit.GetWindowText(stepName);
    ((CMainFrame*) AfxGetApp()->m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STEP_SAVE, TRUE);
    ((CMainFrame*) AfxGetApp()->m_pMainWnd)->GetMenu()->EnableMenuItem(ID_STEP_SAVE, MF_ENABLED | MF_BYCOMMAND);
*/

    FUNCTION_EXIT;
}


unsigned long CPlanStepForm::CalculateStepDelay()
{
    FUNCTION_ENTRY( "CalculateStepDelay" );

    CString hours, mins, secs;
//    m_hourEdit.GetWindowText(hours);
    m_minuteEdit.GetWindowText(mins);
    m_secondEdit.GetWindowText(secs);

    FUNCTION_EXIT;
    return /* 3600 * atoi(hours) + */ 60 * atoi(mins) + atoi(secs);
}


void CPlanStepForm::SetStepDelay()
{
    FUNCTION_ENTRY( "SetStepDelay" );

    /*
    ((CMainFrame*) AfxGetApp()->m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().EnableButton(ID_STEP_SAVE, TRUE);
    ((CMainFrame*) AfxGetApp()->m_pMainWnd)->GetMenu()->EnableMenuItem(ID_STEP_SAVE, MF_ENABLED | MF_BYCOMMAND);
*/

    FUNCTION_EXIT;
}


void CPlanStepForm::OnKillFocus(CWnd* pNewWnd)
{
    FUNCTION_ENTRY( "OnKillFocus" );

    CFormView::OnKillFocus(pNewWnd);

    // TODO: Add your message handler code here

    FUNCTION_EXIT;
}


void CPlanStepForm::OnSetFocus(CWnd* pNewWnd)
{
    FUNCTION_ENTRY( "OnSetFocus" );

    CFormView::OnSetFocus(pNewWnd);
    // TODO: Add your message handler code here

    FUNCTION_EXIT;
}


void CPlanStepForm::OnStepskippableCheck()
{
    FUNCTION_ENTRY( "OnStepskippableCheck" );

    // TODO: Add your control notification handler code here
    if (m_readOnly)
    {
        FUNCTION_EXIT;
        return;
    }


    BOOL enableStepSkipBtn = m_stepSkippableBtn.GetCheck() == 0? FALSE : TRUE;

    m_stepSkipBtn.EnableWindow(enableStepSkipBtn);

    if (enableStepSkipBtn == FALSE)
    {
        m_minuteEdit.EnableWindow(TRUE);   m_minuteSpin.EnableWindow(TRUE);
        m_secondEdit.EnableWindow(TRUE);   m_secondSpin.EnableWindow(TRUE);
    }
    else
    {
        OnStepskipCheck();
    }

    FUNCTION_EXIT;
}


void CPlanStepForm::OnStepskipCheck()
{
    FUNCTION_ENTRY( "OnStepskipCheck" );

    // TODO: Add your control notification handler code here
    if (m_readOnly)
    {
        FUNCTION_EXIT;
        return;
    }


    BOOL enableDelay = m_stepSkipBtn.GetCheck() == 0? TRUE : FALSE;

//    m_hourEdit.EnableWindow(enableDelay);     m_hourSpin.EnableWindow(enableDelay);
    m_minuteEdit.EnableWindow(enableDelay);   m_minuteSpin.EnableWindow(enableDelay);
    m_secondEdit.EnableWindow(enableDelay);   m_secondSpin.EnableWindow(enableDelay);

    FUNCTION_EXIT;
}


void CPlanStepForm::OnDblclkStepparamList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclkStepparamList" );

    // TODO: Add your control notification handler code here
    OnStepEditparam();

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepForm::OnItemchangedStepparamList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemchangedStepparamList" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    ResetMenusAndToolbars();

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepForm::OnStepExit()
{
    FUNCTION_ENTRY( "OnStepExit" );

    // TODO: Add your command handler code here
    GetParentFrame()->DestroyWindow();

    FUNCTION_EXIT;
}


void CPlanStepForm::OnStepEditparam()
{
    FUNCTION_ENTRY( "OnStepEditparam" );

    // TODO: Add your command handler code here
    if (m_readOnly)
    {
        FUNCTION_EXIT;
        return;
    }


    POSITION pos = m_stepParamListCtrl.GetFirstSelectedItemPosition();
    while (pos != NULL)
    {
        int item = m_stepParamListCtrl.GetNextSelectedItem(pos);

        PV::StepParameter *param = m_step->m_parameters[item];

        CStepParameterDlg parameterDlg(param, &m_stepParamListCtrl);
        if (parameterDlg.DoModal() == IDOK)
        {
            m_stepParamListCtrl.SetItemText(item, 1, param->m_value.c_str());
        }
    }

    FUNCTION_EXIT;
}


