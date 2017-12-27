//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditJumpPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "stdafx.h"

#if defined (STEPTYPE_JUMP)

#include "planmanager.h"
#include "PlanStepEditJumpPage.h"
#include "JumpStep.h"
#include "core/utilities/src/TAAssert.h"
#include "StepNumberStepParameter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditJumpPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditJumpPage, CPlanStepEditParameterPage)

CPlanStepEditJumpPage::CPlanStepEditJumpPage(JumpStep* step)
  : CPlanStepEditParameterPage(CPlanStepEditJumpPage::IDD, step),
    m_jumpParameter(*step->getJumpStepParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditJumpPage" );

    // {{AFX_DATA_INIT(CPlanStepEditJumpPage)
    m_jumpStep = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditJumpPage::~CPlanStepEditJumpPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditJumpPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditJumpPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditJumpPage)
    DDX_Text(pDX, IDC_JUMP_STEP, m_jumpStep);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditJumpPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditJumpPage)
    ON_BN_CLICKED(IDC_PICK_STEP_BUTTON, OnPickStepButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditJumpPage message handlers

void CPlanStepEditJumpPage::OnPickStepButton()
{
    FUNCTION_ENTRY( "OnPickStepButton" );

    // Select the required step
    m_jumpParameter.editParameter();
    DisplayParameters();

    FUNCTION_EXIT;
}


void CPlanStepEditJumpPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* targetStepButton = GetDlgItem(IDC_PICK_STEP_BUTTON);
    if (targetStepButton)
    {
        targetStepButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditJumpPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    m_jumpStep = m_jumpParameter.getDestinationStepDescription().c_str();

    UpdateData(false);

    FUNCTION_EXIT;
}


void CPlanStepEditJumpPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    JumpStep* step = dynamic_cast<JumpStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the jump step page");

    StepNumberStepParameter* jumpStepParameter = step->getJumpStepParameter();
    jumpStepParameter->setStepId(m_jumpParameter.getStepId());

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_JUMP)
