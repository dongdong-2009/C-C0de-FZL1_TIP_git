/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyManagerSnapshotConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements DutyManagerSnapshotConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyManagerSnapshotConverter.h"

namespace TA_Base_App
{
	DutyManagerSnapshotConverter::DutyManagerSnapshotConverter( const std::string& session) :
        m_subsystemState(new TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence),
		m_subsystemDuty(new TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence),
		m_regionDuty   (new TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence),
		m_regionIndex(0),
		m_subsystemIndex(0),
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM),
		m_subState(TA_Base_Bus::SubsystemState::SS_NORMAL),
		m_session(session)
	{
            m_subsystemState->length(0);
			m_subsystemDuty->length(0);
			m_regionDuty->length(0);
	}

    TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* DutyManagerSnapshotConverter::getSubsystemState()
    {
        return( m_subsystemState._retn() );
    }

	TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence* DutyManagerSnapshotConverter::getSubsystemDuty()
	{
		return( m_subsystemDuty._retn() );
	}

	TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence* DutyManagerSnapshotConverter::getRegionDuty()
	{
		return( m_regionDuty._retn() );
	}

	void DutyManagerSnapshotConverter::setSession(const std::string& session)
	{
	}


	void DutyManagerSnapshotConverter::setAction(const std::string& action)
	{
	}

	void DutyManagerSnapshotConverter::setRegionCount(unsigned long count)
	{
	}

	void DutyManagerSnapshotConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
	}

	void DutyManagerSnapshotConverter::setSubsystemCount(unsigned long count)
	{
	}

	void DutyManagerSnapshotConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem = subsystem;
		m_subState  = addState;

        // TD12449: Remember the state for each subsystem.
        unsigned int insertPosition = m_subsystemState->length();
        m_subsystemState->length(insertPosition+1);
        m_subsystemState[insertPosition].regionKey    = m_region;
        m_subsystemState[insertPosition].subsystemKey = subsystem;
        m_subsystemState[insertPosition].prevState    = remState;
        m_subsystemState[insertPosition].currState    = addState;
	}


	void DutyManagerSnapshotConverter::setRegionAddCount( unsigned long count )
	{
	}

	void DutyManagerSnapshotConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void DutyManagerSnapshotConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_regionDuty->length(m_regionDuty->length()+1);

		m_regionDuty[m_regionIndex].session         = CORBA::string_dup( session.m_session.c_str() );
		m_regionDuty[m_regionIndex].regionKey      = m_region;
		m_regionDuty[m_regionIndex].profileKey     = session.m_profile;
		m_regionDuty[m_regionIndex].profileTypeKey = TA_Base_Bus::DutyDbCache::instance()->getRawProfileType(session.m_profile);
		
		m_regionIndex++;
	}


	void DutyManagerSnapshotConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DutyManagerSnapshotConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void DutyManagerSnapshotConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void DutyManagerSnapshotConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_subsystemDuty->length(m_subsystemDuty->length()+1);

		m_subsystemDuty[m_subsystemIndex].session = CORBA::string_dup( session.m_session.c_str() );
		m_subsystemDuty[m_subsystemIndex].regionKey      = m_region;
		m_subsystemDuty[m_subsystemIndex].subsystemKey   = m_subsystem;
		m_subsystemDuty[m_subsystemIndex].profileKey     = session.m_profile;
		m_subsystemDuty[m_subsystemIndex].actionGroupKey = TA_Base_Bus::DutyDbCache::instance()->getRights(
			session.m_profile, m_subsystem, m_subState);

		m_subsystemIndex++;
	}


	void DutyManagerSnapshotConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DutyManagerSnapshotConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DutyManagerSnapshotConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void DutyManagerSnapshotConverter::subsystemComplete()
	{
	}

	void DutyManagerSnapshotConverter::regionComplete()
	{
	}
}



