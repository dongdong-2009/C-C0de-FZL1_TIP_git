/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/SessionRegister.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class is contains the list of current sessions. It provides thread-safe
 * access to that list (as the list can be updated from more than one place), and
 * stores the profiles associated with a given session with the session key.
 */


#if !defined(SessionRegister_976BE560_D504_4fb6_9DBD_51F2DF6FF623__INCLUDED_)
#define SessionRegister_976BE560_D504_4fb6_9DBD_51F2DF6FF623__INCLUDED_

#include "bus/security/authentication_library/src/SessionInfo.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <map>
#include <set>

namespace TA_Base_App
{

    class SessionRegister
    {


    public:

        /**
         * constructor
         *
         * Constructs a new session register object. It "does" nothing.
         */
        SessionRegister( bool isControl, unsigned long entityKey );

        
        /**
         * destructor
         */
        virtual ~SessionRegister();


        /**
         * addSession
         *
         * Adds a session to the register.
         *
         * @param session The session to add to the register
         * @param addActiveTime Specifies whether the session should be added to the active times map.
         *
         * @exception AuthenticationAgentException Thrown if the session is already on the register.
         */
        void addSession( const TA_Base_Bus::SessionInfo & session, bool addActiveTime = false);


        /**
         * removeSession
         *
         * Removes the specifed session from the register of currently valid sessions.
         *
         * @param sessionId The sessionId of the session that is to be removed.
         *
         * @exception AuthenticationSecurityException Thrown if the specified session is not currently valid.
         */
        void removeSession(const std::string& sessionId);


        /**
         * addOverride
         *
         * Associates the specified operator and profile with the specified session Id.
         *
         * @param    sessionId  The ID of the session to which the profile is to be added
         * @param    operatorId The operator to add to the session
         * @param    profileId  The profile to add to the session
         *
         * @exception AuthenticationAgentException Thrown if the specified session is not currently valid.
         */
        void addOverride(const std::string& sessionId, unsigned long operatorId, unsigned long profileId);


        /**
         * clearOverrides
         *
         * Removes all but the first operator and profile associated with the specified session Id.
         *
         * @param    sessionId The session ID for which to remove additional profiles.
         *
         * @exception AuthenticationAgentExcetpion Thrown if the specified session is not currently valid.
         */
        void clearOverrides(const std::string& sessionId);


        /**
         * isSessionInRegister
         *
         * Returns true if the session exists in the register.
         *
         * @param sessionId The session ID to search for.
         *
         * @return bool True if the session is in the register.
         */
        bool isSessionInRegister( const std::string& sessionId ) const;


        /**
         * getSession
         *
         * Retrieves the information about the specified session.
         *
         * @param sessionId The sessionId for the session about which to retrieve information.
         *
         * @return The information about the specified session in a SessionInfo struct.
         *
         * @exception AuthenticationAgentException Thrown if the specified session is not currently valid.
         */
        const TA_Base_Bus::SessionInfo& getSession(const std::string& sessionId);


        /**
         * getProfiles
         *
         * Returns a vector of all profile IDs associated with the specified session Id.
         *
         * @param    sessionId The sessionId of the session for which to retrieve the profiles
         *
         * @return The list of profiles associated with the session as a vector. The first profile in 
         * the vector (i.e. index [0]) is the "base" profile, that is, the one the user originally log into.
         *
         * @exception AuthenticationAgentException Thrown if the specified session is not currently valid.
         */
        const std::vector<unsigned long>& getProfiles(const std::string& sessionId) const;


        /**
         * getRegister
         *
         * Retrieves the contents of the SessionRegister, and converts it to a vector for data update purposes.
         *
         * @return the contents of the SessionRegister as a vector of SessionInfo objects.
         */
        void getRegister(std::vector<TA_Base_Bus::SessionInfo> & vtSessinInfo) const;


        /**
         * addSessionActiveTime
         *
         * Adds an entry into the map of session active times and initialises it to the current
         * time on the system running the agent.
         *
         * @param sessionId The session to add to the map of session active times.
         */
        void addSessionActiveTime(const std::string& sessionId);


        /**
         * updateSessionActiveTime
         *
         * Updates the last known time that the session was active.
         *
         * @param   timestamp The last known time
         * @param   sessionId The session Id of the active session
         *
         * @exception AuthenticationAgentException Thrown if the specified session is not currently valid.
         */
        void updateSessionActiveTime(time_t timestamp, const std::string& sessionId);

        /**
         * updateAllSessionsActiveTime
         *
         * Updates all the sessions to current server time.
         *
         */
        void updateAllSessionsActiveTime();
        

        /**
         * removeSessionActiveTime
         *
         * Removes an entry from the map of session active times.
         *
         * @param sessionId The session to remove from the map of session active times.
         */
        void removeSessionActiveTime(const std::string& sessionId);


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

    private:
		TA_Base_Bus::SessionInfo getSessionInfoFromDb( const std::string & sessionId ) const;
		bool getOwnedSessionInfoFromDb( const std::string& sessionId, TA_Base_Bus::SessionInfo& outputSession);
		bool getOwnedSessionInfoFromDb( const std::string& sessionId, TA_Base_Bus::SessionInfo& outputSession,bool& hasEnded);

		bool existInPreOwnedSession( const std::string & sessionId) const
		{
			return m_preOwnedSessions.find(sessionId) != m_preOwnedSessions.end();
		}
        /**
         * Copy Constructor
         *
         * Hidden so it will not be used accidently.
         */
        SessionRegister( const SessionRegister& theSessionRegister);

        /**
         * operator=
         *
         * Hidden so it will not be used accidently.
         */
        SessionRegister& operator=(const SessionRegister& rhs);

        /**
         * m_sessionRegister
         *
         * The map that "is" the session register.
         */
        typedef std::map<std::string,TA_Base_Bus::SessionInfo > TheRegister;
        TheRegister m_sessionRegister;

        /**
         * m_sessionActiveTimes
         *
         * Stores the last time that the session was known to be active. If a session
         * appears in this map then it will be a candiate for termination if any
         * inactivity is detected.
         */
        std::map<std::string, time_t> m_sessionActiveTimes;

        /**
         * m_preOwnedSessions
         *
         * Stores all the sessions that, according to the database, are still active in the
         * system. If a session is found in this set when it is added to the register it will
         * also be added to the session active times map.
         */
        std::set<std::string> m_preOwnedSessions;

        /**
         * m_registerAccessLock
         *
         * Used to guard access to the session register.
         */
        mutable TA_Base_Core::ReEntrantThreadLockable m_registerAccessLock;

        /**
         * m_activeTimesAccessLock
         *
         * Used to guard access to the session active times map.
         */
        TA_Base_Core::ReEntrantThreadLockable m_activeTimesAccessLock;


		// ly++ for CL-20784, date:20090420
	public:
		/**
		* appendGhostSession
		*
		* retrieve DB session to a session lis. then you can judge them as active session
		* Used by  DeadSessionCheckerThread in each run loop
		*
		* @param	
		*/
		void appendGhostSession( std::vector< TA_Base_Bus::SessionInfo > &sessionList ) const;

		/**
		* isGhostSession
		*
		* return true if session is in ghost session list
		*
		* @param	sessionId	The session Id you want to judge
		* @return	true for ghost session, false for not
		*/
		inline bool isGhostSession( const std::string &sessionId ) const
		{
			TA_Base_Core::ThreadGuard guard( m_ghostSessionLock );
			TheRegister::const_iterator it = m_ghostSessionMap.find( sessionId );
			return ( it == m_ghostSessionMap.end() ? false : true );
		}

		/**
		* getGhostSessionInfo
		*
		* Retrieve ghost session info using session id from m_ghostSessionMap
		*
		* @param	sessionId	The session Id you want to get SessionInfo
		* @param	sessionInfo	SessionInfo struct that will fill by result
		* @return	true if sessionInfo has been update, false for not found sessionId in m_ghostSessionMap
		*/
		bool getGhostSessionInfo( const std::string &sessionId, TA_Base_Bus::SessionInfo &sessionInfo ) const;

		/**
		* removeGhostSession
		*
		* Remove session in m_ghostSessionMap
		* 
		* @param	sessionId	The session Id want to remove from ghost session list
		*/
		void removeGhostSession( const std::string &sessionId );

		/**
		 * updateGhostSessionAliveTime
		 *
		 * Set Ghost session alive Time
		 * 
		 * @param	aliveSecond	The alive for Ghost Session in DeadSessionCheckerThread in second, default 60 seconds
		 */
		void updateGhostSessionAliveTime( unsigned long timeout, unsigned long aliveSecond = 60 );

		void moveGhostSessionToRegister( const std::string &sessionId );
		void loadOvrrideInfomation( const std::string &sessionId,TA_Base_Bus::SessionInfo& session);
	private:
		// store DB session which has_ended flag is false
		TheRegister m_ghostSessionMap;
		mutable TA_Base_Core::ReEntrantThreadLockable m_ghostSessionLock;
		unsigned long m_entityKey;
		// ++ly
    };
}

#endif // !defined(SessionRegister_976BE560_D504_4fb6_9DBD_51F2DF6FF623__INCLUDED_)
