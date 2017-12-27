//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainPAStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "TrainPAStep.h"
#include "resource.h"
#include "PlanStepEditPATrainPage.h"
#include "TrainStepParameter.h"
#include "PaMessageStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "YesNoStepParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

TrainPAStep::TrainPAStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail)
    : PlanStep(parent,stepDetail),
    m_trainParameter (NULL),
    m_messageParameter (NULL),
    m_cyclicParameter (NULL),
    m_durationParameter (NULL)
{
    FUNCTION_ENTRY( "TrainPAStep" );
    FUNCTION_EXIT;
}


TrainPAStep::TrainPAStep(PlanNode* parent,PlanStepId stepId)
   : PlanStep(parent,stepId,TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP),
    m_trainParameter (NULL),
    m_messageParameter (NULL),
    m_cyclicParameter (NULL),
    m_durationParameter (NULL)
{
    FUNCTION_ENTRY( "TrainPAStep" );
    FUNCTION_EXIT;
}


TrainPAStep::TrainPAStep(const TrainPAStep& planStep)
   : PlanStep(planStep),
    m_trainParameter (NULL),
    m_messageParameter (NULL),
    m_cyclicParameter (NULL),
    m_durationParameter (NULL)
{
    FUNCTION_ENTRY( "TrainPAStep" );

    m_trainParameter    = new TrainStepParameter         (*this, *planStep.m_trainParameter);
   m_messageParameter  = new PaMessageStepParameter (*this, *planStep.m_messageParameter, true);
   m_cyclicParameter   = new YesNoStepParameter         (*this, *planStep.m_cyclicParameter);
   m_durationParameter = new TimeSpanStepParameter     (*this, *planStep.m_durationParameter);

    addPlanStepParameter(*m_trainParameter);
    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_cyclicParameter);
    addPlanStepParameter(*m_durationParameter);

    FUNCTION_EXIT;
}


TrainPAStep::~TrainPAStep()
{
    FUNCTION_ENTRY( "~TrainPAStep" );
    FUNCTION_EXIT;
}


PlanStep* TrainPAStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new TrainPAStep(*this);
}


void TrainPAStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    TA_Base_Core::PAMessage message;
     message.messageId = TA_Base_Core::PA_UNDEFINED_MESSAGE_ID;

     m_trainParameter    = new TrainStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TRAIN));
    m_messageParameter  = new PaMessageStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PA_MESSAGE), message, true);
    m_cyclicParameter   = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PA_CYCLIC), 0);
    m_durationParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_DURATION), 0, TimeSpanStepParameter::TR_MINUTES, true);

    stepParameters.clear();
    stepParameters.push_back(m_trainParameter);
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_cyclicParameter);
    stepParameters.push_back(m_durationParameter);

    FUNCTION_EXIT;
}


void TrainPAStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                           PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for Train PA step.";

        TA_THROW(TA_Base_Core::PlanStepException (errorMessage.str(), TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP));
    }

    const TA_Base_Core::TrainPublicAddressParameters& trainPAParameter = parameterSet.trainPublicAddress();

    m_trainParameter        = new TrainStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TRAIN), trainPAParameter.destination);
    m_messageParameter    = new PaMessageStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_MESSAGE), trainPAParameter.message, true);
    m_cyclicParameter        = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PRIORITY), trainPAParameter.cyclic);
    m_durationParameter    = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_DURATION), trainPAParameter.duration, TimeSpanStepParameter::TR_MINUTES);

    stepParameters.clear();
    stepParameters.push_back(m_trainParameter);
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_cyclicParameter);
    stepParameters.push_back(m_durationParameter);

    FUNCTION_EXIT;
}


void TrainPAStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::TrainPublicAddressParameters trainPAParameter;

    trainPAParameter.destination = m_trainParameter->getDestination ();
    trainPAParameter.message = m_messageParameter->getPAMessage ();
    trainPAParameter.cyclic = m_cyclicParameter->getAffirmative ();
    trainPAParameter.duration = m_durationParameter->getNumericValue ();

    step.parameters.trainPublicAddress (trainPAParameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* TrainPAStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditPATrainPage(this);
}


#endif //defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)