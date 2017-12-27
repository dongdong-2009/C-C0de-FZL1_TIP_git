/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitor.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * 
 * Encapsulates the logic for monitoring the duty status of regions and subsystems. 
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyMonitor.h"

namespace TA_Base_App
{
	DutyMonitor::MonitorInfo::MonitorInfo() :
		m_dutyCount(0),
		m_zeroSince(ACE_OS::gettimeofday()),
		m_checked(false)
	{
	}

	void DutyMonitor::MonitorInfo::operator++(int)
	{
		m_dutyCount++;
		m_checked = false;
	}

	void DutyMonitor::MonitorInfo::operator--(int)
	{
		if( m_dutyCount == 0 )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Attempt to decrement duty count of zero" );
			return;
		}

		m_dutyCount--;

		if( 0 == m_dutyCount )
		{
			m_zeroSince = ACE_OS::gettimeofday();
			m_checked = false;
		}
	}

	bool DutyMonitor::MonitorInfo::isOk()
	{
		if( 0 == m_dutyCount && !m_checked )
		{
			ACE_Time_Value timeout(TA_Base_Bus::TransactiveAgentInfo::m_uncontrolledTimeout,0);

			if((m_zeroSince + timeout) <= ACE_OS::gettimeofday() )
			{
				m_checked = true;
				return( false );
			}
		}

		return(true);
	}

	bool DutyMonitor::MonitorInfo::isChecked()
	{
		return( m_checked );
	}

	DutyMonitor::DutyMonitor()
	{
		const RegMonitorInfo emptyRmi;
		const MonitorInfo    emptyMi;

		TA_Base_Bus::DASType::RegionSet regions;

		// get all the LOCAL regions
		TA_Base_Bus::DutyDbCache::instance()->getLocationRegions(TA_Base_Bus::TransactiveAgentInfo::m_location, regions);

		TA_Base_Bus::DASType::RegionSet::const_iterator    regIt;
		TA_Base_Bus::DASType::SubsystemSet::const_iterator subIt;

		for( regIt=regions.begin(); regIt!=regions.end(); regIt++ )
		{
			std::pair<RegMonitorMap::iterator,bool> regInsertResult;

			// insert the region with a placeholder subsystem map
			regInsertResult = m_regMonitorMap.insert( RegMonitorMap::value_type(*regIt,emptyRmi) );

			// get all the subs for this reg
			TA_Base_Bus::DASType::SubsystemSet subs;
			TA_Base_Bus::DutyDbCache::instance()->getAllSubsystemKeys(*regIt,subs);

			for( subIt=subs.begin(); subIt!=subs.end(); subIt++ )
			{
				regInsertResult.first->second.m_subMonitorMap.insert( SubMonitorMap::value_type(*subIt,emptyMi) );
			}
		}
	}
	
	DutyMonitor::~DutyMonitor()
	{
	}

	void DutyMonitor::add(const TA_Base_Bus::DASType::DbKey& region)
	{
		FUNCTION_ENTRY("add");

		TA_Base_Core::ThreadGuard guard(m_accessLock);

		RegMonitorMap::iterator it;

		if( (it = m_regMonitorMap.find(region)) != m_regMonitorMap.end() )
		{
			(it->second.m_monitorInfo)++;
		}

		FUNCTION_EXIT;
	}

	void DutyMonitor::rem(const TA_Base_Bus::DASType::DbKey& region)
	{
		FUNCTION_ENTRY("rem");

		RegMonitorMap::iterator it;

		TA_Base_Core::ThreadGuard guard(m_accessLock);

		if( (it = m_regMonitorMap.find(region)) != m_regMonitorMap.end() )
		{
			(it->second.m_monitorInfo)--;
		}

		FUNCTION_EXIT;
	}

	void DutyMonitor::add(const TA_Base_Bus::DASType::DbKey& region, const TA_Base_Bus::DASType::DbKey& subsystem)
	{
		FUNCTION_ENTRY("add");

		RegMonitorMap::iterator regIt;
		SubMonitorMap::iterator subIt;

		TA_Base_Core::ThreadGuard guard(m_accessLock);

		if( (regIt = m_regMonitorMap.find(region)) != m_regMonitorMap.end() )
		{
			if( (subIt = regIt->second.m_subMonitorMap.find(subsystem)) != regIt->second.m_subMonitorMap.end() )
			{
				(subIt->second)++;
			}
		}

		FUNCTION_EXIT;
	}

	void DutyMonitor::rem(const TA_Base_Bus::DASType::DbKey& region, const TA_Base_Bus::DASType::DbKey& subsystem)
	{
		FUNCTION_ENTRY("rem");

		RegMonitorMap::iterator regIt;
		SubMonitorMap::iterator subIt;

		TA_Base_Core::ThreadGuard guard(m_accessLock);

		if( (regIt = m_regMonitorMap.find(region)) != m_regMonitorMap.end() )
		{
			if( (subIt = regIt->second.m_subMonitorMap.find(subsystem)) != regIt->second.m_subMonitorMap.end() )
			{
				(subIt->second)--;
			}
		}

		FUNCTION_EXIT;
	}

	void DutyMonitor::check()
	{
		FUNCTION_ENTRY("check");
		RegMonitorMap::iterator regIt;
		SubMonitorMap::iterator subIt;

		TA_Base_Core::ThreadGuard guard(m_accessLock);

		for( regIt=m_regMonitorMap.begin(); regIt!=m_regMonitorMap.end(); regIt++ )
		{
			if( !(regIt->second.m_monitorInfo.isOk()) )
			{
				m_alarmSender.send(regIt->first);
			}
			else
			{
				if( !(regIt->second.m_monitorInfo.isChecked()) )
				{
					for( subIt=regIt->second.m_subMonitorMap.begin(); subIt!=regIt->second.m_subMonitorMap.end(); subIt++ )
					{
						if( !(subIt->second.isOk()) )
						{
							m_alarmSender.send(subIt->first,regIt->first);
						}
					}
				}
			}
		}
		FUNCTION_EXIT;
	}
}
