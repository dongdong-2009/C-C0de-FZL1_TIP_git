/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/Subsystem.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  Subsystem.cpp
//  Implementation of the Class Subsystem
//  Created on:      07-Jul-2004 01:13:30 PM
//  Original author: Gregg Kirkpatrick
///////////////////////////////////////////////////////////

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	Subsystem::Subsystem( const DASType::DbKey& key ) :
		m_subsystemKey(key)
	{
	}

	void Subsystem::sessionBegin(DutyState& state, const DutyPrimitive& session)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		m_subState.setNextState(SubsystemState::TR_ALLOC, session);

		if( isDuplicate(session) )
		{
			return;
		}

		DutyContext context;
		
		// build the context
		DutyContextBuilderFactory::instance()->getDutyContextBuilder().buildAllocSubContext(
			context, session, m_subsystemKey, m_subState.getCurrentState(), m_subState.getNextState() );

		m_dutyNotToAdd.insert(session);

		IDutyPolicy::POLICY_RESULT result(IDutyPolicy::PR_DENY);

		// apply the policy
		result = EvaluateDutyPolicy::evaluate( DutyPolicyFactory::instance()->getDutyPolicy(),
			&IDutyPolicy::evaluateAllocSubPolicy,
			context, m_dutyHelper.getDuty(), m_dutyToReplace, m_dutyNotToAdd );

		// process the result
		processPolicyResult( state, session, result );
		this->apply(state);
	}

	void Subsystem::sessionEnd(DutyState& state, const DutyPrimitive& session)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);
		if( m_dutyHelper.getDuty().find(session) != m_dutyHelper.getDuty().end() )
		{
			m_dutyHelper.rem( session );
			state.processSubsystemPolicyResult( m_subsystemKey, m_subState.getCurrentState(),
				m_subState.getNextState(), session, IDutyPolicy::PR_REMOVE );
		}
		this->apply(state);
	}

	void Subsystem::directTransfer(DutyState& state, const DutyPrimitive& session,bool apply)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		m_subState.setNextState(SubsystemState::TR_DIRECT_XFER, session);

		privateDirectTransfer( state, session );
		if(apply)
		{
			this->apply(state);
		}
		else
		{
			this->cancel(state);
		}
	}

	void Subsystem::privateDirectTransfer(DutyState& state, const DutyPrimitive& session)
	{
		if( isDuplicate(session) )
		{
			return;
		}

		DutyContext context;
		
		// build the context
		DutyContextBuilderFactory::instance()->getDutyContextBuilder().buildXferSubContext(
			context, session, m_subsystemKey, m_subState.getCurrentState(), m_subState.getNextState() );

		m_dutyNotToAdd.insert(session);

		IDutyPolicy::POLICY_RESULT result(IDutyPolicy::PR_DENY);

		// apply the policy
		result = EvaluateDutyPolicy::evaluate( DutyPolicyFactory::instance()->getDutyPolicy(),
			&IDutyPolicy::evaluateXferSubPolicy, context,
			m_dutyHelper.getDuty(), m_dutyToReplace, m_dutyNotToAdd );

		// process the result
		processPolicyResult( state, session, result );
	}

	void Subsystem::indirectTransfer(DutyState& state, const DutyPrimitive& session)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		m_subState.setNextState(SubsystemState::TR_INDIRECT_XFER, session);

		if( isDuplicate(session) )
		{
			return;
		}

		DutyContext context;
		
		// build the context
		DutyContextBuilderFactory::instance()->getDutyContextBuilder().buildXferSubViaRegionContext(
			context, session, m_subsystemKey, m_subState.getCurrentState(), m_subState.getNextState() );

		m_dutyNotToAdd.insert(session);

		IDutyPolicy::POLICY_RESULT result(IDutyPolicy::PR_DENY);

		// apply the policy 
		result = EvaluateDutyPolicy::evaluate( DutyPolicyFactory::instance()->getDutyPolicy(),
			&IDutyPolicy::evaluateXferSubViaRegionPolicy,
			context, m_dutyHelper.getDuty(), m_dutyToReplace, m_dutyNotToAdd );

		// process the result
		processPolicyResult( state, session, result );

		this->apply(state);
	}

	void Subsystem::degrade(DutyState& state, const DASType::DutySet& eligibleSessions)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		m_subState.setNextState(SubsystemState::TR_DEGRADE );

		DASType::DutySet::const_iterator it;

		for( it = eligibleSessions.begin(); it != eligibleSessions.end(); it++ )
		{
			privateDirectTransfer( state, *it );
		}
		this->apply(state);
	}

	void Subsystem::executeQuery(DutyState& state, IDutyQuery& query) const
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_mutex);
		query.execute( state, m_subsystemKey, m_dutyHelper.getDuty(), m_subState );
		
	}

	void Subsystem::cancel( DutyState& state )
	{
		//state.subsystemCancel( m_subsystemKey );

		m_dutyNotToAdd.clear();
		m_dutyToReplace.m_session = "";
		m_dutyToReplace.m_profile = ULONG_MAX;

		m_dutyHelper.cancel();
		// modify at: 2008-9-24, cancel Subsystem state change
		m_subState.cancel();
	}


	void Subsystem::apply( DutyState& state )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Applying duty changes to subsystem %lu", m_subsystemKey );

		// post-process
		if( m_subState.getCurrentState() != m_subState.getNextState() )
		{
            // TD12082: We want to add subsystem state changes to the duty state so that the required
            //          messages can be sent.
            state.setSubsystem( m_subsystemKey, m_subState.getCurrentState(), m_subState.getNextState() );

			DASType::DutySet::const_iterator it;
			for( it=m_dutyHelper.getDuty().begin(); it!=m_dutyHelper.getDuty().end(); it++ )
			{
				if( DutyDbCache::instance()->dutyMayChange( it->m_profile, m_subsystemKey,
						m_subState.getCurrentState(), m_subState.getNextState() ) )
				{
					// removals
					if( !(m_dutyToReplace == *it) )
					{
						m_dutyHelper.rem(*it);
						state.processSubsystemPolicyResult( m_subsystemKey, m_subState.getCurrentState(),
							m_subState.getNextState(), *it, IDutyPolicy::PR_REMOVE );
					}

					// additions
					if( m_dutyNotToAdd.find(*it) == m_dutyNotToAdd.end() )
					{
                        // TD10899: If the subsystem changes state it may be possible for a given session
                        //          to no longer have a configured action group for the subsystem. If this
                        //          is the case it should not be added back.
                        if ( DASConst::NO_ACTION_GROUP != DutyDbCache::instance()->getRights( it->m_profile,
                             m_subsystemKey, m_subState.getNextState() ) )
                        {
						    m_dutyHelper.add(*it);
						    state.processSubsystemPolicyResult( m_subsystemKey, m_subState.getCurrentState(),
							    m_subState.getNextState(), *it, IDutyPolicy::PR_ADD );
                        }
					}
				}
			}
		}

		m_dutyNotToAdd.clear();
		m_dutyToReplace.m_session = "";
		m_dutyToReplace.m_profile = ULONG_MAX;

		// The transition MUST take place if the system is degraded regardless of whether a session
		// was found that could take duty for the subsystem in the degraded state.
		if( (m_dutyHelper.apply()) || (SubsystemState::SS_DEGRADED == m_subState.getNextState()) )
		{
			m_subState.transition();
		}
	}

	void Subsystem::processPolicyResult( DutyState& state, const DutyPrimitive& dstSession,
		const IDutyPolicy::POLICY_RESULT& result )
	{
		m_dutyHelper.processPolicyResult( dstSession, m_dutyToReplace, result );
		state.processSubsystemPolicyResult ( m_subsystemKey, m_subState.getCurrentState(),
			m_subState.getNextState(), dstSession, m_dutyToReplace, result );

	}

	bool Subsystem::isDuplicate(const DutyPrimitive& session) const
	{
		// IF the the duty this session may have before and after the transition is not going to change
		// AND the session is already in the duty list THEN it is a duplicate!

		if( m_dutyHelper.hasDuty(session) )
		{
			if( !DutyDbCache::instance()->dutyMayChange(session.m_profile, m_subsystemKey,
				m_subState.getCurrentState(), m_subState.getNextState()) )
			{
				return true;
			}
		}

		return false;
	}

	void Subsystem::stateUpdate( const DASType::DutySet& add, const DASType::DutySet& rem,
			const SubsystemState::State& subState )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		DASType::DutySet::const_iterator it;

		for( it=add.begin(); it!=add.end(); it++ )
		{
			m_dutyHelper.add(*it);
		}

		for( it=rem.begin(); it!=rem.end(); it++ )
		{
			m_dutyHelper.rem(*it);
		}

		m_dutyHelper.apply();

		m_subState.set( subState );
	}
// CL19422 ++
//#ifdef DEBUG_DUTY_DETAIL
	void Subsystem::print(std::ostream& strOut) const
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_mutex);
		strOut << "SubsystemKey:" << m_subsystemKey
			<< "   curState:" <<m_subState.getCurrentState()
			<< "   nextState:" << m_subState.getNextState() << std::endl;
		for(DASType::DutySet::const_iterator itDuty = m_dutyHelper.getDuty().begin();
		itDuty != m_dutyHelper.getDuty().end(); ++itDuty)
		{
			strOut << "   session:" << itDuty->m_session.c_str()
				<< "   profile:" << itDuty->m_profile << std::endl;
		}
	}
//#endif //DEBUG_DUTY_DETAIL
// ++ CL19422


} //TA_Base_Bus
