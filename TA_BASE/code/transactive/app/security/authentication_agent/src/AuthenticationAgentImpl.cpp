/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/AuthenticationAgentImpl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// AuthenticationAgentImpl.cpp: implementation of the AuthenticationAgentImpl class.
//
//////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER)
//#pragma warning(disable:4786)
#endif // (_MSC_VER)

#include "app/security/authentication_agent/src/AuthenticationAgentImpl.h"
#include "app/security/authentication_agent/src/IAuthentication.h"
#include "app/security/authentication_agent/src/AuthenticationAgent.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::AuthenticationAgentException;
using TA_Base_Core::AuthenticationSecurityException;

//using TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence;
//using TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence;
//using TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef;

using TA_Base_Core::RunParams;

using TA_Base_App::IAuthentication;

using TA_Base_Bus::GenericAgent;
using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_App
{

    AuthenticationAgentImpl::AuthenticationAgentImpl(IAuthentication& authentication,  AuthenticationAgent* parentClass) :
        m_authentication(authentication),
		m_parentClass(parentClass)
    {
        FUNCTION_ENTRY("AuthenticationAgentImpl::AuthenticationAgentImpl() constructor");
        activateServantWithName(RunParams::getInstance().get(RPARAM_ENTITYNAME));
        FUNCTION_EXIT;
    }

    AuthenticationAgentImpl::~AuthenticationAgentImpl()
    {
    }

    char* AuthenticationAgentImpl::requestSession(CORBA::ULong userId, 
                                                  CORBA::ULong profileId, 
                                                  CORBA::ULong locationId, 
                                                  CORBA::ULong workstationId, 
                                                  const char* password,
												   CORBA::Boolean displayOnly)
    {
		checkOperationMode();
        // wrap this in a try-catch block to convert the C++ exceptions into CORBA exceptions
        std::string sessionId;
        try
        {
			LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo,"[2011-11-04] char* AuthenticationAgentImpl::requestSession");
			sessionId = m_authentication.requestSession( userId, profileId, locationId, workstationId, password,displayOnly );
			LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo,"[2011-11-04] char* AuthenticationAgentImpl::requestSession done");
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }

        return CORBA::string_dup(sessionId.c_str());
    }

    void AuthenticationAgentImpl::changePassword(const char* sessionId, const char* oldPassword, const char* newPassword)
    {

		checkOperationMode();

        // wrap this in a try-catch block to convert the C++ exceptions into CORBA exceptions
        try
        {
            m_authentication.changePassword( sessionId, oldPassword, newPassword );
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }
    }

    void AuthenticationAgentImpl::beginOverride(const char* sessionId, 
                                                CORBA::ULong userId, 
                                                CORBA::ULong profileId, 
                                                CORBA::ULong locationId, 
                                                const char* password)
    {

		checkOperationMode();

        // wrap this in a try-catch block to convert the C++ exceptions into CORBA exceptions
        try
        {
            m_authentication.beginOverride( sessionId, userId, profileId, locationId, password );
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }
    }


    void AuthenticationAgentImpl::endOverride(const char* sessionId)
    {
		checkOperationMode();

        // wrap this in a try-catch block to convert the C++ exceptions into CORBA exceptions
        try
        {
            m_authentication.endOverride( sessionId );
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }
    }

    char* AuthenticationAgentImpl::changeProfile(const char* sessionId, 
                                                 CORBA::ULong profileId, 
                                                 CORBA::ULong locationId, 
                                                 const char* password,
												  CORBA::Boolean displayOnly)
    {

		checkOperationMode();

        // wrap this in a try-catch block to convert the C++ exceptions into CORBA exceptions
        std::string newSessionId;
        try
        {
            newSessionId = m_authentication.changeProfile( sessionId, profileId, locationId, password,displayOnly );
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }

        return CORBA::string_dup(newSessionId.c_str());
    }


    TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence* 
        AuthenticationAgentImpl::getProfiles(const char* sessionId)
    {

		checkOperationMode();

        TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence_var profiles = new TA_Base_Bus::IAuthenticationAgentCorbaDef::ProfileSequence();
        // wrap this in a try-catch block to convert the C++ exceptions into CORBA exceptions
        try
        {
            std::vector<unsigned long> profileVector;
			m_authentication.getProfiles( sessionId, profileVector );

            profiles->length(profileVector.size());
            for (unsigned int i = 0;i < profileVector.size();i++)
            {
                profiles[i] = profileVector[i];
            }
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }

        return profiles._retn();
    }

    TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef* 
        AuthenticationAgentImpl::getSessionInfo(const char* sessionId, const char* requestingSessionId)
    {
		checkOperationMode();

        TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef_var session;
        try
        {
			TA_Base_Bus::SessionInfo sessionInfo;
            m_authentication.getSessionInfo( sessionId, requestingSessionId, sessionInfo );

            session = new TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef(
                TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession( sessionInfo ) );
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }

        return session._retn();
    }


    void AuthenticationAgentImpl::endSession(const char* sessionId)
    {

		checkOperationMode();

        try
        {
            m_authentication.endSession( sessionId );
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
							"[TD15069] AuthenticationAgentImpl::endSession() called (%s)", sessionId);
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }

    }

    TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence* AuthenticationAgentImpl::getActiveSessions(const char* sessionId)
    {
		checkOperationMode();

        TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence_var sessions = new TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoSequence();        
        
        try
        {
            std::vector<TA_Base_Bus::SessionInfo> sessionInfos;
			m_authentication.getActiveSessions( sessionId, sessionInfos );

            sessions->length(sessionInfos.size());
            for (unsigned int i = 0;i < sessionInfos.size();i++)
            {
                sessions[i] = TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession( sessionInfos[i] );
            }
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }

        return sessions._retn();
    }

    void AuthenticationAgentImpl::keepSessionAlive(time_t timestamp, const char* sessionId)
    {

		checkOperationMode();

        try
        {
            m_authentication.keepSessionAlive( timestamp, sessionId );
        }
        catch(AuthenticationSecurityException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException exception;

            exception.reason = (TA_Base_Bus::IAuthenticationAgentCorbaDef::ESecurityReason) ex.getReason();
            exception.description = CORBA::string_dup( ex.what() );

            throw exception;
        }
        catch(AuthenticationAgentException& ex)
        {
            TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException exception;

            exception.reason = CORBA::string_dup( ex.what() );

            throw exception;
        }
    }
	
	void AuthenticationAgentImpl::checkOperationMode()
	{
		TA_ASSERT(m_parentClass != NULL, "parent class is NULL");
		m_parentClass->checkOperationMode();
	}

	void AuthenticationAgentImpl::beginDisplayonly(bool display)
	{
		//[2011-11-04] m_authentication.beginDisplayonly(display);
	}
}
