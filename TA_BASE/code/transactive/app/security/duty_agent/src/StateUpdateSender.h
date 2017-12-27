/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/StateUpdateSender.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Listens to PeerStateChangeSubject and sends updates to the Monitor-Mode Duty Agent.
  *
  */

#ifndef STATEUPDATESENDER_H_INCLUDED
#define STATEUPDATESENDER_H_INCLUDED

namespace TA_Base_App
{
	class StateUpdateSender : public TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl>,
		                      public TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>
	{
	public:
		StateUpdateSender();

		virtual void update( TA_Base_Bus::PeerStateChangeSubjectImpl& peerStateChange, const TA_Base_Bus::DutyState & state );
		virtual void update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate );

		// allows a forced send. usefull for bulk snapshots that are the result of a query
		// rather than a change in duty state. they are picked up by the subscription to
		// the peerstatechangesubject.
		void send( const TA_Base_Bus::DutyState& dutyState, const TA_Base_Core::MessageType& msgType );

		void sendRequest();

	private:

		typedef TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl> PeerStateParent;
		typedef TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>  AgentTerminateParent;

		std::auto_ptr<TA_Base_Core::StateUpdateMessageSender> m_stateSender;
	};
}
#endif // STATEUPDATESENDER_H_INCLUDED
