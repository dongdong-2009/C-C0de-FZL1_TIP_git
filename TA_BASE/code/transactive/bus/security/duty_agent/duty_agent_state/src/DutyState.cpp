/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyState.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Keeps a record of state changes. DutyState is passed into functions that must
  *  change or query state. DutyState is then published and picked up by interested
  *  observers.
  *
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	DutyState::DutyState() :
		m_region(DASConst::NO_REGION),
		m_regionInserted(false)
	{
		m_currentRegion.first = m_dutyMap.end();
	}

	DutyState::DutyState( const DutyState& rhs ) :
		m_initiatingAction(rhs.m_initiatingAction),
		m_initiatingSession(rhs.m_initiatingSession),
		m_accessControlSession(rhs.m_accessControlSession),
		m_dutyMap(rhs.m_dutyMap),
		m_currentRegion(rhs.m_currentRegion),
		m_region(rhs.m_region),
		m_regionInserted(rhs.m_regionInserted)
	{
	}

	DutyState::DutyState( const std::string& initiatingSession, const std::string& initiatingAction ) :
		m_initiatingAction (initiatingAction ),	
		m_initiatingSession(initiatingSession),
		m_accessControlSession(initiatingSession),	
		m_region(DASConst::NO_REGION),
		m_regionInserted(false)
	{
		m_currentRegion.first = m_dutyMap.end();
	}
 
	DutyState::DutyState( const std::string& accessControlSession, const std::string& initiatingSession,
		const std::string& initiatingAction ) :
			m_initiatingAction (initiatingAction ),	
			m_initiatingSession(initiatingSession),
			m_accessControlSession(accessControlSession),
			m_region(DASConst::NO_REGION),
			m_regionInserted(false)
	{
		m_currentRegion.first = m_dutyMap.end();
	}

	void DutyState::set( const DutyAgentTypeCorbaDef::DaDutySequence& state )
	{
		CORBA::ULong regIdx;
		CORBA::ULong subIdx;
		CORBA::ULong dutyIdx;

		for( regIdx=0; regIdx<state.length(); regIdx++ )
		{
			m_currentRegion = m_dutyMap.insert( DutyMap::value_type(state[regIdx].region, m_copyMeDuty) );
			
			// region add
			for( dutyIdx=0; dutyIdx<state[regIdx].regDuty.add.length(); dutyIdx++ )
			{
				DutyPrimitive dp( state[regIdx].regDuty.add[dutyIdx].session.in(),
					state[regIdx].regDuty.add[dutyIdx].profile );

				m_currentRegion.first->second.m_region.m_add.insert( dp );
			}

			// region remove
			for( dutyIdx=0; dutyIdx<state[regIdx].regDuty.remove.length(); dutyIdx++ )
			{
				DutyPrimitive dp( state[regIdx].regDuty.remove[dutyIdx].session.in(),
					state[regIdx].regDuty.remove[dutyIdx].profile );

				m_currentRegion.first->second.m_region.m_remove.insert( dp );
			}

			// subsystems
			for( subIdx=0; subIdx<state[regIdx].subDuty.length(); subIdx++ )
			{
				SubDuty newSubDuty;

				convertSubState( state[regIdx].subDuty[subIdx].addState, newSubDuty.m_addState );
				convertSubState( state[regIdx].subDuty[subIdx].remState, newSubDuty.m_remState );

				// add
				for( dutyIdx=0; dutyIdx<state[regIdx].subDuty[subIdx].add.length(); dutyIdx++ )
				{
					DutyPrimitive dp( state[regIdx].subDuty[subIdx].add[dutyIdx].session.in(),
						state[regIdx].subDuty[subIdx].add[dutyIdx].profile );

					newSubDuty.m_add.insert(dp);
				}

				// remove
				for( dutyIdx=0; dutyIdx<state[regIdx].subDuty[subIdx].remove.length(); dutyIdx++ )
				{
					DutyPrimitive dp( state[regIdx].subDuty[subIdx].remove[dutyIdx].session.in(),
						state[regIdx].subDuty[subIdx].remove[dutyIdx].profile );

					newSubDuty.m_remove.insert(dp);
				}

				// denied by
				if( ::strlen(state[regIdx].subDuty[subIdx].deniedBy.session.in()) > 0 )
				{
					DutyPrimitive dp( state[regIdx].subDuty[subIdx].deniedBy.session.in(),
						state[regIdx].subDuty[subIdx].deniedBy.profile );

					newSubDuty.m_deniedBy = dp;
				}

				// timed out
				if( ::strlen(state[regIdx].subDuty[subIdx].timedOut.session.in()) > 0 )
				{
					DutyPrimitive dp( state[regIdx].subDuty[subIdx].timedOut.session.in(),
						state[regIdx].subDuty[subIdx].timedOut.profile );

					newSubDuty.m_timedOut = dp;
				}

				m_currentRegion.first->second.m_subsystem.insert(
					SubDutyMap::value_type(state[regIdx].subDuty[subIdx].subsystem,newSubDuty) );
			}
		}
	}

	void DutyState::convert( IDutyStateConverter& converter ) const
	{
		DutyMap::const_iterator dmIt;
		SubDutyMap::const_iterator subDmIt;
		DASType::DutySet::const_iterator dsIt;

		const char* pName = typeid(converter).name();
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DutyState::convert  begin call convert for %s", pName);

		converter.setSession( m_initiatingSession );
		converter.setAction ( m_initiatingAction  );

		converter.setRegionCount( m_dutyMap.size() );

		for( dmIt = m_dutyMap.begin(); dmIt != m_dutyMap.end(); dmIt++ )
		{
			converter.setRegion( dmIt->first );
			
			converter.setRegionAddCount( dmIt->second.m_region.m_add.size() );
			converter.setRegionRemoveCount( dmIt->second.m_region.m_remove.size() );

			for( dsIt = dmIt->second.m_region.m_add.begin(); dsIt != dmIt->second.m_region.m_add.end(); dsIt++ )
			{
				converter.regionAdd( *dsIt );
			}

			for( dsIt = dmIt->second.m_region.m_remove.begin(); dsIt != dmIt->second.m_region.m_remove.end(); dsIt++ )
			{
				converter.regionRemove( *dsIt );
			}

			converter.setSubsystemCount( dmIt->second.m_subsystem.size() );

			for( subDmIt = dmIt->second.m_subsystem.begin(); subDmIt != dmIt->second.m_subsystem.end(); subDmIt++ )
			{
				converter.setSubsystem( subDmIt->first, subDmIt->second.m_addState, subDmIt->second.m_remState );

				converter.setSubsystemAddCount( subDmIt->second.m_add.size() );
				converter.setSubsystemRemoveCount( subDmIt->second.m_remove.size() );

				for( dsIt = subDmIt->second.m_add.begin(); dsIt != subDmIt->second.m_add.end(); dsIt++ )
				{
					//TD15346++
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						"TD15346: Inside convert() ADD: SK:%lu, AS:%lu, RS:%lu, PK:%lu",
						subDmIt->first, subDmIt->second.m_addState, subDmIt->second.m_remState, (*dsIt).m_profile);
					std::string temp = "TD15346: session: ";
					temp += (*dsIt).m_session;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
					//++TD15346
					converter.subsystemAdd( *dsIt );
				}

				for( dsIt = subDmIt->second.m_remove.begin(); dsIt != subDmIt->second.m_remove.end(); dsIt++ )
				{
					//TD15346++
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						"TD15346: Inside convert() REMOVE: SK:%lu, AS:%lu, RS:%lu, PK:%lu",
						subDmIt->first, subDmIt->second.m_addState, subDmIt->second.m_remState, (*dsIt).m_profile);
					std::string temp = "TD15346: session: ";
					temp += (*dsIt).m_session;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
					//++TD15346
					converter.subsystemRemove( *dsIt );
				}

				if( !subDmIt->second.m_deniedBy.m_session.empty() )
				{
					//TD15346++
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						"TD15346: Inside convert() DENY: SK:%lu, AS:%lu, RS:%lu, PK:%lu",
						subDmIt->first, subDmIt->second.m_addState, subDmIt->second.m_remState, (subDmIt->second.m_deniedBy).m_profile);
					std::string temp = "TD15346: session: ";
					temp += subDmIt->second.m_deniedBy.m_session;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
					//++TD15346
					converter.subsystemDeny( subDmIt->second.m_deniedBy );
				}

				if( !subDmIt->second.m_timedOut.m_session.empty() )
				{
					//TD15346++
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						"TD15346: Inside convert() TIMEOUT: SK:%lu, AS:%lu, RS:%lu, PK:%lu",
						subDmIt->first, subDmIt->second.m_addState, subDmIt->second.m_remState, (subDmIt->second.m_timedOut).m_profile);
					std::string temp = "TD15346: session: ";
					temp += subDmIt->second.m_timedOut.m_session;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
					//++TD15346
					converter.subsystemTimeout( subDmIt->second.m_timedOut );
				}

				converter.subsystemComplete();
			}
			converter.regionComplete();
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DutyState::convert end call convert for %s", pName);
	}

	void DutyState::insert( DutyState& state )
	{
		DutyMap::const_iterator dmIt;

		for( dmIt = state.m_dutyMap.begin(); dmIt != state.m_dutyMap.end(); dmIt++ )
		{
			m_dutyMap.insert( *dmIt );
		}
	}

	void DutyState::setRegion( const DASType::DbKey& region )
	{
		m_region = region;
		m_regionInserted = false;
	}

	void DutyState::insertCurrentRegion()
	{
		if( true == m_regionInserted )
		{
			return;
		}

		m_currentRegion = m_dutyMap.insert( DutyMap::value_type(m_region, m_copyMeDuty) );
		m_regionInserted = true;
	}

    void DutyState::setSubsystem( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
        const SubsystemState::State& nextState )
    {
        // Need to make sure that the region is inserted first.
        insertCurrentRegion();

        // Make sure the subsystem is inserted with the correct state.
        m_copyMeSubDuty.m_addState = nextState;
        m_copyMeSubDuty.m_remState = currState;
        m_currentRegion.first->second.m_subsystem.insert(
            SubDutyMap::value_type( subsystem, m_copyMeSubDuty ) );
    }

	void DutyState::remove( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
		const SubsystemState::State& nextState, const DutyPrimitive& session )
	{
		insertCurrentRegion();

		std::pair<SubDutyMap::iterator,bool> insertResult;
		
		m_copyMeSubDuty.m_addState = nextState;
		m_copyMeSubDuty.m_remState = currState;

		insertResult = m_currentRegion.first->second.m_subsystem.insert(
			SubDutyMap::value_type(subsystem,m_copyMeSubDuty));

		insertResult.first->second.m_remove.insert(session);
	}

	void DutyState::remove( const DASType::DbKey& region, const DutyPrimitive& session )
	{
		setRegion( region );
		insertCurrentRegion();

		m_currentRegion.first->second.m_region.m_remove.insert(session);
	}

	void DutyState::processRegionPolicyResult( const DASType::DbKey& region, const DutyPrimitive& dstSession,
			const DutyPrimitive& srcSession, const IDutyPolicy::POLICY_RESULT& result )
	{
		switch(result)
		{
			case IDutyPolicy::PR_ADD: // no break
			case IDutyPolicy::PR_ADDONLY:
			{
				add( region, dstSession );
			}
			break;

			case IDutyPolicy::PR_REPLACE: // no break
			case IDutyPolicy::PR_REPONLY:
			{
				replace( region, dstSession, srcSession );
			}
			break;
			
			case IDutyPolicy::PR_REMOVE:
			{
				remove( region, dstSession );
			}
			break;

			case IDutyPolicy::PR_DENY:
			{
				return;
			}
			break;

			default:
			{
				std::ostringstream msg;

				msg << "Invalid policy result: " << result;
				TA_ASSERT(false, msg.str().c_str() );
			}
			break;
		}	
	}

	void DutyState::add( const DASType::DbKey& region, const DutyPrimitive& dstSession )
	{
		setRegion(region);
		insertCurrentRegion();

		m_currentRegion.first->second.m_region.m_add.insert(dstSession);
	}

	void DutyState::replace( const DASType::DbKey& region, const DutyPrimitive& dstSession,
			const DutyPrimitive& srcSession )
	{
			setRegion(region);
			insertCurrentRegion();

			m_currentRegion.first->second.m_region.m_add.insert(dstSession);
			m_currentRegion.first->second.m_region.m_remove.insert(srcSession);
	}

	void DutyState::processRegionPolicyResult( const DASType::DbKey& region, const DutyPrimitive& dstSession,
			const IDutyPolicy::POLICY_RESULT& result )
	{
		processRegionPolicyResult( region, dstSession, m_emptySession, result );
	}


	void DutyState::processSubsystemPolicyResult( const DASType::DbKey& subsystem,
		const SubsystemState::State& currState, const SubsystemState::State& nextState,
		const DutyPrimitive& dstSession, const DutyPrimitive& srcSession, const IDutyPolicy::POLICY_RESULT& result )
	{
		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: Inside processSubsystemPolicyResult(): SK:%lu, CS:%lu, NS:%lu, SP:%lu, DP:%lu", 
			subsystem, currState, nextState, srcSession.m_profile, dstSession.m_profile );
		std::string temp = "TD15346: SS: ";
		temp += srcSession.m_session;
		temp += ", DS: ";
		temp += dstSession.m_session;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
		//++TD15346

		switch(result)
		{
			case IDutyPolicy::PR_ADD: // no break
			case IDutyPolicy::PR_ADDONLY:
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "TD15346: Inside processSubsystemPolicyResult() ADD"); //TD15346
				add( subsystem, currState, nextState, dstSession );
			}
			break;

			case IDutyPolicy::PR_REPLACE: // no break
			case IDutyPolicy::PR_REPONLY:
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "TD15346: Inside processSubsystemPolicyResult() REPLACE"); //TD15346
				replace( subsystem, currState, nextState, dstSession, srcSession );
			}
			break;
			
			case IDutyPolicy::PR_REMOVE:
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "TD15346: Inside processSubsystemPolicyResult() REMOVE"); //TD15346
				remove( subsystem, currState, nextState, dstSession );
			}
			break;

			case IDutyPolicy::PR_DENY:
			{
				return;
			}
			break;

			default:
			{
				std::ostringstream msg;

				msg << "Invalid policy result: " << result;
				TA_ASSERT(false, msg.str().c_str() );
			}
			break;
		}
	}

	void DutyState::processSubsystemPolicyResult( const DASType::DbKey& subsystem,
		const SubsystemState::State& currState, const SubsystemState::State& nextState,
		const DutyPrimitive& dstSession, const IDutyPolicy::POLICY_RESULT& result )
	{
		processSubsystemPolicyResult( subsystem, currState, nextState, dstSession, m_emptySession, result );
	}

	void DutyState::processDeny( const DASType::DbKey& region, const DASType::DbKey& subsystem )
	{
		setRegion(region);
		insertCurrentRegion();

		SubDutyMap::iterator subDutyIt;
		DASType::DutySet     dutyIt;

		// find the subsystem
		if( (subDutyIt=m_currentRegion.first->second.m_subsystem.find(subsystem)) != 
			m_currentRegion.first->second.m_subsystem.end() )
		{
			subDutyIt->second.m_deniedBy = *(subDutyIt->second.m_remove.begin());
			subDutyIt->second.m_remove.clear();
			subDutyIt->second.m_add.clear();
			// add at 2008-9-24, subsystem state should not change
			subDutyIt->second.m_addState = subDutyIt->second.m_remState;
		}
	}

	void DutyState::processTimeout( const DASType::DbKey& region, const DASType::DbKey& subsystem )
	{
		setRegion(region);
		insertCurrentRegion();
		SubDutyMap::iterator subDutyIt;

		DASType::DutySet     dutyIt;

		// find the subsystem
		if( (subDutyIt=m_currentRegion.first->second.m_subsystem.find(subsystem)) != 
			m_currentRegion.first->second.m_subsystem.end() )
		{
			subDutyIt->second.m_timedOut = *(subDutyIt->second.m_remove.begin());
			subDutyIt->second.m_remove.clear();
		}
	}

	void DutyState::add( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
		const SubsystemState::State& nextState, const DutyPrimitive& dstSession )
	{
		insertCurrentRegion();

		std::pair<SubDutyMap::iterator,bool> insertResult;
		
		m_copyMeSubDuty.m_addState = nextState;
		m_copyMeSubDuty.m_remState = currState;

		insertResult = m_currentRegion.first->second.m_subsystem.insert(
			SubDutyMap::value_type(subsystem,m_copyMeSubDuty));

		insertResult.first->second.m_add.insert(dstSession);
	}

	void DutyState::replace( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
		const SubsystemState::State& nextState, const DutyPrimitive& dstSession, const DutyPrimitive& srcSession )
	{
		insertCurrentRegion();

		std::pair<SubDutyMap::iterator,bool> insertResult;
		
		m_copyMeSubDuty.m_addState = nextState;
		m_copyMeSubDuty.m_remState = currState;

		insertResult = m_currentRegion.first->second.m_subsystem.insert(
			SubDutyMap::value_type(subsystem,m_copyMeSubDuty));

		insertResult.first->second.m_add.insert   (dstSession);
		insertResult.first->second.m_remove.insert(srcSession);
	}

	void DutyState::convertSubState( unsigned long srcState, SubsystemState::State& dstState )
	{
		switch(srcState)
		{
		case SubsystemState::SS_NORMAL:
			dstState = SubsystemState::SS_NORMAL;
			break;

		case SubsystemState::SS_DELEGATED:
			dstState = SubsystemState::SS_DELEGATED;
			break;

		case SubsystemState::SS_DEGRADED:
			dstState = SubsystemState::SS_DEGRADED;
			break;

		default:
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Receieved bad subsystem state (%lu), resetting to SS_NORMAL", srcState );
			dstState = SubsystemState::SS_NORMAL;
			break;
		}
	}

// CL19422++
	void DutyState::print(std::ostream& strOut) const
	{
		strOut << "DutyState.m_dutyMap:" << std::endl;
		strOut << "   InitSession:" << m_initiatingSession.c_str() << std::endl
			<< "   InitAction:" << m_initiatingAction.c_str() << std::endl
			<< "   AccessControlSession:" << m_accessControlSession.c_str() << std::endl;

		for(DutyState::DutyMap::const_iterator itMap = m_dutyMap.begin(); itMap != m_dutyMap.end(); ++itMap)
		{
			strOut << "m_dutyMap LocationKey:" << itMap->first << std::endl;
			strOut << "   Duty.m_region:" << std::endl;
			strOut << "      m_add:" << std::endl;
			for(DASType::DutySet::const_iterator itAdd = itMap->second.m_region.m_add.begin(); itAdd != itMap->second.m_region.m_add.end(); ++itAdd)
			{
				strOut << "         " << itAdd->m_session.c_str() << "(" << itAdd->m_profile << ")" << std::endl;
			}
			strOut << "      m_remove:" << std::endl;
			for(DASType::DutySet::const_iterator itRem = itMap->second.m_region.m_remove.begin(); itRem != itMap->second.m_region.m_remove.end(); ++itRem)
			{
				strOut << "         " << itRem->m_session.c_str() << "(" << itRem->m_profile << ")" << std::endl;
			}
			strOut << "   ----------" << std::endl;
			strOut << "   Duty.m_subsystem:" << std::endl;
			for(DutyState::SubDutyMap::const_iterator itSub = itMap->second.m_subsystem.begin(); itSub != itMap->second.m_subsystem.end(); ++itSub)
			{
				strOut << "      subKey:" << itSub->first 
					<< ", addState:"<< itSub->second.m_addState  
					<< ", remState:"<< itSub->second.m_remState  
					<< ", deniedBy:"<< itSub->second.m_deniedBy.m_session << "(" << itSub->second.m_deniedBy.m_profile << ")"
					<< ", timedOut:"<< itSub->second.m_timedOut.m_session << "(" << itSub->second.m_timedOut.m_profile << ")"
					<< std::endl
					<< "         m_add:" << std::endl;
				for(DASType::DutySet::const_iterator itAdd = itSub->second.m_add.begin(); itAdd != itSub->second.m_add.end(); ++itAdd)
				{
					strOut << "            " << itAdd->m_session.c_str() << "(" << itAdd->m_profile << ")" << std::endl;
				}
				strOut << "         m_remove:" << std::endl;
				for(DASType::DutySet::const_iterator itRem = itSub->second.m_remove.begin(); itRem != itSub->second.m_remove.end(); ++itRem)
				{
					strOut << "            " << itRem->m_session.c_str() << "(" << itRem->m_profile << ")" << std::endl;
				}
				strOut << std::endl;
			}
			strOut << "   ----------" << std::endl;					
		}
		strOut << "----------------" << std::endl;
	}


	// yanrong++: CL-19418
	std::string getDutyPrimitiveString(const DutyPrimitive& duty)
	{
		std::string info = "[Nil]";
		if(!duty.m_session.empty())
		{
			info = "[" + duty.m_session + "/" + DutyDbCache::instance()->getProfileName(duty.m_profile) + "]";
		}

		return info;
	}
	void DutyState::printEx(std::ostream& strOut) const
	{
		strOut << "------ DutyState.m_dutyMap: ------" << std::endl;
		strOut << "   InitSession:" << m_initiatingSession.c_str() << std::endl
			<< "   InitAction:" << m_initiatingAction.c_str() << std::endl
			<< "   AccessControlSession:" << m_accessControlSession.c_str() << std::endl;

		for(DutyState::DutyMap::const_iterator itMap = m_dutyMap.begin(); itMap != m_dutyMap.end(); ++itMap)
		{
			strOut << "m_dutyMap Location: " << DutyDbCache::instance()->getRegionName(itMap->first) << std::endl;
			strOut << "   Duty.m_region:" << std::endl;
			strOut << "      m_add:" << std::endl;
			for(DASType::DutySet::const_iterator itAdd = itMap->second.m_region.m_add.begin(); itAdd != itMap->second.m_region.m_add.end(); ++itAdd)
			{
				strOut << "         " << getDutyPrimitiveString((*itAdd)).c_str() << std::endl;
			}
			strOut << "      m_remove:" << std::endl;
			for(DASType::DutySet::const_iterator itRem = itMap->second.m_region.m_remove.begin(); itRem != itMap->second.m_region.m_remove.end(); ++itRem)
			{
				strOut << "         " << getDutyPrimitiveString((*itRem)).c_str() << std::endl;
			}
			strOut << "   ----------" << std::endl;
			strOut << "   Duty.m_subsystem:" << std::endl;
			for(DutyState::SubDutyMap::const_iterator itSub = itMap->second.m_subsystem.begin(); itSub != itMap->second.m_subsystem.end(); ++itSub)
			{
				strOut << "      subsystem: [" << DutyDbCache::instance()->getSubsystemName(itSub->first) << "]" << std::endl
					<< "         addState:"<< itSub->second.m_addState  
					<< ", remState:"<< itSub->second.m_remState  
					<< ", deniedBy:"<< getDutyPrimitiveString(itSub->second.m_deniedBy).c_str()
					<< ", timedOut:"<< getDutyPrimitiveString(itSub->second.m_timedOut).c_str()
					<< std::endl
					<< "         m_add:" << std::endl;
				for(DASType::DutySet::const_iterator itAdd = itSub->second.m_add.begin(); itAdd != itSub->second.m_add.end(); ++itAdd)
				{
					strOut << "            " << getDutyPrimitiveString((*itAdd)).c_str() << std::endl;
				}
				strOut << "         m_remove:" << std::endl;
				for(DASType::DutySet::const_iterator itRem = itSub->second.m_remove.begin(); itRem != itSub->second.m_remove.end(); ++itRem)
				{
					strOut << "            " << getDutyPrimitiveString((*itRem)).c_str() << std::endl;
				}
				strOut << std::endl;
			}
			strOut << "   ----------" << std::endl;					
		}
		strOut << "----------------" << std::endl;
	}
	// ++yanrong

//	TA_Base_Bus::DASType::SubsystemSet DutyState::getStateChangedSubsystem(DASType::DbKey regionKey)
//	{
//		TA_Base_Bus::DASType::SubsystemSet subset;
//		return subset;
//	}
// ++CL19422
} // TA_Base_Bus
