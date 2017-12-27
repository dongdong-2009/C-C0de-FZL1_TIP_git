//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditRunPlanPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_RUN_PLAN)

#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanStepEditRunPlanPage.h"
#include "RunPlanStep.h"
#include "YesNoStepParameter.h"
#include "PlanNode.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditRunPlanPage dialog

CPlanStepEditRunPlanPage::CPlanStepEditRunPlanPage(RunPlanStep* step)
    : CPlanStepEditParameterPage(CPlanStepEditRunPlanPage::IDD, step),
    m_planPathStepParameter(*step->getPathParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditRunPlanPage" );

    // {{AFX_DATA_INIT(CPlanStepEditRunPlanPage)
    m_planPathLabel = _T("");
    m_manualLaunch = FALSE;
    m_waitComplete = FALSE;
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepEditRunPlanPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditRunPlanPage)
    DDX_Text(pDX, IDC_PLAN_PATH, m_planPathLabel);
    DDX_Check(pDX, IDC_MANUAL_LAUNCH, m_manualLaunch);
    DDX_Check(pDX, IDC_WAIT_FOR_COMPLETION, m_waitComplete);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditRunPlanPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditRunPlanPage)
    ON_BN_CLICKED(IDC_PICK_PLAN_PATH_BUTTON, OnPickPlanPathButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditRunPlanPage message handlers

BOOL CPlanStepEditRunPlanPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    RunPlanStep* step = dynamic_cast<RunPlanStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the run step page");

    m_manualLaunch = step->getLaunchParameter()->getAffirmative();
    m_waitComplete = step->getWaitParameter()->getAffirmative();

    UpdateData(FALSE);

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditRunPlanPage::OnPickPlanPathButton()
{
    FUNCTION_ENTRY( "OnPickPlanPathButton" );

    UpdateData(); // TD#1537

    m_planPathStepParameter.editParameter();

    DisplayParameters();

    FUNCTION_EXIT;
}


void CPlanStepEditRunPlanPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* planPathButton = GetDlgItem(IDC_PICK_PLAN_PATH_BUTTON);
    if (planPathButton)
    {
        planPathButton->EnableWindow(enable);
    }

    CWnd* manualLaunch = GetDlgItem(IDC_MANUAL_LAUNCH);
    if (manualLaunch)
    {
        manualLaunch->EnableWindow(enable);
    }

    CWnd* waitForCompletion = GetDlgItem(IDC_WAIT_FOR_COMPLETION);
    if (waitForCompletion)
    {
        waitForCompletion->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditRunPlanPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    try
    {
        m_planPathLabel= m_planPathStepParameter.getPlanPath().c_str();
    }
    catch (...)
    {
        // Couldn't retrieve the plan path - just set it to nothing.
        m_planPathLabel = "";
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditRunPlanPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    RunPlanStep* step = dynamic_cast<RunPlanStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the run plan step page");

    PlanPathStepParameter* pathStepParameter = step->getPathParameter();
    pathStepParameter->setPlanNodeId(m_planPathStepParameter.getPlanNodeId());

    YesNoStepParameter* parameter = step->getLaunchParameter();
    parameter->setAffirmative(m_manualLaunch != 0);
    parameter = step->getWaitParameter();
    parameter->setAffirmative(m_waitComplete != 0);

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_RUN_PLAN)

