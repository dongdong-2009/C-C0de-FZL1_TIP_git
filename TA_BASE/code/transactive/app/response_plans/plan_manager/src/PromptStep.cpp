/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PromptStep.cpp $
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

#if defined (STEPTYPE_PROMPT)

#include "PromptStep.h"
#include "core/utilities/src/TAAssert.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "resource.h"
#include "PlanStepEditPromptPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

PromptStep::PromptStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
       m_messageParameter(NULL),
    m_timeoutParameter(NULL)
{
    FUNCTION_ENTRY( "PromptStep" );
    FUNCTION_EXIT;
}


PromptStep::PromptStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::PROMPT_STEP),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL)
{
    FUNCTION_ENTRY( "PromptStep" );
    FUNCTION_EXIT;
}


PromptStep::PromptStep(const PromptStep& planStep) :
    PlanStep(planStep),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL)
{
    FUNCTION_ENTRY( "PromptStep" );

    m_messageParameter = new TextualStepParameter(*this, *planStep.m_messageParameter);
    m_timeoutParameter = new NumericStepParameter(*this, *planStep.m_timeoutParameter);

    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_timeoutParameter);

    FUNCTION_EXIT;
}


PromptStep::~PromptStep()
{
    FUNCTION_ENTRY( "~PromptStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* PromptStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new PromptStep(*this);
}


void PromptStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_messageParameter;
    delete m_timeoutParameter;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROMPT_MESSAGE), "");
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), 60);

    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);

    FUNCTION_EXIT;
}


void PromptStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::PROMPT_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for prompt step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::PROMPT_STEP));
    }
    delete m_messageParameter;
    delete m_timeoutParameter;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROMPT_MESSAGE), parameterSet.prompt().message.in());
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), parameterSet.prompt().dismissTimeout);

    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);

    FUNCTION_EXIT;
}


void PromptStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::PromptParameters  parameter;

    parameter.message = CORBA::string_dup(m_messageParameter->getMessageText().c_str());
    parameter.dismissTimeout = m_timeoutParameter->getNumericValue();

    step.parameters.prompt(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* PromptStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditPromptPage(this);
}


#endif // defined (STEPTYPE_PROMPT)
