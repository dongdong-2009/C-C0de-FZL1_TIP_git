/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *

  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/TransferableSubsystemQuery.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/29 17:51:30 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Gets a list of transferable subsystems.
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

namespace TA_Base_Bus
{
	TransferableSubsystemQuery::TransferableSubsystemQuery( const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions ) :
		m_session(session),
		m_regions(regions)
	{
		m_subsystems.insert(DASConst::ALL_SUBSYSTEMS);
	}

	std::auto_ptr<IDutyQuery> TransferableSubsystemQuery::clone() const
	{
		std::auto_ptr<IDutyQuery> rval(new TransferableSubsystemQuery(*this));
		return(rval);
	}

	void TransferableSubsystemQuery::execute(DutyState& state, DASType::DutyAgentPeer* peer )
	{
		DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);

		DutyAgentTypeCorbaDef::KeySequence regionSeq;
		regionSeq.length(m_regions.size());
		
		DASType::RegionSet::const_iterator it;
		CORBA::ULong i=0;
		for( it=m_regions.begin(); it!=m_regions.end(); it++, i++ )
		{
			regionSeq[i] = *it;
		}

		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: Before call to peerGetTransferableSubsystems(0: region size:%lu", 
			regionSeq.length());
		//++TD15346

		TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef corbaSession =
			TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(m_session);
		CORBA_CALL( (*peer),
		            peerGetTransferableSubsystems,
		            ( corbaState.out(), corbaSession, regionSeq ) );
		state.set(corbaState.in());
	}

	bool TransferableSubsystemQuery::isValidToCheckSubsystems()
	{
		bool result = false;
		
		result = (m_regionDuty.size()> 0);

		return result; 
	}

	void TransferableSubsystemQuery::execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet)
	{
		m_regionDuty.clear();

		std::vector<DASType::DbKey> profiles;
		std::vector<DASType::DbKey>::const_iterator prfIt;
		DASType::DutySet::const_iterator dsIt;

		//DutySessionCache::instance()->getProfiles( m_session, profiles );
		profiles = m_session.ProfileId;

		for( prfIt = profiles.begin(); prfIt != profiles.end(); prfIt++ )
		{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) Before call to m_regionDuty.insert(): profile dbkey:%lu", 
				*prfIt);
			//++TD15346

			DutyPrimitive dp(m_session.SessionId, *prfIt);

			// check that the operator does have duty for this region
			if( dutySet.find(dp) != dutySet.end() )
			{	
				m_regionDuty.insert(dp);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15346: Operator have duty for this region"); //TD15346
			}

			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) After call to m_regionDuty.insert(): profile dbkey:%lu", 
				*prfIt);
			//++TD15346
		}

		state.setRegion(region);

	}

	void TransferableSubsystemQuery::execute(DutyState& state, const DASType::DbKey& subsystem,
		const DASType::DutySet& dutySet, const SubsystemState& subState)
	{
		DASType::DutySet::const_iterator regionDutyIt;

		// check for duplicates
		bool allDuplicates = true;
		regionDutyIt = m_regionDuty.begin();

		while( (allDuplicates) && (regionDutyIt != m_regionDuty.end()) )
		{
			if( dutySet.find(*regionDutyIt) == dutySet.end() )
			{
				allDuplicates = false;
			}
			else
			{
				//TD15346++
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"TD15346: (COUNT ME) Before call to DutyDbCache::instance()->dutyMayChange(): profile dbkey:%lu, current state:%lu, next state:%lu", 
					regionDutyIt->m_profile, subState.getCurrentState(), subState.getNextState(SubsystemState::TR_DIRECT_XFER, *regionDutyIt) );
				//++TD15346

				if( DutyDbCache::instance()->dutyMayChange(regionDutyIt->m_profile, subsystem, subState.getCurrentState(),
						subState.getNextState(SubsystemState::TR_DIRECT_XFER, *regionDutyIt)) )
				{
					allDuplicates = false;
				}

				//TD15346++
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"TD15346: (COUNT ME) After call to DutyDbCache::instance()->dutyMayChange(): profile dbkey:%lu, current state:%lu, next state:%lu", 
					regionDutyIt->m_profile, subState.getCurrentState(), subState.getNextState(SubsystemState::TR_DIRECT_XFER, *regionDutyIt) );
				//++TD15346
			}

			regionDutyIt++;
		}

		if( allDuplicates )
		{
			return;
		}

		for( regionDutyIt = m_regionDuty.begin(); regionDutyIt != m_regionDuty.end(); regionDutyIt++ )
		{
			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) Duty may change, process this subsystem");
			//++TD15346

			SubsystemState::State nextState = subState.getNextState(SubsystemState::TR_DIRECT_XFER, *regionDutyIt);

			DutyPrimitive    dutyToReplace;
			DASType::DutySet dutyNotToAdd;
			DutyContext context;

			DutyContextBuilderFactory::instance()->getDutyContextBuilder().buildXferSubContext(
				context, *regionDutyIt, subsystem, subState.getCurrentState(), nextState );

			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) Before call to EvaluateDutyPolicy::evaluate(): profile dbkey:%lu, current state:%lu, next state:%lu", 
				regionDutyIt->m_profile, subState.getCurrentState(), nextState );
			//++TD15346

			IDutyPolicy::POLICY_RESULT result = EvaluateDutyPolicy::evaluate(
				DutyPolicyFactory::instance()->getDutyPolicy(),
				&IDutyPolicy::evaluateXferSubPolicy,
				context, dutySet, dutyToReplace, dutyNotToAdd );

			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) Result of EvaluateDutyPolicy::evaluate(): result:%lu", 
				result );
			//++TD15346

			DASType::DutySet::const_iterator dutyNotToAddIT;
			for( dutyNotToAddIT = dutyNotToAdd.begin(); dutyNotToAddIT != dutyNotToAdd.end(); dutyNotToAddIT++ )
			{
				//TD15346++
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"TD15346: (COUNT ME) dutyNotToAdd: profile:%lu", 
					dutyNotToAddIT->m_profile);
				std::string temp = "TD15346: session:";
				temp += dutyNotToAddIT->m_session;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
				//++TD15346
			}

			if( dutyToReplace.m_session.empty() )
			{
				//TD15346++
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"TD15346: state.processSubsystemPolicyResult() m_session.empty: profile dbkey:%lu, current state:%lu, next state:%lu, policy result:%lu", 
					regionDutyIt->m_profile, subState.getCurrentState(), nextState, result );
				//++TD15346

				state.processSubsystemPolicyResult( subsystem, subState.getCurrentState(), nextState, *regionDutyIt, result );
			}
			else
			{
				//TD15346++
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"TD15346: state.processSubsystemPolicyResult() NOT m_session.empty: profile dbkey:%lu, current state:%lu, next state:%lu, policy result:%lu", 
					regionDutyIt->m_profile, subState.getCurrentState(), nextState, result );
				//++TD15346

				state.processSubsystemPolicyResult ( subsystem, subState.getCurrentState(), nextState, *regionDutyIt, dutyToReplace, result );
			}

			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) After call to EvaluateDutyPolicy::evaluate(): profile dbkey:%lu, current state:%lu, next state:%lu", 
				regionDutyIt->m_profile, subState.getCurrentState(), nextState );
			//++TD15346

			// no need for post-processing as we are only interested in an add for the new session or a replace
		}

	}

	const DASType::SubsystemSet& TransferableSubsystemQuery::getSubsystems() const
	{
		return( m_subsystems );
	}

	const DASType::RegionSet& TransferableSubsystemQuery::getRegions() const
	{
		return( m_regions );
	}
}




