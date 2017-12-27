/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Represents the Station parameter of a Station TIS/PA step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include "core/utilities/src/DebugUtil.h"
#include "StationStepParameter.h"
#include "PlanStepParameterEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// StationStepParameter

StationStepParameter::StationStepParameter(PlanStep& parent, const std::string& name) :
    PlanStepParameter(parent, name)
{
    FUNCTION_ENTRY( "StationStepParameter" );

    m_destination.station = CORBA::string_dup("");
    m_destination.locationKey = 0;
    m_destination.allStations = false;

    FUNCTION_EXIT;
}


StationStepParameter::StationStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::StationDestination& destination) :
    PlanStepParameter(parent, name),
    m_destination(destination)
{
    FUNCTION_ENTRY( "StationStepParameter" );
    FUNCTION_EXIT;
}


StationStepParameter::StationStepParameter(PlanStep& parent, const StationStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_destination(parameter.getDestination())
{
    FUNCTION_ENTRY( "StationStepParameter" );
    FUNCTION_EXIT;
}


StationStepParameter::StationStepParameter(const StationStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_destination(parameter.getDestination())
{
    FUNCTION_ENTRY( "StationStepParameter" );
    FUNCTION_EXIT;
}


StationStepParameter::~StationStepParameter()
{
    FUNCTION_ENTRY( "~StationStepParameter" );
    FUNCTION_EXIT;
}


void StationStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );
    
    listCtrl.AddParameter(getName().c_str(), m_destination.station.in(), this);

    FUNCTION_EXIT;
}


void StationStepParameter::setDestination(const TA_Base_Core::StationDestination& destination)
{
    FUNCTION_ENTRY( "setDestination" );

    m_destination = destination;

    FUNCTION_EXIT;
}


void StationStepParameter::setStation(const std::string& station, const unsigned long ulLocationkey)
{
    FUNCTION_ENTRY( "setStation" );

    m_destination.station = station.c_str();
    m_destination.locationKey = ulLocationkey;
    m_destination.allStations = false;

    FUNCTION_EXIT;
}

void StationStepParameter::setAllStations(const std::string& station)
{
    FUNCTION_ENTRY( "setAllStations" );

    m_destination.station = station.c_str();
    //m_destination.locationKey = 0;
    m_destination.allStations = true;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* StationStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this, canEdit);
}


#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM) || defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
