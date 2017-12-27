/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByState.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByState.cpp
//  Implementation of the Class AlarmFilterByState
//  Created on:      24-Jan-2004 04:19:04 PM
///////////////////////////////////////////////////////////

#include "AlarmFilterByState.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;

AlarmFilterByState::AlarmFilterByState(EComparitor compareOp,EAlarmState compareState) 
                  : IAlarmFilter(compareOp),
				    m_compareState(compareState)
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



AlarmFilterByState::~AlarmFilterByState()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

bool AlarmFilterByState::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator== ");
	// See if it is the same filter type
	const AlarmFilterByState* operatorFilter = dynamic_cast<const AlarmFilterByState*>(&filter);
	if (operatorFilter!=NULL)
	{
		// Check parameters are the same
		if ((m_compareOp==operatorFilter->m_compareOp) &&
		    (m_compareState==operatorFilter->m_compareState))
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
bool AlarmFilterByState::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
		case COMPARE_EQUAL:				return alarmStatesMatch(m_compareState,alarm);
		case COMPARE_NOT_EQUAL:			return !alarmStatesMatch(m_compareState,alarm);
	}

	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
    return false;
}

bool AlarmFilterByState::alarmStatesMatch( EAlarmState p_state, const TA_Base_Core::AlarmDetailCorbaDef& p_alarm ) const
{
    if( p_state == EAlarmState::ANY )
    {
        return true;
    }else if( p_state == EAlarmState::ALL_OPEN)
    {
        return (p_alarm.state == TA_Base_Core::AlarmOpen);
    }else if( p_state == EAlarmState::CLOSED)
    {
        return (p_alarm.state == TA_Base_Core::AlarmClosed);
    }else if( p_state == EAlarmState::OPEN_UNACKED)
    {
        return ((p_alarm.state == TA_Base_Core::AlarmOpen) && (true == std::string(p_alarm.alarmAcknowledgedBy).empty()));
    }else if( p_state == EAlarmState::OPEN_ACKED)
    {
        return ((p_alarm.state == TA_Base_Core::AlarmOpen) && ( false == std::string(p_alarm.alarmAcknowledgedBy).empty()));
    }else if( p_state == EAlarmState::ALL_UNACKED)
    {
        return (true == std::string(p_alarm.alarmAcknowledgedBy).empty());
    }else{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
		
	}

    return false; // Keeps compiler happy
}

/*----------------------------------------------New Alarm Store----------------------------------------------*/
bool AlarmFilterByState::alarmStatesMatch( EAlarmState p_state, const TA_Base_Core::AlarmItem* p_alarm ) const
{
    if( p_state == EAlarmState::ANY )
    {
        return true;
    }else if( p_state == EAlarmState::ALL_OPEN)
    {
        return (p_alarm->state == TA_Base_Core::AlarmOpen);
    }else if( p_state == EAlarmState::CLOSED)
    {
        return (p_alarm->state == TA_Base_Core::AlarmClosed);
    }else if( p_state == EAlarmState::OPEN_UNACKED)
    {
        return ((p_alarm->state == TA_Base_Core::AlarmOpen) && (true == std::string(p_alarm->alarmAcknowledgedBy).empty()));
    }else if( p_state == EAlarmState::OPEN_ACKED)
    {
        return ((p_alarm->state == TA_Base_Core::AlarmOpen) && ( false == std::string(p_alarm->alarmAcknowledgedBy).empty()));
    }else if( p_state == EAlarmState::ALL_UNACKED)
    {
        return (true == std::string(p_alarm->alarmAcknowledgedBy).empty());
    }else{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
		
	}
	
    return false;// Keeps compiler happy
}

bool AlarmFilterByState::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarmStatesMatch(m_compareState,alarm);
	case COMPARE_NOT_EQUAL:			return !alarmStatesMatch(m_compareState,alarm);
	}
	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
    return false;
}
/*-----------------------------------------------------------------------------------------------------------*/

//++Noel
bool AlarmFilterByState::alarmStatesMatch( EAlarmState p_state, const TA_Base_Bus::AlarmDataItem* p_alarm ) const
{
    if( p_state == EAlarmState::ANY )
    {
        return true;
    }else if( p_state == EAlarmState::ALL_OPEN)
    {
        return (p_alarm->state == TA_Base_Core::AlarmOpen);
    }else if( p_state == EAlarmState::CLOSED)
    {
        return (p_alarm->state == TA_Base_Core::AlarmClosed);
    }else if( p_state == EAlarmState::OPEN_UNACKED)
    {
        return ((p_alarm->state == TA_Base_Core::AlarmOpen) && (true == p_alarm->alarmAcknowledgedBy.empty()));
    }else if( p_state == EAlarmState::OPEN_ACKED)
    {
        return ((p_alarm->state == TA_Base_Core::AlarmOpen) && ( false == p_alarm->alarmAcknowledgedBy.empty()));
    }else if( p_state == EAlarmState::ALL_UNACKED)
    {
        return (true == p_alarm->alarmAcknowledgedBy.empty());
    }else{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "switch return false as abnomal case" );
		
	}
	
    return false; // Keeps compiler happy
}

bool AlarmFilterByState::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	switch (m_compareOp)
	{
	case COMPARE_EQUAL:				return alarmStatesMatch(m_compareState,alarm);
	case COMPARE_NOT_EQUAL:			return !alarmStatesMatch(m_compareState,alarm);
	}
	// Assertion has already been done on construction
	//TA_ASSERT(false,"Comparison operator not supported.");
	FUNCTION_EXIT;
    return false;
}