/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetExclusiveDutyConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements GetExclusiveDutyConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/GetExclusiveDutyConverter.h"

namespace TA_Base_App
{
	GetExclusiveDutyConverter::GetExclusiveDutyConverter() :
		m_subsystems(new TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence),
		m_regIdx(0),
		m_subIdx(0),
		m_firstRegion(true),
		m_hasDuty(false)
	{
		m_subsystems->length(0);
	}

	bool GetExclusiveDutyConverter::getResult() const
	{
		return( m_hasDuty );
	}
	
	TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence* GetExclusiveDutyConverter::getSubsystems()
	{
		return( m_subsystems._retn() );
	}

	void GetExclusiveDutyConverter::setSession(const std::string& session)
	{
	}


	void GetExclusiveDutyConverter::setAction(const std::string& action)
	{
	}

	void GetExclusiveDutyConverter::setRegionCount(unsigned long count)
	{
		m_subsystems->length(count);

        // TD10809: Having control of a region does not necessarily mean that a session
        //          has duty for an exclusive subsystem.
        /*
		if( count > 0 )
		{
			m_hasDuty = true;
		}
        */
	}

	void GetExclusiveDutyConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		if( m_firstRegion )
		{
			m_firstRegion = false;
		}
		else
		{
			m_regIdx++;
		}

		m_subsystems[m_regIdx].region = region;
		m_subsystems[m_regIdx].subsystems.length(0);
		m_subIdx = 0;
	}

	void GetExclusiveDutyConverter::setSubsystemCount(unsigned long count)
	{
		m_subsystems[m_regIdx].subsystems.length(count);
	}

	void GetExclusiveDutyConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_hasDuty = true;
		m_subsystems[m_regIdx].subsystems[m_subIdx] = subsystem;
		m_subIdx++;
	}


	void GetExclusiveDutyConverter::setRegionAddCount( unsigned long count )
	{
	}

	void GetExclusiveDutyConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void GetExclusiveDutyConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void GetExclusiveDutyConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void GetExclusiveDutyConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void GetExclusiveDutyConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void GetExclusiveDutyConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void GetExclusiveDutyConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void GetExclusiveDutyConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void GetExclusiveDutyConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void GetExclusiveDutyConverter::subsystemComplete()
	{
	}

	void GetExclusiveDutyConverter::regionComplete()
	{
	}
}



