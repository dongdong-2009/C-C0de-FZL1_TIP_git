#ifndef AA_MESSAGE_SENDER_HEADER_INCLUDED
#define AA_MESSAGE_SENDER_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Mahaveer Pareek
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */
//

#include <string>
#include <vector>

#include "ace/Singleton.h"

namespace TA_Base_Core
{
    class MessageType;
    class AuditMessageSender;
    struct NameValuePair;
}

namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "AAMessageSender" and that "AAMessageSenderImpl" can
	// not be used.

	class AAMessageSenderImpl
	{
	// Public methods
	public:
		friend class ACE_Singleton<AAMessageSenderImpl, ACE_Recursive_Thread_Mutex>;

        void submitAuditPlanUnapproval( const std::string& strSession, const std::string& strName );
        void submitAuditPlanApproval( const std::string& strSession, const std::string& strName );
        void submitAuditPlanUnapprovalFailure( const std::string& strSession, const std::string& strName );
        void submitAuditPlanApprovalFailure( const std::string& strSession, const std::string& strName );
        void submitAuditCategoryInsert( const std::string& strSession, const std::string& strName, const std::string& strParentName );
        void submitAuditCategoryDelete( const std::string& strSession, const std::string& strName );
        void submitAuditCategoryUpdate( const std::string& strSession, const std::string& strName, const std::string& strNewName );
        void submitAuditPlanDeleteFailure( const std::string& strSession, const std::string& strPlanName );
        void submitAuditPlanCopyFailure( const std::string& strSession, const std::string& strPlanName, const std::string& strNewName );
        void submitAuditPlanCopy( const std::string& strSession, const std::string& strPlanName, const std::string& strNewName );
        void submitAuditPlanMove( const std::string& strSession, const std::string& strPlanName, const std::string& strNewName );
        void submitAuditPlanInsert( const std::string& strSession, const std::string& strPlanName );
        void submitAuditPlanDelete( const std::string& strSession, const std::string& strPlanName );
        void submitAuditPlanUpdate( const std::string& strSession, const std::string& strPlanName );
        void submitAuditEditLocklRelease( const std::string& strSession, const std::string& strPlanName );
        void submitAuditEditLocklAcquire( const std::string& strSession, const std::string& strPlanName );
        void submitAuditControlAcquire( const std::string& strSession, const std::string& strPlanName, 
            const std::string& strOperator, const unsigned long ulInstanceNum );
        void submitAuditStepCustomize( const std::string& strSession, const std::string& strPlanName, 
            const unsigned long ulInstanceNum, const unsigned long ulStepNum );
        void submitAuditPlanExecution( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditPlanResumeFailure( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditPlanPauseFailure( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditPlanStopFailure( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditPlanResume( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditPlanPause( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditPlanStop( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditPlanPending( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditAutoGrcStarted( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditManualGrcStarted( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditOperatorLog( const std::string& strSession, const std::string& strPlanName, 
            const std::string& strMessage, const unsigned long ulInstanceNum );
        void submitAuditStepExecutedFailure( const std::string& strSession, const std::string& strPlanName, 
            const std::string& strReason, const unsigned long ulInstanceNum, const unsigned long ulStepNum );
        void submitAuditPlanCompleted( const std::string& strSession, const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditAutoGrcFailed( const std::string& strSession, const std::string& strPlanName, 
            const std::string& strReason, const unsigned long ulInstanceNum, const unsigned long ulStepNum );
        void submitAuditManualGrcFailed( const std::string& strSession, const std::string& strPlanName, 
            const std::string& strReason, const unsigned long ulInstanceNum, const unsigned long ulStepNum );
        void submitAuditAutoGrcSucceed( const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditManualGrcSucceed( const std::string& strPlanName, const unsigned long ulInstanceNum );
        void submitAuditManualGrcBadLaunch( const std::string& strEquipDesc, const std::string& strLcc );
        void submitAuditManualGrcBadReturn( const std::string& strEquipDesc, const std::string& strRcc );

        std::string raiseAlarmStepExecutedFailure( const std::string& strPlanName, const std::string& strReason, 
            const unsigned long ulInstanceNum, const unsigned long ulStepNum );


        // Returns the ID of the generated alarm
		std::string raiseAlarm(const TA_Base_Core::MessageType& alarmType, const std::vector<TA_Base_Core::NameValuePair*>& description);

        void closeAlarm(const std::string& alarmId);

        void deleteMessageSenders();

        void enable( const bool bEnable = true );

	// Private methods
	private:
		AAMessageSenderImpl();
		~AAMessageSenderImpl();

		AAMessageSenderImpl( const AAMessageSenderImpl& );
        AAMessageSenderImpl& operator=( const AAMessageSenderImpl& );

        void sendAuditMessage(const std::string& session, const TA_Base_Core::MessageType& auditMessageType, const std::vector<TA_Base_Core::NameValuePair*>& description);
   
    // Private data
    private:
        bool m_bEnable;
		TA_Base_Core::AuditMessageSender* m_auditMessageSender;

	}; // AAMessageSenderImpl

	typedef ACE_Singleton<AAMessageSenderImpl, ACE_Recursive_Thread_Mutex> AAMessageSender;

} // TA_Base_App

#endif //AA_MESSAGE_SENDER_HEADER_INCLUDED
