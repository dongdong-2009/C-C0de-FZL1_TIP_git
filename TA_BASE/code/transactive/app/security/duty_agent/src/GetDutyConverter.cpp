/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetDutyConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements GetDutyConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/GetDutyConverter.h"

namespace TA_Base_App
{
	GetDutyConverter::GetDutyConverter(const char* session, TA_Base_Bus::DutyAgentTypeCorbaDef::Key region,
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem, TA_Base_Bus::DutyAgentTypeCorbaDef::Key profile,
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key& actionGroup) :
		m_session(session),
		m_region(region),
		m_subsystem(subsystem),
		m_profile(profile),
		m_actionGroup(actionGroup),
		m_subState(TA_Base_Bus::SubsystemState::SS_NORMAL),
		m_foundRegion(false),
		m_foundSubsystem(false),
		m_hasDuty(false)
	{
	}

	bool GetDutyConverter::getResult() const
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "foundRegion: %d, foundSubsystem:%d", 
			m_foundRegion, m_foundSubsystem);
		return( m_hasDuty );
	}

	void GetDutyConverter::setSession(const std::string& session)
	{
	}


	void GetDutyConverter::setAction(const std::string& action)
	{
	}

	void GetDutyConverter::setRegionCount(unsigned long count)
	{
	}

	void GetDutyConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		if( m_region == region )
		{
			m_foundRegion = true;
		}
	}

	void GetDutyConverter::setSubsystemCount(unsigned long count)
	{
	}

	void GetDutyConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{

		if( m_foundRegion )
		{
			if( m_subsystem == subsystem )
			{
				m_foundSubsystem = true;
				m_subState       = addState;
			}
		}
	}


	void GetDutyConverter::setRegionAddCount( unsigned long count )
	{
	}

	void GetDutyConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void GetDutyConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void GetDutyConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void GetDutyConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void GetDutyConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void GetDutyConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		if( m_foundRegion && m_foundSubsystem )
		{
			if( m_profile == session.m_profile )
			{
				m_actionGroup = TA_Base_Bus::DutyDbCache::instance()->getRights( m_profile, m_subsystem, m_subState );

				if( TA_Base_Bus::DASConst::NO_ACTION_GROUP != m_actionGroup )
				{
					m_hasDuty = true;
				}
				else
				{
					m_actionGroup = TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY;
				}
			}
		}
	}


	void GetDutyConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void GetDutyConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void GetDutyConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void GetDutyConverter::subsystemComplete()
	{
	}

	void GetDutyConverter::regionComplete()
	{
	}
}



