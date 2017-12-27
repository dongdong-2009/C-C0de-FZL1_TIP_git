/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/JumpStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_JUMP)

#include "core/utilities/src/TAAssert.h"
#include "JumpStep.h"
#include "StepNumberStepParameter.h"
#include "resource.h"
#include "PlanStepEditJumpPage.h"
#include "core/exceptions/src/PlanStepException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

JumpStep::JumpStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail), m_stepParameter(NULL)
{
    FUNCTION_ENTRY( "JumpStep" );
    FUNCTION_EXIT;
}


JumpStep::JumpStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::JUMP_STEP), m_stepParameter(NULL)
{
    FUNCTION_ENTRY( "JumpStep" );
    FUNCTION_EXIT;
}


JumpStep::JumpStep(const JumpStep& planStep) :
    PlanStep(planStep),
    m_stepParameter(NULL)
{
    FUNCTION_ENTRY( "JumpStep" );

    m_stepParameter = new StepNumberStepParameter(*this, *planStep.m_stepParameter);

    addPlanStepParameter(*m_stepParameter);

    FUNCTION_EXIT;
}


JumpStep::~JumpStep()
{
    FUNCTION_ENTRY( "~JumpStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* JumpStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new JumpStep(*this);
}


void JumpStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    m_stepParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_JUMP_STEP), 0);
    stepParameters.clear();
    stepParameters.push_back(m_stepParameter);

    FUNCTION_EXIT;
}


void JumpStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::JUMP_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for jump step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::JUMP_STEP));
    }
    m_stepParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_JUMP_STEP), parameterSet.jump().step);
    stepParameters.clear();
    stepParameters.push_back(m_stepParameter);

    FUNCTION_EXIT;
}


void JumpStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::JumpParameters  jumpParameter;
    jumpParameter.step = m_stepParameter->getStepId();

    step.parameters.jump(jumpParameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* JumpStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditJumpPage(this);
}


#endif // defined (STEPTYPE_JUMP)
