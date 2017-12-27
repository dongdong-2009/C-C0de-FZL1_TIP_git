/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Facilitates the launching of a display when executing a Launch Schematic step.
  *
  */

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanManager.h"
#include "ExecutableLaunchSchematicStep.h"
#include "LaunchSchematicDlg.h"
#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutableLaunchSchematicStep

ExecutableLaunchSchematicStep::ExecutableLaunchSchematicStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    InteractiveStep(stepDetail)
{
    FUNCTION_ENTRY( "ExecutableLaunchSchematicStep" );

    TA_ASSERT(stepDetail.data._d() == TA_Base_Core::LAUNCH_SCHEMATIC_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutableLaunchSchematicStep::~ExecutableLaunchSchematicStep()
{
    FUNCTION_ENTRY( "~ExecutableLaunchSchematicStep" );
    FUNCTION_EXIT;
}


void ExecutableLaunchSchematicStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    // Prompt the user to acknowledge the launch of display.
    CLaunchSchematicDlg* launchSchematicDlg = new CLaunchSchematicDlg(getStepDetail());

    if (launchSchematicDlg->Create(IDD_LAUNCH_SCHEMATIC_DLG) == 0)
    {
        TA_THROW(InteractiveStepException("Failed to create Launch Schematic step confirmation dialog"));
    }

    FUNCTION_EXIT;
}


