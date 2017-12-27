/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/authentication_library/src/AuthenticationLibrary.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// AuthenticationLibrary.h: interface for the AuthenticationLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTHENTICATIONLIBRARY_H__1D9CA11A_5D4A_44CF_9099_0FD424A64B98__INCLUDED_)
#define AFX_AUTHENTICATIONLIBRARY_H__1D9CA11A_5D4A_44CF_9099_0FD424A64B98__INCLUDED_

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <string>
#include <vector>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <time.h>

#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"
#include "bus/security/authentication_library/src/SessionInfo.h"

#include "core/naming/src/NamedObject.h"
#include "core/process_management/IDL/src/IControlStationCorbaDef.h"

namespace TA_Base_Bus
{
	class  AuthenPeerTaskManager;

    typedef TA_Base_Core::NamedObject<TA_Base_Bus::IAuthenticationAgentCorbaDef,
            TA_Base_Bus::IAuthenticationAgentCorbaDef_ptr,
            TA_Base_Bus::IAuthenticationAgentCorbaDef_var> AuthenticationAgentNamedObj;



    class AuthenticationLibrary
    {
    public:

		/**
		 * Constructor
		 *
		 * Constructs an instance of the authentication library, connecting to the named authentication agent.
		 *
		 * @param The name of the authentication agent to contact
		 */
        AuthenticationLibrary(bool isMgr=false);
	    virtual ~AuthenticationLibrary();

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
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
	     */
        std::string requestSession(unsigned long userId, 
                                   unsigned long profileId, 
                                   unsigned long locationId, 
                                   unsigned long workstationId, 
                                   const std::string & password,
								   bool display = false);


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
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
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
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
         */
	    void beginOverride(const std::string & sessionId, 
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
         * @exception AuthenticationAgentException Thrown if there is a problem removing the overrides.
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
         */
	    void endOverride( const std::string & sessionId);


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
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
         */
        std::string changeProfile(const std::string & sessionId, 
                                  unsigned long profileId, 
                                  unsigned long locationId, 
                                  const std::string & password,
								  bool display = false);


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
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
	     */
        void getProfiles(const std::string & sessionId, std::vector<unsigned long> & vtProfile );
		//obsolete function
        std::vector<unsigned long> getProfiles(const std::string & sessionId );

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
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
         */
        SessionInfo getSessionInfo(const std::string & sessionId, const std::string & requestingSessionId);


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
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
         */
        void endSession( const std::string & sessionId);




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
         * @exception ObjectResolutionException Thrown if there is an resolving the authentication agent
         */
        void getActiveSessions(const std::string & sessionId, std::vector<SessionInfo> & vtSessionInfo);

        void getActiveSessionsForOneLocation(const std::string & noUseSessionId, AuthenticationAgentNamedObj & authAgent,  std::vector<SessionInfo> & vtSessionInfo);
        
		//obsolete function
		std::vector<SessionInfo> getActiveSessions(const std::string & sessionId);

        static SessionInfo convertCORBASessionToCSession( const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& corbaSession);
		static TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef convertCSessionToCORBASession( const SessionInfo& cSession);

		AuthenPeerTaskManager * getAuthenPeerTaskManager();

		void dumpSessionInfo( std::vector<SessionInfo> & vtSessionInfo );


		/**
		* GetHash
		*
		* This is called to encrypt the CString composed by username/password
		*
		* @return The data is retunred as an int(0 indicate a successful
		*
		*/
		static int GetHash(const std::string& data, std::string& results);

    private:
        AuthenticationLibrary(const AuthenticationLibrary& theAuthenticationLibrary);
        AuthenticationLibrary& operator=(const AuthenticationLibrary& rhs);

        void throwSecurityException(const std::string & description,TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason reason);

        // TD12427: Before requesting a session make sure an equivalent one does not already exist. Note
        // that only operator ID, profile ID, and workstation ID are checked as that is all the
        // authentication agent checks when it looks for dead sessions.
        std::string findEquivalentSessionNotUsingSessionId(const TA_Base_Bus::SessionInfo& session);
		
		SessionInfo getSessionInfoFromDb( const std::string & sessionId );

		static bool isVectorEqual(const std::vector<unsigned long> & vtFirst, const std::vector<unsigned long> & vtSecond );

		void EnsureSpecificCondition(bool fCondition, char * msgStr );
		void updateOverrideInfoToDb( const std::string & sessionId, unsigned long userOverrideId, unsigned long profileOverrideId );
		void updateOverrideInfoToDb(const std::string & sessionId);
		unsigned long	getActualLocationKey(unsigned long sourceLocationKey);
	private:
		AuthenPeerTaskManager * m_pAuthenPeerTaskManager;

		
    };
}

#endif // !defined(AFX_AUTHENTICATIONLIBRARY_H__1D9CA11A_5D4A_44CF_9099_0FD424A64B98__INCLUDED_)
