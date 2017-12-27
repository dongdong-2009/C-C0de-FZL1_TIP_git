/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByAssociatedEntityName.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByAssociatedEntityName.cpp
//  Implementation of the Class AlarmFilterByAssociatedEntityName
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByAssociatedEntityName.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterByAssociatedEntityName::AlarmFilterByAssociatedEntityName(EComparitor compareOp, const std::string &compareEntityName) 
                  : IAlarmFilter(compareOp),
					m_compareEntityName(compareEntityName.c_str())
{
	FUNCTION_ENTRY("Constructor");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for filter.");
	FUNCTION_EXIT;
}



AlarmFilterByAssociatedEntityName::~AlarmFilterByAssociatedEntityName()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByAssociatedEntityName::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterByAssociatedEntityName* entityNameFilter = dynamic_cast<const AlarmFilterByAssociatedEntityName*>(&filter);
	if (entityNameFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==entityNameFilter->m_compareOp) &&
		    (m_compareEntityName==entityNameFilter->m_compareEntityName))
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
bool AlarmFilterByAssociatedEntityName::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return strcmp(alarm.entityName.in(), m_compareEntityName.c_str())==0;
		case COMPARE_NOT_EQUAL:			return strcmp(alarm.entityName.in(), m_compareEntityName.c_str())!=0;
		case COMPARE_LESS_THAN:			return strcmp(alarm.entityName.in(), m_compareEntityName.c_str())<0;
		case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm.entityName.in(), m_compareEntityName.c_str())<=0;
		case COMPARE_GREATER_THAN:		return strcmp(alarm.entityName.in(), m_compareEntityName.c_str())>0;
		case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm.entityName.in(), m_compareEntityName.c_str())>=0;
		case COMPARE_CONTAINS:			return strstr(alarm.entityName.in(), m_compareEntityName.c_str())!=NULL;
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
			}
	}

	FUNCTION_EXIT;
    return false;
}

/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterByAssociatedEntityName::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->entityName, m_compareEntityName.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->entityName, m_compareEntityName.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->entityName, m_compareEntityName.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->entityName, m_compareEntityName.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->entityName, m_compareEntityName.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->entityName, m_compareEntityName.c_str())>=0;
	case COMPARE_CONTAINS:			return strstr(alarm->entityName, m_compareEntityName.c_str())!=NULL;
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
bool AlarmFilterByAssociatedEntityName::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return strcmp(alarm->entityName.c_str(), m_compareEntityName.c_str())==0;
	case COMPARE_NOT_EQUAL:			return strcmp(alarm->entityName.c_str(), m_compareEntityName.c_str())!=0;
	case COMPARE_LESS_THAN:			return strcmp(alarm->entityName.c_str(), m_compareEntityName.c_str())<0;
	case COMPARE_LESS_THAN_EQUAL:	return strcmp(alarm->entityName.c_str(), m_compareEntityName.c_str())<=0;
	case COMPARE_GREATER_THAN:		return strcmp(alarm->entityName.c_str(), m_compareEntityName.c_str())>0;
	case COMPARE_GREATER_THAN_EQUAL:return strcmp(alarm->entityName.c_str(), m_compareEntityName.c_str())>=0;
	case COMPARE_CONTAINS:			return strstr(alarm->entityName.c_str(), m_compareEntityName.c_str())!=NULL;
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}