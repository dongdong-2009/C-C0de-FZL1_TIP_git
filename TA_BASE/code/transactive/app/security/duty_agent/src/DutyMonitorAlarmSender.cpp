/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitorAlarmSender.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Implements DutyMonitorAlarmSender.
  *
  */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyMonitorAlarmSender.h"

namespace TA_Base_App
{
	const std::string DutyMonitorAlarmSender::REGION_NAME("RegionName");
	const std::string DutyMonitorAlarmSender::SUBSYSTEM_NAME("SubsystemName");

	DutyMonitorAlarmSender::DutyMonitorAlarmSender()
		//TD18093,jianghp
		//m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper())
		//TD18093
	{
		FUNCTION_ENTRY("DutyMonitorAlarmSender");
		FUNCTION_EXIT;
	}
	
	void DutyMonitorAlarmSender::send( const TA_Base_Bus::DASType::DbKey& region )
	{
		FUNCTION_ENTRY("send");

		TA_Base_Core::DescriptionParameters descParams;

		std::string regionName = addRegion( descParams, region );

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Sending RegionNotAssigned alarm for %s", regionName.c_str() );
		
		send( TA_Base_Core::DutyAgentAlarms::RegionNotAssigned, descParams );
			
		FUNCTION_EXIT;
	}

	void DutyMonitorAlarmSender::send( const TA_Base_Bus::DASType::DbKey& subsystem, const TA_Base_Bus::DASType::DbKey& region )
	{
		FUNCTION_ENTRY("send");

		TA_Base_Core::DescriptionParameters descParams;

		std::string subsystemName = addSubsystem( descParams, subsystem );
		std::string regionName    = addRegion   ( descParams, region    );

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Sending SubsystemNotAssigned alarm for %s at %s", subsystemName.c_str(), regionName.c_str() );
		
		send( TA_Base_Core::DutyAgentAlarms::SubsystemNotAssigned, descParams );
			
		FUNCTION_EXIT;
	}

	std::string DutyMonitorAlarmSender::addRegion( TA_Base_Core::DescriptionParameters& description, const TA_Base_Bus::DASType::DbKey& region    )
	{
		const std::string regionName = TA_Base_Bus::DutyDbCache::instance()->getRegionName(region);

		std::auto_ptr<TA_Base_Core::NameValuePair> regPair( new TA_Base_Core::NameValuePair(REGION_NAME,regionName) );

		description.push_back(regPair.release());

		return( regionName );
	}

	std::string DutyMonitorAlarmSender::addSubsystem( TA_Base_Core::DescriptionParameters& description, const TA_Base_Bus::DASType::DbKey& subsystem )
	{
		const std::string subsystemName = TA_Base_Bus::DutyDbCache::instance()->getSubsystemName(subsystem);

		std::auto_ptr<TA_Base_Core::NameValuePair> subPair( new TA_Base_Core::NameValuePair(SUBSYSTEM_NAME,subsystemName) );

		description.push_back(subPair.release());

		return( subsystemName );
	}

	void DutyMonitorAlarmSender::send( const TA_Base_Core::MessageType& alarmType,
		TA_Base_Core::DescriptionParameters& description )
	{
		TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
			alarmType,
			TA_Base_Bus::TransactiveAgentInfo::m_entity,
			TA_Base_Bus::TransactiveAgentInfo::m_entityType,
			description,
			TA_Base_Bus::TransactiveAgentInfo::m_name,
			TA_Base_Bus::TransactiveAgentInfo::m_location,
			TA_Base_Bus::TransactiveAgentInfo::m_subsystem,
			TA_Base_Bus::TransactiveAgentInfo::m_assetName,
			TA_Base_Core::AlarmConstants::defaultTime );

		TA_Base_Core::DescriptionParameters::iterator descIt;

		for( descIt=description.begin(); descIt!=description.end(); descIt++ )
		{
			delete *descIt;
		}
	}
}

