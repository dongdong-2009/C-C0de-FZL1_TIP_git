/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSWorkerHelper.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/10/19 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ISCSWorkerHelper is ...
  *
  */

#include "ISCSWorkerHelper.h"
#include "ConstantItems.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_App
{
	ISCSWorkerHelper::ISCSWorkerHelper(void)
	{
	}

	ISCSWorkerHelper::~ISCSWorkerHelper(void)
	{
	}

	bool ISCSWorkerHelper::iscsWillNeverMeetFilter(FilterCriteria& filter)
	{
		// for some situation that will load nothing:
		// e.g. User selected ATS system but no ISCS system, or user selected ATS subsystem but no ISCS subystem
		if ( (filter.SystemNameEqualOr.size()!=0 || filter.SubsystemNameEqualOr.size() != 0 ) &&
			filter.appSubsystemIds.size() ==0 &&  filter.physSubsystemIds.size()==0 )
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

	void ISCSWorkerHelper::populatePublicFilter(FilterCriteria filterCriteria, 
		TA_Base_Core::CombinedEventAccessFactory::EventFilter& filter )
	{
		FUNCTION_ENTRY("ISCSWokerHelper::populatePublicFilter");
		// 
		// Define two vectors for retrieveing filter data
		std::list<std::string> stringList;
		std::vector<unsigned long> ulVector;

		filter.dutyEnabled = true;  

		// Application subsystem Ids
		ulVector = filterCriteria.appSubsystemIds;
		if ( !ulVector.empty() )
		{
			filter.appSubsystemIds = ulVector;
		}

		// Physical subsystem Ids.
		ulVector = filterCriteria.physSubsystemIds;
		if ( !ulVector.empty() )
		{
			filter.physSubsystemIds = ulVector;
		}

		// Location Ids
		ulVector = filterCriteria.locationIds;
		if ( !ulVector.empty() )
		{
			filter.locationIds = ulVector;
		}

		// Operator Ids
		ulVector = filterCriteria.operatorIds;
		if ( !ulVector.empty() )
		{
			filter.operatorIds = ulVector;
		}

		// System IDs
		ulVector = filterCriteria.systemIds;
		if ( !ulVector.empty() )
		{
			filter.systemIds = ulVector;
		}

		// Event Types
		ulVector = filterCriteria.eventTypeIds;
		if ( !ulVector.empty() )
		{
			filter.eventTypeIds = ulVector;
		}

		// profiles
		ulVector = filterCriteria.profiles;
		if ( !ulVector.empty() )
		{
			filter.profiles = ulVector;
		}

		// Asset Name
		stringList = filterCriteria.Asset;
		if ( !stringList.empty() )
		{
			filter.assetName = "%" + stringList.front() + "%";
		}

		// TM type:
		stringList = filterCriteria.TMTypeEqualOr;
		if( !stringList.empty())
		{
			filter.tmType = stringList.front();
		}

		// Start with description
		stringList = filterCriteria.Description;
		if ( !stringList.empty() )
		{
			filter.description = "%" + stringList.front() + "%";
		}

		// Set the time range.
		if(filterCriteria.StartTimePeriodDefined == true)
		{
			filter.fromTime = filterCriteria.StartTimePeriod.getTime_t();
		}else
		{
			filter.fromTime = 0;
		}

		if( filterCriteria.EndTimePeriodDefined == true)
		{
			filter.toTime = filterCriteria.EndTimePeriod.getTime_t();
		}else
		{
			filter.toTime = 0;
		}
		

		// Don't show non-alarm events by default.
		filter.showNonAlarmEvents = false;

		// Severity Ids
		std::vector<int> intVector = filterCriteria.severityIds;
		if ( !intVector.empty() )
		{

			// Search through and see if non-alarm events are to be displayed.
			std::vector<int>::iterator it = intVector.begin();
			for ( ; it!=intVector.end(); it++ )
			{
				if ( *it == (EventViewerConstants::EVENT_ONLY_KEY) )
				{
					// Remove it from the list of severities and continue.
					intVector.erase( it );
					filter.showNonAlarmEvents = true;
					break;
				}
			}

			// Set severity filters.
			for(int i =0; i< (int)intVector.size(); i++)
			{
				filter.severityIds.push_back(intVector[i]);
			}

		}

		// Alarm Types
		ulVector = filterCriteria.alarmTypes;
		if ( !ulVector.empty() )
		{
			filter.alarmTypes = ulVector;
		}

		// Alarm ID
		stringList = filterCriteria.AlarmIDEqualOr;
		if ( !stringList.empty() )
		{
			// clear filter.alarmId vector
			filter.alarmId.clear();
			// push_back the vector of alarm id string
			for (std::list<std::string>::iterator itr=stringList.begin(); itr!=stringList.end(); ++itr)
			{
				filter.alarmId.push_back(*itr);
			}
		}


		// Event level.
		filter.eventLevel = filterCriteria.eventLevel;

		// Column to sort by.
		filter.sortColumn = filterCriteria.sortColumn;
		filter.sortOrder = filterCriteria.sortOrder;

	}

};

