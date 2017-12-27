/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/TransferableRegionQuery.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Gets a list of transferable regions.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	TransferableRegionQuery::TransferableRegionQuery( const TA_Base_Bus::SessionInfo& session, bool checkSubs ) :
		m_session(session),
		m_checkSubs(checkSubs)
	{
		m_regions.insert(DASConst::ALL_REGIONS);

		//DutySessionCache::instance()->getProfiles( m_session, m_profiles );
		m_profiles = session.ProfileId;

		if( m_checkSubs )
		{
			m_subsystems.insert(DASConst::ALL_SUBSYSTEMS);
		}
	}

	std::auto_ptr<IDutyQuery> TransferableRegionQuery::clone() const
	{ 
		std::auto_ptr<IDutyQuery> rval(new TransferableRegionQuery(*this));
		return(rval);
   	}

	void TransferableRegionQuery::execute(DutyState& state, DASType::DutyAgentPeer* peer )
	{
		DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession = 
			TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(m_session);
		CORBA_CALL( (*peer),
		            peerGetTransferableRegions,
		            ( corbaState.out(), corbaSession, m_checkSubs ) );
		state.set(corbaState.in());
	}

	bool TransferableRegionQuery::isValidToCheckSubsystems()
	{
		return m_checkSubs;
	}
	void TransferableRegionQuery::execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet)
	{
		m_transfered.clear();

		std::vector<DASType::DbKey>::const_iterator prfIt;

		for( prfIt = m_profiles.begin(); prfIt != m_profiles.end(); prfIt++ )
		{
			DutyPrimitive    dutyToReplace;
			DASType::DutySet dutyNotToAdd;
			DutyPrimitive dp(m_session.SessionId,*prfIt);
			
			// check that the operator doesn't already have duty for this region
			// note that we need to double-check the region because of opertor override
			if( (dutySet.find(dp) != dutySet.end()) ||
				(!DutySessionCache::instance()->isValidRegion(dp, region)) )
			{
				continue;
			}

			DutyContext context;

			DutyContextBuilderFactory::instance()->getDutyContextBuilder().buildXferRegionContext(context, dp);

			IDutyPolicy::POLICY_RESULT result = EvaluateDutyPolicy::evaluate( 
				DutyPolicyFactory::instance()->getDutyPolicy(),
				&IDutyPolicy::evaluateXferRegionPolicy,
				context, dutySet, dutyToReplace, dutyNotToAdd );

			if( dutyToReplace.m_session.empty() )
			{
				state.processRegionPolicyResult( region, dp, result );
			}
			else
			{
				state.processRegionPolicyResult ( region, dp, dutyToReplace, result );
			}

			if( result != IDutyPolicy::PR_DENY )
			{
				m_transfered.insert(dp);
			}
		}
	}

	void TransferableRegionQuery::execute(DutyState& state, const DASType::DbKey& subsystem,
		const DASType::DutySet& dutySet, const SubsystemState& subState)
	{
		if( !m_checkSubs || (m_transfered.size() == 0) )
		{
			return;
		}

		if( dutySet.size() == 0 )
		{
			// already uncontrolled.
			return;
		}

		DASType::DutySet::const_iterator trIt;
		
		// fake the indirect transfer of the subsystem
		PendingDutyHelper duplicateDuty(dutySet);

		for( trIt=m_transfered.begin(); trIt!=m_transfered.end(); trIt++ )
		{
			DASType::DutySet dutyNotToAdd;
			DutyPrimitive dutyToReplace;
			SubsystemState::State nextState(subState.getNextState(SubsystemState::TR_INDIRECT_XFER, *trIt));

			dutyNotToAdd.insert(*trIt);

			DutyContext context;
			
			// build the context
			DutyContextBuilderFactory::instance()->getDutyContextBuilder().buildXferSubViaRegionContext(
				context, *trIt, subsystem, subState.getCurrentState(), nextState );

			IDutyPolicy::POLICY_RESULT result(IDutyPolicy::PR_DENY);

			// apply the policy
			result = EvaluateDutyPolicy::evaluate( DutyPolicyFactory::instance()->getDutyPolicy(),
				&IDutyPolicy::evaluateXferSubViaRegionPolicy,
				context, duplicateDuty.getDuty(), dutyToReplace, dutyNotToAdd );

			duplicateDuty.processPolicyResult( *trIt, dutyToReplace, result );

			// post-processing
			if( subState.getCurrentState() != nextState )
			{
				DASType::DutySet::const_iterator it;
				for( it=duplicateDuty.getDuty().begin(); it!=duplicateDuty.getDuty().end(); it++ )
				{
					if( DutyDbCache::instance()->dutyMayChange( it->m_profile, subsystem,
							subState.getCurrentState(), nextState ) )
					{
						// removals
						if( !(dutyToReplace == *it) )
						{
							duplicateDuty.rem(*it);
						}

						// additions
						if( dutyNotToAdd.find(*it) == dutyNotToAdd.end() )
						{
							duplicateDuty.add(*it);
						}
					}
				}
			}

			duplicateDuty.apply();
		}

		if( duplicateDuty.getDuty().size() == 0 )
		{
			// this is an uncontrolled subsystem!
			state.processSubsystemPolicyResult( subsystem, subState.getCurrentState(), subState.getNextState(),
				m_emptyDuty, IDutyPolicy::PR_ADD );
		}
		
	}

	const DASType::SubsystemSet& TransferableRegionQuery::getSubsystems() const
	{
		return( m_subsystems );
	}

	const DASType::RegionSet& TransferableRegionQuery::getRegions() const
	{
		return( m_regions );
	}
}




