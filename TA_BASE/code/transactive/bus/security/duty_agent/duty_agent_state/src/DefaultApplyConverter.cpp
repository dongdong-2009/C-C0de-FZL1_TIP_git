/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DefaultApplyConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Implements DefaultApplyConverter.
 */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	DefaultApplyConverter::DefaultApplyConverter( DASType::SubsystemMap& subsToApply ) :
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM),
		m_addCount(0),
		m_remCount(0),
		m_subsToApply(subsToApply)
	{
	}

	void DefaultApplyConverter::setSession(const std::string& session)
	{
	}


	void DefaultApplyConverter::setAction(const std::string& action)
	{
	}

	void DefaultApplyConverter::setRegionCount(unsigned long count)
	{
	}

	void DefaultApplyConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
	}

	void DefaultApplyConverter::setSubsystemCount(unsigned long count)
	{
	}

	void DefaultApplyConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem = subsystem;
		m_addCount  = 0;
		m_remCount  = 0;
	}


	void DefaultApplyConverter::setRegionAddCount( unsigned long count )
	{
	}

	void DefaultApplyConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void DefaultApplyConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DefaultApplyConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DefaultApplyConverter::setSubsystemAddCount( unsigned long count )
	{
		m_addCount = count;
	}

	void DefaultApplyConverter::setSubsystemRemoveCount( unsigned long count )
	{
		m_remCount = count;
	}

	void DefaultApplyConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		// add in 2008-9-23
		m_subAddSession.m_profile = session.m_profile;
		m_subAddSession.m_session = session.m_session;
		// add end
	}

	void DefaultApplyConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		// add in 2008-9-23
		m_subRemoveSession.m_profile = session.m_profile;
		m_subRemoveSession.m_session = session.m_session;
		// add end
	}

	void DefaultApplyConverter::subsystemDeny( const DutyPrimitive& session )
	{
	}

	void DefaultApplyConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void DefaultApplyConverter::subsystemComplete()
	{
		if( ((m_remCount==0) && (m_addCount>0))
			|| (m_remCount == 1 && m_remCount == m_addCount && m_subAddSession == m_subRemoveSession))
			// add in 2008-9-23
			// if m_remove == m_add, means Session lost Control Duty and Gain Control Duty in this
			// Subsystem through Subsystem state change. 
			// No need to send Request and should be applied automatically
			// No need to send Request is implement in DutyRequestConvertor
			// Here we implement automatically apply
		{
			std::pair<DASType::SubsystemMap::iterator,bool> insertResult;

			insertResult = m_subsToApply.insert( DASType::SubsystemMap::value_type(m_region,m_emptySubSet) );

			insertResult.first->second.insert( m_subsystem );
		}
	}

	void DefaultApplyConverter::regionComplete()
	{
	}
}



