/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitorStateConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements DutyMonitorStateConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyMonitorStateConverter.h"
#include "app/security/duty_agent/src/DutyMonitorTask.h"

namespace TA_Base_App
{
	DutyMonitorStateConverter::DutyMonitorStateConverter( DutyMonitorTask& monitorTask ) :
		m_monitorTask(monitorTask),
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM)
	{
	}

	void DutyMonitorStateConverter::setSession(const std::string& session)
	{
	}


	void DutyMonitorStateConverter::setAction(const std::string& action)
	{
	}

	void DutyMonitorStateConverter::setRegionCount(unsigned long count)
	{
	}

	void DutyMonitorStateConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
	}
	
	void DutyMonitorStateConverter::setRegionAddCount( unsigned long count )
	{
	}

	void DutyMonitorStateConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void DutyMonitorStateConverter::setSubsystemCount(unsigned long count)
	{
	}

	void DutyMonitorStateConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem = subsystem;
	}


	void DutyMonitorStateConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_monitorTask.add(m_region);
	}


	void DutyMonitorStateConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_monitorTask.rem(m_region);
	}

	void DutyMonitorStateConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void DutyMonitorStateConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void DutyMonitorStateConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_monitorTask.add(m_region, m_subsystem);
	}


	void DutyMonitorStateConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_monitorTask.rem(m_region, m_subsystem);
	}


	void DutyMonitorStateConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DutyMonitorStateConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_monitorTask.rem(m_region, m_subsystem);
	}

	void DutyMonitorStateConverter::subsystemComplete()
	{
	}

	void DutyMonitorStateConverter::regionComplete()
	{
	}
}



