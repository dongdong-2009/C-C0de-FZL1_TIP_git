//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditTerminatePlanPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"

#if defined (STEPTYPE_TERMINATE_PLAN)

#include "planmanager.h"
#include "PlanStepEditTerminatePlanPage.h"
#include "TerminatePlanStep.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditTerminatePlanPage dialog

CPlanStepEditTerminatePlanPage::CPlanStepEditTerminatePlanPage(TerminatePlanStep* step)
    : CPlanStepEditParameterPage(CPlanStepEditTerminatePlanPage::IDD, step),
    m_planPathStepParameter(*step->getPathParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditTerminatePlanPage" );

    // {{AFX_DATA_INIT(CPlanStepEditTerminatePlanPage)
    m_planPathLabel = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepEditTerminatePlanPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditTerminatePlanPage)
    DDX_Text(pDX, IDC_PLAN_PATH, m_planPathLabel);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditTerminatePlanPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditTerminatePlanPage)
    ON_BN_CLICKED(IDC_PICK_PLAN_PATH_BUTTON, OnPickPlanPathButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditTerminatePlanPage message handlers

void CPlanStepEditTerminatePlanPage::OnPickPlanPathButton()
{
    FUNCTION_ENTRY( "OnPickPlanPathButton" );

    m_planPathStepParameter.editParameter();

    DisplayParameters();

    FUNCTION_EXIT;
}


void CPlanStepEditTerminatePlanPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* planPathButton = GetDlgItem(IDC_PICK_PLAN_PATH_BUTTON);
    if (planPathButton)
    {
        planPathButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditTerminatePlanPage::DisplayParameters()
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


void CPlanStepEditTerminatePlanPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    // Load the initial values
    TerminatePlanStep* step = dynamic_cast<TerminatePlanStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the terminate plan step page");

    PlanPathStepParameter* pathStepParameter = step->getPathParameter();
    pathStepParameter->setPlanNodeId(m_planPathStepParameter.getPlanNodeId());

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_TERMINATE_PLAN)

