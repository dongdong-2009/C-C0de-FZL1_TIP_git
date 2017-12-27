/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingSessionManager.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/04/16 15:14:32 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  * This class is used to manage session keys for the Scheduling Agent.
  *
  */

#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/scheduling/scheduling_agent/src/SchedulingSessionManager.h"


namespace TA_Base_App
{

SchedulingSessionManager* SchedulingSessionManager::m_instance = NULL;
ACE_Mutex SchedulingSessionManager::m_mtxSingletonLock;


SchedulingSessionManager::SchedulingSessionManager() :
m_authenticationLibrary(NULL),
m_operatorID(0),
m_workstationID(0),
m_password(""),
m_isInitialised(false)
{
    FUNCTION_ENTRY("SchedulingSessionManager");

    m_authenticationLibrary = new TA_Base_Bus::AuthenticationLibrary();
    TA_ASSERT( m_authenticationLibrary != NULL, "m_authenticationLibrary should never be NULL");

    FUNCTION_EXIT;
}


SchedulingSessionManager::~SchedulingSessionManager()
{
    FUNCTION_ENTRY("~SchedulingSessionManager");

    if ( NULL != m_authenticationLibrary )
    {
        delete m_authenticationLibrary;
        m_authenticationLibrary = NULL;
    }

    FUNCTION_EXIT;
}


void SchedulingSessionManager::initialise(unsigned long operatorID, unsigned long workstationID, const std::string& password)
{
    FUNCTION_ENTRY("initialise");

    m_operatorID    = operatorID;
    m_workstationID = workstationID;
    m_password      = password;

    m_isInitialised = true;

    FUNCTION_EXIT;
}


SchedulingSessionManager& SchedulingSessionManager::getInstance()
{
    if ( NULL == m_instance )
    {
        ACE_Guard<ACE_Mutex> guard(m_mtxSingletonLock);

        //
        // If this method has been called before and initialise was not called,
        // assert.
        //
        TA_ASSERT(m_instance == NULL || m_instance->m_isInitialised,
                  "SchedulingSessionManager::initialise() must be called the "
                  "first time getInstance() is invoked");

        if( NULL == m_instance )
        {
            m_instance = new SchedulingSessionManager();
            TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
        }
    }

    return *m_instance;
}



void SchedulingSessionManager::setOperatorID(unsigned long operatorID)
{
    m_operatorID = operatorID;
}

void SchedulingSessionManager::setWorkstationID(unsigned long workstationID)
{
    m_workstationID = workstationID;
}

void SchedulingSessionManager::setPassword(const std::string& password)
{
    m_password = password;
}


std::string SchedulingSessionManager::getSessionKey(unsigned long profileID, unsigned long locationID)
{
    std::string sessionKey="";
    bool bInactiveSessionFound = false;

    {
        ACE_Read_Guard<ACE_RW_Mutex> grdReadLock( m_mtxCurSessionsLock );

        //	Get the current session key if it exists and if not then get a new session
        for ( std::list<SimpleSessionInfo>::iterator sessionIter = m_currentSessions.begin(); 
            sessionIter != m_currentSessions.end(); ++sessionIter )
        {
            bool profileMatches = false;

            //  Find a session that matches the fixed fields and then check its ProfileID vector
            if ((sessionIter->ulLocationId == locationID) &&
                (sessionIter->ulWorkstationId == m_workstationID) &&
                (sessionIter->ulUserId == m_operatorID))
            {
                std::vector<unsigned long>::const_iterator profileIter = sessionIter->vecProfiles.begin();

                for ( ; profileIter != sessionIter->vecProfiles.end(); profileIter++)
                {
                    if ( *profileIter == profileID)
                    {
                        profileMatches = true;
                        break;
                    }
                }

                if (profileMatches)
                {
                    //	Use this session key because it matches the required location, operator,
                    //  workstation and profile IDs ... but first make sure the session is active
                    std::vector<TA_Base_Bus::SessionInfo> activeSessions;
                    try
                    {
                        activeSessions = m_authenticationLibrary->getActiveSessions( sessionIter->strSessionId );
                    }
                    catch ( std::exception& expWhat )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
                    }
                    catch( ... )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unknown exception when getActiveSessions" );
                    }

                    if ( activeSessions.empty() )
                    {
                        // Mark as inactive session
                        bInactiveSessionFound = true;
                        sessionIter->bInactive = true;
                        continue;
                    }

                    sessionKey = sessionIter->strSessionId.c_str();
                    return sessionKey;
                }
            }
        }
    }

    if ( bInactiveSessionFound )
    {
        ACE_Write_Guard<ACE_RW_Mutex> grdNewSessionLock( m_mtxCurSessionsLock );

        // Remove inactive sessions
        for ( std::list<SimpleSessionInfo>::iterator itLoop = m_currentSessions.begin(); itLoop != m_currentSessions.end(); )
        {
            if ( itLoop->bInactive )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removing inactive session %s", itLoop->strSessionId.c_str() );
                itLoop = m_currentSessions.erase( itLoop );
            }
            else
            {
                ++itLoop;
            }
        }
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> grdNewSessionLock( m_mtxCurSessionsLock );

        // Get a new session key and return it.
        sessionKey = m_authenticationLibrary->requestSession( m_operatorID, profileID, locationID, m_workstationID, m_password);

        TA_Base_Bus::SessionInfo stSessionInfo = getSessionInfo( sessionKey);

        SimpleSessionInfo stSession;
        m_currentSessions.push_back( stSession );

        SimpleSessionInfo& refNewSession = m_currentSessions.back();
        refNewSession.bInactive = false;
        refNewSession.strSessionId = stSessionInfo.SessionId.c_str();
        refNewSession.ulWorkstationId = stSessionInfo.WorkstationId;
        refNewSession.ulLocationId = stSessionInfo.LocationId;
        refNewSession.ulUserId = stSessionInfo.UserId;
        refNewSession.vecProfiles = stSessionInfo.ProfileId;
    }

    return sessionKey;
}


TA_Base_Bus::SessionInfo SchedulingSessionManager::getSessionInfo( const std::string& sessionID )
{
    //
    // Get the session info from the authentication library.
    // The second parameter is the requesting session ID.
    // This is currently ignored by the authentication agent.
    //
    return m_authenticationLibrary->getSessionInfo(sessionID, "");
}


}; // TA_Base_App

