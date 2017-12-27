/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterBySubSystem.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterBySubSystem.cpp
//  Implementation of the Class AlarmFilterBySubSystem
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterBySubSystem.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterBySubSystem::AlarmFilterBySubSystem(EComparitor compareOp,unsigned long compareSubSystem) 
                  : IAlarmFilter(compareOp),
					m_compareSubSystem(compareSubSystem)
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_CONTAINS,"Invalid comparison for location filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for location filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for location filter.");
	FUNCTION_EXIT;
}



AlarmFilterBySubSystem::~AlarmFilterBySubSystem()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterBySubSystem::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator==");
	// See if it is the same filter type
	const AlarmFilterBySubSystem* subSystemFilter = dynamic_cast<const AlarmFilterBySubSystem*>(&filter);
	if (subSystemFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==subSystemFilter->m_compareOp) &&
		    (m_compareSubSystem==subSystemFilter->m_compareSubSystem))
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
bool AlarmFilterBySubSystem::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return alarm.subsystemKey==m_compareSubSystem;
		case COMPARE_NOT_EQUAL:			return alarm.subsystemKey!=m_compareSubSystem;
		case COMPARE_LESS_THAN:			return alarm.subsystemKey<m_compareSubSystem;
		case COMPARE_LESS_THAN_EQUAL:	return alarm.subsystemKey<=m_compareSubSystem;
		case COMPARE_GREATER_THAN:		return alarm.subsystemKey>m_compareSubSystem;
		case COMPARE_GREATER_THAN_EQUAL:return alarm.subsystemKey>=m_compareSubSystem;
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
	}
	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
    return false;
}

/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterBySubSystem::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->subsystemKey==m_compareSubSystem;
	case COMPARE_NOT_EQUAL:			return alarm->subsystemKey!=m_compareSubSystem;
	case COMPARE_LESS_THAN:			return alarm->subsystemKey<m_compareSubSystem;
	case COMPARE_LESS_THAN_EQUAL:	return alarm->subsystemKey<=m_compareSubSystem;
	case COMPARE_GREATER_THAN:		return alarm->subsystemKey>m_compareSubSystem;
	case COMPARE_GREATER_THAN_EQUAL:return alarm->subsystemKey>=m_compareSubSystem;
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
    return false;
}
/*-----------------------------------------------------------------------------------------------------------*/

//++Noel
bool AlarmFilterBySubSystem::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->subsystemKey==m_compareSubSystem;
	case COMPARE_NOT_EQUAL:			return alarm->subsystemKey!=m_compareSubSystem;
	case COMPARE_LESS_THAN:			return alarm->subsystemKey<m_compareSubSystem;
	case COMPARE_LESS_THAN_EQUAL:	return alarm->subsystemKey<=m_compareSubSystem;
	case COMPARE_GREATER_THAN:		return alarm->subsystemKey>m_compareSubSystem;
	case COMPARE_GREATER_THAN_EQUAL:return alarm->subsystemKey>=m_compareSubSystem;
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
    return false;
}