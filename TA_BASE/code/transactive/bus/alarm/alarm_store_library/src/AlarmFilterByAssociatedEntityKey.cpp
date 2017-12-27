/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByAssociatedEntityKey.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByAssociatedEntityKey.cpp
//  Implementation of the Class AlarmFilterByAssociatedEntityKey
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByAssociatedEntityKey.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterByAssociatedEntityKey::AlarmFilterByAssociatedEntityKey(EComparitor compareOp,unsigned long compareEntityKey) 
                  : IAlarmFilter(compareOp),
					m_compareEntityKey(compareEntityKey)
{
	FUNCTION_ENTRY("Constructor");

	TA_ASSERT(compareOp!=COMPARE_CONTAINS,"Invalid comparison for associated entity key filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for associated entity key filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for associated entity key filter.");
	TA_ASSERT(compareOp!=COMPARE_LESS_THAN,"Invalid comparison for associated entity key filter.");
	TA_ASSERT(compareOp!=COMPARE_LESS_THAN_EQUAL,"Invalid comparison for associated entity key filter.");
	TA_ASSERT(compareOp!=COMPARE_GREATER_THAN,"Invalid comparison for associated entity key filter.");
	TA_ASSERT(compareOp!=COMPARE_GREATER_THAN_EQUAL,"Invalid comparison for associated entity key filter.");

	FUNCTION_EXIT;
}



AlarmFilterByAssociatedEntityKey::~AlarmFilterByAssociatedEntityKey()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByAssociatedEntityKey::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator==");

	// See if it is the same filter type
	const AlarmFilterByAssociatedEntityKey* entityKeyFilter = dynamic_cast<const AlarmFilterByAssociatedEntityKey*>(&filter);
	if (entityKeyFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==entityKeyFilter->m_compareOp) &&
		    (m_compareEntityKey==entityKeyFilter->m_compareEntityKey))
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
bool AlarmFilterByAssociatedEntityKey::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");

	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return alarm.assocEntityKey == m_compareEntityKey;
		case COMPARE_NOT_EQUAL:			return alarm.assocEntityKey != m_compareEntityKey;
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			}
	}

	FUNCTION_EXIT;
    return false;
}

/*--------------------------------------New Alarm Store--------------------------------------------------*/
bool AlarmFilterByAssociatedEntityKey::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->assocEntityKey == m_compareEntityKey;
	case COMPARE_NOT_EQUAL:			return alarm->assocEntityKey != m_compareEntityKey;
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
		}
	}
	
	FUNCTION_EXIT;
    return false;
}
/*-------------------------------------------------------------------------------------------------------*/

//++ Noel
bool AlarmFilterByAssociatedEntityKey::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->assocEntityKey == m_compareEntityKey;
	case COMPARE_NOT_EQUAL:			return alarm->assocEntityKey != m_compareEntityKey;
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}