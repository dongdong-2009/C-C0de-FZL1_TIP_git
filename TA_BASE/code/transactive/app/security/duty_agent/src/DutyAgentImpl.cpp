/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyAgentImpl.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the TA_Base_Bus::IDutyAgentCorbaDef interface.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyAgentImpl.h"
#include "app/security/duty_agent/src/IDutyAgentExecutive.h"
#include "app/security/duty_agent/src/DutyManagerSnapshotConverter.h"
#include "app/security/duty_agent/src/TransferableRegionStateConverter.h" 
#include "app/security/duty_agent/src/TransferableSubsystemStateConverter.h"
#include "app/security/duty_agent/src/CorbaDutyStateConverter.h" 
#include "app/security/duty_agent/src/GetDutyConverter.h" 
#include "app/security/duty_agent/src/GetExclusiveDutyConverter.h" 
#include "app/security/duty_agent/src/SubsystemStateSnapshotConverter.h"
#include "app/security/duty_agent/src/SubsystemStateUpdateConverter.h"
#include "app/security/duty_agent/src/AllocateDAC.h"
#include "app/security/duty_agent/src/DegradeDAC.h"
#include "app/security/duty_agent/src/FreeDAC.h"
#include "app/security/duty_agent/src/GetStateDAC.h"
#include "app/security/duty_agent/src/GetSessionStateDAC.h"
#include "app/security/duty_agent/src/GetSubsystemStateDAC.h"
#include "app/security/duty_agent/src/GetTransferableRegionsDAC.h"
#include "app/security/duty_agent/src/GetTransferableSubsystemsDAC.h"
#include "app/security/duty_agent/src/HasDutyDAC.h"
#include "app/security/duty_agent/src/HasExclusiveDutyDAC.h"
#include "app/security/duty_agent/src/OverrideBeginDAC.h"
#include "app/security/duty_agent/src/OverrideEndDAC.h"
#include "app/security/duty_agent/src/TransferRegionDAC.h"
#include "app/security/duty_agent/src/TransferSubsystemDAC.h"
#include "core/utilities/src/Timer.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::RunParams;

namespace TA_Base_App
{

    DutyAgentImpl::DutyAgentImpl(const std::string& agentName) :
		m_rightsLib(NULL),
		m_resourceKey(ULONG_MAX),
		m_executive(NULL),
		m_pDutyManagerSender(new DutyManagerUpdateSender()),
		m_pPopupSender(new PopupMessageSender()),
		m_pAlarmSender(new DutyAlarmSender()),
		m_pAuditSender(new DutyAuditMessageSender()),
		m_pStateUpdateSender(new StateUpdateSender()),
		m_pDutyMonitorManager(new DutyMonitorManager()),
		m_pSubsystemStateUpdateSender(new SubsystemStateUpdateSender())
    {
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::DutyAgentImpl");

		std::auto_ptr<TA_Base_Core::IEntityData> ieData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(agentName));
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after get entity of %s", agentName.c_str());
		std::auto_ptr<TA_Base_Core::IResource>   iRes  (TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( ieData->getAgent() ) );

		m_resourceKey = iRes->getKey();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after get resource of %d", m_resourceKey);
		TA_Base_Bus::RightsLibraryFactory rlf;

		m_rightsLib = rlf.buildRightsLibrary();
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after buildRightsLibrary");

		TA_ASSERT( NULL != m_rightsLib, "m_rightsLib == NULL" );

        activateServantWithName( agentName );
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after activateServantWithName %s",agentName.c_str());

		m_feLocSet.insert( TA_Base_Bus::DASConst::ALL_LOCATIONS );
		m_localLocSet.insert( TA_Base_Bus::TransactiveAgentInfo::m_location );

		TA_Base_Bus::TransactiveAgentInfo::m_responseObjRef = _this();
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::DutyAgentImpl");
    }

    DutyAgentImpl::~DutyAgentImpl()
    {
		delete m_rightsLib;
		m_rightsLib = NULL;
		
		delete m_executive;
		m_executive = NULL;
    }

	void DutyAgentImpl::setExecutive( IDutyAgentExecutive* exec )
	{
		FUNCTION_ENTRY( "setExecutive" );

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_executiveLock);
		if( (exec != NULL) && (m_executive != exec) )
		{
			delete m_executive;
			m_executive = exec;
		}

		FUNCTION_EXIT;
	}

	// ***** local *****

	void DutyAgentImpl::localDegrade()
	{
		FUNCTION_ENTRY( "localDegrade" );

		try
		{
			// TA_Base_Bus::DutyState dutyState( "", DutyAction::SYSTEMWIDE_DELEGATION );
			TA_Base_Bus::DutyState dutyState( "", DutyAction::AUTOMATIC_DELEGATION );

			TA_Base_Bus::DASType::SubsystemSet subsystems;
			subsystems.insert( TA_Base_Bus::DASConst::ALL_SUBSYSTEMS );

			m_taskManager.degrade( dutyState, subsystems, m_localLocSet );

			TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
		}
		catch( TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
			throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
		}
		catch(...)
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Unknown exception caught at top level, DutyAgentImpl::localDegrade." );
		}

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::localSendBulkUpdate()
	{
		FUNCTION_ENTRY( "localSendBulkUpdate" );
        
		TA_Base_Bus::DutyState dutyState;

		getDutyState( dutyState, m_localLocSet );

		m_pStateUpdateSender->send(dutyState, TA_Base_Core::DutyStateUpdate::DutyAgentBulkUpdate );

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::localSendBulkUpdateRequest()
	{
		FUNCTION_ENTRY( "localSendBulkUpdateRequest" );

		m_pStateUpdateSender->sendRequest();

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::localStartDutyMonitor()
	{
		m_pDutyMonitorManager->start();
	}

	void DutyAgentImpl::localStopDutyMonitor()
	{
		m_pDutyMonitorManager->stop();
	}

	// ***** auth *****

	void DutyAgentImpl::authAllocateDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session)
	{
		FUNCTION_ENTRY( "authAllocateDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
        LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::authAllocateDuty ");
        
        try
        {
    		TA_Base_Bus::DutyState dutyState( session.SessionId.in(), DutyAction::SESSION_BEGIN);

    		sessionBegin( dutyState, session, m_feLocSet,true );

		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }

		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for authAllocateDuty",myTimer.elapsedTime());
        FUNCTION_EXIT;
	}

	void DutyAgentImpl::authReallocateDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& b4Session,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& aftSession)
	{
		FUNCTION_ENTRY( "authReallocateDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::authReallocateDuty ");
        try
        {
			TA_Base_Bus::SessionInfo b4SessionInfo =
				TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession( b4Session );
			
			TA_Base_Bus::SessionInfo aftSessionInfo=
				TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession( aftSession );

			
			if(aftSessionInfo.ProfileId.size()> b4SessionInfo.ProfileId.size())
		    {
				TA_Base_Bus::DutyState dutyState(aftSessionInfo.SessionId, DutyAction::SESSION_OVR_BEGIN);
				sessionOverrideBegin( dutyState, aftSession, m_feLocSet, true );

			    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
		    }
		    else
		    {
			    // fix the info we get from the auth agent. i.e. we want to know which session is
			    // ending not which one is left...
				const TA_Base_Bus::DASType::DbKey remainingProfile = aftSessionInfo.ProfileId.front();

			    // get a list of ALL the profiles
			    std::vector<TA_Base_Bus::DASType::DbKey> profiles;
				
				profiles = b4SessionInfo.ProfileId;

			    std::vector<TA_Base_Bus::DASType::DbKey>::iterator it;

			    // for each of the profiles, if it is not the remaining profile then end the session. note
			    // that this logic reflects the way the auth agent works. that is, an end override causes
			    // all "extra" profiles to be removed.
			    for( it=profiles.begin(); it!=profiles.end(); it++ )
			    {
				    if( remainingProfile != *it )
				    {
						aftSessionInfo.ProfileId.clear();
						aftSessionInfo.ProfileId.push_back(*it);

						TA_Base_Bus::DutyState dutyState(aftSessionInfo.SessionId, DutyAction::SESSION_OVR_END);
						sessionOverrideEnd( dutyState, aftSessionInfo, m_feLocSet, true );

					    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
				    }
			    }
		    }
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException.");
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for authReallocateDuty",myTimer.elapsedTime());
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::authFreeDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session)
	{
		FUNCTION_ENTRY( "authFreeDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::authFreeDuty ");

        try
        {
		    TA_Base_Bus::DutyState dutyState(session.SessionId.in(), DutyAction::SESSION_END);

		    TA_Base_Bus::DASType::RegionSet regSet;

		    regSet.insert( TA_Base_Bus::DASConst::ALL_REGIONS );

		    TA_Base_Bus::SessionInfo sessionInfo =
			    TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession( session );

		    sessionEnd( dutyState, sessionInfo, m_feLocSet, regSet, true );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }

		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for authFreeDuty",myTimer.elapsedTime());

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::autoDegradeSubsystems(const char* session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems)
	{
		FUNCTION_ENTRY( "autoDegradeSubsystems" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::autoDegradeSubsystems ");
		
        try
        {
		    if( subsystems.length() <= 0 )
		    {
			    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Received Auto-degrade request with zero-length subsystem sequence" );
			    return;
		    }

		    TA_Base_Bus::DutyState dutyState( session, DutyAction::AUTOMATIC_DELEGATION );

		    TA_Base_Bus::DASType::SubsystemSet subSet;

		    for( CORBA::ULong i=0; i<subsystems.length(); i++ )
		    {
			    subSet.insert( subsystems[i] );
		    }

		    degradeSubsystems( dutyState, session, subSet, m_feLocSet );

		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for autoDegradeSubsystems",myTimer.elapsedTime());
		FUNCTION_EXIT;
	}

	// ***** mgr *****

	void DutyAgentImpl::mgrDelegateSubsystemDuty(const char* session, 
		const ::TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions, 
		const ::TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems) 
	{
		FUNCTION_ENTRY("DutyAgentImpl::mgrDelegateSubsystemDuty");
		EnsureInControlState();
		try
		{
			if( subsystems.length() <= 0 )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Received delegate  Subsystem request with zero-length subsystem sequence" );
				return;
			}
			if( regions.length() <= 0 )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Received delegate  Subsystem request with zero-length region sequence" );
				return;
			}
			TA_Base_Core::Timer myTimer;
			TA_Base_Bus::DutyState dutyState( session, DutyAction::MANUAL_DELEGATION );

		    TA_Base_Bus::DASType::SubsystemSet subsystemSet;
			for(CORBA::ULong i =0; i < subsystems.length(); i++)
			{
				subsystemSet.insert(subsystems[i]);
			}
		    
			TA_Base_Bus::DASType::LocationSet locSet;
			for(CORBA::ULong i =0; i < regions.length(); i++)
			{
				locSet.insert(regions[i]);
			}
			
		    degradeSubsystems( dutyState, session, subsystemSet, locSet );

		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );

            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %lu seconds.",myTimer.elapsedTime());
		}
      catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::mgrDegradeSubsystemsAtLocation(const char* session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems, unsigned long locationKey)
	{
		FUNCTION_ENTRY( "mgrDegradeSubsystemsAtLocation" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrDegradeSubsystemsAtLocation ");

        try
        {
		    if( subsystems.length() <= 0 )
		    {
			    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Received degrade Subsystem request with zero-length subsystem sequence" );
			    return;
		    }
		    if (locationKey <= 0)
		    {
			    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Received degrade Subsystem request with zero or negative location key" );
			    return;
		    }

		    TA_Base_Bus::DutyState dutyState( session, DutyAction::SUBSYSTEM_DEGRADED );

		    TA_Base_Bus::DASType::SubsystemSet subSet;

		    for( CORBA::ULong i=0; i<subsystems.length(); i++ )
		    {
			    subSet.insert( subsystems[i] );
		    }

		    TA_Base_Bus::DASType::LocationSet locationSet;
		    
		    locationSet.insert(locationKey);
		    
		    // Do a corba call to affected duty agent at locationKey via DutyPeerTask to degrade selected subsystems
		    degradeSubsystems( dutyState, session, subSet, locationSet, true );

		    // Send broadcast messages to the affected or interested parties via SubsystemStateUpdateSender
		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrDegradeSubsystemsAtLocation",myTimer.elapsedTime());
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::mgrTransferRegionDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions)
	{
		FUNCTION_ENTRY( "mgrTransferRegionDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrTransferRegionDuty ");

        try
        {
		    TA_Base_Bus::DutyState dutyState(session.SessionId.in(), DutyAction::REQUEST_REGION_DUTY_TRANSFER);

		    transferRegionDuty( dutyState, session, regions, true );

		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrTransferRegionDuty",myTimer.elapsedTime());
		FUNCTION_EXIT;
	}
	void DutyAgentImpl::mgrDegradeSubsystemsAtRegions(const char* session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems, const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions)
	{
		FUNCTION_ENTRY( "mgrDegradeSubsystemsAtRegions" );

        EnsureInControlState();

        try
        {
		    if( subsystems.length() <= 0 )
		    {
			    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Received degrade Subsystem request with zero-length subsystem sequence" );
			    return;
		    }
			if (regions.length() <= 0)
		    {
			    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Received degrade Subsystem request with zero-length region sequence" );
			    return;
		    }

		    TA_Base_Bus::DutyState dutyState( session, DutyAction::SUBSYSTEM_DEGRADED );

		    TA_Base_Bus::DASType::SubsystemSet subSet;

		    for( CORBA::ULong i=0; i<subsystems.length(); i++ )
		    {
			    subSet.insert( subsystems[i] );
		    }

		    TA_Base_Bus::DASType::LocationSet locationSet;

			for(CORBA::ULong i=0; i< regions.length() ; i++)
			{
				locationSet.insert(regions[i]);
			}
		    
		    
		    
		    // Do a corba call to affected duty agent at locationKey via DutyPeerTask to degrade selected subsystems
		    degradeSubsystems( dutyState, session, subSet, locationSet );

		    // Send broadcast messages to the affected or interested parties via SubsystemStateUpdateSender
		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::mgrTransferSubsystemDuty(const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence& subsystems)
	{
		FUNCTION_ENTRY( "mgrTransferSubsystemDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrTransferSubsystemDuty ");
		
        try
        {
		    TA_Base_Bus::DutyState dutyState(session.SessionId.in(), DutyAction::REQUEST_SUBSYSTEM_DUTY_TRANSFER);

		    transferSubsystemDuty( dutyState, session, subsystems, true );

		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrTransferSubsystemDuty",myTimer.elapsedTime());

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::mgrTransferAllSubsystemDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session)
	{
		FUNCTION_ENTRY( "mgrTransferAllSubsystemDuty" );

        EnsureInControlState();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrTransferAllSubsystemDuty ");
		TA_Base_Core::Timer myTimer;
        try
        {
            TA_Base_Core::Timer myTimer;
		    TA_Base_Bus::DutyState dutyState(session.SessionId.in(), DutyAction::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER);

		    const std::string sessionStr(session.SessionId.in());
		    
		    TA_Base_Bus::DASType::SubsystemMap subMap;
		    
		    TA_Base_Bus::DASType::RegionSet allRegions;
		    TA_Base_Bus::DASType::RegionSet::const_iterator regIt;

		    TA_Base_Bus::DutyDbCache::instance()->getAllRegionKeys(allRegions);

		    for( regIt = allRegions.begin(); regIt!=allRegions.end(); regIt++ )
		    {
			    TA_Base_Bus::DASType::SubsystemSet subSet;

			    subSet.insert( TA_Base_Bus::DASConst::ALL_SUBSYSTEMS );

			    subMap.insert( TA_Base_Bus::DASType::SubsystemMap::value_type(*regIt, subSet) );
		    }

			TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession(session);
		    TransferSubsystemDAC command(dutyState, m_taskManager, sessionInfo, subMap);

		    execute( command );

            // TD10432: Transferring all subsystems should still notify all operators involved.
            TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );

            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %lu seconds.",myTimer.elapsedTime());
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrTransferAllSubsystemDuty",myTimer.elapsedTime());
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::mgrDegradeAllSubsystems(const char* session)
	{
		FUNCTION_ENTRY( "mgrDegradeAllSubsystems" );

        EnsureInControlState();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrDegradeAllSubsystems ");
		TA_Base_Core::Timer myTimer;
        try
        {
		    TA_Base_Bus::DutyState dutyState( session, DutyAction::SYSTEMWIDE_DELEGATION );

		    TA_Base_Bus::DASType::SubsystemSet subsystems;
		    subsystems.insert( TA_Base_Bus::DASConst::ALL_SUBSYSTEMS );

		    degradeSubsystems( dutyState, session, subsystems, m_feLocSet, true );

		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrDegradeAllSubsystems",myTimer.elapsedTime());
		FUNCTION_EXIT;
	}
	
	void DutyAgentImpl::mgrGetDutyState(const char* session,
            TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_out subState,
			TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence_out subDuty,
			TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence_out regDuty)
	{
		FUNCTION_ENTRY( "mgrGetDutyState" );

        EnsureInControlState();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrGetDutyState ");
		TA_Base_Core::Timer myTimer;
        try
        {
		    TA_Base_Bus::DutyState dutyState(session,"");

		    getDutyState( dutyState, m_feLocSet, true );

		    DutyManagerSnapshotConverter dmsc(session);

		    dutyState.convert(dmsc);

            subState = dmsc.getSubsystemState();
		    subDuty = dmsc.getSubsystemDuty();
		    regDuty = dmsc.getRegionDuty();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrGetDutyState",myTimer.elapsedTime());
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::mgrGetSessionDutyState(const char* session,
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_out subState,
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence_out subDuty,
		TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence_out regDuty)
	{
		FUNCTION_ENTRY( "mgrGetDutyState" );
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrGetSessionDutyState ");
		TA_Base_Core::Timer myTimer;
		
        EnsureInControlState();
		
        try
        {
			TA_Base_Bus::DutyState dutyState(session,"");
			
			getSessionDutyState( dutyState,session, m_feLocSet, true );
			
			DutyManagerSnapshotConverter dmsc(session);
			
			dutyState.convert(dmsc);
			
            subState = dmsc.getSubsystemState();
			subDuty = dmsc.getSubsystemDuty();
			regDuty = dmsc.getRegionDuty();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrGetSessionDutyState",myTimer.elapsedTime());
		
		FUNCTION_EXIT;
	}
	TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* DutyAgentImpl::mgrGetSubsystemDutyState(const char* session, 
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subKeys, TA_Base_Bus::DutyAgentTypeCorbaDef::Key locationKey)
	{
        FUNCTION_ENTRY( "mgrGetSubsystemDutyState" );

        EnsureInControlState();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrGetSubsystemDutyState ");
		TA_Base_Core::Timer myTimer;
        try
        {
            TA_Base_Bus::DutyState dutyState(session,"");

            // Don't bother checking the size of the subsystem sequence. If an empty sequence is provided
            // it will return the state of ALL subsystems at the specified location.

            // Convert the sequence of subsystems into a subsystem set.
            TA_Base_Bus::DASType::SubsystemSet subSet;
            for( CORBA::ULong i=0; i<subKeys.length(); i++ )
            {
                subSet.insert( subKeys[i] );
            }

            // Create the set used to limit the query to a particular location.
            TA_Base_Bus::DASType::LocationSet locationSet;

            // If the location is zero it will query all locations.
            if ( 0 == locationKey )
            {
                locationSet = m_feLocSet;
            }
            else
            {
                locationSet.insert(locationKey);
            }

            // Execute the snapshot query.
            getSubsystemDutyState( dutyState, subSet, locationSet, true );

            // Convert the duty state into a sequence of subsystem state objects.
            SubsystemStateSnapshotConverter stateConv( subKeys );
            dutyState.convert( stateConv );

            FUNCTION_EXIT;
            return ( stateConv.getSubsystemStates() );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrGetSubsystemDutyState",myTimer.elapsedTime());
	}
	
	TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableRegionSequence*
		DutyAgentImpl::mgrGetTransferableRegions(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session)
	{
		FUNCTION_ENTRY( "mgrGetTransferableRegions" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrGetTransferableRegions ");

        try
        {
		    TA_Base_Bus::DutyState dutyState(session.SessionId.in(),"");

		    getTransferableRegions( dutyState, session, m_feLocSet, false, true );

		    TransferableRegionStateConverter trConverter;

		    dutyState.convert(trConverter);

		    FUNCTION_EXIT;
		    return( trConverter.getRegions() );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrGetTransferableRegions",myTimer.elapsedTime());
	}

	TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableSubsystemSequence*
		DutyAgentImpl::mgrGetTransferableSubsystems(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions)
	{
		FUNCTION_ENTRY( "mgrGetTransferableSubsystems" );

        EnsureInControlState();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::mgrGetTransferableSubsystems ");
		TA_Base_Core::Timer myTimer;
        try
        {
		    TA_Base_Bus::DutyState dutyState(session.SessionId.in(),"");

			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: Before call to getTransferableSubsystems(): region size:%lu", 
				regions.length() );
			std::string temp = "TD15346: session: ";
			temp += session.SessionId.in();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
			//++TD15346

		    getTransferableSubsystems( dutyState, session, regions, true );

			{
			///TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: After call to getTransferableSubsystems(): region size:%lu", 
				regions.length() );
			std::string temp = "TD15346: session: ";
			temp += session.SessionId.in();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
			//++TD15346
			}

		    TransferableSubsystemStateConverter tssc;

		    dutyState.convert(tssc);

		    FUNCTION_EXIT;
		    return( tssc.getResult() );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for mgrGetTransferableSubsystems",myTimer.elapsedTime());
	}

	TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableRegionSequence* DutyAgentImpl::regMgrGetTransferableRegions(
			const char* session, TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence_out uncontrolledSubs )
	{
		FUNCTION_ENTRY( "regMgrGetTransferableRegions" );

        EnsureInControlState();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::regMgrGetTransferableRegions ");
		TA_Base_Core::Timer myTimer;
        try
        {
		    //uncontrolledSubs->length(0);

		    TA_Base_Bus::DutyState dutyState(session,"");

			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession;
			corbaSession.SessionId = CORBA::string_dup(session);

		    getTransferableRegions( dutyState, corbaSession, m_feLocSet, true );

		    TransferableRegionStateConverter trConverter;

		    dutyState.convert(trConverter);
		    
		    uncontrolledSubs = trConverter.getSubsystems();

		    FUNCTION_EXIT;
		    return( trConverter.getRegions() );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for regMgrGetTransferableRegions",myTimer.elapsedTime());
	}

	TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence* DutyAgentImpl::regMgrGetRemovableRegions(
			const char* session )
	{
		FUNCTION_ENTRY( "regMgrGetRemovableRegions" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::regMgrGetRemovableRegions ");
		
        try
        {
		    TA_Base_Bus::DutyState state(session, "");

			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession; 
			corbaSession.SessionId = CORBA::string_dup(session);
		    hasExclusiveDuty( state, corbaSession, m_feLocSet );

		    GetExclusiveDutyConverter gedc;
		    state.convert(gedc);
        
		    FUNCTION_EXIT;
		    return( gedc.getSubsystems() );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for regMgrGetRemovableRegions",myTimer.elapsedTime());
	}
 
	void DutyAgentImpl::regMgrTransferRegionDuty( const char* user, const char* session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions )
	{
		FUNCTION_ENTRY( "regMgrTransferRegionDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::regMgrTransferRegionDuty ");
		
        try
        {
		    TA_Base_Bus::DutyState dutyState(user, session, DutyAction::REQUEST_REGION_DUTY_TRANSFER);

			TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession;
			corbaSession.SessionId = CORBA::string_dup(session);
		    transferRegionDuty( dutyState, corbaSession, regions );

		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for regMgrTransferRegionDuty",myTimer.elapsedTime());
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::regMgrRemoveRegionDuty( const char* user, const char* session,
			const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions )
	{
		FUNCTION_ENTRY( "regMgrRemoveRegionDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::regMgrRemoveRegionDuty ");
		
        try
        {
		    TA_Base_Bus::DutyState dutyState(user, session, DutyAction::REQUEST_REGION_DUTY_TRANSFER);

		    TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::DutySessionCache::instance()->getSessionInfo(session);

		    TA_Base_Bus::DASType::RegionSet regionSet;

		    for( CORBA::ULong i=0; i < regions.length(); i++ )
		    {
			    regionSet.insert(regions[i]);
		    }

		    sessionEnd( dutyState, sessionInfo, m_feLocSet, regionSet, true );

		    TA_Base_Bus::FrontEndStateChangeSubject::instance()->update( dutyState );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for regMgrTransferRegionDuty",myTimer.elapsedTime());

		FUNCTION_EXIT;
	}

	// ***** peer *****

	void DutyAgentImpl::peerSessionBegin(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session)
	{
		FUNCTION_ENTRY( "peerSessionBegin" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerSessionBegin ");
		
        try
        {
		    TA_Base_Bus::DutyState state( session.SessionId.in(), "");
		    sessionBegin( state, session, m_localLocSet,false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerSessionBegin",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerSessionBegin ");
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerSessionOverrideBegin(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session)
	{
		FUNCTION_ENTRY( "peerSessionOverrideBegin" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerSessionOverrideBegin ");
		
        try
        {
		    TA_Base_Bus::DutyState state(session.SessionId.in(), "");

		    sessionOverrideBegin( state, session, m_localLocSet, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerSessionOverrideBegin",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerSessionOverrideBegin ");

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerSessionOverrideEnd(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session)
	{
		FUNCTION_ENTRY( "peerSessionOverrideEnd" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerSessionOverrideEnd ");
		
        try
        {
		    TA_Base_Bus::SessionInfo sessionInfo =
			    TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession( session );

		    TA_Base_Bus::DutyState state(sessionInfo.SessionId, "");
		    sessionOverrideEnd( state, sessionInfo, m_localLocSet, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerSessionOverrideEnd",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerSessionOverrideEnd ");

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerSessionEnd(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions )
	{
		FUNCTION_ENTRY( "peerSessionEnd" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerSessionEnd ");
		
        try
        {
		    TA_Base_Bus::DutyState state(session.SessionId.in(), "");

		    TA_Base_Bus::DASType::RegionSet regionSet;

		    for( CORBA::ULong i=0; i < regions.length(); i++ )
		    {
			    regionSet.insert(regions[i]);
		    }

		    TA_Base_Bus::SessionInfo sessionInfo =
			    TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession( session );

		    sessionEnd( state, sessionInfo, m_localLocSet, regionSet, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerSessionEnd",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerSessionEnd ");

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerGetTransferableRegions(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, bool checkSubs )
	{
		FUNCTION_ENTRY( "peerGetTransferableRegions" );
		
        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerGetTransferableRegions ");
		
        try
        {
		    TA_Base_Bus::DutyState state(session.SessionId.in(),"");
		    getTransferableRegions( state, session, m_localLocSet, checkSubs, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerGetTransferableRegions",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerGetTransferableRegions ");

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerGetTransferableSubsystems(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session, const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions )
	{
		FUNCTION_ENTRY( "peerGetTransferableSubsystems" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: Inside peerGetTransferableSubsystems(): region size:%lu", 
			regions.length() );
		std::string temp = "TD15346: session: ";
		temp += session.SessionId.in();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
		//++TD15346
		
        try
        {
		    TA_Base_Bus::DutyState state(session.SessionId.in(),"");
		    getTransferableSubsystems( state, session, regions, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerGetTransferableSubsystems",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerGetTransferableSubsystems ");

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerTransferRegions(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const char* user, const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&   session, const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions)
	{
		FUNCTION_ENTRY( "peerTransferRegions" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerTransferRegions ");

        try
        {
		    TA_Base_Bus::DutyState state(user, session.SessionId.in(), "");
		    transferRegionDuty( state, session, regions, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerTransferRegions",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerTransferRegions ");
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerTransferSubsystems(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const  TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence& subsystems, const char* action)
	{
		FUNCTION_ENTRY( "peerTransferSubsystems" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerTransferSubsystems ");

        try
        {
		    TA_Base_Bus::DutyState state(session.SessionId.in(), action);
		    transferSubsystemDuty( state, session, subsystems, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerTransferSubsystems",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerTransferSubsystems ");
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerGetState(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState)
	{
		FUNCTION_ENTRY( "peerGetState" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerGetState ");
		
        try
        {
		    TA_Base_Bus::DutyState state;
		    getDutyState( state, m_localLocSet, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerGetState",myTimer.elapsedTime());

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerGetSessionState(const char* session,TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState)
	{
		FUNCTION_ENTRY( "peerGetState" );
		
        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerGetSessionState ");
		
        try
        {
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerGetSessionState session %s ", session);
			TA_Base_Bus::DutyState state(session,"");
			getSessionDutyState( state, session,m_localLocSet, false );
			
			CorbaDutyStateConverter cdsc;
			state.convert(cdsc);
			dutyState = cdsc.getCorbaDutyState();
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerGetSessionState");
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerGetSessionState",myTimer.elapsedTime());
		
		FUNCTION_EXIT;
	}


	void DutyAgentImpl::peerGetSubsystemState(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems)
	{
		FUNCTION_ENTRY( "peerGetState" );
		EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerGetSubsystemState ");

		try
		{
			
			TA_Base_Bus::DutyState state;

			if( subsystems.length() <= 0 )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Received peerGetSubsystemState with zero-length subsystem sequence" );
				return;
			}
		
			
			TA_Base_Bus::DASType::SubsystemSet subSet;
			
			for( CORBA::ULong i=0; i<subsystems.length(); i++ )
			{
				subSet.insert( subsystems[i] );
		    }

			getSubsystemDutyState( state, subSet, m_localLocSet, false );
			
			CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
		}
		catch (TA_Base_Core::TransactiveException& ex)
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
		}
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerGetSubsystemState",myTimer.elapsedTime());
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerDegradeSubsystems(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const char* session, const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems)
	{
		FUNCTION_ENTRY( "peerDegradeSubsystems" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerDegradeSubsystems ");
		
        try
        {
		    if( subsystems.length() <= 0 )
		    {
			    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Received degrade request with zero-length subsystem sequence" );
			    return;
		    }

		    TA_Base_Bus::DutyState state(session,"");

		    TA_Base_Bus::DASType::SubsystemSet subSet;

		    for( CORBA::ULong i=0; i<subsystems.length(); i++ )
		    {
			    subSet.insert( subsystems[i] );
		    }

		    degradeSubsystems( state, session, subSet, m_localLocSet, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerDegradeSubsystems",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerDegradeSubsystems ");
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerGetDuty(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, TA_Base_Bus::DutyAgentTypeCorbaDef::Key region, TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem )
	{
		FUNCTION_ENTRY( "peerGetDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerGetDuty ");
		
        try
        {
		    TA_Base_Bus::DutyState state(session.SessionId.in(),"");

		    hasDuty( state, session, region, subsystem, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerGetDuty",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerGetDuty ");
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::peerGetExclusiveDuty(TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_out dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session)
	{
		FUNCTION_ENTRY( "peerGetExclusiveDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::peerGetExclusiveDuty ");
		
        try
        {
		    TA_Base_Bus::DutyState state(session.SessionId.in(),"");
			
		    hasExclusiveDuty( state, session, m_localLocSet, false );

		    CorbaDutyStateConverter cdsc;
		    state.convert(cdsc);
		    dutyState = cdsc.getCorbaDutyState();
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for peerGetExclusiveDuty",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::peerGetExclusiveDuty ");

		FUNCTION_EXIT;
	}

	// ***** query *****

	CORBA::Boolean DutyAgentImpl::queryHasDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, TA_Base_Bus::DutyAgentTypeCorbaDef::Key region,
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem, TA_Base_Bus::DutyAgentTypeCorbaDef::Key profile,
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key& actionGroup)
	{
		FUNCTION_ENTRY( "queryHasDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::queryHasDuty ");
		
        try
        {
		    TA_Base_Bus::DutyState state(session.SessionId.in(), DutyAction::HAS_DUTY_QUERY);

		    hasDuty( state, session, profile, region, subsystem );

		    GetDutyConverter gdc(session.SessionId.in(), region, subsystem, profile, actionGroup);
		    state.convert(gdc);

			if (gdc.getResult())
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
					"In DutyAgentImpl::queryHasDuty, getResult()=true, for session=%s, region=%lu, subsystem=%lu, profile=%lu, actionGroup=%lu",
					session.SessionId.in(),region,subsystem,profile,actionGroup);
			} 
			else
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
					"In DutyAgentImpl::queryHasDuty, getResult()=false, for session=%s, region=%lu, subsystem=%lu, profile=%lu, actionGroup=%lu",
					session.SessionId.in(),region,subsystem,profile,actionGroup);
			}
		    FUNCTION_EXIT;
		    return( gdc.getResult() );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for queryHasDuty",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::queryHasDuty ");
	}

	CORBA::Boolean DutyAgentImpl::queryHasExclusiveDuty(const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence_out subsystems)
	{
		FUNCTION_ENTRY( "queryHasExclusiveDuty" );

        EnsureInControlState();
		TA_Base_Core::Timer myTimer;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::queryHasExclusiveDuty ");

        try
        {
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyAgentImpl::queryHasExclusiveDuty ");
		    TA_Base_Bus::DutyState state(session.SessionId.in(), DutyAction::HAS_EXCL_DUTY_QUERY);

		    hasExclusiveDuty( state, session, m_feLocSet );

		    GetExclusiveDutyConverter gedc;
		    state.convert(gedc);

		    subsystems = gedc.getSubsystems();

		    FUNCTION_EXIT;
		    return( gedc.getResult() );
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "TransactiveException caught at top level, converting to UnhandledException." );
            throw TA_Base_Bus::DutyAgentTypeCorbaDef::UnhandledException( ex.what() );
        }
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "time spent: %f seconds for queryHasExclusiveDuty",myTimer.elapsedTime());
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyAgentImpl::queryHasExclusiveDuty ");
	}

	// ***** response *****
	void DutyAgentImpl::respResponse( const char* uniqueId, TA_Base_Bus::IDutyAgentRespCorbaDef::EDutyResponse response )
	{
		FUNCTION_ENTRY( "respResponse" );

        EnsureInControlState();

		TA_Base_Bus::DutyResponseSubject::instance()->update( uniqueId, response );
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::execute( IDutyAgentCommand& command, bool checkRights/*=true*/ )
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_executiveLock);

		m_executive->execute( command, *m_rightsLib, m_resourceKey, checkRights );
	}

	void DutyAgentImpl::sessionBegin( TA_Base_Bus::DutyState& dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		const TA_Base_Bus::DASType::LocationSet& locSet, bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "sessionBegin" );

		TA_Base_Bus::SessionInfo  sessionInfo =
			TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession( session );

		AllocateDAC command(dutyState, m_taskManager, sessionInfo, locSet);

		execute( command, checkRights );

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::sessionOverrideBegin( TA_Base_Bus::DutyState& dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		const TA_Base_Bus::DASType::LocationSet& locSet, bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "sessionOverrideBegin" );

		TA_Base_Bus::SessionInfo sessionInfo =
			TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession( session );

		OverrideBeginDAC command(dutyState, m_taskManager, sessionInfo, locSet);
		execute( command, checkRights );
		

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::sessionOverrideEnd( TA_Base_Bus::DutyState& dutyState,
		const TA_Base_Bus::SessionInfo& session,	const TA_Base_Bus::DASType::LocationSet& locSet,
		bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "sessionOverrideEnd" );

		OverrideEndDAC command(dutyState, m_taskManager, session, locSet);
		execute( command, checkRights );
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::sessionEnd( TA_Base_Bus::DutyState& dutyState,
		const TA_Base_Bus::SessionInfo& session,
		const TA_Base_Bus::DASType::LocationSet& locSet,
		const TA_Base_Bus::DASType::RegionSet& regions,
		bool checkRights /*=true*/ )
	{
		FUNCTION_ENTRY( "sessionEnd" );

		FreeDAC command(dutyState, m_taskManager, session, locSet, regions );

		execute( command, checkRights );

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::transferRegionDuty( TA_Base_Bus::DutyState& dutyState, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions, bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "transferRegionDuty" );

		//const std::string sessionStr(session);
		
		TA_Base_Bus::DASType::RegionSet regionSet;

		for( CORBA::ULong i=0; i < regions.length(); i++ )
		{
			regionSet.insert(regions[i]);
		}

		TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession(session);
		TransferRegionDAC command(dutyState, m_taskManager, sessionInfo, regionSet);

		execute( command, checkRights );

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::transferSubsystemDuty( TA_Base_Bus::DutyState& dutyState, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef & session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence& subsystems, bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "transferSubsystemDuty" );

		const std::string sessionStr(session.SessionId.in());
		
		TA_Base_Bus::DASType::SubsystemMap subMap;
		
		CORBA::ULong regIdx = 0;
		CORBA::ULong subIdx = 0;

		for( regIdx = 0; regIdx < subsystems.length(); regIdx++ )
		{
			TA_Base_Bus::DASType::SubsystemSet subSet;

			for( subIdx = 0; subIdx < subsystems[regIdx].subsystems.length(); subIdx++ )
			{
				subSet.insert( subsystems[regIdx].subsystems[subIdx] );
			}

			subMap.insert( TA_Base_Bus::DASType::SubsystemMap::value_type(subsystems[regIdx].region, subSet) );
		}

		TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession(session);
		TransferSubsystemDAC command(dutyState, m_taskManager, sessionInfo, subMap);

		execute( command, checkRights );

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::degradeSubsystems( TA_Base_Bus::DutyState& dutyState, const char* session,
		const TA_Base_Bus::DASType::SubsystemSet& subsystems, const TA_Base_Bus::DASType::LocationSet& locSet,
		bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "degradeSubsystems" );

		DegradeDAC command(dutyState, m_taskManager, subsystems, locSet);

		execute( command, checkRights );

		FUNCTION_EXIT;
	}
	
	void DutyAgentImpl::getDutyState( TA_Base_Bus::DutyState& dutyState, const TA_Base_Bus::DASType::LocationSet& locSet,
		bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "getDutyState" );

		GetStateDAC command( dutyState, m_taskManager, locSet );

		execute( command, checkRights );

		FUNCTION_EXIT;
	}
	
	void DutyAgentImpl::getSessionDutyState( TA_Base_Bus::DutyState& dutyState, const char* session,  const TA_Base_Bus::DASType::LocationSet& locSet,
		bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "getSessionDutyState" );
		
		const std::string sessionStr(session);

		GetSessionStateDAC command( dutyState,sessionStr, m_taskManager, locSet );
		
		execute( command, checkRights );
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::getSubsystemDutyState( TA_Base_Bus::DutyState& dutyState,const TA_Base_Bus::DASType::SubsystemSet& subsystems, const TA_Base_Bus::DASType::LocationSet& locSet,
		bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "getDutyState" );
		
		GetSubsystemStateDAC command( dutyState, m_taskManager, subsystems, locSet );
		
		execute( command, checkRights );
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::getTransferableRegions( TA_Base_Bus::DutyState& dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session, const TA_Base_Bus::DASType::LocationSet& locSet, bool checkSubs,
		bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "getTransferableRegions" );

		//const std::string sessionStr(session);
		TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession(session);

		GetTransferableRegionsDAC command(dutyState, m_taskManager, sessionInfo, locSet, checkSubs );

		execute( command, checkRights );

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::getTransferableSubsystems( TA_Base_Bus::DutyState& dutyState,
		const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef&  session,
		const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& regions,
		bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "getTransferableSubsystems" );

		//const std::string sessionStr(session);
		
		TA_Base_Bus::DASType::RegionSet regSet;

		for( CORBA::ULong regIdx = 0; regIdx != regions.length(); regIdx++ )
		{
			regSet.insert(regions[regIdx]);
		}

		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: Before call to GetTransferableSubsystemsDAC: region size:%lu", 
			regions.length() );
		std::string temp = "TD15346: session: ";
		temp += session.SessionId.in();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
		//++TD15346

		TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession(session);
		GetTransferableSubsystemsDAC command(dutyState, m_taskManager, sessionInfo, regSet );

		{
		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: After call to GetTransferableSubsystemsDAC: region size:%lu", 
			regions.length() );
		std::string temp = "TD15346: session: ";
		temp += session.SessionId.in();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
		//++TD15346
		}

		execute( command, checkRights );


		FUNCTION_EXIT;
	}

	void DutyAgentImpl::hasDuty( TA_Base_Bus::DutyState& state, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key region, TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem, bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "hasDuty" );

		//const std::string sessionStr(session);

		TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession(session);
		HasDutyDAC command(state, m_taskManager, sessionInfo, region, subsystem);

		execute( command, checkRights );

		FUNCTION_EXIT;
	}

	void DutyAgentImpl::hasDuty( TA_Base_Bus::DutyState& state, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key profile,
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key region, TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem, bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "hasDuty" );
		
		//const std::string sessionStr(session);
		
		TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession(session);
		sessionInfo.ProfileId.clear();
		sessionInfo.ProfileId.push_back(profile);
		
		HasDutyDAC command(state, m_taskManager, sessionInfo, region, subsystem);
		
		execute( command, checkRights );
		
		FUNCTION_EXIT;
	}

	void DutyAgentImpl::hasExclusiveDuty( TA_Base_Bus::DutyState& state, const TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef& session,
		const TA_Base_Bus::DASType::LocationSet& locSet, bool checkRights/*=true*/ )
	{
		FUNCTION_ENTRY( "hasExclusiveDuty" );

		//const std::string sessionStr(session);
		TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCORBASessionToCSession(session);

		HasExclusiveDutyDAC command( state, m_taskManager, sessionInfo, locSet );

		execute( command, checkRights );

		FUNCTION_EXIT;
	}

}
