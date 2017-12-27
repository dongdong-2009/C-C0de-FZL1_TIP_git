/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LogStep.cpp $
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

#if defined (STEPTYPE_LOG)

#include "LogStep.h"
#include "core/utilities/src/TAAssert.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "YesNoStepParameter.h"
#include "resource.h"
#include "PlanStepEditLogPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

LogStep::LogStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_mandatoryLogParameter(NULL)
{
    FUNCTION_ENTRY( "LogStep" );
    FUNCTION_EXIT;
}


LogStep::LogStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::LOG_STEP),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_mandatoryLogParameter(NULL)
{
    FUNCTION_ENTRY( "LogStep" );
    FUNCTION_EXIT;
}


LogStep::LogStep(const LogStep& planStep) :
    PlanStep(planStep),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_mandatoryLogParameter(NULL)
{
    FUNCTION_ENTRY( "LogStep" );

    m_messageParameter = new TextualStepParameter(*this, *planStep.m_messageParameter);
    m_timeoutParameter = new NumericStepParameter(*this, *planStep.m_timeoutParameter);
    m_mandatoryLogParameter = new YesNoStepParameter(*this, *planStep.m_mandatoryLogParameter);

    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_timeoutParameter);
    addPlanStepParameter(*m_mandatoryLogParameter);

    FUNCTION_EXIT;
}


LogStep::~LogStep()
{
    FUNCTION_ENTRY( "~LogStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* LogStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new LogStep(*this);
}


void LogStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_messageParameter;
    delete m_timeoutParameter;
    delete m_mandatoryLogParameter;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROMPT_MESSAGE), "");
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), 60);
    m_mandatoryLogParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_MANDATORY_LOG), true);

    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);
    stepParameters.push_back(m_mandatoryLogParameter);

    FUNCTION_EXIT;
}


void LogStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::LOG_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for log step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::LOG_STEP));
    }

    delete m_messageParameter;
    delete m_timeoutParameter;
    delete m_mandatoryLogParameter;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROMPT_MESSAGE), parameterSet.log().message.in());
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), parameterSet.log().responseTimeout);
    m_mandatoryLogParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_MANDATORY_LOG), parameterSet.log().logRequired);

    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);
    stepParameters.push_back(m_mandatoryLogParameter);

    FUNCTION_EXIT;
}


void LogStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::LogParameters  parameter;

    parameter.message = CORBA::string_dup(m_messageParameter->getMessageText().c_str());
    parameter.responseTimeout = m_timeoutParameter->getNumericValue();
    parameter.logRequired = m_mandatoryLogParameter->getAffirmative();

    step.parameters.log(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* LogStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditLogPage(this);
}


#endif // defined (STEPTYPE_LOG)
