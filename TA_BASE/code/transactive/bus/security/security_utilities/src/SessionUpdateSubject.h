#ifndef SESSIONUPDATESUBJECT_H_INCLUDED
#define SESSIONUPDATESUBJECT_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/SessionUpdateSubject.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Receives session updates from the Authentication Agent and publishes
  * the update using the observer pattern.
  *
  */

namespace TA_Base_Bus
{
	class SessionUpdateSubject : public SeSubject<SessionUpdateSubject>, 
		                         public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
	{
	public:
		SessionUpdateSubject();
		~SessionUpdateSubject();

		void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);
		void interpretUpdate( ISessionUpdateObserver& observer );

	private:
		SessionUpdateSubject( const SessionUpdateSubject& );
		SessionUpdateSubject& operator=( const SessionUpdateSubject& );
		void begin   ( ISessionUpdateObserver& observer );
		void end     ( ISessionUpdateObserver& observer );
		void overrideBegin( ISessionUpdateObserver& observer );
		void overrideEnd  ( ISessionUpdateObserver& observer );

		SessionInfoCorbaDef* m_session;
		void (SessionUpdateSubject::*m_updateFn)( ISessionUpdateObserver& observer );
	};
}

#endif // SESSIONUPDATESUBJECT_H_INCLUDED
