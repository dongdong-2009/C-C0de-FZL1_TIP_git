/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainTisStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents a Train Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "TrainTisStep.h"
#include "resource.h"
#include "TrainStepParameter.h"
#include "TisMessageStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "TisPriorityStepParameter.h"
#include "PlanStepEditTrainTisPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TrainTisStep

TrainTisStep::TrainTisStep(PlanNode* parent,const TA_Base_Core::StepDetail& stepDetail) :
    PlanStep(parent,stepDetail),
    m_trainParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL)
{
    FUNCTION_ENTRY( "TrainTisStep" );
    FUNCTION_EXIT;
}


TrainTisStep::TrainTisStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP),
    m_trainParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL)
{
    FUNCTION_ENTRY( "TrainTisStep" );
    FUNCTION_EXIT;
}


TrainTisStep::TrainTisStep(const TrainTisStep& planStep) :
    PlanStep(planStep),
    m_trainParameter(NULL),
    m_messageParameter(NULL),
    m_durationParameter(NULL),
    m_priorityParameter(NULL)
{
    FUNCTION_ENTRY( "TrainTisStep" );

    m_trainParameter = new TrainStepParameter(*this, *planStep.m_trainParameter);
    m_messageParameter = new TisMessageStepParameter(*this, *planStep.m_messageParameter);
    m_priorityParameter = new TisPriorityStepParameter(*this, *planStep.m_priorityParameter);
    m_durationParameter = new TimeSpanStepParameter(*this, *planStep.m_durationParameter);

    addPlanStepParameter(*m_trainParameter);
    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_priorityParameter);
    addPlanStepParameter(*m_durationParameter);

    FUNCTION_EXIT;
}


TrainTisStep::~TrainTisStep()
{
    FUNCTION_ENTRY( "~TrainTisStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* TrainTisStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new TrainTisStep(*this);
}


void TrainTisStep::createPlanStepParameters(PlanStepParameterList& stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    m_trainParameter = new TrainStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TRAIN));
    m_messageParameter = new TisMessageStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_MESSAGE), TisMessageStepParameter::MT_TRAIN);
    m_priorityParameter = new TisPriorityStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PRIORITY), 0, true);
    m_durationParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_DURATION), 0, TimeSpanStepParameter::TR_MINUTES, true);

    stepParameters.clear();
    stepParameters.push_back(m_trainParameter);
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_priorityParameter);
    stepParameters.push_back(m_durationParameter);

    FUNCTION_EXIT;
}


void TrainTisStep::createPlanStepParameters(const TA_Base_Core::StepParameters& parameterSet,
                                              PlanStepParameterList& stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for Train TIS step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP));
    }

    const TA_Base_Core::TrainTravellerInfoSystemParameters& trainTisParameter = parameterSet.trainTravellerInfoSystem();

    m_trainParameter = new TrainStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TRAIN), trainTisParameter.destination);
    m_messageParameter = new TisMessageStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_MESSAGE), trainTisParameter.message, TisMessageStepParameter::MT_TRAIN);
    m_priorityParameter = new TisPriorityStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PRIORITY), trainTisParameter.priority);
    m_durationParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_DURATION), trainTisParameter.duration, TimeSpanStepParameter::TR_MINUTES);

    stepParameters.clear();
    stepParameters.push_back(m_trainParameter);
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_priorityParameter);
    stepParameters.push_back(m_durationParameter);

    FUNCTION_EXIT;
}


void TrainTisStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::TrainTravellerInfoSystemParameters trainTisParameter;

    trainTisParameter.destination = m_trainParameter->getDestination();
    trainTisParameter.message = m_messageParameter->getMessage();
    trainTisParameter.priority = m_priorityParameter->getPriority();
    trainTisParameter.duration = m_durationParameter->getNumericValue();

    step.parameters.trainTravellerInfoSystem(trainTisParameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* TrainTisStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditTrainTisPage(this);
}


#endif // defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)
