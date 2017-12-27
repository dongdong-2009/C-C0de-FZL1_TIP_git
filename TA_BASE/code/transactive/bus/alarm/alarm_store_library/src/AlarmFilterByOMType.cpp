/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByOMType.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by:  $Author: grace.koh $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByOMType.cpp
//  Implementation of the Class AlarmFilterByOMType
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByOMType.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterByOMType::AlarmFilterByOMType(EComparitor compareOp,EAlarmOMType compareOMType) 
                  : IAlarmFilter(compareOp),
				    m_compareOMType(compareOMType)
{
	FUNCTION_ENTRY("Constructor");

	TA_ASSERT(compareOp!=COMPARE_CONTAINS,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_INCLUSIVE,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_BETWEEN_EXCLUSIVE,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_LESS_THAN,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_LESS_THAN_EQUAL,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_GREATER_THAN,"Invalid comparison for operator filter.");
	TA_ASSERT(compareOp!=COMPARE_GREATER_THAN_EQUAL,"Invalid comparison for operator filter.");
	FUNCTION_EXIT;
}



AlarmFilterByOMType::~AlarmFilterByOMType()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByOMType::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterByOMType* operatorFilter = dynamic_cast<const AlarmFilterByOMType*>(&filter);
	if (operatorFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==operatorFilter->m_compareOp) &&
		    (m_compareOMType==operatorFilter->m_compareOMType))
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
bool AlarmFilterByOMType::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return alarmOMTypeMatch(m_compareOMType,alarm);
		case COMPARE_NOT_EQUAL:			return !alarmOMTypeMatch(m_compareOMType,alarm);
	}
	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
    return false;
}

bool AlarmFilterByOMType::alarmOMTypeMatch( EAlarmOMType p_state, const TA_Base_Core::AlarmDetailCorbaDef& p_alarm ) const
{
	/*
    if( p_state == EAlarmOMType::OMTYPE_ALL )
    {
        return true;
    }
	else if( p_state == EAlarmOMType::OMTYPE_OPERATION)
    {
        return (p_alarm.state == 'O' || p_alarm.state == 'o');
    }
	else if( p_state == EAlarmOMType::OMTYPE_MAINTENANCE)
    {
        return (p_alarm.state == 'M' || p_alarm.state == 'm');
    }
	else if( p_state == EAlarmOMType::OMTYPE_BOTH)
    {
        return (p_alarm.state == 'B' || p_alarm.state == 'b');
    }
	else
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
		return false;
	}*/

    return true; // Keeps compiler happy
}

/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterByOMType::alarmOMTypeMatch( EAlarmOMType p_state, const TA_Base_Core::AlarmItem* p_alarm ) const
{
    if( p_state == EAlarmOMType::OMTYPE_ALL )
    {
        return true;
    }
	else if( p_state == EAlarmOMType::OMTYPE_OPERATION)
    {
        return (strcmp(p_alarm->alarmOperatorMode,"O")==0 || strcmp(p_alarm->alarmOperatorMode,"o")==0);
    }
	else if( p_state == EAlarmOMType::OMTYPE_MAINTENANCE)
    {
        return (strcmp(p_alarm->alarmOperatorMode,"M")==0 || strcmp(p_alarm->alarmOperatorMode,"m")==0);
    }
	else if( p_state == EAlarmOMType::OMTYPE_BOTH)
    {
        return (strcmp(p_alarm->alarmOperatorMode,"B")==0 || strcmp(p_alarm->alarmOperatorMode,"b")==0);
    }
	else{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
		
	}
	
    return false; // Keeps compiler happy
}

bool AlarmFilterByOMType::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarmOMTypeMatch(m_compareOMType,alarm);
	case COMPARE_NOT_EQUAL:			return !alarmOMTypeMatch(m_compareOMType,alarm);
	}
	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
    return false;
}
/*-----------------------------------------------------------------------------------------------------------*/

//++Noel
bool AlarmFilterByOMType::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarmOMTypeMatch(m_compareOMType,alarm);
	case COMPARE_NOT_EQUAL:			return !alarmOMTypeMatch(m_compareOMType,alarm);
	}
	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
	return false;
}

bool AlarmFilterByOMType::alarmOMTypeMatch( EAlarmOMType p_state, const TA_Base_Bus::AlarmDataItem* p_alarm ) const
{
	if( p_state == EAlarmOMType::OMTYPE_ALL )
	{
		return true;
	}
	else if( p_state == EAlarmOMType::OMTYPE_OPERATION)
	{
		return (strcmp(p_alarm->alarmOperatorMode.c_str(),"O")==0 || strcmp(p_alarm->alarmOperatorMode.c_str(),"o")==0);
	}
	else if( p_state == EAlarmOMType::OMTYPE_MAINTENANCE)
	{
		return (strcmp(p_alarm->alarmOperatorMode.c_str(),"M")==0 || strcmp(p_alarm->alarmOperatorMode.c_str(),"m")==0);
	}
	else if( p_state == EAlarmOMType::OMTYPE_BOTH)
	{
		return (strcmp(p_alarm->alarmOperatorMode.c_str(),"B")==0 || strcmp(p_alarm->alarmOperatorMode.c_str(),"b")==0);
	}
	else{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
		
	}

	return false; // Keeps compiler happy	
}