//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/EndStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "EndStep.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

EndStep::EndStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail)
  : PlanStep(parent,stepDetail)
{
    FUNCTION_ENTRY( "EndStep" );
    FUNCTION_EXIT;
}


EndStep::EndStep(const EndStep& planStep)
      : PlanStep(planStep)
{
    FUNCTION_ENTRY( "EndStep" );
    FUNCTION_EXIT;
}


EndStep::~EndStep()
{
    FUNCTION_ENTRY( "~EndStep" );
    FUNCTION_EXIT;
}


PlanStep* EndStep::clone()
{
    FUNCTION_ENTRY( "clone" );
    FUNCTION_EXIT;
    return new EndStep(*this);
}


void EndStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}


void EndStep::createPlanStepParameters(         const TA_Base_Core::StepParameters &parameterSet,
                                                PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::END_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for plan end step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::END_STEP));
    }

    FUNCTION_EXIT;
}


void EndStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    step.parameters.end(0);

    FUNCTION_EXIT;
}


bool EndStep::canDeleteStep()
{
    FUNCTION_ENTRY( "canDeleteStep" );
    FUNCTION_EXIT;
    return false;
}


