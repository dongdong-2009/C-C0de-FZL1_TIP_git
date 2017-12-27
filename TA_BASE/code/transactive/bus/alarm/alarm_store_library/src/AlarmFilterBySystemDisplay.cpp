/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmFilterBySystemDisplay.cpp $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  */

#include "AlarmFilterBySystemDisplay.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{

	AlarmFilterBySystemDisplay::AlarmFilterBySystemDisplay(Vec_displayFilter_T pDisplayFilter)
		: IAlarmFilter(IAlarmFilter::COMPARE_EQUAL), m_pDisplayFilter(pDisplayFilter)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}
	AlarmFilterBySystemDisplay::~AlarmFilterBySystemDisplay()
	{
		FUNCTION_ENTRY("Destructor");
		for (int idx=0; idx<m_pDisplayFilter.size(); idx++)
		{
			displayFilter* pTemp = m_pDisplayFilter[idx];
			
			delete pTemp;
			pTemp = NULL;
		}
			
		m_pDisplayFilter.clear();
		FUNCTION_EXIT;
	}

	bool AlarmFilterBySystemDisplay::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
	{
		FUNCTION_ENTRY("includeAlarm - AlarmDetailCorbaDef");
		// Filter alarm
		for (int idx=0; idx<m_pDisplayFilter.size(); idx++)
		{
			// If one condition is not met then discard alarm
			displayFilter* pFilter = m_pDisplayFilter[idx];
			if (pFilter->displayCondition == EXCLUDE)
			{	//Discard alarm if all condition is met
				if (pFilter->alarmType == alarm.messageTypeKey &&
					pFilter->subSystemKey == alarm.subsystemKey && 
					pFilter->alarmLocationKey == alarm.locationKey)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Alarm display filter Exclude AlarmID: %s", alarm.alarmID.in())
					return false;
				}
			}
			else if (pFilter->displayCondition == INCLUDE)
			{	// Discard alarm only if it matches alarmType, subSystemKey 
				// and location key does not match
				if (pFilter->alarmType == alarm.messageTypeKey &&
					pFilter->subSystemKey == alarm.subsystemKey &&
					pFilter->alarmLocationKey != alarm.locationKey)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Alarm display filter Exclude AlarmID: %s", alarm.alarmID.in())
					return false;
				}
			}
			else
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid display filter condition.");
		}

		FUNCTION_EXIT;
		return true;
	}

	bool AlarmFilterBySystemDisplay::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
	{
		FUNCTION_ENTRY("includeAlarm - AlarmItem");
		// Filter alarm
		for (int idx=0; idx<m_pDisplayFilter.size(); idx++)
		{
			// If one condition is not met then discard alarm
			displayFilter* pFilter = m_pDisplayFilter[idx];
			if (pFilter->displayCondition == EXCLUDE)
			{	//Discard alarm if all condition is met
				if (pFilter->alarmType == alarm->messageTypeKey &&
					pFilter->subSystemKey == alarm->subsystemKey && 
					pFilter->alarmLocationKey == alarm->locationKey)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Alarm display filter Exclude AlarmID: %s", alarm->alarmID)
					return false;
				}
			}
			else if (pFilter->displayCondition == INCLUDE)
			{	// Discard alarm only if it matches alarmType, subSystemKey 
				// and location key does not match
				if (pFilter->alarmType == alarm->messageTypeKey &&
					pFilter->subSystemKey == alarm->subsystemKey &&
					pFilter->alarmLocationKey != alarm->locationKey)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Alarm display filter Exclude AlarmID: %s", alarm->alarmID)
					return false;
				}
			}
			else
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid display filter condition.");
		}

		FUNCTION_EXIT;
		return true;
	}

	bool AlarmFilterBySystemDisplay::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
	{
		FUNCTION_ENTRY("includeAlarm - AlarmDataItem");
		// Filter alarm
		for (int idx=0; idx<m_pDisplayFilter.size(); idx++)
		{
			// If one condition is not met then discard alarm
			displayFilter* pFilter = m_pDisplayFilter[idx];
			if (pFilter->displayCondition == EXCLUDE)
			{	//Discard alarm if all condition is met
				if (pFilter->alarmType == alarm->messageTypeKey &&
					pFilter->subSystemKey == alarm->subsystemKey && 
					pFilter->alarmLocationKey == alarm->locationKey)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Alarm display filter Exclude AlarmID: %s", alarm->alarmID.c_str())
					return false;
				}
			}
			else if (pFilter->displayCondition == INCLUDE)
			{	// Discard alarm only if it matches alarmType, subSystemKey 
				// and location key does not match
				if (pFilter->alarmType == alarm->messageTypeKey &&
					pFilter->subSystemKey == alarm->subsystemKey &&
					pFilter->alarmLocationKey != alarm->locationKey)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Alarm display filter Exclude AlarmID: %s", alarm->alarmID.c_str())
					return false;
				}
			}
			else
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid display filter condition.");
		}

		FUNCTION_EXIT;
		return true;
	}

	// Comparison operator used to check if the filter is the same as another.
	bool AlarmFilterBySystemDisplay::operator == (const IAlarmFilter& filter) const
	{
		FUNCTION_ENTRY("Operator== ");
		// See if it is the same filter type
		const AlarmFilterBySystemDisplay* systemDisplayFilter = dynamic_cast<const AlarmFilterBySystemDisplay*>(&filter);
		if (systemDisplayFilter != NULL)
		{
			// if filter size is not same return false
			if (m_pDisplayFilter.size() != systemDisplayFilter->m_pDisplayFilter.size())
			{
				return false;
			}

			for (int outerIdx=0; outerIdx<m_pDisplayFilter.size(); outerIdx++)
			{
				bool bFound = false;
				for (int innerIdx=0; innerIdx<m_pDisplayFilter.size(); innerIdx++)
				{
					if (m_pDisplayFilter[outerIdx]->filterKey == systemDisplayFilter->m_pDisplayFilter[innerIdx]->filterKey)
					{
						bFound = true;
						break;
					}
				}

				if (!bFound)
				{
					// Filter did not match
					return false;
				}
			}
		}
		FUNCTION_EXIT;
		return true;
	}

} // End TA_Base_Bus

