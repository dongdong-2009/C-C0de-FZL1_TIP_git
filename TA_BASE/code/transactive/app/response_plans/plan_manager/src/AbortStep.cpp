//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/AbortStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_ABORT)

#include "AbortStep.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

AbortStep::AbortStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) : PlanStep(parent,stepDetail)
{
    FUNCTION_ENTRY( "AbortStep" );
    FUNCTION_EXIT;
}


AbortStep::AbortStep(PlanNode* parent,PlanStepId stepId) : PlanStep(parent,stepId,TA_Base_Core::ABORT_STEP)
{
    FUNCTION_ENTRY( "AbortStep" );
    FUNCTION_EXIT;
}


AbortStep::AbortStep(const AbortStep &other) : PlanStep(other)
{
    FUNCTION_ENTRY( "AbortStep" );
    FUNCTION_EXIT;
}


AbortStep::~AbortStep()
{
    FUNCTION_ENTRY( "~AbortStep" );
    FUNCTION_EXIT;
}


PlanStep* AbortStep::clone()
{
    FUNCTION_ENTRY( "clone" );
    FUNCTION_EXIT;
    return new AbortStep(*this);
}


void AbortStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}


void AbortStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::ABORT_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for abort step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::ABORT_STEP));
    }

    FUNCTION_EXIT;
}


void AbortStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    step.parameters.abort(0);

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_ABORT)
