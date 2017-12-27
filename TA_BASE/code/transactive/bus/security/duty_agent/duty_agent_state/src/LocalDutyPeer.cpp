/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/LocalDutyPeer.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/21 10:20:13 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  <description>
  */
#include "DutyAgentState.h"
#include "RegSubMultiProfCommandSessionInfo.h"

using namespace TA_Base_Core;
namespace TA_Base_Bus
{

	LocalDutyPeer::RegXferSubCommand::RegXferSubCommand( DutyState& state, const TA_Base_Bus::SessionInfo& session,
		const DASType::SubsystemSet& subsystems,bool apply ) :
		m_state(state),
		m_session(session),
		m_subsystems(subsystems),
		m_apply(apply)
	{
	}
	
	void LocalDutyPeer::RegXferSubCommand::execute( Region& region, bool doApply )
	{
		std::vector<DASType::DbKey> profiles;
		std::vector<DASType::DbKey>::const_iterator prfIt;

		//DutySessionCache::instance()->getProfiles( m_session, profiles );
		profiles = m_session.ProfileId;

		for( prfIt = profiles.begin(); prfIt != profiles.end(); prfIt++ )
		{
			DutyPrimitive dp(m_session.SessionId,*prfIt);

			region.transfer( m_state, dp, m_subsystems,m_apply );
		}

// 		if( doApply )
// 		{
// 			region.apply( m_state );
// 		}
	}

	LocalDutyPeer::RegSubSetCommand::RegSubSetCommand( void (Region::*subSetCmd)(DutyState&, const DASType::SubsystemSet&), DutyState& state,
		const DASType::SubsystemSet& subsystems ) :
		m_subSetCmd(subSetCmd),
		m_state(state),
		m_subsystems(subsystems)
	{
	}
	
	void LocalDutyPeer::RegSubSetCommand::execute( Region& region , bool doApply)
	{
		(region.*m_subSetCmd)( m_state, m_subsystems );
	}

	LocalDutyPeer::LocalDutyPeer( const DASType::DbKey& location ) :
		SeDutyStateObserver<StateUpdateSubjectImpl   >(*StateUpdateSubject::instance()   ),
		SeObserver<AgentTerminateSubjectImpl>(*AgentTerminateSubject::instance())
	{
		FUNCTION_ENTRY("LocalDutyPeer");
		DASType::RegionSet regions;

		DutyDbCache::instance()->getLocationRegions(location, regions);

		DASType::RegionSet::const_iterator it;

		for( it = regions.begin(); it != regions.end(); it++ )
		{
			Region* newRegion = new Region(*it);

			m_regions.insert( RegionMap::value_type(*it, newRegion) );
		}
		FUNCTION_EXIT;
	}

	LocalDutyPeer::~LocalDutyPeer()
	{
		FUNCTION_ENTRY("~LocalDutyPeer");

		RegionMap::iterator it;

		for( it = m_regions.begin(); it != m_regions.end(); it++ )
		{
			delete it->second;
			it->second = NULL;
		}

		m_regions.clear();
		FUNCTION_EXIT;
	}

	
	void LocalDutyPeer::sessionBegin( DutyState& state, const SessionInfo& session )
	{
		FUNCTION_ENTRY("sessionBegin");

		if( session.ProfileId.size() <= 0 )
		{
			return;
		}

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin LocalDutyPeer::sessionBegin");

		DutyPrimitive dp(session.SessionId, *(session.ProfileId.begin()));

		RegSubUniProfCommand<Region> sesBegin( &Region::sessionBegin, state, dp );
		regionExecute( sesBegin, true );

		PeerStateChangeSubject::instance()->update(state);

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end LocalDutyPeer::sessionBegin");
		FUNCTION_EXIT;
	}

	void LocalDutyPeer::sessionEnd( DutyState& state, const SessionInfo& session,
		const DASType::RegionSet& regions )
	{
		FUNCTION_ENTRY("sessionEnd");

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin LocalDutyPeer::sessionEnd");
		RegSubMultiProfCommandSessionInfo<Region> sesEnd( &Region::sessionEnd, state, session );
		regionExecute( sesEnd, regions, true );

		PeerStateChangeSubject::instance()->update(state);
		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end LocalDutyPeer::sessionEnd");
		FUNCTION_EXIT;
	}

	void LocalDutyPeer::sessionOverrideBegin( DutyState& state, const SessionInfo& session )
	{
		FUNCTION_ENTRY("sessionOverrideBegin");

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin LocalDutyPeer::sessionOverrideBegin");

		DutyPrimitive dp(session.SessionId, session.ProfileId.back());

		RegSubUniProfCommand<Region> sesBegin( &Region::sessionBegin, state, dp );
		regionExecute( sesBegin, true );

		PeerStateChangeSubject::instance()->update(state);

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end LocalDutyPeer::sessionOverrideBegin");
		FUNCTION_EXIT;
	}

	void LocalDutyPeer::sessionOverrideEnd( DutyState& state, const SessionInfo& session )
	{
		FUNCTION_ENTRY("sessionOverrideEnd");

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin LocalDutyPeer::sessionOverrideEnd");

		DutyPrimitive dp(session.SessionId, session.ProfileId.back());

		RegSubUniProfCommand<Region> sesEnd( &Region::sessionEnd, state, dp );
		regionExecute( sesEnd, true );

		PeerStateChangeSubject::instance()->update(state);

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end LocalDutyPeer::sessionOverrideEnd");
		FUNCTION_EXIT;
	}
	
	void LocalDutyPeer::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session,
		const DASType::RegionSet& regions )
	{
		FUNCTION_ENTRY("transfer");

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin LocalDutyPeer::transferRegion");

		RegSubMultiProfCommand<Region> xfer( &Region::transfer, state, session );
		regionExecute( xfer, regions, true );

		//PeerStateChangeSubject::instance()->update(state);

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end LocalDutyPeer::transferRegion");
		FUNCTION_EXIT;
	}

	void LocalDutyPeer::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session,
		const DASType::SubsystemMap& subsystems )
	{
		FUNCTION_ENTRY("transfer");

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin LocalDutyPeer::transferSubsystem");
		
		DASType::SubsystemMap::const_iterator subMapIt;

		for( subMapIt = subsystems.begin(); subMapIt != subsystems.end(); subMapIt++ )
		{
			RegXferSubCommand xfer( state, session, subMapIt->second,false );
			regionExecute( xfer, subMapIt->first, false );
		}

		// send request
		DASType::SubsystemMap subsToApply;
		DASType::SubsystemMap subsToCancel; //here we don't care subsystems to cancel

		std::vector<DASType::DbKey> profiles;
		//DutySessionCache::instance()->getProfiles( session, profiles );
		profiles = session.ProfileId;
		DutyPrimitive dp( session.SessionId, profiles.front() );

		DutyRequestSender dutyRequestSender;
		dutyRequestSender.send( dp, state, subsToApply, subsToCancel );

		for( subMapIt = subsToApply.begin(); subMapIt != subsToApply.end(); subMapIt++ )
		{
			RegXferSubCommand xfer( state, session, subMapIt->second,true );
			regionExecute( xfer, subMapIt->first, true);
		}
// 		for( subMapIt = subsToApply.begin(); subMapIt != subsToApply.end(); subMapIt++ )
// 		{
// 			RegSubSetCommand applyCmd( &Region::apply, state, subMapIt->second );
// 			regionExecute( applyCmd, subMapIt->first, false );
// 		}

// 		for( subMapIt = subsToCancel.begin(); subMapIt != subsToCancel.end(); subMapIt++ )
// 		{
// 			RegSubSetCommand cancelCmd( &Region::cancel, state, subMapIt->second );
// 			regionExecute( cancelCmd, subMapIt->first, false );
// 		}

		// CL19422++
		// regain local duty for subsystem
// 		for( subMapIt = subsystems.begin(); subMapIt != subsystems.end(); subMapIt++ )
// 		{
// 			// before regain duty, apply current subsystem state change
// 			RegSubSetCommand applyCmd( &Region::apply, state, subMapIt->second );
// 			regionExecute( applyCmd, subMapIt->first, false );
// 
// 			// This operation only need when subsystem state change from Degrade to
// 			//	Normal when a central profile ask for
// 			RegSubSimpleCommand<Region> regain( &Region::regainLocalProfileDuty, state );
// 			regionExecute( regain, false );
// 		}
		// ++ CL19422

		// CL-19422 +++
		// no matter what, accept all subsystem changes
// 		for( subMapIt = subsystems.begin(); subMapIt != subsystems.end(); subMapIt++ )
// 		{
// 			// before regain duty, apply current subsystem state change
// 			RegSubSetCommand applyCmd( &Region::apply, state, subMapIt->second );
// 			regionExecute( applyCmd, subMapIt->first, false );
// 		}

		// modify at: 2008-9-25
		// We have modify DefaultApplyConvertor and SubsystemState to make sure
		// each subsystems state are explicit. We can regainSubsystem directly
		// For Local Profile, there is no need to regainSubsystemDuty again
// 		if (IDutyPolicy::PT_CENTRAL == DutyDbCache::instance()->getProfileType(dp.m_profile))
// 		{
// 			for( subMapIt = subsToApply.begin(); subMapIt != subsToApply.end(); subMapIt++ )
//  			{
//  				// This operation only need when subsystem state change from 		
//  				//	Degrade/Delegation to Normal 
//   				RegSubSimpleCommand<Region> regain( &Region::regainLocalProfileDuty, state );
//  				regionExecute( regain, false );
//  			}
// 		}
// 		else
// 		{
// 			// For local profile, do not do anything
// 		}
		// +++ CL-19422

		// CL19422 ++
#ifdef DEBUG_DUTY_DETAIL
		{
			std::ofstream strOut;
			strOut.open("./DutyAgentSpecial.log", std::ios::app);
			if (strOut.is_open())
			{
				strOut << "\n\n---In LocalDutyPeer::transfer()-direct---" << std::endl
					<< "   check session:" << session.c_str() << std::endl;
				
				strOut << "\n...DutyStateInfo..." << std::endl;
				state.print(strOut);
			
				strOut << "---End LocalDutyPeer::transfer()-direct---" << std::endl;
				strOut.close();
			}
		}
#endif //DEBUG_DUTY_DETAIL
		// ++ CL19422

		PeerStateChangeSubject::instance()->update(state);

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end LocalDutyPeer::transferSubsystem");
		FUNCTION_EXIT;
	}
	
	void LocalDutyPeer::executeQuery( DutyState& state, IDutyQuery& query )
		
	{
		FUNCTION_ENTRY("executeQuery");
		
		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin LocalDutyPeer::executeQuery");		

		RegSubQueryCommand<Region> queryCmd(state, query);
		regionExecute( queryCmd, query.getRegions(), false );
	
		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end LocalDutyPeer::executeQuery");
		FUNCTION_EXIT;
	}

	void LocalDutyPeer::degrade( DutyState& state, const DASType::SubsystemSet& subsystems )
	{
		FUNCTION_ENTRY("degrade");

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"begin LocalDutyPeer::degrade");

		RegSubSetCommand degradeCmd(&Region::degradeSubsystems, state, subsystems);
		regionExecute( degradeCmd, false );

		//PeerStateChangeSubject::instance()->update(state);

		LOG_GENERIC(SOURCE_INFO,DebugUtil::DebugDebug,"end LocalDutyPeer::degrade");
		FUNCTION_EXIT;
	}


	void LocalDutyPeer::regionExecute( IRegSubCommand<Region>& command, bool doApply )
	{
		FUNCTION_ENTRY("regionExecute");

		RegionMap::iterator regIt;

		for( regIt = m_regions.begin(); regIt != m_regions.end(); regIt++ )
		{
			command.execute(*(regIt->second), doApply);			
		}
		FUNCTION_EXIT;
	}

	void LocalDutyPeer::regionExecute( IRegSubCommand<Region>& command, const DASType::RegionSet& regions, bool doApply )
	{
		FUNCTION_ENTRY("regionExecute");

		DASType::RegionSet::const_iterator cmdIt;
		
		if( (regions.find(DASConst::ALL_REGIONS)) != regions.end() )
		{
			regionExecute( command, doApply );
		}
		else
		{
			for( cmdIt = regions.begin(); cmdIt != regions.end(); cmdIt++ )
			{
				regionExecute( command, *cmdIt, doApply );
			}
		}
		FUNCTION_EXIT;
	}


	void LocalDutyPeer::regionExecute( IRegSubCommand<Region>& command, const DASType::DbKey& region, bool doApply )
	{
		FUNCTION_ENTRY("regionExecute");

		RegionMap::iterator regIt;

		if( (regIt = m_regions.find(region)) != m_regions.end() )
		{
			command.execute(*(regIt->second), doApply);			
		}
		FUNCTION_EXIT;
	}

	void LocalDutyPeer::update( StateUpdateSubjectImpl& stateUpdate, const DutyState & state)
	{
		FUNCTION_ENTRY("update");

 		//DutyState* state = stateUpdate.getStateUpdate();
		StateUpdateConverter suc(m_regions);
		state.convert(suc);
	}

	void LocalDutyPeer::update( AgentTerminateSubjectImpl&  terminateUpdate )
	{
		FUNCTION_ENTRY("update");

		StateUpdateParent::detach();
		AgentTerminateParent::detach();

		FUNCTION_EXIT;
	}

}
