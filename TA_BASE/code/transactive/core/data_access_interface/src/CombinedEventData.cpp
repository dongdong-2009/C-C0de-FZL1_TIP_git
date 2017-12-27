/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/CombinedEventData.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is an implementation of the ICombinedEvent interface. It provides all
  * the data required by the Event Viewer from both the EVENT and LO_DATAPT_STATE_CHANGE
  * tables.
  *
  * All data is populated when the object is constructed and is read-only.
  *
  */

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include <memory>
#include <sstream>

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IDatabase;

namespace TA_Base_Core
{
    const ECombinedEventType CombinedEventData::getEventType()
    {
        return m_type;
    }

	const EventCategory CombinedEventData::getEventCategoryType()
	{
		return m_categoryType;
	}



    const unsigned long CombinedEventData::getKey()
    {
        return m_key;
    }


    const timeb CombinedEventData::getCreateTime()
    {
        return m_createTime;
    }


    const std::string& CombinedEventData::getAlarmSeverityName()
    {
        return m_alarmSeverityName;
    }


    const std::string& CombinedEventData::getAssetName()
    {
        return m_assetName;
    }

	const std::string& CombinedEventData::getEventTypeName()
	{
		return m_EventTypeName;
	}

    const std::string& CombinedEventData::getDescription()
    {
        return m_description;
    }


    const std::string& CombinedEventData::getValue()
    {
        return m_value;
    }


    const std::string& CombinedEventData::getAlarmMmsState()
    {
        return m_alarmMmsState;
    }

	const std::string& CombinedEventData::getOperationMode()
	{
		return m_operationMode;
	}

    const std::string& CombinedEventData::getAlarmDssState()
    {
        return m_alarmDssState;
    }


    const std::string& CombinedEventData::getAlarmAvlState()
    {
        return m_alarmAvlState;
    }


    const std::string& CombinedEventData::getOperatorName()
    {
        return m_operatorName;        
    }


    const std::string& CombinedEventData::getAlarmComment()
    {
        return m_alarmComment;
    }

    const std::string& CombinedEventData::getAlarmId()
    {
        return m_alarmId;
    }

    const std::string& CombinedEventData::getAlarmAck()
    {
        return m_alarmAck;
    }

    const std::string& CombinedEventData::getAlarmStatus()
    {
        return m_alarmStatus;
    }

	//++Noel
	const std::string& CombinedEventData::getSourceTable()
	{
		return m_sourceTable;
	}

	const unsigned long CombinedEventData::getSubsystemKey()
	{
		return m_subsystemKey;
	}

	const unsigned long CombinedEventData::getPhysicalSubsystemKey()
	{
		return m_physicalSubsystemKey;
	}

	const unsigned long CombinedEventData::getLocationKey()
	{
		return m_locationKey;
	}

	const unsigned long CombinedEventData::getSeverityKey()
	{
		return m_severityKey;
	}

	const unsigned long CombinedEventData::getEventTypeKey()
	{
		return m_eventTypeKey;
	}

	const unsigned long CombinedEventData::getAlarmTypeKey()
	{
		return m_alarmTypeKey;
	}

	const unsigned long CombinedEventData::getOperatorKey()
	{
		return m_operatorKey;
	}

	const unsigned long CombinedEventData::getEventLevel()
	{
		return m_eventLevel;
	}

	const std::string& CombinedEventData::getSessionKey()
	{
		return m_sessionKey;
	}

	const unsigned long CombinedEventData::getSessionLocation()
	{
		return m_sessionLocation;
	}

	const unsigned long CombinedEventData::getProfileId()
	{
		return m_profileId;
	}

	const unsigned long CombinedEventData::getActionId()
	{
		return m_actionId;
	}

	const unsigned long CombinedEventData::getEntityKey()
	{
		return m_entityKey;
	}

	const std::string& CombinedEventData::getAvlAlarmHeadId()
	{
		return m_avlalarmheadid;
	}

	const unsigned long CombinedEventData::getSystemKey()
	{
		return m_systemKey;
	}

	const std::string& CombinedEventData::getEventID()
	{
		return m_eventID;
	}

};