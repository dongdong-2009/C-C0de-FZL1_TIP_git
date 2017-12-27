/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanCheckStep.cpp $
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

#if defined (STEPTYPE_ACTIVE_PLAN_CHECK)

#include "ActivePlanCheckStep.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanPathStepParameter.h"
#include "StepNumberStepParameter.h"
#include "resource.h"
#include "PlanStepEditActivePlanCheckPage.h"
#include "core/exceptions/src/PlanStepException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

ActivePlanCheckStep::ActivePlanCheckStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_planPathParameter(NULL),
    m_yesStepParameter(NULL),
    m_noStepParameter(NULL)
{
    FUNCTION_ENTRY( "ActivePlanCheckStep" );
    FUNCTION_EXIT;
}


ActivePlanCheckStep::ActivePlanCheckStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::ACTIVE_PLAN_CHECK_STEP),
    m_planPathParameter(NULL),
    m_yesStepParameter(NULL),
    m_noStepParameter(NULL)
{
    FUNCTION_ENTRY( "ActivePlanCheckStep" );
    FUNCTION_EXIT;
}


ActivePlanCheckStep::ActivePlanCheckStep(const ActivePlanCheckStep& planStep) :
    PlanStep(planStep),
    m_planPathParameter(NULL),
    m_yesStepParameter(NULL),
    m_noStepParameter(NULL)
{
    FUNCTION_ENTRY( "ActivePlanCheckStep" );

    m_planPathParameter = new PlanPathStepParameter(*this, *planStep.m_planPathParameter);
    m_yesStepParameter = new StepNumberStepParameter(*this, *planStep.m_yesStepParameter);
    m_noStepParameter = new StepNumberStepParameter(*this, *planStep.m_noStepParameter);

    addPlanStepParameter(*m_planPathParameter);
    addPlanStepParameter(*m_yesStepParameter);
    addPlanStepParameter(*m_noStepParameter);

    FUNCTION_EXIT;
}


ActivePlanCheckStep::~ActivePlanCheckStep()
{
    FUNCTION_ENTRY( "~ActivePlanCheckStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* ActivePlanCheckStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new ActivePlanCheckStep(*this);
}


void ActivePlanCheckStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_planPathParameter;
    delete m_yesStepParameter;
    delete m_noStepParameter;

    m_planPathParameter   = new PlanPathStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PLAN_PATH),0);
    m_yesStepParameter    = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_YES_STEP), 0);
    m_noStepParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_NO_STEP), 0);

    stepParameters.clear();
    stepParameters.push_back(m_planPathParameter);
    stepParameters.push_back(m_yesStepParameter);
    stepParameters.push_back(m_noStepParameter);

    FUNCTION_EXIT;
}


void ActivePlanCheckStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                                   PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::ACTIVE_PLAN_CHECK_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for active plan chack step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::ACTIVE_PLAN_CHECK_STEP));
    }

    delete m_planPathParameter;
    delete m_yesStepParameter;
    delete m_noStepParameter;

    m_planPathParameter   = new PlanPathStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PLAN_PATH),parameterSet.activePlanCheck().plan);
    m_yesStepParameter    = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_YES_STEP), parameterSet.activePlanCheck().yesStep);
    m_noStepParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_NO_STEP), parameterSet.activePlanCheck().noStep);

    stepParameters.clear();
    stepParameters.push_back(m_planPathParameter);
    stepParameters.push_back(m_yesStepParameter);
    stepParameters.push_back(m_noStepParameter);

    FUNCTION_EXIT;
}


void ActivePlanCheckStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::ActivePlanCheckParameters  parameter;

    parameter.plan = m_planPathParameter->getPlanNodeId();
    parameter.yesStep = m_yesStepParameter->getStepId();
    parameter.noStep = m_noStepParameter->getStepId();

    step.parameters.activePlanCheck(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* ActivePlanCheckStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditActivePlanCheckPage(this);
}


#endif // defined (STEPTYPE_ACTIVE_PLAN_CHECK)
