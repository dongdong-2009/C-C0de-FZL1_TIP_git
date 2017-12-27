/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmQueueWrapper.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#include <string>
#include <sstream>
#include <iostream>
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/AlarmData.h"
#include "app/alarm/alarm_agent/src/AlarmQueueWrapper.h"
 
using TA_Base_Core::DebugUtil;


using TA_Base_Core::IAlarmData;

namespace TA_Base_App
{

AlarmQueueWrapper::AlarmQueueWrapper( 
    TA_Base_Core::AlarmAgentQueueItemCorbaDef* pAlarmQueueItem, 
    const TA_Base_Core::AlarmDetailCorbaDef& refDetail ) :
m_pAlarmQueueItem( pAlarmQueueItem ),
m_alarmMMSState( "" ),
m_alarmDSSState( "" ),
m_alarmAVLstate( "" ), 
m_alarmStatus( "" ),
m_eventLelOrSeverityKey( refDetail.alarmSeverity ),
m_alarmTypeKey( refDetail.messageTypeKey ),
m_shouldSendCloseEvent(0)
{
    if (pAlarmQueueItem==NULL)
	{
		return;
	}

	m_alarmValue= refDetail.alarmValue.in();
	m_alarmComments=refDetail.alarmComments.in(); 
	m_alarmACKedBy=refDetail.alarmAcknowledgedBy.in();

    if (pAlarmQueueItem->containsAlarmUpdateCorbaDef)
	{
		m_alarmDescription= pAlarmQueueItem->theUpdate.alarmDescription.in();
	}
	else
	{
		m_alarmDescription= pAlarmQueueItem->theMessage.alarmDescription.in();
	}
	
	
	m_alarmMMSState = getMMSStateDBString( refDetail );
    m_alarmDSSState = getPlanStateDBString(refDetail);
    m_alarmAVLstate = getAVLStateDBString( refDetail );
    m_alarmStatus   = getAlarmStateDBString( refDetail );
	m_OMFlag        = getOMFlagString( refDetail );
}


AlarmQueueWrapper::~AlarmQueueWrapper()
{
    delete m_pAlarmQueueItem;
    m_pAlarmQueueItem = 0;
}


IAlarmData::EAlarmState AlarmQueueWrapper::convertAlarmStatus( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm )
{
	IAlarmData::EAlarmState alarmState;
 
	switch (p_alarm.state)
	{
	case TA_Base_Core::AlarmClosed:
		alarmState=TA_Base_Core::IAlarmData::CLOSED;
		break;
	case TA_Base_Core::AlarmOpen:
	    alarmState=TA_Base_Core::IAlarmData::OPEN;
		break;
	case TA_Base_Core::AlarmSuppressed:
		alarmState=TA_Base_Core::IAlarmData::SUPPRESSED;
		break;
	default:
		TA_ASSERT( false, "Unknown alarm state" );
				}
	return alarmState;
}

IAlarmData::EAlarmAVLStatus AlarmQueueWrapper::convertAVLStatus( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm )
{
	if (p_alarm.isHeadOfAvalanche)
	{
		return TA_Base_Core::IAlarmData::HEAD_AVL;
	}
	else if (p_alarm.isChildOfAvalanche)
	{
		return TA_Base_Core::IAlarmData::CHILD_AVL;
	}
	else
	{
		return TA_Base_Core::IAlarmData::NO_ASSOC_AVL;
	}
}

IAlarmData::EMmsState AlarmQueueWrapper::convertMmsStateType ( const TA_Base_Core::MmsStateType mmsState)
{
	IAlarmData::EMmsState eMmsState;
	
	switch( mmsState )
	{
	case TA_Base_Core::MMS_NONE:
		eMmsState = TA_Base_Core::IAlarmData::TYPE_NONE;
		break;
	case TA_Base_Core::MMS_SEMI:
		eMmsState = TA_Base_Core::IAlarmData::TYPE_SEMI;
		break;
	case TA_Base_Core::MMS_AUTO:
		eMmsState = TA_Base_Core::IAlarmData::TYPE_AUTO;
		break;
	case TA_Base_Core::MMS_JR_SENT:
		eMmsState = TA_Base_Core::IAlarmData::TYPE_JR_SENT;
		break;
	case TA_Base_Core::MMS_ALM_SENT:
		eMmsState = TA_Base_Core::IAlarmData::TYPE_ALM_SENT;
		break;
	case TA_Base_Core::MMS_INHIBIT:
		eMmsState = TA_Base_Core::IAlarmData::TYPE_INHIBIT;
		break;
	case TA_Base_Core::MMS_FAILED:
		eMmsState = TA_Base_Core::IAlarmData::TYPE_FAILED;
		break;
	default:
		TA_ASSERT( false, "Unknown alarm state" );
		
	} // end of switch
	
	return eMmsState;
}

//
// planStateToString
//
std::string AlarmQueueWrapper::convertPlanState( const TA_Base_Core::DecisionSupportStateType planState )
{
	std::string planStateString;
	planStateString="";
	
	switch (planState)	
	{
	case TA_Base_Core::RelatedAutomaticPlanNotRun:
		planStateString = TA_Base_Core::AlarmData::AUTO_PLAN_NOT_RUN;
		break;
	case TA_Base_Core::RelatedSemiAutomaticPlanNotRun:
		planStateString = TA_Base_Core::AlarmData::SEMIAUTO_PLAN_NOT_RUN;
		break;
	case TA_Base_Core::RelatedPlanHasBeenRun:
		planStateString = TA_Base_Core::AlarmData::PLAN_RUN;
		break;
	case TA_Base_Core::RelatedPlanHasBeenPostponed:
		planStateString = TA_Base_Core::AlarmData::PLAN_POSTPONED;
		break;
	case TA_Base_Core::NoRelatedPlan:
		planStateString = TA_Base_Core::AlarmData::NO_ASSOC_PLAN;
		break;
	default:
		TA_ASSERT( false, "Unknown alarm DSS state type" );
	}
	
	return planStateString.c_str();
}
std::string AlarmQueueWrapper::getMMSStateDBString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail )
{
	IAlarmData::EMmsState  MmsState=convertMmsStateType(refDetail.mmsState);
	std::ostringstream oss;
	oss << MmsState;
    return oss.str(); 
}

std::string AlarmQueueWrapper::getAVLStateDBString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail )
{
	IAlarmData::EAlarmAVLStatus avlState=convertAVLStatus(refDetail);
	std::ostringstream oss;
	oss << avlState;
    return oss.str();  
}




std::string AlarmQueueWrapper::getAlarmStateDBString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail )
{
	IAlarmData::EAlarmState  alarmState=convertAlarmStatus(refDetail);	
	std::ostringstream oss;
	oss << alarmState;
    return oss.str();
 
}

std::string AlarmQueueWrapper::getPlanStateDBString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail ) 
{
   return convertPlanState(refDetail.decisionSupportState);
}

const std::string AlarmQueueWrapper::getAlarmDescritpion()
{
    return m_alarmDescription.c_str();
}

const std::string AlarmQueueWrapper::getAlarmValue()
{
    return m_alarmValue.c_str();
}

const std::string AlarmQueueWrapper::getAlarmComments()
{
    return m_alarmComments.c_str();
}

const std::string AlarmQueueWrapper::getAlarmMMSState()
{
    return m_alarmMMSState.c_str();
}

const std::string AlarmQueueWrapper::getAlarmDSSState()
{
    return m_alarmDSSState.c_str();
}

const std::string AlarmQueueWrapper::getAlarmAVLstate()
{
    return m_alarmAVLstate.c_str();
}

const std::string AlarmQueueWrapper::getAlarmACKedBy()
{
    return m_alarmACKedBy.c_str();
}

const std::string AlarmQueueWrapper::getAlarmStatus()
{
    return m_alarmStatus.c_str();
}

const unsigned long AlarmQueueWrapper::getEventLelOrSeverityKey()
{
    return m_eventLelOrSeverityKey;
}

const unsigned long AlarmQueueWrapper::getAlarmTypeKey()
{
    return m_alarmTypeKey;
}

TA_Base_Core::AlarmAgentQueueItemCorbaDef* AlarmQueueWrapper::getAlarmAgentQueueItem()
{
    return m_pAlarmQueueItem;
}

const std::string AlarmQueueWrapper::getOMFlag()
{
	return m_OMFlag.c_str();
}


std::string AlarmQueueWrapper::getOMFlagString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail )
{
	const char * OperatorMaintenanceFlag = "OMFlag:";
	char const* flagPosition = strstr(refDetail.alarmParameterList,OperatorMaintenanceFlag);
	char flagValue=' ';
	if (flagPosition!=NULL)
	{
		// Get the setting (OMFlag:M, or OMFlag:O, or OMFlag:B, or OMFlag:)
		flagValue = *(flagPosition+strlen(OperatorMaintenanceFlag));
		if(flagValue == 'M')
		{
			return "M";
		}
		else if(flagValue=='O')
		{
			return "O";
		}
		else if(flagValue=='B')
		{
			return "B";
		}
		else
		{
			return "";
		}
	}
	else
	{
		return "";
	}

}
};
