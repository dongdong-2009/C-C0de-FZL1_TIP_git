/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/CombinedEventFilter.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/07/18 15:34:16 $
  * Last modified by:  $Author: huirong.luo $
  *
  * CombinedEventFilter is an implementation of the abstract class Filter, from the
  * EventListComponent. It specifies the details of events from both the EVENT table
  * and the LO_DATAPT_STATE_UPDATE table, and includes the column names, and
  * filterable data items, in constant static variables.
  *
  */

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "stdafx.h"

#include <algorithm>
#include <functional>
#include <map>
#include <utility>


#include "app/event/event_viewer/src/ConstNames.h"
#include "app/event/event_viewer/src/ActionCreateIncidentReport.h"
#include "app/event/event_viewer/src/ActionShowComment.h"
#include "app/event/event_viewer/src/AvalancheNamedFilter.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"
#include "app/event/event_viewer/src/CombinedEventFilterDlg.h"
#include "app/event/event_viewer/src/CombinedEventItem.h"
#include "app/event/event_viewer/src/DatabaseAccess.h"
#include "app/event/event_viewer/src/CreateDefaultFilterThread.h" //zhiqiang++
#include "ConstantItems.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/CombinedEventAccessFactory.h"
#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

using namespace constnames;
using TA_Base_Core::CombinedEventAccessFactory;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ICombinedEventData;
using TA_Base_Core::IDatabaseCallbackUser;

// Assign the required values to the column constants.
const std::string CombinedEventFilter::COLUMN_DATETIME(constnames::COLUMN_DATETIME);
const std::string CombinedEventFilter::COLUMN_SEVERITY(constnames::COLUMN_SEVERITY);
const std::string CombinedEventFilter::COLUMN_ASSET(constnames::COLUMN_ASSET);
const std::string CombinedEventFilter::COLUMN_ALARM_STATUS(constnames::COLUMN_ALARM_STATUS); // add this column
const std::string CombinedEventFilter::COLUMN_DESCRIPTION(constnames::COLUMN_DESCRIPTION);
const std::string CombinedEventFilter::COLUMN_VALUE(constnames::COLUMN_VALUE);
const std::string CombinedEventFilter::COLUMN_T(constnames::COLUMN_T); //add this column
const std::string CombinedEventFilter::COLUMN_MMS(constnames::COLUMN_MMS);
const std::string CombinedEventFilter::COLUMN_DSS(constnames::COLUMN_DSS); //change from "D" to "P"
const std::string CombinedEventFilter::COLUMN_AVL(constnames::COLUMN_AVL);
const std::string CombinedEventFilter::COLUMN_OPERATOR(constnames::COLUMN_OPERATOR);
const std::string CombinedEventFilter::COLUMN_COMMENT(constnames::COLUMN_COMMENT);

//hiden columns for saving redundant info : upper case value for column (equipment, description, val, operator)
const std::string CombinedEventFilter::COLUMN_DESCRIPTION_UPPER("Desription_Upper");
const std::string CombinedEventFilter::COLUMN_VALUE_UPPER("Value_Upper");
const std::string CombinedEventFilter::COLUMN_OPERATOR_UPPER("Operator_Upper");
const std::string CombinedEventFilter::COLUMN_ASSET_UPPER("Equipment_Upper");

// Assign the required values to the hidden column constants.
const std::string CombinedEventFilter::HIDDEN_COLUMN_EVENTID("Event ID");
const std::string CombinedEventFilter::HIDDEN_COLUMN_ALARMID("Alarm ID");
const std::string CombinedEventFilter::HIDDEN_COLUMN_ACKED("Acked");
const std::string CombinedEventFilter::HIDDEN_COLUMN_STATUS("Status");
//const std::string CombinedEventFilter::HIDDEN_COLUMN_SELECTED("Selected");

// Assign the required values to the filter constants.
const std::string CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM(constnames::FILTER_APPLICATION_SUBSYSTEM);
const std::string CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM(constnames::FILTER_PHYSICAL_SUBSYSTEM);
const std::string CombinedEventFilter::FILTER_SYSTEM_STR("System string list");  // This is for saving NamedFilter into DB
const std::string CombinedEventFilter::FILTER_SUBSYSTEM_STR("Subsystem string list");  // This is for saving NamedFilter into DB
const std::string CombinedEventFilter::FILTER_LOCATION(constnames::FILTER_LOCATION);
const std::string CombinedEventFilter::FILTER_OPERATOR(constnames::FILTER_OPERATOR);
const std::string CombinedEventFilter::FILTER_OPERATOR_STR("Operator name list");
const std::string CombinedEventFilter::FILTER_LOCATION_STR("Location name list");
const std::string CombinedEventFilter::FILTER_EVENTTYPE(constnames::FILTER_EVENTTYPE);  
const std::string CombinedEventFilter::FILTER_ISCS_SYSTEM("System");  // add for 955
const std::string CombinedEventFilter::FILTER_TMTYPE("OMType");  // add for 955
const std::string CombinedEventFilter::FILTER_ALARMSTATE("Alarm State");  // add for 955
const std::string CombinedEventFilter::FILTER_ASSET(constnames::FILTER_ASSET);
const std::string CombinedEventFilter::FILTER_DESCRIPTION(constnames::FILTER_DESCRIPTION);
const std::string CombinedEventFilter::FILTER_SEVERITY(constnames::FILTER_SEVERITY);
const std::string CombinedEventFilter::FILTER_ALARMTYPE(constnames::FILTER_ALARMTYPE);
const std::string CombinedEventFilter::FILTER_ALARMID(constnames::FILTER_ALARMID);


// The name of the default filter.
const std::string CombinedEventFilter::NAMEDFILTER_DEFAULT( constnames::NAMEDFILTER_DEFAULT );

const unsigned long CombinedEventFilter::MIN_DISPLAY_ITEM_NUM = 50;

using namespace std;


CombinedEventFilter::CombinedEventFilter() : m_latestPkey(0), m_authenticationLib(NULL), 
                                             m_pCreateDefaultFilterThread(NULL),//zhiqiang++
											 m_eventHanlderManager(NULL)
{
    // Initialise the default background colour to beige and foreground to black.
	m_foregroundColour = EventViewerConstants::COLOUR_BLACK;
	m_backgroundColour = EventViewerConstants::COLOUR_BEIGE;

    // Set up the actions.	
    m_actions.push_back( new ActionCreateIncidentReport() );
    m_actions.push_back( new ActionShowComment() );

    // Add all the required columns to the vector.
	// TD 13144 increase width of date/time and asset name to ensure they are fully visible
    m_columns.push_back( Filter::Column( COLUMN_DATETIME, 165, ACTION_NO_ACTION ) ); //TD15005
    m_columns.push_back( Filter::Column( COLUMN_SEVERITY, 50, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_ASSET, 210, ACTION_NO_ACTION ) ); //TD15005
    m_columns.push_back( Filter::Column( COLUMN_DESCRIPTION, 450, ACTION_NO_ACTION ) ); //TD15005
    m_columns.push_back( Filter::Column( COLUMN_VALUE, 167, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_MMS, 24, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_DSS, 24, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_AVL, 24, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_OPERATOR, 150, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_COMMENT, 24, ACTION_SHOW_COMMENT ) );

	//*zhiqiang++begin
	//m_pCreateDefaultFilterThread = new CCreateDefaultFilterThread(this);
	//m_pCreateDefaultFilterThread->start();
	CreateDefaultFilter();
	//*/ //zhiqiang++end

	// Setup loading thread.
	//m_loadingThread = new LoadingThread(this);
 
	m_eventHanlderManager = new EventHandlerManager(this);
	m_eventHanlderManager->sethwndDlg(AfxGetMainWnd()->GetSafeHwnd());
	// Set default sorting.
    sort( getDefaultSortColumn(), false );

}

CombinedEventFilter::~CombinedEventFilter()
{
    FUNCTION_ENTRY("destructor");

    unsigned int i;

    // Remove all columns.
    m_columns.clear();

    // The filter owns any actions so we need to delete them.
    for ( i=0; i<m_actions.size(); i++ )
    {
        delete m_actions[i];
    }
    m_actions.clear();
	if( m_authenticationLib != NULL )
	{
		delete m_authenticationLib;
		m_authenticationLib = NULL;
	}
	//zhiqiang++begin
	if (m_pCreateDefaultFilterThread != NULL)
	{
		delete m_pCreateDefaultFilterThread;
		m_pCreateDefaultFilterThread = NULL;
	}
    //zhiqiang++end
	if(m_eventHanlderManager != NULL)
	{
		delete m_eventHanlderManager ;
		m_eventHanlderManager = NULL;
	}
    FUNCTION_EXIT;
}



const std::vector<Filter::Column>& CombinedEventFilter::getColumns() const
{
    return m_columns;
}

const std::vector<std::string> CombinedEventFilter::getFilters() const
{
    // Filters will be returned 
    std::vector<std::string> filters;

    // Add all the filters to the list.
    filters.push_back(FILTER_APPLICATION_SUBSYSTEM);
    filters.push_back(FILTER_PHYSICAL_SUBSYSTEM);
	filters.push_back(FILTER_SUBSYSTEM_STR);
    filters.push_back(FILTER_LOCATION);
    filters.push_back(FILTER_OPERATOR);

	filters.push_back(FILTER_ISCS_SYSTEM);   
	filters.push_back(FILTER_SYSTEM_STR);  
	filters.push_back(FILTER_OPERATOR_STR);  
	filters.push_back(FILTER_LOCATION_STR);  

	filters.push_back(FILTER_EVENTTYPE);
	filters.push_back(FILTER_ALARMSTATE);
    filters.push_back(FILTER_ASSET);
    filters.push_back(FILTER_DESCRIPTION);
    filters.push_back(FILTER_SEVERITY);
    filters.push_back(FILTER_ALARMTYPE);
    filters.push_back(FILTER_ALARMID);

    return filters;
}




// yanrong++ CL-15055
std::string CombinedEventFilter::privGetCurrentFilterString()
{
	FUNCTION_ENTRY("CombinedEventFilter::privGetCurrentFilterString");

	// Filter data is contained in two maps - a string-string and a string-long map, so create iterators for both.
	TA_Base_Core::NamedFilter::StringList stringFilterList = m_namedFilter->getStringFilterList();
	TA_Base_Core::NamedFilter::StringList numericFilterList = m_namedFilter->getNumericFilterList();

	TA_Base_Core::NamedFilter::StringList::iterator stringIter;

	// Iterate through each map, adding any filters to the string..
	std::string filterString("");
	for ( stringIter=stringFilterList.begin(); stringIter!=stringFilterList.end(); stringIter++ )
	{
		if( *stringIter == CombinedEventFilter::FILTER_SYSTEM_STR
			|| *stringIter == CombinedEventFilter::FILTER_SUBSYSTEM_STR
			|| *stringIter == CombinedEventFilter::FILTER_OPERATOR_STR
			|| *stringIter == CombinedEventFilter::FILTER_LOCATION_STR)
		{
			continue;
		}

		filterString += " [" + *stringIter + "]";
	}

	//to make sure from time display in front of to time on caption
	bool hasFilterToTime = false;
	bool hasFilterFromeTime = false;
	for ( stringIter=numericFilterList.begin(); stringIter!=numericFilterList.end(); stringIter++ )
	{
		{
			if( *stringIter == Filter::FILTER_TOTIME )
			{
				if(hasFilterFromeTime)
				{
					filterString += " [" + *stringIter + "]";
				}
				else
				{
					hasFilterToTime = true;
				}
			}
			else
			{
				filterString += " [" + *stringIter + "]";
				if( *stringIter == Filter::FILTER_FROMTIME && hasFilterToTime)
				{
					hasFilterFromeTime = true;
					hasFilterToTime = false;
					filterString += " [" + Filter::FILTER_TOTIME + "]";
				}
			}
		}
	}
	if(hasFilterToTime)
	{
		filterString += " [" + Filter::FILTER_TOTIME + "]";
	}

	FUNCTION_EXIT;
	return filterString;
}


void CombinedEventFilter::saveAsDefaultFilter(const TA_Base_Core::NamedFilter& filter)
{
	m_defaultFilterString = privGetCurrentFilterString();
}
// ++yanrong CL-15055



std::string CombinedEventFilter::getCurrentFilterString()
{
    FUNCTION_ENTRY("CombinedEventFilter::getCurrentFilterString");

    std::string filterString = privGetCurrentFilterString();
	if(filterString.compare(m_defaultFilterString) == 0)
		filterString = "";

    FUNCTION_EXIT;
    return filterString.c_str();
}

const std::string CombinedEventFilter::getDefaultSortColumn()
{
    // Default sort will always apply to time column.
    return COLUMN_DATETIME.c_str();
}

void CombinedEventFilter::resetListLoadFlag()
{
    // Reset load flag on the DAI.
    CombinedEventAccessFactory::getInstance().resetStopFlag();
}

void CombinedEventFilter::populateTimeAndSortingToFilterCriteria (FilterCriteria & filter)
{
	// Set the time range.
	TA_Base_Core::NamedFilter::NumericList fromTimeList = m_namedFilter->getNumericFilter( FILTER_FROMTIME );
	if ( 0 < fromTimeList.size() && fromTimeList[0]!=0)
	{
		filter.StartTimePeriodDefined = true;
		CTime ct (static_cast<time_t>(fromTimeList[0]));
		filter.StartTimePeriod.Year = ct.GetYear();
		filter.StartTimePeriod.Month = ct.GetMonth();
		filter.StartTimePeriod.Day = ct.GetDay();
		filter.StartTimePeriod.Hour = ct.GetHour();
		filter.StartTimePeriod.Minute = ct.GetMinute();
		filter.StartTimePeriod.Second = ct.GetSecond();
	}else{
		// this value may not be correct when a filter applied, so will reset this before submit to eventHandlerManager
		filter.StartTimePeriodDefined = false;   
	}

	TA_Base_Core::NamedFilter::NumericList toTimeList = m_namedFilter->getNumericFilter( FILTER_TOTIME);
	if ( 0 < toTimeList.size() && toTimeList[0]!=0 )
	{
		filter.EndTimePeriodDefined = true;
		CTime ct (static_cast<time_t>(toTimeList[0]));
		filter.EndTimePeriod.Year = ct.GetYear();
		filter.EndTimePeriod.Month = ct.GetMonth();
		filter.EndTimePeriod.Day = ct.GetDay();
		filter.EndTimePeriod.Hour = ct.GetHour();
		filter.EndTimePeriod.Minute = ct.GetMinute();
		filter.EndTimePeriod.Second = ct.GetSecond();

	}else{
		filter.EndTimePeriodDefined = false;
	}

	// Column to sort by.
	if ( COLUMN_DATETIME == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_CREATETIME;
	}
	else if ( COLUMN_SEVERITY == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_SEVERITY;
	}
	else if ( COLUMN_ASSET == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_ASSET;
	}
	else if ( COLUMN_ALARM_STATUS == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_ALARM_STATUS;
	}
	else if ( COLUMN_DESCRIPTION == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_DESCRIPTION;
	}
	else if ( COLUMN_VALUE == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_VALUE;
	}
	else if ( COLUMN_T == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_OPERATION_MODE;
	}
	else if ( COLUMN_MMS == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_MMS;
	}
	else if ( COLUMN_DSS == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_DSS;
	}
	else if ( COLUMN_AVL == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_AVL;
	}
	else if ( COLUMN_OPERATOR == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_OPERATOR;
	}
	else if ( COLUMN_COMMENT == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_COMMENT;
	}
	else
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_CREATETIME;
	}

	// Order in which the sort should be performed.
	if ( DisplayItem::s_sortAscending )
	{
		filter.sortOrder = CombinedEventAccessFactory::SORT_ASCENDING;
	}
	else
	{
		filter.sortOrder = CombinedEventAccessFactory::SORT_DESCENDING;
	}



}


void CombinedEventFilter::populateFilterCriteria (FilterCriteria & filter)
{
	std::vector<std::string> stringVector;
	std::vector<unsigned long> ulVector;

	/* for profile ID*/  
	if ( m_rightEnabled )
	{
		filter.profiles.clear();
		{
			TA_Base_Core::ThreadGuard storeGuard(m_sessionMapLock);
			std::string str_sessionId( TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) );
			SESSION_MAP::iterator iter=m_SessionMap.find(str_sessionId) ;
			if (iter!=m_SessionMap.end())
			{
				TA_Base_Bus::SessionInfo sessionInfo = iter->second;
				filter.profiles = sessionInfo.ProfileId;
			}
			else
			{
				if( m_authenticationLib != NULL )  
				{				
					TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib->getSessionInfo(str_sessionId, str_sessionId);
					filter.profiles = sessionInfo.ProfileId;	
					m_SessionMap.insert(SESSION_MAP::value_type(str_sessionId.c_str(), sessionInfo));
 					//filter.profiles.push_back(0); //TODO: just for testing

				}
			}
		}
	}
	//

	ulVector = m_namedFilter->getNumericFilter(FILTER_APPLICATION_SUBSYSTEM);
	if ( !ulVector.empty() )
	{
		filter.appSubsystemIds = ulVector;
	}

	// Physical subsystem Ids.
	ulVector = m_namedFilter->getNumericFilter(FILTER_PHYSICAL_SUBSYSTEM);
	if ( !ulVector.empty() )
	{
		filter.physSubsystemIds = ulVector;
	}


	// Location Ids
	ulVector = m_namedFilter->getNumericFilter(FILTER_LOCATION);
	if ( !ulVector.empty() )
	{
		filter.locationIds = ulVector;
	}

	// Operator Ids
	ulVector = m_namedFilter->getNumericFilter(FILTER_OPERATOR);
	if ( !ulVector.empty() )
	{
		filter.operatorIds = ulVector;
	}

	// Event Types
	ulVector = m_namedFilter->getNumericFilter(FILTER_EVENTTYPE);
	if ( !ulVector.empty() )
	{
		for (int i=0; i<(int)ulVector.size(); i++)
		{
			filter.eventTypeIds.push_back(ulVector[i]);
		}
		
	}
	
	//operatorNameEqualOr
	stringVector = m_namedFilter->getStringFilter(FILTER_OPERATOR_STR);
	for (int i=0; i< (int)stringVector.size(); i++)
	{
		filter.OperatorEqualOr.push_back(stringVector[i]);
	}

	//locationNameEqualOr
	stringVector = m_namedFilter->getStringFilter(FILTER_LOCATION_STR);
	for (int i=0; i<(int)stringVector.size(); i++)
	{
		filter.LocationEqualOr.push_back(stringVector[i]);
	}

	// ATS subsystem name
	stringVector = m_namedFilter->getStringFilter(FILTER_SUBSYSTEM_STR);
	for (int i=0; i<(int)stringVector.size(); i++)
	{
		filter.SubsystemNameEqualOr.push_back(stringVector[i]);
	}

	// ATS system name 
	stringVector = m_namedFilter->getStringFilter(FILTER_SYSTEM_STR);
	for (int i=0; i<(int)stringVector.size(); i++)
	{
		filter.SystemNameEqualOr.push_back(stringVector[i]);
	}

	// Asset Name
	stringVector = m_namedFilter->getStringFilter(FILTER_ASSET);
	if ( !stringVector.empty() )
	{
		filter.Asset.push_back(stringVector[0]);
	}

	// Start with description
	stringVector = m_namedFilter->getStringFilter(FILTER_DESCRIPTION);
	if ( !stringVector.empty() )
	{
		filter.Description.push_back(stringVector[0]) ;
	}

	// Set the time range.
	TA_Base_Core::NamedFilter::NumericList fromTimeList = m_namedFilter->getNumericFilter( FILTER_FROMTIME );
	if ( 0 < fromTimeList.size() && fromTimeList[0]!=0)
	{
		filter.StartTimePeriodDefined = true;
		CTime ct (static_cast<time_t>(fromTimeList[0]));
		filter.StartTimePeriod.Year = ct.GetYear();
		filter.StartTimePeriod.Month = ct.GetMonth();
		filter.StartTimePeriod.Day = ct.GetDay();
		filter.StartTimePeriod.Hour = ct.GetHour();
		filter.StartTimePeriod.Minute = ct.GetMinute();
		filter.StartTimePeriod.Second = ct.GetSecond();
	}else{
		// this value may not be correct when a filter applied, so will reset this before submit to eventHandlerManager
		filter.StartTimePeriodDefined = false;   
	}

	TA_Base_Core::NamedFilter::NumericList toTimeList = m_namedFilter->getNumericFilter( FILTER_TOTIME);
	if ( 0 < toTimeList.size() && toTimeList[0]!=0 )
	{
		filter.EndTimePeriodDefined = true;
		CTime ct (static_cast<time_t>(toTimeList[0]));
		filter.EndTimePeriod.Year = ct.GetYear();
		filter.EndTimePeriod.Month = ct.GetMonth();
		filter.EndTimePeriod.Day = ct.GetDay();
		filter.EndTimePeriod.Hour = ct.GetHour();
		filter.EndTimePeriod.Minute = ct.GetMinute();
		filter.EndTimePeriod.Second = ct.GetSecond();
			
	}else{
		filter.EndTimePeriodDefined = false;
	}



	// Severity Ids
	ulVector = m_namedFilter->getNumericFilter(FILTER_SEVERITY);
	if ( !ulVector.empty() )
	{
		/*filter.severityIds.assign();*/
		for( std::vector<unsigned long>::iterator itr = ulVector.begin(); itr!=ulVector.end(); itr++)
		{

			filter.severityIds.push_back( static_cast<int>(*itr));
			
		}
	}

	// Alarm Types
	ulVector = m_namedFilter->getNumericFilter(FILTER_ALARMTYPE);
	if ( !ulVector.empty() )
	{
		filter.alarmTypes = ulVector;
	}


	// Alarm ID
	stringVector = m_namedFilter->getStringFilter(FILTER_ALARMID);
	if ( !stringVector.empty() )
	{
		// clear filter.alarmId vector
		filter.AlarmIDEqualOr.clear();
		// push_back the vector of alarm id string
		for (std::vector<std::string>::iterator itr=stringVector.begin(); itr!=stringVector.end(); ++itr)
		{
			filter.AlarmIDEqualOr.push_back(*itr);
		}
	}


	// Event level.
	filter.eventLevel = m_eventLevel;

	// TM Type:
    stringVector = m_namedFilter->getStringFilter(FILTER_TMTYPE);
	if ( !stringVector.empty() )
	{
		filter.TMTypeEqualOr.push_back(stringVector[0]) ;
	}

	// Column to sort by.
	if ( COLUMN_DATETIME == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_CREATETIME;
	}
	else if ( COLUMN_SEVERITY == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_SEVERITY;
	}
	else if ( COLUMN_ASSET == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_ASSET;
	}
	else if ( COLUMN_ALARM_STATUS == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_ALARM_STATUS;
	}
	else if ( COLUMN_DESCRIPTION == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_DESCRIPTION;
	}
	else if ( COLUMN_VALUE == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_VALUE;
	}
	else if ( COLUMN_T == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_OPERATION_MODE;
	}
	else if ( COLUMN_MMS == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_MMS;
	}
	else if ( COLUMN_DSS == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_DSS;
	}
	else if ( COLUMN_AVL == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_AVL;
	}
	else if ( COLUMN_OPERATOR == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_OPERATOR;
	}
	else if ( COLUMN_COMMENT == DisplayItem::s_sortColumn )
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_COMMENT;
	}
	else
	{
		filter.sortColumn = CombinedEventAccessFactory::SORT_CREATETIME;
	}

	// Order in which the sort should be performed.
	if ( DisplayItem::s_sortAscending )
	{
		filter.sortOrder = CombinedEventAccessFactory::SORT_ASCENDING;
	}
	else
	{
		filter.sortOrder = CombinedEventAccessFactory::SORT_DESCENDING;
	}

}


void CombinedEventFilter::previousPage()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering previousPage() function. ");
	FUNCTION_ENTRY("previousPage");
	if( m_currentPage > 0 )
	{
		m_currentPage--;
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:current page %d. ", m_currentPage);
	//getListData(false);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving previousPage() function. ");
	FUNCTION_EXIT;
}


void CombinedEventFilter::sortListItems()
{
	FUNCTION_ENTRY("sortListItems");

    m_eventHanlderManager->sendListUpdate( 0 );

	int dispSize = 0;

	if ( HistoricalMode == m_eventHanlderManager->getDisplayMode() && m_eventHanlderManager->m_historicalHandler != NULL )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Will call sorting by CombinedEventFilter::sortListItems()");
		//m_eventHanlderManager->m_historicalHandler->sortAndSynchronizeList();  // has locker inside the function
		m_eventHanlderManager->m_historicalHandler->sortDispListByGUI();  // has locker inside the function
		dispSize = m_eventHanlderManager->m_historicalHandler->getDislplaySize();
	}
	else if (RealTimeMode == m_eventHanlderManager->getDisplayMode() && m_eventHanlderManager->m_realtimeHandler != NULL)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Will call sorting by CombinedEventFilter::sortListItems()");
		//m_eventHanlderManager->m_realtimeHandler->sortAndSynchronizeList();
		m_eventHanlderManager->m_realtimeHandler->sortDispListByGUI();
		dispSize = m_eventHanlderManager->m_realtimeHandler->getDislplaySize();	
	}
	
	m_eventHanlderManager->sendListViewUpdate(dispSize, getPageStatus());

	FUNCTION_EXIT;
}



void CombinedEventFilter::reverseListItems()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering reverseListItems() function. ");
	FUNCTION_ENTRY("reverseListItems");

    m_eventHanlderManager->sendListUpdate( 0 );

	int listSize = 0;

	if ( HistoricalMode == m_eventHanlderManager->getDisplayMode() && m_eventHanlderManager->m_historicalHandler != NULL){
			m_eventHanlderManager->m_historicalHandler->reverseDispListByGUI();  // has locker inside the function
			listSize = m_eventHanlderManager->m_historicalHandler->getDislplaySize();
	}
	else if( RealTimeMode == m_eventHanlderManager->getDisplayMode() && m_eventHanlderManager->m_realtimeHandler != NULL)
	{
			m_eventHanlderManager->m_realtimeHandler->reverseDispListByGUI();
			listSize = m_eventHanlderManager->m_realtimeHandler->getDislplaySize();
	}

	m_eventHanlderManager->sendListViewUpdate(listSize, getPageStatus());

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving reverseListItems() function. ");
	FUNCTION_EXIT;
}

void CombinedEventFilter::sessionChanged()
{
	TA_Base_Core::ThreadGuard storeGuard(m_sessionMapLock);
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sessionChanged");
	m_SessionMap.clear();
}



//*zhiqiang++begin
void CombinedEventFilter::CreateDefaultFilter()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "void CombinedEventFilter::InitialFilterSetupInfo [enter initial filter info] ");

	//delete try catch block because it's unnecessary any more
// 	try
//     {
        m_earliestTime = 0; //too expensive to get the earliest time, so remove it.
		// Retrieve the earliest event time.
        //m_earliestTime = CombinedEventAccessFactory::getInstance().getEarliestEventTime();
//     }
//     catch(...)
//     {
//         // Earliest time will be set to 0, so "Previous" button will never be disabled.
//         m_earliestTime = 0;
//     }

    // Create the default filter.
    std::auto_ptr<TA_Base_Core::NamedFilter> defaultFilter( new TA_Base_Core::NamedFilter() );
	defaultFilter->addNumericFilter( Filter::FILTER_TOTIME, static_cast<unsigned long> (Filter::CURRENT_TIME) );
    defaultFilter->addNumericFilter( Filter::FILTER_FROMTIME, static_cast<unsigned long> (Filter::PREVIOUS_DAY) );
	
    // Create the avalanche filter.
    std::auto_ptr<TA_Base_Core::NamedFilter> avalancheFilter( new AvalancheNamedFilter( this ) );
	avalancheFilter->addNumericFilter( Filter::FILTER_TOTIME, static_cast<unsigned long> (Filter::CURRENT_TIME));
	avalancheFilter->addNumericFilter( Filter::FILTER_FROMTIME, static_cast<unsigned long>(Filter::CURRENT_TIME) );
	
    // Set the severities for both filters.
    DatabaseAccess& data=DatabaseAccess::getInstance();
    std::vector<std::pair<unsigned long,std::string> > severities = data.getSeverities();
    std::vector<std::pair<unsigned long,std::string> >::iterator it = severities.begin();
    for( it=severities.begin(); it!=severities.end(); it++ )
    {
        defaultFilter->addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, it->first );
        avalancheFilter->addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, it->first );
    }
    defaultFilter->addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, CombinedEventFilterDlg::EVENT_ONLY_KEY );
    avalancheFilter->addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, CombinedEventFilterDlg::EVENT_ONLY_KEY );

    // Give the filters appropriate names.
    defaultFilter->setName( NAMEDFILTER_DEFAULT );
    avalancheFilter->setName( AvalancheNamedFilter::NAMEDFILTER_AVALANCHE );
	
    // Use the default filter and event level.
    m_namedFilter = std::auto_ptr<TA_Base_Core::NamedFilter>( defaultFilter->clone() );
	m_eventLevel = EventViewerConstants::DEFAULT_EVENT_LEVEL;

	// yanrong++ CL-15055
	saveAsDefaultFilter(*defaultFilter.get());
	// ++yanrong CL-15055
	
	// Initialise and obtain the event colour maps
	m_eventColoursMap = data.getEventColours();
	
	// obtain the default foreground and background colours from the map
	std::map<std::string, TA_Base_Core::IEventColourData*>::iterator colourItr = m_eventColoursMap.find("DEFAULT");
	if (colourItr != m_eventColoursMap.end() )
	{
		TA_Base_Core::IEventColourData* eventColData = colourItr->second;
		m_foregroundColour = eventColData->getEventColourKey(TA_Base_Core::IEventColourData::OPEN_ACKED_FG1);
		m_backgroundColour = eventColData->getEventColourKey(TA_Base_Core::IEventColourData::OPEN_ACKED_BG1);
	}
    // Register the static filters with the access factory.
    TA_Base_Core::NamedFilterAccessFactory::getInstance().registerStaticNamedFilter( defaultFilter );
    TA_Base_Core::NamedFilterAccessFactory::getInstance().registerStaticNamedFilter( avalancheFilter );
	
	// Load the authentication agent library
    try
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log: CombinedEventFilter::CreateDefaultFilter new AuthenticationLibrary");
		m_authenticationLib = new TA_Base_Bus::AuthenticationLibrary(true);
    }
    catch(const TA_Base_Core::DataException& )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException","Cannot create the authentication library. This is because the authentication agent configuration has not yet been set up so we go straight to the database login instead");
    }
    catch (const TA_Base_Core::DatabaseException&)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException","Cannot create the authentication library. This is because the authentication agent configuration has not yet been set up so we go straight to the database login instead");
    }
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log: CombinedEventFilter::CreateDefaultFilter come out");
}
//*/ //zhiqiang++end

	COLORREF CombinedEventFilter::getRowColour( const unsigned long row )
	{
		// Make sure the row isn't out of range.
		COLORREF foreground = m_foregroundColour;
		COLORREF background = m_backgroundColour;
		if(m_eventHanlderManager->getDisplayMode() == RealTimeMode){

			if ( row < (unsigned long)m_eventHanlderManager->m_realtimeHandler->getDislplaySize() )
			{

				// Colour decision should be based upon the existance of an alarm id.
				if ( 0 < m_eventHanlderManager->m_realtimeHandler->getColumnData(row,CombinedEventFilter::HIDDEN_COLUMN_ALARMID).size() )
				{
					determineRowColours(row, foreground, background);
				}

			}
		}
		else
		{
			if ( row < (unsigned long)m_eventHanlderManager->m_historicalHandler->getDislplaySize() )
			{

				// Colour decision should be based upon the existance of an alarm id.
				if ( 0 < m_eventHanlderManager->m_historicalHandler->getColumnData(row,CombinedEventFilter::HIDDEN_COLUMN_ALARMID).size() )
				{
					determineRowColours(row, foreground, background);
				}
			}
		}

		return foreground;
	}

void CombinedEventFilter::getRowColour( const unsigned long row, COLORREF& foreground, COLORREF& background )
	{
		determineRowColours(row, foreground, background);

		if (foreground < 0) foreground = getRowColour(row);
		if (background < 0) background = getBackgroundColour();
	}

	void CombinedEventFilter::determineRowColoursByHandler( EventHandler* eventHandler, const unsigned long row,unsigned long& foregroundColour, unsigned long& backgroundColour )
	{
		// Make sure the row isn't out of range.
		if ( row < (unsigned long)eventHandler->getDislplaySize() )
		{
			// Only proceed if the it is an alarm related event
			if ( 0 < eventHandler->getColumnData(row,CombinedEventFilter::HIDDEN_COLUMN_ALARMID).size() )
			{
				// EventColourType variables
				TA_Base_Core::IEventColourData::EEventColourType foregroundColourType;
				TA_Base_Core::IEventColourData::EEventColourType backgroundColourType;

				// Determine the event's is acked and closed status
				bool isAcked = false;
				bool isClosed = false;

				if ( 0 < eventHandler->getColumnData(row,CombinedEventFilter::HIDDEN_COLUMN_ACKED).size() ) isAcked = true;
				
				if ( 0 ==  atoi(eventHandler->getColumnData(row,CombinedEventFilter::HIDDEN_COLUMN_STATUS).c_str() ) ) isClosed = true;

				// Now determine the colour types based on the 3 parameter-combinations
				if ( !isAcked && !isClosed )
				{
					foregroundColourType = TA_Base_Core::IEventColourData::OPEN_UNACKED_FG1;
					backgroundColourType = TA_Base_Core::IEventColourData::OPEN_UNACKED_BG1;
				}
				else if ( !isAcked && isClosed )	// CL17270++
				{
					foregroundColourType = TA_Base_Core::IEventColourData::CLOSED_UNACKED_FG1;
					backgroundColourType = TA_Base_Core::IEventColourData::CLOSED_UNACKED_BG1;
				}
				else if ( isAcked && !isClosed )
				{
					foregroundColourType = TA_Base_Core::IEventColourData::OPEN_ACKED_FG1;
					backgroundColourType = TA_Base_Core::IEventColourData::OPEN_ACKED_BG1;
				}
				else if ( isAcked && isClosed )
				{
					foregroundColourType = TA_Base_Core::IEventColourData::CLOSED_ACKED_FG1;
					backgroundColourType = TA_Base_Core::IEventColourData::CLOSED_ACKED_BG1;
				}

				//This else will never enter:
// 				else // set to default and return
// 				{
// 					foregroundColour = m_foregroundColour;
// 					backgroundColour = m_backgroundColour;
// 					return;
// 				}

				// Now determine the colour reference				
				foregroundColour = getColourReference(row, foregroundColourType);
				backgroundColour = getColourReference(row, backgroundColourType);
			}
			// return the default colours
			else
			{
				foregroundColour = m_foregroundColour;
				backgroundColour = m_backgroundColour;
			}

		}
	}

	void CombinedEventFilter::determineRowColours( const unsigned long row, unsigned long& foregroundColour, unsigned long& backgroundColour )
	{
		FUNCTION_ENTRY("determineRowColours");

		 if(m_eventHanlderManager->getDisplayMode() == HistoricalMode)
		 {
			 determineRowColoursByHandler( m_eventHanlderManager->m_historicalHandler,row,foregroundColour,backgroundColour);
		 }
		 else
		 {
			determineRowColoursByHandler( m_eventHanlderManager->m_realtimeHandler,row,foregroundColour,backgroundColour);
		 }

		FUNCTION_EXIT;
	}

	// make sure the caller has used a locker
	const COLORREF CombinedEventFilter::getColourReference(const unsigned long row, TA_Base_Core::IEventColourData::EEventColourType colourType)
	{
		std::string severityName;
		FUNCTION_ENTRY("getColourReference");
		{
			if(m_eventHanlderManager->getDisplayMode() == RealTimeMode)
			{
				severityName = m_eventHanlderManager->m_realtimeHandler->getColumnData(row,CombinedEventFilter::COLUMN_SEVERITY);
			}
			else
			{
				severityName = m_eventHanlderManager->m_historicalHandler->getColumnData(row,CombinedEventFilter::COLUMN_SEVERITY);
			}
			
		}

		std::map<std::string, TA_Base_Core::IEventColourData*>::iterator colourItr = m_eventColoursMap.find(severityName);
		if (colourItr != m_eventColoursMap.end() )
		{
			TA_Base_Core::IEventColourData* eventColData = colourItr->second;

			FUNCTION_EXIT;
			return eventColData->getEventColourKey(colourType);
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Can't determine row: %li colours. Reason: Can't find severity name in map. Severity name is %s", row, severityName.c_str());

		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Getting the default colours");

		FUNCTION_EXIT;
		//return getRowColour(row); //This causes infinite loop, which stops the event viewer
		return getRowColourDefault(colourType); //Use other getRowColour function to avoid infinite loop
	}

	void CombinedEventFilter::sort(const std::string& columnName,const bool sortAscending)
{
	// Sorting is now performed in the SQL query, however we still need to set this
	// to correctly insert new events when refreshing.
	DisplayItem::setSortInfo(columnName,sortAscending);

	// Move back to the first page.
	m_currentPage = 0;


	//if( !TA_Base_Core::RunParams::getInstance().isSet(EventConstants::RPARAM_NO_VISIBILITY_RULE.c_str()) )
	{
		// wenguang++ TD14188
		sortListItems();
		LPARAM pageStatus = 0;

		if( m_eventHanlderManager->m_historicalHandler == NULL || m_eventHanlderManager->m_realtimeHandler==NULL)
		{
			return;
		}
		int listSize = (m_eventHanlderManager->getDisplayMode() == HistoricalMode) ? 
			m_eventHanlderManager->m_historicalHandler->getDislplaySize(): m_eventHanlderManager->m_realtimeHandler->getDislplaySize();
		if ( listSize >= (int)EventHandlerManager::s_maxEvents )
		{
			// If we retrieved a full page of events assume we can move down.
			pageStatus |= MASK_PAGEDOWN;
		}
		// Send the message to make sure page navigation buttons have the correct status.
		PostMessage( getHWndDlg(), WM_USER_LIST_UPDATE, listSize, pageStatus );
		PostMessage( getHWndDlg(), WM_USER_LISTVIEW_UPDATE, listSize, pageStatus );

		// ++wenguang TD14188
	}
}

void CombinedEventFilter::reverse(const std::string& columnName,const bool sortAscending)
{
	DisplayItem::setSortInfo(columnName,sortAscending);
	// Move back to the first page.
	m_currentPage = 0;

	reverseListItems();

	LPARAM pageStatus = 0;
	if(m_eventHanlderManager->m_historicalHandler == NULL || m_eventHanlderManager->m_realtimeHandler == NULL)
	{
		return;
	}

	int listSize = (m_eventHanlderManager->getDisplayMode() == HistoricalMode) ? 
		m_eventHanlderManager->m_historicalHandler->getDislplaySize(): m_eventHanlderManager->m_realtimeHandler->getDislplaySize();

	if ( listSize >= (int) EventHandlerManager::s_maxEvents )
	{
		// If we retrieved a full page of events assume we can move down.
		pageStatus |= MASK_PAGEDOWN;
	}
	// Send the message to make sure page navigation buttons have the correct status.
	PostMessage( AfxGetMainWnd()->GetSafeHwnd(), WM_USER_LIST_UPDATE, listSize, pageStatus );
	PostMessage( getHWndDlg(), WM_USER_LISTVIEW_UPDATE, listSize, pageStatus );


}

void CombinedEventFilter::setSelectedItemIDs(std::vector<unsigned long>& selectedItems ) 
{
	m_selectedEventIDs.clear();
	m_selectedItems.clear();

	string eventID="";
	if(m_eventHanlderManager->getDisplayMode() ==RealTimeMode)
	{
		for(int i=0; i<selectedItems.size(); i++)
		{
			eventID = m_eventHanlderManager->m_realtimeHandler->getColumnData(selectedItems[i], HIDDEN_COLUMN_EVENTID);
			m_selectedEventIDs.insert(eventID);
		}
	}
	else
	{
		for(int i=0; i<selectedItems.size(); i++)
		{
			eventID = m_eventHanlderManager->m_historicalHandler->getColumnData(selectedItems[i], HIDDEN_COLUMN_EVENTID);
			m_selectedEventIDs.insert(eventID);
		}
	}
	m_selectedItems = selectedItems;
}

bool CombinedEventFilter::isRowSeleted(unsigned long index)
{
	if(index==1)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Going to check whether record 2 is selected.");
	}
	string eventID="";
	if(m_eventHanlderManager->getDisplayMode() ==RealTimeMode)
	{
		eventID = m_eventHanlderManager->m_realtimeHandler->getColumnData(index, HIDDEN_COLUMN_EVENTID);
	}
	else
	{
		eventID = m_eventHanlderManager->m_historicalHandler->getColumnData(index, HIDDEN_COLUMN_EVENTID);
	}
	if( m_selectedEventIDs.find(eventID)!= m_selectedEventIDs.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "This record is seleceted. Index=%d, eventID=%s", index, eventID.c_str());
		return true;
	}
	else return false;

}

    void CombinedEventFilter::nextPage()
    {
		FUNCTION_ENTRY("nextPage");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering nextPage() function. ");

		m_currentPage++;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:current page %d. ", m_currentPage);
		//getListData(false);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving nextPage() function. ");
		FUNCTION_EXIT;
    }

		const std::string CombinedEventFilter::getItemData(const unsigned long index,const std::string& columnName,const bool summarise/*=true*/)
	{
		// This array index operation has a small window where it can fail - which is the time between the data vector
		// being updated, but before the "SetItemCount" method can be called on the list. For this time (possilby only two-to
		// three calls of this method, and only when the vector is being shrunk, while the end of the list is still visible)
		// there needs to be a backup solution - so instead of crashing the application, we just return an empty string. It 
		// does nothing, and will leave an empty cell (if an update ever has time to complete its paint - which is unlikely),
		// but it is the best solution.

		LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: Getting Item No. %d.", index);

		if(m_eventHanlderManager->getDisplayMode() ==RealTimeMode){

			if (index >= (unsigned long)m_eventHanlderManager->m_realtimeHandler->getDislplaySize())
			{
				return EventViewerConstants::EMPTY_STRING.c_str();
			}
			try
			{
				if ( summarise )
				{
					return  m_eventHanlderManager->m_realtimeHandler->getColumnData(index,columnName); 
				}
				else
				{
					return  m_eventHanlderManager->m_realtimeHandler->getCompleteColumnData(index,columnName); 
				}
			}
			catch(...)
			{
				return EventViewerConstants::EMPTY_STRING.c_str();
			}

		}
		else
		{
			if (index >= m_eventHanlderManager->m_historicalHandler->getDislplaySize())
			{
				return EventViewerConstants::EMPTY_STRING.c_str();
			}
			try
			{
				if ( summarise )
				{
					return  m_eventHanlderManager->m_historicalHandler->getColumnData(index,columnName); 
				}
				else
				{
					return  m_eventHanlderManager->m_historicalHandler->getCompleteColumnData(index,columnName); 
				}
			}
			catch(...)
			{
				return EventViewerConstants::EMPTY_STRING.c_str();
			}

		}

	}

	void CombinedEventFilter::startNewTask(FilterCriteria searchfilter, EventDisplayMode mode)
	{
		//save current FilterCriteria.
		setFilterCriteria(searchfilter);

		//if the task is just the same as last time. Then do nothing.
		if(m_eventHanlderManager->isStartNewTask(searchfilter,mode))
		{
			m_selectedItems.clear();
			m_selectedEventIDs.clear();

			m_eventHanlderManager->statusBarUpdateWhenStartQuery();

			if( !m_eventHanlderManager->m_ATSEnabled && !m_eventHanlderManager->m_ISCSEnabled)
			{
				return;
			}

			//clear page:
			m_currentPage = 0;
			clearPageTimeMap();
			insertPageTimeMap(m_currentPage, EventViewerConstants::EMPTY_STRING);
  			LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", m_currentPage);
			m_eventHanlderManager->startNewTask(searchfilter,mode);



		}


	}

	void CombinedEventFilter::initEventsCount(int maxEvents, int refreshTime , int refreshTimeAgent)
	{
		m_eventHanlderManager->setMaxEvents(maxEvents);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set Max Events Number: %d ",maxEvents);
		m_eventHanlderManager->initialize_ATS_ISCS_MaxCount();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set ISCS max events number: %d, ATS max events number: %d ", 
		EventHandlerManager::getMaxISCSEvents(), EventHandlerManager::getMaxATSEvents() );
		m_eventHanlderManager->setRefreshRateDB(refreshTime);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set ISCS event refresh rate: %d", refreshTime);
		m_eventHanlderManager->setRefreshRateAgent(refreshTimeAgent);

	}


	void CombinedEventFilter::setSortingPriority()
	{
		m_eventHanlderManager->setIsISCSSortingPriority();
	}


	void CombinedEventFilter::initialHandler()
	{
		m_eventHanlderManager->initialHandler();
	}

	bool CombinedEventFilter::currentListWasCutted()
	{
		bool res = false;
		if( m_eventHanlderManager->getDisplayMode() == RealTimeMode)
		{
			return m_eventHanlderManager->m_realtimeHandler->currentListWasCutted();
			
		}
		else 
		{
			return m_eventHanlderManager->m_historicalHandler->currentListWasCutted();
		}
	}



	void CombinedEventFilter::reloadEvents()
	{
		//m_eventHanlderManager->reloadEvents();
		FilterCriteria old_filterCriteria =  getFilterCriteria();
		FilterCriteria new_filterCriteria ;
		populateFilterCriteria(new_filterCriteria);
		new_filterCriteria.StartTimePeriodDefined = old_filterCriteria.StartTimePeriodDefined;
		new_filterCriteria.EndTimePeriodDefined = old_filterCriteria.EndTimePeriodDefined;
		new_filterCriteria.StartTimePeriod = old_filterCriteria.StartTimePeriod;
		new_filterCriteria.EndTimePeriod = old_filterCriteria.EndTimePeriod;
		if( m_eventHanlderManager->getDisplayMode() == TA_Base_App::RealTimeMode)
		{
			startNewTask(new_filterCriteria,TA_Base_App::RealTimeMode);

		}else
		{
			startNewTask(new_filterCriteria,TA_Base_App::HistoricalMode);
		}
	}


	time_t CombinedEventFilter::getEarliestTimeFromList() 
	{
		if( m_eventHanlderManager->getDisplayMode() == RealTimeMode)
		{
			return m_eventHanlderManager->m_realtimeHandler->getEarliestTimeFromList();

		}
		else 
		{
			return m_eventHanlderManager->m_historicalHandler->getEarliestTimeFromList();
		}
	}

	time_t CombinedEventFilter::getLatestTimeFromList() 
	{
		if(  m_eventHanlderManager->getDisplayMode()  == RealTimeMode)
		{
			return m_eventHanlderManager->m_realtimeHandler->getLatestTimeFromList();

		}
		else 
		{
			return m_eventHanlderManager->m_historicalHandler->getLatestTimeFromList();
		}
	}

	void CombinedEventFilter::setBothPkeyRightBoundary(FilterCriteria & filterCriteria) 
	{
		if( m_eventHanlderManager->getDisplayMode()  == RealTimeMode)
		{
			m_eventHanlderManager->m_realtimeHandler->setBothPkeyRightBoundary(filterCriteria);
		}
		else
		{
			m_eventHanlderManager->m_historicalHandler->setBothPkeyRightBoundary(filterCriteria);
		}
	}

	void CombinedEventFilter::setBothPkeyLeftBoundary(FilterCriteria & filterCriteria) 
	{
		if( m_eventHanlderManager->getDisplayMode()  == RealTimeMode)
		{
			m_eventHanlderManager->m_realtimeHandler->setBothPkeyLeftBoundary(filterCriteria);
		}
		else
		{
			m_eventHanlderManager->m_historicalHandler->setBothPkeyLeftBoundary(filterCriteria);
		}
	}

		void CombinedEventFilter::setFilterCriteria(FilterCriteria filterCriteria)
	{
		m_filterCriteria = filterCriteria;
	}

	FilterCriteria CombinedEventFilter::getFilterCriteria()
	{
		return m_filterCriteria;
	}



