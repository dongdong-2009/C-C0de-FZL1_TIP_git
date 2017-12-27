/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmHelper.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <time.h>
#include "core/message/types/MessageTypes.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperBase.h"
#include "core/alarm/src/AlarmConfig.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

	AlarmHelper::AlarmHelper(AlarmHelperBase& alarmHelperBase)
		:m_alarmHelperBase(alarmHelperBase)
	{
		////TD17994,jianghp
		//TA_ASSERT(NULL != m_alarmHelperBase, "AlarmHelperBase was NULL");
	}


	AlarmHelper::~AlarmHelper()
	{
		// Do not destroy the alarmHelperBase as it is
		// handled by the AlarmHelperManager
	}


	void AlarmHelper::submitAlarm(const TA_Base_Core::MessageType& alarmType,
											unsigned long entityKey,
											unsigned long entityTypeKey,
											const TA_Base_Core::DescriptionParameters& description,
											const std::string& entityName,
											unsigned long locationKey,
											unsigned long subsystemKey,
											timeb sourceTime /*= 0*/)
	{
		TA_ASSERT(entityKey > 0, "EntityKey for alarm to submit was 0");
		TA_ASSERT(!entityName.empty(), "EntityName for alarm to submit was 0");
				
		// get the severity
		unsigned long alarmSeverityKey( AlarmTypeTableAccessor::getInstance().getSeverityKey(alarmType) );

		// submit the alarm
		submitAlarmWithSeverity(
			alarmType,
			entityKey,
			entityTypeKey,
			description,
			entityName,
			locationKey,
			subsystemKey,
			alarmSeverityKey,
			sourceTime
			);
	}


	void AlarmHelper::submitAlarmWithSeverity(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
			timeb sourceTime /*= 0*/ )
	{
		TA_ASSERT(entityKey > 0, "EntityKey for alarm to submit was 0");
		TA_ASSERT(!entityName.empty(), "EntityName for alarm to submit was 0");
				
		// submit the alarm
		submitAlarmWithSeverityAndAsset(
			alarmType,
			entityKey,
			entityTypeKey,
			description,
			entityName,
			locationKey,
			subsystemKey,
			alarmSeverityKey,
			entityName,           // The entity name is used as the asset name if none is specified.
			sourceTime
			);
	}

	void AlarmHelper::submitAlarmWithAsset(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			const std::string& assetName,
			timeb sourceTime /*= 0*/ )
	{
		TA_ASSERT(entityKey > 0, "EntityKey for alarm to submit was 0");
		TA_ASSERT(!entityName.empty(), "EntityName for alarm to submit was 0");

		// get the severity
		unsigned long alarmSeverityKey( AlarmTypeTableAccessor::getInstance().getSeverityKey(alarmType) );
				
		// submit the alarm
		submitAlarmWithSeverityAndAsset(
			alarmType,
			entityKey,
			entityTypeKey,
			description,
			entityName,
			locationKey,
			subsystemKey,
			alarmSeverityKey,
			assetName,
			sourceTime
			);
	}

	void AlarmHelper::submitAlarmWithSeverityAndAsset(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
			const std::string&  assetName,
			timeb sourceTime /*= 0*/,
            TA_Base_Core::MmsStateType mmsState /*=MMS_NONE*/)
	{
		TA_ASSERT( 0 != alarmSeverityKey, "The alarm severity must be set!" );

		// TD 15195 Check whether this alarm type is used
		bool isVisible = AlarmTypeTableAccessor::getInstance().isVisible(alarmType);
		if (!isVisible) return; // ignore the submission

		// Wenguang++ Alarm Message Size Reduction
		if( locationKey == 0)
		{
			return; // ignore the submission when location key is 0
		}
	
		// get the sourcetime if it is not set
		if ( 0 == sourceTime.time )
		{
			ftime( &sourceTime );
		}

		// get the Alarm Object
		AlarmDetailCorbaDef* alarm = m_alarmHelperBase.createAlarmWithParams(
			alarmType,
			entityKey,
			entityTypeKey,
			description,
			entityName,
			locationKey,
			subsystemKey,
			alarmSeverityKey,
			sourceTime,
			assetName,
            true,
            mmsState);

		// Create the new queue item
		AlarmHelperBase::AlarmQueueItem* queueItem = new AlarmHelperBase::AlarmQueueItem();
		queueItem->operation = SubmitAlarmUnique;
		queueItem->alarm = alarm;
		std::stringstream stringStream;
		stringStream << alarmType.getTypeKey();
		stringStream >> queueItem->alarmTypeKey;
		queueItem->entityKey = entityKey;
		queueItem->locationKey = locationKey;

		// Finally, submit the alarm
		m_alarmHelperBase.addAlarmQueueItem(queueItem);
	}

	AlarmSequence* AlarmHelper::getActiveAlarms( unsigned long locationKey )
	{
		return m_alarmHelperBase.getActiveAlarms(locationKey);
	}
	void AlarmHelper::closeAlarm(const TA_Base_Core::MessageType& alarmType,
								unsigned long entityKey,
								const std::string &closeValue /*= ""*/,
								const timeb closeTime /*= AlarmConstants::defaultTime*/)
	{
		TA_ASSERT(entityKey > 0, "EntityKey for alarm to submit was 0");

		// TD 15195 Check whether this alarm type is used
		bool isVisible = AlarmTypeTableAccessor::getInstance().isVisible(alarmType);
		if (!isVisible) return; // ignore the submission
		
		AlarmHelperBase::AlarmQueueItem* queueItem = new AlarmHelperBase::AlarmQueueItem();

		queueItem->alarm = NULL;
		queueItem->operation = CloseAlarmUnique;
		std::stringstream stringStream;
		stringStream << alarmType.getTypeKey();
		stringStream >> queueItem->alarmTypeKey;
		queueItem->entityKey = entityKey;
		queueItem->locationKey = 0;
		queueItem->closeValue = closeValue;
		queueItem->closeTime = closeTime.time;

		// Add it to the queue alarm
		m_alarmHelperBase.addAlarmQueueItem(queueItem);
	}

	void AlarmHelper::closeAlarmAtLocation(const TA_Base_Core::MessageType& alarmType,
											unsigned long entityKey,
											unsigned long locationKey,
											const std::string &closeValue /*= ""*/,
											const timeb closeTime /*= AlarmConstants::defaultTime*/)
	{
		TA_ASSERT(entityKey > 0, "EntityKey for alarm to submit was 0");

		// TD 15195 Check whether this alarm type is used
		bool isVisible = AlarmTypeTableAccessor::getInstance().isVisible(alarmType);
		if (!isVisible) return; // ignore the submission

		AlarmHelperBase::AlarmQueueItem* queueItem = new AlarmHelperBase::AlarmQueueItem();

		queueItem->alarm = NULL;
		queueItem->operation = CloseAlarmUniqueAtLocation;
		std::stringstream stringStream;
		stringStream << alarmType.getTypeKey();
		stringStream >> queueItem->alarmTypeKey;
		queueItem->entityKey = entityKey;
		queueItem->locationKey = locationKey;
		queueItem->closeValue = closeValue;
		queueItem->closeTime = closeTime.time;

		// Add it to the queue alarm
		m_alarmHelperBase.addAlarmQueueItem(queueItem);
	}


	AlarmDetailCorbaDef* AlarmHelper::getMostRecentActiveAlarmForEntity( unsigned long entityKey, 
			unsigned long locationKey )
	{
		return m_alarmHelperBase.getMostRecentActiveAlarmForEntity( entityKey, locationKey );
	}

	//TD 13619 provide a function for other systems/agents to obtain a unique alarms' detail for acknowlegement/closing, etc
	AlarmDetailCorbaDef* AlarmHelper::getActiveAlarmForEntityOfType( const TA_Base_Core::MessageType& alarmType, 
			unsigned long entityKey, unsigned long locationKey )
	{
		std::stringstream stringStream;
		stringStream << alarmType.getTypeKey();
		unsigned long alarmTypeKey;
		stringStream >> alarmTypeKey;
		return m_alarmHelperBase.getActiveAlarmForEntityOfType(alarmTypeKey, entityKey, locationKey);
	}

}; // namespace TA_Base_Core
