/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByDate.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByDate.cpp
//  Implementation of the Class AlarmFilterByDate
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByDate.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/utilities/src/RunParams.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"

using namespace TA_Base_Bus;

AlarmFilterByDate::AlarmFilterByDate(EComparitor compareOp,unsigned long compareDate1) 
                  : IAlarmFilter(compareOp),
					m_compareDate1(compareDate1),
					m_compareDate2(0)
{
	FUNCTION_ENTRY("Constructor1");
	TA_ASSERT(compareOp!=COMPARE_CONTAINS,"Invalid comparison for date filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for date filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for date filter.");
	FUNCTION_EXIT;
}

AlarmFilterByDate::AlarmFilterByDate(EComparitor compareOp,unsigned long compareDate1,unsigned long compareDate2) 
                  : IAlarmFilter(compareOp),
					m_compareDate1(compareDate1),
					m_compareDate2(compareDate2)
{
	FUNCTION_ENTRY("Constructor2");
	TA_ASSERT((compareOp==COMPARE_BETWEEN_EXCLUSIVE)||(compareOp==COMPARE_BETWEEN_INCLUSIVE),
		      "Invalid comparison for date filter.");
	FUNCTION_EXIT;
}


AlarmFilterByDate::~AlarmFilterByDate()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByDate::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator==");
	// See if it is the same filter type
	const AlarmFilterByDate* dateFilter = dynamic_cast<const AlarmFilterByDate*>(&filter);
	if (dateFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==dateFilter->m_compareOp) &&
		    (m_compareDate1==dateFilter->m_compareDate1) &&
		    (m_compareDate2==dateFilter->m_compareDate2))
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
bool AlarmFilterByDate::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	//is it supposed to be configured this way
	if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_USESOURCETIME.c_str()))
    {
		switch (m_compareOp)
		{			
			case COMPARE_EQUAL:				return alarm.sourceTime.time==m_compareDate1;
			case COMPARE_NOT_EQUAL:			return alarm.sourceTime.time!=m_compareDate1;
			case COMPARE_LESS_THAN:			return alarm.sourceTime.time<m_compareDate1;
			case COMPARE_LESS_THAN_EQUAL:	return alarm.sourceTime.time<=m_compareDate1;
			case COMPARE_GREATER_THAN:		return alarm.sourceTime.time>m_compareDate1;
			case COMPARE_GREATER_THAN_EQUAL:return alarm.sourceTime.time>=m_compareDate1;
			case COMPARE_BETWEEN_INCLUSIVE: return (alarm.sourceTime.time>=m_compareDate1&&
													alarm.sourceTime.time<=m_compareDate2);
			case COMPARE_BETWEEN_EXCLUSIVE: return (alarm.sourceTime.time>m_compareDate1&&
													alarm.sourceTime.time<m_compareDate2);
			default:
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
					
				}
		}
	}
	else
	{
		switch (m_compareOp)
		{
			case COMPARE_EQUAL:				return alarm.displayTime.time==m_compareDate1;
			case COMPARE_NOT_EQUAL:			return alarm.displayTime.time!=m_compareDate1;
			case COMPARE_LESS_THAN:			return alarm.displayTime.time<m_compareDate1;
			case COMPARE_LESS_THAN_EQUAL:	return alarm.displayTime.time<=m_compareDate1;
			case COMPARE_GREATER_THAN:		return alarm.displayTime.time>m_compareDate1;
			case COMPARE_GREATER_THAN_EQUAL:return alarm.displayTime.time>=m_compareDate1;
			case COMPARE_BETWEEN_INCLUSIVE: return (alarm.displayTime.time>=m_compareDate1&&
													alarm.displayTime.time<=m_compareDate2);
			case COMPARE_BETWEEN_EXCLUSIVE: return (alarm.displayTime.time>m_compareDate1&&
													alarm.displayTime.time<m_compareDate2);
			default:
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
					
				}
		}
	}

	FUNCTION_EXIT;
    return false;
}
/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterByDate::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	//is it supposed to be configured this way
	if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_USESOURCETIME.c_str()))
    {
		switch (m_compareOp)
		{			
		case COMPARE_EQUAL:				return alarm->transactiveTime.time==m_compareDate1;
		case COMPARE_NOT_EQUAL:			return alarm->transactiveTime.time!=m_compareDate1;
		case COMPARE_LESS_THAN:			return alarm->transactiveTime.time<m_compareDate1;
		case COMPARE_LESS_THAN_EQUAL:	return alarm->transactiveTime.time<=m_compareDate1;
		case COMPARE_GREATER_THAN:		return alarm->transactiveTime.time>m_compareDate1;
		case COMPARE_GREATER_THAN_EQUAL:return alarm->transactiveTime.time>=m_compareDate1;
		case COMPARE_BETWEEN_INCLUSIVE: return (alarm->transactiveTime.time>=m_compareDate1&&
											alarm->transactiveTime.time<=m_compareDate2);
		case COMPARE_BETWEEN_EXCLUSIVE: return (alarm->transactiveTime.time>m_compareDate1&&
											alarm->transactiveTime.time<m_compareDate2);
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
		}
	}
	else
	{
		switch (m_compareOp)
		{
		case COMPARE_EQUAL:				return alarm->transactiveTime.time==m_compareDate1;
		case COMPARE_NOT_EQUAL:			return alarm->transactiveTime.time!=m_compareDate1;
		case COMPARE_LESS_THAN:			return alarm->transactiveTime.time<m_compareDate1;
		case COMPARE_LESS_THAN_EQUAL:	return alarm->transactiveTime.time<=m_compareDate1;
		case COMPARE_GREATER_THAN:		return alarm->transactiveTime.time>m_compareDate1;
		case COMPARE_GREATER_THAN_EQUAL:return alarm->transactiveTime.time>=m_compareDate1;
		case COMPARE_BETWEEN_INCLUSIVE: return (alarm->transactiveTime.time>=m_compareDate1&&
											alarm->transactiveTime.time<=m_compareDate2);
		case COMPARE_BETWEEN_EXCLUSIVE: return (alarm->transactiveTime.time>m_compareDate1&&
											alarm->transactiveTime.time<m_compareDate2);
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
		}
	}
	
	FUNCTION_EXIT;
    return false;
}
/*-----------------------------------------------------------------------------------------------------------*/

//++Noel
bool AlarmFilterByDate::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	//is it supposed to be configured this way
	if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_USESOURCETIME.c_str()))
    {
		switch (m_compareOp)
		{			
		case COMPARE_EQUAL:				return alarm->transactiveTime.time==m_compareDate1;
		case COMPARE_NOT_EQUAL:			return alarm->transactiveTime.time!=m_compareDate1;
		case COMPARE_LESS_THAN:			return alarm->transactiveTime.time<m_compareDate1;
		case COMPARE_LESS_THAN_EQUAL:	return alarm->transactiveTime.time<=m_compareDate1;
		case COMPARE_GREATER_THAN:		return alarm->transactiveTime.time>m_compareDate1;
		case COMPARE_GREATER_THAN_EQUAL:return alarm->transactiveTime.time>=m_compareDate1;
		case COMPARE_BETWEEN_INCLUSIVE: return (alarm->transactiveTime.time>=m_compareDate1&&
											alarm->transactiveTime.time<=m_compareDate2);
		case COMPARE_BETWEEN_EXCLUSIVE: return (alarm->transactiveTime.time>m_compareDate1&&
											alarm->transactiveTime.time<m_compareDate2);
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
		}
	}
	else
	{
		switch (m_compareOp)
		{
		case COMPARE_EQUAL:				return alarm->transactiveTime.time==m_compareDate1;
		case COMPARE_NOT_EQUAL:			return alarm->transactiveTime.time!=m_compareDate1;
		case COMPARE_LESS_THAN:			return alarm->transactiveTime.time<m_compareDate1;
		case COMPARE_LESS_THAN_EQUAL:	return alarm->transactiveTime.time<=m_compareDate1;
		case COMPARE_GREATER_THAN:		return alarm->transactiveTime.time>m_compareDate1;
		case COMPARE_GREATER_THAN_EQUAL:return alarm->transactiveTime.time>=m_compareDate1;
		case COMPARE_BETWEEN_INCLUSIVE: return (alarm->transactiveTime.time>=m_compareDate1&&
											alarm->transactiveTime.time<=m_compareDate2);
		case COMPARE_BETWEEN_EXCLUSIVE: return (alarm->transactiveTime.time>m_compareDate1&&
											alarm->transactiveTime.time<m_compareDate2);
		default:
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
				
			}
		}
	}
	
	FUNCTION_EXIT;
    return false;
}
