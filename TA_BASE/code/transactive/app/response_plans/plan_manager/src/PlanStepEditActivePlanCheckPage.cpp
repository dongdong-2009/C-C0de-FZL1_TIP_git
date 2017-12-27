//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditActivePlanCheckPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"

#if defined (STEPTYPE_ACTIVE_PLAN_CHECK)

#include "planmanager.h"
#include "PlanStepEditActivePlanCheckPage.h"
#include "ActivePlanCheckStep.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditActivePlanCheckPage dialog


CPlanStepEditActivePlanCheckPage::CPlanStepEditActivePlanCheckPage(ActivePlanCheckStep* step)
    : CPlanStepEditParameterPage(CPlanStepEditActivePlanCheckPage::IDD, step),
    m_noStepParameter(*step->getNoParameter()),
    m_yesStepParameter(*step->getYesParameter()),
    m_planPathStepParameter(*step->getPathParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditActivePlanCheckPage" );

    // {{AFX_DATA_INIT(CPlanStepEditActivePlanCheckPage)
    m_noStepLabel = _T("");
    m_yesStepLabel = _T("");
    m_planPathLabel = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditActivePlanCheckPage)
    DDX_Text(pDX, IDC_NO_STEP, m_noStepLabel);
    DDX_Text(pDX, IDC_YES_STEP, m_yesStepLabel);
    DDX_Text(pDX, IDC_PLAN_PATH, m_planPathLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditActivePlanCheckPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditActivePlanCheckPage)
    ON_BN_CLICKED(IDC_PICK_NO_STEP_BUTTON, OnPickNoStepButton)
    ON_BN_CLICKED(IDC_PICK_YES_STEP_BUTTON, OnPickYesStepButton)
    ON_BN_CLICKED(IDC_PICK_PLAN_PATH_BUTTON, OnPickPlanPathButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditActivePlanCheckPage message handlers

void CPlanStepEditActivePlanCheckPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* planPathButton = GetDlgItem(IDC_PICK_PLAN_PATH_BUTTON);
    if (planPathButton)
    {
        planPathButton->EnableWindow(enable);
    }

    CWnd* yesStepButton = GetDlgItem(IDC_PICK_YES_STEP_BUTTON);
    if (yesStepButton)
    {
        yesStepButton->EnableWindow(enable);
    }

    CWnd* noStepButton = GetDlgItem(IDC_PICK_NO_STEP_BUTTON);
    if (noStepButton)
    {
        noStepButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    try
    {
        m_planPathLabel= m_planPathStepParameter.getPlanPath().c_str();
    }
    catch(TA_Base_Core::TransactiveException&)
    {
        // Couldn't retrieve the plan path - just set it to nothing.
        m_planPathLabel = "";
    }

    m_yesStepLabel = m_yesStepParameter.getDestinationStepDescription().c_str();
    m_noStepLabel  = m_noStepParameter.getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::OnPickNoStepButton()
{
    FUNCTION_ENTRY( "OnPickNoStepButton" );

    m_noStepParameter.editParameter();

    m_noStepLabel  = m_noStepParameter.getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::OnPickYesStepButton()
{
    FUNCTION_ENTRY( "OnPickYesStepButton" );

    m_yesStepParameter.editParameter();

    m_yesStepLabel = m_yesStepParameter.getDestinationStepDescription().c_str();

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::OnPickPlanPathButton()
{
    FUNCTION_ENTRY( "OnPickPlanPathButton" );

    m_planPathStepParameter.editParameter();

    try
    {
        m_planPathLabel= m_planPathStepParameter.getPlanPath().c_str();
    }
    catch(TA_Base_Core::TransactiveException&)
    {
        // Couldn't retrieve the plan path - just set it to nothing.
        m_planPathLabel = "";
    }

    UpdateData(FALSE);

    FUNCTION_EXIT;
}


void CPlanStepEditActivePlanCheckPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    ActivePlanCheckStep* step = dynamic_cast<ActivePlanCheckStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the active plan check step page");

    PlanPathStepParameter* pathStepParameter = step->getPathParameter();
    pathStepParameter->setPlanNodeId(m_planPathStepParameter.getPlanNodeId());

    StepNumberStepParameter* yesStepParameter = step->getYesParameter();
    yesStepParameter->setStepId(m_yesStepParameter.getStepId());

    StepNumberStepParameter* noStepParameter = step->getNoParameter();
    noStepParameter->setStepId(m_noStepParameter.getStepId());

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_ACTIVE_PLAN_CHECK)
