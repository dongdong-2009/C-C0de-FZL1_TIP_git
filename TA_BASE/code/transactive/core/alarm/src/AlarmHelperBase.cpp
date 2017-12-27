/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmHelperBase.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <time.h>
#include <sstream>
#include "core/message/src/AuditMessageSender.h"
#include "core/alarm/src/AlarmHelperBase.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/message/types/AlarmAudit_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AlarmAgentEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"
#include "core/message/src/MessageConfig.h"
#include "core/alarm/src/AlarmConfig.h"
#include "core/exceptions/src/NoAlarmAgentException.h"
//TD15302 Mintao++
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/MMSComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/IDL/src/EntityStatusUpdateDataCorbaDef.h"

namespace TA_Base_Core
{

	AlarmHelperBase::AlarmHelperBase()
		: m_auditMessageSender( NULL ), m_terminated( true ), m_queueSemaphore( 0 ), m_initialised( false )
	{
		// Resolve the AuditMessageSender
		m_auditMessageSender =
				TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
					TA_Base_Core::AlarmAudit::Context);

		TA_ASSERT(NULL != m_auditMessageSender, "AuditMessageSender was NULL in AlarmHelperBase");
        //TD15302 Mintao++
        subscribeToEntityMMSInhibitUpdateMsg();
        m_inhibitAlarmMMSEntityMap.clear();
        //TD15302 Mintao++
	}


	AlarmHelperBase::~AlarmHelperBase()
	{
		AlarmAgents::iterator it = m_alarmAgents.begin();

		// Clean up all the NamedObjects
		for ( ; it != m_alarmAgents.end(); it++ )
		{
			// Clean up
			delete it->second;
			it->second = NULL;
		}

		// Clean up the message sender
		if ( NULL != m_auditMessageSender )
		{
			delete m_auditMessageSender;
			m_auditMessageSender = NULL;
		}
        //TD15302 Mintao++
        unsubscribeToEntityMMSInhibitUpdateMsg();
        //TD15302 Mintao++
	}

    //TD15302 Mintao++
    bool AlarmHelperBase::checkSystemAlarmMMSInhibition(unsigned long entityKey)
    {
		LOG_GENERIC( SourceInfo,
		        TA_Base_Core::DebugUtil::DebugInfo,
		        "AlarmHelperBase::checkSystemAlarmMMSInhibition");
		// guard the queue
		TA_Base_Core::ThreadGuard guard(m_inhibitAlarmMMSEntityMapLock);
        bool toReturn = false;
        std::map<unsigned long, bool>::iterator itr;
        itr = m_inhibitAlarmMMSEntityMap.find(entityKey);
        if (itr != m_inhibitAlarmMMSEntityMap.end())
        {
            toReturn = itr->second;
        }
		else//can not locate in the map, search the database
		{
			try
			{
				IEntityData* entityData = EntityAccessFactory::getInstance().getEntity(entityKey);
				EntityStatusData statusData = entityData->getEntityStatusData(MMSInhibitStatus);
                toReturn = entityData->getBoolEntityStatusValue(MMSInhibitStatus);
				if (toReturn)
				{
                    m_inhibitAlarmMMSEntityMap.insert(std::pair<unsigned long, bool>(entityKey, toReturn));
				}
			}
			catch (...) 
			{
		        LOG_GENERIC( SourceInfo,
			        TA_Base_Core::DebugUtil::DebugWarn,
			        "failed at checkSystemAlarmMMSInhibition");
			}			
		}
        return toReturn;
    }

    void AlarmHelperBase::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
		LOG_GENERIC( SourceInfo,
			        TA_Base_Core::DebugUtil::DebugInfo,
			        "AlarmHelperBase::receiveSpecialisedMessage");
		TA_Base_Core::EntityStatusUpdateCorbaDef* entityStatusCorbaData = NULL;

        std::string messageTypeKey(message.messageTypeKey);

		try
		{	
			if ( (message.messageState >>= entityStatusCorbaData) == 0 )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
					"Failed to interpret message");
				return;
			}

			unsigned long entitykey = 0; 
			if (NULL != entityStatusCorbaData)
			{
				entitykey = entityStatusCorbaData->entityKey;        
				bool action;
				if (messageTypeKey == TA_Base_Core::MMSComms::NonPhysicalEntityMMSAlarmInhibitApplied.getTypeKey())
				{
					action = true;
				}
				else if (messageTypeKey == TA_Base_Core::MMSComms::NonPhysicalEntityMMSAlarmInhibitRemoved.getTypeKey())
				{
					action = false;
				}
				else
				{
					LOG_GENERIC( SourceInfo,
						TA_Base_Core::DebugUtil::DebugDebug,
						"receive unknown status update message, ignore");
					return;
				}
		
				updateInhibitAlarmMMSEntityMap(entitykey, action);
			}
			else
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
					"Failed to retrieve EntityStatusUpdateCorbaDef data");
			}
		}
		catch (...)
		{
		 
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, 
				"Failed to process EntityStatusUpdateCorbaDef");
		}
    }

    void AlarmHelperBase::subscribeToEntityMMSInhibitUpdateMsg()
    {
        //MessageSubscriptionManager &manager = MessageSubscriptionManager::getInstance();
        MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(MMSComms::Context, this);
    }

    void AlarmHelperBase::updateInhibitAlarmMMSEntityMap(unsigned long entitykey, bool action)
    {
		TA_Base_Core::ThreadGuard guard(m_inhibitAlarmMMSEntityMapLock);
        std::map<unsigned long, bool>::iterator itr;
        itr = m_inhibitAlarmMMSEntityMap.find(entitykey);
        if (itr == m_inhibitAlarmMMSEntityMap.end())
        {
		    LOG_GENERIC( SourceInfo,
			    TA_Base_Core::DebugUtil::DebugDebug,
			    "receive MMS inhibit status for entity : %lu but not interested ", entitykey );
        }
        else
        {
            itr->second = action;
            std::string updatetypestr = "false";
            if (action)
            {
                updatetypestr = "true";
            }
		    LOG_GENERIC( SourceInfo,
			    TA_Base_Core::DebugUtil::DebugDebug,
			    "receive MMS inhibit status for entity : %lu and update to %s", entitykey, updatetypestr.c_str());
        }
    }

    void AlarmHelperBase::unsubscribeToEntityMMSInhibitUpdateMsg()
    {
        //MessageSubscriptionManager &manager = MessageSubscriptionManager::getInstance();
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }
    //TD15302 Mintao++

	AlarmDetailCorbaDef* AlarmHelperBase::createAlarmWithParams( const TA_Base_Core::MessageType& alarmType,
			unsigned long entityKey,
			unsigned long entityTypeKey,
			const TA_Base_Core::DescriptionParameters& description,
			const std::string& entityName, 
			unsigned long locationKey, 
			unsigned long subsystemKey,
			unsigned long alarmSeverityKey,
			timeb sourceTime,
			const std::string& assetName,
            bool isUnique /*=true*/, 
			TA_Base_Core::MmsStateType mmsState /*=MMS_NONE*/)
	{
		// Create the AlarmDetail object
		AlarmDetailCorbaDef* alarm = new AlarmDetailCorbaDef();

		// Fill in all the necessary default fields
		alarm->alarmAcknowledgedBy = CORBA::string_dup("");
		alarm->alarmID = CORBA::string_dup( TA_Uuid::generateUUID().c_str() );
		alarm->state = AlarmOpen;
		alarm->transactiveTime.time = 0;
        alarm->transactiveTime.millitm = 0;
		alarm->ackTime = 0;
        alarm->closeTime = 0;
		alarm->alarmComments = CORBA::string_dup("");
		alarm->parentAlarmID = CORBA::string_dup("");
		alarm->avalancheHeadID = CORBA::string_dup("");

		// Fill in the provided fields
		// Wenguang TD18266
		std::stringstream stringStream;
		stringStream << alarmType.getTypeKey();
		stringStream >> alarm->messageTypeKey;
		alarm->messageContext = CORBA::string_dup( alarmType.getContextObj().getContextName().c_str() );
		alarm->assocEntityKey = entityKey;
		alarm->assocEntityTypeKey = entityTypeKey;
		//alarm->entityName = CORBA::string_dup(entityName.c_str());
		// Wenguang++ Alarm Message Size Reduction
		alarm->entityName = CORBA::string_dup("");
		alarm->locationKey = locationKey;
		alarm->sourceTime.time = sourceTime.time;
        alarm->sourceTime.millitm = sourceTime.millitm;

		// Fill in the display time as the source time upon initialisation
		alarm->displayTime = alarm->sourceTime;

		alarm->subsystemKey = subsystemKey;
		alarm->alarmSeverity = alarmSeverityKey;
		alarm->assetName = CORBA::string_dup(assetName.c_str());
		alarm->messageSequenceNumber = 0;  // Used by agent notifications only
		
		// Get the alarm parameters
		std::string alarmParameters;
		gGetStringFromParameters( description, alarmParameters );
		alarm->alarmParameterList = CORBA::string_dup( alarmParameters.c_str() );
		alarm->alarmValue = CORBA::string_dup( getValueFromParameters( alarmType, description ).c_str() );

		// Create the alarm description
		//alarm->alarmDescription = CORBA::string_dup(
		//	AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarmType, description ).c_str() );

		// Wenguang++ Alarm Message Size Reduction
		alarm->alarmDescription = CORBA::string_dup("");

        alarm->isHeadOfAvalanche = CORBA::Boolean( false );
		alarm->isChildOfAvalanche = CORBA::Boolean( false );
		alarm->isUniqueAlarm = CORBA::Boolean( isUnique );

        alarm->decisionSupportState = TA_Base_Core::NoRelatedPlan;
        //Mintao++ TD15302
        if (AlarmTypeTableAccessor::getInstance().isSystemAlarm(alarmType))
        {
            if (AlarmTypeTableAccessor::getInstance().isMMSEnabled(alarmType))
            {
                if (checkSystemAlarmMMSInhibition(entityKey))
                {
                    alarm->mmsState = TA_Base_Core::MMS_INHIBIT;                        
                }
                else
                {
                    //alarm->mmsState = TA_Base_Core::MMS_AUTO;                        
					//TD17862 marc_bugfix support other mms types for System alarms (semi-automatic, none)
					 switch(AlarmTypeTableAccessor::getInstance().getRelatedMMSType(alarmType)) 
					 {
					 case  IAlarmData::TYPE_NONE:
						LOG( SourceInfo,
						TA_Base_Core::DebugUtil::GenericLog,
						TA_Base_Core::DebugUtil::DebugInfo,
						"marc>>current alarm is mms none ");

						 alarm->mmsState = TA_Base_Core::MMS_NONE;
						 break;
					 case IAlarmData::TYPE_SEMI:
						 alarm->mmsState = TA_Base_Core::MMS_SEMI;
						LOG( SourceInfo,
						TA_Base_Core::DebugUtil::GenericLog,
						TA_Base_Core::DebugUtil::DebugInfo,
						"marc>>current alarm is mms semi ");

						 break;
					 case IAlarmData::TYPE_AUTO:
					 default:
						LOG( SourceInfo,
						TA_Base_Core::DebugUtil::GenericLog,
						TA_Base_Core::DebugUtil::DebugInfo,
						"marc>>current alarm is mms auto  ");

						 alarm->mmsState = TA_Base_Core::MMS_AUTO;
						 break;
					 }
                }
            }
			else
			{
				alarm->mmsState = mmsState;
			}
        }
        else
        {
		    alarm->mmsState = mmsState;
        }
        //Mintao++ TD15302

		return alarm;
	}

	std::string AlarmHelperBase::getValueFromParameters(const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& description)
	{
		std::string value("");
		
		// check whether it is system alarm return ALARM if yes		
		bool isSystem = AlarmTypeTableAccessor::getInstance().isSystemAlarm(type);
		std::string alarmType = AlarmTypeTableAccessor::getInstance().getAlarmTypeName(type);


		if (isSystem)
		{
			// Obtain the value from DB configuration
			// Attribute's name = "AlarmValue" in alarm type data
			value = AlarmTypeTableAccessor::getInstance().getAlarmValue(type);
		}
		else // this is not system alarm and can be a datapoint type
		{        
			TA_Base_Core::ConstDescriptionParametersIterator valueParam;
			bool found=false;
			for (valueParam=description.begin(); valueParam!=description.end(); ++valueParam)
			{
				if ( (*valueParam)->name.compare("DataPointValue") == 0)
				{
					found = true;
					break;
				}
			}

			//DTL-1719 -- remove unnecessary code and just retrieve alarm value from database
			value = AlarmTypeTableAccessor::getInstance().getAlarmValue(type);
			// TD 12785 Only display the value without the alarm type if there is parameter value
			// if not, use the alarm type description
			if(found==true && valueParam != description.end())
			{
				if (value.empty())
					value += (*valueParam)->value;
				else
					value += " " + (*valueParam)->value;
			}
		}

		return value;
	}


	AlarmSequence* AlarmHelperBase::getActiveAlarms(unsigned long locationKey )
	{
		// Get the agent
		AlarmAgentNamedObject* alarmAgent = getAlarmAgentAtLocation( locationKey );
		
		if ( NULL == alarmAgent )
		{
			char buf[128] = {0};
			sprintf( buf, 
				"Unable to locate AlarmAgent for locationKey specified: %li",
				locationKey);
			
			TA_THROW( NoAlarmAgentException( buf ) ); 
		}
		
		// Perform the function call
		TA_Base_Core::AlarmSequence* alarmList = NULL;
		LOG( SourceInfo,	TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo, "call to getActiveAlarms begin %d",locationKey );
        CORBA_CALL_RETURN( alarmList, (*alarmAgent), getEntityAlarms, (0) );
		LOG( SourceInfo,	TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo, "call to getActiveAlarms end" );

		if (alarmList==NULL)
		{
		   TA_THROW(TA_Base_Core::TransactiveException("Retreve active alarm failed") );
		}
        return alarmList;
	}
	AlarmDetailCorbaDef* AlarmHelperBase::getMostRecentActiveAlarmForEntity( unsigned long entityKey, 
			unsigned long locationKey )
	{
		// Get the agent
		AlarmAgentNamedObject* alarmAgent = getAlarmAgentAtLocation( locationKey );

		if ( NULL == alarmAgent )
		{
			char buf[128] = {0};
			sprintf( buf, 
				"Unable to locate AlarmAgent for locationKey specified: %li",
				locationKey );

			TA_THROW( NoAlarmAgentException( buf ) ); 
		}

		// Perform the function call
        AlarmDetailCorbaDef* result = NULL;
        CORBA_CALL_RETURN( result, (*alarmAgent), getMostRecentActiveAlarmForEntity, ( entityKey ) );
		return result;
	}

	AlarmDetailCorbaDef* AlarmHelperBase::getActiveAlarmForEntityOfType(unsigned long alarmTypeKey, 
		unsigned long entityKey, unsigned long locationKey )
	{
		// Get the agent
		AlarmAgentNamedObject* alarmAgent = getAlarmAgentAtLocation( locationKey );

		if ( NULL == alarmAgent )
		{
			char buf[128] = {0};
			sprintf( buf, 
				"Unable to locate AlarmAgent for locationKey specified: %li",
				locationKey );

			TA_THROW( NoAlarmAgentException( buf ) ); 
		}

		// Perform the function call
        AlarmDetailCorbaDef* result = NULL;
        CORBA_CALL_RETURN( result, (*alarmAgent), getActiveAlarmForEntityOfType, ( alarmTypeKey, entityKey ) );
        return result;
	}

	void AlarmHelperBase::addAlarmQueueItem(AlarmQueueItem* alarmQueueItem)
	{
		TA_ASSERT(NULL != alarmQueueItem, "AlarmQueueItem was NULL");

		// guard the queue
		TA_Base_Core::ThreadGuard guard(m_queueLock);

		m_alarmItemQueue.push( alarmQueueItem );

		LOG_GENERIC( SourceInfo,
			TA_Base_Core::DebugUtil::DebugInfo,
			"AlarmQueueItem added. Queue size: %lu",
			m_alarmItemQueue.size() );

		// Notify the waiting thread
		m_queueSemaphore.post();
	}

	bool AlarmHelperBase::submitQueueItemAtLocation( AlarmQueueItem* alarmQueueItem )
	{
		// Only get the agent we need

		AlarmAgentNamedObject* alarmAgent = getAlarmAgentAtLocation( alarmQueueItem->locationKey );	
		
		if ( NULL == alarmAgent )
		{
			// We can safely log the locationKey here as there will
			// always be an agent for a close/ack (FOR NOW!!)
			LOG( SourceInfo,
				TA_Base_Core::DebugUtil::GenericLog,
				TA_Base_Core::DebugUtil::DebugError,
				"Unable to locate AlarmAgent for locationKey specified: %lu. Discarding alarm.",
				alarmQueueItem->locationKey );

			return true;
		}

		// submit the queue item
		try
		{
			std::stringstream alarmDetail;

			alarmDetail << "AlarmOperation : ";
			switch(alarmQueueItem->operation)
			{
			case SubmitAlarmUnique:
				alarmDetail << "SubmitAlarmUnique;" << std::endl;
				break;
			case SubmitAlarmNonUnique:
				alarmDetail << "SubmitAlarmNonUnique;" << std::endl;
				break;
			case CloseAlarmUnique:
				alarmDetail << "CloseAlarmUnique;" << std::endl;
				break;
			case CloseAlarmNonUnique:
				alarmDetail << "CloseAlarmNonUnique;" << std::endl;
				break;
			case CloseAlarmUniqueAtLocation:
				alarmDetail << "CloseAlarmUniqueAtLocation;" << std::endl;
				break;
			case CloseAlarmNonUniqueAtLocation:
				alarmDetail << "CloseAlarmNonUniqueAtLocation;" << std::endl;
				break;
			default:
				alarmDetail << "unknown AlarmOperation;" << std::endl;
			}

			alarmDetail << "alarmID : " << alarmQueueItem->alarmID << std::endl;
			alarmDetail << "entityKey : " << alarmQueueItem->entityKey << std::endl;
			alarmDetail << "alarmTypeKey : " << alarmQueueItem->alarmTypeKey << std::endl;
			alarmDetail << "locationKey : " << alarmQueueItem->locationKey << std::endl;
			alarmDetail << "closeValue : " << alarmQueueItem->closeValue << std::endl;
			alarmDetail << "retryTime : " << alarmQueueItem->retryTime << std::endl;
			

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "the alarm item is : %s", alarmDetail.str().c_str());


			alarmDetail.str("");

			if (alarmQueueItem->alarm == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "the alarm item's detail is NULL");
			}
			else
			{
				alarmDetail << "sourceTime : time " << alarmQueueItem->alarm->sourceTime.time << "ms " << alarmQueueItem->alarm->sourceTime.millitm << std::endl;
				alarmDetail << "transactiveTime : time " << alarmQueueItem->alarm->transactiveTime.time << "ms " << alarmQueueItem->alarm->transactiveTime.millitm << std::endl;
				alarmDetail << "ackTime : " << alarmQueueItem->alarm->ackTime << std::endl;
				alarmDetail << "closeTime : " << alarmQueueItem->alarm->closeTime << std::endl;			
				alarmDetail << "displayTime : time " << alarmQueueItem->alarm->displayTime.time << "ms " << alarmQueueItem->alarm->displayTime.millitm << std::endl;
				alarmDetail << "messageTypeKey : " << alarmQueueItem->alarm->messageTypeKey << std::endl;
				alarmDetail << "messageContext : " << alarmQueueItem->alarm->messageContext << std::endl;
				alarmDetail << "assocEntityKey : " << alarmQueueItem->alarm->assocEntityKey << std::endl;			
				alarmDetail << "assocEntityTypeKey : " << alarmQueueItem->alarm->assocEntityTypeKey << std::endl;
				alarmDetail << "assetName : " << alarmQueueItem->alarm->assetName << std::endl;			
				alarmDetail << "alarmID : " << alarmQueueItem->alarm->alarmID << std::endl;
				alarmDetail << "alarmSeverity : " << alarmQueueItem->alarm->alarmSeverity << std::endl;
				alarmDetail << "alarmDescription : " << alarmQueueItem->alarm->alarmDescription << std::endl;			
				alarmDetail << "alarmParameterList : " << alarmQueueItem->alarm->alarmParameterList << std::endl;
				alarmDetail << "alarmAcknowledgedBy : " << alarmQueueItem->alarm->alarmAcknowledgedBy << std::endl;
				
				alarmDetail << "AlarmState : ";
				switch(alarmQueueItem->alarm->state)
				{
				case TA_Base_Core::AlarmClosed:
					alarmDetail << "AlarmClosed;" << std::endl;
					break;
				case TA_Base_Core::AlarmOpen:
					alarmDetail << "AlarmOpen;" << std::endl;
					break;
				case TA_Base_Core::AlarmSuppressed:
					alarmDetail << "CloseAlarmUnique;" << std::endl;
					break;
				default:
					alarmDetail << "unknown AlarmState;" << std::endl;
				}
				
				alarmDetail << "alarmComments : " << alarmQueueItem->alarm->alarmComments << std::endl;
				alarmDetail << "entityName : " << alarmQueueItem->alarm->entityName << std::endl;
				alarmDetail << "locationKey : " << alarmQueueItem->alarm->locationKey << std::endl;			
				alarmDetail << "subsystemKey : " << alarmQueueItem->alarm->subsystemKey << std::endl;
				alarmDetail << "parentAlarmID : " << alarmQueueItem->alarm->parentAlarmID << std::endl;
				

				alarmDetail << "avalancheHeadID : " << alarmQueueItem->alarm->avalancheHeadID << std::endl;
				
				alarmDetail << "isHeadOfAvalanche : " << (alarmQueueItem->alarm->isHeadOfAvalanche == 0 ? "false" : "true") << std::endl;
				alarmDetail << "isChildOfAvalanche : " << (alarmQueueItem->alarm->isChildOfAvalanche == 0 ? "false" : "true") << std::endl;			
				alarmDetail << "isUniqueAlarm : " << (alarmQueueItem->alarm->isUniqueAlarm == 0 ? "false" : "true") << std::endl;

				alarmDetail << "decisionSupportState : ";
				switch(alarmQueueItem->alarm->decisionSupportState)
				{
				case TA_Base_Core::NoRelatedPlan:
					alarmDetail << "NoRelatedPlan;" << std::endl;
					break;
				case TA_Base_Core::RelatedAutomaticPlanNotRun:
					alarmDetail << "RelatedAutomaticPlanNotRun;" << std::endl;
					break;
				case TA_Base_Core::RelatedSemiAutomaticPlanNotRun:
					alarmDetail << "RelatedSemiAutomaticPlanNotRun;" << std::endl;
					break;
				case TA_Base_Core::RelatedPlanHasBeenRun:
					alarmDetail << "RelatedPlanHasBeenRun;" << std::endl;
					break;
				case TA_Base_Core::RelatedPlanHasBeenPostponed:
					alarmDetail << "RelatedPlanHasBeenPostponed;" << std::endl;
					break;
				default:
					alarmDetail << "unknown DecisionSupportStateType;" << std::endl;
				}

				alarmDetail << "mmsState : ";
				switch(alarmQueueItem->alarm->mmsState)
				{
				case TA_Base_Core::MMS_NONE:
					alarmDetail << "MMS_NONE;" << std::endl;
					break;
				case TA_Base_Core::MMS_SEMI:
					alarmDetail << "MMS_SEMI;" << std::endl;
					break;
				case TA_Base_Core::MMS_AUTO:
					alarmDetail << "MMS_AUTO;" << std::endl;
					break;
				case TA_Base_Core::MMS_JR_SENT:
					alarmDetail << "MMS_JR_SENT;" << std::endl;
					break;
				case TA_Base_Core::MMS_ALM_SENT:
					alarmDetail << "MMS_ALM_SENT;" << std::endl;
					break;
				case TA_Base_Core::MMS_INHIBIT:
					alarmDetail << "MMS_INHIBIT;" << std::endl;
					break;
				case TA_Base_Core::MMS_FAILED:
					alarmDetail << "MMS_FAILED;" << std::endl;
					break;
				default:
					alarmDetail << "unknown MmsStateType;" << std::endl;
				}

				
				alarmDetail << "messageSequenceNumber : " << alarmQueueItem->alarm->messageSequenceNumber << std::endl;
				alarmDetail << "alarmValue : " << alarmQueueItem->alarm->alarmValue << std::endl;

				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "the alarm item's detail is : %s", alarmDetail.str().c_str());
			}

			// Get the alarm
			AlarmDetailCorbaDef* alarm = alarmQueueItem->alarm;

			// Determine the operation
			switch ( alarmQueueItem->operation )
			{
			case CloseAlarmUniqueAtLocation:
				{
                    CORBA_CALL( (*alarmAgent),
                                closeAlarmByTypeAndKey,
                                ( alarmQueueItem->alarmTypeKey,
                                  alarmQueueItem->entityKey, 
                                  alarmQueueItem->closeValue.c_str(),
						          alarmQueueItem->closeTime ) );

					LOG( SourceInfo,
						TA_Base_Core::DebugUtil::GenericLog,
						TA_Base_Core::DebugUtil::DebugInfo,
						"All Alarms with EntityKey %d and AlarmType %u closed with Value: %s",
						alarmQueueItem->entityKey,
						alarmQueueItem->alarmTypeKey,
						alarmQueueItem->closeValue.c_str() );
				}
				break;
			case CloseAlarmNonUniqueAtLocation:
				{
                    CORBA_CALL( (*alarmAgent),
                                closeAlarmById,
                                ( alarmQueueItem->alarmID.c_str(),
                                  alarmQueueItem->closeValue.c_str(),
						          alarmQueueItem->closeTime ) );

					LOG( SourceInfo,
						TA_Base_Core::DebugUtil::GenericLog,
						TA_Base_Core::DebugUtil::DebugInfo,
						"Alarm with ID %s of Value: %s is closed",
						alarmQueueItem->alarmID.c_str(),
						alarmQueueItem->closeValue.c_str() );
				}
				break;
			case SubmitAlarmUnique:
				{
                    CORBA_CALL( (*alarmAgent),
                                submitAlarm,
                                ( CORBA::Boolean(false), *alarm ) );

					LOG(SourceInfo,
						TA_Base_Core::DebugUtil::GenericLog,
						TA_Base_Core::DebugUtil::DebugInfo,
						"Unique Alarm submitted with ID=%s",
						alarm->alarmID.in() );
				}
				break;
			case SubmitAlarmNonUnique:
				{
                    CORBA_CALL( (*alarmAgent),
                                submitAlarm,
                                ( CORBA::Boolean(true), *alarm ) );

					LOG(SourceInfo,
						TA_Base_Core::DebugUtil::GenericLog,
						TA_Base_Core::DebugUtil::DebugInfo,
						"Non Unique Alarm submitted with ID=%s",
						alarm->alarmID.in() );
				}
				break;

			case CloseAlarmUnique:
				break;
			case CloseAlarmNonUnique:
				break;
			}
		}
		catch ( const TA_Base_Core::AlarmAgentCorbaDef::AlarmException& aex )
		{
			LOG(SourceInfo,
				TA_Base_Core::DebugUtil::ExceptionCatch,
				"AlarmAgentCorbaDef::AlarmException - discarding AlarmQueueItem",
				aex.reason.in() );

			//discard the queue item
			return true;
		}
        catch ( const AlarmAgentCorbaDef::AlarmCapacityExceededException& /*cee*/)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"AlarmAgentCorbaDef::AlarmCapacityExceededException",
                "Alarm agent past capacity for new alarms, keeping AlarmQueueItem");

            // keep the queue item
            return false;
        }
		catch ( const TA_Base_Core::OperationModeException& omx )
		{
			LOG( SourceInfo,
				TA_Base_Core::DebugUtil::ExceptionCatch,
				"AlarmAgentCorbaDef::OperationModeException, keepting AlarmQueueItem",
				omx.reason.in() );

			//keep the queue item
			return false;
		}
		catch ( const TA_Base_Core::ObjectResolutionException& orex)
		{
			LOG(SourceInfo,
				TA_Base_Core::DebugUtil::ExceptionCatch,
				"ObjectResolutionException - keeping AlarmQueueItem",
				orex.what());

			// unable to get to the AlarmAgent
			return false;
		}
		catch ( const CORBA::Exception& corbaex)
		{
			LOG(SourceInfo,
				TA_Base_Core::DebugUtil::ExceptionCatch,
				"Corba::Exception - keeping AlarmQueueItem",
				CorbaUtil::getInstance().exceptionToString(corbaex).c_str());

			// unable to get to the AlarmAgent
			return false;
		}
		catch ( ... )
		{
			LOG(SourceInfo,
				TA_Base_Core::DebugUtil::ExceptionCatch,
				"UnknownException", "Keeping AlarmQueueItem");

			// unable to get to the AlarmAgent
			return false;
		}

		// The call was successful
		return true;
	}


	bool AlarmHelperBase::submitQueueItem( AlarmQueueItem* alarmQueueItem )
	{
		// Load the alarm agents
		loadAlarmAgents();

		if ( m_alarmAgents.empty() )
		{
			// We can safely log the locationKey here as there will
			// always be an agent for a close/ack (FOR NOW!!)
			LOG( SourceInfo,
				TA_Base_Core::DebugUtil::GenericLog,
				TA_Base_Core::DebugUtil::DebugError,
				"Unable to locate AlarmAgent for locationKey specified: %lu. Discarding alarm.",
				alarmQueueItem->locationKey );

			return true;
		}

		// If this item has not been tried before, 
		if ( alarmQueueItem->retryTime == 0 )
		{
			// Add all the agents to the list to try			
			AlarmAgents::iterator it = m_alarmAgents.begin();
			for ( ; it != m_alarmAgents.end(); it++ )
			{
				alarmQueueItem->agentsLeftToTry.push_back( it->second );
			}
		}

		// Attempt to close the item on each alarm agent
		AlarmAgentList::iterator agentIt = alarmQueueItem->agentsLeftToTry.begin();
		while ( agentIt != alarmQueueItem->agentsLeftToTry.end() )
		{
			try
			{
				// get the Agent
				AlarmAgentNamedObject* alarmAgent = *agentIt;

				// Determine the operation
				switch ( alarmQueueItem->operation )
				{
				case CloseAlarmUnique:
					{
                        CORBA_CALL( (*alarmAgent),
                                    closeAlarmByTypeAndKey,
                                    ( alarmQueueItem->alarmTypeKey,
                                      alarmQueueItem->entityKey,
                                      alarmQueueItem->closeValue.c_str(),
                                      alarmQueueItem->closeTime ) );

						LOG( SourceInfo,
							TA_Base_Core::DebugUtil::GenericLog,
							TA_Base_Core::DebugUtil::DebugInfo,
							"All Alarms with EntityKey %d and AlarmType %u closed with Value: %s",
							alarmQueueItem->entityKey,
							alarmQueueItem->alarmTypeKey,
							alarmQueueItem->closeValue.c_str() );
					}
					break;
				case CloseAlarmNonUnique:
					{
                        CORBA_CALL( (*alarmAgent),
                                    closeAlarmById,
                                    ( alarmQueueItem->alarmID.c_str(),
                                      alarmQueueItem->closeValue.c_str(),
							          alarmQueueItem->closeTime ) );

						LOG( SourceInfo,
							TA_Base_Core::DebugUtil::GenericLog,
							TA_Base_Core::DebugUtil::DebugInfo,
							"Alarm with ID %s of Value: %s is closed",
							alarmQueueItem->alarmID.c_str(),
							alarmQueueItem->closeValue.c_str() );
					}
					break;
				}

				// if we get here, we can remove the current agent from the list 
				// because the call succeeded
				agentIt = alarmQueueItem->agentsLeftToTry.erase( agentIt );
			}
			catch ( const TA_Base_Core::OperationModeException& omx )
			{
				LOG( SourceInfo,
					TA_Base_Core::DebugUtil::ExceptionCatch,
					"AlarmAgentCorbaDef::OperationModeException, keepting AlarmQueueItem",
					omx.reason.in() );

				// increment the iterator but keep the agent in the list to try
				agentIt++;
			}
			catch ( const TA_Base_Core::ObjectResolutionException& orex)
			{
				LOG(SourceInfo,
					TA_Base_Core::DebugUtil::ExceptionCatch,
					"ObjectResolutionException - keeping AlarmQueueItem",
					orex.what());

				// increment the iterator but keep the agent in the list to try
				agentIt++;
			}
			catch ( const CORBA::Exception& corbaex)
			{
				LOG(SourceInfo,
					TA_Base_Core::DebugUtil::ExceptionCatch,
					"Corba::Exception - keeping AlarmQueueItem",
					CorbaUtil::getInstance().exceptionToString(corbaex).c_str());

				// increment the iterator but keep the agent in the list to try
				agentIt++;
			}
			catch ( ... )
			{
				LOG(SourceInfo,
					TA_Base_Core::DebugUtil::ExceptionCatch,
					"UnknownException", "Keeping AlarmQueueItem");

				// increment the iterator but keep the agent in the list to try
				agentIt++;
			}
		}

		// If there are any agents left in the list we need to retry a little later on
		if ( ! alarmQueueItem->agentsLeftToTry.empty() )
		{
			// set the retry timer for 10 secs later
			alarmQueueItem->retryTime = time( NULL ) + 5;

			// Item was not successfully submitted
			return false;
		}

		// No agents left in the list, item successfully submitted
		return true;
	}


	void AlarmHelperBase::generateAuditMessage( const TA_Base_Core::MessageType& msgType,
		                                       const std::string& alarmType,
                                               const std::string& associatedEntityName,
                                               const std::string& alarmId,
											   unsigned long entityKey )
	{
		// See if there is a sessionID
		std::string sessionID("");
		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_SESSIONID))
		{
			sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		}

		TA_Base_Core::DescriptionParameters dp;
        TA_Base_Core::NameValuePair alarmTypeDescriptionPair("alarmType", alarmType);
        TA_Base_Core::NameValuePair entityNameDescriptionPair("entityName", associatedEntityName);

        dp.push_back(&alarmTypeDescriptionPair);
        dp.push_back(&entityNameDescriptionPair);

		m_auditMessageSender->sendAuditMessage(msgType,
			entityKey,
			dp,
			"",
			sessionID,
			alarmId,
			"",
			"",
			NULL);
	}


	AlarmHelperBase::AlarmAgentNamedObject* AlarmHelperBase::getAlarmAgentAtLocation( unsigned long locationKey )
	{		
		// Load the alarm agents, this will return if the function
		// has already been loaded
		loadAlarmAgents();

		// Lookup the location in the map
		AlarmAgents::iterator it = m_alarmAgents.find( locationKey );

		// If found then return the pointer
		if ( it != m_alarmAgents.end() )
		{
			return it->second;
		}
		
		return NULL;
	}


	void AlarmHelperBase::run()
	{
		m_terminated = false;

		while ( true )
		{
			// Wait for an alarm to arrive
			m_queueSemaphore.wait();

			// check to make sure we should still be running
			if ( m_terminated )
			{
				break;
			}
 
			if (getAlarmQueueSize() == 0)
			{
				continue;
			}
			// Get the item, it should never be null
			AlarmQueueItem* item = NULL;
			item = getAlarmQueueItem();
			TA_ASSERT( item != NULL, "AlarmQueueItem should never be NULL!" )

			// if the location must be greater than zero
			bool submitted( false );

			if ( ( item->locationKey > 0 ) )
			{
				// submit to individual agent
				submitted = submitQueueItemAtLocation( item );
			}
			else // This is a case of location key is either zero or negative ie. invalid
			{
				// According to business rule the location key should not be zero
				// and should not be submitted to all agents
				
				// Log the defect and inform invalid location but allow the transaction for time being
				std::stringstream str;
                str << "Location key is invalid. Submitted key is: " << item->locationKey;
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, str.str().c_str());

				//TA_ASSERT (item->locationKey > 0, str.str().c_str());

				// Set submitted to true for deletion of the alarm item and move on
				//submitted = true;

				
				// Make sure we don't try too soon
				if ( ( item->retryTime != 0 ) && ( item->retryTime > time( NULL ) ) )
				{
					{//Enclose in code block to avoid holding the lock during sleep
						// Throw it to the back of the queue, and wait a while
						// Guard the queue and pop off
						TA_Base_Core::ThreadGuard guard(m_queueLock);
						m_alarmItemQueue.pop();
						m_alarmItemQueue.push( item );

						// signal the queue semaphore to try again immediately
						m_queueSemaphore.post(); 
					}
					//addAlarmQueueItem(item);
					// Sleep for 100ms before the next attempt
					TA_Base_Core::Thread::sleep( 100 );

					// continue from here
					continue;
				}
				else
				{	
					// submit to all agents, we have a list of agents to try
					submitted = submitQueueItem( item );
				}
				
			}

			// If the item is not submitted.. what do we do.
			if ( ! submitted )
			{
				LOG_GENERIC( SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"AlarmQueueItem submission failed. Queue size: %lu",
					getAlarmQueueSize() );

				// If this is a close we can throw it to the back of the queue and 
				// process other stuff in the mean-time
				if ( ( item->operation != SubmitAlarmUnique ) &&
					 ( item->operation != SubmitAlarmNonUnique ) )
				{
					// throw it to the back of the queue
					// Guard the queue and pop off
					TA_Base_Core::ThreadGuard guard(m_queueLock);
					m_alarmItemQueue.pop();
					m_alarmItemQueue.push( item );

					// Sleep for 100ms before the next attempt as this item has been 
					// moved to the back of the queue
					// TA_Base_Core::Thread::sleep( 100 );
				}

				// signal the semaphore to try again
				m_queueSemaphore.post();

				// Sleep for 5000ms before the next attempt, as this item will stay at the front of the
				// list forever until the alarm agent is available
				TA_Base_Core::Thread::sleep( 5000 );
			}
			else
			{
				// Delete the alarm if it exists
				if ( NULL != item->alarm )
				{
					delete item->alarm;
				}

				// delete the item
				delete item;
				{ // Enclose in a code block to avoid lock access problem since we are using non-reentrant thread lock.
					// Guard the queue and pop off
					TA_Base_Core::ThreadGuard guard(m_queueLock);
					m_alarmItemQueue.pop();
				}
				LOG_GENERIC( SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"AlarmQueueItem submission succeeded. Queue size: %lu",
					getAlarmQueueSize() );
			}
		} // end of while loop

		if ( getAlarmQueueSize() != 0 )
		{
			LOG( SourceInfo,
				TA_Base_Core::DebugUtil::GenericLog,
				TA_Base_Core::DebugUtil::DebugError,
				"Discarding %lu un-submitted AlarmQueueItem(s)",
				getAlarmQueueSize() );

			deleteAllAlarmQueueItems();
		} // end of alarm item queue deletion
	}


	void AlarmHelperBase::terminate()
	{
		m_terminated = true;

		// Notify of terminate
		m_queueSemaphore.post();
	}


	void AlarmHelperBase::loadAlarmAgents( )
	{
		if ( m_initialised )
		{
			return;
		}

		// load the agents
		// If not found load the corba name and create the new NamedObject
		try
		{
			// Load the CorbaName from the database
			CorbaNameList names = 
				EntityAccessFactory::getInstance().getCorbaNamesOfType(
				AlarmAgentEntityData::getStaticType(),true );

			// Iterate through the names and store them for lookup
			CorbaNameList::iterator it = names.begin();
			for ( ; it != names.end(); it++ )
			{
				// We have loaded the NamedObject, so assign it at the location
				m_alarmAgents[ (*it).getLocationKey() ] = new AlarmAgentNamedObject( (*it) );
			}
		}
		catch ( ... )
		{
			LOG(SourceInfo,
				TA_Base_Core::DebugUtil::ExceptionCatch,
				"Unable to load list of AlarmAgent NamedObjects." );
		}

		m_initialised = true;
	}

	AlarmHelperBase::AlarmQueueItem* AlarmHelperBase::getAlarmQueueItem()
	{
		TA_Base_Core::ThreadGuard guard(m_queueLock);
		AlarmQueueItem* pQueue = NULL;
		if(!m_alarmItemQueue.empty())
		{
			pQueue = m_alarmItemQueue.front();
			//m_alarmItemQueue.pop();
		}

		return pQueue;
	}

	unsigned long AlarmHelperBase::getAlarmQueueSize()
	{
		TA_Base_Core::ThreadGuard guard(m_queueLock);
		unsigned long ulRetVal = 0;
		ulRetVal = m_alarmItemQueue.size();
		return ulRetVal;
	}

	void AlarmHelperBase::deleteAllAlarmQueueItems()
	{
		TA_Base_Core::ThreadGuard guard(m_queueLock);
		while (	!m_alarmItemQueue.empty())
		{
			AlarmQueueItem* item = m_alarmItemQueue.front();

			// Delete the alarm if it exists
			if ( NULL != item->alarm )
			{
				delete item->alarm;
			}

			// delete the item
			delete item;
			m_alarmItemQueue.pop();
		}
	}
}; // namespace TA_Base_Core

