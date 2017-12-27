/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmSortingSet.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmSortingSet.cpp
//  Implementation of the Class AlarmSortingSet
//  Created on:      24-Jan-2004 04:13:19 PM
///////////////////////////////////////////////////////////

#include "AlarmSortingSet.h"
#include "core/utilities/src/DebugUtil.h"


using namespace TA_Base_Bus;


AlarmSortingSet::AlarmSortingSet()
{
	FUNCTION_ENTRY("Constructor");
	FUNCTION_EXIT;
}



AlarmSortingSet::~AlarmSortingSet()
{
	FUNCTION_ENTRY("Destructor");

	// Release all the sort items
	SortList::iterator it = m_alarmSortList.begin();
	while (it!=m_alarmSortList.end())
	{
		delete (*it);
		++it;
	}
	m_alarmSortList.clear();

	FUNCTION_EXIT;
}


bool AlarmSortingSet::operator == (const AlarmSortingSet& sortSet) const
{
	FUNCTION_ENTRY("Operator==");

	if (m_alarmSortList.size()!=sortSet.m_alarmSortList.size())
	{
		FUNCTION_EXIT;
		return false;
	}

	for (unsigned int iLoop=0;iLoop<m_alarmSortList.size();++iLoop)
	{
		IAlarmSort*  thisItem  = m_alarmSortList.at(iLoop);
		IAlarmSort*  otherItem = sortSet.m_alarmSortList.at(iLoop);

		if (!((*thisItem)==(*otherItem)))
		{
			FUNCTION_EXIT;
			return false;
		}
	}
	FUNCTION_EXIT;
	return true;
}


void AlarmSortingSet::addSort(IAlarmSort*  sort)
{
	FUNCTION_ENTRY("addSort");
	if (NULL == sort)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL == sort");
		FUNCTION_EXIT;
		return ;
	}
	m_alarmSortList.push_back(sort);
	FUNCTION_EXIT;
}

bool AlarmSortingSet::sortingRequired() const
{
	return m_alarmSortList.size() > 0;
}


IAlarmSort::ECompareResult AlarmSortingSet::compareAlarms(const TA_Base_Core::AlarmDetailCorbaDef& alarm1, 
														  const TA_Base_Core::AlarmDetailCorbaDef& alarm2) const
{
	FUNCTION_ENTRY("compareAlarms");
	SortList::const_iterator it = m_alarmSortList.begin();
	while (it!=m_alarmSortList.end())
	{
		IAlarmSort::ECompareResult sortResult = (*it)->compareAlarms(alarm1,alarm2);
		if (sortResult!=IAlarmSort::ECompareResult::SORT_EQUAL)
		{
			FUNCTION_EXIT;
			return sortResult;
		}
		++it;
	}
	FUNCTION_EXIT;
	return IAlarmSort::ECompareResult::SORT_EQUAL;
}


