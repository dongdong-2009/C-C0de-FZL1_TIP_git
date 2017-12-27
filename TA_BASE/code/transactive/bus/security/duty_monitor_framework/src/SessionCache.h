#if !defined(_SESSIONCACHE_H__2FAC2CA3_A0CC_4874_922F_4AB20E91184E__INCLUDED_)
#define _SESSIONCACHE_H__2FAC2CA3_A0CC_4874_922F_4AB20E91184E__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/SessionCache.h $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/29 10:20:41 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This class stores the details of active operator sessions.
  * If queries are made on sessions that are not currently stored, the Authentication Agent
  * is contacted and the sessions added to the cache.
  * SessionCache subscribes to Authentication Agent messages and adjusts its contents accordingly.
  * After the cache has been updated, the updates are passed onto attached observers.
  *
  */
#include <string>

namespace TA_Base_Bus
{
    class SessionCache : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
                         public SeSubject<SessionCache>
    {
    public:
        SessionCache();
        virtual ~SessionCache();

        /**
          * Returns the ID of the current session.
          *
          */
        SessionId getCurrentSessionId();

        /**
          * Returns the IDs of all currently active sessions.
          * If ignoreSystemOperators on DutyDataStoreSingleton has been
          * enabled, the list will contain session IDs for human operators only.
          *
          */
        SessionIdList getSessionIds();

        /**
          * Returns true if the specified session ID denotes a cached session.
          *
          */
        bool isKnownSession(const SessionId& sessionId);

        /** 
          * Returns true if the specified session ID denotes a session associated
          * with a system operator (agent).
          * Throws a DutyMonitorFrameworkException if the ID does not denote a valid session
          * or if there are problems communicating with the Authentication Agent.
          *
          */
        bool isSystemOperatorSession(const SessionId& sessionId);

		/**
		  * Returns true if the specified session ID contains a central profile.
		  * Throws a DutyMonitorFrameworkException if the ID does not denote a valid session
		  * or if there are problems communicating with the Authentication Agent.
		  *
		  */
		bool isContainCentralProfile(const SessionId& sessionId);

        /**
          * Returns details of the session with the specified ID. 
          * Throws a DutyMonitorFrameworkException if the ID does not denote a valid session
          * or if there are problems communicating with the Authentication Agent.
          *
          */
        SessionInfoEx getSessionInfo(const SessionId& sessionId);

        /**
          * Returns details of the session with the specified key. 
          * Throws a DutyMonitorFrameworkException is the key does not denote a valid session.
          *
          */
        SessionInfoEx getSessionInfo(SessionKey sessionKey);

        /**
          * Returns session update generated as the result of an Auth Agent comms message.
          * NULL when there is no update to be picked up.
          *
          */
        const SessionUpdate* getSessionUpdate();       

        /**
          * TA_Base_Core::SpecialisedMessageSubscriber interface
          *
          */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

    private:
        SessionCache(const SessionCache& sessionCache);
        SessionCache& operator=(const SessionCache& sessionCache);

        void setCurrentSessionId(const SessionId& sessionId);
        SessionKey addOrUpdateSession(const SessionInfo& sessionInfo);
        SessionKey addOrUpdateSession(const SessionInfoCorbaDef& sessionInfoCorbaDef);
        SessionKey removeSession(const SessionInfoCorbaDef& sessionInfoCorbaDef);
        SessionKey getNextSessionKey();        

   
    private:
        typedef std::map<SessionId, SessionInfoEx> SessionInfoMap;
        typedef std::map<SessionKey, SessionId> SessionKeyMap;

        typedef struct
        {
            SessionId sessionId;
            OperatorKey userId;
            WorkstationKey workstationId;
        } CurrentSessionDetail;

    
    private:
        bool m_sessionsLoaded;
        AuthenticationLibrary m_authLibrary;

        CurrentSessionDetail m_currentSessionDetail;
        TA_Base_Core::ReEntrantThreadLockable m_currentSessionIdLock;

        SessionKey m_nextSessionKey;
        TA_Base_Core::ReEntrantThreadLockable m_nextSessionKeyLock;
 
		typedef std::map<SessionId, SessionId> GhostSessionInfoMap;
		
        GhostSessionInfoMap  m_ghostSession;
        SessionInfoMap m_sessionInfoMap;
        SessionKeyMap m_sessionKeyMap;
        TA_Base_Core::ReEntrantThreadLockable m_sessionMapLock;

        SessionUpdate* m_sessionUpdate;
        TA_Base_Core::NonReEntrantThreadLockable m_sessionUpdateLock;
		static const std::string IGNORE_GHOST_SESSION;
    };
}

#endif // !defined(_SESSIONCACHE_H__2FAC2CA3_A0CC_4874_922F_4AB20E91184E__INCLUDED_)
