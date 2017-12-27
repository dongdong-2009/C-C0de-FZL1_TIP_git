/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/GetDutyQuery.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/29 17:51:30 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Gets a state snapshot.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	GetDutyQuery::GetDutyQuery( const TA_Base_Bus::SessionInfo& session, const DASType::DbKey& region , const DASType::DbKey& subsystem ) :
		m_session(session),
		m_profiles(session.ProfileId)
	{
		m_subsystems.insert(subsystem);
		m_regions.insert   (region   );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"session: %s, profile size: %d ", session.SessionId.c_str(), m_profiles.size());
		//DutySessionCache::instance()->getProfiles( m_session.SessionId, m_profiles );
	}

	std::auto_ptr<IDutyQuery> GetDutyQuery::clone() const
	{
		std::auto_ptr<IDutyQuery> rval(new GetDutyQuery(*this));
		return(rval);
	}

	void GetDutyQuery::execute(DutyState& state, DASType::DutyAgentPeer* peer )
	{
		DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
			AuthenticationLibrary::convertCSessionToCORBASession(this->m_session);


		CORBA_CALL( (*peer),
		            peerGetDuty,
		            ( corbaState.out(), corbaSession, *(m_regions.begin()), *(m_subsystems.begin()) ) );
		
		state.set(corbaState.in());
	}

	bool GetDutyQuery::isValidToCheckSubsystems()
	{
		bool result = false;
		
		DASType::DutySet::iterator end = m_regionDuty.end();
		for(DASType::DutySet::iterator itr = m_regionDuty.begin(); itr != end; ++itr)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"session: %s, profile:%lu", itr->m_session.c_str(), itr->m_profile);
		}
		result = (m_regionDuty.size()> 0);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"region duties: %d", m_regionDuty.size());
		return result;
	}
	void GetDutyQuery::execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet)
	{
		m_regionDuty.clear();

		std::vector<DASType::DbKey>& profiles = m_profiles;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"session: %s", m_session.SessionId.c_str());
		//DutySessionCache::instance()->getProfiles( m_session.SessionId, profiles );

		std::vector<DASType::DbKey>::const_iterator prfIt;
		//DASType::DutySet::const_iterator dsIt;
		state.setRegion(region);
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "profiles: %d", profiles.size());
		
		for(prfIt = profiles.begin(); prfIt != profiles.end(); prfIt++ )
		{
			DutyPrimitive dp(m_session.SessionId, *prfIt);		
			// check that the operator has duty for this region
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "session:%s, profile:%lu", dp.m_session.c_str(), dp.m_profile);
			if( dutySet.find(dp) != dutySet.end() )
			{
				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "found DutyPrimitive session:%s, profile:%lu", dp.m_session.c_str(), dp.m_profile);
				m_regionDuty.insert(dp);
				state.processRegionPolicyResult(region, dp, IDutyPolicy::PR_ADD );
			}
		}
		
	}

	void GetDutyQuery::execute(DutyState& state, const DASType::DbKey& subsystem,
		const DASType::DutySet& dutySet, const SubsystemState& subState)
	{
		DASType::DutySet::const_iterator regionDutyIt;
		
		state.setSubsystem( subsystem, subState.getCurrentState(), subState.getCurrentState() );
		for( regionDutyIt = m_regionDuty.begin(); regionDutyIt != m_regionDuty.end(); regionDutyIt++ )
		{
			if( dutySet.find(*regionDutyIt) != dutySet.end() )
			{
				state.processSubsystemPolicyResult( subsystem, subState.getCurrentState(),
					subState.getCurrentState(), *regionDutyIt, IDutyPolicy::PR_ADD );
			}
		}
	}

	const DASType::SubsystemSet& GetDutyQuery::getSubsystems() const
	{
		return( m_subsystems );
	}

	const DASType::RegionSet& GetDutyQuery::getRegions() const
	{
		return( m_regions );
	}
}




