/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/RunPlanStep.cpp $
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

#if defined (STEPTYPE_RUN_PLAN)

#include "RunPlanStep.h"
#include "core/utilities/src/TAAssert.h"
#include "YesNoStepParameter.h"
#include "PlanPathStepParameter.h"
#include "resource.h"
#include "PlanStepEditRunPlanPage.h"
#include "core/exceptions/src/PlanStepException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

RunPlanStep::RunPlanStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_planParameter(NULL),
    m_launchParameter(NULL),
    m_waitParameter(NULL)
{
    FUNCTION_ENTRY( "RunPlanStep" );
    FUNCTION_EXIT;
}


RunPlanStep::RunPlanStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::RUN_PLAN_STEP),
    m_planParameter(NULL),
    m_launchParameter(NULL),
    m_waitParameter(NULL)
{
    FUNCTION_ENTRY( "RunPlanStep" );
    FUNCTION_EXIT;
}


RunPlanStep::RunPlanStep(const RunPlanStep& planStep) :
    PlanStep(planStep),
    m_planParameter(NULL),
    m_launchParameter(NULL),
    m_waitParameter(NULL)
{
    FUNCTION_ENTRY( "RunPlanStep" );

    m_planParameter   = new PlanPathStepParameter(*this, *planStep.m_planParameter);
    m_launchParameter = new YesNoStepParameter(*this, *planStep.m_launchParameter);
    m_waitParameter   = new YesNoStepParameter(*this, *planStep.m_waitParameter);

    addPlanStepParameter(*m_planParameter);
    addPlanStepParameter(*m_launchParameter);
    addPlanStepParameter(*m_waitParameter);

    FUNCTION_EXIT;
}


RunPlanStep::~RunPlanStep()
{
    FUNCTION_ENTRY( "~RunPlanStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* RunPlanStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new RunPlanStep(*this);
}


void RunPlanStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_planParameter;
   delete m_launchParameter;
   delete m_waitParameter;

   m_planParameter = new PlanPathStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PLAN_PATH), 0);
   m_launchParameter = new YesNoStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_MANUAL_PLAN_LAUNCH), 0);
   m_waitParameter = new YesNoStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_WAIT_FOR_PLAN_COMPLETION), 0);

   stepParameters.clear();
   stepParameters.push_back(m_planParameter);
   stepParameters.push_back(m_launchParameter);
   stepParameters.push_back(m_waitParameter);

    FUNCTION_EXIT;
}


void RunPlanStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::RUN_PLAN_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for run plan step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::RUN_PLAN_STEP));
    }

   delete m_planParameter;
   delete m_launchParameter;
   delete m_waitParameter;

   m_planParameter = new PlanPathStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PLAN_PATH), parameterSet.runPlan().plan);
   m_launchParameter = new YesNoStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_MANUAL_PLAN_LAUNCH), parameterSet.runPlan().manualLaunch);
   m_waitParameter = new YesNoStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_WAIT_FOR_PLAN_COMPLETION), parameterSet.runPlan().waitForCompletion);

   stepParameters.clear();
   stepParameters.push_back(m_planParameter);
   stepParameters.push_back(m_launchParameter);
   stepParameters.push_back(m_waitParameter);

    FUNCTION_EXIT;
}


void RunPlanStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::RunPlanParameters  parameter;


    parameter.plan = m_planParameter->getPlanNodeId();
    parameter.manualLaunch = m_launchParameter->getAffirmative();
    parameter.waitForCompletion = m_waitParameter->getAffirmative();

    step.parameters.runPlan(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* RunPlanStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditRunPlanPage(this);
}


#endif // defined (STEPTYPE_RUN_PLAN)

