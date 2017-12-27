/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/StateUpdateConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Implements StateUpdateConverter.
 */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	StateUpdateConverter::StateUpdateConverter( std::map<DASType::DbKey, Region*>& regionMap ) :
		m_regionMap(regionMap),
		m_currentRegion(regionMap.end()),
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM),
		m_subState(SubsystemState::SS_NORMAL)
	{
	}

	void StateUpdateConverter::setSession(const std::string& session)
	{
	}


	void StateUpdateConverter::setAction(const std::string& action)
	{
	}

	void StateUpdateConverter::setRegionCount(unsigned long count)
	{
	}

	void StateUpdateConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
		m_regAdd.clear();
		m_regRem.clear();

		m_currentRegion = m_regionMap.find(region);
	}

	void StateUpdateConverter::setSubsystemCount(unsigned long count)
	{
	}

	void StateUpdateConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		if( m_currentRegion == m_regionMap.end() )
		{
			return;
		}

		m_subsystem = subsystem;
		m_subAdd.clear();
		m_subRem.clear();

		m_subState = addState;
	}


	void StateUpdateConverter::setRegionAddCount( unsigned long count )
	{
	}

	void StateUpdateConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void StateUpdateConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		if( m_currentRegion == m_regionMap.end() )
		{
			return;
		}

		m_regAdd.insert(session);
	}


	void StateUpdateConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		if( m_currentRegion == m_regionMap.end() )
		{
			return;
		}

		m_regRem.insert(session);
	}


	void StateUpdateConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void StateUpdateConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void StateUpdateConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		if( m_currentRegion == m_regionMap.end() )
		{
			return;
		}

		m_subAdd.insert(session);
	}

	void StateUpdateConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		if( m_currentRegion == m_regionMap.end() )
		{
			return;
		}

		m_subRem.insert(session);
	}

	void StateUpdateConverter::subsystemDeny( const DutyPrimitive& session )
	{
	}

	void StateUpdateConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
		// this session timed out (did not respond to the request) and will be removed.
		subsystemRemove(session);
	}

	void StateUpdateConverter::subsystemComplete()
	{
		if( m_currentRegion == m_regionMap.end() )
		{
			return;
		}

		m_currentRegion->second->stateUpdate( m_subsystem, m_subAdd, m_subRem, m_subState );
	}

	void StateUpdateConverter::regionComplete()
	{
		if( m_currentRegion == m_regionMap.end() )
		{
			return;
		}

		m_currentRegion->second->stateUpdate( m_regAdd, m_regRem );
	}
}



