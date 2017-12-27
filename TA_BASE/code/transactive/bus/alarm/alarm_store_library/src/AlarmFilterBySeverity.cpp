/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterBySeverity.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterBySubSystem.cpp
//  Implementation of the Class AlarmFilterByDate
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterBySeverity.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterBySeverity::AlarmFilterBySeverity(EComparitor compareOp,unsigned long compareSeverity) 
                  : IAlarmFilter(compareOp),
					m_compareSeverity(compareSeverity)
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_CONTAINS,"Invalid comparison for location filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for location filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for location filter.");
	FUNCTION_EXIT;
}



AlarmFilterBySeverity::~AlarmFilterBySeverity()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterBySeverity::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterBySeverity* severityFilter = dynamic_cast<const AlarmFilterBySeverity*>(&filter);
	if (severityFilter != NULL)
	{
		// Check parameters are the same
		if ((m_compareOp == severityFilter->m_compareOp) &&
		    (m_compareSeverity == severityFilter->m_compareSeverity))
		{
			return true;
		}
	}
	FUNCTION_EXIT;
	return false;
}

/**
 * Check if the specified alarm meets the filter requirements
 * @param alarm
 * 
 */
bool AlarmFilterBySeverity::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return alarm.alarmSeverity == m_compareSeverity;
		case COMPARE_NOT_EQUAL:			return alarm.alarmSeverity != m_compareSeverity;
		case COMPARE_LESS_THAN:			return alarm.alarmSeverity < m_compareSeverity;
		case COMPARE_LESS_THAN_EQUAL:	return alarm.alarmSeverity <= m_compareSeverity;
		case COMPARE_GREATER_THAN:		return alarm.alarmSeverity > m_compareSeverity;
		case COMPARE_GREATER_THAN_EQUAL:return alarm.alarmSeverity >= m_compareSeverity;
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
	}

	FUNCTION_EXIT;
    return false;
}


/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterBySeverity::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->alarmSeverity == m_compareSeverity;
	case COMPARE_NOT_EQUAL:			return alarm->alarmSeverity != m_compareSeverity;
	case COMPARE_LESS_THAN:			return alarm->alarmSeverity < m_compareSeverity;
	case COMPARE_LESS_THAN_EQUAL:	return alarm->alarmSeverity <= m_compareSeverity;
	case COMPARE_GREATER_THAN:		return alarm->alarmSeverity > m_compareSeverity;
	case COMPARE_GREATER_THAN_EQUAL:return alarm->alarmSeverity >= m_compareSeverity;
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}
/*-----------------------------------------------------------------------------------------------------------*/

//++Noel
bool AlarmFilterBySeverity::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->alarmSeverity == m_compareSeverity;
	case COMPARE_NOT_EQUAL:			return alarm->alarmSeverity != m_compareSeverity;
	case COMPARE_LESS_THAN:			return alarm->alarmSeverity < m_compareSeverity;
	case COMPARE_LESS_THAN_EQUAL:	return alarm->alarmSeverity <= m_compareSeverity;
	case COMPARE_GREATER_THAN:		return alarm->alarmSeverity > m_compareSeverity;
	case COMPARE_GREATER_THAN_EQUAL:return alarm->alarmSeverity >= m_compareSeverity;
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}