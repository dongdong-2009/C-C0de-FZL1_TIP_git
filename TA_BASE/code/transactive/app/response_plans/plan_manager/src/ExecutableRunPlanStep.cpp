//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanManager.h"
#include "ExecutableRunPlanStep.h"
#include "RunPlanDlg.h"
#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutableRunPlanStep

ExecutableRunPlanStep::ExecutableRunPlanStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    InteractiveStep(stepDetail)
{
    FUNCTION_ENTRY( "ExecutableRunPlanStep" );

    TA_ASSERT(stepDetail.data._d() == TA_Base_Core::RUN_PLAN_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutableRunPlanStep::~ExecutableRunPlanStep()
{
    FUNCTION_ENTRY( "~ExecutableRunPlanStep" );
    FUNCTION_EXIT;
}


void ExecutableRunPlanStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    // Prompt the user to acknowledge the launch of the Plan Controller for child plan.
    CRunPlanDlg* runPlanDlg = new CRunPlanDlg(getStepDetail());

    if (runPlanDlg->Create(IDD_RUN_PLAN_DLG) == 0)
    {
        TA_THROW(InteractiveStepException("Failed to create Run Plan step confirmation dialog"));
    }

    FUNCTION_EXIT;
}


