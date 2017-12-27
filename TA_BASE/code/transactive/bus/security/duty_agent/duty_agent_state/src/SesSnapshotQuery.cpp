/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_TIP/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/SesSnapshotQuery.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/10/31 12:12:40 $
  *  Last modified by:  $Author: grace.koh $
  *
  *  Gets a session state snapshot.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	SesSnapshotQuery::SesSnapshotQuery(const std::string &sessionId):
	m_sessionId(sessionId),
	m_validToCheckSubsystems(false)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"SesSnapshotQuery::SesSnapshotQuery, session: %s", m_sessionId.c_str());
		m_subsystems.insert(DASConst::ALL_SUBSYSTEMS);
		m_regions.insert   (DASConst::ALL_REGIONS   );
	}

	std::auto_ptr<IDutyQuery> SesSnapshotQuery::clone() const
	{
		std::auto_ptr<IDutyQuery> rval(new SesSnapshotQuery(*this));
		return(rval);
	}

	void SesSnapshotQuery::execute(DutyState& state, DASType::DutyAgentPeer* peer )
	{
		DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"SesSnapshotQuery::execute peer, session: %s", m_sessionId.c_str());
		CORBA_CALL( (*peer),
		            peerGetSessionState,
		            (m_sessionId.c_str(), corbaState.out() ) );
		state.set(corbaState.in());
	}


	void SesSnapshotQuery::execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet)
	{
		DASType::DutySet::const_iterator dsIt;
		m_validToCheckSubsystems = false;
        // TD12082: Make sure generic region information is included for regions that don't have
        //          any allocated duty.
        state.setRegion( region );

		for( dsIt = dutySet.begin(); dsIt != dutySet.end(); dsIt++ )
		{
			state.processRegionPolicyResult(region, *dsIt, IDutyPolicy::PR_ADD );
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"SesSnapshotQuery::execute region DutySet DutyPrimitive: session %s, profile:%lu", 
				 (*dsIt).m_session.c_str(),(*dsIt).m_profile );
		}

		std::vector<DASType::DbKey> profiles;
		DutySessionCache::instance()->getProfiles(m_sessionId, profiles);
		
		std::vector<DASType::DbKey>::iterator end = profiles.end();
		for(std::vector<DASType::DbKey>::iterator itr = profiles.begin(); itr != end; ++itr)
		{
			DutyPrimitive duty(m_sessionId, *itr);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"SesSnapshotQuery::execute region  DutyPrimitive: session %s, profile:%lu", m_sessionId.c_str(),*itr );
			if( dutySet.count(duty) == 1)
			{
				m_validToCheckSubsystems = true;
				break;
			}
		}
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"SesSnapshotQuery::execute region  validToChechSubsystem: %d", m_validToCheckSubsystems);

	}
	bool SesSnapshotQuery::isValidToCheckSubsystems()
	{
		return true;
	}
	void SesSnapshotQuery::execute(DutyState& state, const DASType::DbKey& subsystem,
		const DASType::DutySet& dutySet, const SubsystemState& subState)
	{
		DASType::DutySet::const_iterator dsIt;

        // TD12082: Make sure generic subsystem state information is included for subsystems
        //          that don't have any allocated duty.
        state.setSubsystem( subsystem, subState.getCurrentState(), subState.getCurrentState() );

		if(m_validToCheckSubsystems)
		{
			for( dsIt = dutySet.begin(); dsIt != dutySet.end(); dsIt++ )
			{
				if(m_sessionId.compare(dsIt->m_session) ==0)
				{
					state.processSubsystemPolicyResult(subsystem, subState.getCurrentState(),
						subState.getCurrentState(), *dsIt, IDutyPolicy::PR_ADD );
				}
			}
		}	
	}

	const DASType::SubsystemSet& SesSnapshotQuery::getSubsystems() const
	{
		return( m_subsystems );
	}

	const DASType::RegionSet& SesSnapshotQuery::getRegions() const
	{
		return( m_regions );
	}
}




