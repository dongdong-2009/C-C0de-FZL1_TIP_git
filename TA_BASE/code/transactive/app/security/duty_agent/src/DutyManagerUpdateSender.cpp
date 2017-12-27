/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyManagerUpdateSender.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/03 17:57:59 $
  * Last modified by:  $Author: liwei.gong $
  *
  * Listens to PeerStateChangeSubject and sends updates to Duty Managers.
  *
  */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyManagerUpdateSender.h"
#include "app/security/duty_agent/src/DutyManagerUpdateConverter.h"
#include "app/security/duty_agent/src/RightsLibraryUpdateConverter.h"

namespace TA_Base_App
{
	
	DutyManagerUpdateSender::DutyManagerUpdateSender() :
		TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl>(*TA_Base_Bus::PeerStateChangeSubject::instance()),
		TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> (*TA_Base_Bus::AgentTerminateSubject::instance() ),
		m_commsSender(TA_Base_Core::MessagePublicationManager::getInstance().
			getCommsMessageSender(TA_Base_Core::DutyAgentBroadcastComms::Context))
	{
		FUNCTION_ENTRY("DutyManagerUpdateSender");
		FUNCTION_EXIT;
	}
	
		void DutyManagerUpdateSender::update( TA_Base_Bus::PeerStateChangeSubjectImpl& peerStateChange , const TA_Base_Bus::DutyState & state)
	{
		FUNCTION_ENTRY("update");

		//TA_Base_Bus::DutyState* pState = peerStateChange.getStateUpdate();

        // TD12082: Don't bother processing the update if no duty information is contained in it.
        if ( state.isEmpty() )
        {
            FUNCTION_EXIT;
            return;
        }

		DutyManagerUpdateConverter dmc;

		state.convert(dmc);

		TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutyChanges regionDutyChanges;

		const TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence& regionRem= dmc.getRegionsToRemove();
		if( regionRem.length() > 0 )
		{
// 			CORBA::Any commsData;
// 			commsData <<= dmc.getRegionsToRemove();
// 		
// 			m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::RemoveLocationDuty, commsData, NULL );
			
			regionDutyChanges.rem = regionRem;
		}
		
		const TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence& regionAdd= dmc.getRegionsToAdd();

		if( regionAdd.length() > 0 )
		{
// 			CORBA::Any commsData;
// 			commsData <<= dmc.getRegionsToAdd();
// 
// 			m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::AddLocationDuty, commsData, NULL );
			
			regionDutyChanges.add = regionAdd;
		}
	
		if(regionDutyChanges.add.length() >0 ||regionDutyChanges.rem.length( ) > 0 )
		{
			CORBA::Any commsData;
			commsData <<= regionDutyChanges;
			m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::LocationDutyChangeNotification, commsData, NULL );
		}
					
		
		
		RightsLibraryUpdateConverter rluc; 
		state.convert(rluc);

		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutyChanges changes;
		changes.add = dmc.getSubsystemsToAdd();
		changes.remove = dmc.getSubsystemsToRemove();
		changes.rightsUpdate = rluc.getUpdate();
		

		CORBA::Any commsData;
		commsData<<=changes;
	
		m_commsSender->sendBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::SubsubsystemDutyChangeNotification, commsData, NULL );

		FUNCTION_EXIT;
	}

	void DutyManagerUpdateSender::update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate )
	{
		FUNCTION_ENTRY("update");

		PeerStateParent::detach();
		AgentTerminateParent::detach();

		FUNCTION_EXIT;
	}
}
