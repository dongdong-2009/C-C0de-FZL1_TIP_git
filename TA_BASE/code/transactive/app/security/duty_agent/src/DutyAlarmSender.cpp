/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyAlarmSender.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Listens to FrontEndStateChangeSubject and sends messages for displaying to affected operators.
  *
  */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyAlarmSender.h"
#include "app/security/duty_agent/src/HandoverAlarmConverter.h"

namespace TA_Base_App
{
	
	DutyAlarmSender::DutyAlarmSender() :
		TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::FrontEndStateChangeSubjectImpl>(*TA_Base_Bus::FrontEndStateChangeSubject::instance()),
		TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> (*TA_Base_Bus::AgentTerminateSubject::instance() )
		//TD18093, m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper())
	{
		FUNCTION_ENTRY("DutyAlarmSender");
		FUNCTION_EXIT;
	}
	
		void DutyAlarmSender::update( TA_Base_Bus::FrontEndStateChangeSubjectImpl& frontEndStateChange , const TA_Base_Bus::DutyState & state)
	{
		FUNCTION_ENTRY("update");

		 //TA_Base_Bus::DutyState* state = frontEndStateChange.getStateUpdate();

		if( DutyAction::REQUEST_SUBSYSTEM_DUTY_TRANSFER == state.getInitiatingAction() ||
		    DutyAction::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER == state.getInitiatingAction() )
		{
			HandoverAlarmConverter hac;

			state.convert(hac);

			std::vector<TA_Base_Core::DescriptionParameters*>& deniedDesc   = hac.buildDeniedAlarmDesc();
			std::vector<TA_Base_Core::DescriptionParameters*>& timedOutDesc = hac.buildTimedOutAlarmDesc();

			std::vector<TA_Base_Core::DescriptionParameters*>::const_iterator descIt;

			for( descIt=deniedDesc.begin(); descIt!=deniedDesc.end(); descIt++ )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Sending \"handover denied\" alarm" );
				TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
							TA_Base_Core::DutyAgentAlarms::SubsystemHandoverDenied,
							TA_Base_Bus::TransactiveAgentInfo::m_entity,
							TA_Base_Bus::TransactiveAgentInfo::m_entityType,
							*(*descIt),
							TA_Base_Bus::TransactiveAgentInfo::m_name,
							TA_Base_Bus::TransactiveAgentInfo::m_location,
							TA_Base_Bus::TransactiveAgentInfo::m_subsystem,
							TA_Base_Bus::TransactiveAgentInfo::m_assetName,
							TA_Base_Core::AlarmConstants::defaultTime );
			}

			for( descIt=timedOutDesc.begin(); descIt!=timedOutDesc.end(); descIt++ )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Sending \"handover by timeout\" alarm" );
				TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
							TA_Base_Core::DutyAgentAlarms::SubsystemHandoverByTimeout,
							TA_Base_Bus::TransactiveAgentInfo::m_entity,
							TA_Base_Bus::TransactiveAgentInfo::m_entityType,
							*(*descIt),
							TA_Base_Bus::TransactiveAgentInfo::m_name,
							TA_Base_Bus::TransactiveAgentInfo::m_location,
							TA_Base_Bus::TransactiveAgentInfo::m_subsystem,
							TA_Base_Bus::TransactiveAgentInfo::m_assetName,
							TA_Base_Core::AlarmConstants::defaultTime );
			}
		}

			
		FUNCTION_EXIT;
	}

	void DutyAlarmSender::update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate )
	{
		FUNCTION_ENTRY("update");

		FrontEndStateParent::detach();
		AgentTerminateParent::detach();

		FUNCTION_EXIT;
	}
}
