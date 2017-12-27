/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/StateUpdateSender.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Listens to PeerStateChangeSubject and sends updates to Duty Managers.
  *
  */

#include "DutyAgent.h"
#include "StateUpdateSender.h"
#include "CorbaDutyStateConverter.h"

namespace TA_Base_App
{
	
	StateUpdateSender::StateUpdateSender() :
		TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl>(*TA_Base_Bus::PeerStateChangeSubject::instance()),
		TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl> (*TA_Base_Bus::AgentTerminateSubject::instance() ),
		m_stateSender(TA_Base_Core::MessagePublicationManager::getInstance().
			getStateUpdateMessageSender(TA_Base_Core::DutyStateUpdate::Context))
	{
		FUNCTION_ENTRY("StateUpdateSender");
		FUNCTION_EXIT;
	}
	
		void StateUpdateSender::update( TA_Base_Bus::PeerStateChangeSubjectImpl& peerStateChange, const TA_Base_Bus::DutyState & state)
	{
		FUNCTION_ENTRY("update");

		//TA_Base_Bus::DutyState* state = peerStateChange.getStateUpdate();

        // TD12082: Don't bother processing the update if no duty information is contained in it.
        if ( state.isEmpty() )
        {
            FUNCTION_EXIT;
            return;
        }

		send(state, TA_Base_Core::DutyStateUpdate::DutyAgentStateUpdate);
	}

	void StateUpdateSender::send( const TA_Base_Bus::DutyState& dutyState, const TA_Base_Core::MessageType& msgType )
	{
		FUNCTION_ENTRY("send");

		CorbaDutyStateConverter cdsc;
		dutyState.convert(cdsc);
		
		// note that getCorbaDutyState returns a pointer so the "any" should consume
		// the object and we should not delete it.
		CORBA::Any stateData;
		stateData <<= cdsc.getCorbaDutyState();

		m_stateSender->sendStateUpdateMessage( msgType, TA_Base_Bus::TransactiveAgentInfo::m_entity,
			TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME), stateData );

		FUNCTION_EXIT;
	}

	void StateUpdateSender::sendRequest()
	{
		FUNCTION_ENTRY("sendRequest");

		CORBA::Any zero;

		zero <<=  static_cast<CORBA::UShort>(0);

		m_stateSender->sendStateUpdateMessage( TA_Base_Core::DutyStateUpdate::DutyAgentStateRequest,
			TA_Base_Bus::TransactiveAgentInfo::m_entity, TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),
			zero);

		FUNCTION_EXIT;
	}

	void StateUpdateSender::update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate )
	{
		FUNCTION_ENTRY("update");

		PeerStateParent::detach();
		AgentTerminateParent::detach();

		FUNCTION_EXIT;
	}
}
