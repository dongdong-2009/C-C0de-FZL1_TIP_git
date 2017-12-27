/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/RightsLibraryUpdateConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements RightsLibraryUpdateConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/RightsLibraryUpdateConverter.h"

namespace TA_Base_App
{
	RightsLibraryUpdateConverter::RightsLibraryUpdateConverter() :
		m_regIdx(0),
		m_subIdx(0),
		m_sesIdx(0),
		m_firstRegion(true),
		m_firstSubsystem(true)
	{
		m_update.length(0);
	}

	const TA_Base_Bus::DutyAgentTypeCorbaDef::RightsLibraryUpdateSequence& RightsLibraryUpdateConverter::getUpdate() const
	{
		return(m_update);
	}

	void RightsLibraryUpdateConverter::setSession(const std::string& session)
	{
	}


	void RightsLibraryUpdateConverter::setAction(const std::string& action)
	{
	}

	void RightsLibraryUpdateConverter::setRegionCount(unsigned long count)
	{
		m_update.length(count);
	}

	void RightsLibraryUpdateConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		if(m_firstRegion)
		{
			m_firstRegion = false;
		}
		else
		{
			m_regIdx++;
		}

		m_update[m_regIdx].region = region;

		m_firstSubsystem  = true;
		m_subIdx = 0;

	}

	void RightsLibraryUpdateConverter::setSubsystemCount(unsigned long count)
	{
		m_update[m_regIdx].duty.length(count);
	}

	void RightsLibraryUpdateConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		if(m_firstSubsystem)
		{
			m_firstSubsystem = false;
		}
		else
		{
			m_subIdx++;
		}

		m_update[m_regIdx].duty[m_subIdx].subsystem = subsystem;
		m_sesIdx = 0;
	}


	void RightsLibraryUpdateConverter::setRegionAddCount( unsigned long count )
	{
	}

	void RightsLibraryUpdateConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void RightsLibraryUpdateConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void RightsLibraryUpdateConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void RightsLibraryUpdateConverter::setSubsystemAddCount( unsigned long count )
	{
		m_update[m_regIdx].duty[m_subIdx].sessions.length(
			m_update[m_regIdx].duty[m_subIdx].sessions.length()+count);
	}

	void RightsLibraryUpdateConverter::setSubsystemRemoveCount( unsigned long count )
	{
		m_update[m_regIdx].duty[m_subIdx].sessions.length(
			m_update[m_regIdx].duty[m_subIdx].sessions.length()+count);
	}

	void RightsLibraryUpdateConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		subUpdate(session.m_session);
	}


	void RightsLibraryUpdateConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		subUpdate(session.m_session);
	}


	void RightsLibraryUpdateConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void RightsLibraryUpdateConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_update[m_regIdx].duty[m_subIdx].sessions.length(
			m_update[m_regIdx].duty[m_subIdx].sessions.length()+1);

		subUpdate(session.m_session);

	}

	void RightsLibraryUpdateConverter::subUpdate( const std::string& session )
	{
		m_update[m_regIdx].duty[m_subIdx].sessions[m_sesIdx] = CORBA::string_dup(session.c_str());
		m_sesIdx++;
	}

	void RightsLibraryUpdateConverter::subsystemComplete()
	{
	}

	void RightsLibraryUpdateConverter::regionComplete()
	{
	}
}



