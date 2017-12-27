/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/RemoteDutyPeer.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */
#include "DutyAgentState.h"

using namespace TA_Base_Core;
namespace TA_Base_Bus
{

	const unsigned long RemoteDutyPeer::TIME_OF_FLIGHT=10;
	const unsigned long RemoteDutyPeer::MAX_PEER_TIMEOUT = 300;

	RemoteDutyPeer::RemoteDutyPeer( const DASType::DbKey& location ) :
		m_peer(NULL), m_peerLocation(location)
	{
		std::string agentType = TA_Base_Core::DutyAgentEntityData::getStaticType();

		TA_Base_Core::CorbaNameList dutyAgent;

		dutyAgent = 
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( agentType, location,true );
		
		if( 0 >= dutyAgent.size() )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "No %s found at location %d", agentType.c_str(), location );
		}
		else
		{
			m_peer = new DASType::DutyAgentPeer( dutyAgent[0] );

			if( NULL != m_peer )
			{
				//unsigned long peerTimeout = TIME_OF_FLIGHT; 
				unsigned long peerTimeout =DutyDbCache::instance()->getPopupTimeout().sec() + TIME_OF_FLIGHT;
				
				if( peerTimeout > MAX_PEER_TIMEOUT )
				{
					peerTimeout = MAX_PEER_TIMEOUT;
				}

				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Setting remote peer timeout to %ds", peerTimeout );
				m_peer->setObjectTimeout(static_cast<unsigned short>(peerTimeout));
			}
		}
	}

	RemoteDutyPeer::~RemoteDutyPeer()
	{
		delete m_peer;
	}

	
	void RemoteDutyPeer::sessionBegin( DutyState& state, const SessionInfo& session )
	{
		FUNCTION_ENTRY("sessionBegin");
		if( NULL == m_peer )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Ignoring call to non-existant Duty Agent" );
			return;
		}

        try
        {
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin RemoteDutyPeer::sessionBegin at location %d", m_peerLocation);
		    DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		    IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
			    AuthenticationLibrary::convertCSessionToCORBASession(session);

            CORBA_CALL( (*m_peer), peerSessionBegin, ( corbaState.out(), corbaSession ) );

		    state.set(corbaState.in());
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end RemoteDutyPeer::sessionBegin at location %d", m_peerLocation);
        }
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::BadParameterException& ex )
        {
            std::stringstream msg;
            msg << "BadParameterException (" << std::string(ex.reason) << ") while calling peerSessionBegin for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::OperationModeException& ex )
        {
            std::stringstream msg;
            msg << "OperationModeException (" << std::string(ex.reason) << ") while calling peerSessionBegin for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );         
        }
        catch( DutyAgentTypeCorbaDef::UnhandledException& ex )
        {
            std::stringstream msg;
            msg << "UnhandledException (" << std::string(ex.reason) << ") while calling peerSessionBegin for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( CORBA::Exception& ex )
        {
            std::stringstream msg;
            msg << "CORBA::Exception (" << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) << ") while calling peerSessionBegin for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }

		FUNCTION_EXIT;
	}

	void RemoteDutyPeer::sessionEnd( DutyState& state, const SessionInfo& session,
		const DASType::RegionSet& regions )
	{
		FUNCTION_ENTRY("sessionEnd");
		if( NULL == m_peer )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Ignoring call to non-existant Duty Agent" );
			return;
		}

        try
        {
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin RemoteDutyPeer::sessionEnd at location %d", m_peerLocation);
		    DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		    IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
			    AuthenticationLibrary::convertCSessionToCORBASession(session);

		    DutyAgentTypeCorbaDef::KeySequence regionSeq;
		    regionSeq.length( regions.size() );

		    DASType::RegionSet::const_iterator it;
		    CORBA::ULong i = 0;
		    for( it = regions.begin(); it != regions.end(); it++, i++ )
		    {
			    regionSeq[i] = *it;	
		    }

            CORBA_CALL( (*m_peer), peerSessionEnd, ( corbaState.out(), corbaSession, regionSeq ) );

		    state.set(corbaState.in());

			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end RemoteDutyPeer::sessionEnd at location %d", m_peerLocation);
        }
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::BadParameterException& ex )
        {
            std::stringstream msg;
            msg << "BadParameterException (" << std::string(ex.reason) << ") while calling peerSessionEnd for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::OperationModeException& ex )
        {
            std::stringstream msg;
            msg << "OperationModeException (" << std::string(ex.reason) << ") while calling peerSessionEnd for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );         
        }
        catch( DutyAgentTypeCorbaDef::UnhandledException& ex )
        {
            std::stringstream msg;
            msg << "UnhandledException (" << std::string(ex.reason) << ") while calling peerSessionEnd for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( CORBA::Exception& ex )
        {
            std::stringstream msg;
            msg << "CORBA::Exception (" << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) << ") while calling peerSessionEnd for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }

		FUNCTION_EXIT;
	}

	void RemoteDutyPeer::sessionOverrideBegin( DutyState& state, const SessionInfo& session )
	{
		FUNCTION_ENTRY("sessionOverrideBegin");
		if( NULL == m_peer )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Ignoring call to non-existant Duty Agent" );
			return;
		}

        try
        {
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin RemoteDutyPeer::sessionOverrideBegin at location %d", m_peerLocation);

		    DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		    IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
			    AuthenticationLibrary::convertCSessionToCORBASession(session);

            CORBA_CALL( (*m_peer), peerSessionOverrideBegin, ( corbaState.out(), corbaSession ) );

		    state.set(corbaState.in());

			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end RemoteDutyPeer::sessionOverrideBegin at location %d", m_peerLocation);
        }
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::BadParameterException& ex )
        {
            std::stringstream msg;
            msg << "BadParameterException (" << std::string(ex.reason) << ") while calling peerSessionOverrideBegin for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::OperationModeException& ex )
        {
            std::stringstream msg;
            msg << "OperationModeException (" << std::string(ex.reason) << ") while calling peerSessionOverrideBegin for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );         
        }
        catch( DutyAgentTypeCorbaDef::UnhandledException& ex )
        {
            std::stringstream msg;
            msg << "UnhandledException (" << std::string(ex.reason) << ") while calling peerSessionOverrideBegin for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( CORBA::Exception& ex )
        {
            std::stringstream msg;
            msg << "CORBA::Exception (" << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) << ") while calling peerSessionOverrideBegin for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }

		FUNCTION_EXIT;
	}

	void RemoteDutyPeer::sessionOverrideEnd( DutyState& state, const SessionInfo& session )
	{
		FUNCTION_ENTRY("sessionOverrideEnd");
		if( NULL == m_peer )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Ignoring call to non-existant Duty Agent" );
			return;
		}

        try
        {
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin RemoteDutyPeer::sessionOverrideEnd at location %d", m_peerLocation);
		    DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		    IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
			    AuthenticationLibrary::convertCSessionToCORBASession(session);

            CORBA_CALL( (*m_peer), peerSessionOverrideEnd, ( corbaState.out(), corbaSession ) );

		    state.set(corbaState.in());
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end RemoteDutyPeer::sessionOverrideEnd at location %d", m_peerLocation);

        }
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::BadParameterException& ex )
        {
            std::stringstream msg;
            msg << "BadParameterException (" << std::string(ex.reason) << ") while calling peerSessionOverrideEnd for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::OperationModeException& ex )
        {
            std::stringstream msg;
            msg << "OperationModeException (" << std::string(ex.reason) << ") while calling peerSessionOverrideEnd for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );         
        }
        catch( DutyAgentTypeCorbaDef::UnhandledException& ex )
        {
            std::stringstream msg;
            msg << "UnhandledException (" << std::string(ex.reason) << ") while calling peerSessionOverrideEnd for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( CORBA::Exception& ex )
        {
            std::stringstream msg;
            msg << "CORBA::Exception (" << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) << ") while calling peerSessionOverrideEnd for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }

		FUNCTION_EXIT;
	}
	
	void RemoteDutyPeer::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session,
		const DASType::RegionSet& regions )
	{
		FUNCTION_ENTRY("transfer");
		if( NULL == m_peer )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Ignoring call to non-existant Duty Agent" );
			return;
		}

		if( regions.size() <= 0 )
		{
			return;
		}

        try
        {
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin RemoteDutyPeer::transferRegion at location %d", m_peerLocation);
		    DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		    DutyAgentTypeCorbaDef::KeySequence regionSeq;
		    regionSeq.length( regions.size() );

		    DASType::RegionSet::const_iterator it;
		    CORBA::ULong i = 0;
		    for( it = regions.begin(); it != regions.end(); it++, i++ )
		    {
			    regionSeq[i] = *it;	
		    }

			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = 
				TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session);

            CORBA_CALL( (*m_peer),
                        peerTransferRegions,
                        ( corbaState.out(), state.getAccessControlSession().c_str(), corbaSession, regionSeq ) );

		    state.set(corbaState.in());
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end RemoteDutyPeer::transferRegion at location %d", m_peerLocation);
        }
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::BadParameterException& ex )
        {
            std::stringstream msg;
            msg << "BadParameterException (" << std::string(ex.reason) << ") while calling peerTransferRegions for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::DatabaseErrorException& ex )
        {
            std::stringstream msg;
            msg << "DatabaseErrorException (" << std::string(ex.reason) << ") while calling peerTransferRegions for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::OperationModeException& ex )
        {
            std::stringstream msg;
            msg << "OperationModeException (" << std::string(ex.reason) << ") while calling peerTransferRegions for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );         
        }
        catch( DutyAgentTypeCorbaDef::UnhandledException& ex )
        {
            std::stringstream msg;
            msg << "UnhandledException (" << std::string(ex.reason) << ") while calling peerTransferRegions for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( CORBA::Exception& ex )
        {
            std::stringstream msg;
            msg << "CORBA::Exception (" << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) << ") while calling peerTransferRegions for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }

		FUNCTION_EXIT;
	}

	void RemoteDutyPeer::transfer( DutyState& state, const  TA_Base_Bus::SessionInfo& session,
		const DASType::SubsystemMap& subsystems )
	{
		FUNCTION_ENTRY("transfer");
		if( NULL == m_peer )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Ignoring call to non-existant Duty Agent" );
			return;
		}

		if( subsystems.size() <= 0 )
		{
			return;
		}

        try
        {
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin RemoteDutyPeer::transferSubsystem at location %d", m_peerLocation);

		    DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		    DutyAgentTypeCorbaDef::SubsystemTreeSequence subTree;
		    subTree.length( subsystems.size() );

		    CORBA::ULong regIdx = 0;
		    CORBA::ULong subIdx = 0;

		    DASType::SubsystemMap::const_iterator regIt;
		    DASType::SubsystemSet::const_iterator subIt;

		    for( regIt=subsystems.begin(); regIt!=subsystems.end(); regIt++, regIdx++ )
		    {
			    subTree[regIdx].region = regIt->first;
			    subTree[regIdx].subsystems.length( regIt->second.size() );

			    for( subIt=regIt->second.begin(), subIdx=0; subIt!=regIt->second.end(); subIt++, subIdx++ )
			    {
				    subTree[regIdx].subsystems[subIdx] = *subIt;
			    }
		    }

            // log audit
            typedef std::map<TA_Base_Bus::DASType::DbKey, TA_Base_Bus::DutyState::Duty> DutyMap;
            DutyMap dutyMap = state.getDutyMap();

			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = 
				TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(session);
            CORBA_CALL( (*m_peer),
                        peerTransferSubsystems,
                        ( corbaState.out(), corbaSession, subTree, state.getInitiatingAction().c_str() ) );

		    state.set( corbaState.in() );

			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end RemoteDutyPeer::transferSubsystem at location %d", m_peerLocation);
        }
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::BadParameterException& ex )
        {
            std::stringstream msg;
            msg << "BadParameterException (" << std::string(ex.reason) << ") while calling peerTransferSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::DatabaseErrorException& ex )
        {
            std::stringstream msg;
            msg << "DatabaseErrorException (" << std::string(ex.reason) << ") while calling peerTransferSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::OperationModeException& ex )
        {
            std::stringstream msg;
            msg << "OperationModeException (" << std::string(ex.reason) << ") while calling peerTransferSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );         
        }
        catch( DutyAgentTypeCorbaDef::UnhandledException& ex )
        {
            std::stringstream msg;
            msg << "UnhandledException (" << std::string(ex.reason) << ") while calling peerTransferSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( CORBA::Exception& ex )
        {
            std::stringstream msg;
            msg << "CORBA::Exception (" << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) << ") while calling peerTransferSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }

		FUNCTION_EXIT;
	}
	
	void RemoteDutyPeer::executeQuery( DutyState& state, IDutyQuery& query )
	{
		FUNCTION_ENTRY("executeQuery");
		if( NULL == m_peer )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Ignoring call to non-existant Duty Agent" );
			return;
		}

        try
        {
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin RemoteDutyPeer::executeQuery at location %d", m_peerLocation);
		    query.execute( state, m_peer );
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end RemoteDutyPeer::executeQuery at location %d", m_peerLocation);
        }
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::BadParameterException& ex )
        {
            std::stringstream msg;
            msg << "BadParameterException (" << std::string(ex.reason) << ") while executing a query for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::DatabaseErrorException& ex )
        {
            std::stringstream msg;
            msg << "DatabaseErrorException (" << std::string(ex.reason) << ") while executing a query for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::OperationModeException& ex )
        {
            std::stringstream msg;
            msg << "OperationModeException (" << std::string(ex.reason) << ") while executing a query for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );         
        }
        catch( DutyAgentTypeCorbaDef::UnhandledException& ex )
        {
            std::stringstream msg;
            msg << "UnhandledException (" << std::string(ex.reason) << ") while executing a query for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( CORBA::Exception& ex )
        {
            std::stringstream msg;
            msg << "CORBA::Exception (" << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) << ") while executing a query for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }

		FUNCTION_EXIT;
	}

	void RemoteDutyPeer::degrade( DutyState& state, const DASType::SubsystemSet& subsystems )
	{
		FUNCTION_ENTRY("degrade");

		if( NULL == m_peer )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Ignoring call to non-existant Duty Agent" );
			return;
		}

		if( subsystems.size() <= 0 )
		{
			return;
		}

        try
        {
			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin RemoteDutyPeer::degrade at location %d ", m_peerLocation);

		    DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		    DutyAgentTypeCorbaDef::KeySequence subsystemSeq;
		    subsystemSeq.length( subsystems.size() );

		    DASType::SubsystemSet::const_iterator it;
		    CORBA::ULong i = 0;
		    for( it = subsystems.begin(); it != subsystems.end(); it++, i++ )
		    {
			    subsystemSeq[i] = *it;	
		    }

            CORBA_CALL( (*m_peer),
                        peerDegradeSubsystems,
                        ( corbaState.out(), state.getInitiatingSession().c_str(), subsystemSeq ) );

		    state.set(corbaState.in());

			LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end RemoteDutyPeer::degrade at location %d", m_peerLocation);
        }
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::BadParameterException& ex )
        {
            std::stringstream msg;
            msg << "BadParameterException (" << std::string(ex.reason) << ") while calling peerDegradeSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::DatabaseErrorException& ex )
        {
            std::stringstream msg;
            msg << "DatabaseErrorException (" << std::string(ex.reason) << ") while calling peerDegradeSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( TA_Base_Core::OperationModeException& ex )
        {
            std::stringstream msg;
            msg << "OperationModeException (" << std::string(ex.reason) << ") while calling peerDegradeSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );         
        }
        catch( DutyAgentTypeCorbaDef::UnhandledException& ex )
        {
            std::stringstream msg;
            msg << "UnhandledException (" << std::string(ex.reason) << ") while calling peerDegradeSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }
        catch( CORBA::Exception& ex )
        {
            std::stringstream msg;
            msg << "CORBA::Exception (" << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) << ") while calling peerDegradeSubsystems for location "
                << m_peerLocation;
            throw TA_Base_Core::DutyAgentException( msg.str() );
        }

		FUNCTION_EXIT;
	}
}
