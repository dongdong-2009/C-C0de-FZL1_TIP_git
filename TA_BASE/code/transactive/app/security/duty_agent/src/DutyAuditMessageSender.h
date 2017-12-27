/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyAuditMessageSender.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Listens to FrontEndStateChangeSubject and sends messages for displaying to affected operators.
  *
  */

#ifndef DUTYAUDITMESSAGESENDER_H_INCLUDED
#define DUTYAUDITMESSAGESENDER_H_INCLUDED

#include "core/threads/src/IWorkItem.h"

namespace TA_Base_Core{
	class ThreadPoolSingletonManager;
}
namespace TA_Base_App
{
	class DutyAuditMessageSender : public TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::FrontEndStateChangeSubjectImpl>,
		                    public TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>
	{
	public:
		DutyAuditMessageSender();

		virtual void update( TA_Base_Bus::FrontEndStateChangeSubjectImpl& frontEndStateChange,const TA_Base_Bus::DutyState & state );
		virtual void update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate );

		void update(const TA_Base_Bus::DutyState & state);

		virtual ~DutyAuditMessageSender();

	private:
        // If srcIsSender is true, the src session is the sender (originator) of the audit message,
        // otherwise the dst session is the sender.
        void DutyAuditMessageSender::sendAuditMessage(
            const TA_Base_Core::MessageType& messageType,
            const std::string& srcSessionStr,
            const std::string& dstSessionStr,
            bool srcIsSender,                         
            const std::string& subsystemIdList,
            const std::string& regionIdList,
            const std::string& msgDetailsStr );

        // Returns a string of the form "left/right".
        std::string concatStr(const std::string& left, const std::string& right);
		// Returns a string connect left and right using connector
		std::string concatStr(const std::string& left, const std::string& right, const std::string& connector);

        void sendSubsystemsMessage( const TA_Base_Core::MessageType& messageType, const TA_Base_Bus::DutyState& state );

		std::auto_ptr<TA_Base_Core::AuditMessageSender> m_auditSender;

		//azenitha++
		enum AuditMessageType
		{
			AUDIT_ACCEPT,
			AUDIT_DENY,
			AUDIT_TIMEOUT
		};

		struct AuditMessageData //Request to subsystem/s
		{
			std::string srcSession;
			std::string destSession;
			std::string subsystemName;
			std::vector<std::string> regionNames;
			AuditMessageType auditMessageType;
		};

		struct AuditMessageDataAll //Request to ALL subsystem
		{
			std::string srcSession;
			std::string destSession;
			AuditMessageType auditMessageType;
		};

		typedef std::vector<AuditMessageData> AuditMessageDataList; //Request to subsystem/s
		typedef std::vector<AuditMessageDataAll> AuditMessageDataAllList; //Request to ALL subsystem

		//AuditMessageDataList m_auditMessageDataList; //Request to subsystem/s
		//AuditMessageDataAllList m_auditMessageDataAllList; //Request to ALL subsystem
	
		void clearPendingAuditMessages( AuditMessageDataList& auditMessageDataList ); //Request to subsystem/s
		void sendPendingAuditMessages( AuditMessageDataList& auditMessageDataList ); //Request to subsystem/s
		void pendingAuditMessageToSend(
			const std::string& srcSession,
			const std::string& destSession,
			const std::string& subsystemName,
			const std::string& regionName,
			const AuditMessageType& auditMessageType,
			AuditMessageDataList& auditMessageDataList); //Request to subsystem/s

		void clearPendingAuditMessagesAll( AuditMessageDataAllList& auditMessageDataAllList ); //Request to ALL subsystem
		void sendPendingAuditMessagesAll( AuditMessageDataAllList& auditMessageDataAllList ); //Request to ALL subsystem
		void pendingAuditMessageToSendAll(
			const std::string& srcSession,
			const std::string& destSession,
			const AuditMessageType& auditMessageType,
			AuditMessageDataAllList& auditMessageDataAllList); //Request to ALL subsystem
		TA_Base_Core::ThreadPoolSingletonManager * m_threadPool;
		//++azenitha
	};
	class DutyAuditMessageWorkItem: public TA_Base_Core::IWorkItem
	{
	public:
		DutyAuditMessageWorkItem(DutyAuditMessageSender& parent,boost::shared_ptr<TA_Base_Bus::DutyState> dutyState);
		virtual ~DutyAuditMessageWorkItem();
		//operations specific to IWorkItem
		virtual void executeWorkItem();
	protected:
		DutyAuditMessageSender& parent_;
		boost::shared_ptr<TA_Base_Bus::DutyState> dutyState_;
	};
}
#endif // DUTYAUDITMESSAGESENDER_H_INCLUDED
