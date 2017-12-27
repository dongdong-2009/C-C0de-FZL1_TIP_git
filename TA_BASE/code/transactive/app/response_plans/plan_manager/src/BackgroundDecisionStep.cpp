/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BackgroundDecisionStep.cpp $
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

#if defined (STEPTYPE_BACKGROUND_DECISION)

#include "BackgroundDecisionStep.h"
#include "core/utilities/src/TAAssert.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "StepNumberStepParameter.h"
#include "YesNoStepParameter.h"
#include "resource.h"
#include "PlanStepEditBackgroundDecisionParameterPage.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

BackgroundDecisionStep::BackgroundDecisionStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
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
    m_enablePauseParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundDecisionStep" );
    FUNCTION_EXIT;
}


BackgroundDecisionStep::BackgroundDecisionStep(PlanNode* parent,PlanStepId stepId) :
PlanStep(parent,stepId,TA_Base_Core::BACKGROUND_DECISION_STEP),
    m_messageParameter(NULL),
    m_timeoutParameter(NULL),
    m_yesParameter(NULL),
    m_noParameter(NULL),
    // lizettejl++ (DP-changes)
    m_yesLabelParameter(NULL),
    m_noLabelParameter(NULL),
    m_pauseLabelParameter(NULL),
    m_enableNoParameter(NULL),
    m_enablePauseParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundDecisionStep" );
    FUNCTION_EXIT;
}


BackgroundDecisionStep::BackgroundDecisionStep(const BackgroundDecisionStep& planStep) :
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
    m_enablePauseParameter(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDisplay(NULL)
{
    FUNCTION_ENTRY( "BackgroundDecisionStep" );

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
    m_operatorProfileNumber = new NumericStepParameter(*this, *planStep.m_operatorProfileNumber);
    m_operatorProfileDisplay = new TextualStepParameter(*this, *planStep.m_operatorProfileDisplay);

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
    //addPlanStepParameter(*m_operatorProfileNumber);
    addPlanStepParameter(*m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


BackgroundDecisionStep::~BackgroundDecisionStep()
{
    FUNCTION_ENTRY( "~DecisionStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* BackgroundDecisionStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new BackgroundDecisionStep(*this);
}


void BackgroundDecisionStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
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
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DECISION_MESSAGE), "");
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), 60);
    m_yesParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_YES_STEP), 0);
    m_noParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_STEP), 0);

    // lizettejl++ (DP-changes)

	// dahe++ for GF-75
    m_yesLabelParameter = new TextualStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_YES_LABEL), "ÊÇ");
    m_noLabelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_LABEL), "·ñ");
	m_pauseLabelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PAUSE_LABEL), "ÔÝÍ£");
	// ++dahe

    m_enableNoParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_ENABLED), true);
    m_enablePauseParameter = new YesNoStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PAUSE_ENABLED), false);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), TA_Base_Core::DatabaseKey::getInvalidKey(), true);
    m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), "");

    //try
    //{
    //    typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
    //    ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
    //    for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
    //    {
    //        if (TA_Base_Core::DatabaseKey::getInvalidKey() == (*iter)->getKey())
    //        {
    //            m_operatorProfileDisplay = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), (*iter)->getName());
    //        }
    //        delete *iter;
    //    }
    //    allProfile.clear();
    //}
    //catch (...)
    //{
    //    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to populate profile name");
    //    return;
    //}
    

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
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


void BackgroundDecisionStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::BACKGROUND_DECISION_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for background decision step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::BACKGROUND_DECISION_STEP));
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
    delete m_operatorProfileNumber;
    delete m_operatorProfileDisplay;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DECISION_MESSAGE), parameterSet.bgDecision().decisionMessage.in());
    m_timeoutParameter = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RESPONSE_TIMEOUT), parameterSet.bgDecision().responseTimeout);
    m_yesParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_YES_STEP), parameterSet.bgDecision().yesStep);
    m_noParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_STEP), parameterSet.bgDecision().noStep);
    // lizettejl++ (DP-changes)
    // TD17498 - modified the initialization to retrieve data from the given step parameters
    m_yesLabelParameter = new TextualStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_YES_LABEL), parameterSet.bgDecision().yesButtonLabel.in());
    m_noLabelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_LABEL), parameterSet.bgDecision().noButtonLabel.in());
    m_pauseLabelParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PAUSE_LABEL), parameterSet.bgDecision().pauseButtonLabel.in());
    m_enableNoParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_ENABLED), parameterSet.bgDecision().noButtonEnabled);
    m_enablePauseParameter = new YesNoStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_PAUSE_ENABLED), parameterSet.bgDecision().pauseButtonEnabled);
    // ++TD17498
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), parameterSet.bgDecision().profile, true);

    try
    {
        typedef std::vector<TA_Base_Core::IProfile *> ProfileList;
        ProfileList allProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getAllProfiles(false);
        for (ProfileList::const_iterator iter = allProfile.begin(); iter != allProfile.end(); iter++)
        {
            if (parameterSet.bgDecision().profile == (*iter)->getKey())
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
    stepParameters.push_back(m_yesParameter);
    stepParameters.push_back(m_noParameter);
    // lizettejl++ (DP-changes)
    stepParameters.push_back(m_yesLabelParameter);
    stepParameters.push_back(m_noLabelParameter);
    stepParameters.push_back(m_pauseLabelParameter);
    stepParameters.push_back(m_enableNoParameter);
    stepParameters.push_back(m_enablePauseParameter);
    //stepParameters.push_back(m_operatorProfileNumber);
    stepParameters.push_back(m_operatorProfileDisplay);

    FUNCTION_EXIT;
}


void BackgroundDecisionStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::BackgroundDecisionParameters  parameter;

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
    parameter.profile = m_operatorProfileNumber->getNumericValue();
    // ++lizettejl
    step.parameters.bgDecision(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* BackgroundDecisionStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditBackgroundDecisionParameterPage(this);
}


#endif // defined (STEPTYPE_DECISION)
