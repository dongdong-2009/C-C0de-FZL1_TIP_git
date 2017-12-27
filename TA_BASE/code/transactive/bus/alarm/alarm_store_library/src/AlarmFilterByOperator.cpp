/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByOperator.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByOperator.cpp
//  Implementation of the Class AlarmFilterByOperator
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByOperator.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"

using namespace TA_Base_Bus;

AlarmFilterByOperator::AlarmFilterByOperator(EComparitor compareOp, const std::string &compareOperator) 
                  : IAlarmFilter(compareOp)
{
	FUNCTION_ENTRY("Constructor");

	m_compareOperator = compareOperator.c_str();

	TA_ASSERT(compareOp!=COMPARE_CONTAINS,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_LESS_THAN,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_LESS_THAN_EQUAL,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_GREATER_THAN,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_GREATER_THAN_EQUAL,"Invalid comparison for operator filter.");
	FUNCTION_EXIT;
}



AlarmFilterByOperator::~AlarmFilterByOperator()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByOperator::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterByOperator* operatorFilter = dynamic_cast<const AlarmFilterByOperator*>(&filter);
	if (operatorFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==operatorFilter->m_compareOp) &&
		    (m_compareOperator.compare(operatorFilter->m_compareOperator.c_str()) == 0) )
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
bool AlarmFilterByOperator::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");

	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return  operatorInList(alarm.alarmAcknowledgedBy.in());
		case COMPARE_NOT_EQUAL:			return !operatorInList(alarm.alarmAcknowledgedBy.in());
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
	}

	FUNCTION_EXIT;
    return false;
}


bool AlarmFilterByOperator::operatorInList( const char* delimList )
{
	// need a copy because strtok wants to modify delimList
	// TD #10898
	// check that the destination string is not larger than the source,
	// so that it will not try and copy past the size of the source
	m_operatorList = delimList;

	bool found=false;
	char* token = NULL;

	// TODO: should this be using ACE_OS_String::strtok_r? 
	token = strtok((char*) m_operatorList.c_str(), AlarmConstants::OPERATOR_DELIMITER.c_str() );
	while( token != NULL && found==false)
	{
		if( strcmp(token, m_compareOperator.c_str())==0 )
		{
			found = true;
		}
		token = strtok( NULL, AlarmConstants::OPERATOR_DELIMITER.c_str() );
	}

	return( found );
}
/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterByOperator::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return  operatorInList(alarm->alarmAcknowledgedBy);
	case COMPARE_NOT_EQUAL:			return !operatorInList(alarm->alarmAcknowledgedBy);
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
bool AlarmFilterByOperator::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return  operatorInList(alarm->alarmAcknowledgedBy.c_str());
	case COMPARE_NOT_EQUAL:			return !operatorInList(alarm->alarmAcknowledgedBy.c_str());
	default:
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
			
		}
	}
	
	FUNCTION_EXIT;
    return false;
}