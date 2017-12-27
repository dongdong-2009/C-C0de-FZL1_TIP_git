/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PidsStepParameter.cpp $
* @author:  Bart Golab
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2010/06/09 22:25:17 $
* Last modified by:  $Author: qi.huang $
*
* Represents the Station parameter of a Station TIS/PA step.
*
**/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include <sstream>
#include "boost/tokenizer.hpp"
#include "core/utilities/src/DebugUtil.h"
#include "PidsStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "StationZonesProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PidsStepParameter


namespace
{
	static std::vector<std::string> tokenizeString( std::string theString, const std::string& separatorList )
	{
		std::vector<std::string> parts;
		typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

		boost::char_separator<char> sep( separatorList.c_str() );
		tokenizer tokens( theString, sep );

		for ( tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter )
		{
			parts.push_back( *tok_iter );
		}

		// if parts is empty, then this should return the entire string
		if ( parts.size() == 0 )
		{
			parts.push_back( theString );
		}

		return parts;
	}
}


PidsStepParameter::PidsStepParameter(PlanStep& parent, const std::string& name) :
PlanStepParameter(parent, name)
{
	FUNCTION_ENTRY( "PidsStepParameter" );

	m_travelinfo.destinationList.length(0);

	FUNCTION_EXIT;
}


PidsStepParameter::PidsStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::StationTravellerInfoSystemParameters& stationtinfo):
PlanStepParameter(parent, name),
m_travelinfo(stationtinfo)
{
	FUNCTION_ENTRY( "PidsStepParameter" );
	FUNCTION_EXIT;
}


PidsStepParameter::PidsStepParameter(PlanStep& parent, const PidsStepParameter& parameter) :
PlanStepParameter(parent, parameter.getName()),
m_travelinfo(parameter.getStationTravelInfo())
{
	FUNCTION_ENTRY( "PidsStepParameter" );
	FUNCTION_EXIT;
}


PidsStepParameter::PidsStepParameter(const PidsStepParameter& parameter) :
PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
m_travelinfo(parameter.getStationTravelInfo())
{
	FUNCTION_ENTRY( "PidsStepParameter" );
	FUNCTION_EXIT;
}


PidsStepParameter::~PidsStepParameter()
{
	FUNCTION_ENTRY( "~PidsStepParameter" );
	FUNCTION_EXIT;
}


void PidsStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
	FUNCTION_ENTRY( "addToParameterList" );
	/*

	std::string destStr = (m_destination.allStations? "All stations" : m_destination.station.in());

	listCtrl.AddParameter(getName().c_str(), destStr.c_str(), this);
	*/
	FUNCTION_EXIT;
}


void PidsStepParameter::setStationTravellerInfo(const TA_Base_Core::StationTravellerInfoSystemParameters& stationtinfo)
{
	FUNCTION_ENTRY( "setDestination" );

	m_travelinfo = stationtinfo;

	FUNCTION_EXIT;
}



TA_Base_App::CPlanStepParameterEditDlg* PidsStepParameter::getEditDialog(bool canEdit)
{
	FUNCTION_ENTRY( "getEditDialog" );
	FUNCTION_EXIT;
	return new TA_Base_App::CPlanStepParameterEditDlg(*this, canEdit);
}


void PidsStepParameter::setPidsDestinationList(TA_Base_Core::PidsDestinationList& destinationList,
							const TA_Base_Core::TISPidList& pidlist
							)
{
	//wenbo++
	const std::string SEPERATOR(".");
	//reset destinationList length
	destinationList.length(0);
	std::vector<std::string> strVector;
	std::map<std::string,unsigned long> stationMap = makestaionMap();

	for(int i = 0; i < pidlist.length(); i++)
	{
		strVector = tokenizeString(pidlist[i].in(), SEPERATOR);
		int j = 0;
		for(j = 0; j < destinationList.length(); j++)
		{
			//if station name is already in destinationList
			if(destinationList[j].station == stationMap[strVector[0]])
			{
				if(strVector[2] == zonetypes[0])
					destinationList[j].upTrack = true;
				else if(strVector[2] == zonetypes[1])
					destinationList[j].downTrack = true;
				else  if(strVector[2] == zonetypes[2])
					destinationList[j].concourse = true;
				else
					destinationList[j].gateway = true;
				break;
			}
		}

		if(j == destinationList.length())
		{
			destinationList.length(destinationList.length() + 1);
			destinationList[destinationList.length() - 1].station = stationMap[strVector[0]];

			//initialization
			destinationList[destinationList.length() - 1].gateway = false;
			destinationList[destinationList.length() - 1].downTrack = false;
			destinationList[destinationList.length() - 1].upTrack = false;
			destinationList[destinationList.length() - 1].concourse = false;

			if(strVector[2] == zonetypes[0])
				destinationList[destinationList.length() - 1].upTrack = true;
			else if(strVector[2] == zonetypes[1])
				destinationList[destinationList.length() - 1].downTrack = true;
			else if(strVector[2] == zonetypes[2])
				destinationList[destinationList.length() - 1].concourse = true;
			else
				destinationList[destinationList.length() - 1].gateway = true;
		}
	}
	

	
}

void PidsStepParameter::setStationTravellerInfo(const TA_Base_Core::TISPidList& pidlist, 
							 bool msgtype,
							 bool showtype,
							 std::wstring msgcontent)
{
	//wenbo++
	m_travelinfo.pidList = pidlist;
	//set PidsDestinationList
	setPidsDestinationList(m_travelinfo.destinationList, pidlist);
	m_travelinfo.msgType = msgtype;
	m_travelinfo.showType = showtype;
	
	//set m_travelinfo.messageContent
	
	CString tmpStr(msgcontent.c_str());
	int size = tmpStr.GetLength();
	m_travelinfo.messageContent.length(size);

	std::string temp = tmpStr.GetBuffer(size);
	std::string::iterator it = temp.begin();

	for(unsigned int i=0; i<size; ++i, ++it)
	{
		m_travelinfo.messageContent[i] = *it;
	}	

	//startTime,endTime,interVal... not use,these values will assignment 0 by planagent datawriter

}
#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)
