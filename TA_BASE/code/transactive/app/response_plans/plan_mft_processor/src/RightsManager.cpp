/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/RightsManager.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  *
  **/

#include "core/utilities/src/DebugUtil.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "app/response_plans/plan_mft_processor/src/RightsManager.h"


namespace TA_Base_App
{

TA_Base_App::RightsManager *TA_Base_App::RightsManager::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::RightsManager::s_singletonLock;

/////////////////////////////////////////////////////////////////////////////
// PlanAgentUpdateProcessor

RightsManager::RightsManager() : TA_Base_Core::RunParamUser(),
m_authLibrary( NULL ),
m_rightsLibrary( NULL ),
m_strSessionId( "" ),
m_setProfileIds(),
m_lckProfiles()
{
    FUNCTION_ENTRY( "PlanAgentUpdateProcessor" );

    FUNCTION_EXIT;
}


RightsManager::~RightsManager()
{
    FUNCTION_ENTRY( "~RightsManager" );

    uninit();

    FUNCTION_EXIT;
}

void RightsManager::init()
{
    if(NULL == m_authLibrary)
    {
        m_authLibrary = new TA_Base_Bus::AuthenticationLibrary(true);
    }

    TA_Base_Core::RunParams::getInstance().registerRunParamUser( this, RPARAM_SESSIONID );

    updateRights( TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ));

    TA_Base_Bus::RightsLibraryFactory rlFactory;
    m_rightsLibrary = rlFactory.buildRightsLibrary();
}

void RightsManager::uninit()
{
    if(NULL != m_authLibrary)
    {
        delete m_authLibrary;
        m_authLibrary = NULL;
    }

    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser( this );

    if (NULL != m_rightsLibrary)
    {
        delete m_rightsLibrary;
        m_rightsLibrary = NULL;
    }
}

RightsManager &RightsManager::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    if ( NULL == s_instance )
    {
        TA_THREADGUARD(s_singletonLock);

        if ( NULL == s_instance )
        {
            s_instance = new RightsManager();
        }
    }

    FUNCTION_EXIT;
    return *s_instance;
}


void RightsManager::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    TA_THREADGUARD(s_singletonLock);

    if ( NULL != s_instance )
    {
        delete s_instance;

        s_instance = NULL;
    }

    FUNCTION_EXIT;
}

void RightsManager::onRunParamChange( const std::string& name, const std::string& value )
{
    FUNCTION_ENTRY( "onRunParamChange" );

    if ( 0 != name.compare( RPARAM_SESSIONID ))
    {
        return;
    }

    if ( value.empty() )
    {
        rightsCleared();
    }
    else
    {
        updateRights( value );
    }

    FUNCTION_EXIT;
}

void RightsManager::rightsCleared()
{
    m_strSessionId = "";

    {
        TA_THREADGUARD(m_lckProfiles);
        m_setProfileIds.clear();
    }
}

void RightsManager::updateRights( const std::string& strSessionId )
{
    FUNCTION_ENTRY( "updateRights" );

    m_strSessionId = strSessionId.c_str();

    TA_Base_Bus::SessionInfo sessionInfo = { "", 0u };

    try
    {
        if ( NULL != m_authLibrary )
        {
            sessionInfo = m_authLibrary->getSessionInfo( m_strSessionId, m_strSessionId );
        }
    }
    catch ( const CORBA::Exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when checking profile" );
    }

    {
        TA_THREADGUARD(m_lckProfiles);
        m_setProfileIds.clear();
        for ( std::vector<unsigned long>::iterator itLoop = sessionInfo.ProfileId.begin(); itLoop != sessionInfo.ProfileId.end(); ++itLoop )
        {
            m_setProfileIds.insert( *itLoop );
        }
    }

    FUNCTION_EXIT;
}

bool RightsManager::isProfileMatched( const unsigned long ulProfileId )
{
    FUNCTION_ENTRY( "isProfileMatched" );

    TA_Base_Core::ThreadGuard guard(m_lckProfiles);

    FUNCTION_EXIT;
    return ( m_setProfileIds.end() != m_setProfileIds.find( ulProfileId ));
}

bool RightsManager::isRightsPermittedOnResource( const unsigned long ulEntityKey, const unsigned long ulActionId )
{
    FUNCTION_ENTRY( "isActionPermittedOnResource" );

    if ( NULL == m_rightsLibrary )
    {
        FUNCTION_EXIT;
        return false;
    }

    std::string reason = "";
    TA_Base_Bus::EDecisionModule decisionModuleIgnored = TA_Base_Bus::UNDEFINED_DM;
    bool bPermitted = false;

    try
    {
        bPermitted = m_rightsLibrary->isActionPermittedOnResourceWithoutDuty( m_strSessionId, ulEntityKey, ulActionId, reason, decisionModuleIgnored );
    }
    catch ( const CORBA::Exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when checking permission" );
    }

    FUNCTION_EXIT;
    return bPermitted;
}

bool RightsManager::isDutyPermittedOnResource( const unsigned long ulEntityKey, const unsigned long ulActionId )
{
    FUNCTION_ENTRY( "isActionPermittedOnResource" );

    if ( NULL == m_rightsLibrary )
    {
        FUNCTION_EXIT;
        return false;
    }

    if ( m_strSessionId.empty() )
    {
        FUNCTION_EXIT;
        return false;
    }

    std::string reason = "";
    TA_Base_Bus::EDecisionModule decisionModuleIgnored = TA_Base_Bus::UNDEFINED_DM;
    bool bPermitted = false;

    try
    {
        bPermitted = m_rightsLibrary->isActionPermittedOnResource( m_strSessionId, ulEntityKey, ulActionId, reason, decisionModuleIgnored );
    }
    catch ( const CORBA::Exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when checking permission" );
    }

    FUNCTION_EXIT;
    return bPermitted;
}

}
