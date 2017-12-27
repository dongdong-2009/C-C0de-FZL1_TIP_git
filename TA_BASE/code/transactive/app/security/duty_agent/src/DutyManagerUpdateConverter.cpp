/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyManagerUpdateConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements DutyManagerUpdateConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyManagerUpdateConverter.h"

namespace TA_Base_App
{
	DutyManagerUpdateConverter::DutyManagerUpdateConverter() :
		m_rtaIndex(0),
		m_rtrIndex(0),
		m_staIndex(0),
		m_strIndex(0),
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM),
		m_addSubState(TA_Base_Bus::SubsystemState::SS_NORMAL),
		m_remSubState(TA_Base_Bus::SubsystemState::SS_NORMAL)
	{
		m_regionsToAdd.length(0);
		m_regionsToRemove.length(0);

		m_subsystemsToAdd.length(0);
		m_subsystemsToRemove.length(0);
	}

	const TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence& DutyManagerUpdateConverter::getRegionsToAdd() const
	{
		return( m_regionsToAdd );
	}

	const TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence& DutyManagerUpdateConverter::getRegionsToRemove() const
	{
		return( m_regionsToRemove );
	}

	const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence& DutyManagerUpdateConverter::getSubsystemsToAdd() const
	{
		return( m_subsystemsToAdd );
	}

	const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence& DutyManagerUpdateConverter::getSubsystemsToRemove() const
	{
		return( m_subsystemsToRemove );
	}

	void DutyManagerUpdateConverter::setSession(const std::string& session)
	{
	}


	void DutyManagerUpdateConverter::setAction(const std::string& action)
	{
	}

	void DutyManagerUpdateConverter::setRegionCount(unsigned long count)
	{
	}

	void DutyManagerUpdateConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
	}

	void DutyManagerUpdateConverter::setSubsystemCount(unsigned long count)
	{
	}

	void DutyManagerUpdateConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem   = subsystem;
		m_addSubState = addState;
		m_remSubState = remState;
	}


	void DutyManagerUpdateConverter::setRegionAddCount( unsigned long count )
	{
		m_regionsToAdd.length( m_regionsToAdd.length() + count );
	}

	void DutyManagerUpdateConverter::setRegionRemoveCount( unsigned long count )
	{
		m_regionsToRemove.length( m_regionsToRemove.length() + count );
	}

	void DutyManagerUpdateConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		updateRegionSeq( session, m_regionsToAdd, m_rtaIndex );
	}


	void DutyManagerUpdateConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		updateRegionSeq( session, m_regionsToRemove, m_rtrIndex );
	}


	void DutyManagerUpdateConverter::setSubsystemAddCount( unsigned long count )
	{
		m_subsystemsToAdd.length( m_subsystemsToAdd.length() + count);
	}

	void DutyManagerUpdateConverter::setSubsystemRemoveCount( unsigned long count )
	{
		m_subsystemsToRemove.length( m_subsystemsToRemove.length() + count );
	}

	void DutyManagerUpdateConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		updateSubsystemSeq( session, m_addSubState, m_subsystemsToAdd, m_staIndex );
	}


	void DutyManagerUpdateConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		updateSubsystemSeq( session, m_remSubState, m_subsystemsToRemove, m_strIndex );
	}


	void DutyManagerUpdateConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DutyManagerUpdateConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_subsystemsToRemove.length(m_subsystemsToRemove.length()+1);
		updateSubsystemSeq( session, m_remSubState, m_subsystemsToRemove, m_strIndex );
	}

	void DutyManagerUpdateConverter::updateRegionSeq( const TA_Base_Bus::DutyPrimitive& session,
		TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence& regionSeq, CORBA::ULong& index )
	{
		regionSeq[index].session         = CORBA::string_dup( session.m_session.c_str() );
		regionSeq[index].regionKey      = m_region;
		regionSeq[index].profileKey     = session.m_profile;
		regionSeq[index].profileTypeKey =  TA_Base_Bus::DutyDbCache::instance()->getRawProfileType(session.m_profile);

		index++;
	}

	void DutyManagerUpdateConverter::updateSubsystemSeq( const TA_Base_Bus::DutyPrimitive& session,
		const TA_Base_Bus::SubsystemState::State& subState,
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence& subsystemSeq,	CORBA::ULong& index )
	{
		subsystemSeq[index].session       = CORBA::string_dup( session.m_session.c_str() );
		subsystemSeq[index].regionKey      = m_region;
		subsystemSeq[index].subsystemKey   = m_subsystem;
		subsystemSeq[index].profileKey     = session.m_profile;
		subsystemSeq[index].actionGroupKey = TA_Base_Bus::DutyDbCache::instance()->getRights(
			session.m_profile, m_subsystem, subState);

		index++;
	}

	void DutyManagerUpdateConverter::subsystemComplete()
	{
	}

	void DutyManagerUpdateConverter::regionComplete()
	{
	}
}



