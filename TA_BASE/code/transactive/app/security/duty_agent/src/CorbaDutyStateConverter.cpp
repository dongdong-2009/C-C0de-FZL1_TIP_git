/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/CorbaDutyStateConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements CorbaDutyStateConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/CorbaDutyStateConverter.h"

namespace TA_Base_App
{
	CorbaDutyStateConverter::CorbaDutyStateConverter() :
		m_corbaDutyState(new TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence),
		m_regIdx(0),
		m_subIdx(0),
		m_regAddIdx(0),
		m_regRemIdx(0),
		m_subAddIdx(0),
		m_subRemIdx(0),
		m_firstRegion(true),
		m_firstSubsystem(true)
		
	{
		m_corbaDutyState->length(0);
	}

	TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence* CorbaDutyStateConverter::getCorbaDutyState()
	{
		return( m_corbaDutyState._retn() );
	}

	void CorbaDutyStateConverter::setSession(const std::string& session)
	{
	}


	void CorbaDutyStateConverter::setAction(const std::string& action)
	{
	}

	void CorbaDutyStateConverter::setRegionCount(unsigned long count)
	{
		m_corbaDutyState->length(count);
	}

	void CorbaDutyStateConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		if( !m_firstRegion )
		{
			m_regIdx++;
		}
		else
		{
			m_firstRegion = false;
		}

		m_corbaDutyState[m_regIdx].region = region;

		m_firstSubsystem = true;
		m_subIdx = 0;
	}

	void CorbaDutyStateConverter::setSubsystemCount(unsigned long count)
	{
		m_corbaDutyState[m_regIdx].subDuty.length(count);
	}

	void CorbaDutyStateConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		if( !m_firstSubsystem )
		{
			m_subIdx++;
		}
		else
		{
			m_firstSubsystem = false;
		}

		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].subsystem = subsystem;
		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].addState  = addState;
		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].remState  = remState;
	}


	void CorbaDutyStateConverter::setRegionAddCount( unsigned long count )
	{
		m_corbaDutyState[m_regIdx].regDuty.add.length(count);
		m_regAddIdx = 0;
	}

	void CorbaDutyStateConverter::setRegionRemoveCount( unsigned long count )
	{
		m_corbaDutyState[m_regIdx].regDuty.remove.length(count);
		m_regRemIdx = 0;
	}

	void CorbaDutyStateConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_corbaDutyState[m_regIdx].regDuty.add[m_regAddIdx].session = 
			CORBA::string_dup(session.m_session.c_str());

		m_corbaDutyState[m_regIdx].regDuty.add[m_regAddIdx].profile = session.m_profile;

		m_regAddIdx++;
	}


	void CorbaDutyStateConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_corbaDutyState[m_regIdx].regDuty.remove[m_regRemIdx].session = 
			CORBA::string_dup(session.m_session.c_str());

		m_corbaDutyState[m_regIdx].regDuty.remove[m_regRemIdx].profile = session.m_profile;

		m_regRemIdx++;
	}


	void CorbaDutyStateConverter::setSubsystemAddCount( unsigned long count )
	{
		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].add.length(count);
		m_subAddIdx = 0;
	}

	void CorbaDutyStateConverter::setSubsystemRemoveCount( unsigned long count )
	{
		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].remove.length(count);
		m_subRemIdx = 0;
	}

	void CorbaDutyStateConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].add[m_subAddIdx].session = 
			CORBA::string_dup(session.m_session.c_str());

		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].add[m_subAddIdx].profile = session.m_profile;

		m_subAddIdx++;
	}


	void CorbaDutyStateConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].remove[m_subRemIdx].session = 
			CORBA::string_dup(session.m_session.c_str());

		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].remove[m_subRemIdx].profile = session.m_profile;

		m_subRemIdx++;
	}


	void CorbaDutyStateConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].deniedBy.session = 
			CORBA::string_dup(session.m_session.c_str());

		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].deniedBy.profile = session.m_profile;
	}


	void CorbaDutyStateConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].timedOut.session = 
			CORBA::string_dup(session.m_session.c_str());

		m_corbaDutyState[m_regIdx].subDuty[m_subIdx].timedOut.profile = session.m_profile;
	}

	void CorbaDutyStateConverter::subsystemComplete()
	{
	}

	void CorbaDutyStateConverter::regionComplete()
	{
	}
}



