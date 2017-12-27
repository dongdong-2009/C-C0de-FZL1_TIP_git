/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/rights_agent/src/RightsAgentImpl.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/07/03 17:57:59 $
 * Last modified by:  $Author: liwei.gong $
 *
 * This class implements the IRightsAgentCorbaDef interface.
 */



#include "RightsAgent.h"
#include "RightsAgentUser.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/security/rights_agent/xacml_factory/src/XacmlFactoryInternal.h"

namespace C  = TA_Base_Core;

using TA_Base_Bus::GenericAgent;


namespace TA_Base_App
{
    RightsAgentImpl::RightsAgentImpl(const std::string& agentName, RightsAgentUser* parentClass) :
		m_rightsFactory(),
		m_policy(m_rightsFactory.buildPolicy()),
		m_sessionCache(m_sessionUpdates),
		m_parentClass(parentClass)
    {
		FUNCTION_ENTRY("RightsAgentImpl");
		
		TA_ASSERT( NULL != m_policy.get(), "Failed to allocate m_policy" );

		activateServantWithName( agentName );

		TA_ASSERT( C::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), "RPARAM_LOCATIONKEY unset" );

		// Get the entity name based on the locationKey
		unsigned long locationKey = strtoul( C::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );

		std::string agentType = TA_Base_Core::AuthenticationAgentEntityData::getStaticType();

		// this will only return a single entry
		C::CorbaNameList authAgentName = 
			C::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( agentType, locationKey,true );

        // Set the agent name - this is the name of the agent to contact. It is passed in as a variable 
        // because the Authentication Agent is distributed, and there can be many instances running
        // in any one system.

		//m_authAgent = new AuthAgentType( authAgentName[0] );

		FUNCTION_EXIT;
	}

    RightsAgentImpl::~RightsAgentImpl()
    {
    }

	// yanrong++: called by authentication agent when session changed.
    void RightsAgentImpl::sessionUpdate(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
			bool bSessionEnd)
    {
		FUNCTION_ENTRY("sessionUpdate");

		checkOperationMode();
		
		TA_Base_Bus::SessionInfo sessionInfo =
			    TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession( session );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"[xxx] sessionUpdate, session=%s", sessionInfo.SessionId.c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"[xxx] sessionUpdate, session.isdisplayonly=%lu", sessionInfo.isDisplayOnly);

		// C::ThreadGuard guard(m_profileCacheLock);
		if(bSessionEnd)
		{			
			m_sessionCache.remove(sessionInfo.SessionId);
		}
		else
		{
			std::vector<unsigned long> & vtProfileId = sessionInfo.ProfileId;
			std::list<unsigned long> profileList;
			for( unsigned long i=0; i< vtProfileId.size(); i++ )
			{
				profileList.push_back( vtProfileId[i] );				
			}
			m_sessionCache.add(sessionInfo.SessionId.c_str(),profileList);
		}

		FUNCTION_EXIT;
    }
	// ++yanrong
	void RightsAgentImpl::agentSetControl(){
		//clear session cache to avoid inconsistence with authentication agent
		m_sessionCache.removeAll();
	}
    TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
        RightsAgentImpl::isActionPermittedOnResource(
            const char* sessionId, 
            CORBA::ULong resourceKey, 
            CORBA::ULong actionKey, 
            CORBA::String_out reasonString, 
            TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
    {
		
		TA_Base_Bus::GenericAgent::ensureControlMode();

		checkOperationMode();

		std::string opMode = TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE);
		decisionModule = TA_Base_Bus::IRightsAgentCorbaDef::Rights;

		if( RPARAM_CONTROL != opMode )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Attempted call on interface in %s mode", opMode.c_str() );

			// Throw an exception for debugging purposes
			throw TA_Base_Bus::IRightsAgentCorbaDef::MonitorModeCall();
			/*std::ostringstream reason;
			reason << "Invalid operation mode: " << opMode;
			reasonString = CORBA::string_dup(reason.str().c_str());
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );*/
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s, R:%lu, A:%lu",
			sessionId, resourceKey, actionKey );
		
		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse rval = TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate;

		try
		{
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"begin privateIsActionPermittedOnResource");

			rval = privateIsActionPermittedOnResource( sessionId, resourceKey, actionKey, reasonString, decisionModule );

			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"end privateIsActionPermittedOnResource");
		}
		catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& e )
		{
			std::ostringstream reason;
			reason << "ESecurityReason value: " << e.reason << ", " << e.description;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationSecurityException", reason.str().c_str() );
			
			std::ostringstream msg;
			msg << "AuthenticationSecurityException: " << reason;
			msg << ", " << e.description;
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& e )
		{
			std::ostringstream reason;
			reason << e.reason;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationAgentException", reason.str().c_str() );
			
			std::ostringstream msg;
			msg << "AuthenticationAgentException: " << reason;
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );

			std::ostringstream msg;
			msg << typeid(e).name() << ": " << e.what();
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
			
			std::ostringstream msg;
			msg << typeid(e).name() << "CORBA::Exception: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
			
			//reasonString = CORBA::string_dup("Unhandled Rights Agent exception");
			reasonString = CORBA::string_dup("未处理的RightsAgent异常");
		}

		return( rval );
    }

	    TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
        RightsAgentImpl::isActionPermittedOnLocSub(
            const char* sessionId,            
            CORBA::ULong actionKey,
			CORBA::ULong locationKey,
			CORBA::ULong subsystemKey,
            CORBA::String_out reasonString, 
            TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
    {
		
		TA_Base_Bus::GenericAgent::ensureControlMode();

		checkOperationMode();

		std::string opMode = TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE);
		decisionModule = TA_Base_Bus::IRightsAgentCorbaDef::Rights;

		if( RPARAM_CONTROL != opMode )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Attempted call on interface in %s mode", opMode.c_str() );

			// Throw an exception for debugging purposes
			throw TA_Base_Bus::IRightsAgentCorbaDef::MonitorModeCall();
			/*std::ostringstream reason;
			reason << "Invalid operation mode: " << opMode;
			reasonString = CORBA::string_dup(reason.str().c_str());
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );*/
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "SES:%s, L:%lu, subsystem:%lu A:%lu",
			sessionId, locationKey, subsystemKey, actionKey );
		
		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse rval = TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate;

		try
		{
			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"begin privateIsActionPermittedOnResource");

			rval = privateIsActionPermittedOnLocSub( sessionId, actionKey, locationKey, subsystemKey, reasonString, decisionModule );

			LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"end privateIsActionPermittedOnResource");
		}
		catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& e )
		{
			std::ostringstream reason;
			reason << "ESecurityReason value: " << e.reason << ", " << e.description;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationSecurityException", reason.str().c_str() );
			
			std::ostringstream msg;
			msg << "AuthenticationSecurityException: " << reason;
			msg << ", " << e.description;
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& e )
		{
			std::ostringstream reason;
			reason << e.reason;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationAgentException", reason.str().c_str() );
			
			std::ostringstream msg;
			msg << "AuthenticationAgentException: " << reason;
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );

			std::ostringstream msg;
			msg << typeid(e).name() << ": " << e.what();
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
			
			std::ostringstream msg;
			msg << typeid(e).name() << "CORBA::Exception: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
			
			//reasonString = CORBA::string_dup("Unhandled Rights Agent exception");
			reasonString = CORBA::string_dup("未处理的RightsAgent异常");
		}

		return( rval );
    }
    TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq* 
        RightsAgentImpl::isActionPermittedOnResources(
            const char* sessionId, 
            const TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq& resourceKeys, 
            CORBA::ULong actionKey, 
            CORBA::String_out reasonString, 
            TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
    {
		
		TA_Base_Bus::GenericAgent::ensureControlMode();

		checkOperationMode();

		std::string opMode = TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE);
		decisionModule = TA_Base_Bus::IRightsAgentCorbaDef::Rights;

		if( RPARAM_CONTROL != opMode )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Attempted call on interface in %s mode", opMode.c_str() );

			// Throw an exception for debugging purposes
			throw TA_Base_Bus::IRightsAgentCorbaDef::MonitorModeCall();
			/*std::ostringstream reason;
			reason << "Invalid operation mode: " << opMode;
			reasonString = CORBA::string_dup(reason.str().c_str());
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );*/
		}
		for(CORBA::ULong seqIdx=0; seqIdx!=resourceKeys.length(); seqIdx++ ){
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s, R:%lu, A:%lu",
				sessionId, resourceKeys[seqIdx], actionKey );
			}
		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse rval = TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate;
		TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq* rlist = new TA_Base_Bus::IRightsAgentCorbaDef::ResourceSeq();
		
		for(CORBA::ULong seqIdx1=0; seqIdx1!=resourceKeys.length(); seqIdx1++ ){
			try
			{
					LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"begin privateIsActionPermittedOnResource");

					rval = privateIsActionPermittedOnResource( sessionId, resourceKeys[seqIdx1], actionKey, reasonString, decisionModule );
					if(rval==TA_Base_Bus::IRightsAgentCorbaDef::Permit){
							int lengthTemp=(*rlist).length();
							(*rlist).length(lengthTemp+1);
							(*rlist)[lengthTemp]=resourceKeys[seqIdx1];
							
						}
					LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"end privateIsActionPermittedOnResource");
			}
			catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& e )
			{
				std::ostringstream reason;
				reason << "ESecurityReason value: " << e.reason << ", " << e.description;

				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationSecurityException", reason.str().c_str() );
				
				std::ostringstream msg;
				msg << "AuthenticationSecurityException: " << reason;
				msg << ", " << e.description;
				reasonString = CORBA::string_dup( msg.str().c_str() );
			}
			catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& e )
			{
				std::ostringstream reason;
				reason << e.reason;

				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationAgentException", reason.str().c_str() );
				
				std::ostringstream msg;
				msg << "AuthenticationAgentException: " << reason;
				reasonString = CORBA::string_dup( msg.str().c_str() );
			}
			catch( const std::exception& e )
			{
				LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );

				std::ostringstream msg;
				msg << typeid(e).name() << ": " << e.what();
				reasonString = CORBA::string_dup( msg.str().c_str() );
			}
			catch( const CORBA::Exception& e )
			{
				LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
				
				std::ostringstream msg;
				msg << typeid(e).name() << "CORBA::Exception: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
				reasonString = CORBA::string_dup( msg.str().c_str() );
			}
			catch( ... )
			{
				LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
				
				//reasonString = CORBA::string_dup("Unhandled Rights Agent exception");
				reasonString = CORBA::string_dup("未处理的RightsAgent异常");
			}
		}
		LOG_GENERIC(SourceInfo,DEBUG_LOG_LEVEL,"isActionPermittedOnResources returns rlist length: %lu",rlist->length());
		return( rlist );
    }
	    TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
        RightsAgentImpl::isActionPermittedOnResourceWithoutDuty(
            const char* sessionId, 
            CORBA::ULong resourceKey, 
            CORBA::ULong actionKey, 
            CORBA::String_out reasonString, 
            TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
    {
		
		TA_Base_Bus::GenericAgent::ensureControlMode();

		checkOperationMode();

		std::string opMode = TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE);
		decisionModule = TA_Base_Bus::IRightsAgentCorbaDef::Rights;

		if( RPARAM_CONTROL != opMode )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Attempted call on interface in %s mode", opMode.c_str() );

			// Throw an exception for debugging purposes
			throw TA_Base_Bus::IRightsAgentCorbaDef::MonitorModeCall();
			/*std::ostringstream reason;
			reason << "Invalid operation mode: " << opMode;
			reasonString = CORBA::string_dup(reason.str().c_str());
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );*/
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s, R:%lu, A:%lu",
			sessionId, resourceKey, actionKey );
		
		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse rval = TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate;

		try
		{
			rval = privateIsActionPermittedOnResourceWithoutDuty( sessionId, resourceKey, actionKey, reasonString, decisionModule );
		}
		catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& e )
		{
			std::ostringstream reason;
			reason << "ESecurityReason value: " << e.reason << ", " << e.description;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationSecurityException", reason.str().c_str() );
			
			std::ostringstream msg;
			msg << "AuthenticationSecurityException: " << reason;
			msg << ", " << e.description;
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& e )
		{
			std::ostringstream reason;
			reason << e.reason;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationAgentException", reason.str().c_str() );
			
			std::ostringstream msg;
			msg << "AuthenticationAgentException: " << reason;
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );

			std::ostringstream msg;
			msg << typeid(e).name() << ": " << e.what();
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
			
			std::ostringstream msg;
			msg << typeid(e).name() << "CORBA::Exception: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
			
			//reasonString = CORBA::string_dup("Unhandled Rights Agent exception");
			reasonString = CORBA::string_dup("未处理的RightsAgent异常");
		}

		return( rval );
    }

	TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
        RightsAgentImpl::privateIsActionPermittedOnResource(
            const char* sessionId, 
            CORBA::ULong resourceKey, 
            CORBA::ULong actionKey, 
            CORBA::String_out reasonString, 
            TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
    {

		checkOperationMode();

		// get list of profiles associated with the sessionId
		// from the Authentication Agent
		std::list<unsigned long> profileList;
		std::list<unsigned long>::const_iterator plIt;
		TA_Base_Bus::SessionInfo sessionInfo;

		populateProfileList( sessionId, profileList,sessionInfo );
		sessionInfo.SessionId = sessionId;
		//sessionInfo.isDisplayOnly=this->m_authAgentLib.getSessionInfo(sessionId,sessionId).isDisplayOnly;
		sessionInfo.isDisplayOnly=false;

		if( profileList.size() == 0 )
		{
			//reasonString = CORBA::string_dup("Invalid session");
			reasonString = CORBA::string_dup("Session是无效的");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );
		}

		decisionModule = TA_Base_Bus::IRightsAgentCorbaDef::Rights;

		plIt = profileList.begin();

		bool atLeastOnePermit        = false;
		bool atLeastOneDeny          = false;
		bool atLeastOneIndeterminate = false;
		bool resultKnown             = false;

		while ( (!resultKnown) && ( plIt != profileList.end() ) )
		{
			std::auto_ptr<TA_Base_Bus::Context> context;

			{
				//C::ThreadGuard guard(m_contextLock);
				std::auto_ptr<TA_Base_Bus::Context> tmp( m_rightsFactory.buildContext( sessionInfo, *plIt, resourceKey, actionKey ) );
				context = tmp;
			}

			if( NULL == context.get() )
			{
				//reasonString = CORBA::string_dup("Failed to allocate context");
				reasonString = CORBA::string_dup("上下文环境出错");
				return TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate;
			}

			// TODO: allow the policy set to return reasons to make the result more
			// useful
			TA_Base_Bus::PdpConst::EPolicyResult pr = TA_Base_Bus::PdpConst::UNDEFINED_PR;

			{
				//C::ThreadGuard guard(m_policyLock);
				pr = m_policy->evaluate( *(context.get()) );
			}

			switch( pr )
			{
				case TA_Base_Bus::PdpConst::PERMIT_PR:
					atLeastOnePermit = true;
					resultKnown = true;
					break;

				case TA_Base_Bus::PdpConst::DENY_PR:
					atLeastOneDeny = true;
					break;

				case TA_Base_Bus::PdpConst::NOT_APPLICABLE_PR:
					break;

				case TA_Base_Bus::PdpConst::INDETERMINATE_PR:
					atLeastOneIndeterminate = true;
					break;

				default:
					atLeastOneIndeterminate = true;
					break;
			}

			plIt++;
		}

		if( atLeastOnePermit )
		{
			reasonString = CORBA::string_dup("");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Permit );
		}

		if( atLeastOneDeny )
		{
			//reasonString = CORBA::string_dup("Insufficient privileges");
			reasonString = CORBA::string_dup("用户没有足够的权限");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Deny );
		}

		if( atLeastOneIndeterminate )
		{
			//reasonString = CORBA::string_dup("Insufficient information available or policy processing error");
			reasonString = CORBA::string_dup("信息不足或策略处理出错");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );
		}

		//reasonString = CORBA::string_dup("No applicable policy");
		reasonString = CORBA::string_dup("没有适合的策略");
		return( TA_Base_Bus::IRightsAgentCorbaDef::NotApplicable );
	}

		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
        RightsAgentImpl::privateIsActionPermittedOnLocSub(
            const char* sessionId,
            CORBA::ULong actionKey, 
			CORBA::ULong locationKey,
			CORBA::ULong subsystemKey,
            CORBA::String_out reasonString, 
            TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
    {
		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"privateIsActionPermittedOnLocSub :   SES: %s , Loc: %lu ,Subsystem:%lu A: %lu", 
			sessionId,locationKey, subsystemKey,actionKey);
		checkOperationMode();
	
		// get list of profiles associated with the sessionId
		// from the Authentication Agent
		std::list<unsigned long> profileList;
		std::list<unsigned long>::const_iterator plIt;
		TA_Base_Bus::SessionInfo sessionInfo;

		populateProfileList( sessionId, profileList,sessionInfo );

		//reset the session info 
		sessionInfo.SessionId = sessionId;
		sessionInfo.isDisplayOnly = false;


		if( profileList.size() == 0 )
		{
			//reasonString = CORBA::string_dup("Invalid session");
			reasonString = CORBA::string_dup("Session是无效的");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );
		}

		decisionModule = TA_Base_Bus::IRightsAgentCorbaDef::Rights;

		plIt = profileList.begin();

		bool atLeastOnePermit        = false;
		bool atLeastOneDeny          = false;
		bool atLeastOneIndeterminate = false;
		bool resultKnown             = false;

		while ( (!resultKnown) && ( plIt != profileList.end() ) )
		{
			std::auto_ptr<TA_Base_Bus::Context> context;

			{
				//C::ThreadGuard guard(m_contextLock);
				std::auto_ptr<TA_Base_Bus::Context> tmp( m_rightsFactory.buildContext( sessionInfo, *plIt, locationKey, subsystemKey, actionKey ) );
				context = tmp;
			}

			if( NULL == context.get() )
			{
				//reasonString = CORBA::string_dup("Failed to allocate context");
				reasonString = CORBA::string_dup("上下文环境出错");
				return TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate;
			}

			// TODO: allow the policy set to return reasons to make the result more
			// useful
			TA_Base_Bus::PdpConst::EPolicyResult pr = TA_Base_Bus::PdpConst::UNDEFINED_PR;

			{
				//C::ThreadGuard guard(m_policyLock);
				pr = m_policy->evaluate( *(context.get()) );
			}

			switch( pr )
			{
				case TA_Base_Bus::PdpConst::PERMIT_PR:
					atLeastOnePermit = true;
					resultKnown = true;
					break;

				case TA_Base_Bus::PdpConst::DENY_PR:
					atLeastOneDeny = true;
					break;

				case TA_Base_Bus::PdpConst::NOT_APPLICABLE_PR:
					break;

				case TA_Base_Bus::PdpConst::INDETERMINATE_PR:
					atLeastOneIndeterminate = true;
					break;

				default:
					atLeastOneIndeterminate = true;
					break;
			}

			plIt++;
		}

		if( atLeastOnePermit )
		{
			reasonString = CORBA::string_dup("");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Permit );
		}

		if( atLeastOneDeny )
		{
			//reasonString = CORBA::string_dup("Insufficient privileges");
			reasonString = CORBA::string_dup("用户没有足够的权限");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Deny );
		}

		if( atLeastOneIndeterminate )
		{
			//reasonString = CORBA::string_dup("Insufficient information available or policy processing error");
			reasonString = CORBA::string_dup("信息不足或策略处理出错");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );
		}

		//reasonString = CORBA::string_dup("No applicable policy");
		reasonString = CORBA::string_dup("没有适合的策略");
		return( TA_Base_Bus::IRightsAgentCorbaDef::NotApplicable );
	}
		TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse 
        RightsAgentImpl::privateIsActionPermittedOnResourceWithoutDuty(
            const char* sessionId, 
            CORBA::ULong resourceKey, 
            CORBA::ULong actionKey, 
            CORBA::String_out reasonString, 
            TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
    {

		//if (isPhysicalResource(resourceKey) == true)
		//{
		//	return privateIsActionPermittedOnResource(sessionId, resourceKey, actionKey, reasonString, decisionModule);
		//}

		checkOperationMode();

		// get list of profiles associated with the sessionId
		// from the Authentication Agent
		std::list<unsigned long> profileList;
		std::list<unsigned long>::const_iterator plIt;
		bool isPermittedOnResource;
		TA_Base_Bus::SessionInfo sessionInfo;
		populateProfileList( sessionId, profileList ,sessionInfo);
		sessionInfo.SessionId=sessionId;
		sessionInfo.isDisplayOnly = false;

		if( profileList.size() == 0 )
		{
			//reasonString = CORBA::string_dup("Invalid session");
			reasonString = CORBA::string_dup("Session是无效的");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );
		}

		decisionModule = TA_Base_Bus::IRightsAgentCorbaDef::Rights;

		plIt = profileList.begin();

		bool atLeastOnePermit        = false;
		bool atLeastOneDeny          = false;
		bool atLeastOneIndeterminate = false;
		bool resultKnown             = false;

		while ( (!resultKnown) && ( plIt != profileList.end() ) )
		{
			std::auto_ptr<TA_Base_Bus::Context> context;

			{
				C::ThreadGuard guard(m_contextLock);
				std::auto_ptr<TA_Base_Bus::Context> tmp( m_rightsFactory.buildContextWoDuty( sessionInfo, *plIt, resourceKey, actionKey ) );
				context = tmp;
			}

			if( NULL == context.get() )
			{
				//reasonString = CORBA::string_dup("Failed to allocate context");
				reasonString = CORBA::string_dup("上下文环境出错");
				return TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate;
			}

			// TODO: allow the policy set to return reasons to make the result more
			// useful
			TA_Base_Bus::PdpConst::EPolicyResult pr = TA_Base_Bus::PdpConst::UNDEFINED_PR;

			{
				C::ThreadGuard guard(m_policyLock);
				pr = m_policy->evaluateWithoutDuty( *(context.get()) );
			}

			switch( pr )
			{
				case TA_Base_Bus::PdpConst::PERMIT_PR:
					atLeastOnePermit = true;
					resultKnown = true;
					break;
					
				case TA_Base_Bus::PdpConst::DENY_PR:
					atLeastOneDeny = true;
					break;

				case TA_Base_Bus::PdpConst::NOT_APPLICABLE_PR:
					break;

				case TA_Base_Bus::PdpConst::INDETERMINATE_PR:
					atLeastOneIndeterminate = true;
					break;

				default:
					atLeastOneIndeterminate = true;
					break;
			}

			plIt++;
		}

		if( atLeastOnePermit )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"atLeastOnePermit, return Permit");
			reasonString = CORBA::string_dup("");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Permit );
		}

		if( atLeastOneDeny )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"atLeastOneDeny, return Deny");
			//reasonString = CORBA::string_dup("Insufficient privileges");
			reasonString = CORBA::string_dup("用户没有足够的权限");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Deny );
		}

		if( atLeastOneIndeterminate )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"atLeastOneIndeterminate, return Indeterminate");
			//reasonString = CORBA::string_dup("Insufficient information available or policy processing error");
			reasonString = CORBA::string_dup("信息不足或策略处理出错");
			return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"return NotApplicable");
		//reasonString = CORBA::string_dup("No applicable policy");
		reasonString = CORBA::string_dup("没有适合的策略");
		return( TA_Base_Bus::IRightsAgentCorbaDef::NotApplicable );
	}

	void RightsAgentImpl::populateProfileList( const char* sessionId, std::list<unsigned long>& profileList,TA_Base_Bus::SessionInfo& aSessionInfo )
	{
		FUNCTION_ENTRY("populateProfileList");

		checkOperationMode();

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Requesting profiles for session %s", sessionId );
		{
			// C::ThreadGuard guard(m_profileCacheLock);

			if( !m_sessionCache.lookup(sessionId, profileList) )
			{
				try
				{
					TA_Base_Bus::SessionInfo sessionInfo = m_authAgentLib.getSessionInfo(sessionId, "no useful id");
					std::vector<unsigned long> & vtProfileId = sessionInfo.ProfileId;
					for( unsigned long i=0; i< vtProfileId.size(); i++ )
					{
						profileList.push_back( vtProfileId[i] );
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "add profile id %lu to profile list", vtProfileId[i] );
					}
					
					m_sessionCache.add( sessionId, profileList );
					ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);
					m_sessionInfoMap.insert(SessionInfoMap::value_type(sessionId, sessionInfo));
					aSessionInfo = sessionInfo;
				}
				catch(TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", ex.reason.in());
				}
				catch(TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationSecurityException", ex.description.in()); 
				}
				catch(CORBA::Exception& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); 
				} 
				catch( const C::ObjectResolutionException& e )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", e.what() );
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "RightsAgentImpl::populateProfileList(","Unknown Exception caught during (*m_authAgent)->getProfiles()");
				}
				
			}
			else
			{
				ACE_Read_Guard<ACE_RW_Mutex> guard(m_mutex);

				SessionInfoMap::const_iterator itr = m_sessionInfoMap.find(sessionId);
				if(itr !=m_sessionInfoMap.end() )
				{
					aSessionInfo = itr->second;
				}
			}
		}

		FUNCTION_EXIT;
	}

	void RightsAgentImpl::checkOperationMode()
	{
		TA_ASSERT(m_parentClass != NULL, "parent class should not be null");
		m_parentClass->checkOperationMode();
	}
TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq*
RightsAgentImpl::areActionsPermittedOnResource(
	const char* sessionId, 
	CORBA::ULong resourceKey, 
	const ::TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq& actionKeys,
	CORBA::String_out reasonString, 
	TA_Base_Bus::IRightsAgentCorbaDef::EDecisionModule& decisionModule)
{

	TA_Base_Bus::GenericAgent::ensureControlMode();

	checkOperationMode();

	std::string opMode = TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE);
	decisionModule = TA_Base_Bus::IRightsAgentCorbaDef::Rights;

	if( RPARAM_CONTROL != opMode )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Attempted call on interface in %s mode", opMode.c_str() );

		// Throw an exception for debugging purposes
		throw TA_Base_Bus::IRightsAgentCorbaDef::MonitorModeCall();
		/*std::ostringstream reason;
		reason << "Invalid operation mode: " << opMode;
		reasonString = CORBA::string_dup(reason.str().c_str());
		return( TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate );*/
	}
	for(CORBA::ULong seqIdx=0; seqIdx!=actionKeys.length(); seqIdx++ ){
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s, R:%lu, A:%lu",
		sessionId, resourceKey, actionKeys[seqIdx] );
	}
	TA_Base_Bus::IRightsAgentCorbaDef::ERightsResponse rval = TA_Base_Bus::IRightsAgentCorbaDef::Indeterminate;


	TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq* rlist = new TA_Base_Bus::IRightsAgentCorbaDef::ActionSeq();

	for(CORBA::ULong seqIdx1=0; seqIdx1!=actionKeys.length(); seqIdx1++ ){
		try
		{			
				rval = privateIsActionPermittedOnResource( sessionId, resourceKey, (actionKeys)[seqIdx1], reasonString, decisionModule );
				//check rval, push "permit" action into return list
				if(TA_Base_Bus::IRightsAgentCorbaDef::Permit==rval){
					int lengthTemp=(*rlist).length();
					(*rlist).length(lengthTemp+1);
					(*rlist)[lengthTemp]=actionKeys[seqIdx1];
					
				}
		}
		catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationSecurityException& e )
		{
			std::ostringstream reason;
			reason << "ESecurityReason value: " << e.reason << ", " << e.description;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationSecurityException", reason.str().c_str() );

			std::ostringstream msg;
			msg << "AuthenticationSecurityException: " << reason;
			msg << ", " << e.description;
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const TA_Base_Bus::IAuthenticationAgentCorbaDef::AuthenticationAgentException& e )
		{
			std::ostringstream reason;
			reason << e.reason;

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "AuthenticationAgentException", reason.str().c_str() );

			std::ostringstream msg;
			msg << "AuthenticationAgentException: " << reason;
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );

			std::ostringstream msg;
			msg << typeid(e).name() << ": " << e.what();
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );

			std::ostringstream msg;
			msg << typeid(e).name() << "CORBA::Exception: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
			reasonString = CORBA::string_dup( msg.str().c_str() );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );

			//reasonString = CORBA::string_dup("Unhandled Rights Agent exception");
			reasonString = CORBA::string_dup("未处理的RightsAgent异常");
		}
	}
	//for(CORBA::ULong idx=0; idx<rlist->length(); idx++ ){
	//	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL,"%d   in rlist is %u",idx,(*rlist)[idx]);
	//}
	return( rlist );
}

//void RightsAgentImpl::getSubsystem( TA_Base_Core::IEntityData* resource, unsigned long& subsystem,
//									   bool& isPhysical ) const
//{
//	TA_ASSERT( NULL != resource, "resource == NULL" );
//
//	unsigned long tmpSub = 0xFFFFFFFF;
//
//	tmpSub = resource->getPhysicalSubsystem();
//
//	if( 0 == tmpSub )
//	{
//		subsystem     = resource->getSubsystem();
//		isPhysical    = false;
//		return;
//	}
//
//	subsystem     = resource->getPhysicalSubsystem();
//	isPhysical    = true;
//}
//
//bool RightsAgentImpl::isPhysicalResource( unsigned long resource )
//{
//	bool isPhysical = false;
//	unsigned long subsystem = 0xFFFFFFFF;
//
//	std::auto_ptr<TA_Base_Core::IEntityData> newResource(
//		TA_Base_Core::EntityAccessFactory::getInstance().getEntity( resource ));
//
//	getSubsystem( newResource.get(), subsystem, isPhysical );
//	LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "resource: %lu, subsystem: %lu, isPhysical: %s",
//		resource, subsystem, (isPhysical==true)?"yes":"no");
//
//	return( isPhysical );
//}

}
