/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/Authentication.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IAuthentication interface. It will service all incomming CORBA requests.
 */


#if !defined(Authentication_E5F65BA4_C07C_4e19_9B5A_973AAD80B752__INCLUDED_)
#define Authentication_E5F65BA4_C07C_4e19_9B5A_973AAD80B752__INCLUDED_

#include "core/message/types/MessageTypes.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/src/CommsMessageSender.h"

#include "app/security/authentication_agent/src/DeadSessionCheckerThread.h"

#include "app/security/authentication_agent/src/IAuthentication.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "app/security/authentication_agent/src/SessionQueue.h"
#include "app/security/authentication_agent/src/SessionQueueProcessor.h"
#include "app/security/authentication_agent/src/UserCache.h"

#include "bus/security/duty_agent/IDL/src/IDutyAgentAuthCorbaDef.h"
#include "bus/security/rights_agent/IDL/src/IRightsAgentCorbaDef.h"

#include "core/naming/src/NamedObject.h"

namespace TA_Base_App
{

    static const char* RPARAM_SESSION_CHECKING_FREQUENCY = "SessionCheckingFrequency";
    static const char* RPARAM_SESSION_TIMEOUT_PERIOD = "SessionTimeoutPeriod";

    class SessionRegister;
    class AuthenticationMessageReceiver;
    class DeadSessionCheckerThread;

    class Authentication : public IAuthentication
    {

    public:
	    Authentication(SessionQueue& queue, SessionQueueProcessor& queueProcessor, 
                        bool isControl, unsigned long entityKey);
	    virtual ~Authentication();

    public:

        /**
         * constants
         *
         * Constants used for distributed message sending, and checking the length of new passwords.
         */
        static const std::string AGENT_ID;
        static const std::string AGENT_ID_STRING;
        static const std::string AGENT_NAME;
        static const int MIN_PASSWORD_LENGTH;
        static const int MAX_PASSWORD_LENGTH;

        

	    /**
         * requestSession
         *
	     * This method checks the supplied credentials and creates sessions if they are sufficient.
         *
	     * @param    userId         The key to the user id in the operation table
	     * @param    profileId      The key to the profile the user is logging in under
	     * @param    locationId     The key to the location the user wants to use the profile for
	     * @param    workstationId  The key to the workstation from which the user is attempting to gain access
	     * @param    password       The password with which the user is attempting to log in.
         *
         * @return The ID of the new session as a string
         *
         * @exception AuthenticationSecurityException Thrown if the log in attpemt fails for security reasons.
         * @exception AuthenticationAgentException Thrown if there is a general error with the
         * log in attempt.
	     */
        std::string requestSession(unsigned long userId, 
                                   unsigned long profileId, 
                                   unsigned long locationId, 
                                   unsigned long workstationId, 
                                   const std::string & password,
								   bool displayOnly=false);

        /**
         * changePassword
         *
         * This method updates the password for the user of the specified session, and notifies all
         * other Authentication Agents of the change.
         *
         * @param sessionId     The session ID from which the password change request is being made.
         * @param oldPassword   The users current password.
         * @param newPassword   The new password to set for the user related to the specified session.
         *
         * @return true if the password change was successful, false otherwise.
         *
         * @exception AuthenticationSecurityException Thrown if the session is not valid.
         * @exception AuthenticationAgentException Thrown if there is a problem changing the password.
         *
         * pre: The password CANNOT be the empty string.
         */
	    void changePassword(const std::string & sessionId, const std::string & oldPassword, const std::string & newPassword);

        /**
         * beginOverride
         *
         * Associates the profile the overriding user is logging into with the session of the base user.
         *
         * @param sessionId      The session that is being overriden
         * @param userId         The key to the user id in the operation table
	     * @param profileId      The key to the profile the user is logging in under
	     * @param locationId     The key to the location the user wants to use the profile for
	     * @param password       The password with which the user is attempting to log in.
         *
         * Note: The workstation ID is not a parameter, as it is taken from the passed in session ID.
         *
         * @return true if the override was successful
         *
         * @exception AuthenitcationSecurityException Thrown if the override is denied for seucrity reasons.
         * @exception AuthenticationAgentException Thrown if there is a probelm initaiting override.
         */
	    void beginOverride(const std::string &sessionId, 
                           unsigned long userId, 
                           unsigned long profileId, 
                           unsigned long locationId, 
                           const std::string & password);

        /**
         * endOverride
         *
         * This method removes any additional profiles associated with the specified session.
         *
         * @param sessionId     The session for which to remove overrides.
         *
         * @exception Thrown if there is a problem removing the overrides.
         */
	    void endOverride(const std::string & sessionId);

        /**
         * changeProfile
         *
         * This method moves the operator logged into the specified session to a new profile, with a new session ID. This method
         * creates the new session before ending the old one, to allow the operator to continue using their current session of the
         * log in is denied.
         *
         * @param sessionId     The session the operator is currently using.
         * @param profileId     The ID of the new profile the operator wishes to change to.
         * @param locationId    The ID of the location for the new profile
         * @param password      The password for the new user.
         *
         * @return The ID of the new session as a string
         *
         * @exception AuthenitcationSecurityException Thrown if the override is denied for seucrity reasons.
         * @exception AuthentiactionAgentException Thrown if there is a problem logging the operator into the new session.
         */
        std::string changeProfile(const std::string & sessionId, unsigned long profileId, unsigned long locationId, const std::string & password, bool displayOnly=false);


	    /**
         * getProfiles
         *
	     * Checks the supplied session Id to determine if it is a current session (that is, not yet expired), and if it is, 
         * will return the profiles associated with the session.
         *
	     * @param    sessionId    The session Id to check for validity.
         *
         * @return A vector of profileId's representing the profiles associated with the session. If the vector is empty
         * (i.e. vector.size() == 0), the session is not currently valid.
         *
         * @exception AuthenticationAgentException Thrown if there is a problem retrieving the profile IDs.
	     */
        void getProfiles(const std::string & sessionId, std::vector<unsigned long> & vtProfile);


        /**
         * getSessionInfo
         *
         * This method retrieves information about the requested session. It should be used by applications wishing
         * to determine session information, rather than going directly to the security tables in the database.
         *
         * @param sessionId             The ID of the session for which to retrieve information
         * @param requestingSessionId   The ID of the session making the request.
         *
         * @return The information about this session in a SessionInfo struct.
         *
         * @exception AuthenticationSecurityExcetpion Thrown if either session is not valid.
         * @exception AuthentiactionAgentException Thrown if there is a problem retrieving the session information.
         */
        void getSessionInfo(const std::string & sessionId, const std::string & requestingSessionId, TA_Base_Bus::SessionInfo & sessionInfo);


        /**
         * endSession
         *
         * Ends the specified session, by removing it from the session register. After this method has been called,
         * any calls to getProfiles() will return an empty vector.
         *
         * @param sessionId     The ID of the session to end.
         *
         * @exception AuthenticationSecurityExcetpion Thrown if the session is not valid.
         * @exception AuthenticationAgentExcetpion  Thrown if there is a problem ending the session.
         */
        void endSession(const std::string & sessionId);


        /**
         * getActiveSessions
         *
         * Retrieves a list of all the currently active sessions in the system.
         *
         * @param sessionId     The ID of the session that is retrieving all others.
         *
         * @return the list of all currently active sessions as a vector of SessionInfo objects
         *
         * @exception AuthenticationAgentExcetpion  Thrown if there is a problem ending the session.
         */
        void getActiveSessions(const std::string & sessionId, std::vector<TA_Base_Bus::SessionInfo> & vtSessinInfo );

        /**
         * keepSessionAlive
         *
         * Informs the Authentication Agent that a session is still alive at a specific time.
		 * This should be called periodically while a session is active.
         *
		 * @param timestmp		Last time known that session was still active (current time)
         * @param sessionId     The ID of the session that is retrieving all others.
         *
         * @exception AuthenticationAgentException  Thrown if session is not valid.
         */
		void keepSessionAlive(time_t timestamp, const char* sessionId);

        /**
         * registerUpdate
         *
         * Adds a session to the register. This is for internal use only - for state updates.
         */
	    void registerUpdate(const TA_Base_Bus::SessionInfo & session);

        /**
         * startObjects
         *
         * Instructs the class to start all objects associated with it (e.g. threaded objects,
         * message listeners, etc)
         *
         */
        void startObjects();

        /**
         * getLastSessionActiveTime
         *
         * Retrieves the last known timestamp for a session. It will return false if the session does not
		 * (for whatever reason) have an "active time". This method used to throw an exception for
		 * this case but as the case was common, this was a bad thing.
         *
         * @param   sessionId  The session Id of the active session.
         * @param   activeTime The timestamp.
         * @return  whether the timestamp is a valid one.
         *
	     */
        bool getLastSessionActiveTime(const std::string& sessionId, time_t& activeTime, time_t agentStartUpTime);

        void setControl();

        void setMonitor();

        void updateSessionTime(const TA_Base_Bus::SessionInfo & session, bool isRemove);

        void updateAllSessionsActiveTime();

		SessionRegister & getSessionRegister()
		{
			return m_sessionRegister;
		}
		
		void beginDisplayonly(bool display);

    private:

        /**
         * checkUserSecurity
         *
         * Performs the user security checks - that the user is permitted to use the profile, that
         * the profile is permitted in the given location, and that the users password is correct.
         *
         * @param user The deatils of the user for which to check security
         * @param profileId The id number of the profile the user is attempting to log into.
         * @param locationId The id number of the location the user is attempting to log into.
         * @param password The password the user is attemtping to log on with.
         *
         * @return The name of the profile the user has logged on as.
         *
         * @exception AuthenticationSecurityExcetpion Thrown if there is a mis-match between user and profile
         * or profile and location.
         */
        std::string checkUserSecurity(UserCache::UserDetails& user,unsigned long profileId, unsigned long locationId,
            const std::string& password);


        /**
         * endOldDeadSessions
         *
         * Checks to see if the combination of user/profile/workstation currently exists in the session register.
         * If it does, the old session is ended.
         *
         * @param user The deatils of the user for which to check security
         * @param profileId The profile the user is logging in under
         * @param workstationId The workstation the user is logging on to.
         */
        void endOldDeadSessions(UserCache::UserDetails& user,unsigned long profileId,unsigned long workstationId);

        enum EUpdateType
        {
            SessionStart,
            SessionEnd,
            ProfileUpdate,
            BeginOverride,
            EndOverride
        };

        /**
         * sendSessionNotification
         *
         * Sends the message that will be received by all other distributed authentication agents of a change to the session register.
         *
         * @param session The session details that are to be sent
         * @param user The details of the user that is carrying out the action. This enables the event to include string details.
         * @param updateType The type of update the message is going to carry.
         */
        void sendSessionNotification(const TA_Base_Bus::SessionInfo & session, EUpdateType updateType);

        void sendStateUpdateMessage(const TA_Base_Bus::SessionInfo & session,EUpdateType updateType);

        void sendSessionStateAuditMessage(const std::string& sessionId,const std::string& operatorName,
            const std::string& consoleName, const std::string& profileName,
			const TA_Base_Core::MessageType& updateType);

		void sendChangeProfileCommMessage(const TA_Base_Bus::SessionInfo & session,EUpdateType updateType);

        void sendChangePasswordAuditMessage(const std::string& sessionId);

        void sendOverrideAuditMessage(const std::string& sessionId,const std::string& supervisorName,
            const std::string& operatorName, const std::string& profileName,
			const TA_Base_Core::MessageType& messageType);

        std::string getWorkstationName(unsigned long workstationId);

        std::string getProfileName(unsigned long profileId);

        std::string getUserName(unsigned long userId);
		
		inline void ensureIsLocalLocationId( unsigned long locationId );

		void checkWorkstationExist(unsigned long workStationId );
	private:
        
	    UserCache m_userCache;
	    
	    SessionQueue& m_sessionQueue;
        SessionQueueProcessor& m_sessionQueueProcessor;
	    SessionRegister m_sessionRegister;
		//AuthenticationMessageReceiver m_messageReceiver;
        DeadSessionCheckerThread m_deadSessionCheckerThread;

        unsigned long m_entityKey;
		unsigned long m_localLocationId;

        TA_Base_Core::NamedObject<TA_Base_Bus::IDutyAgentAuthCorbaDef,
			TA_Base_Bus::IDutyAgentAuthCorbaDef_ptr,
			TA_Base_Bus::IDutyAgentAuthCorbaDef_var> m_dutyAgent;

		// yanrong++
		// RihgtsAgent named object: use for synchronizing session update when session changed.
		TA_Base_Core::NamedObject<TA_Base_Bus::IRightsAgentCorbaDef,
			TA_Base_Bus::IRightsAgentCorbaDef_ptr,
			TA_Base_Bus::IRightsAgentCorbaDef_var> m_rightsAgent;
		// ++yanrong

		std::auto_ptr<TA_Base_Core::StateUpdateMessageSender>  m_localStateSender;
		std::auto_ptr<TA_Base_Core::CommsMessageSender>        m_bcastStateSender;
		std::auto_ptr<TA_Base_Core::AuditMessageSender>        m_auditSender;
		std::auto_ptr<TA_Base_Core::ConfigUpdateMessageSender> m_configSender;

		static const std::string AUDIT_MSG_CONSOLE_NAME;
		static const std::string AUDIT_MSG_OPERATOR_ID;
		static const std::string AUDIT_MSG_PROFILE_ID;
		static const std::string AUDIT_MSG_SUPERVISOR_ID;

		bool isDisplayOnlyMode;
    };
}

#endif // !defined(Authentication_E5F65BA4_C07C_4e19_9B5A_973AAD80B752__INCLUDED_)
