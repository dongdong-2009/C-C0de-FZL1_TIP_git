/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterSet.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class manages a collection of alarm filters to be applied to an alarm to
  * determine if it should be included.
  *
  * Note: The filter set will hold separate filter lists for each type of filter.  This is 
  * required to apply the rule that filters of the same type are OR'd in the comparison,
  * whilst across filter types we must AND the results.  Note that for dates we must now 
  * have 
  *
  * <description>
  *
  */

///////////////////////////////////////////////////////////
//  AlarmFilterSet.cpp
//  Implementation of the Class AlarmFilterSet
//  Created on:      24-Jan-2004 04:12:48 PM
///////////////////////////////////////////////////////////


#include "AlarmFilterSet.h"
#include "typeinfo.h"
#include "core/utilities/src/debugutil.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_Base_Bus;

AlarmFilterSet::AlarmFilterSet() : m_filterCount(0)
{
	FUNCTION_ENTRY("Constructor");
	FUNCTION_EXIT;
}

AlarmFilterSet::~AlarmFilterSet()
{
	FUNCTION_ENTRY("Destructor");

	// Release all filters held in the lists
	std::map<std::string, FilterList*>::iterator  mapIT = m_filters.begin();
	while (mapIT!=m_filters.end())
	{
		std::vector<IAlarmFilter*>::iterator it = mapIT->second->begin();
		while (it!=mapIT->second->end())
		{
			delete (*it);
			it++;
		}
		mapIT->second->clear();

		delete (mapIT->second);
		mapIT->second = NULL;
		mapIT++;  // next filter type
	}
	// Clear all items from map (and associated vectors)
	m_filters.clear();

	FUNCTION_EXIT;
}

bool AlarmFilterSet::operator == (const AlarmFilterSet& filterSet) const
{
	FUNCTION_ENTRY("Operator==");

	// Check that the total number of filters is the same
	if (m_filterCount!=filterSet.m_filterCount)
	{
		return false;
	}

	// Check each filter list
	FilterCollection::const_iterator  mapIT = filterSet.m_filters.begin();
	while (mapIT!=filterSet.m_filters.end())
	{
		// Check each one of the filters in this filter set exists in the passed filter set
		// Order is not important for filtering
		for (unsigned int iLoop=0;iLoop<mapIT->second->size();iLoop++)
		{
			// check each filter in the passed set for a match
			if (!isFilterPresent(*(*mapIT->second)[iLoop])) 
			{
				return false;
			}
		}

		mapIT++;
	}

	// check the otherway around
// 	FilterCollection::const_iterator  mapIT_this = m_filters.begin();
// 
// 	while (mapIT_this != m_filters.end())
// 	{
// 		// Check each one of the filters in this filter set exists in the passed filter set
// 		// Order is not important for filtering
// 		for (unsigned int iLoop=0;iLoop<mapIT_this->second->size();iLoop++)
// 		{
// 			// check each filter in the passed set for a match
// 			if (!filterSet.isFilterPresent(*(*mapIT_this->second)[iLoop])) 
// 			{
// 				return false;
// 			}
// 		}
// 
// 		mapIT_this++;
// 	}

	FUNCTION_EXIT;
	return true;
}



void AlarmFilterSet::addFilter(IAlarmFilter* filter)
{
	FUNCTION_ENTRY("AddFilter");
	if (filter == NULL)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "filter == NULL");
		return ;
	}
	// Get the filter type
	std::string filterType = typeid(*filter).name();

	// See if we already have this type of filter
	FilterCollection::iterator  mapIT = m_filters.find(filterType);
	if (mapIT==m_filters.end())
	{
		// New filter type - will be cleaned up by destructor of this class
		m_filters[filterType] = new FilterList;
		mapIT = m_filters.find(filterType);
		TA_ASSERT(mapIT!=m_filters.end(),"New filter list entry not found.");
	}
	
	// Add the filter to the list
	mapIT->second->push_back(filter);

	m_filterCount++;
	FUNCTION_EXIT;
}


bool AlarmFilterSet::isFilterPresent(const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("isFilterPresent");
	
	if (NULL == &filter) return false;

	// Get the filter type
	std::string filterType = typeid(filter).name();

	// See if we already have this type of filter
	FilterCollection::const_iterator  mapIT = m_filters.find(filterType);
	if (mapIT!=m_filters.end())
	{
		// See if filter is in the list for this filter type
		std::vector<IAlarmFilter*>::const_iterator it = mapIT->second->begin();
		while (it!=mapIT->second->end())
		{
			if (filter==(**it))
			{
				return true;
			}
			it++;
		}
	}

	FUNCTION_EXIT;
	return false;
}


/* This filter set will check the alarm against each filter in the set.  Duplicate filter types
 * will be OR'd and different filters AND'd.  For example; filter by date1, filter by location, filter by date 2
 * will be (filter by date1 OR filter by date2) AND filter by location.
 *
 */
bool AlarmFilterSet::passesFilters(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("passesFilters");
	if (&alarm == NULL)return false;

	// If no filters then all alarms are ok
	if (m_filterCount<1) 
	{	
		return true;
	}

	// Process each filter type (ANDed)
	FilterCollection::const_iterator  mapIT = m_filters.begin();
	while (mapIT!=m_filters.end())
	{
		// Process each filter in the filter type (ORed)
		bool bMatchFound = false;
		std::vector<IAlarmFilter*>::const_iterator it = mapIT->second->begin();
		while (it!=mapIT->second->end())
		{
			if ((*it)->includeAlarm(alarm))
			{
				bMatchFound = true;
				break;
			}
			it++;
		}

		// If no match is found for any filter type then 
		// exclude the alarm.
		if (!bMatchFound)
		{
			return false;
		}

		mapIT++;
	}

	FUNCTION_EXIT;
	return true;
}

bool AlarmFilterSet::passesFilters(const TA_Base_Core::AlarmItem* alarm) 
{
	FUNCTION_ENTRY("passesFilters");
	if (alarm == NULL)return false;
 	// If no filters then all alarms are ok
 	if (m_filterCount<1) 
 	{	
 		return true;
 	}
 	
 	// Process each filter type (ANDed)
 	FilterCollection::const_iterator  mapIT = m_filters.begin();
 	while (mapIT!=m_filters.end())
 	{
 		// Process each filter in the filter type (ORed)
 		bool bMatchFound = false;
 		std::vector<IAlarmFilter*>::const_iterator it = mapIT->second->begin();
 		while (it!=mapIT->second->end())
		{
			if ((*it)->includeAlarm(alarm))
			{
				bMatchFound = true;
				break;
			}
			it++;
		}
 		
 		// If no match is found for any filter type then 
		// exclude the alarm.
 		if (!bMatchFound)
 		{
 			return false;
 		}
 		
 		mapIT++;
	}
 	
	FUNCTION_EXIT;
	return true;
}

//++Noel
bool AlarmFilterSet::passesFilters(const TA_Base_Bus::AlarmDataItem* alarm) 
{
	FUNCTION_ENTRY("passesFilters");
	if (alarm == NULL) return false;
	
	// If no filters then all alarms are ok
	if (m_filterCount<1) 
	{	
		return true;
	}
	
	// Process each filter type (ANDed)
	FilterCollection::const_iterator  mapIT = m_filters.begin();
	while (mapIT!=m_filters.end())
	{
		// Process each filter in the filter type (ORed)
		bool bMatchFound = false;
		std::vector<IAlarmFilter*>::const_iterator it = mapIT->second->begin();
		while (it!=mapIT->second->end())
		{
			if ((*it)->includeAlarm(alarm))
			{
				bMatchFound = true;
				break;
			}
			it++;
		}
		
		// If no match is found for any filter type then 
		// exclude the alarm.
		if (!bMatchFound)
		{
			return false;
		}
		
		mapIT++;
	}
	
	FUNCTION_EXIT;
	return true;
}

// yanrong++: reduce access to rights agent.
void AlarmFilterSet::resetFilters()
{
	FUNCTION_ENTRY("resetFilters");
	// If no filters then all alarms are ok
	if (m_filterCount<1) 
	{	
		return;
	}
	
	// Process each filter type
	FilterCollection::const_iterator  mapIT = m_filters.begin();
	while (mapIT!=m_filters.end())
	{
		// Process each filter in the filter type
		std::vector<IAlarmFilter*>::const_iterator it = mapIT->second->begin();
		while (it!=mapIT->second->end())
		{
			(*it)->reset();
			it++;
		}

		mapIT++;
	}

	FUNCTION_EXIT;
}
// ++yanrong.
