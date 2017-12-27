/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TerminatePlanStep.cpp $
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

#if defined (STEPTYPE_TERMINATE_PLAN)

#include "core/utilities/src/TAAssert.h"
#include "TerminatePlanStep.h"
#include "PlanPathStepParameter.h"
#include "resource.h"
#include "PlanStepEditTerminatePlanPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

TerminatePlanStep::TerminatePlanStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail), m_planParameter(NULL)
{
    FUNCTION_ENTRY( "TerminatePlanStep" );
    FUNCTION_EXIT;
}


TerminatePlanStep::TerminatePlanStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::TERMINATE_PLAN_STEP), m_planParameter(NULL)
{
    FUNCTION_ENTRY( "TerminatePlanStep" );
    FUNCTION_EXIT;
}


TerminatePlanStep::TerminatePlanStep(const TerminatePlanStep& planStep) :
    PlanStep(planStep),m_planParameter(NULL)
{
    FUNCTION_ENTRY( "TerminatePlanStep" );

    m_planParameter = new PlanPathStepParameter(*this, *planStep.m_planParameter);

    addPlanStepParameter(*m_planParameter);

    FUNCTION_EXIT;
}


TerminatePlanStep::~TerminatePlanStep()
{
    FUNCTION_ENTRY( "~TerminatePlanStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* TerminatePlanStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new TerminatePlanStep(*this);
}


void TerminatePlanStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_planParameter;
    m_planParameter = new PlanPathStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PLAN_PATH),0);
    stepParameters.clear();
    stepParameters.push_back(m_planParameter);

    FUNCTION_EXIT;
}


void TerminatePlanStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::TERMINATE_PLAN_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for terminate plan step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::TERMINATE_PLAN_STEP));
    }
    delete m_planParameter;
    m_planParameter = new PlanPathStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PLAN_PATH),parameterSet.terminatePlan().plan);
    stepParameters.clear();
    stepParameters.push_back(m_planParameter);

    FUNCTION_EXIT;
}


void TerminatePlanStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::TerminatePlanParameters  parameter;

    parameter.plan = m_planParameter->getPlanNodeId();

    step.parameters.terminatePlan(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* TerminatePlanStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditTerminatePlanPage(this);
}


#endif // defined (STEPTYPE_TERMINATE_PLAN)
