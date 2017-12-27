/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BackgroundLogStep.cpp $
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

#if defined (STEPTYPE_BACKGROUND_LOG)

#include "BackgroundLogStep.h"
#include "core/utilities/src/TAAssert.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "YesNoStepParameter.h"
#include "resource.h"
#include "PlanStepEditBackgroundLogPage.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

BackgroundLogStep::BackgroundLogStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_mandatoryLogParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLogStep" );
    FUNCTION_EXIT;
}


BackgroundLogStep::BackgroundLogStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::BACKGROUND_LOG_STEP),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_mandatoryLogParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLogStep" );
    FUNCTION_EXIT;
}


BackgroundLogStep::BackgroundLogStep(const BackgroundLogStep& planStep) :
    PlanStep(planStep),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_mandatoryLogParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundLogStep" );

    m_messageParameter = new TextualStepParameter(*this, *planStep.m_messageParameter);
    m_timeoutParameter = new NumericStepParameter(*this, *planStep.m_timeoutParameter);
    m_mandatoryLogParameter = new YesNoStepParameter(*this, *planStep.m_mandatoryLogParameter);
    m_operatorProfileNumber = new NumericStepParameter(*this, *planStep.m_operatorProfileNumber);
    m_operatorProfileDisplay = new TextualStepParameter(*this, *planStep.m_operatorProfileDisplay);

    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_timeoutParameter);
    addPlanStepParameter(*m_mandatoryLogParameter);
    //addPlanStepParameter(*m_operatorProfileNumber);
    addPlanStepParameter(*m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


BackgroundLogStep::~BackgroundLogStep()
{
    FUNCTION_ENTRY( "~BackgroundLogStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* BackgroundLogStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new BackgroundLogStep(*this);
}


void BackgroundLogStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_messageParameter;
    delete m_timeoutParameter;
    delete m_mandatoryLogParameter;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROMPT_MESSAGE), "");
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), 60);
    m_mandatoryLogParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_MANDATORY_LOG), true);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), TA_Base_Core::DatabaseKey::getInvalidKey());
    m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), "");


    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_timeoutParameter);
    stepParameters.push_back(m_mandatoryLogParameter);
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


void BackgroundLogStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::BACKGROUND_LOG_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for background log step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::BACKGROUND_LOG_STEP));
    }

    delete m_messageParameter;
    delete m_timeoutParameter;
    delete m_mandatoryLogParameter;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROMPT_MESSAGE), parameterSet.bgLog().message.in());
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), parameterSet.bgLog().responseTimeout);
    m_mandatoryLogParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_MANDATORY_LOG), parameterSet.bgLog().logRequired);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), parameterSet.bgLog().profile);

    try
    {
        typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
        ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
        for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
        {
            if (parameterSet.bgLog().profile == (*iter)->getKey())
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
    stepParameters.push_back(m_mandatoryLogParameter);
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


void BackgroundLogStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::BackgroundLogParameters  parameter;

    parameter.message = CORBA::string_dup(m_messageParameter->getMessageText().c_str());
    parameter.responseTimeout = m_timeoutParameter->getNumericValue();
    parameter.logRequired = m_mandatoryLogParameter->getAffirmative();
    parameter.profile = m_operatorProfileNumber->getNumericValue();

    step.parameters.bgLog(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* BackgroundLogStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditBackgroundLogPage(this);
}


#endif // defined (STEPTYPE_BACKGROUND_LOG)
