/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event_processing/src/RealTimeEventQueue.cpp $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by:  $Author: 
 * 
 * RealTimeEventQueue is a thread for sending event to real-time Helper which will send events to Agent.
 *
 */

#include "core/event_processing/src/RealTimeEventQueue.h"
#include "core/event/src/EventHelper.h"
#include "core/event/src/EventHelperException.h"
#include "core/message/src/MessageConfig.h"
#include "core/utilities/src/RunParams.h"

using namespace std;
namespace TA_Base_Core
{
    const int BATCH_SIZE = 1000;

	
	RealTimeEventQueue::RealTimeEventQueue() : m_terminated(false),m_sessionHelper(NULL)
	{
		FUNCTION_ENTRY( "Constructor" );

		unsigned long locationKey =0;
		std::string strLocationKey = (const std::string&)TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
		std::istringstream strStream(strLocationKey);
		strStream >> locationKey;
		//LOG ( SourceInfo, DebugUtil::DebugInfo, "Get Location Key: %lu", locationKey);?
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get Location Key: %lu", locationKey);

		EventHelper::getInstance().init(locationKey);

		start();


		FUNCTION_EXIT;
	}

	//
	// terminate
	//
	void RealTimeEventQueue::terminate()
	{
		FUNCTION_ENTRY( "terminate" );
		ThreadGuard guard(m_receiveQueueLock);
		m_terminated = true;
		m_queueSemaphore.post();
		FUNCTION_EXIT;
	}

	T_EventItemInfoList RealTimeEventQueue::prepareEventListFromPushQueue()
	{
		FUNCTION_ENTRY( "prepareEventListFromPushQueue" );
		T_EventItemInfoList list;

		ThreadGuard guard(m_pushQueueLock);

		for (int i=0; i<m_eventPushQueue.size(); i++)
		{
			list.push_back(m_eventPushQueue[i]);
		}

		FUNCTION_EXIT;
		return list;

	}

	void RealTimeEventQueue::cleanPushQueue()
	{
		ThreadGuard guard(m_pushQueueLock);

		EventItemInfo* temp=NULL;
		for(int i=0; i<m_eventPushQueue.size(); i++)
		{
			temp = m_eventPushQueue[i];
			delete temp;
			temp = 0;
		}
		m_eventPushQueue.clear();
	}

	void RealTimeEventQueue::moveABatchToPushQueue()
	{
		ThreadGuard guard(m_receiveQueueLock);

		if (!m_eventReceiveQueue.empty())
		{
			int moveLength =0;
			int totalEventSize = m_eventReceiveQueue.size();
			if(totalEventSize>BATCH_SIZE)
			{
				moveLength = BATCH_SIZE;
				for (int i=0; i<moveLength; i++)
				{
					EventItemInfo * event = m_eventReceiveQueue.front();
					ThreadGuard guardWrite(m_pushQueueLock);
					m_eventPushQueue.push_back(event);		
					m_eventReceiveQueue.pop_front();
				}
			}
			else
			{
				moveLength = totalEventSize;
				//move all the item from queue1 to queue2
				ThreadGuard guardWrite(m_pushQueueLock);
				m_eventPushQueue.insert(m_eventPushQueue.end(), m_eventReceiveQueue.begin(), m_eventReceiveQueue.end() );			
				m_eventReceiveQueue.clear();
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Queue1 size is %d , will move %d event to queue2.", totalEventSize, moveLength);

			
		}
	}

	void RealTimeEventQueue::prepareDataForPushQueue()
	{
		ThreadGuard guard(m_pushQueueLock);
		EventItemInfo * theEvent = NULL;
		for (int i=0; i<m_eventPushQueue.size(); i++)
		{
			theEvent = m_eventPushQueue[i];
			//RealTimeEventDataHelper::getInstance().prepareAllDataForEvent(theEvent, m_sessionHelper);
			prepareDataForAnEvent(theEvent);
		}
	}

	void RealTimeEventQueue::getMessageTypeInfoByKey(unsigned long eventTypeKey, unsigned long & viewLevel,
		std::string & eventTypeName, unsigned long & actionId)
	{
		//change ul to string:
		std::ostringstream os;
		os << eventTypeKey;
		std::string eventTypeKeyStr = os.str();

		actionId = MessageTypeTableAccessor::getInstance().getActionId(eventTypeKeyStr);
		viewLevel = MessageTypeTableAccessor::getInstance().getViewLevel(eventTypeKeyStr);
		eventTypeName = MessageTypeTableAccessor::getInstance().getMessageTypeName(eventTypeKeyStr);

	}


	void RealTimeEventQueue::processSubsystemKeyNPhisicalSubsystemKey(unsigned long & subsystemKey,long & physical_subsytem_key)
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

	void RealTimeEventQueue::modifyValue(std::string & value, std::string eventTypeName)
	{
		std::size_t found = value.find(EventConstants::EVENT_TYPE_NAME_STUB);
		if (found!=std::string::npos)  //found
		{
			value.replace(found, EVENT_TYPE_NAME_STUB.length(), eventTypeName);
		}

	}

// 	AgentPrepareData()
// 	{
// 		EventItem* event = eventItem->eventItemDetail;
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Begin prepareAllDataForEvent, eventId= %s ", event->eventID );
// 		string sourceTable = event->sourceTable;
// 		unsigned long entityKey = event->entitykey;
// 		string sessionID = event->sessionID;
// 		unsigned long eventTypeKey = event->eventTypeKey;
// 		string alarmId = event->alarmID;
// 		unsigned long currentLocationKey = strtoul( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);
// 
// 		unsigned long subsystemKey = event->subsystemKey;
// 		long physicalSubsystemKey =  event->physicalSubsystemKey;
// 		unsigned long entityLocationKey = event->locationKey;
// 
// 		unsigned long operatorKey = event->operatorKey;
// 		long profileID = event->profileID;
// 		unsigned long sessionLocationKey = event->sessionLocation;
// 		string operatorName = event->operatorName;
// 		unsigned long alarmSeverityKey = event->severityKey;
// 
// 
// 		unsigned long locKey = entityLocationKey;
// 
// 
// 		if (sourceTable==EventConstants::SOURCE_TABLE_EVENT)
// 		{
// 
// 			//entity  data
// 			if(eventItem->m_etyInfoValid == false)
// 			{
// 				getEntityInfoByKey(entityKey, subsystemKey,physicalSubsystemKey, entityLocationKey );
// 			}
// 
// 			//session data : if sessionId is invalid
// 			if(eventItem->isSessionInfoValid == false )
// 			{
// 				getSessionInfoByKey(sessionHelper, sessionID,operatorKey, profileID, sessionLocationKey );
// 				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Session Id is valid and get session info by sessionID.");
// 			}
// 			getOperatorNameNAdjustProfileID(operatorKey, operatorName,profileID); 
// 
// 			if(alarmId == "")  //for non-alarm event
// 			{
// 				locKey = processLocKeyForEvent( currentLocationKey, entityLocationKey);
// 			}
// 			else
// 			{
// 				locKey = processLocKeyForAlarm(currentLocationKey , entityLocationKey);  //sourceLocationKey from RPARAM_LOCATIONKEY
// 				strncpy(event->severityname,(getSeverityNameByKey(alarmSeverityKey)).c_str(), EV_SEVERITY_NAME_SIZE );
// 			}
// 			processSubsystemKeyNPhisicalSubsystemKey(subsystemKey,physicalSubsystemKey); 
// 
// 			//update the data in event:
// 			event->subsystemKey = subsystemKey;
// 			event->physicalSubsystemKey = physicalSubsystemKey;
// 			event->locationKey = locKey;
// 			event->profileID = profileID;
// 			event->operatorKey = operatorKey;
// 			strncpy(event->operatorName,operatorName.c_str(), EV_OPERATOR_NAME_SIZE );
// 			event->sessionLocation = sessionLocationKey;
// 		}
// 
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Finished prepareAllDataForEvent, eventId= %s ", event->eventID );
// 	}
	void RealTimeEventQueue::prepareDataForAnEvent(EventItemInfo * eventItem)
	{
		EventItem* event = eventItem->eventItemDetail;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Begin prepareAllDataForEvent, eventId= %s ", event->eventID );
		string sourceTable = event->sourceTable;
		string sessionID = event->sessionID;
		unsigned long eventTypeKey = event->eventTypeKey;
		string alarmId = event->alarmID;

		unsigned long subsystemKey = event->subsystemKey;
		long physicalSubsystemKey =  event->physicalSubsystemKey;

		unsigned long operatorKey = event->operatorKey;
		long profileID = event->profileID;
		unsigned long sessionLocationKey = event->sessionLocation;
		string operatorName = event->operatorName;

		unsigned long viewLevel =event->eventLevel;
		string eventTypeName = event->eventTypeName;
		unsigned long actionId = event->actionID;

		string value= event->value;

		//get me_message_Type+action_id data from cache :
		getMessageTypeInfoByKey(eventTypeKey, viewLevel, eventTypeName, actionId);
		event->eventLevel = viewLevel;
		strncpy(event->eventTypeName, eventTypeName.c_str(), EV_EVENT_TYPE_NAME_SIZE);
		event->actionID = actionId;

		//for dp status change event, we need to get message_type(together with action_id) data here
		if(sourceTable==EventConstants::SOURCE_TABLE_DP_CHANGE)
		{
			//process some data, e.g. subsystemKey, desc
			processSubsystemKeyNPhisicalSubsystemKey(subsystemKey, physicalSubsystemKey); 

			event->subsystemKey = subsystemKey;
			event->physicalSubsystemKey = physicalSubsystemKey;
		}
		else //sourceTable==EventConstants::SOURCE_TABLE_EVENT
		{
			//session data : if sessionId is invalid
			if(sessionID.compare("")==0 || sessionID.compare(INTERNAL_PROCESSING_SESSION_KEY)==0 )
			{
				profileID = -1;
				operatorKey = 0;
				operatorName =  "";
				sessionLocationKey = 0;
				
				eventItem->isSessionInfoValid = true;

				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Session Id is invalid. Set default value to session info.");

			}
			if(alarmId == "")  //for non-alarm event
			{
				modifyValue(value, eventTypeName);  
			}
 			strncpy(event->value,  value.c_str(), EV_ALARM_VALUE_SIZE);
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Finished prepareAllDataForEvent, eventId= %s ", event->eventID );
	}

	void RealTimeEventQueue::run()
	{
		while( true )
		{
			m_queueSemaphore.wait();
			if( m_terminated )
			{
				break;
			}
			moveABatchToPushQueue(); //move <=a batch of the events from queue1 to the queue2, later we will fill in all the data for queue2(may query db or cache).
			                //In this way, we don't have to lock the queue1 all the time till we retrieve all the data. So the outer thread can push data to queue1 smoothly

			prepareDataForPushQueue(); //get all the data(some data may not provided by client, e.g. me_mssage_type data, entityData) for queue2

			try
			{
				//consume the queue by batch: get a batch from the queue and send to EA
				ThreadGuard guard(m_pushQueueLock);	

				if(m_eventPushQueue.size()>0)
				{
					//get the a batch/total from top and send to EA
					T_EventItemInfoList seq = prepareEventListFromPushQueue();   //copy pointer from m_eventReceiveQueue

					//void submitEvents( in EventSequence p_eventDataSeq ) raises ( OperationModeException, EventAgentException );
					EventHelper::getInstance().submitEvents(seq);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%d event have been sent to EventHelper successfully. ", m_eventPushQueue.size());
					cleanPushQueue(); //if successful, delete the cache
					continue;
				}
			}
			catch (TA_Base_Core::EventHelperException ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "EventHelperException", "Caught EventHelperException");
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "error code is %d", ex.getErrorCode());
			}
			cleanPushQueue(); //if unsuccessful, delete the cache also
		}
	}

	void RealTimeEventQueue::cleanReceiveQueue()
	{
		FUNCTION_ENTRY( "cleanReceiveQueue" );
		ThreadGuard guard(m_receiveQueueLock);		
		std::deque<EventItemInfo*>::iterator p;
		std::deque<EventItemInfo*>::iterator begin = m_eventReceiveQueue.begin();
		std::deque<EventItemInfo*>::iterator end = m_eventReceiveQueue.end();

		for( p = begin; p != end; p++ )
		{
			if( *p != NULL )
			{
				delete *p;
			}
		}
		m_eventReceiveQueue.clear();
		FUNCTION_EXIT;
	}

	RealTimeEventQueue::~RealTimeEventQueue()
	{
		FUNCTION_ENTRY( "Destructor" );
		cleanPushQueue();
		cleanReceiveQueue();
		delete m_sessionHelper;
		m_sessionHelper = NULL;
		//RealTimeEventDataHelper::releaseInstance();
		FUNCTION_EXIT;
	}

	void RealTimeEventQueue::push(EventItemInfo * event)
	{
		FUNCTION_ENTRY( "push" );

		ThreadGuard guard(m_receiveQueueLock);

		if( !m_terminated )
		{            
			m_eventReceiveQueue.push_back( event );
			m_queueSemaphore.post();
		}
		FUNCTION_EXIT;
	}

	//Note: this method needs to be called before any real time events are processed.
	// hence assumed, no lock needed since only once thread sets it.
	void RealTimeEventQueue::setSessionHelper(ISessionHelper* sessionHelper)
	{
		m_sessionHelper = sessionHelper;
	}

}
