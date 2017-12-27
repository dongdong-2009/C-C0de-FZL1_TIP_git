/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event_processing/src/RealTimeEventDataHelper.cpp $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by:  $Author: 
 * 
 * RealTimeEventDataHelper is a thread for 1).retreiving all the data for event 2)processing some data in event
 *
 */

#include "RealTimeEventDataHelper.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/Profile.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/Operator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/MessageTypeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/AlarmSeverityData.h"


using namespace std;
using namespace TA_Base_Core::EventConstants;
namespace TA_Base_Core
{

	RealTimeEventDataHelper* RealTimeEventDataHelper::m_instance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable RealTimeEventDataHelper::m_singletonLock;

	RealTimeEventDataHelper& RealTimeEventDataHelper::getInstance()
	{
		ThreadGuard guard(m_singletonLock);
		// Create a new instance if it doesn't exist.
		if ( 0 == m_instance )
		{
			m_instance = new RealTimeEventDataHelper();

		}

		return *m_instance;
	}

	void RealTimeEventDataHelper::releaseInstance()
	{
		ThreadGuard guard(m_singletonLock);
		if ( m_instance != 0)
		{
			delete m_instance;
			m_instance = NULL;
		}
		return;
	}

	RealTimeEventDataHelper::RealTimeEventDataHelper(void)
	{
		m_OCCLocationKey = 0;
		m_alarmSeverityLoaded = false;
		initMessageTypeData();
		initOperatorData();
		initOCCLoactionKey();
	}

	void RealTimeEventDataHelper::initOCCLoactionKey()
	{
		//SELECT pkey from LOCATION where name = 'OCC';  
		m_OCCLocationKey = LocationAccessFactory::getInstance().getOccLocationKey();
	}
	RealTimeEventDataHelper::~RealTimeEventDataHelper(void)
	{
		releaseOperatorCache();
		releaseMessageTypeCache();
		releaseAlarmSeverityCache();
	}

	void RealTimeEventDataHelper::releaseOperatorCache()
	{
		std::map< unsigned long, TA_Base_Core::IOperator* >::iterator it;
		for( it = m_operatorMap.begin(); it != m_operatorMap.end(); it++ )
		{
			delete it->second;
			it->second = NULL;
		}
	}

	void RealTimeEventDataHelper::releaseMessageTypeCache()
	{
		std::map< unsigned long, TA_Base_Core::MessageTypeEventData* >::iterator it;
		for( it = m_msgTypeMap.begin(); it != m_msgTypeMap.end(); it++ )
		{
			delete it->second;
			it->second = NULL;
		}
	}

	

	//In this function, we will update the "invalid value"(which means the client didn't supply but it should be some value)
	//to the meaningful value.
	void RealTimeEventDataHelper::prepareAllDataForEvent(EventItemEx * eventItem, ISessionHelper* sessionHelper)
	{
		EventItem* event = eventItem->m_event;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Begin prepareAllDataForEvent, eventId= %s ", event->eventID );
		string sourceTable = event->sourceTable;
		unsigned long entityKey = event->entitykey;
		string sessionID = event->sessionID;
		unsigned long eventTypeKey = event->eventTypeKey;
		string alarmId = event->alarmID;
		unsigned long currentLocationKey = strtoul( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);

		unsigned long subsystemKey = event->subsystemKey;
		long physicalSubsystemKey =  event->physicalSubsystemKey;
		unsigned long entityLocationKey = event->locationKey;
	
		unsigned long operatorKey = event->operatorKey;
		long profileID = event->profileID;
		unsigned long sessionLocationKey = event->sessionLocation;
		string operatorName = event->operatorName;
		unsigned long alarmSeverityKey = event->severityKey;
		
		unsigned long viewLevel =event->eventLevel;
		string eventTypeName = event->eventTypeName;
		unsigned long actionId = event->actionID;

		unsigned long locKey = entityLocationKey;

		string value= event->value;
		//for dp status change event, we need to get message_type(together with action_id) data here
		if(sourceTable==EventConstants::SOURCE_TABLE_DP_CHANGE)
		{
			//get me_message_Type+action_id data from cache :
			getMessageTypeInfoByKey(eventTypeKey, viewLevel, eventTypeName, actionId);

			//process some data, e.g. subsystemKey, desc
			processSubsystemKeyNPhisicalSubsystemKey(subsystemKey, physicalSubsystemKey); 

			event->eventLevel = viewLevel;
			strncpy ( event->eventTypeName, eventTypeName.c_str(), EV_EVENT_TYPE_NAME_SIZE );
			event->actionID = actionId;
			event->subsystemKey = subsystemKey;
			event->physicalSubsystemKey = physicalSubsystemKey;
		}
		else //sourceTable==EventConstants::SOURCE_TABLE_EVENT
		{

			//entity  data
			if(eventItem->isEntityInfoValid == false)
			{
				getEntityInfoByKey(entityKey, subsystemKey,physicalSubsystemKey, entityLocationKey );
			}
			//session data : if sessionId is invalid
			if(sessionID.compare("")==0 || sessionID.compare(INTERNAL_PROCESSING_SESSION_KEY)==0 )
			{
				profileID = -1;
				operatorKey = 0;
				operatorName =  "";
				sessionLocationKey = 0;

				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Session Id is invalid. Set default value to session info.");

			}else //if sessionId is valid
			{
				if(eventItem->isSessionInfoValid == false )
				{
					getSessionInfoByKey(sessionHelper, sessionID,operatorKey, profileID, sessionLocationKey );

					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Session Id is valid and get session info by sessionID.");
				}
				getOperatorNameNAdjustProfileID(operatorKey, operatorName,profileID); 
			}

			//get me_message_Type+action_id data from cache :
			getMessageTypeInfoByKey(eventTypeKey, viewLevel, eventTypeName, actionId);

			if(alarmId == "")  //for non-alarm event
			{
				modifyValue(value, eventTypeName);  

			    locKey = processLocKeyForEvent( currentLocationKey, entityLocationKey);
			}
			else
			{
				locKey = processLocKeyForAlarm(currentLocationKey , entityLocationKey);  //sourceLocationKey from RPARAM_LOCATIONKEY
				strncpy(event->severityname,(getSeverityNameByKey(alarmSeverityKey)).c_str(), EV_SEVERITY_NAME_SIZE );

			}

			processSubsystemKeyNPhisicalSubsystemKey(subsystemKey,physicalSubsystemKey); 
			

			//update the data in event:
			event->subsystemKey = subsystemKey;
			event->physicalSubsystemKey = physicalSubsystemKey;
			event->locationKey = locKey;
			strncpy(event->value,  value.c_str(), EV_ALARM_VALUE_SIZE);
			event->profileID = profileID;
			event->eventLevel = viewLevel;
			strncpy(event->eventTypeName, eventTypeName.c_str(), EV_EVENT_TYPE_NAME_SIZE);
			event->actionID = actionId;
			event->operatorKey = operatorKey;
			strncpy(event->operatorName,operatorName.c_str(), EV_OPERATOR_NAME_SIZE );
			event->sessionLocation = sessionLocationKey;
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Finished prepareAllDataForEvent, eventId= %s ", event->eventID );
	}

	void RealTimeEventDataHelper::modifyValue(string & value, string eventTypeName)
	{
		std::size_t found = value.find(EventConstants::EVENT_TYPE_NAME_STUB);
		if (found!=std::string::npos)  //found
		{
			value.replace(found, EVENT_TYPE_NAME_STUB.length(), eventTypeName);
		}

	}

	void RealTimeEventDataHelper::processSubsystemKeyNPhisicalSubsystemKey(unsigned long & subsystemKey,long & physical_subsytem_key)
	{
		//here 0 means physical_substystem_key from entity table is NULL. 
		//The entity with physical_subsystem_key=0 will not occur in events
		if ( 0 < physical_subsytem_key)  
		{ 
			subsystemKey =  physical_subsytem_key; 
		}
		else
		{ 
			physical_subsytem_key = -1;
		}

	}



	unsigned long RealTimeEventDataHelper::processLocKeyForEvent(unsigned long sourceLocKey, unsigned long etyLocationKey)  
	{
		if (etyLocationKey != 0) 
		{ 
			return etyLocationKey; 
		}
		else if(sourceLocKey != 0) 
		{
			return sourceLocKey;
		}
		else 
		{ 
			return etyLocationKey;
		}
	}



	unsigned long RealTimeEventDataHelper::processLocKeyForAlarm(unsigned long sourceLocKey, unsigned long etyLocationKey)  // can alarm provide the processed location key and put it into param "entityLocation"?
	{
		if (etyLocationKey != 0) 
		{ return etyLocationKey; }
		else if(sourceLocKey != 0) 
		{return sourceLocKey;}
		else 
		{ 
			return m_OCCLocationKey;
		}
	}

	void RealTimeEventDataHelper::getSessionInfoByKey(ISessionHelper* sessionHelper,std::string sessionId,unsigned long & operatorKey, long& profileID, unsigned long& sessionLocationKey )
	{

		try 
		{
			/*LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Getting Session From DB for sesesionID: %s", sessionId);
			// get session data , refer to authentication agent
			std::auto_ptr<TA_Base_Core::ISession> session( TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId) );

			// Copy the details into the session object
			operatorKey = session->getOperatorKey();
			profileID = session->getProfileKey();
			sessionLocationKey = session->getLocationKey();
			*/

			//ISessionHelper * sessionHelper = new TA_Base_Bus::SessionHelper();

			//ISessionHelper * sessionHelper = RealTimeEventSenderInstance::instance().getSessionHelper();
			if(sessionHelper != NULL)
			{
				sessionHelper->getSessionInfo(sessionId,operatorKey,profileID,sessionLocationKey);  
			}
			else
			{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Session helper Interface is not set for this application.");
					//todo check to throw exception in this case.
			}	

		}

		catch(...)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "...",
				"Unknown exception thrown while retrieving session information");
		}
	}

	void RealTimeEventDataHelper::getOperatorNameNAdjustProfileID(unsigned long operatorKey, std::string & operatorName, long & profileID)
	{
		bool is_system = false;
		//get opearatorName by operatorKey , from cache:
		std::map< unsigned long, TA_Base_Core::IOperator* > ::iterator it ;
		it = m_operatorMap.find(operatorKey);
		if(it == m_operatorMap.end()) //can't find prifile by id in cache.
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Operator with ID %lu can't be found in cache.", operatorKey);

		}else
		{
			TA_Base_Core::Operator* theOpertor = static_cast<TA_Base_Core::Operator*>(it->second);
			operatorName = theOpertor->getName();
			is_system = theOpertor->isSystemOperator();
            if(is_system) 
			{ 
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Is_system is true, so we set operatorName and profileID to default value.");
				operatorName = "";
				profileID = -1;
			}
		}
	}

	void RealTimeEventDataHelper::getMessageTypeInfoByKey(unsigned long eventTypeKey, unsigned long & viewLevel,
		std::string & eventTypeName, unsigned long & actionId)
	{
		//get message type by event type key , from cache:
		std::map< unsigned long, TA_Base_Core::MessageTypeEventData* > ::iterator it ;
		it = m_msgTypeMap.find(eventTypeKey);
		if(it == m_msgTypeMap.end()) //can't find prifile by id in cache.
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Event Message type with ID %lu can't be found in cache.", eventTypeKey);
// 			viewLevel =0;
// 			eventTypeName = "";
		}else
		{
			TA_Base_Core::MessageTypeEventData* theMsgType = it->second;
			viewLevel = theMsgType->getViewLevel();  //unsigned int -> unsigned long
			eventTypeName = theMsgType->getName();
			actionId = theMsgType->getActionId();
		}
	}

	void RealTimeEventDataHelper::getEntityInfoByKey(unsigned long entityKey, unsigned long & subsystemKey,long & physicalSubsystemKey, unsigned long & entityLocationKey )
	{
		//we don't keep cache for entity data. Just query DB by key each time when necessary
		IEntityData* entityData = EntityAccessFactory::getInstance().getEntity(entityKey,true);  //"true": will not retrieve entity parameter info.
		subsystemKey = entityData->getSubsystem();
		physicalSubsystemKey = entityData->getPhysicalSubsystem();  //unsigned long-> long
		entityLocationKey =  entityData->getLocation();

	}
	void RealTimeEventDataHelper::initOperatorData()
	{
		unsigned long tempKey=0;
		// Now reload all the operators
		std::vector<TA_Base_Core::IOperator*> oprators = TA_Base_Core::OperatorAccessFactory::getInstance().getAllOperators( false);

		// And insert the operators into our map. 
		for (std::vector<TA_Base_Core::IOperator*>::iterator iter = oprators.begin();
			iter != oprators.end();
			++iter
			)
		{
			TA_Base_Core::Operator* theOperator = static_cast<TA_Base_Core::Operator*>(*iter);

			tempKey=theOperator->getKey();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "about to insert operator: %lu into cache", tempKey );
			m_operatorMap.insert(std::map< unsigned long, TA_Base_Core::IOperator* >::value_type( tempKey, theOperator  ) );

		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Operator cache size : %lu ", (unsigned long)m_operatorMap.size() );
	}

	void RealTimeEventDataHelper::initMessageTypeData()
	{
		unsigned long tempKey=0;
		// Now reload all the operators
		std::vector<TA_Base_Core::MessageTypeEventData*> messageTypes = TA_Base_Core::MessageTypeAccessFactory::getInstance().getAllEventMessageTypes();

		// And insert the operators into our map. 
		for (std::vector<TA_Base_Core::MessageTypeEventData*>::iterator iter = messageTypes.begin();
			iter != messageTypes.end();
			++iter
			)
		{
			TA_Base_Core::MessageTypeEventData* theMessage = *iter;

			tempKey=theMessage->getKey();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "about to insert message type: %lu into cache", tempKey );
			m_msgTypeMap.insert(std::map< unsigned long, TA_Base_Core::MessageTypeEventData* >::value_type( tempKey, theMessage ) );

		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Message Type cache size : %lu ", (unsigned long)m_msgTypeMap.size() );
	}

	void RealTimeEventDataHelper::InitAlarmSeverity()
	{
		ThreadGuard guard(m_lock);
		if(!m_alarmSeverityLoaded)
		{
			// Now reload all the alarmSeverity
			std::vector<TA_Base_Core::IAlarmSeverityData*> alarmSeverities = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();
			for (std::vector<TA_Base_Core::IAlarmSeverityData*>::iterator iter = alarmSeverities.begin();
				iter != alarmSeverities.end();
				++iter
				)
			{				
				TA_Base_Core::AlarmSeverityData* alarmSeverity =  static_cast<TA_Base_Core::AlarmSeverityData*>(*iter);
				unsigned long pKey=alarmSeverity->getKey();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "about to insert alarm Severity : %lu into cache", pKey );
				m_alarmSeverityMap.insert(std::map< unsigned long, TA_Base_Core::IAlarmSeverityData* >::value_type( pKey, alarmSeverity ) );
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Alarm Severity cache size : %lu ", (unsigned long)m_alarmSeverityMap.size() );
			m_alarmSeverityLoaded = true;
		}
	}
	std::string RealTimeEventDataHelper::getSeverityNameByKey(unsigned long severityKey)
	{
		if(!IsAlarmSeverityLoaded())
		{
			InitAlarmSeverity();
		}

		std::map< unsigned long, TA_Base_Core::IAlarmSeverityData* > ::iterator it ;
		it = m_alarmSeverityMap.find(severityKey);
		if(it == m_alarmSeverityMap.end()) //can't find
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Alarm Severrity with ID %lu can't be found in cache.", severityKey);
			return "";
		}else
		{
			TA_Base_Core::AlarmSeverityData* alarmSeverity = static_cast<TA_Base_Core::AlarmSeverityData*>(it->second);
			return alarmSeverity->getName();
		}
	}
	bool RealTimeEventDataHelper::IsAlarmSeverityLoaded()
	{
		ThreadGuard guard(m_lock);
		return m_alarmSeverityLoaded;
	}

	void RealTimeEventDataHelper::releaseAlarmSeverityCache()
	{
		ThreadGuard guard(m_lock);
		if(m_alarmSeverityLoaded)
		{
			std::map< unsigned long, TA_Base_Core::IAlarmSeverityData* >::iterator it;
			for( it = m_alarmSeverityMap.begin(); it != m_alarmSeverityMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
		}
	}
}
