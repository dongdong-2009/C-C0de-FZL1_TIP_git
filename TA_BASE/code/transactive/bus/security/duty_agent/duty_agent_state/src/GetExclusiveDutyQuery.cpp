/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/GetExclusiveDutyQuery.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Queries exclusive duty for the session for all regions.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	GetExclusiveDutyQuery::GetExclusiveDutyQuery( const TA_Base_Bus::SessionInfo& session ) :
		m_session(session)
	{
		m_subsystems.insert(DASConst::ALL_SUBSYSTEMS);
		m_regions.insert   (DASConst::ALL_REGIONS   );

		//DutySessionCache::instance()->getProfiles( m_session, m_profiles );
		m_profiles = session.ProfileId;
	}

	std::auto_ptr<IDutyQuery> GetExclusiveDutyQuery::clone() const
	{
		std::auto_ptr<IDutyQuery> rval(new GetExclusiveDutyQuery(*this));
		return(rval);
	}

	void GetExclusiveDutyQuery::execute(DutyState& state, DASType::DutyAgentPeer* peer )
	{
		TA_ASSERT( NULL != peer, "Peer must not be NULL" );

		DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
			AuthenticationLibrary::convertCSessionToCORBASession(this->m_session);

		CORBA_CALL( (*peer),
		            peerGetExclusiveDuty,
		            ( corbaState.out(), corbaSession ) );
		state.set(corbaState.in());
	}

	bool GetExclusiveDutyQuery::isValidToCheckSubsystems()
	{
		bool result = false;

		result = (m_regionDuty.size()> 0);

		return result;
	}
	void GetExclusiveDutyQuery::execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet)
	{
		state.setRegion(region);

		m_regionDuty.clear();

		std::vector<DASType::DbKey>::const_iterator prfIt;
		DASType::DutySet::const_iterator dsIt;

		for(prfIt = m_profiles.begin(); prfIt != m_profiles.end(); prfIt++ )
		{
			DutyPrimitive dp(m_session.SessionId, *prfIt);

			// check that the operator doesn't already have duty for this region
			if( dutySet.find(dp) != dutySet.end() )
			{
				m_regionDuty.insert(dp);
				state.processRegionPolicyResult( region, dp, IDutyPolicy::PR_ADD );
			}
		}		
	}

	void GetExclusiveDutyQuery::execute(DutyState& state, const DASType::DbKey& subsystem,
		const DASType::DutySet& dutySet, const SubsystemState& subState)
	{
		if( DASConst::POLICY_TYPE_RAIL == TransactiveAgentInfo::m_policyType )
		{
			if( IDutyPolicy::ST_EXCLUSIVE != DutyDbCache::instance()->getSubsystemType( subsystem ) )
			{
				return;
			}
		DASType::DutySet::const_iterator regionDutyIt;

		for( regionDutyIt = m_regionDuty.begin(); regionDutyIt != m_regionDuty.end(); regionDutyIt++ )
		{
			if( dutySet.find(*regionDutyIt) != dutySet.end() )
			{
				if( IDutyPolicy::RT_EXCLUSIVE == DutyDbCache::instance()->getRightsType( regionDutyIt->m_profile,
					subsystem, subState.getCurrentState() ) )
				{
					state.processSubsystemPolicyResult( subsystem, subState.getCurrentState(),
						subState.getCurrentState(), *regionDutyIt, IDutyPolicy::PR_ADD );
				}
				}
			}
		}
		else if( DASConst::POLICY_TYPE_ROAD == TransactiveAgentInfo::m_policyType )
		{
			// for either of these cases the sub is not going to be left dutyless
			if( (m_regionDuty.size() == 0)  || (m_regionDuty.size() < dutySet.size()) )
			{
				return;
			}

			// for this case, all the session/profile combos must be present otherwise
			// someone else has duty
			if( m_regionDuty.size() >= dutySet.size() )
			{
				DASType::DutySet::const_iterator subDutyIt = dutySet.begin();

				bool otherDuty = false;
			
				// check that the subsystem duties are a subset of the region duties.
				while( !otherDuty && subDutyIt != dutySet.end() )
				{
					if( m_regionDuty.find(*subDutyIt) == m_regionDuty.end() )
					{
						// this subsystem duty is not in the region duty set
						otherDuty = true;
					}
					subDutyIt++;
				}

				if( otherDuty )
				{
					return;
				}

				state.processSubsystemPolicyResult( subsystem, subState.getCurrentState(),
					subState.getCurrentState(), *(m_regionDuty.begin()), IDutyPolicy::PR_ADD );
				
			}
		}
	}

	const DASType::SubsystemSet& GetExclusiveDutyQuery::getSubsystems() const
	{
		return( m_subsystems );
	}

	const DASType::RegionSet& GetExclusiveDutyQuery::getRegions() const
	{
		return( m_regions );
	}
}




