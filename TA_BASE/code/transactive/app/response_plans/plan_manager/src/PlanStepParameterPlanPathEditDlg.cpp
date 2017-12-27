//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterPlanPathEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/plan_tree/src/PlanItem.h"
#include "PlanManager.h"
#include "PlanStepParameterPlanPathEditDlg.h"
#include "PlanPathStepParameter.h"
#include "PlanStep.h"
#include "PlanNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterPlanPathEditDlg dialog

CPlanStepParameterPlanPathEditDlg::CPlanStepParameterPlanPathEditDlg(PlanPathStepParameter& parameter,bool canEdit)
    : CPlanStepParameterEditDlg(parameter,CPlanStepParameterPlanPathEditDlg::IDD, canEdit)
{
    FUNCTION_ENTRY( "CPlanStepParameterPlanPathEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterPlanPathEditDlg)
    m_planPath = _T("");
    // }}AFX_DATA_INIT

    // We want the plan selector tree to show only approved plans + the current plan (which should be unapproved).
    // We therefore specify the plan filter as PFT_APPROVED_PLANS but make the current plan unfilterable.
    // An example of where the user may want to select the current plan is in an Active Plan Check step that
    // they want to use to make sure that only one instance of the plan runs at a time.
    std::vector<TA_Base_Core::NodeId> nonFilterablePlans;
    nonFilterablePlans.push_back(parameter.getParentStep()->getParentPlan()->getNodeId());

    m_planTree = new TA_Base_Bus::CPlanTreeCtrl(TA_Base_Bus::PlanFilter::PFT_APPROVED_PLANS, nonFilterablePlans, false);

    FUNCTION_EXIT;
}


CPlanStepParameterPlanPathEditDlg::~CPlanStepParameterPlanPathEditDlg()
{
    FUNCTION_ENTRY( "~CPlanStepParameterPlanPathEditDlg" );

    delete m_planTree;

    FUNCTION_EXIT;
}


void CPlanStepParameterPlanPathEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterPlanPathEditDlg)
    DDX_Control(pDX, IDC_PLAN_TREE, *m_planTree);
    DDX_Text(pDX, IDC_PLAN_PATH, m_planPath);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterPlanPathEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterPlanPathEditDlg)
    ON_NOTIFY(TVN_SELCHANGED, IDC_PLAN_TREE, OnPlanTreeItemSelected)
    ON_BN_CLICKED(IDOK, OnOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterPlanPathEditDlg message handlers

BOOL CPlanStepParameterPlanPathEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    // TODO: Add extra initialization here
    PlanPathStepParameter* parameter = dynamic_cast<PlanPathStepParameter*>(GetStepParameter());
    TA_ASSERT(parameter!=NULL,"Incorrect parameter type.");

    // Select the appropriate plan if one is associated with the parameter.
    if (parameter->getPlanNodeId() > 0)
    {
        m_planTree->setSelectedPlan(parameter->getPlanNodeId());
    }

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterPlanPathEditDlg::OnPlanTreeItemSelected(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnPlanTreeItemSelected" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    TA_Base_Bus::CPlanItem* planItem = m_planTree->getSelectedPlan();

    m_planPath = (planItem != NULL? _T(planItem->getPlanPath().c_str()) : _T(""));
    UpdateData(FALSE);

    CWnd* okButton = GetDlgItem(IDOK);
    if (okButton)
    {
        okButton->EnableWindow(planItem != NULL);
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CPlanStepParameterPlanPathEditDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    PlanPathStepParameter* parameter = dynamic_cast<PlanPathStepParameter*>(GetStepParameter());
    TA_ASSERT(parameter!=NULL,"Incorrect parameter type.");

    TA_Base_Bus::CPlanItem* planItem = m_planTree->getSelectedPlan();
    TA_ASSERT(planItem != NULL, "No plan selected in the tree");

    parameter->setPlanNodeId(planItem->getPlanId());

    EndDialog(IDOK);

    FUNCTION_EXIT;
}


