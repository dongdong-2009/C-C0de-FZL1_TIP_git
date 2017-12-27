/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/AuthenticationAgentImpl.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IAuthenticationAgent interface.
 */

#if !defined(AFX_AUTHENTICATIONAGENTIMPL_H__835454DE_F132_45A3_BFF1_6612F3954243__INCLUDED_)
#define AFX_AUTHENTICATIONAGENTIMPL_H__835454DE_F132_45A3_BFF1_6612F3954243__INCLUDED_

#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"
#include "bus/security/authentication_library/src/SessionInfo.h"

#include "core/corba/src/ServantBase.h"

namespace TA_Base_Bus
{
	class GenericAgent;
}

namespace TA_Base_App
{

    class IAuthentication;   //forward declaration
	class AuthenticationAgent;
    class AuthenticationAgentImpl  : public virtual POA_TA_Base_Bus::IAuthenticationAgentCorbaDef,
                                     public virtual TA_Base_Core::ServantBase
    {
    public:
        AuthenticationAgentImpl(IAuthentication& authenticaiton,  AuthenticationAgent* parentClass);
	    virtual ~AuthenticationAgentImpl();

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
        char* requestSession(CORBA::ULong userId, 
                             CORBA::ULong profileId, 
                             CORBA::ULong locationId, 
                             CORBA::ULong workstationId, 
                             const char* password,
							  CORBA::Boolean displayOnly=false);


        /**
         * changePassword
         *
         * This method updates the password for the user of the specified session, and notifies all
         * other Authentication Agents of the change.
         *
         * @param sessionId     The session ID from which the password change request is being made.
         * @param newPassword   The new password to set for the user related to the specified session.
         *
         * @return true if the password change was successful, false otherwise.
         *
         * @exception AuthenticationSecurityException Thrown if the session is not valid.
         * @exception AuthenticationAgentException Thrown if there is a problem changing the password.
         *
         * pre: The password CANNOT be the empty string.
         */
        void changePassword(const char* sessionId, const char* oldPassword, const char* newPassword);


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
        void beginOverride(const char* sessionId, 
                           CORBA::ULong userId, 
                           CORBA::ULong profileId, 
                           CORBA::ULong locationId, 
                           const char* password);


        /**
         * endOverride
         *
         * This method removes any additional profiles associated with the specified session.
         *
         * @param sessionId     The session for which to remove overrides.
         *
         * @exception Thrown if there is a problem removing the overrides.
         */
        void endOverride(const char* sessionId);


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
        char* changeProfile(const char* sessionId, 
                            CORBA::ULong profileId, 
                            CORBA::ULong locationId, 
                            const char* password,
							 CORBA::Boolean displayOnly=false);


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
        TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence* getProfiles(const char* sessionId);


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
         * @exception AuthentiactionAgentException Thrown if there is a problem retrieving the session information.
         */
        TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef* getSessionInfo(const char* sessionId, const char* requestingSessionId);


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
        void endSession(const char* sessionId);


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
        TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence* getActiveSessions(const char* sessionId);

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
		
		void beginDisplayonly(bool display);

	private:
		void checkOperationMode();
    private:

        /**
         * m_authentication
         *
         * A reference to the class that will perform all the work of the Authentication Agent
         */
        IAuthentication& m_authentication;
		AuthenticationAgent* m_parentClass;

    };
}

#endif // !defined(AFX_AUTHENTICATIONAGENTIMPL_H__835454DE_F132_45A3_BFF1_6612F3954243__INCLUDED_)
