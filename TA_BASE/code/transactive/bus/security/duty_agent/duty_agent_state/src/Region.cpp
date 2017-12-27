/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/Region.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/29 17:51:30 $
  *  Last modified by:  $Author: liwei.gong $
  *
  * Implements Region.
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

namespace TA_Base_Bus
{
	Region::SubDegradeCommand::SubDegradeCommand( DutyState& state, const DASType::DutySet& eligibleSessions ) :
		m_state(state),
		m_eligibleSessions(eligibleSessions)
	{
	}

	void Region::SubDegradeCommand::execute(Subsystem& subsystem, bool doApply)
	{
		subsystem.degrade( m_state, m_eligibleSessions );

// 		if( doApply )
// 		{
// 			subsystem.apply( m_state );
// 		}
	}

	Region::Region( const DASType::DbKey& key ) :
		m_regionKey(key)//,
		//m_dutyHelper("region", key)
	{
		DASType::SubsystemSet subsystems;
		DASType::SubsystemSet::iterator subIt;

		DutyDbCache::instance()->getAllSubsystemKeys(m_regionKey, subsystems);

		for( subIt = subsystems.begin(); subIt != subsystems.end(); subIt++ )
		{
			Subsystem* newSub = new Subsystem(*subIt);

			m_subsystems.insert( Subsystems::value_type(*subIt,newSub) );
		}
	}

	Region::~Region()
	{
		Subsystems::iterator it;

		for( it = m_subsystems.begin(); it != m_subsystems.end(); it++ )
		{
			delete it->second;
			it->second = NULL;
		}
		m_subsystems.clear();
	}

	void Region::sessionBegin(DutyState& state, const DutyPrimitive& session)
	{
		{	
			ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

			DutyContext context;

		DutyContextBuilderFactory::instance()->getDutyContextBuilder().buildAllocRegionContext(
			context, session, m_regionKey);

			DutyPrimitive    dutyToReplace;
			DASType::DutySet dutyNotToAdd;

			IDutyPolicy::POLICY_RESULT result = EvaluateDutyPolicy::evaluate( 
				DutyPolicyFactory::instance()->getDutyPolicy(),
				&IDutyPolicy::evaluateAllocRegionPolicy,
				context, m_dutyHelper.getDuty(), dutyToReplace, dutyNotToAdd );

			// We are not interested in "dutyNotToAdd" for Regions as there is (currently) no post-processing
			// required because Regions do not change state like subsystems
			processPolicyResult(state, session, dutyToReplace, result ); 

			// Don't allocate duty for subsystems unless duty was granted for the region!
			if( IDutyPolicy::PR_DENY != result )
			{
				RegSubUniProfCommand<Subsystem> stateCmd(&Subsystem::sessionBegin, state, session);
				subsystemExecute( stateCmd, true );
				this->apply(state);
			}	
		}
		// CL19422 ++
#ifdef DEBUG_DUTY_DETAIL
		{
			std::ofstream strOut;
			strOut.open("./DutyAgentSpecial.log", std::ios::app);
			if (strOut.is_open())
			{
				strOut << "\n\n---In Region::sessionBegin()---" << std::endl
					<< "   check session:" << session.m_session.c_str()
					<< "   profile:" << session.m_profile << std::endl;

				strOut << "\n...RegionInfo..." << std::endl;
				print(strOut);

				strOut << "\n...DutyStateInfo..." << std::endl;
				state.print(strOut);

				strOut << "---End Region::sessionBegin()---" << std::endl;
				strOut.close();
			}
		}
#endif //DEBUG_DUTY_DETAIL
		// ++ CL19422
	}

	void Region::sessionEnd(DutyState& state, const DutyPrimitive& session)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		if( !m_dutyHelper.hasDuty(session))
		{
			return;
		}
		
		m_dutyHelper.rem( session );
		state.processRegionPolicyResult( m_regionKey, session, IDutyPolicy::PR_REMOVE );

		RegSubUniProfCommand<Subsystem> stateCmd(&Subsystem::sessionEnd, state, session);
		subsystemExecute( stateCmd, true );
		this->apply(state);

	}

	void Region::transfer(DutyState& state, const DutyPrimitive& session)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);
		// check that this region is potentially able to be transferred to the session.
		// this is required to cater for operator-override.
		if( !DutySessionCache::instance()->isValidRegion(session, m_regionKey) )
		{
			return;
		}

		// check that the operator doesn't already have duty for this region
		// the operator has duty for this region, no further processing
		if( m_dutyHelper.hasDuty(session) )
		{
			return;
		}

		DutyContext context;

		DutyContextBuilderFactory::instance()->getDutyContextBuilder().buildXferRegionContext(context, session);

		DutyPrimitive    dutyToReplace;
		DASType::DutySet dutyNotToAdd;

		IDutyPolicy::POLICY_RESULT result = EvaluateDutyPolicy::evaluate( 
			DutyPolicyFactory::instance()->getDutyPolicy(),
			&IDutyPolicy::evaluateXferRegionPolicy,
			context, m_dutyHelper.getDuty(), dutyToReplace, dutyNotToAdd );

		processPolicyResult(state, session, dutyToReplace, result ); 

		if( IDutyPolicy::PR_DENY != result )
		{
			RegSubUniProfCommand<Subsystem> xferCmd(&Subsystem::indirectTransfer, state, session);
			subsystemExecute( xferCmd, true );
			this->apply(state);
			PeerStateChangeTask::instance()->updateSubject(state);
		}	
	}

	void Region::transfer(DutyState& state, const DutyPrimitive& session, const DASType::SubsystemSet& subsystems, bool apply)
	{
		//here don't change region or location duty, only get subsystem information

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		state.setRegion(m_regionKey);

		// check the session has region duty then attempt the transfer
		if( m_dutyHelper.hasDuty(session))
		{
			RegSubUniProfCommand2<Subsystem> xferCmd(&Subsystem::directTransfer, state, session,apply);
			subsystemExecute( xferCmd, subsystems, false );

			if(apply)
			{
				if(IDutyPolicy::PT_CENTRAL == DutyDbCache::instance()->getProfileType(session.m_profile))
				{
					this->regainLocalProfileDuty(state);
				}
				PeerStateChangeTask::instance()->updateSubject(state);
			}
			
		}
		// CL19422 ++
#ifdef DEBUG_DUTY_DETAIL
		{
			std::ofstream strOut;
			strOut.open("./DutyAgentSpecial.log", std::ios::app);
			if (strOut.is_open())
			{
				strOut << "\n\n---In Region::transfer()-direct, do not apply state---" << std::endl
					<< "   check session:" << session.m_session.c_str()
					<< "   profile:" << session.m_profile << std::endl;
				strOut << "   check Subsystems:" << std::endl;
				if (subsystems.find(DASConst::ALL_SUBSYSTEMS) != subsystems.end())
				{
					strOut << "\t All subsystem contains in!!!" << std::endl;
				}
				else
				{
					for(DASType::SubsystemSet::const_iterator itSub = subsystems.begin();
					itSub != subsystems.end(); ++itSub)
					{
						strOut << "\t Subsystem Key:" << *itSub << std::endl;
					}
				}

				strOut << "\n...RegionInfo..." << std::endl;
				print(strOut);

				//strOut << "\n...DutyStateInfo..." << std::endl;
				//state.print(strOut);

				strOut << "---End Region::transfer()-direct---" << std::endl;
				strOut.close();
			}
		}
#endif //DEBUG_DUTY_DETAIL	
		// ++ CL19422
	}

	void Region::executeQuery(DutyState& state, IDutyQuery& query)
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_mutex);

		query.execute(state, m_regionKey, m_dutyHelper.getDuty() );

		if( query.isValidToCheckSubsystems())
		{
			RegSubQueryCommand<Subsystem> queryCmd(state, query);
			subsystemExecute( queryCmd, query.getSubsystems(), false );
		}
	}

	void Region::apply( DutyState& state )
	{
		m_dutyHelper.apply();
	}

	void Region::apply(DutyState& state, const DASType::SubsystemSet& subsystems)
	{
		//RegSubSimpleCommand<Subsystem> simpleCmd(&Subsystem::apply, state );
		//subsystemExecute( simpleCmd, subsystems, false );
		// CL19422 ++
#ifdef DEBUG_DUTY_DETAIL
		{
			std::ofstream strOut;
			strOut.open("./DutyAgentSpecial.log", std::ios::app);
			if (strOut.is_open())
			{
				strOut << "\n\n---In Region::apply(), current state is true---" << std::endl;
				strOut << " check Subsystems:" << std::endl;
				if (subsystems.find(DASConst::ALL_SUBSYSTEMS) != subsystems.end())
				{
					strOut << "\t All subsystem contains in!!!" << std::endl;
				}
				else
				{
					for(DASType::SubsystemSet::const_iterator itSub = subsystems.begin();
					itSub != subsystems.end(); ++itSub)
					{
						strOut << "\t Subsystem Key:" << *itSub << std::endl;
					}
				}

				strOut << "\n...RegionInfo..." << std::endl;
				print(strOut);

				strOut << "---End Region::apply()---" << std::endl;
				strOut.close();
			}
		}
#endif //DEBUG_DUTY_DETAIL
		// ++ CL19422
	}


	void Region::cancel(DutyState& state, const DASType::SubsystemSet& subsystems)
	{
// 		RegSubSimpleCommand<Subsystem> simpleCmd(&Subsystem::cancel, state);
// 		subsystemExecute( simpleCmd, subsystems, false );
	}



	void Region::degradeSubsystems(DutyState& state, const DASType::SubsystemSet& subsystems )
	{
		//location duty don't change

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		DASType::DutySet eligibleSessions;
		DASType::DutySet::const_iterator sesIt;

		// make a list if eligible sessions
		for( sesIt = m_dutyHelper.getDuty().begin(); sesIt != m_dutyHelper.getDuty().end(); sesIt++ )
		{
			if( IDutyPolicy::PT_LOCAL == DutyDbCache::instance()->getProfileType(sesIt->m_profile) )
			{
				eligibleSessions.insert(*sesIt);
			}
		}

		state.setRegion(m_regionKey);

		// construct the command
		SubDegradeCommand degradeCmd(state, eligibleSessions);

		subsystemExecute(degradeCmd, subsystems, true);
		
		PeerStateChangeTask::instance()->updateSubject(state);

		// CL19422 ++
#ifdef DEBUG_DUTY_DETAIL
		{
			std::ofstream strOut;
			strOut.open("./DutyAgentSpecial.log", std::ios::app);
			if (strOut.is_open())
			{
				strOut << "---In Region::degradeSubsystems() finished, state has changed and applyed---" << std::endl
					<< "check SubsystemSet:" << std::endl;
				if (subsystems.find(DASConst::ALL_SUBSYSTEMS) != subsystems.end())
				{
					strOut << "\t All subsystem contains in!!!" << std::endl;
				}
				else
				{
					for(DASType::SubsystemSet::const_iterator itSub = subsystems.begin();
					itSub != subsystems.end(); ++itSub)
					{
						strOut << "\t Subsystem Key:" << *itSub << std::endl;
					}
				}

				strOut << "\n...RegionInfo..." << std::endl;
				print(strOut);

				//strOut << "\n...DutyStateInfo..." << std::endl;
				//state.print(strOut);
				strOut << "---End Region::degradeSubsystems()---\n" << std::endl;
				strOut.close();
			}
		}
#endif //DEBUG_DUTY_DETAIL
		// ++ CL19422
	}

	void Region::subsystemExecute( IRegSubCommand<Subsystem>& command, bool doApply )
	{
		Subsystems::iterator subIt;

		for( subIt = m_subsystems.begin(); subIt != m_subsystems.end(); subIt++ )
		{
			command.execute(*(subIt->second), doApply);		
		}
	}

	void Region::subsystemExecute( IRegSubCommand<Subsystem>& command, const DASType::SubsystemSet& subsystems, bool doApply )
	{
		DASType::SubsystemSet::const_iterator cmdIt;
		Subsystems::iterator subIt;

		if( (subsystems.find(DASConst::ALL_SUBSYSTEMS)) != subsystems.end() )
		{
			subsystemExecute( command, doApply );
		}
		else
		{
			for( cmdIt = subsystems.begin(); cmdIt != subsystems.end(); cmdIt++ )
			{
				if( (subIt = m_subsystems.find(*cmdIt)) != m_subsystems.end() )
				{
					command.execute(*(subIt->second), doApply);					
				}
			}
		}
	}

	void Region::processPolicyResult( DutyState& state, const DutyPrimitive& dstSession,
		const DutyPrimitive& dutyToReplace, const IDutyPolicy::POLICY_RESULT& result )
	{
		m_dutyHelper.processPolicyResult( dstSession, dutyToReplace, result );
		state.processRegionPolicyResult ( m_regionKey, dstSession, dutyToReplace, result );

		// no need to post-process as regions do not change state like subsystems
	}

	void Region::stateUpdate( const DASType::DutySet& add, const DASType::DutySet& rem )
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
	}

	void Region::stateUpdate( const DASType::DbKey& subsystem, const DASType::DutySet& add,
		const DASType::DutySet& rem, const SubsystemState::State& subState)
	{
		Subsystems::iterator subIt;

		if( (subIt = m_subsystems.find(subsystem)) != m_subsystems.end() )
		{
			subIt->second->stateUpdate( add, rem, subState );
		}
	}

	void Region::regainLocalProfileDuty(DutyState& state)
	{
		//ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		state.setRegion(m_regionKey);
		// CL19422 ++
		// CL19422: region duty is all right, but for subsytem, state change from degrade to normal, local profile info 
		// 					do not contain in Subsystem, they can not regain its duty automaticlly
		// NOTE: degrade mode will delete subsystem session info for local profile.
		//			 so we just regain duty for local profile.
		{
			for(DASType::DutySet::const_iterator itDuty = m_dutyHelper.getDuty().begin(); itDuty != m_dutyHelper.getDuty().end(); ++itDuty)
			{
				if( IDutyPolicy::PT_LOCAL == DutyDbCache::instance()->getProfileType(itDuty->m_profile) )
				{
					RegSubUniProfCommand<Subsystem> regainCmd(&Subsystem::indirectTransfer, state, *itDuty);
					// apply change
					subsystemExecute( regainCmd, true );
				}
			}
		}
		// ++ CL19422
	}
	
// CL19422 ++
	void Region::print(std::ostream& strOut) const
	{
		strOut << "Region Key:" << m_regionKey << std::endl;
		for(DASType::DutySet::const_iterator itDuty = m_dutyHelper.getDuty().begin();
		itDuty != m_dutyHelper.getDuty().end(); ++itDuty)
		{
			strOut << "   session:" << itDuty->m_session.c_str()
				<< "   profile:" << itDuty->m_profile << std::endl;
		}

		//print all subsystem info
		for(Subsystems::const_iterator itSub = m_subsystems.begin(); itSub != m_subsystems.end(); ++itSub)
		{
			itSub->second->print(strOut);
		}
	}
// ++ CL19422
} // TA_Base_Bus
