//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterStepNumberEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"
#include "planmanager.h"
#include "PlanStepParameterStepNumberEditDlg.h"
#include "StepNumberStepParameter.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "core/utilities/src/TAAssert.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterStepNumberEditDlg dialog

CPlanStepParameterStepNumberEditDlg::CPlanStepParameterStepNumberEditDlg(StepNumberStepParameter& parameter,bool canEdit)
    : CPlanStepParameterEditDlg(parameter,CPlanStepParameterStepNumberEditDlg::IDD,canEdit), m_stepList(false)

{
    FUNCTION_ENTRY( "CPlanStepParameterStepNumberEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterStepNumberEditDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterStepNumberEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterStepNumberEditDlg)
    DDX_Control(pDX, IDC_STEP_LIST, m_stepList);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterStepNumberEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterStepNumberEditDlg)
    ON_BN_CLICKED(IDOK, onOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterStepNumberEditDlg message handlers

BOOL CPlanStepParameterStepNumberEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    PlanStepParameter*  planStepParameter = GetStepParameter();
    PlanStep*           planStep = planStepParameter->getParentStep();
    PlanNode*           plan = planStep->getParentPlan();

    // Load all the steps for the selected plan

    plan->addStepsToStepList(m_stepList);

    // Select the current step
    StepNumberStepParameter* planStepNumberStepParameter = dynamic_cast<StepNumberStepParameter*>(GetStepParameter());
    TA_ASSERT(planStepNumberStepParameter!=NULL,"Not a StepNumberStepParameter");
    PlanStepId  planStepId = planStepNumberStepParameter->getStepId();
    planStep = plan->getPlanStep(planStepId);

    m_stepList.SetSelectedStep(planStep, true);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepParameterStepNumberEditDlg::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    // Get the selected step from the list control
    PlanStep* planStep = m_stepList.GetSelectedStep();
    if (planStep!=NULL)
    {
        // Update the jump to step
        StepNumberStepParameter* planStepParameter = dynamic_cast<StepNumberStepParameter*>(GetStepParameter());
        TA_ASSERT(planStepParameter!=NULL,"Not a StepNumberStepParameter");
        planStepParameter->setStepId(planStep->getStepId());
    }

    // Normal exit
    EndDialog(IDOK);

    FUNCTION_EXIT;
}


