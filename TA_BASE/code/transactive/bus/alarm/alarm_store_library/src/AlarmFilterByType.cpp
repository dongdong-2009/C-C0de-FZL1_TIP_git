/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByType.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByType.cpp
//  Implementation of the Class AlarmFilterByType
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByType.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterByType::AlarmFilterByType(EComparitor compareOp,unsigned long alarmTypeKey) 
                  : IAlarmFilter(compareOp),
					m_compareTypeKey(alarmTypeKey)
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for filter.");
	FUNCTION_EXIT;
}



AlarmFilterByType::~AlarmFilterByType()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByType::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterByType* typeFilter = dynamic_cast<const AlarmFilterByType*>(&filter);
	if (typeFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==typeFilter->m_compareOp) &&
		    (m_compareTypeKey==typeFilter->m_compareTypeKey))
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
bool AlarmFilterByType::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");

	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return alarm.messageTypeKey == m_compareTypeKey;
		case COMPARE_NOT_EQUAL:			return alarm.messageTypeKey != m_compareTypeKey;
		/*
		case COMPARE_LESS_THAN:			return strcmp(alarm.messageTypeKey.in(), m_compareType.c_str())<0;
		case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm.messageTypeKey.in(), m_compareType.c_str())<=0;
		case COMPARE_GREATER_THAN:		return strcmp(alarm.messageTypeKey.in(), m_compareType.c_str())>0;
		case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm.messageTypeKey.in(), m_compareType.c_str())>=0;
		case COMPARE_CONTAINS:          return strstr(alarm.messageTypeKey.in(), m_compareType.c_str())!=NULL;
		*/
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
	}

	FUNCTION_EXIT;
    return false;
}


/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterByType::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");

	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return alarm->messageTypeKey == m_compareTypeKey;
		case COMPARE_NOT_EQUAL:			return alarm->messageTypeKey != m_compareTypeKey;		
		
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
bool AlarmFilterByType::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarm->messageTypeKey == m_compareTypeKey;
	case COMPARE_NOT_EQUAL:			return alarm->messageTypeKey != m_compareTypeKey;		
		
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}