/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DecisionStep.cpp $
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

#if defined (STEPTYPE_DECISION)

#include "DecisionStep.h"
#include "core/utilities/src/TAAssert.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "StepNumberStepParameter.h"
#include "YesNoStepParameter.h"
#include "resource.h"
#include "PlanStepEditDecisionParameterPage.h"
#include "core/exceptions/src/PlanStepException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

DecisionStep::DecisionStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_yesParameter(NULL),
    m_noParameter(NULL),
    // lizettejl++ (DP-changes)
    m_yesLabelParameter(NULL),
    m_noLabelParameter(NULL),
    m_pauseLabelParameter(NULL),
    m_enableNoParameter(NULL),
    m_enablePauseParameter(NULL)
{
    FUNCTION_ENTRY( "DecisionStep" );
    FUNCTION_EXIT;
}


DecisionStep::DecisionStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::DECISION_STEP),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_yesParameter(NULL),
    m_noParameter(NULL),
    // lizettejl++ (DP-changes)
    m_yesLabelParameter(NULL),
    m_noLabelParameter(NULL),
    m_pauseLabelParameter(NULL),
    m_enableNoParameter(NULL),
    m_enablePauseParameter(NULL)
{
    FUNCTION_ENTRY( "DecisionStep" );
    FUNCTION_EXIT;
}


DecisionStep::DecisionStep(const DecisionStep& planStep) :
    PlanStep(planStep),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_yesParameter(NULL),
    m_noParameter(NULL),
    // lizettejl++ (DP-changes)
    m_yesLabelParameter(NULL),
    m_noLabelParameter(NULL),
    m_pauseLabelParameter(NULL),
    m_enableNoParameter(NULL),
    m_enablePauseParameter(NULL)
{
    FUNCTION_ENTRY( "DecisionStep" );

    m_messageParameter = new TextualStepParameter(*this, *planStep.m_messageParameter);
    m_timeoutParameter = new NumericStepParameter(*this, *planStep.m_timeoutParameter);
    m_yesParameter = new StepNumberStepParameter(*this, *planStep.m_yesParameter);
    m_noParameter = new StepNumberStepParameter(*this, *planStep.m_noParameter);

    // lizettejl++ (DP-changes)
    m_yesLabelParameter = new TextualStepParameter(*this, *planStep.m_yesLabelParameter);
    m_noLabelParameter = new TextualStepParameter(*this, *planStep.m_noLabelParameter);
    m_pauseLabelParameter = new TextualStepParameter(*this, *planStep.m_pauseLabelParameter);
    m_enableNoParameter = new YesNoStepParameter(*this, *planStep.m_enableNoParameter);
    m_enablePauseParameter = new YesNoStepParameter(*this, *planStep.m_enablePauseParameter);

    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_timeoutParameter);
    addPlanStepParameter(*m_yesParameter);
    addPlanStepParameter(*m_noParameter);
    // lizettejl++ (DP-changes)
    addPlanStepParameter(*m_yesLabelParameter);
    addPlanStepParameter(*m_noLabelParameter);
    addPlanStepParameter(*m_pauseLabelParameter);
    addPlanStepParameter(*m_enableNoParameter);
    addPlanStepParameter(*m_enablePauseParameter);

    FUNCTION_EXIT;
}


DecisionStep::~DecisionStep()
{
    FUNCTION_ENTRY( "~DecisionStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* DecisionStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new DecisionStep(*this);
}


void DecisionStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_messageParameter;
    delete m_timeoutParameter;
    delete m_yesParameter;
    delete m_noParameter;
    // lizettejl++ (DP-changes)
    delete m_yesLabelParameter;
    delete m_noLabelParameter;
    delete m_pauseLabelParameter;
    delete m_enableNoParameter;
    delete m_enablePauseParameter;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DECISION_MESSAGE), "");
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), 60);
    m_yesParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_YES_STEP), 0);
    m_noParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_STEP), 0);

    // lizettejl++ (DP-changes)
    m_yesLabelParameter = new TextualStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_YES_LABEL), "ÊÇ");
    m_noLabelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_LABEL), "·ñ");
	m_pauseLabelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PAUSE_LABEL), "ÔÝÍ£");
    m_enableNoParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_ENABLED), true);
    m_enablePauseParameter = new YesNoStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PAUSE_ENABLED), false);
    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);
    stepParameters.push_back(m_yesParameter);
    stepParameters.push_back(m_noParameter);

    // lizettejl++ (DP-changes)
    stepParameters.push_back(m_yesLabelParameter);
    stepParameters.push_back(m_noLabelParameter);
    stepParameters.push_back(m_pauseLabelParameter);
    stepParameters.push_back(m_enableNoParameter);
    stepParameters.push_back(m_enablePauseParameter);

    FUNCTION_EXIT;
}


void DecisionStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::DECISION_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for decision step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::DECISION_STEP));
    }

    delete m_messageParameter;
    delete m_timeoutParameter;
    delete m_yesParameter;
    delete m_noParameter;
    // lizettejl++ (DP-changes)
    delete m_yesLabelParameter;
    delete m_noLabelParameter;
    delete m_pauseLabelParameter;
    delete m_enableNoParameter;
    delete m_enablePauseParameter;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DECISION_MESSAGE), parameterSet.decision().decisionMessage.in());
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), parameterSet.decision().responseTimeout);
    m_yesParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_YES_STEP), parameterSet.decision().yesStep);
    m_noParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_STEP), parameterSet.decision().noStep);
    // lizettejl++ (DP-changes)
    // TD17498 - modified the initialization to retrieve data from the given step parameters
    m_yesLabelParameter = new TextualStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_YES_LABEL), parameterSet.decision().yesButtonLabel.in());
    m_noLabelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_LABEL), parameterSet.decision().noButtonLabel.in());
    m_pauseLabelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PAUSE_LABEL), parameterSet.decision().pauseButtonLabel.in());
    m_enableNoParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_ENABLED), parameterSet.decision().noButtonEnabled);
    m_enablePauseParameter = new YesNoStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PAUSE_ENABLED), parameterSet.decision().pauseButtonEnabled);
    // ++TD17498

    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);
    stepParameters.push_back(m_yesParameter);
    stepParameters.push_back(m_noParameter);
    // lizettejl++ (DP-changes)
    stepParameters.push_back(m_yesLabelParameter);
    stepParameters.push_back(m_noLabelParameter);
    stepParameters.push_back(m_pauseLabelParameter);
    stepParameters.push_back(m_enableNoParameter);
    stepParameters.push_back(m_enablePauseParameter);

    FUNCTION_EXIT;
}


void DecisionStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::DecisionParameters  parameter;

    parameter.decisionMessage = CORBA::string_dup(m_messageParameter->getMessageText().c_str());
    parameter.responseTimeout = m_timeoutParameter->getNumericValue();
    parameter.yesStep = m_yesParameter->getStepId();
    parameter.noStep = m_noParameter->getStepId();
    // lizettejl++ (DP-changes)
    parameter.yesButtonLabel = CORBA::string_dup(m_yesLabelParameter->getMessageText().c_str());
    parameter.noButtonLabel = CORBA::string_dup(m_noLabelParameter->getMessageText().c_str());
    parameter.pauseButtonLabel = CORBA::string_dup(m_pauseLabelParameter->getMessageText().c_str());
    parameter.noButtonEnabled = m_enableNoParameter->getAffirmative();
    parameter.pauseButtonEnabled = m_enablePauseParameter->getAffirmative();
    // ++lizettejl
    step.parameters.decision(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* DecisionStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditDecisionParameterPage(this);
}


#endif // defined (STEPTYPE_DECISION)
