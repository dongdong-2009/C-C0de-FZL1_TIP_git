/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanForm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanForm.cpp : implementation file
//

#include "stdafx.h"
#include "PlanViewer.h"
#include "PlanForm.h"
#include "MainFrm.h"
#include "PlanDetailsDlg.h"
#include "StepListDlg.h"
#include "PlanFlowDlg.h"
#include "PvData.h"
#include "Globals.h"
#include "PlanEditMainFrm.h"
#include "PlanControlMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPlanForm

IMPLEMENT_DYNCREATE(CPlanForm, CFormView)

CPlanForm::CPlanForm()
: CFormView(CPlanForm::IDD), m_selectedInstance(0), m_plan(NULL), m_readOnly(true), m_controlMode(false)
{
    FUNCTION_ENTRY( "CPlanForm" );

    // {{AFX_DATA_INIT(CPlanForm)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanForm::~CPlanForm()
{
    FUNCTION_ENTRY( "~CPlanForm" );

    for (TabItemList::iterator iter = m_tabItems.begin(); iter != m_tabItems.end(); iter++)
    {
        CDialog *dlg = *iter;
        delete dlg;
    }

    m_tabItems.clear();

    FUNCTION_EXIT;
}


void CPlanForm::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CFormView::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanForm)
    DDX_Control(pDX, IDC_PLANEDIT_TAB, m_planEditTabCtrl);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanForm, CFormView)
    // {{AFX_MSG_MAP(CPlanForm)
    ON_WM_SIZE()
    ON_NOTIFY(TCN_SELCHANGE, IDC_PLANEDIT_TAB, OnSelchangePlaneditTab)
    ON_COMMAND(ID_STEP_EDIT, OnStepEdit)
    ON_COMMAND(ID_STEP_CUSTOMISE, OnStepCustomise)
    ON_COMMAND(ID_PLAN_EXIT, OnPlanExit)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanForm diagnostics

#ifdef _DEBUG
void CPlanForm::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFormView::AssertValid();

    FUNCTION_EXIT;
}


void CPlanForm::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFormView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanForm message handlers

void CPlanForm::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CFormView::OnSize(nType, cx, cy);

    if (::IsWindow(m_planEditTabCtrl.m_hWnd))
    {
        CRect tmpRect;

        m_planEditTabCtrl.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 10;
        tmpRect.bottom = cy - 10;
        m_planEditTabCtrl.MoveWindow(tmpRect);

        m_planEditTabCtrl.GetClientRect(tmpRect);
        m_planEditTabCtrl.AdjustRect(FALSE, tmpRect);
        m_tabItems[m_planEditTabCtrl.GetCurSel()]->MoveWindow(tmpRect);
        m_tabItems[m_planEditTabCtrl.GetCurSel()]->SendMessage(WM_SIZE, nType, MAKEWPARAM(tmpRect.Width(), tmpRect.Height()));
    }

    FUNCTION_EXIT;
}


void CPlanForm::PopulateData(PV::Plan *plan)
{
    FUNCTION_ENTRY( "PopulateData" );

    ASSERT(plan != NULL);

    m_plan = plan;
    m_selectedInstance = 0;

    for (TabItemList::iterator iter = m_tabItems.begin(); iter != m_tabItems.end(); iter++)
    {
        CDialog *dlg = *iter;
        dlg->SendMessage(WM_UPDATE_PLAN_DETAILS, (WPARAM) m_plan);
    }

    FUNCTION_EXIT;
}


void CPlanForm::EnableEditing()
{
    FUNCTION_ENTRY( "EnableEditing" );

    m_readOnly = false;

    for (TabItemList::iterator iter = m_tabItems.begin(); iter != m_tabItems.end(); iter++)
    {
        CDialog *dlg = *iter;
        dlg->SendMessage(WM_ENABLE_EDITING);
    }

    FUNCTION_EXIT;
}


void CPlanForm::EnableControl()
{
    FUNCTION_ENTRY( "EnableControl" );

    m_controlMode = true;

    for (TabItemList::iterator iter = m_tabItems.begin(); iter != m_tabItems.end(); iter++)
    {
        CDialog *dlg = *iter;
        dlg->SendMessage(WM_ENABLE_CONTROL);
    }

    FUNCTION_EXIT;
}


void CPlanForm::Resize()
{
    FUNCTION_ENTRY( "Resize" );

    CRect rect;
    m_planEditTabCtrl.GetClientRect(rect);
    m_planEditTabCtrl.AdjustRect(FALSE, rect);

    for (TabItemList::iterator iter = m_tabItems.begin(); iter != m_tabItems.end(); iter++)
    {
        CDialog *dlg = *iter;
        dlg->SendMessage(WM_SIZE, SIZE_RESTORED, MAKEWPARAM(rect.Width(), rect.Height()));
    }

    FUNCTION_EXIT;
}


void CPlanForm::OnInitialUpdate()
{
    FUNCTION_ENTRY( "OnInitialUpdate" );

    CFormView::OnInitialUpdate();


    m_planEditTabCtrl.InsertItem(m_planEditTabCtrl.GetItemCount(), "General");
    m_planEditTabCtrl.InsertItem(m_planEditTabCtrl.GetItemCount(), "Steps");
//    m_planEditTabCtrl.InsertItem(m_planEditTabCtrl.GetItemCount(), "Flow");

    CMenu *menu = NULL;
    CToolBar *editToolBar = NULL;
    CToolBar *ctrlToolBar = NULL;

    CPlanEditMainFrame *peFrame = dynamic_cast<CPlanEditMainFrame *>(GetParentFrame());
    if (peFrame)
    {
        menu = peFrame->GetMenu();
        editToolBar = &peFrame->m_editToolBar;
    }
    else
    {
        CPlanControlMainFrame *pcFrame = dynamic_cast<CPlanControlMainFrame *>(GetParentFrame());
        if (pcFrame)
        {
            menu = pcFrame->GetMenu();
            ctrlToolBar = &pcFrame->m_ctrlToolBar;
        }
    }

    CPlanDetailsDlg *planDetailsDlg = new CPlanDetailsDlg(menu, editToolBar, ctrlToolBar);
    planDetailsDlg->Create(CPlanDetailsDlg::IDD, &m_planEditTabCtrl);
    planDetailsDlg->ShowWindow(SW_SHOW);

    CStepListDlg *stepListDlg = new CStepListDlg(menu, editToolBar, ctrlToolBar);
    stepListDlg->Create(CStepListDlg::IDD, &m_planEditTabCtrl);
    stepListDlg->ShowWindow(SW_HIDE);

/*
    CPlanFlowDlg *planFlowDlg = new CPlanFlowDlg(menu, editToolBar, ctrlToolBar);
    planFlowDlg->Create(CPlanFlowDlg::IDD, &m_planEditTabCtrl);
    planFlowDlg->ShowWindow(SW_HIDE);
*/

    m_tabItems.push_back(planDetailsDlg);
    m_tabItems.push_back(stepListDlg);
//    m_tabItems.push_back(planFlowDlg);

    FUNCTION_EXIT;
}


void CPlanForm::OnSelchangePlaneditTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnSelchangePlaneditTab" );

    // TODO: Add your control notification handler code here
    int selItem = m_planEditTabCtrl.GetCurSel();
    int itemCount = m_tabItems.size();

    ASSERT(selItem < itemCount);

    for (int i = 0; i < itemCount; i++)
    {
        CDialog *dlg = m_tabItems[i];

        if (i == selItem)
        {
            CRect rect;
            m_planEditTabCtrl.GetClientRect(rect);
            m_planEditTabCtrl.AdjustRect(FALSE, rect);
            dlg->MoveWindow(rect);
            dlg->ShowWindow(SW_SHOW);
            dlg->SendMessage(WM_UPDATE_PLAN_DETAILS, (WPARAM) m_plan);
            dlg->SendMessage(WM_SIZE, SIZE_RESTORED, MAKEWPARAM(rect.Width(), rect.Height()));
        }
        else
        {
            dlg->ShowWindow(SW_HIDE);
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanForm::OnStepEdit()
{
    FUNCTION_ENTRY( "OnStepEdit" );

    // TODO: Add your command handler code here
    for (TabItemList::iterator iter = m_tabItems.begin(); iter != m_tabItems.end(); iter++)
    {
        CDialog *dlg = *iter;
        dlg->PostMessage(WM_COMMAND, MAKEWPARAM(ID_STEP_EDIT, 0), (LPARAM)NULL);
    }

    FUNCTION_EXIT;
}


void CPlanForm::OnStepCustomise()
{
    FUNCTION_ENTRY( "OnStepCustomise" );

    // TODO: Add your command handler code here
    for (TabItemList::iterator iter = m_tabItems.begin(); iter != m_tabItems.end(); iter++)
    {
        CDialog *dlg = *iter;
        dlg->PostMessage(WM_COMMAND, MAKEWPARAM(ID_STEP_CUSTOMISE, 0), (LPARAM)NULL);
    }

    FUNCTION_EXIT;
}


void CPlanForm::OnPlanExit()
{
    FUNCTION_ENTRY( "OnPlanExit" );

    // TODO: Add your command handler code here
    GetParentFrame()->DestroyWindow();

    FUNCTION_EXIT;
}


