/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/SessionCache.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/03/10 10:15:27 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This class stores the details of active operator sessions.
  * If queries are made on sessions that are not currently stored, the Authentication Agent
  * is contacted and the sessions added to the cache.
  * SessionCache subscribes to Authentication Agent messages and adjusts its contents accordingly.
  * After the cache has been updated, the updates are passed onto attached observers.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

namespace
{
    const TA_Base_Bus::SessionKey INVALID_SESSION_KEY(-1);
}

using namespace TA_Base_Bus;


const std::string SessionCache::IGNORE_GHOST_SESSION="IgnoreGhostSession";
const unsigned long CENTRAL_PROFILE_TYPE(1); //1 is central profile //TD15533
SessionCache::SessionCache() :
    m_sessionsLoaded(false),
    m_nextSessionKey(0),
    m_sessionUpdate(NULL)
{
    FUNCTION_ENTRY("SessionCache()");

    try
    {
        // Get the console and operator associated with the current session. 
        // These will be useful when deciding whether a session begin update relates
        // to a profile change by current operator.
        SessionId currentSessionId(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
        SessionInfo sessionInfo(m_authLibrary.getSessionInfo(currentSessionId, currentSessionId));

        m_currentSessionDetail.sessionId = sessionInfo.SessionId;
        m_currentSessionDetail.userId = sessionInfo.UserId;
        m_currentSessionDetail.workstationId = sessionInfo.WorkstationId;

		addOrUpdateSession(sessionInfo);
    }
    catch (...)
    {
        // Do nothing. We'll try to extract the details when getSessionIds() is invoked.
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"failed to get session info of session: %s", m_currentSessionDetail.sessionId.c_str());
    }

    // Register for operator login/logout/override updates
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::AuthenticationAgentStateUpdateBroadcast::AuthenticationAgentDistributedUpdate, this, NULL);

    FUNCTION_EXIT;
}


SessionCache::~SessionCache()
{
    FUNCTION_ENTRY("~SessionCache()");

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    TA_Base_Core::ThreadGuard sessionMapGuard(m_sessionMapLock);

    m_sessionInfoMap.clear();
    m_sessionKeyMap.clear();

    TA_Base_Core::ThreadGuard sessionUpdateGuard(m_sessionUpdateLock);

    delete m_sessionUpdate;
    m_sessionUpdate = NULL;

    FUNCTION_EXIT;
}


SessionIdList SessionCache::getSessionIds()
{
    FUNCTION_ENTRY("getSessionIds()");

    TA_Base_Core::ThreadGuard sessionMapGuard(m_sessionMapLock);
    
    TA_ASSERT(m_sessionInfoMap.size() == m_sessionKeyMap.size(), "Session info map / key map size mismatch");

    // If necessary talk to the Authentication Agent to get the list of current sessions.
    if (!m_sessionsLoaded)
    {
        try
        {
            typedef std::vector<SessionInfo> SessionInfoList;
            SessionInfoList activeSessions(m_authLibrary.getActiveSessions(getCurrentSessionId()));
            SessionId currentSessionId(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

			m_authLibrary.dumpSessionInfo(activeSessions);
            for (SessionInfoList::const_iterator sessionInfoIt = activeSessions.begin(); sessionInfoIt != activeSessions.end(); sessionInfoIt++)
            {
                // Initialise the details associated with the current session if not already set.
                if (m_currentSessionDetail.sessionId.empty() && sessionInfoIt->SessionId == currentSessionId)
                {
                    m_currentSessionDetail.sessionId = sessionInfoIt->SessionId;
                    m_currentSessionDetail.userId = sessionInfoIt->UserId;
                    m_currentSessionDetail.workstationId = sessionInfoIt->WorkstationId;
                }

                addOrUpdateSession(*sessionInfoIt);    
            }

            m_sessionsLoaded = true;
        }
        catch (const TA_Base_Core::AuthenticationAgentException& ex)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Auth Library failure in getActiveSessions(). Caught AuthenticationAgentException: %s", ex.what());

            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve active sessions from Authentication Agent.", ex.what()));
			TA_THROW(DutyMonitorFrameworkException("未能从AuthenticationAgent获取到当前有效的session列表", ex.what()));
        }
        catch (const TA_Base_Core::ObjectResolutionException& ex)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Auth Library failure in getActiveSessions(). Caught ObjectResolutionException: %s", ex.what());

			//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve active sessions from Authentication Agent.", ex.what()));
			TA_THROW(DutyMonitorFrameworkException("未能从AuthenticationAgent获取到当前有效的session列表", ex.what()));
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Auth Library failure in getSessionInfo(). Caught TransactiveException: %s", ex.what());

			//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve active sessions from Authentication Agent.", ex.what()));
			TA_THROW(DutyMonitorFrameworkException("未能从AuthenticationAgent获取到当前有效的session列表", ex.what()));
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Auth Library failure in getActiveSessions(). Caught unknown exception.");

            //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve active sessions from Authentication Agent."));
			TA_THROW(DutyMonitorFrameworkException("未能从AuthenticationAgent获取到当前有效的session列表"));
        }
    }

    //DutyDataStore dutyDataStore;
    DataCache& dataCache = DutyDataStore::getDataCache();
    bool ignoreSystemOperators(DutyDataStore::ignoreSystemOperators());

    SessionIdList sessionIds;
    sessionIds.reserve(m_sessionInfoMap.size());

    for (SessionInfoMap::const_iterator sessionInfoIt = m_sessionInfoMap.begin(); sessionInfoIt != m_sessionInfoMap.end(); sessionInfoIt++)
    {
        // Discard any unwanted sessions.
        if (ignoreSystemOperators && dataCache.isSystemOperator(sessionInfoIt->second.UserId))
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                        "Ignoring system operator session %s", sessionInfoIt->first.c_str());

            continue;            
        }

        sessionIds.push_back(sessionInfoIt->first);
    }

    FUNCTION_EXIT;
    return sessionIds;
}


bool SessionCache::isKnownSession(const SessionId& sessionId)
{
//    FUNCTION_ENTRY("isKnownSession(const SessionId& sessionId)");

    TA_Base_Core::ThreadGuard sessionMapGuard(m_sessionMapLock);

//    FUNCTION_EXIT;
    return (m_sessionInfoMap.find(sessionId) != m_sessionInfoMap.end());
}


bool SessionCache::isSystemOperatorSession(const SessionId& sessionId)
{
//    FUNCTION_ENTRY("isSystemOperatorSession(const SessionId& sessionId)");

    const SessionInfoEx& sessionInfoEx = getSessionInfo(sessionId);

//    FUNCTION_EXIT;
    return DutyDataStore::getDataCache().isSystemOperator(sessionInfoEx.UserId);
}

bool SessionCache::isContainCentralProfile(const SessionId& sessionId)
{
	bool result = false;

	const SessionInfoEx& sessionInfoEx = getSessionInfo(sessionId);

	try
	{
		//overrided
		if (sessionInfoEx.ProfileId.size() == 2)
		{
			//get profile from DB if local or central for base
			unsigned long baseProfile = sessionInfoEx.ProfileId[0];
			TA_Base_Core::IProfile* profileB;
			profileB = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( baseProfile );

			//get profile from DB if local or central for override
			unsigned long overrideProfile = sessionInfoEx.ProfileId[1];
			TA_Base_Core::IProfile* profileO;
			profileO = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( overrideProfile );
			if (profileB->getType() == CENTRAL_PROFILE_TYPE
				|| profileO->getType() == CENTRAL_PROFILE_TYPE)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"Session %s contains central profile.", sessionId.c_str());

				result = true;
			}
			//no need to add local profile with assumption that location is the same
		}
		//single operator
		else if (sessionInfoEx.ProfileId.size() == 1)
		{
			//get profile from DB if local or central for base
			unsigned long baseProfile = sessionInfoEx.ProfileId[0];
			TA_Base_Core::IProfile* profileB;
			profileB = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( baseProfile );
			if (profileB->getType() == CENTRAL_PROFILE_TYPE)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"Session %s contains central profile.", sessionId.c_str());

				result = true;
			}
			//no need to add local profile with assumption that location is the same
		}
		//else... profile cant be 0
	}
	catch (...) 
	{
		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve profile from Database."));
		TA_THROW(DutyMonitorFrameworkException("未能从数据库中获取到用户属性信息"));
	}

	return result;
}

SessionId SessionCache::getCurrentSessionId()
{
//    FUNCTION_ENTRY("getCurrentSessionId()");

    TA_Base_Core::ThreadGuard currentSessionIdGuard(m_currentSessionIdLock);

//    FUNCTION_EXIT;
    return m_currentSessionDetail.sessionId;
}


SessionInfoEx SessionCache::getSessionInfo(const SessionId& sessionId)
{
//    FUNCTION_ENTRY("getSessionInfo(const SessionId& sessionId)");

    TA_Base_Core::ThreadGuard sessionMapGuard(m_sessionMapLock);
    
    TA_ASSERT(m_sessionInfoMap.size() == m_sessionKeyMap.size(), "Session info map / key map size mismatch");

    // Return cached session details where available.
    SessionInfoMap::const_iterator sessionInfoIt(m_sessionInfoMap.find(sessionId));
    if (sessionInfoIt != m_sessionInfoMap.end())
    {
//        FUNCTION_EXIT;
        return sessionInfoIt->second;
    }

	GhostSessionInfoMap::const_iterator ghostSessionInfoIt(m_ghostSession.find(sessionId));
    if (ghostSessionInfoIt != m_ghostSession.end())
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Ghost Session %s",sessionId.c_str());
		//TA_THROW(DutyMonitorFrameworkException("retrive ghost session details.",sessionId.c_str()));
		TA_THROW(DutyMonitorFrameworkException("恢复ghost session详细信息",sessionId.c_str()));
    }
	if (TA_Base_Core::RunParams::getInstance().isSet(IGNORE_GHOST_SESSION.c_str()))  
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Ignore ghost Session %s",sessionId.c_str());
		//TA_THROW(DutyMonitorFrameworkException("ignore ghost session details.",sessionId.c_str()));
		TA_THROW(DutyMonitorFrameworkException("忽略ghost session详细信息",sessionId.c_str()));
	}
    // Nothing in the cache - get session details from the Authentication Agent.
    try
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"try to get ghost session %s",sessionId.c_str());
        addOrUpdateSession(m_authLibrary.getSessionInfo(sessionId, getCurrentSessionId()));    
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"finish to get ghost session %s",sessionId.c_str());
    }
    catch (const TA_Base_Core::AuthenticationAgentException& ex)
    {
        m_ghostSession.insert(std::make_pair(sessionId.c_str(), sessionId.c_str()));
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Auth Library failure in getSessionInfo(). Caught AuthenticationAgentException: %s", ex.what());

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve session details from Authentication Agent.", ex.what()));
		TA_THROW(DutyMonitorFrameworkException("未能从AuthenticationAgent获取到当前有效的session列表", ex.what()));
    }
    catch (const TA_Base_Core::ObjectResolutionException& ex)
    {
        m_ghostSession.insert(std::make_pair(sessionId.c_str(), sessionId.c_str()));
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Auth Library failure in getSessionInfo(). Caught ObjectResolutionException: %s %s", ex.what(), sessionId.c_str());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve session details from Authentication Agent.", ex.what()));
		TA_THROW(DutyMonitorFrameworkException("未能从AuthenticationAgent获取到当前有效的session列表", ex.what()));
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        m_ghostSession.insert(std::make_pair(sessionId.c_str(), sessionId.c_str()));
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Auth Library failure in getSessionInfo(). Caught TransactiveException: %s", ex.what());

		//TA_THROW(DutyMonitorFrameworkException("Failed to retrieve session details from Authentication Agent.", ex.what()));
		TA_THROW(DutyMonitorFrameworkException("未能从AuthenticationAgent获取到当前有效的session列表", ex.what()));
    }
    catch (...)
    {
        m_ghostSession.insert(std::make_pair(sessionId.c_str(), sessionId.c_str()));
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Auth Library failure in getSessionInfo(). Caught unknown exception.");

        //TA_THROW(DutyMonitorFrameworkException("Failed to retrieve session details from Authentication Agent."));
		TA_THROW(DutyMonitorFrameworkException("未能从AuthenticationAgent获取到当前有效的session列表"));
    }

    sessionInfoIt = m_sessionInfoMap.find(sessionId);
    TA_ASSERT(sessionInfoIt != m_sessionInfoMap.end(), "Session not found");
        
//    FUNCTION_EXIT;
    return sessionInfoIt->second;
}


SessionInfoEx SessionCache::getSessionInfo(SessionKey sessionKey)
{
//    FUNCTION_ENTRY("getSessionInfo(SessionKey sessionKey)"); 

    TA_Base_Core::ThreadGuard sessionMapGuard(m_sessionMapLock);
    
    TA_ASSERT(m_sessionInfoMap.size() == m_sessionKeyMap.size(), "Session info map / key map size mismatch");

    // Find the session ID corresponding to the specified session key and
    // return the cached session details.
    SessionKeyMap::const_iterator sessionKeyIt(m_sessionKeyMap.find(sessionKey));
    if (sessionKeyIt != m_sessionKeyMap.end())
    {
//        FUNCTION_EXIT;
        return getSessionInfo(sessionKeyIt->second);
    }
    
    std::ostringstream msg;
    //msg << "Failed to retrieve session details matching session key " << sessionKey;
	msg << "未能检索到Session" << sessionKey<<"的详细信息";

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());

    TA_THROW(DutyMonitorFrameworkException(msg.str()));
    
//    FUNCTION_EXIT;
}


const SessionUpdate* SessionCache::getSessionUpdate()
{
    FUNCTION_ENTRY("getSessionUpdate()");

    TA_Base_Core::ThreadGuard sessionUpdateGuard(m_sessionUpdateLock);
    
    FUNCTION_EXIT;
    return m_sessionUpdate;
}


// TA_Base_Core::SpecialisedMessageSubscriber implementation
void SessionCache::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)");

    SessionInfoCorbaDef* sessionInfoCorbaDef = NULL;
    SessionKey sessionKey;

    if ((message.messageState >>= sessionInfoCorbaDef))
    {
        try
        {
            // Update the cache with received session details.
            switch (sessionInfoCorbaDef->updateType)
            {
                case TA_Security::SessionStart:
                    // Where the operator ID and console ID match those associated with the original
                    // session, the new session must be the result of a profile change. Refresh the
                    // current session ID and process the update just like any other.
                    if (m_currentSessionDetail.userId == sessionInfoCorbaDef->UserId &&
                        m_currentSessionDetail.workstationId == sessionInfoCorbaDef->WorkstationId)
                    {
                        setCurrentSessionId(sessionInfoCorbaDef->SessionId.in());
                    }
                    // fall through to ProfileUpdate

                case TA_Security::ProfileUpdate:
                    sessionKey = addOrUpdateSession(*sessionInfoCorbaDef);
                    break;

                case TA_Security::SessionEnd:
                    sessionKey = removeSession(*sessionInfoCorbaDef);
                    if (sessionKey == INVALID_SESSION_KEY)
                    {
                        FUNCTION_EXIT;
                        return;
                    }
                    break;

                default:
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                                "Ignoring invalid session update type %d", sessionInfoCorbaDef->updateType );

                    FUNCTION_EXIT;
                    return;
            }

            // TD11998: If an agent logs in with a non-system profile and obtains duty (E.g. the Scheduling Agent)
            // it is possible that a system operator WILL have details in the grid. We therefore need to pass on
            // session end updates for system operators.

            // Do not propagate updates concerning system operators if clients are not interested in them.
            //DutyDataStore dutyDataStore;
            if ( DutyDataStore::ignoreSystemOperators() && DutyDataStore::getDataCache().isSystemOperator(sessionInfoCorbaDef->UserId)
                && TA_Security::SessionEnd != sessionInfoCorbaDef->updateType )
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                            "Ignoring system operator session %s", sessionInfoCorbaDef->SessionId.in());

            }
            else
            {
                {
                    TA_Base_Core::ThreadGuard sessionUpdateGuard(m_sessionUpdateLock);
                      
                    TA_ASSERT(m_sessionUpdate == NULL, "Stored session update pointer is not NULL");

                    // Create a new subsystem duty update based on processed message details.
                    m_sessionUpdate = new SessionUpdate;
                    m_sessionUpdate->updateType = sessionInfoCorbaDef->updateType;
                    m_sessionUpdate->sessionId = sessionInfoCorbaDef->SessionId;
                    m_sessionUpdate->sessionKey = sessionKey;
                }

                // First pass the update to the region duty matrix so that it can add/remove
                // the session if necessary. This is done *before* notifying observers in case
                // they run queries on the matrix as a result of the update.
                DutyDataStore::getRegionDutyMatrix().receiveSessionUpdate(*m_sessionUpdate);

                // Also pass the update to the local rights library so that it can reset its rights
                // cache. Again, this is done before notifying observers in case they want to look up
                // duty rights as a result of the update. 
                DutyRightsSingleton::getInstance().receiveSessionUpdate(*m_sessionUpdate);

                // Now let all observers know there's a new update waiting to be picked up.
                notify(*this);

                {
                    TA_Base_Core::ThreadGuard sessionUpdateGuard(m_sessionUpdateLock);
                         
                    // All observers should now have processed the update.
                    delete m_sessionUpdate;
                    m_sessionUpdate = NULL;
                }
            }

            // Reset the current session ID when the session has ended. We do this after
            // observers have been notified in case they check for current session while
            // processing the update.
            if (sessionInfoCorbaDef->updateType == TA_Security::SessionEnd &&
                m_currentSessionDetail.sessionId == sessionInfoCorbaDef->SessionId.in())
            {                       
                setCurrentSessionId("");
            }
        }
        catch (const DutyMonitorFrameworkException& ex)
        {
            // Possibly invalid session details.
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "Ignoring session update for session %s (%s)", sessionInfoCorbaDef->SessionId.in(), ex.what());
        }              
        catch (...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "Ignoring session update for session %s", sessionInfoCorbaDef->SessionId.in());
        }              
    }

    FUNCTION_EXIT;
}


SessionKey SessionCache::addOrUpdateSession(const SessionInfo& sessionInfo)
{
    FUNCTION_ENTRY("addOrUpdateSession(const SessionInfo& sessionInfo)"); 

    TA_Base_Core::ThreadGuard sessionMapGuard(m_sessionMapLock);

    TA_ASSERT(m_sessionInfoMap.size() == m_sessionKeyMap.size(), "Session info map / key map size mismatch");

    // If the session is already in the map, then update its details, otherwise add it.
    SessionInfoMap::iterator sessionInfoIt(m_sessionInfoMap.find(sessionInfo.SessionId));
    if (sessionInfoIt != m_sessionInfoMap.end())
    {
        sessionInfoIt->second.UserId = sessionInfo.UserId;
        sessionInfoIt->second.OperatorId = sessionInfo.OperatorId;
        sessionInfoIt->second.ProfileId = sessionInfo.ProfileId;
        sessionInfoIt->second.LocationId = sessionInfo.LocationId;
        sessionInfoIt->second.WorkstationId = sessionInfo.WorkstationId;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Updated details for session %d (%s).", sessionInfoIt->second.sessionKey, sessionInfoIt->second.SessionId.c_str() );

        FUNCTION_EXIT;
        return sessionInfoIt->second.sessionKey;
    }
    
    SessionInfoEx sessionInfoEx;

    sessionInfoEx.sessionKey = getNextSessionKey();
    sessionInfoEx.SessionId = sessionInfo.SessionId;
    sessionInfoEx.UserId = sessionInfo.UserId;
    sessionInfoEx.OperatorId = sessionInfo.OperatorId;
    sessionInfoEx.ProfileId = sessionInfo.ProfileId;
    sessionInfoEx.LocationId = sessionInfo.LocationId;
    sessionInfoEx.WorkstationId = sessionInfo.WorkstationId;

    m_sessionInfoMap.insert(std::make_pair(sessionInfoEx.SessionId, sessionInfoEx));
    m_sessionKeyMap.insert(std::make_pair(sessionInfoEx.sessionKey, sessionInfoEx.SessionId));

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Added details for session %d (%s).", sessionInfoEx.sessionKey, sessionInfoEx.SessionId.c_str() );

    FUNCTION_EXIT;
    return sessionInfoEx.sessionKey;
}


SessionKey SessionCache::addOrUpdateSession(const SessionInfoCorbaDef& sessionInfoCorbaDef)
{
    FUNCTION_ENTRY("addOrUpdateSession(const SessionInfoCorbaDef& sessionInfoCorbaDef)"); 

    TA_ASSERT(sessionInfoCorbaDef.updateType == TA_Security::SessionStart ||
              sessionInfoCorbaDef.updateType == TA_Security::ProfileUpdate, "Not an operator login/override update");

    SessionInfo sessionInfo;

    sessionInfo.SessionId = sessionInfoCorbaDef.SessionId.in();
    sessionInfo.UserId = sessionInfoCorbaDef.UserId;
    sessionInfo.LocationId = sessionInfoCorbaDef.LocationId;
    sessionInfo.WorkstationId = sessionInfoCorbaDef.WorkstationId;

    sessionInfo.OperatorId.reserve(1 + sessionInfoCorbaDef.additionalOperators.length());
    sessionInfo.OperatorId.push_back(sessionInfoCorbaDef.UserId); // base operator
    for (unsigned long o = 0; o < sessionInfoCorbaDef.additionalOperators.length(); o++)
    {
        sessionInfo.OperatorId.push_back(sessionInfoCorbaDef.additionalOperators[o]);
    }

    sessionInfo.ProfileId.reserve(1 + sessionInfoCorbaDef.additionalProfiles.length());
    sessionInfo.ProfileId.push_back(sessionInfoCorbaDef.ProfileId); // base profile
    for (unsigned long p = 0; p < sessionInfoCorbaDef.additionalProfiles.length(); p++)
    {
        sessionInfo.ProfileId.push_back(sessionInfoCorbaDef.additionalProfiles[p]);
    }

    FUNCTION_EXIT;
    return addOrUpdateSession(sessionInfo);
}


SessionKey SessionCache::removeSession(const SessionInfoCorbaDef& sessionInfoCorbaDef)
{
    FUNCTION_ENTRY("removeSession(const SessionInfoCorbaDef& sessionInfoCorbaDef)"); 

    TA_ASSERT(sessionInfoCorbaDef.updateType == TA_Security::SessionEnd, "Not an operator logout update");

    TA_Base_Core::ThreadGuard sessionMapGuard(m_sessionMapLock);

    TA_ASSERT(m_sessionInfoMap.size() == m_sessionKeyMap.size(), "Session info map / key map size mismatch");

    SessionInfoMap::iterator sessionInfoIt(m_sessionInfoMap.find(std::string(sessionInfoCorbaDef.SessionId.in())));
    if (sessionInfoIt != m_sessionInfoMap.end())
    {
        SessionKey sessionKey(sessionInfoIt->second.sessionKey);
        SessionKeyMap::iterator sessionKeyIt(m_sessionKeyMap.find(sessionKey));
        TA_ASSERT(sessionKeyIt != m_sessionKeyMap.end(), "Session map entry has no corresponding session key entry");

        m_sessionInfoMap.erase(sessionInfoIt);
        m_sessionKeyMap.erase(sessionKeyIt);

        //LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Removed details for session %lu (%s).", sessionInfoIt->second.sessionKey, sessionInfoIt->second.SessionId.c_str() );

        FUNCTION_EXIT;   
        return sessionKey;
    }
   
    FUNCTION_EXIT;   
    return INVALID_SESSION_KEY;
}


SessionKey SessionCache::getNextSessionKey()
{
    FUNCTION_ENTRY("getNextSessionKey()"); 

    TA_Base_Core::ThreadGuard nextSessionKeyGuard(m_nextSessionKeyLock);
    TA_Base_Core::ThreadGuard sessionMapGuard(m_sessionMapLock);

    // Mark the starting point in the search for next available session key.
    SessionKey markerKey(m_nextSessionKey);

    // Make sure the session key is not already in use.
    while (m_sessionKeyMap.find(m_nextSessionKey) != m_sessionKeyMap.end())
    {
        // We have encountered a key that is already taken, so let's try the next one.
        m_nextSessionKey++;

        // Test for session key wraparound. The key is a ulong so this is highly unlikely.
        if (m_nextSessionKey == markerKey)
        {
            //TA_THROW(DutyMonitorFrameworkException("Session cache out of session keys."));
			TA_THROW(DutyMonitorFrameworkException("Session主键值越界"));
        }
    }

    // We now have a session key to return.
    SessionKey nextSessionKey(m_nextSessionKey);
    
    // Use a different key next time.
    m_nextSessionKey++;
 
    FUNCTION_EXIT;
    return nextSessionKey;
}


void SessionCache::setCurrentSessionId(const SessionId& sessionId)
{
    FUNCTION_ENTRY("setCurrentSessionId(const SessionId& sessionId)");

    TA_Base_Core::ThreadGuard currentSessionIdGuard(m_currentSessionIdLock);

    m_currentSessionDetail.sessionId = sessionId;

    FUNCTION_EXIT;
}
