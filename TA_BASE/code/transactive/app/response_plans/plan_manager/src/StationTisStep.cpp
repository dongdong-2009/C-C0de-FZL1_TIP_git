/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/StationTisStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Represents a Station Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "StationTisStep.h"
#include "resource.h"
#include "PidsStepParameter.h"
#include "TisPidListStepParameter.h"
#include "TisMessageStepParameter.h"
#include "TisPriorityStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "TextualStepParameter.h"
#include "PlanStepEditStationTisPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// StationTisStep

StationTisStep::StationTisStep(PlanNode* parent,const TA_Base_Core::StepDetail& stepDetail) :
    PlanStep(parent,stepDetail),
    m_pidsstepParameter(NULL),
	m_pidListParameter(NULL)
{
    FUNCTION_ENTRY( "StationTisStep" );
    FUNCTION_EXIT;
}


StationTisStep::StationTisStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP),
    m_pidsstepParameter(NULL),
	m_pidListParameter(NULL)
{
    FUNCTION_ENTRY( "StationTisStep" );
    FUNCTION_EXIT;
}


StationTisStep::StationTisStep(const StationTisStep& planStep) :
    PlanStep(planStep),
    m_pidsstepParameter(NULL),
	m_pidListParameter(NULL)
{
    FUNCTION_ENTRY( "StationTisStep" );


	m_pidsstepParameter = new PidsStepParameter(*this, *planStep.m_pidsstepParameter);
	m_pidListParameter = new TisPidListStepParameter(*this, *planStep.m_pidListParameter);
    addPlanStepParameter(*m_pidsstepParameter);   
	addPlanStepParameter(*m_pidListParameter);

    FUNCTION_EXIT;
}


StationTisStep::~StationTisStep()
{
    FUNCTION_ENTRY( "~StationTisStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* StationTisStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new StationTisStep(*this);
}


void StationTisStep::createPlanStepParameters(PlanStepParameterList& stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

	m_pidsstepParameter = new PidsStepParameter(*this, "");	
    m_pidListParameter = new TisPidListStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PID_LIST));   

    stepParameters.clear();
    stepParameters.push_back(m_pidsstepParameter);
	stepParameters.push_back(m_pidListParameter);
	/*
    stepParameters.push_back(m_stationLevelParameter);
    stepParameters.push_back(m_pidListParameter);
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_priorityParameter);
    stepParameters.push_back(m_durationParameter);
	*/

    FUNCTION_EXIT;
}


void StationTisStep::createPlanStepParameters(const TA_Base_Core::StepParameters& parameterSet,
                                              PlanStepParameterList& stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for Station TIS step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP));
    }

    const TA_Base_Core::StationTravellerInfoSystemParameters& stationTisParameter = parameterSet.stationTravellerInfoSystem();

	
    m_pidListParameter = new TisPidListStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIS_PID_LIST), stationTisParameter.pidList);
    
	m_pidsstepParameter = new PidsStepParameter(*this, "", stationTisParameter);

    stepParameters.clear();
	stepParameters.push_back(m_pidsstepParameter);
	stepParameters.push_back(m_pidListParameter);

	/*
    stepParameters.push_back(m_stationParameter);
    stepParameters.push_back(m_stationLevelParameter);
    stepParameters.push_back(m_pidListParameter);
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_priorityParameter);
    stepParameters.push_back(m_durationParameter);
	*/

    FUNCTION_EXIT;
}


void StationTisStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::StationTravellerInfoSystemParameters stationTisParameter;

	stationTisParameter = m_pidsstepParameter->getStationTravelInfo();
	/*
    stationTisParameter.destination = m_stationParameter->getDestination();
    stationTisParameter.stationLevel = CORBA::string_dup(m_stationLevelParameter->getMessageText().c_str());
    stationTisParameter.stationPids = m_pidListParameter->getPids();
    stationTisParameter.message = m_messageParameter->getMessage();
    stationTisParameter.priority = m_priorityParameter->getPriority();
    stationTisParameter.duration = m_durationParameter->getNumericValue();
	*/

    step.parameters.stationTravellerInfoSystem(stationTisParameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* StationTisStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditStationTisPage(this);
}


#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)
