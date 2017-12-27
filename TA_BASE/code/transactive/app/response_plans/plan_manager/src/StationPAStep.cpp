//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/StationPAStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "StationPAStep.h"
#include "core/exceptions/src/PlanStepException.h"
#include "PlanManagerCommonDefs.h"
#include "PlanStepStationPAPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep
StationPAStep::StationPAStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent, stepDetail),
    m_station  (NULL),
     m_zones (NULL),
     m_message (NULL),
     m_chime    (NULL),
     m_cyclic    (NULL),
     m_TISSynch (NULL),
     m_duration  (NULL),
     m_interval        (NULL),
     m_cylicTimes( NULL )
{
    FUNCTION_ENTRY( "StationPAStep" );
    FUNCTION_EXIT;
}


StationPAStep::StationPAStep(PlanNode* parent, PlanStepId stepId) :
    PlanStep(parent,stepId, TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP),
    m_station  (NULL),
     m_zones (NULL),
     m_message (NULL),
     m_chime    (NULL),
     m_cyclic    (NULL),
     m_TISSynch (NULL),
     m_duration  (NULL),
     m_interval        (NULL),
     m_cylicTimes( NULL )
{
    FUNCTION_ENTRY( "StationPAStep" );
    FUNCTION_EXIT;
}


StationPAStep::StationPAStep(const StationPAStep& planStep) :
    PlanStep(planStep),
    m_station  (NULL),
     m_zones (NULL),
     m_message (NULL),
     m_chime    (NULL),
     m_cyclic    (NULL),
     m_TISSynch (NULL),
     m_duration  (NULL),
     m_interval        (NULL),
     m_cylicTimes( NULL )
{
    FUNCTION_ENTRY( "StationPAStep" );

    m_station  = new StationStepParameter  (*this, *planStep.m_station);
     m_duration = new TimeSpanStepParameter   (*this, *planStep.m_duration);
     m_interval = new NumericStepParameter   (*this, *planStep.m_interval);
     m_cylicTimes = new NumericStepParameter   (*this, *planStep.m_cylicTimes);
     m_chime    = new YesNoStepParameter      (*this, *planStep.m_chime);
     m_cyclic    = new YesNoStepParameter      (*this, *planStep.m_cyclic);
     m_TISSynch = new YesNoStepParameter      (*this, *planStep.m_TISSynch);
     m_message  = new PaMessageStepParameter (*this, *planStep.m_message);
     m_zones      =  new PaZonesStepParameter      (*this, *planStep.m_zones);

    addPlanStepParameter(*m_station);
     //addPlanStepParameter(*m_duration);
     //addPlanStepParameter(*m_interval);
     addPlanStepParameter(*m_cylicTimes);
     //addPlanStepParameter(*m_chime);
     //addPlanStepParameter(*m_cyclic);
     //addPlanStepParameter(*m_TISSynch);
     addPlanStepParameter(*m_message);
     addPlanStepParameter(*m_zones);

    FUNCTION_EXIT;
}


StationPAStep::~StationPAStep()
{
    FUNCTION_ENTRY( "~StationPAStep" );
    FUNCTION_EXIT;
}


PlanStep* StationPAStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new StationPAStep(*this);
}


void StationPAStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                             PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for PA Station step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP));
    }
    delete m_station;
    delete m_duration;
    delete m_interval;
    delete m_cylicTimes;
    delete m_chime;
    delete m_cyclic;
    delete m_TISSynch;
    delete m_message;
    delete m_zones;

    m_station    = new StationStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_STATION), parameterSet.stationPublicAddress().station);
    m_duration     = new TimeSpanStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_DURATION), parameterSet.stationPublicAddress().duration, TimeSpanStepParameter::TR_SECONDS);
    m_interval     = new NumericStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_INTERVAL), parameterSet.stationPublicAddress().interval);
    m_cylicTimes     = new NumericStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_CYLIC_TIMES), parameterSet.stationPublicAddress().cyclicTimes);
    m_chime         = new YesNoStepParameter        (*this, getParameterName (IDS_STEP_PARAMETER_PA_CHIME), parameterSet.stationPublicAddress().chime);
    m_cyclic         = new YesNoStepParameter        (*this, getParameterName (IDS_STEP_PARAMETER_PA_CYCLIC), parameterSet.stationPublicAddress().cyclic);
    m_TISSynch   = new YesNoStepParameter     (*this, getParameterName (IDS_STEP_PARAMETER_PA_TIS_SYNCH), parameterSet.stationPublicAddress().TISSynch);
    m_message    = new PaMessageStepParameter (*this, getParameterName (IDS_STEP_PARAMETER_PA_MESSAGE), parameterSet.stationPublicAddress().message);
    m_zones         = new PaZonesStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_ZONE),parameterSet.stationPublicAddress().zones);

    stepParameters.clear();
    stepParameters.push_back(m_station);
    //stepParameters.push_back(m_duration);
    //stepParameters.push_back(m_interval);
    stepParameters.push_back(m_cylicTimes);
    //stepParameters.push_back(m_chime);
    //stepParameters.push_back(m_cyclic);
    //stepParameters.push_back(m_TISSynch);
    stepParameters.push_back(m_message);
    stepParameters.push_back(m_zones);

    FUNCTION_EXIT;
}


void StationPAStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_station;
    delete m_duration;
    delete m_interval;
    delete m_cylicTimes;
    delete m_chime;
    delete m_cyclic;
    delete m_TISSynch;
    delete m_message;

    TA_Base_Core::StationDestination station;
    station.station = CORBA::string_dup("");
   station.allStations = false;
    TA_Base_Core::PAMessage message;
    message.messageId = TA_Base_Core::PA_UNDEFINED_MESSAGE_ID;
    TA_Base_Core::StationZones zones;

    m_station    = new StationStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_STATION), station);
    m_duration     = new TimeSpanStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_DURATION), 0, TimeSpanStepParameter::TR_SECONDS, true);
    m_interval     = new NumericStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_INTERVAL), 5);
    m_cylicTimes     = new NumericStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_CYLIC_TIMES), MIN_PA_CYLIC_TIMES);
    m_chime         = new YesNoStepParameter        (*this, getParameterName (IDS_STEP_PARAMETER_PA_CHIME), MIN_PA_MESSAGE_INTERVAL);
    m_cyclic         = new YesNoStepParameter        (*this, getParameterName (IDS_STEP_PARAMETER_PA_CYCLIC), 0);
    m_TISSynch   = new YesNoStepParameter     (*this, getParameterName (IDS_STEP_PARAMETER_PA_TIS_SYNCH), 0);
    m_message    = new PaMessageStepParameter (*this, getParameterName (IDS_STEP_PARAMETER_PA_MESSAGE), message);
    m_zones         = new PaZonesStepParameter    (*this, getParameterName (IDS_STEP_PARAMETER_PA_ZONE), zones);

    stepParameters.clear();
    stepParameters.push_back(m_station);
    //stepParameters.push_back(m_duration);
    //stepParameters.push_back(m_interval);
    stepParameters.push_back(m_cylicTimes);
    //stepParameters.push_back(m_chime);
    //stepParameters.push_back(m_cyclic);
    //stepParameters.push_back(m_TISSynch);
    stepParameters.push_back(m_message);
    stepParameters.push_back(m_zones);

    FUNCTION_EXIT;
}


void StationPAStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::StationPublicAddressParameters parameters;

   parameters.station   = m_station->getDestination ();
    parameters.zones     = m_zones->getPAZones ();
    parameters.message   = m_message->getPAMessage ();
    parameters.chime     = m_chime->getAffirmative ();
    parameters.cyclic        = m_cyclic->getAffirmative ();
    parameters.TISSynch  = m_TISSynch->getAffirmative ();
    parameters.duration    = m_duration->getNumericValue ();
    parameters.interval  = m_interval->getNumericValue ();
    parameters.cyclicTimes  = m_cylicTimes->getNumericValue ();

    step.parameters.stationPublicAddress(parameters);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* StationPAStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepStationPAPage (this);
}


#endif // defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
