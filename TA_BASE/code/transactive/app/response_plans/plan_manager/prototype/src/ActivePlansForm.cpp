/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ActivePlansForm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ActivePlansForm.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "ActivePlansForm.h"
#include "PvData.h"
#include "Globals.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActivePlansForm

IMPLEMENT_DYNCREATE(CActivePlansForm, CFormView)

CActivePlansForm::CActivePlansForm()
    : CFormView(CActivePlansForm::IDD)
{
    FUNCTION_ENTRY( "CActivePlansForm" );

    // {{AFX_DATA_INIT(CActivePlansForm)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    m_notes.AddItem("This will be the default startup view for users without plan editing rights.");
    m_notes.AddItem("Double-click on an entry in the Active Instances list to bring up the "
                    "corresponding plan control window.");
    m_notes.AddItem("Vertical splitter to the left of the Active Instances list can be dragged "
                    "right to reveal the Active Plans tree. This tree demonstrates the concept "
                    "of 'Flat Mode' (View->Flat Mode).");

    FUNCTION_EXIT;
}


CActivePlansForm::~CActivePlansForm()
{
    FUNCTION_ENTRY( "~CActivePlansForm" );
    FUNCTION_EXIT;
}


void CActivePlansForm::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CFormView::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CActivePlansForm)
    DDX_Control(pDX, IDC_NOTES_PICT, m_notes);
    DDX_Control(pDX, IDC_ALLACTIVEPLANINST_LIST, m_allActivePlansListCtrl);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CActivePlansForm, CFormView)
    // {{AFX_MSG_MAP(CActivePlansForm)
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_ALLACTIVEPLANINST_LIST, OnDblclkAllactiveplaninstList)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActivePlansForm diagnostics

#ifdef _DEBUG
void CActivePlansForm::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFormView::AssertValid();

    FUNCTION_EXIT;
}


void CActivePlansForm::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFormView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CActivePlansForm message handlers

void CActivePlansForm::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    CFormView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (::IsWindow(m_allActivePlansListCtrl.m_hWnd))
    {
        CRect tmpRect;

        m_allActivePlansListCtrl.GetWindowRect(tmpRect);
        ScreenToClient(tmpRect);
        tmpRect.right = cx - 10;
        tmpRect.bottom = cy - 10;
        m_allActivePlansListCtrl.MoveWindow(tmpRect);
    }

    FUNCTION_EXIT;
}


void CActivePlansForm::OnInitialUpdate()
{
    FUNCTION_ENTRY( "OnInitialUpdate" );

    CFormView::OnInitialUpdate();

    // TODO: Add your specialized code here and/or call the base class
    m_allActivePlansListCtrl.SetExtendedStyle(m_allActivePlansListCtrl.GetExtendedStyle() |
                                              LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    int columnIndex = 0;
    m_allActivePlansListCtrl.InsertColumn(columnIndex++, "Plan", LVCFMT_LEFT, 100);
    m_allActivePlansListCtrl.InsertColumn(columnIndex++, "Category", LVCFMT_LEFT, 100);
    m_allActivePlansListCtrl.InsertColumn(columnIndex++, "Instance", LVCFMT_RIGHT, 55);
    m_allActivePlansListCtrl.InsertColumn(columnIndex++, "Owner", LVCFMT_LEFT, 100);
    m_allActivePlansListCtrl.InsertColumn(columnIndex++, "Activation", LVCFMT_LEFT, 120);
    m_allActivePlansListCtrl.InsertColumn(columnIndex++, "State", LVCFMT_LEFT, 100);
    m_allActivePlansListCtrl.InsertColumn(columnIndex++, "Step", LVCFMT_LEFT, 200);
    m_allActivePlansListCtrl.InsertColumn(columnIndex++, "Remarks", LVCFMT_LEFT, 200);

    FUNCTION_EXIT;
}


void CActivePlansForm::PopulateData()
{
    FUNCTION_ENTRY( "PopulateData" );

    ASSERT(::IsWindow(m_allActivePlansListCtrl.m_hWnd));

    PV::PlanList plans;
    CPvData::instance().getActivePlans(plans);

    // Sort active plans by name
    std::sort(plans.begin(), plans.end(), CActivePlansForm::PlanCompare);

    // Remove current entries from the active instances list
    m_allActivePlansListCtrl.DeleteAllItems();

    // Populate the active instance list with all plans' active instances
    for (PV::PlanList::iterator piter = plans.begin(); piter != plans.end(); piter++)
    {
        PV::Plan *plan = *piter;
        PV::Category *category = CPvData::instance().getCategory(plan->m_categoryPkey);

        CString categoryPath;
        categoryPath.Format("/%s", category->m_name.c_str()); // assume all categories hang off the root

        for (PV::PlanInstanceList::iterator iiter = plan->m_instances.begin(); iiter != plan->m_instances.end(); iiter++)
        {
            PV::PlanInstance *instance = *iiter;
            PV::Step *curStep = CPvData::instance().getStep(instance->m_currentStepPkey);

            CTime activationTime = CTime(time(NULL) - (rand() % 300));

            CString instanceNumStr, actTimeStr, execStateStr, currentStepStr, remarkStr;
            instanceNumStr.Format("%d", instance->m_instanceNumber);
            actTimeStr = activationTime.Format("%Y/%m/%d %H:%M:%S");
            execStateStr = instance->m_execState == PV::PlanInstance::ExecState::ES_RUNNING? "Running" : "Paused";
            currentStepStr.Format("%d - %s", curStep->m_executionOrder, curStep->m_name.c_str());
            remarkStr = instance->m_execState == PV::PlanInstance::ExecState::ES_PAUSED && rand() % 3 == 0? "Step failed" : "Plan loaded";

            int rowIndex = m_allActivePlansListCtrl.GetItemCount();
            int columnIndex = 1;
            m_allActivePlansListCtrl.InsertItem(rowIndex, plan->m_name.c_str());
            m_allActivePlansListCtrl.SetItemText(rowIndex, columnIndex++, categoryPath);
            m_allActivePlansListCtrl.SetItemText(rowIndex, columnIndex++, instanceNumStr);
            m_allActivePlansListCtrl.SetItemText(rowIndex, columnIndex++, instance->m_owner.c_str());
            m_allActivePlansListCtrl.SetItemText(rowIndex, columnIndex++, actTimeStr);
            m_allActivePlansListCtrl.SetItemText(rowIndex, columnIndex++, execStateStr);
            m_allActivePlansListCtrl.SetItemText(rowIndex, columnIndex++, currentStepStr);
            m_allActivePlansListCtrl.SetItemText(rowIndex, columnIndex++, remarkStr);
        }
    }

    FUNCTION_EXIT;
}


bool CActivePlansForm::PlanCompare(const PV::Plan *plan1, const PV::Plan *plan2)
{
    FUNCTION_ENTRY( "PlanCompare" );
    FUNCTION_EXIT;
    return plan1->m_name < plan2->m_name;
}


void CActivePlansForm::OnDblclkAllactiveplaninstList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclkAllactiveplaninstList" );

    // TODO: Add your control notification handler code here
    PV::PlanList plans;
    CPvData::instance().getActivePlans(plans);

    POSITION pos = m_allActivePlansListCtrl.GetFirstSelectedItemPosition();
    while (pos != NULL)
    {
        int item = m_allActivePlansListCtrl.GetNextSelectedItem(pos);
        CString planName = m_allActivePlansListCtrl.GetItemText(item, 0);
        int planInstance = atoi((LPCTSTR) m_allActivePlansListCtrl.GetItemText(item, 1));

        for (PV::PlanList::iterator piter = plans.begin(); piter != plans.end(); piter++)
        {
            PV::Plan *plan = *piter;

            if (planName == CString(plan->m_name.c_str()))
            {
                for (PV::PlanInstanceList::iterator iiter = plan->m_instances.begin(); iiter != plan->m_instances.end(); iiter++)
                {
                    PV::PlanInstance *instance = *iiter;

                    if (planInstance == instance->m_instanceNumber)
                    {
                        plan->m_selectedInstance = instance->m_instanceNumber - 1;
                        ASSERT(plan->m_selectedInstance >= 0 && plan->m_selectedInstance < plan->m_instances.size());

                        CMainFrame* mainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
                        ASSERT(mainFrame != NULL);

                        CPlansTreeView *treeView = mainFrame->GetTree();
                        ASSERT(treeView != NULL);

                        treeView->PostMessage(WM_EDIT_PLAN, plan->m_pkey);
                    }
                }
            }
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


