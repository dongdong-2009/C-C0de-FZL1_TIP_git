/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/authentication_library/src/AuthenticationLibrary.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// AuthenticationLibrary.cpp: implementation of the AuthenticationLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined(_MSC_VER)

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/AuthenPeerTaskManager.h"
#include "bus/security/authentication_library/src/KeepSessionAliveThreadManager.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/IOperatorPassword.h"
#include "core/data_access_interface/src/OperatorPasswordAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/OperatorPasswordAccessFactory.h"
#include "core/data_access_interface/src/IOperatorPassword.h"
#include <boost/lexical_cast.hpp>

using TA_Base_Core::IDatabase;
using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace
{
    std::set<std::string> g_sessionIds;
}

namespace TA_Base_Bus
{
    SessionInfo info;

    AuthenticationLibrary::AuthenticationLibrary(bool isMgr /*=false*/)
        : m_pAuthenPeerTaskManager(NULL)
    {
        info.OperatorId.push_back(0);
        info.isDisplayOnly = false;
        info.LocationId = 1;
        info.ProfileId.push_back(0);
        info.SessionId = "test session";
        info.UserId = 0;
        info.WorkstationId = 0;
    }

    AuthenticationLibrary::~AuthenticationLibrary()
    {
    }

    std::string AuthenticationLibrary::requestSession(unsigned long userId,
            unsigned long profileId,
            unsigned long locationId,
            unsigned long workstationId,
            const std::string& password,
            bool displayOnly)
    {
        if ( TA_Base_Core::RunParams::getInstance().isSet("LocalTestSessionId") )
        {
            return TA_Base_Core::RunParams::getInstance().get("LocalTestSessionId");
        }

        static size_t i = 1;
        std::string sessionId = "Local Test Session ID " + boost::lexical_cast<std::string>(i++);
        g_sessionIds.insert(sessionId);
        return sessionId;
    }

    void AuthenticationLibrary::changePassword(const std::string& sessionId, const std::string& oldPassword, const std::string& newPassword)
    {
    }

    void AuthenticationLibrary::beginOverride(const std::string& sessionId,
            unsigned long userId,
            unsigned long profileId,
            unsigned long locationId,
            const std::string& password)
    {
    }

    void AuthenticationLibrary::endOverride(const std::string& sessionId)
    {
    }

    std::string AuthenticationLibrary::changeProfile(const std::string& sessionId,
            unsigned long profileId,
            unsigned long locationId,
            const std::string& password,
            bool displayOnly)
    {
        return "test profile";
    }

    void AuthenticationLibrary::getProfiles(const std::string& sessionId, std::vector<unsigned long>& profileVector)
    {
        profileVector.push_back(0);
    }

    std::vector<unsigned long> AuthenticationLibrary::getProfiles(const std::string& sessionId)
    {
        std::vector<unsigned long> vtProfiles;
        getProfiles(sessionId, vtProfiles);
        return vtProfiles;
    }

    SessionInfo AuthenticationLibrary::getSessionInfo(const std::string& sessionId, const std::string& requestingSessionId)
    {
        return info;
    }

    SessionInfo AuthenticationLibrary::getSessionInfoFromDb(const std::string& sessionId)
    {
        return info;
    }

    void AuthenticationLibrary::endSession(const std::string& sessionId)
    {
        g_sessionIds.erase(sessionId);
    }

    void AuthenticationLibrary::getActiveSessions(const std::string& sessionId, std::vector<SessionInfo>& vtSessionInfo)
    {
    }

    void AuthenticationLibrary::getActiveSessionsForOneLocation(const std::string& noUseSessionId, AuthenticationAgentNamedObj& authAgent, std::vector<SessionInfo>& sessionInfoVector)
    {
    }

    std::vector<SessionInfo> AuthenticationLibrary::getActiveSessions(const std::string& sessionId)
    {
        std::vector<SessionInfo> vtSessionInfo;
        return vtSessionInfo;
    }

    SessionInfo AuthenticationLibrary::convertCORBASessionToCSession(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& corbaSession)
    {
        return info;
    }

    TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef AuthenticationLibrary::convertCSessionToCORBASession(const SessionInfo& cSession)
    {
        IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession;
        return (corbaSession);
    }

    void AuthenticationLibrary::throwSecurityException(const std::string& description, TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason reason)
    {
    }

    std::string AuthenticationLibrary::findEquivalentSessionNotUsingSessionId(const TA_Base_Bus::SessionInfo& sessionToMatch)
    {
        return "test";
    }

    bool AuthenticationLibrary::isVectorEqual(const std::vector<unsigned long>& vtFirst, const std::vector<unsigned long>& vtSecond)
    {
        return true;
    }

    void AuthenticationLibrary::EnsureSpecificCondition(bool fCondition, char* msgStr)
    {
    }

    AuthenPeerTaskManager* AuthenticationLibrary::getAuthenPeerTaskManager()
    {
        return NULL;
    }

    void AuthenticationLibrary::dumpSessionInfo(std::vector<SessionInfo>& vtSessionInfo)
    {
    }

    void AuthenticationLibrary::updateOverrideInfoToDb(const std::string& sessionId, unsigned long userOverrideId, unsigned long profileOverrideId)
    {
    }

    void AuthenticationLibrary::updateOverrideInfoToDb(const std::string& sessionId)
    {
    }

    int AuthenticationLibrary::GetHash(const std::string& data, std::string& results)
    {
        return 0;
    }

    unsigned long	AuthenticationLibrary::getActualLocationKey(unsigned long sourceLocationKey)
    {
        return 0;
    }
}
