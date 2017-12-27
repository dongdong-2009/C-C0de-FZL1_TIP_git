/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/EvaluateExpressionStep.cpp $
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

#if defined (STEPTYPE_EVALUATE_EXPRESSION)

#include "core/utilities/src/TAAssert.h"
#include "resource.h"
#include "EvaluateExpressionStep.h"
#include "TextualStepParameter.h"
#include "StepNumberStepParameter.h"
#include "PlanStepEditEvaluateExpressionParameterPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

EvaluateExpressionStep::EvaluateExpressionStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
PlanStep(parent,stepDetail),
m_evaluateExpressionParameter (NULL),
m_successParameter(NULL),
m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "EvaluateExpressionStep" );
    FUNCTION_EXIT;
}


EvaluateExpressionStep::EvaluateExpressionStep(PlanNode* parent,PlanStepId stepId) :
PlanStep(parent,stepId,TA_Base_Core::EVALUATE_EXPRESSION_STEP),
m_evaluateExpressionParameter (NULL),
m_successParameter(NULL),
m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "EvaluateExpressionStep" );
    FUNCTION_EXIT;
}


EvaluateExpressionStep::EvaluateExpressionStep(const EvaluateExpressionStep& planStep) :
PlanStep(planStep),
m_evaluateExpressionParameter (NULL),
m_successParameter(NULL),
m_failureParameter(NULL)
{
    FUNCTION_ENTRY( "EvaluateExpressionStep" );

    m_evaluateExpressionParameter = new  TextualStepParameter(*this, *planStep.m_evaluateExpressionParameter);
         m_successParameter = new StepNumberStepParameter(*this, *planStep.m_successParameter);
         m_failureParameter = new StepNumberStepParameter(*this, *planStep.m_failureParameter);

         addPlanStepParameter(*m_successParameter);
         addPlanStepParameter(*m_failureParameter);

    FUNCTION_EXIT;
}


EvaluateExpressionStep::~EvaluateExpressionStep()
{
    FUNCTION_ENTRY( "~EvaluateExpressionStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* EvaluateExpressionStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new EvaluateExpressionStep(*this);
}


void EvaluateExpressionStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_evaluateExpressionParameter;
    delete m_successParameter;
    delete m_failureParameter;

    m_evaluateExpressionParameter = new TextualStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_EVAL_EXP), "");
    m_successParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_SUCCESS_STEP), 0);
    m_failureParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_FAILURE_STEP), 0);

    stepParameters.clear();
    stepParameters.push_back(m_evaluateExpressionParameter);
    stepParameters.push_back(m_successParameter);
    stepParameters.push_back(m_failureParameter);

    FUNCTION_EXIT;
}


void EvaluateExpressionStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                                                        PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::EVALUATE_EXPRESSION_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for evaluate expression step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::EVALUATE_EXPRESSION_STEP));
    }

    delete m_evaluateExpressionParameter;
    delete m_successParameter;
    delete m_failureParameter;

    m_evaluateExpressionParameter = new TextualStepParameter (*this,getParameterName(IDS_STEP_PARAMETER_EVAL_EXP), parameterSet.evaluateExpression().expression.in ());
    m_successParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_SUCCESS_STEP), parameterSet.evaluateExpression().successStep);
    m_failureParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_FAILURE_STEP), parameterSet.evaluateExpression().failureStep);

    stepParameters.clear();
    stepParameters.push_back(m_evaluateExpressionParameter);
    stepParameters.push_back(m_successParameter);
    stepParameters.push_back(m_failureParameter);

    FUNCTION_EXIT;
}


void EvaluateExpressionStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::EvaluateExpressionParameters  parameter;

    parameter.expression =    CORBA::string_dup(m_evaluateExpressionParameter->getMessageText().c_str());
    parameter.successStep = m_successParameter->getStepId();
    parameter.failureStep = m_failureParameter->getStepId();

    step.parameters.evaluateExpression(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* EvaluateExpressionStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditEvaluateExpressionParameterPage    (this);
}


#endif // defined (STEPTYPE_EVALUATE_EXPRESSION)
