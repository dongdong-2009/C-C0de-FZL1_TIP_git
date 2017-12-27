/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/NonUniqueAlarmHelper.cpp $
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
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmHelperBase.h"
#include "core/alarm/src/AlarmConfig.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/NoAlarmAgentException.h"

namespace TA_Base_Core
{
	const std::string NonUniqueAlarmHelper::NOT_VISIBLE_DEFAULT_STRING = "NOT_VISIBLE_ALARM";

	NonUniqueAlarmHelper::NonUniqueAlarmHelper(AlarmHelperBase& alarmHelperBase)
		:m_alarmHelperBase(alarmHelperBase)
	{
		//TA_ASSERT(NULL != alarmHelperBase, "AlarmHelperBase was NULL");
	}


	NonUniqueAlarmHelper::~NonUniqueAlarmHelper()
	{

	}


	std::string NonUniqueAlarmHelper::submitAlarm(const TA_Base_Core::MessageType& alarmType,
											unsigned long entityKey,
											unsigned long entityTypeKey,
											const TA_Base_Core::DescriptionParameters& description,
											const std::string &entityName,
											unsigned long locationKey,
											unsigned long subsystemKey,
											timeb sourceTime /*= 0*/)
	{
		TA_ASSERT(entityKey > 0, "EntityKey for alarm to submit was 0");
		TA_ASSERT(!entityName.empty(), "EntityName for alarm to submit was 0");
		
		// get the severity
		unsigned long alarmSeverityKey( AlarmTypeTableAccessor::getInstance().getSeverityKey(alarmType) );

		// submit the alarm
		return submitAlarmWithSeverity(
			alarmType,
			entityKey,
			entityTypeKey,
			description,
			entityName,
			locationKey,
			subsystemKey,
			alarmSeverityKey,
			sourceTime );
	}


	std::string NonUniqueAlarmHelper::submitAlarmWithSeverity(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
			timeb sourceTime /* = 0*/)
	{

        TA_ASSERT(entityKey > 0, "EntityKey for alarm to submit was 0");
		TA_ASSERT(!entityName.empty(), "EntityName for alarm to submit was 0");

		// submit the alarm
		return submitAlarmWithSeverityAndAsset(
			alarmType,
			entityKey,
			entityTypeKey,
			description,
			entityName,
			locationKey,
			subsystemKey,
			alarmSeverityKey,
			entityName,     // The entity name is used as the asset name if none is specified.
			sourceTime );
	}

	std::string NonUniqueAlarmHelper::submitAlarmWithAsset(const TA_Base_Core::MessageType& alarmType,
											unsigned long entityKey,
											unsigned long entityTypeKey,
											const TA_Base_Core::DescriptionParameters& description,
											const std::string& entityName,
											unsigned long locationKey,
											unsigned long subsystemKey,
											const std::string& assetName,
											timeb sourceTime /*= 0*/,
                                            TA_Base_Core::MmsStateType mmsState /*= MMS_NONE*/)
	{
		TA_ASSERT(entityKey > 0, "EntityKey for alarm to submit was 0");
		TA_ASSERT(!entityName.empty(), "EntityName for alarm to submit was 0");
		
		// get the severity
		unsigned long alarmSeverityKey( AlarmTypeTableAccessor::getInstance().getSeverityKey(alarmType) );

		// submit the alarm
		return submitAlarmWithSeverityAndAsset(
			alarmType,
			entityKey,
			entityTypeKey,
			description,
			entityName,
			locationKey,
			subsystemKey,
			alarmSeverityKey,
			assetName,
			sourceTime,
            mmsState);
	}

	std::string NonUniqueAlarmHelper::submitAlarmWithSeverityAndAsset(const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
			const std::string& assetName,
			timeb sourceTime /* = 0*/,
            TA_Base_Core::MmsStateType mmsState /*= MMS_NONE*/)
	{
		TA_ASSERT( 0 != alarmSeverityKey, "The alarm severity must be set!" );

		// TD 15195 Check whether this alarm type is used
		bool isVisible = AlarmTypeTableAccessor::getInstance().isVisible(alarmType);
		if (!isVisible) return NOT_VISIBLE_DEFAULT_STRING; // ignore the submission and return not visible string

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
			false,
            mmsState);

		// store the uuid and return it at the end
		std::string uuid( alarm->alarmID.in() );

		// Create the new queue item
		AlarmHelperBase::AlarmQueueItem* queueItem = new AlarmHelperBase::AlarmQueueItem();
		queueItem->entityKey = entityKey;
		std::stringstream stringStream;
		stringStream << alarmType.getTypeKey();
		stringStream >> queueItem->alarmTypeKey;
		queueItem->operation = SubmitAlarmNonUnique;
		queueItem->alarm = alarm;
		queueItem->locationKey = locationKey;

		// Finally, submit the alarm
		m_alarmHelperBase.addAlarmQueueItem(queueItem);

		return uuid;
	}

	void NonUniqueAlarmHelper::closeAlarm(const std::string& alarmID,
										const std::string& closeValue /*= ""*/,
										const timeb closeTime /*= AlarmConstants::defaultTime*/)
	{
		TA_ASSERT(!alarmID.empty(), "EntityKey for alarm to submit was 0");

		// TD 15195 Check whether this alarm ID is visible
		if (alarmID==NOT_VISIBLE_DEFAULT_STRING) return; // ignore the submission
		
		AlarmHelperBase::AlarmQueueItem* queueItem = new AlarmHelperBase::AlarmQueueItem();
		queueItem->entityKey = 0;
		queueItem->alarmTypeKey = 0;
		queueItem->alarm = NULL;
		queueItem->operation = CloseAlarmNonUnique;
		queueItem->alarmID = alarmID;
		queueItem->locationKey = 0;
		queueItem->closeValue = closeValue;
		queueItem->closeTime = closeTime.time;

		// Add it to the queue alarm
		m_alarmHelperBase.addAlarmQueueItem(queueItem);
	}

	void NonUniqueAlarmHelper::closeAlarmAtLocation(const std::string& alarmID,
													unsigned long locationKey,
													const std::string& closeValue /*= ""*/,
													const timeb closeTime /*= AlarmConstants::defaultTime*/)
	{
		TA_ASSERT(!alarmID.empty(), "EntityKey for alarm to submit was 0");

		// TD 15195 Check whether this alarm ID is visible
		if (alarmID==NOT_VISIBLE_DEFAULT_STRING) return; // ignore the submission
		
		AlarmHelperBase::AlarmQueueItem* queueItem = new AlarmHelperBase::AlarmQueueItem();
		queueItem->entityKey = 0;
		queueItem->alarmTypeKey = 0;
		queueItem->alarm = NULL;
		queueItem->operation = CloseAlarmNonUniqueAtLocation;
		queueItem->alarmID = alarmID;
		queueItem->locationKey = locationKey;
		queueItem->closeValue = closeValue;
		queueItem->closeTime = closeTime.time;

		// Add it to the queue alarm
		m_alarmHelperBase.addAlarmQueueItem(queueItem);
	}

	AlarmDetailCorbaDef* NonUniqueAlarmHelper::getAlarmAtLocation(const std::string& alarmID,unsigned long locationKey)
	{
		// TD 15195 Check whether this alarm ID is visible
		if (alarmID==NOT_VISIBLE_DEFAULT_STRING)
		{
			// ignore the submission & throw an exception
			TA_Base_Core::AlarmAgentCorbaDef::AlarmException ae;
            ae.reason = CORBA::string_dup("Alarm is not visible and cannot be found");
            throw(ae);
		}

		//get the agent at the specified location
        AlarmHelperBase::AlarmAgentNamedObject* agent = m_alarmHelperBase.getAlarmAgentAtLocation(locationKey);
        
		//if you did not retrieve the agent throw NoAlarmAgentException
        if (agent == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find the alarm agent at location %d", locationKey);
            TA_THROW( NoAlarmAgentException("Could not resolve the Alarm Agent.") );
        }

		//try and get the alarm
        try
        {
			AlarmDetailCorbaDef* details = NULL;
			CORBA_CALL_RETURN(details, (*agent), getAlarm, (alarmID.c_str() ));
            //AlarmDetailCorbaDef* details = (*agent)->getAlarm(alarmID.c_str() );
			//happy days, return the alarm
            return details;
        }
        catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ex) //just rethrow this
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::AlarmAgentCorbaDef::AlarmException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			throw;
        }
        catch(...) //translate unk exception type to NoAlarmAgentException
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not retrieve the alarm");
			TA_THROW( NoAlarmAgentException("Could not retrieve the alarm.") );
        }
	}

	AlarmDetailCorbaDef* NonUniqueAlarmHelper::getMostRecentActiveAlarmForEntity( unsigned long entityKey, 
			unsigned long locationKey )
	{
		return m_alarmHelperBase.getMostRecentActiveAlarmForEntity( entityKey, locationKey );
	}
}; // namespace TA_Base_Core

