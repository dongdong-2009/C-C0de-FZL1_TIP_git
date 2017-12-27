/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BackgroundPromptStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_BACKGROUND_PROMPT)

#include "BackgroundPromptStep.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "resource.h"
#include "PlanStepEditBackgroundPromptPage.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

BackgroundPromptStep::BackgroundPromptStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
       m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundPromptStep" );
    FUNCTION_EXIT;
}


BackgroundPromptStep::BackgroundPromptStep(PlanNode* parent,PlanStepId stepId) :
PlanStep(parent,stepId,TA_Base_Core::BACKGROUND_PROMPT_STEP),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundPromptStep" );
    FUNCTION_EXIT;
}


BackgroundPromptStep::BackgroundPromptStep(const BackgroundPromptStep& planStep) :
    PlanStep(planStep),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundPromptStep" );

    m_messageParameter = new TextualStepParameter(*this, *planStep.m_messageParameter);
    m_timeoutParameter = new NumericStepParameter(*this, *planStep.m_timeoutParameter);
    m_operatorProfileNumber = new NumericStepParameter(*this, *planStep.m_operatorProfileNumber);
    m_operatorProfileDisplay = new TextualStepParameter(*this, *planStep.m_operatorProfileDisplay);

    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_timeoutParameter);
    //addPlanStepParameter(*m_operatorProfileNumber);
    addPlanStepParameter(*m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


BackgroundPromptStep::~BackgroundPromptStep()
{
    FUNCTION_ENTRY( "~BackgroundPromptStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* BackgroundPromptStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new BackgroundPromptStep(*this);
}


void BackgroundPromptStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_messageParameter;
    delete m_timeoutParameter;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROMPT_MESSAGE), "");
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), 60);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), TA_Base_Core::DatabaseKey::getInvalidKey());
    m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), "");



    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


void BackgroundPromptStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::BACKGROUND_PROMPT_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for prompt step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::PROMPT_STEP));
    }
    delete m_messageParameter;
    delete m_timeoutParameter;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROMPT_MESSAGE), parameterSet.bgPrompt().message.in());
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), parameterSet.bgPrompt().dismissTimeout);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), parameterSet.bgPrompt().profile);

    try
    {
        typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
        ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
        for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
        {
            if (parameterSet.bgPrompt().profile == (*iter)->getKey())
            {
                m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), (*iter)->getName());
            }
            delete *iter;
        }
        allProfile.clear();
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to populate profile name");
        return;
    }

    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


void BackgroundPromptStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::BackgroundPromptParameters  parameter;

    parameter.message = CORBA::string_dup(m_messageParameter->getMessageText().c_str());
    parameter.dismissTimeout = m_timeoutParameter->getNumericValue();
    parameter.profile = m_operatorProfileNumber->getNumericValue();

    step.parameters.bgPrompt(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* BackgroundPromptStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditBackgroundPromptPage(this);
}


#endif // defined (STEPTYPE_PROMPT)
