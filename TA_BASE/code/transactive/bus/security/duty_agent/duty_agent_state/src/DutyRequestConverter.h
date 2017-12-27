/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyRequestConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Converts DutyState to the format used for duty requests.
 */

#if !defined(DUTYREQUESTCONVERTER_H_INCLUDED)
#define DUTYREQUESTCONVERTER_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyRequestConverter : public IDutyStateConverter
	{
	public:

		typedef std::map<std::string, DutyAgentTypeCorbaDef::DutyRequest> RequestMap;

		DutyRequestConverter( const DutyPrimitive& requestingSession );

		void buildRequestMap(RequestMap& requests, DutyState& dutyState);
		
		virtual void setSession(const std::string& session);
		virtual void setAction(const std::string& action);
	
		virtual void setRegionCount( unsigned long count );

		virtual void setRegion(const DASType::DbKey& region);
		
		virtual void setSubsystemCount( unsigned long count );

		virtual void setSubsystem(const DASType::DbKey& subsystem,
			const SubsystemState::State& addState, const SubsystemState::State& remState );
		
		virtual void setRegionAddCount   ( unsigned long count );
		virtual void setRegionRemoveCount( unsigned long count );

		virtual void regionAdd   ( const DutyPrimitive& session );
		virtual void regionRemove( const DutyPrimitive& session );
		
		virtual void setSubsystemAddCount    ( unsigned long count );
		virtual void setSubsystemRemoveCount ( unsigned long count );

		virtual void subsystemAdd    ( const DutyPrimitive& session );
		virtual void subsystemRemove ( const DutyPrimitive& session );
		virtual void subsystemDeny   ( const DutyPrimitive& session );
		virtual void subsystemTimeout( const DutyPrimitive& session );

		virtual void subsystemComplete();
		virtual void regionComplete();

	private:

		typedef std::map<std::string, DASType::SubsystemMap > InternalRequestMap;

		DASType::DbKey m_region;
		DASType::DbKey m_subsystem;
		DutyPrimitive  m_requestingSession;
		
		InternalRequestMap    m_requestMap;
		DASType::SubsystemMap m_emptySubMap;
		DASType::SubsystemSet m_emptySubSet;
        std::string           m_initiatingAction;

        std::auto_ptr<TA_Base_Core::AuditMessageSender> m_auditSender;

        // If srcIsSender is true, the src session is the sender (originator) of the audit message,
        // otherwise the dst session is the sender.
        void sendAuditMessage(
            const TA_Base_Core::MessageType& messageType,
            const std::string& srcSessionStr,
            const std::string& dstSessionStr,
            bool srcIsSender,                         
            const std::string& subsystemIdList,
            const std::string& regionIdList,
            const std::string& msgDetailsStr ); 

        // Returns a string of the form "left(right)".
        std::string concatStr(const std::string& left, const std::string& right);
		// Returns a string connect left and right using connector
		std::string concatStr(const std::string& left, const std::string& right, const std::string& connector);

		//azenitha++
		struct AuditMessageData //Request to subsystem/s
		{
			std::string srcSession;
			std::string destSession;
			std::string subsystemName;
		};

		struct AuditMessageDataAll //Request to ALL subsystem
		{
			std::string srcSession;
			std::string destSession;
		};

		typedef std::vector<AuditMessageData> AuditMessageDataList; //Request to subsystem/s
		typedef std::vector<AuditMessageDataAll> AuditMessageDataAllList; //Request to ALL subsystem

		AuditMessageDataList m_auditMessageDataList; //Request to subsystem/s
		AuditMessageDataAllList m_auditMessageDataAllList; //Request to ALL subsystem
	
		void clearPendingAuditMessages(); //Request to subsystem/s
		void sendPendingAuditMessages(); //Request to subsystem/s
		void pendingAuditMessageToSend(
			const std::string& srcSession,
			const std::string& destSession,
			const std::string& subsystemName); //Request to subsystem/s

		void clearPendingAuditMessagesAll(); //Request to ALL subsystem
		void sendPendingAuditMessagesAll(); //Request to ALL subsystem
		void pendingAuditMessageToSendAll(
			const std::string& srcSession,
			const std::string& destSession); //Request to ALL subsystem
		//++azenitha
		// TD19417++ Yanrong
		typedef std::string SubsystemName;
		typedef std::string SessionString;
		typedef std::string SubsystemList;
		typedef std::map<SessionString, SubsystemList> SubsystemsMap;
		typedef std::map<SubsystemName, SessionString> SessionsMap;
		SessionsMap m_addedSessions;	// hold all sessions which add subsystem
		SessionsMap m_removedSessions;	// hold all sessions which remove subsystem
		// ++TD19417
	};
}

#endif // !defined(DUTYREQUESTCONVERTER_H_INCLUDED)
