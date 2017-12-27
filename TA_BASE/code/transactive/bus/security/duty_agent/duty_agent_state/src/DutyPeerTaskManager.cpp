/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskManager.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/04/08 18:05:51 $
 * Last modified by:  $Author: liwei.gong $
 *
 * Maintains a list of Duty Agent peers and acts as an intemdiary between the loca
 * Duty Agent front end and the peers.
 * 
 * Also maintains a list of outstanding requests. This is used to prevent multiple
 * requests being processed for the same subsystem. If this situation should occur
 * it will deny the request giving the reason as "outstanding requests".
 *
 */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Bus
{
	// SesInfoCmd
	DutyPeerTaskManager::SesInfoCmd::SesInfoCmd( ACE_Future<bool> (DutyPeerTask::*cmd)(DutyState&, const SessionInfo&),
		void (DutyPeerTask::*cmdImpl)(DutyState&, const SessionInfo&),
		const SessionInfo& session ) :
		m_cmd(cmd),
		m_session(session),
		m_cmdImpl(cmdImpl)
	{
	}

	std::auto_ptr<IDutyPeerTaskCommand> DutyPeerTaskManager::SesInfoCmd::clone() const
	{
		std::auto_ptr<IDutyPeerTaskCommand> rval(new SesInfoCmd(*this));
		return(rval);
   	}

	ACE_Future<bool> DutyPeerTaskManager::SesInfoCmd::execute(DutyPeerTask& peerTask, DutyState& state)
	{
		return( (peerTask.*m_cmd)(state, m_session) );
	}
	void DutyPeerTaskManager::SesInfoCmd::directExecute(DutyPeerTask& peerTask, DutyState& state)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before SesInfoCmd::directExecute");
		 (peerTask.*m_cmdImpl)(state, m_session) ;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after SesInfoCmd::directExecute");
	}


	// SesEndCmd
	DutyPeerTaskManager::SesEndCmd::SesEndCmd( const SessionInfo& session, const DASType::RegionSet& regions ) :
		m_session(session),
		m_regions(regions)
	{
	}

	std::auto_ptr<IDutyPeerTaskCommand> DutyPeerTaskManager::SesEndCmd::clone() const
	{
		std::auto_ptr<IDutyPeerTaskCommand> rval(new SesEndCmd(*this));
		return(rval);
   	}

	ACE_Future<bool> DutyPeerTaskManager::SesEndCmd::execute(DutyPeerTask& peerTask, DutyState& state)
	{
		return( peerTask.sessionEnd(state, m_session, m_regions) );
	}

	void DutyPeerTaskManager::SesEndCmd::directExecute(DutyPeerTask& peerTask, DutyState& state)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before SesEndCmd::directExecute");
		peerTask.sessionEndImpl(state, m_session, m_regions) ;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after SesEndCmd::directExecute");
	}
	// DegradeCmd
	DutyPeerTaskManager::DegradeCmd::DegradeCmd( const DASType::SubsystemSet& subsystems ) :
		m_subsystems(subsystems)
	{
	}

	std::auto_ptr<IDutyPeerTaskCommand> DutyPeerTaskManager::DegradeCmd::clone() const
	{
		std::auto_ptr<IDutyPeerTaskCommand> rval(new DegradeCmd(*this));
		return(rval);
	}

	ACE_Future<bool> DutyPeerTaskManager::DegradeCmd::execute(DutyPeerTask& peerTask, DutyState& state)
	{
		return( peerTask.degrade( state, m_subsystems ) );
	}

	void DutyPeerTaskManager::DegradeCmd::directExecute(DutyPeerTask& peerTask, DutyState& state)
	{
		 LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before DegradeCmd::directExecute");
		 peerTask.degradeImpl( state, m_subsystems ) ;
		 LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after DegradeCmd::directExecute");
	}
	// XferSubCmd
	DutyPeerTaskManager::XferSubCmd::XferSubCmd( const TA_Base_Bus::SessionInfo& session, const DASType::SubsystemMap& subsystems ) :
		m_session(session),
		m_subsystems(subsystems)
	{
	}

	std::auto_ptr<IDutyPeerTaskCommand> DutyPeerTaskManager::XferSubCmd::clone() const
	{
		std::auto_ptr<IDutyPeerTaskCommand> rval(new XferSubCmd(*this));
		return(rval);
	}

	ACE_Future<bool> DutyPeerTaskManager::XferSubCmd::execute(DutyPeerTask& peerTask, DutyState& state)
	{
		return( peerTask.transfer( state, m_session, m_subsystems ) );
	}

	void DutyPeerTaskManager::XferSubCmd::directExecute(DutyPeerTask& peerTask, DutyState& state)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before XferSubCmd::directExecute");
		peerTask.transferImpl( state, m_session, m_subsystems ) ;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after XferSubCmd::directExecute");
	}

	// XferRegCmd
	DutyPeerTaskManager::XferRegCmd::XferRegCmd( const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions ) :
		m_session(session),
		m_regions(regions)
	{
	}

	std::auto_ptr<IDutyPeerTaskCommand> DutyPeerTaskManager::XferRegCmd::clone() const
	{
		std::auto_ptr<IDutyPeerTaskCommand> rval(new XferRegCmd(*this));
		return(rval); 
	}

	ACE_Future<bool> DutyPeerTaskManager::XferRegCmd::execute( DutyPeerTask& peerTask, DutyState& state )
	{
		return( peerTask.transfer( state, m_session, m_regions ) );
	}


	void DutyPeerTaskManager::XferRegCmd::directExecute( DutyPeerTask& peerTask, DutyState& state )
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before XferRegCmd::directExecute");
		peerTask.transferImpl( state, m_session, m_regions ) ;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after XferRegCmd::directExecute");
	}



	// QueryCmd
	DutyPeerTaskManager::QueryCmd::QueryCmd( std::auto_ptr<IDutyQuery> query ) :
		m_query(query)
	{
	}

	std::auto_ptr<IDutyPeerTaskCommand> DutyPeerTaskManager::QueryCmd::clone() const
	{ 
		std::auto_ptr<IDutyPeerTaskCommand> rval(new QueryCmd(m_query->clone()));
		return(rval);
	}

	ACE_Future<bool> DutyPeerTaskManager::QueryCmd::execute(DutyPeerTask& peerTask, DutyState& state)
	{
		return( peerTask.executeQuery( state, *m_query ) );
	}

	void DutyPeerTaskManager::QueryCmd::directExecute(DutyPeerTask& peerTask, DutyState& state)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before QueryCmd::directExecute");
		peerTask.executeQueryImpl( state, *m_query ) ;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after QueryCmd::directExecute");
	}
	// DutyPeerTaskManager
	DutyPeerTaskManager::DutyPeerTaskManager()
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyPeerTaskManager::DutyPeerTaskManager()");
		// Build the peer task map
		DASType::LocationSet locations;
		DASType::LocationSet::const_iterator locIt;

		int threadCount = 0;
		if( RunParams::getInstance().isSet(RPARAM_DUTY_THREAD_COUNT) )
		{
			threadCount = atoi( RunParams::getInstance().get(RPARAM_DUTY_THREAD_COUNT).c_str() );
			threadCount = threadCount > 0 ? threadCount : 0;
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Duty Peer Task thread count is set to %d",threadCount);
		}

		DutyDbCache::instance()->getAllLocationKeys( locations );

		for( locIt = locations.begin(); locIt != locations.end(); locIt++ )
		{
			std::auto_ptr<IDutyPeer> newDutyPeer;

			bool bLocalPeer = false;
			if( *locIt == TA_Base_Bus::TransactiveAgentInfo::m_location )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "begin Creating LocalDutyPeer for location %lu", *locIt );
				std::auto_ptr<IDutyPeer> tmp(new LocalDutyPeer(*locIt));
				newDutyPeer = tmp;
				bLocalPeer = true;
				//threadCount = 1; //jinmin+
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "after Creating LocalDutyPeer for location %lu", *locIt );
			}
			else
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "begin Creating RemoteDutyPeer for location %lu", *locIt );
				std::auto_ptr<IDutyPeer> tmp(new RemoteDutyPeer(*locIt));
				newDutyPeer = tmp;
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "after Creating RemoteDutyPeer for location %lu", *locIt );
			}

			std::auto_ptr<DutyPeerTask> newTask(new DutyPeerTask(newDutyPeer,bLocalPeer,threadCount));

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin DutyPeerTaskManager open DutyPeerTask");
			newTask->open(0);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyPeerTaskManager open DutyPeerTask");

			m_taskMap.insert( TaskMap::value_type(*locIt, newTask.release()) );
		}

		// Build the region map
		DutyDbCache::instance()->buildRegionMap(m_regionMap);
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end DutyPeerTaskManager::DutyPeerTaskManager()");
	}

	DutyPeerTaskManager::~DutyPeerTaskManager()
	{
		TaskMap::iterator it;

		for( it = m_taskMap.begin(); it != m_taskMap.end(); it++ )
		{
			it->second->terminate();
		}

		for( it = m_taskMap.begin(); it != m_taskMap.end(); it++ )
		{
			it->second->wait();
			delete it->second;
			it->second = NULL;
		}
	}

	void DutyPeerTaskManager::sessionBegin( DutyState& state, const SessionInfo& session,
		const DASType::LocationSet& locations )
	{
		SesInfoCmd sesBegin(&DutyPeerTask::sessionBegin,&DutyPeerTask::sessionBeginImpl, session);

		DASType::LocationSet optLocations;
		restrictLocations( session, locations, optLocations );

		peerExecute( state, sesBegin, optLocations );
	}

	void DutyPeerTaskManager::sessionEnd( DutyState& state, const SessionInfo& session,
		const DASType::LocationSet& locations, const DASType::RegionSet& regions )
	{
		SesEndCmd sesEnd( session, regions );

		DASType::LocationSet optLocations;
		restrictLocations( session, locations, optLocations );

		peerExecute( state, sesEnd, optLocations );
	}

	void DutyPeerTaskManager::sessionOverrideBegin( DutyState& state, const SessionInfo& session,
		const DASType::LocationSet& locations )
	{
		SesInfoCmd sesOvrBegin(&DutyPeerTask::sessionOverrideBegin,&DutyPeerTask::sessionOverrideBeginImpl ,session);


		DASType::LocationSet optLocations;
		std::vector<DASType::DbKey> profile;
		profile.push_back( session.ProfileId.back() );
		restrictLocations( profile, session.LocationId, locations, optLocations );

		peerExecute( state, sesOvrBegin, optLocations );
	}

	void DutyPeerTaskManager::sessionOverrideEnd( DutyState& state, const SessionInfo& session,
		const DASType::LocationSet& locations )
	{
		SesInfoCmd sesOvrEnd(&DutyPeerTask::sessionOverrideEnd,&DutyPeerTask::sessionOverrideEndImpl, session);

		DASType::LocationSet optLocations;
		std::vector<DASType::DbKey> profile;
		profile.push_back( session.ProfileId.back() );
		restrictLocations( profile, session.LocationId, locations, optLocations );

		peerExecute( state, sesOvrEnd, optLocations );
	}

	void DutyPeerTaskManager::degrade( DutyState& state, const DASType::SubsystemSet& subsystems,
		const DASType::LocationSet& locations )
	{
		DegradeCmd degradeCmd( subsystems );
		peerExecute( state, degradeCmd, locations );
	}

	void DutyPeerTaskManager::getTransferableRegions( DutyState& state, const TA_Base_Bus::SessionInfo& session,
		const DASType::LocationSet& locations, bool checkSubs )
	{
		DASType::LocationSet optLocations;
		restrictLocations( session, locations, optLocations );

		std::auto_ptr<IDutyQuery> xferQuery( new TransferableRegionQuery(session, checkSubs) );

		QueryCmd xferQueryCmd(xferQuery);
		peerExecute( state, xferQueryCmd, optLocations );
	}

	void DutyPeerTaskManager::getTransferableSubsystems( DutyState& state, const TA_Base_Bus::SessionInfo& session,
		const DASType::RegionSet& regions )
	{
		DASType::RegionSet::const_iterator regIt;
		DASType::ReverseRegionMap::const_iterator regMapIt;
		
		std::map<DASType::DbKey, DASType::RegionSet* > newRegSets;
		std::map<DASType::DbKey, DASType::RegionSet* >::iterator newRegSetIt;

		// create a new region set for each location
		for( regIt=regions.begin(); regIt!=regions.end(); regIt++ )
		{
			if( (regMapIt=m_regionMap.find(*regIt)) != m_regionMap.end() )
			{
				if( (newRegSetIt = newRegSets.find(regMapIt->second)) != newRegSets.end() )
				{
					newRegSetIt->second->insert( *regIt );
				}
				else
				{
					DASType::RegionSet* newSet = new DASType::RegionSet;
					newSet->insert(*regIt);
					newRegSets.insert( std::map<DASType::DbKey, DASType::RegionSet* >::value_type(
						regMapIt->second, newSet) );
				}
			}
		}

		// create a command for each location
		CommandMap cmdMap;

		for( newRegSetIt = newRegSets.begin(); newRegSetIt != newRegSets.end(); newRegSetIt++ )
		{
			{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) Before call to TransferableSubsystemQuery(): region dbkey:%lu", 
				newRegSetIt->first);
			std::string temp = "TD15346: session: ";
			temp += session.SessionId;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
			//++TD15346
			}

			std::auto_ptr<IDutyQuery> xferQuery( new TransferableSubsystemQuery(session, *(newRegSetIt->second)) );

			{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) After call to TransferableSubsystemQuery(): region dbkey:%lu", 
				newRegSetIt->first);
			std::string temp = "TD15346: session: ";
			temp += session.SessionId;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
			//++TD15346
			}

			IDutyPeerTaskCommand* xferQueryCmd = new QueryCmd(xferQuery);

			cmdMap.insert( CommandMap::value_type(newRegSetIt->first,xferQueryCmd) );
		}
		
		peerExecute( state, cmdMap );
		
		CommandMap::iterator cmdMapIt;

		for( newRegSetIt = newRegSets.begin(); newRegSetIt != newRegSets.end(); newRegSetIt++ )
		{
			delete newRegSetIt->second;
			newRegSetIt->second = NULL;
		}

		for( cmdMapIt = cmdMap.begin(); cmdMapIt != cmdMap.end(); cmdMapIt++ )
		{
			delete cmdMapIt->second;
			cmdMapIt->second = NULL;
		}
	}

	// All the mucking about with maps and sets here is to restrict the amount of data that needs to cross
	// the wire. That is, we only send out data to peers that the peers acutally need. Thus trading CPU time
	// for bandwidth.
	void DutyPeerTaskManager::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session,
		const DASType::SubsystemMap& subsystems )
	{
		DASType::ReverseRegionMap::const_iterator regionMapIt;
		DASType::RegionSet::const_iterator regionIt;
		DASType::SubsystemMap::const_iterator subMapIt;
		CommandMap cmdMap;

		std::map<DASType::DbKey, DASType::SubsystemMap* > newSubMaps;
		std::map<DASType::DbKey, DASType::SubsystemMap* >::iterator newSubMapsIt;

		// create a new subsystem map for each location
		for( subMapIt = subsystems.begin(); subMapIt != subsystems.end(); subMapIt++ )
		{
			if( (regionMapIt = m_regionMap.find(subMapIt->first)) != m_regionMap.end() )
			{
				if( (newSubMapsIt = newSubMaps.find(regionMapIt->second)) != newSubMaps.end() )
				{
					newSubMapsIt->second->insert( DASType::SubsystemMap::value_type(subMapIt->first,subMapIt->second) );
				}
				else
				{
					DASType::SubsystemMap* newMap = new DASType::SubsystemMap;
					newMap->insert( DASType::SubsystemMap::value_type(subMapIt->first,subMapIt->second) );
					newSubMaps.insert( std::map<DASType::DbKey, DASType::SubsystemMap* >::value_type(regionMapIt->second,
						newMap) );
				}
			}
		}

		// create a coresponding command for each locaiton
		for( newSubMapsIt = newSubMaps.begin(); newSubMapsIt != newSubMaps.end(); newSubMapsIt++ )
		{
			IDutyPeerTaskCommand* newXferCmd = new XferSubCmd( session, *(newSubMapsIt->second) );

			cmdMap.insert( CommandMap::value_type(newSubMapsIt->first,newXferCmd) );
		}
		
		// execute the map of commands
		peerExecute( state, cmdMap );

		// clean up
		CommandMap::iterator cmdMapIt;

		for( newSubMapsIt = newSubMaps.begin(); newSubMapsIt != newSubMaps.end(); newSubMapsIt++ )
		{
			delete newSubMapsIt->second;
			newSubMapsIt->second = NULL;
		}

		for( cmdMapIt = cmdMap.begin(); cmdMapIt != cmdMap.end(); cmdMapIt++ )
		{
			delete cmdMapIt->second;
			cmdMapIt->second = NULL;
		}
	}

	// All the mucking about with maps and sets here is to restrict the amount of data that needs to cross
	// the wire. That is, we only send out data to peers that the peers acutally need. Thus trading CPU time
	// for bandwidth.
	void DutyPeerTaskManager::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions )
	{
		DASType::ReverseRegionMap::const_iterator regionMapIt;
		DASType::RegionSet::const_iterator regionIt;
		DASType::RegionSet::const_iterator extRegionIt;
		CommandMap cmdMap;

		std::map<DASType::DbKey,DASType::RegionSet* > newRegSets;
		std::map<DASType::DbKey,DASType::RegionSet* >::iterator newRegSetsIt;

		// create a new set of region set for each location
		for( extRegionIt = regions.begin(); extRegionIt != regions.end(); extRegionIt++ )
		{
			if( (regionMapIt = m_regionMap.find(*extRegionIt)) != m_regionMap.end() )
			{
				if( (newRegSetsIt = newRegSets.find(regionMapIt->second)) != newRegSets.end() )
				{
					newRegSetsIt->second->insert( *extRegionIt );
				}
				else
				{
					DASType::RegionSet* newSet = new DASType::RegionSet;
					newSet->insert( *extRegionIt );
					newRegSets.insert( std::map<DASType::DbKey, DASType::RegionSet* >::value_type(regionMapIt->second,
						newSet) );
				}
			}
		}

		// create a coresponding command for each locaiton
		for( newRegSetsIt = newRegSets.begin(); newRegSetsIt != newRegSets.end(); newRegSetsIt++ )
		{
			IDutyPeerTaskCommand* newXferCmd = new XferRegCmd( session, *(newRegSetsIt->second) );

			cmdMap.insert( CommandMap::value_type(newRegSetsIt->first,newXferCmd) );
		}
		
		// execute the map of commands
		peerExecute( state, cmdMap );

		// clean up
		CommandMap::iterator cmdMapIt;

		for( newRegSetsIt = newRegSets.begin(); newRegSetsIt != newRegSets.end(); newRegSetsIt++ )
		{
			delete newRegSetsIt->second;
			newRegSetsIt->second = NULL;
		}

		for( cmdMapIt = cmdMap.begin(); cmdMapIt != cmdMap.end(); cmdMapIt++ )
		{
			delete cmdMapIt->second;
			cmdMapIt->second = NULL;
		}
	}

	void DutyPeerTaskManager::getState( DutyState& state, const DASType::LocationSet& locations )
	{
		std::auto_ptr<IDutyQuery> snapQuery(new SnapshotQuery());
		
		QueryCmd snapQueryCmd(snapQuery);
		peerExecute( state, snapQueryCmd, locations );
	}

	void DutyPeerTaskManager::getSessionState( DutyState& state,const std::string& session, const DASType::LocationSet& locations )
	{
		std::auto_ptr<IDutyQuery> snapQuery(new SesSnapshotQuery(session));
		
		QueryCmd snapQueryCmd(snapQuery);
		peerExecute( state, snapQueryCmd, locations );
	}
	void DutyPeerTaskManager::getSubsystemState( DutyState& state, const DASType::SubsystemSet& subsystems,const DASType::LocationSet& locations )
	{
		std::auto_ptr<IDutyQuery> snapQuery(new SubSnapshotQuery(subsystems));
		
		QueryCmd snapQueryCmd(snapQuery);
		peerExecute( state, snapQueryCmd, locations );
	}

	void DutyPeerTaskManager::getDuty( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::DbKey& region,
		const DASType::DbKey& subsystem )
	{
		DASType::ReverseRegionMap::const_iterator regIt;
		bool needToAskPeer = false;

		DASType::LocationSet constraints;
		constraints.insert(DASConst::ALL_LOCATIONS);

		
		DASType::LocationSet optLocations;
		restrictLocations( session, constraints, optLocations );

		// Find the region being asked about in our map and see if any of the restrictd locations
		// are associated with that region. If they are then we need to phone a friend.
		if( (regIt = m_regionMap.find(region)) == m_regionMap.end() )
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"can't found region %lu", region);
			return;
		}

// 		if( (optLocations.find(regIt->second) != optLocations.end()) ||
// 			(optLocations.find(DASConst::ALL_LOCATIONS) != optLocations.end()) )
// 		{
// 			needToAskPeer = true;
// 		}

		// If we need to ask a peer then build the query and execute it.
// 		if( needToAskPeer )
// 		{
			std::auto_ptr<IDutyQuery> getDutyQuery(new GetDutyQuery(session, region, subsystem));

			QueryCmd getDutyQueryCmd(getDutyQuery);

			DASType::LocationSet locSet;
			locSet.insert(regIt->second);
			peerExecute( state, getDutyQueryCmd, locSet );
	// 	}
	}

	void DutyPeerTaskManager::getExclusiveDuty( DutyState& state, const TA_Base_Bus::SessionInfo& session, 
			const DASType::LocationSet& locations )
	{
		DASType::LocationSet optLocations;
		restrictLocations( session, locations, optLocations );

		std::auto_ptr<IDutyQuery> getDutyQuery(new GetExclusiveDutyQuery(session));

		QueryCmd getDutyQueryCmd(getDutyQuery);
		peerExecute( state, getDutyQueryCmd, optLocations );
	}

	void DutyPeerTaskManager::peerExecute( DutyState& state, IDutyPeerTaskCommand& command,
		const DASType::LocationSet& locations )
	{
		FUNCTION_ENTRY("DutyPeerTaskManager::peerExecute");
		
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "begin DutyPeerTaskManager::peerExecute location command");
		this->showLocationString(locations);
		TaskMap::iterator taskIt;
		DutyPeerTaskResultHandler resultHandler(state);

		if( locations.find(DASConst::ALL_LOCATIONS) != locations.end() )
		{
			for( taskIt = m_taskMap.begin(); taskIt != m_taskMap.end(); taskIt++ )
			{
				resultHandler.set( taskIt->first, *(taskIt->second), command.clone(),false );
			}

		}
		else
		{
			DASType::LocationSet::const_iterator locIt;
			if(locations.size() ==1) //direct call, instead of going through queue
			{
				locIt = locations.begin();
				taskIt = m_taskMap.find(*locIt);
				if(taskIt != m_taskMap.end())
				{
					resultHandler.set( taskIt->first, *(taskIt->second), command.clone(),true);
				}

			}
			else
			{
				for( locIt = locations.begin(); locIt != locations.end(); locIt++ )
				{
					if( (taskIt = m_taskMap.find(*locIt)) != m_taskMap.end() )
					{
						resultHandler.set( taskIt->first, *(taskIt->second), command.clone(),false);
					}
				}

			}

		}

		DASType::LocationSet failedPeers;

		resultHandler.waitOnAll(failedPeers);

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "end DutyPeerTaskManager::peerExecute location command");
		//handleFailedPeers(failedPeers);
		FUNCTION_EXIT;
	}

	void DutyPeerTaskManager::peerExecute( DutyState& state, CommandMap& commandMap )
	{
		FUNCTION_ENTRY("DutyPeerTaskManager::peerExecute");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "begin DutyPeerTaskManager::peerExecute command");
		this->showLocationString(commandMap);
		CommandMap::iterator cmdMapIt;
		TaskMap::iterator taskIt;
		DutyPeerTaskResultHandler resultHandler(state);

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "DutyPeerTaskManager::peerExecute commandMap.size %d", commandMap.size());
		if(commandMap.size()==1)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "DutyPeerTaskManager::peerExecute before directCALL");
			for( cmdMapIt = commandMap.begin(); cmdMapIt != commandMap.end(); cmdMapIt++ )
			{
				if( (taskIt = m_taskMap.find(cmdMapIt->first)) != m_taskMap.end() )
				{
					resultHandler.set( taskIt->first, *(taskIt->second), *(cmdMapIt->second),true );
				}
			}
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "DutyPeerTaskManager::peerExecute after directCALL");
		}
		else
		{
			for( cmdMapIt = commandMap.begin(); cmdMapIt != commandMap.end(); cmdMapIt++ )
			{
				if( (taskIt = m_taskMap.find(cmdMapIt->first)) != m_taskMap.end() )
				{
					resultHandler.set( taskIt->first, *(taskIt->second), *(cmdMapIt->second),false );
				}
			}
		}

		DASType::LocationSet failedPeers;

		resultHandler.waitOnAll(failedPeers);

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "end DutyPeerTaskManager::peerExecute command");
		FUNCTION_EXIT;
		//handleFailedPeers(failedPeers);
	}

	void DutyPeerTaskManager::restrictLocations( const SessionInfo& session,
		const DASType::LocationSet& locationConstraints, DASType::LocationSet& restrictedLocations )
	{
		restrictLocations( session.ProfileId, session.LocationId, locationConstraints, restrictedLocations );
	}

	void DutyPeerTaskManager::restrictLocations( const std::string& session,
		const DASType::LocationSet& locationConstraints, DASType::LocationSet& restrictedLocations )
	{
		std::vector<DASType::DbKey> profiles;
		DutySessionCache::instance()->getProfiles(session, profiles);

		restrictLocations( profiles, DutySessionCache::instance()->getLocation(session),
			locationConstraints, restrictedLocations );
	}

	void DutyPeerTaskManager::restrictLocations( const std::vector<DASType::DbKey>& profiles,
		const DASType::DbKey& sessionLocation, const DASType::LocationSet& locationConstraints,
		DASType::LocationSet& restrictedLocations )
	{
		std::vector<unsigned long>::const_iterator prfIt;

		for( prfIt = profiles.begin(); prfIt != profiles.end(); prfIt++ )
		{
			if( IDutyPolicy::PT_CENTRAL == DutyDbCache::instance()->getProfileType( *prfIt ) )
			{
				DASType::LocationSet::const_iterator it;

				for( it = locationConstraints.begin(); it != locationConstraints.end(); it++ )
				{
					restrictedLocations.insert( *it );
				}
			}
			else
			{
				if( (locationConstraints.find(sessionLocation) != locationConstraints.end()) ||
					(locationConstraints.find(DASConst::ALL_LOCATIONS) != locationConstraints.end()) )
				{
					restrictedLocations.insert( sessionLocation );
				}
			}
		}
	}


	void DutyPeerTaskManager::updateDutyPeer(DASType::DbKey locationKey, std::auto_ptr<IDutyPeer> dutyPeer)
	{
		std::map<DASType::DbKey, DutyPeerTask*>::iterator findItr = m_taskMap.find(locationKey);
		if(findItr != m_taskMap.end())
		{
			findItr->second->terminateAndWait();
			findItr->second->setDutyPeer(dutyPeer);
			findItr->second->open(NULL);
		}
		
	}

	void	DutyPeerTaskManager::showLocationString(const DASType::LocationSet& locations)
	{
		if (!locations.empty())
		{
			DASType::LocationSet::const_iterator locationItr = locations.begin();
			std::ostringstream resultStream;
			resultStream<<"[";
			for(;locationItr != locations.end();++locationItr)
			{
				resultStream<<(*locationItr)<<",";
			}
			resultStream<<"]";
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "using location %s",resultStream.str().c_str());

			return;
		}
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn, "Location set is empty, cannot show location string.");
	}

	
	void	DutyPeerTaskManager::showLocationString(CommandMap& commandMap)
	{
		if (!commandMap.empty())
		{
			CommandMap::iterator itr = commandMap.begin();
			std::ostringstream resultStream;
			resultStream<<"[";
			for(;itr != commandMap.end();++itr)
			{
				resultStream<<itr->first<<",";
			}
			resultStream<<"]";
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "using location %s",resultStream.str().c_str());

			return;
		}

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn, "Location set is empty, cannot show location string.");
	}
}
