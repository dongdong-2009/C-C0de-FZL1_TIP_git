// SpecialRightLibrary.cpp: implementation of the SpecialRightLibrary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "security_simulator.h"
#include "SpecialRightLibrary.h"


#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace
{
    const unsigned long INVALID_KEY( 0xFFFFFFFF );
}

namespace TA_Base_Bus
{
	
	SpecialRightLibrary::SpecialRightLibrary(std::string& entityName) :
		m_rightsAgent(NULL),
		m_cache(m_sessionUpdates),
		m_subscriber(m_cache),
        m_superProfileKey(INVALID_KEY)
    {
		FUNCTION_ENTRY("SpecialRightLibrary");

		m_rightsAgent = new RightsAgentType( entityName );

        FUNCTION_EXIT;
	}

	SpecialRightLibrary::~SpecialRightLibrary()
	{
		FUNCTION_ENTRY("~SpecialRightLibrary");
		delete m_rightsAgent;

        FUNCTION_EXIT;
	}

	bool SpecialRightLibrary::isActionPermittedOnResource(
			const std::string& sessionId, unsigned long resourceKey,
			unsigned long actionKey, std::string& reasonString,
			EDecisionModule& decisionModule )
	{
		FUNCTION_ENTRY("isActionPermittedOnResource");

		LOG(SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s,R:%lu,A:%lu", sessionId.c_str(),
			resourceKey, actionKey);

		// initialise the client's return values
		reasonString = "";
		bool response  = false;
		decisionModule = UNDEFINED_DM;

		// initialise our return values
		CORBA::String_var corbaReason;
		IRightsAgentCorbaDef::EDecisionModule corbaDm = IRightsAgentCorbaDef::Rights;
		IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;

		try
		{
			{
				TA_Base_Core::ThreadGuard guard(m_cacheLock);
				// check the cache
				if( m_cache.lookup(sessionId, resourceKey, actionKey, response, reasonString, decisionModule) )
				{
					FUNCTION_EXIT;
					return( response );
				}
			}

            // TD11013: Now check to see if the request requires any of the 'super' behaviour.
            if ( isSuperProfile( sessionId ) || isSuperSession( sessionId ) )
            {
                // Create some fake query results.
                reasonString = "Privileged session, result is always true.";
                response = true;
                decisionModule = RIGHTS_DM;

                // Add it to the cache and then return true.
                {
			        TA_Base_Core::ThreadGuard guard(m_cacheLock);
			        m_cache.add( sessionId, resourceKey, actionKey, response, reasonString, decisionModule );
                }

                FUNCTION_EXIT;
                return response;
            }

            
			// make the call!
			int tryCount(0);
			bool tryAgain( true );
			
			const int MAX_RETRIES=2;
			// Begin the retry-loop
			while ( tryAgain )
			{
				try
				{
					// Increment the counter
					tryCount++;

					if ( tryCount > MAX_RETRIES )
					{
						// The tryCount will only ever reach the third time if we have a MonitorModeCalled exception!
						LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Monitor Mode exception caught twice!" );

						// Fill out the error message
						corbaReason = CORBA::string_dup("Attempted call on interface in Monitor mode");

						// We don't want to retry
						tryAgain = false;
					}
					else
					{
                        CORBA_CALL_RETURN( corbaRsp,
                                           (*m_rightsAgent),
                                           isActionPermittedOnResource,
                                           ( sessionId.c_str(), resourceKey, actionKey, corbaReason.out(), corbaDm ) );

						// We don't want to retry
						tryAgain = false;
					}

				}
				catch ( const IRightsAgentCorbaDef::MonitorModeCall& )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "OperationModeException:", "Agent reporting monitor mode! Invalidating NamedObject and Trying again" );
					
					//m_rightsAgent->invalidateObject( m_rightsAgent->getAgentName() );
				}
			}
        }
        catch( const TA_Base_Core::ObjectResolutionException& ex )
        {
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", ex.what() );
			FUNCTION_EXIT;
            TA_THROW( TA_Base_Core::RightsException(ex.what(), TA_Base_Core::RightsException::UNCONTACTABLE) );
        }
		catch( const TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", ex.what() );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what()) );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str()) );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
		}

		// assign the string
		reasonString = corbaReason.in();

		// convert the response
		switch (corbaRsp)
		{
			case IRightsAgentCorbaDef::Permit:
				response = true;
				break;

			case IRightsAgentCorbaDef::Deny:
				response = false;
				break;
			
			case IRightsAgentCorbaDef::Indeterminate:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::INDETERMINATE) );
				break;
			
			case IRightsAgentCorbaDef::NotApplicable:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::UNAPPLICABLE) );
				break;

			default:
				break;
		}

		// convert the decision module
		switch (corbaDm)
		{
			case IRightsAgentCorbaDef::Rights:
				decisionModule = RIGHTS_DM;
				break;

			case IRightsAgentCorbaDef::Mutex:
				decisionModule = MUTEX_DM;
				break;
		
			default:
				break;
		}

		// remember it
		{
			TA_Base_Core::ThreadGuard guard(m_cacheLock);
			m_cache.add( sessionId, resourceKey, actionKey, response, reasonString, decisionModule );
		}
		
		FUNCTION_EXIT;
		return( response );
	}

	bool SpecialRightLibrary::isActionPermittedOnResourceAndLocSub(
			const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
			unsigned long actionKey, std::string& reasonString,
			EDecisionModule& decisionModule )
	{
		FUNCTION_ENTRY("isActionPermittedOnResource");

		LOG(SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "S:%s,R:%lu,A:%lu", sessionId.c_str(),
			resourceKey, actionKey);

		// initialise the client's return values
		reasonString = "";
		bool response  = false;
		decisionModule = UNDEFINED_DM;

		// initialise our return values
		CORBA::String_var corbaReason;
		IRightsAgentCorbaDef::EDecisionModule corbaDm = IRightsAgentCorbaDef::Rights;
		IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;

		try
		{
			{
				TA_Base_Core::ThreadGuard guard(m_cacheLock);
				// check the cache
				if( m_cache.lookup(sessionId, locationKey, subsystemKey, actionKey, response, reasonString, decisionModule) )
				{
					FUNCTION_EXIT;
					return( response );
				}
			}

            // TD11013: Now check to see if the request requires any of the 'super' behaviour.
            if ( isSuperProfile( sessionId ) || isSuperSession( sessionId ) )
            {
                // Create some fake query results.
                reasonString = "Privileged session, result is always true.";
                response = true;
                decisionModule = RIGHTS_DM;

                // Add it to the cache and then return true.
                {
			        TA_Base_Core::ThreadGuard guard(m_cacheLock);
			        m_cache.add( sessionId, locationKey, subsystemKey, actionKey, response, reasonString, decisionModule );
                }

                FUNCTION_EXIT;
                return response;
            }

            
			// make the call!
			int tryCount(0);
			bool tryAgain( true );
			
			const int MAX_RETRIES=2;
			// Begin the retry-loop
			while ( tryAgain )
			{
				try
				{
					// Increment the counter
					tryCount++;

					if ( tryCount > MAX_RETRIES )
					{
						// The tryCount will only ever reach the third time if we have a MonitorModeCalled exception!
						LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Monitor Mode exception caught twice!" );

						// Fill out the error message
						corbaReason = CORBA::string_dup("Attempted call on interface in Monitor mode");

						// We don't want to retry
						tryAgain = false;
					}
					else
					{
                        CORBA_CALL_RETURN( corbaRsp,
                                           (*m_rightsAgent),
                                           isActionPermittedOnResource,
                                           ( sessionId.c_str(), resourceKey, actionKey, corbaReason.out(), corbaDm ) );

						// We don't want to retry
						tryAgain = false;
					}

				}
				catch ( const IRightsAgentCorbaDef::MonitorModeCall& )
				{
					LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "OperationModeException:", "Agent reporting monitor mode! Invalidating NamedObject and Trying again" );
					
					//m_rightsAgent->invalidateObject( m_rightsAgent->getAgentName() );
				}
			}
        }
        catch( const TA_Base_Core::ObjectResolutionException& ex )
        {
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", ex.what() );
			FUNCTION_EXIT;
            TA_THROW( TA_Base_Core::RightsException(ex.what(), TA_Base_Core::RightsException::UNCONTACTABLE) );
        }
		catch( const TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", ex.what() );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what()) );
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str()) );
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled" );
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
		}

		// assign the string
		reasonString = corbaReason.in();

		// convert the response
		switch (corbaRsp)
		{
			case IRightsAgentCorbaDef::Permit:
				response = true;
				break;

			case IRightsAgentCorbaDef::Deny:
				response = false;
				break;
			
			case IRightsAgentCorbaDef::Indeterminate:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::INDETERMINATE) );
				break;
			
			case IRightsAgentCorbaDef::NotApplicable:
                TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::UNAPPLICABLE) );
				break;

			default:
				break;
		}

		// convert the decision module
		switch (corbaDm)
		{
			case IRightsAgentCorbaDef::Rights:
				decisionModule = RIGHTS_DM;
				break;

			case IRightsAgentCorbaDef::Mutex:
				decisionModule = MUTEX_DM;
				break;
		
			default:
				break;
		}

		// remember it
		{
			TA_Base_Core::ThreadGuard guard(m_cacheLock);
			m_cache.add( sessionId, resourceKey, actionKey, response, reasonString, decisionModule );
		}
		
		FUNCTION_EXIT;
		return( response );
	}


	void SpecialRightLibrary::resetCache()
	{
		TA_Base_Core::ThreadGuard guard(m_cacheLock);
		m_cache.reset();
	}

    bool SpecialRightLibrary::isSuperProfile( const std::string& sessionId )
    {
        try
        {
            if( INVALID_KEY == m_superProfileKey )
            {
                std::auto_ptr<TA_Base_Core::IProfile> superProfile( TA_Base_Core::ProfileAccessFactory::getInstance().getTransActiveSuperProfile() );
                m_superProfileKey = superProfile->getKey();
            }
            if ( INVALID_KEY != m_superProfileKey )
            {
                // TD11013: Use the authentication library to get session information.
                SessionInfo sessionInfo = m_authLibrary.getSessionInfo( sessionId, sessionId );

                // Check the super profile against all profiles associated with the session.
                std::vector<unsigned long>::iterator it;
                for ( it=sessionInfo.ProfileId.begin(); it!=sessionInfo.ProfileId.end(); it++ )
                {
                    if( *it == m_superProfileKey )
                    {
                        return true;
                    }
                }
            }
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown");
        }

        return false;
    }

    bool SpecialRightLibrary::isSuperSession( const std::string& sessionId )
    {
        return ( sessionId == TA_Base_Core::SessionAccessFactory::getSuperSessionId() );
    }
}
