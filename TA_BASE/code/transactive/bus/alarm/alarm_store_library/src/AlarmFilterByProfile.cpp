/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmFilterByProfile.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmFilterByProfile.cpp
//  Implementation of the Class AlarmFilterByProfile
//  Created on:      24-Jan-2004 04:19:27 PM
///////////////////////////////////////////////////////////

#define  Enable_Profile_Cache  "enableprofilecache" 
#include "AlarmFilterByProfile.h"
#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Bus;
using TA_Base_Core::DebugUtil;

////////////////////////////////////////////////////////////////////////////

// yanrong++: reduce access to righs agent.
AlarmRightsQuery::AlarmRightsQuery(unsigned long locationKey, unsigned long subsystemKey, bool isVisible)
: m_locationKey(locationKey)
, m_subsystemKey(subsystemKey)
, m_isVisible(isVisible)
{

}

bool AlarmRightsQuery::operator == (const AlarmRightsQuery& rhs) const
{
	return (rhs.m_locationKey == m_locationKey && rhs.m_subsystemKey == m_subsystemKey);
}

bool AlarmRightsQuery::operator < (const AlarmRightsQuery& rhs) const
{
	if(m_locationKey < rhs.m_locationKey)
	{
		return true;
	}
	
	if(m_locationKey == rhs.m_locationKey)
	{
		if(m_subsystemKey < rhs.m_subsystemKey)
		{
			return true;
		}
	}

	return false;
}
// ++yanrong.

//////////////////////////////////////////////////////////////////////////////////

AlarmFilterByProfile::AlarmFilterByProfile(const std::string &sessionID)
					 : IAlarmFilter(COMPARE_EQUAL),  // Does not use a comparitor!
						m_sessionID(sessionID.c_str()),
						m_rightsLibrary(NULL) 
{
	FUNCTION_ENTRY("Constructor");
	LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"Session:%s" ,m_sessionID.c_str());


	std::string paraValue="true";
	FUNCTION_EXIT;
}



AlarmFilterByProfile::~AlarmFilterByProfile()
{
	FUNCTION_ENTRY("Desstructor");
	
	// TD #10075
	// Let the AlarmStoreServant delete the rights library when it's done
	/*if (m_rightsLibrary != NULL)
	{
		delete m_rightsLibrary;
		m_rightsLibrary = NULL;
	}*/

	FUNCTION_EXIT;
}

void AlarmFilterByProfile::setRightsLibrary(TA_Base_Bus::RightsLibrary* rightsLibrary)
{
	m_rightsLibrary = rightsLibrary;
}


bool AlarmFilterByProfile::operator == (const IAlarmFilter& filter) const
{
	FUNCTION_ENTRY("Operator==");
	// See if it is the same filter type
	const AlarmFilterByProfile* profileFilter = dynamic_cast<const AlarmFilterByProfile*>(&filter);
	if (profileFilter!=NULL)
	{
		// Check parameters are the same
		if (m_sessionID.compare(profileFilter->m_sessionID.c_str()) ==0 )
		{
			return true;
		}
	}

	FUNCTION_EXIT;
	return false;
}


bool AlarmFilterByProfile::includeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	TA_Base_Bus::AlarmActions::AlarmActionType   alarmAction = TA_Base_Bus::AlarmActions::VIEW_ALARM;

	bool isVisible = false;

	try
	{
		// Determine the action type that is associated with this alarm type
		TA_Base_Bus::AccessControlledAction action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm.messageTypeKey,
																					 alarmAction);

		// Get the resource key
		//TA_Base_Core::IResource* r = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(alarm.assocEntityKey );
		//unsigned long resourceID = r->getKey();
		//delete r;
		//r = NULL;

		// Get the rights - AJP TODO need to use a new version of this call to request if profile rights/duty flag should be checked.
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;
		
		// the rights library should be created in the AlarmStoreServant and handed to
		// this filter via the AlarmFilterByProfile::setRightsLibrary() function,
		// but create it here in the event that it hasn't been set
        if (NULL == m_rightsLibrary)
        {
	        // Setup access to the rights library
	        TA_Base_Bus::RightsLibraryFactory rlf;
            m_rightsLibrary = rlf.buildRightsLibrary();
        }

		isVisible = m_rightsLibrary->isActionPermittedOnResourceAndLocSubWithoutDuty(std::string(m_sessionID), 
									alarm.assocEntityKey, alarm.locationKey, alarm.subsystemKey,
									action, reason, decisionModule);
	}
	catch (...)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			"Unable to determine visibility rights for alarm '%s' type %u, excluding",alarm.alarmID.in(),alarm.messageTypeKey);//TD14337 yezh++
	}

	FUNCTION_EXIT;
	return isVisible;
}

// yanrong++: reduce access to righs agent.
bool AlarmFilterByProfile::rightsLookup(unsigned long locationKey, unsigned long subsystemKey, bool& isVisible)
{
	FUNCTION_ENTRY("rightsLookup");
	TA_Base_Core::ThreadGuard guard(m_lock);

	AlarmRightsQuery query(locationKey, subsystemKey, isVisible);
	AlarmRithsCache::iterator iter = m_cache.find(query);

	if(iter == m_cache.end())
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "rightsLookup can't find rights in cache, locationKey:%li, subsystemKey:%li",
			locationKey, subsystemKey);
		return false;
	}

	isVisible = iter->m_isVisible;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "rightsLookup locationKey:%li, subsystemKey:%li, isVisible:%li",
		locationKey, subsystemKey, isVisible?1:0);

	FUNCTION_EXIT;
	return true;
}

void AlarmFilterByProfile::rightsAdd(unsigned long locationKey, unsigned long subsystemKey, bool isVisible)
{
	FUNCTION_ENTRY("rightsAdd");
	TA_Base_Core::ThreadGuard guard(m_lock);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "rightsAdd insert rihgts: session:%s, locationKey:%li, subsystemKey:%li, isVisible:%li",
	m_sessionID.c_str(),locationKey, subsystemKey, isVisible?1:0);
	AlarmRightsQuery query(locationKey, subsystemKey, isVisible);
	m_cache.insert(query);
	FUNCTION_EXIT;
}

void AlarmFilterByProfile::reset()
{
	FUNCTION_ENTRY("reset");
	TA_Base_Core::ThreadGuard guard(m_lock);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "AlarmFilterByProfile::reset");
	m_cache.clear();
	FUNCTION_EXIT;
}
// ++yanrong.

/*-------------------------------------- New Alarm Store ---------------------------------------------*/

bool AlarmFilterByProfile::includeAlarm(const TA_Base_Core::AlarmItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	if (alarm->isExternalAlarm) return true;
	
	TA_Base_Bus::AlarmActions::AlarmActionType   alarmAction = TA_Base_Bus::AlarmActions::VIEW_ALARM;

	bool isVisible = false;
	
	try
	{
		// Determine the action type that is associated with this alarm type
		TA_Base_Bus::AccessControlledAction action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
			alarmAction);
		
		// Get the resource key
		//TA_Base_Core::IResource* r = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(alarm.assocEntityKey );
		//unsigned long resourceID = r->getKey();
		//delete r;
		//r = NULL;
		
		// Get the rights - AJP TODO need to use a new version of this call to request if profile rights/duty flag should be checked.
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;
		
		// the rights library should be created in the AlarmStoreServant and handed to
		// this filter via the AlarmFilterByProfile::setRightsLibrary() function,
		// but create it here in the event that it hasn't been set
        if (NULL == m_rightsLibrary)
        {
			// Setup access to the rights library
			TA_Base_Bus::RightsLibraryFactory rlf;
            m_rightsLibrary = rlf.buildRightsLibrary();
        }
// 		m_enable_profile_cache=false;// GF project,disable the rightcache
// 		if (m_enable_profile_cache)
// 		{
// 			// yanrong++: reduce access to rihgts agent.
// 			if( !rightsLookup(alarm->locationKey, alarm->subsystemKey, isVisible) )
// 			{
// 				isVisible = m_rightsLibrary->isActionPermittedOnResourceAndLocSubWithoutDuty(std::string(m_sessionID), 
// 					alarm->assocEntityKey, alarm->locationKey, alarm->subsystemKey,
// 					action, reason, decisionModule);
// 
// 				rightsAdd(alarm->locationKey, alarm->subsystemKey, isVisible);
// 			}
// 		}
// 		else
		{
			isVisible = m_rightsLibrary->isActionPermittedOnResourceAndLocSubWithoutDuty(std::string(m_sessionID), 
				alarm->assocEntityKey, alarm->locationKey, alarm->subsystemKey,
				action, reason, decisionModule);
		}
		// ++yanrong.
	}
	catch (...)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			"Unable to determine visibility rights for alarm '%s' type %d, excluding",alarm->alarmID,alarm->messageTypeKey);//TD14337 yezh++
	}
	
	FUNCTION_EXIT;
	return isVisible;
}

/*--------------------------------------------------------------------------------------------------------*/

//++Noel
bool AlarmFilterByProfile::includeAlarm(const TA_Base_Bus::AlarmDataItem* alarm)
{
	FUNCTION_ENTRY("includeAlarm");
	
	TA_Base_Bus::AlarmActions::AlarmActionType   alarmAction = TA_Base_Bus::AlarmActions::VIEW_ALARM;
	
	bool isVisible = false;
	
	try
	{
		// Determine the action type that is associated with this alarm type
		TA_Base_Bus::AccessControlledAction action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
			alarmAction);
		
		// Get the resource key
		//TA_Base_Core::IResource* r = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(alarm.assocEntityKey );
		//unsigned long resourceID = r->getKey();
		//delete r;
		//r = NULL;
		
		// Get the rights - AJP TODO need to use a new version of this call to request if profile rights/duty flag should be checked.
		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;
		
		// the rights library should be created in the AlarmStoreServant and handed to
		// this filter via the AlarmFilterByProfile::setRightsLibrary() function,
		// but create it here in the event that it hasn't been set
        if (NULL == m_rightsLibrary)
        {
			// Setup access to the rights library
			TA_Base_Bus::RightsLibraryFactory rlf;
            m_rightsLibrary = rlf.buildRightsLibrary();
        }
		
		// yanrong++: reduce access to rihgts agent.
		// 		if( !rightsLookup(alarm->locationKey, alarm->subsystemKey, isVisible) )
		// 		{
		isVisible = m_rightsLibrary->isActionPermittedOnResourceAndLocSubWithoutDuty(std::string(m_sessionID), 
			alarm->assocEntityKey, alarm->locationKey, alarm->subsystemKey,
			action, reason, decisionModule);
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Rights check: AlarmID=%s, EntityKey=%ld, LocationKey=%ld, SubSystem=%ld, isVisible=%s", alarm->alarmID.c_str(), alarm->assocEntityKey, alarm->locationKey, alarm->subsystemKey, (isVisible?"TRUE":"FALSE"));
		/*}*/
		// ++yanrong.
	}
	catch (...)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			"Unable to determine visibility rights for alarm '%s' type %ld, excluding",alarm->alarmID.c_str() ,alarm->messageTypeKey);//TD14337 yezh++
	}
	
	FUNCTION_EXIT;
	return isVisible;
}