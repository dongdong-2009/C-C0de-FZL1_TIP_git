/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByLocation.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByLocation.cpp
//  Implementation of the Class AlarmFilterByDate
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByLocation.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterByLocation::AlarmFilterByLocation(EComparitor compareOp,unsigned long compareLocation) 
                  : IAlarmFilter(compareOp),
					m_compareLocation(compareLocation)
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_CONTAINS,"Invalid comparison for location filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for location filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for location filter.");
	FUNCTION_EXIT;
}



AlarmFilterByLocation::~AlarmFilterByLocation()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByLocation::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator==");
	// See if it is the same filter type
	const AlarmFilterByLocation* locationFilter = dynamic_cast<const AlarmFilterByLocation*>(&filter);
	if (locationFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==locationFilter->m_compareOp) &&
		    (m_compareLocation==locationFilter->m_compareLocation))
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
bool AlarmFilterByLocation::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return alarm.locationKey==m_compareLocation;
		case COMPARE_NOT_EQUAL:			return alarm.locationKey!=m_compareLocation;
		case COMPARE_LESS_THAN:			return alarm.locationKey<m_compareLocation;
		case COMPARE_LESS_THAN_EQUAL:	return alarm.locationKey<=m_compareLocation;
		case COMPARE_GREATER_THAN:		return alarm.locationKey>m_compareLocation;
		case COMPARE_GREATER_THAN_EQUAL:return alarm.locationKey>=m_compareLocation;
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
	}

	FUNCTION_EXIT;
    return false;
}


/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterByLocation::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->locationKey==m_compareLocation;
	case COMPARE_NOT_EQUAL:			return alarm->locationKey!=m_compareLocation;
	case COMPARE_LESS_THAN:			return alarm->locationKey<m_compareLocation;
	case COMPARE_LESS_THAN_EQUAL:	return alarm->locationKey<=m_compareLocation;
	case COMPARE_GREATER_THAN:		return alarm->locationKey>m_compareLocation;
	case COMPARE_GREATER_THAN_EQUAL:return alarm->locationKey>=m_compareLocation;
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
bool AlarmFilterByLocation::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->locationKey==m_compareLocation;
	case COMPARE_NOT_EQUAL:			return alarm->locationKey!=m_compareLocation;
	case COMPARE_LESS_THAN:			return alarm->locationKey<m_compareLocation;
	case COMPARE_LESS_THAN_EQUAL:	return alarm->locationKey<=m_compareLocation;
	case COMPARE_GREATER_THAN:		return alarm->locationKey>m_compareLocation;
	case COMPARE_GREATER_THAN_EQUAL:return alarm->locationKey>=m_compareLocation;
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}