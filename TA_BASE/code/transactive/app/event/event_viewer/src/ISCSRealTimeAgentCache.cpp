/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSRealTimeAgentCache.cpp $
  * @author 
  * @version $Revision: 
  *
  * Last modification: $DateTime:
  * Last modified by: 
  * 
  * ISCSRealTimeAgentCache is a thread for for holding data and process data before notifying GUI to show.
  *
  */
#include "app/event/event_viewer/src/stdafx.h"

#include "app/event/event_viewer/src/EventHandlerManager.h"
#include "app/event/event_viewer/src/ConstantItems.h"
#include "ISCSRealTimeAgentCache.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/event_processing/src/IEvent.h "
namespace TA_Base_App{

	ISCSRealTimeAgentCache::ISCSRealTimeAgentCache(IEventWorkObserver* observer)
	{
		m_isRightsCheckNeeded = EventHandlerManager::getRightsCheck();
		m_ProfileId4CurrentRightsCache = 0; //this value is a temprary value, it's invalid actually
		m_rightCachePrepared = false;

		m_wokerObserver = observer;
		m_currentTaskID = 0;
		m_statusReadySent = false;
		m_needToTerminate = false;
	}

	ISCSRealTimeAgentCache::~ISCSRealTimeAgentCache(void)
	{
		clearList();
	}
	void ISCSRealTimeAgentCache::terminate()
	{
		m_needToTerminate = true;
	}

	void ISCSRealTimeAgentCache::pushEventsToCache(TA_Base_Core::T_EventItemList & events)
	{
		//add events to the bottom of the cache. Because the last one is latest
		TA_Base_Core::ThreadGuard guard(m_cacheLock);

		m_events.insert(m_events.end(), events.begin(), events.end());

	}

	void ISCSRealTimeAgentCache::clearList()
	{
		TA_Base_Core::T_EventItemList::iterator it;
		TA_Base_Core::ThreadGuard guard(m_cacheLock);
		TA_Base_Core::EventItem * item = NULL;
		for(it=m_events.begin(); it!=m_events.end(); it++)
		{
			item = (*it);
			
			delete item;
			item= NULL;
		}
		
		m_events.clear();
	}

	void ISCSRealTimeAgentCache::clearCurrentRightsCache()
	{
		m_rights.clear();
	}
	void ISCSRealTimeAgentCache::getRightsCacheFromFactory(unsigned long profileId)
	{

		while( !m_needToTerminate )
		{
			//copy the rights cache from factory which is loaded by DB worker.
			if( TA_Base_Core::T6AccessFactory::getInstance().getRightsByProfileId(profileId, m_rights))
			{				
				break;
			}
			else //if cache not ready in factory, wait for it.
			{
				sleep(EventViewerConstants::RIGHTS_CACHE_CHECK_INTERVAL);
			}
		}

		m_ProfileId4CurrentRightsCache = profileId;
		m_rightCachePrepared = true;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After getting rights from factory.");
	}
	void ISCSRealTimeAgentCache::prepareRightsCache()
	{
		//update Right Cache for checking rights for events get from agent
		//will check whether profile changed, if profile not changed,and rights cache is not empty, do nothing
		if( (m_currentFilter.profiles.size()>0 && m_isRightsCheckNeeded==true)
			&& ( m_rightCachePrepared== false || (m_rightCachePrepared==true && m_ProfileId4CurrentRightsCache!=m_currentFilter.profiles[0]) ) )
		{

			clearCurrentRightsCache();

			getRightsCacheFromFactory(m_currentFilter.profiles[0]);

		}
	}


	void ISCSRealTimeAgentCache::setNewTask(long taskID, FilterCriteria filter)
	{
		TA_Base_Core::ThreadGuard guard(m_cacheLock);
		clearList();
		m_currentTaskID = taskID;
		m_statusReadySent = false;
		m_currentFilter = filter;
		m_currentFilter.analyzeFilter();

	}
	void ISCSRealTimeAgentCache::ProcessEvents()
	{

		TA_Base_Core::ThreadGuard guard(m_cacheLock);
		if(m_events.size()>0)
		{
			int mergeDeleted = m_wokerObserver->mergeEventFromAgent( &m_events);   

			if(m_needToTerminate)
			{
				clearList();
				return;
			}

			prepareRightsCache();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Before filtering events. Total %d events", m_events.size()-mergeDeleted);

			std::vector<TA_Base_Core::ICombinedEventData*>  events = filterEvents(); 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After filtering events. %d events remain.", events.size());

			clearList();//clear T_EventItemList
			if(events.size()>0)
			{
				m_wokerObserver->EventNotification(TA_Base_Core::ISCSCategory, events, TA_Base_App::NoCommand, m_currentTaskID);
			}
			if(!m_statusReadySent)
			{
				m_wokerObserver->statusReadyNotification(TA_Base_Core::ISCSCategory, m_currentTaskID);
			}

		}


	}

	void ISCSRealTimeAgentCache::setInterval(unsigned long interval)
	{
		m_cachProcessInterval = interval;
	}
	void ISCSRealTimeAgentCache::run()
	{
		while(!m_needToTerminate)
		{
			ProcessEvents();
			sleep(m_cachProcessInterval);
		}
	}

	std::vector<TA_Base_Core::ICombinedEventData*> ISCSRealTimeAgentCache::filterEvents()
	{
		TA_Base_Core::T6AccessFactory::SubSystemKeys keys ;

		int eventDetailDebugLevel = EventHandlerManager::getEventDetailInDebugLevel();
		//loop the event which has non-empty eventID ( the empty eventId means it's already deleted in last step)
		//delete the events which not meet the filter m_currentFilter
		//check each m_currentFilter*** (e.g. m_filterSubsystemKey) if is true:check this critea.
		string tempEventId = "";
		string createTimeStr="";

		bool meetFilter =false;
		std::vector<TA_Base_Core::ICombinedEventData*> eventResult ;//= new std::vector<TA_Base_Core::ICombinedEventData*>();
		TA_Base_Core::ICombinedEventData* tempData = NULL;
		TA_Base_Core::T_EventItemList::iterator it;

		TA_Base_Core::ThreadGuard guard(m_cacheLock);
		int i=0;
		for (it=m_events.begin(); it!=m_events.end(); it++)
		{
			i++;
			tempEventId = (*it)->eventID;

			if(eventDetailDebugLevel>0 && i%eventDetailDebugLevel == 0)
			{
				createTimeStr = CTime((*it)->createTime.time).Format("%d-%b-%y %H:%M:%S");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Going to check an event[%d] with eventID=%s, createtime=%s,profileID=%d, subsystemKey=%lu, physicalSubsystemKey=%d,",
					i, tempEventId.c_str(), createTimeStr.c_str(),(*it)->profileID,(*it)->subsystemKey, (*it)->physicalSubsystemKey);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "operatorKey=%lu, operatorName=%s, locationKey=%lu, actionId=%lu, severityKey=%d, assetName=%s, decription=%s",
					(*it)->operatorKey,((*it)->operatorName),
					(*it)->locationKey, (*it)->actionID, (*it)->severityKey, (*it)->assetname, (*it)->description);
			}
			else if(eventDetailDebugLevel==0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Going to check an event with eventID=%s, profileID=%d, subsystemKey=%lu, physicalSubsystemKey=%d,",
					tempEventId.c_str(),(*it)->profileID,(*it)->subsystemKey, (*it)->physicalSubsystemKey);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "operatorKey=%lu, operatorName=%s, locationKey=%lu, actionId=%lu, severityKey=%d, assetName=%s, decription=%s",
					(*it)->operatorKey,((*it)->operatorName),
					(*it)->locationKey, (*it)->actionID, (*it)->severityKey, (*it)->assetname, (*it)->description);
			}


			//if the eventID="", it means this event is invalid(it's already marked as deleted in last step: when merge in EventHandler
			if(tempEventId.compare("") ==0) 
			{
				continue;
			}
			else
			{
				keys.subsystemKey = (*it)->subsystemKey;
				keys.physicalSubsystemKey = (*it)->physicalSubsystemKey;

				bool pass = checkAnEventByRights(keys, (*it)->actionID);
				if(pass)
				{ 
					pass = checkAnEventByFilter(*it);
					if(pass) //this event meet the filter, should be submitted.
					{
						tempData = createEventDataFromDetail(*it);
						eventResult.push_back(tempData); 
					}
					else
					{
						LOG_GENERIC(SourceInfo, eventDetailDebugLevel?TA_Base_Core::DebugUtil::DebugDebug:TA_Base_Core::DebugUtil::DebugTrace, "Filtering check failed for event - %s",tempEventId.c_str());
					}

				}else
				{
					LOG_GENERIC(SourceInfo, eventDetailDebugLevel?TA_Base_Core::DebugUtil::DebugDebug:TA_Base_Core::DebugUtil::DebugTrace, "Rights check failed for event - %s",tempEventId.c_str());

				}

			}
		}
		return eventResult;
	}

	bool ISCSRealTimeAgentCache::checkAnEventByRights(TA_Base_Core::T6AccessFactory::SubSystemKeys subKeys, unsigned long actionId)
	{

		if(!m_isRightsCheckNeeded) //if right checking is not needed, always return true
		{
			return true;
		}

		TA_Base_Core::T6AccessFactory::Rights::iterator it;
		it = m_rights.find(subKeys);
		if(it == m_rights.end()) //can't find subsystemkey and physicalSubsystemKey
		{
			return false;
		}
		else
		{
			std::set<unsigned long>  actionIdSet = it->second;
			if(actionIdSet.find(actionId) != actionIdSet.end())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}



	bool ISCSRealTimeAgentCache::checkAnEventByAFilterItem(FilterItermToCheck filterItem, TA_Base_Core::EventItem * event)
	{
		if(filterItem==FilterItermToCheck::StartTime)
		{
			if ( event->createTime.time < m_currentFilter.StartTimePeriod.getTime_t())
			{
				return false;
			}
		}
		else if( filterItem==FilterItermToCheck::EndTime)
		{
			if ( event->createTime.time > m_currentFilter.EndTimePeriod.getTime_t())
			{
				return false;
			}
		}
		else if( filterItem==FilterItermToCheck::Asset)
		{
			string assetName = event->assetname;
			std::transform(assetName.begin(), assetName.end(), assetName.begin(),toupper);
			if (assetName.find( m_currentFilter.Asset.front()) == -1 )
			{
				return false;
			}
		}
		else if( filterItem==FilterItermToCheck::SubsystemID)
		{
			bool findSubsystemKey = false;
			//appSubsystemIds is NumericList(vector<unsinged long>)
			for(int i=0; i<m_currentFilter.appSubsystemIds.size(); i++)
			{
				if( m_currentFilter.appSubsystemIds[i] == event->subsystemKey)
				{
					findSubsystemKey = true;
					break;
				}
			}
			if(!findSubsystemKey) return false;
		}
		else if( filterItem==FilterItermToCheck::PhysicalSubsystemID)
		{
			bool findPhysSubsystemKey = false;
			//physSubsystemIds is NumericList(vector<unsinged long>)
			for(int i=0; i<m_currentFilter.physSubsystemIds.size(); i++)
			{
				if( m_currentFilter.physSubsystemIds[i] == event->physicalSubsystemKey)
				{
					findPhysSubsystemKey = true;
					break;
				}
			}
			if(!findPhysSubsystemKey) return false;
		}
		else if( filterItem==FilterItermToCheck::LocationID)
		{
			bool findLocKey = false;
			//appSubsystemIds is NumericList(vector<unsinged long>)
			for(int i=0; i<m_currentFilter.locationIds.size(); i++)
			{
				if( m_currentFilter.locationIds[i] == event->locationKey)
				{
					findLocKey = true;
					break;
				}
			}
			if(!findLocKey) return false;
		}
		else if( filterItem==FilterItermToCheck::OperatorID)
		{
			bool findOperatorKey = false;
			//appSubsystemIds is NumericList(vector<unsinged long>)
			for(int i=0; i<m_currentFilter.operatorIds.size(); i++)
			{
				if( m_currentFilter.operatorIds[i] == event->operatorKey)
				{
					findOperatorKey = true;
					break;
				}
			}
			if(!findOperatorKey) return false;
		}
		else if( filterItem==FilterItermToCheck::SystemID)
		{
			bool findSystemKey = false;
			//appSubsystemIds is NumericList(vector<unsinged long>)
			for(int i=0; i<m_currentFilter.systemIds.size(); i++)
			{
				if( m_currentFilter.systemIds[i] == event->systemKey)
				{
					findSystemKey = true;
					break;
				}
			}
			if(!findSystemKey) return false;
		}
		else if( filterItem==FilterItermToCheck::EventTypeID)
		{
			bool findEventTypeID = false;
			//appSubsystemIds is NumericList(vector<unsinged long>)
			for(int i=0; i<m_currentFilter.eventTypeIds.size(); i++)
			{
				if( m_currentFilter.eventTypeIds[i] == event->eventTypeKey)
				{
					findEventTypeID = true;
					break;
				}
			}
			if(!findEventTypeID) return false;
		}
		else if( filterItem==FilterItermToCheck::TMType)
		{
			if( (m_currentFilter.TMTypeEqualOr.front()).compare(event->operatorMode) != 0 )
				return false;
		}
		else if( filterItem==FilterItermToCheck::Description)
		{
			string desc = event->description;
			std::transform(desc.begin(), desc.end(), desc.begin(),toupper);

			if (desc.find( m_currentFilter.Description.front()) == -1 )
			{
				return false;
			}
		}
		else if( filterItem==FilterItermToCheck::SeverityKey) //whether the DB NULL value is saved as "NULL_VALUE_NUMBER" in Event Agent.(sent by client)
		{
			bool foundSevirtyKey = false;
			for(int i=0; i<m_currentFilter.severityIds.size(); i++)
			{
				//for"non-alarm event", the severitykey=0
				//in filter, "show non-alarm event" was saved in severity filter as EventViewerConstants::EVENT_ONLY_KEY 
				if(m_currentFilter.severityIds[i] == event->severityKey)  
				{
					foundSevirtyKey = true;
					break;
				}
			}
			if(!foundSevirtyKey) return false;
		}
		else if( filterItem==FilterItermToCheck::AlarmType)
		{
			bool findAlarmType = false;
			//appSubsystemIds is NumericList(vector<unsinged long>)
			for(int i=0; i<m_currentFilter.alarmTypes.size(); i++)
			{
				if( m_currentFilter.alarmTypes[i] == event->alarmTypeKey)
				{
					findAlarmType = true;
					break;
				}
			}
			if(!findAlarmType) return false;
		}
		else if( filterItem==FilterItermToCheck::AlarmID)
		{
			string eventAlamId = event->alarmID;
			if( std::find(m_currentFilter.AlarmIDEqualOr.begin(), m_currentFilter.AlarmIDEqualOr.end(),eventAlamId ) == m_currentFilter.AlarmIDEqualOr.end())
				return false;
		}
		else if( filterItem==FilterItermToCheck::EventLevel)
		{
			if(m_currentFilter.eventLevel<event->eventLevel)
				return false;
		}
		return true;
	}

	//check by filter critea one by one:
	//"Asset" and "Description" has been transfer to upper in "analyzeFilter"
	bool ISCSRealTimeAgentCache::checkAnEventByFilter(TA_Base_Core::EventItem * event)
	{
		bool passedFilter = true;
		for(int i=0; i<m_currentFilter.m_checkItems.size(); i++)
		{
			if( !checkAnEventByAFilterItem(m_currentFilter.m_checkItems[i], event))
			{
				return false;
			}
		}
		return true;

	}

	TA_Base_Core::ICombinedEventData* ISCSRealTimeAgentCache::createEventDataFromDetail(TA_Base_Core::EventItem* detail)
	{
		string eventID = detail->eventID;
		string sourceTable = detail->sourceTable;
		TA_Base_Core::ECombinedEventType eventType = ( TA_Base_Core::EventConstants::SOURCE_TABLE_EVENT == sourceTable ) ? TA_Base_Core::E_AUDIT_EVENT : TA_Base_Core::E_DPSTATECHANGE_EVENT;
		//unsigned long key = detail.key  //pkey shouldn't be retrieved from the event which is from Agent.
		timeb createTime;
		createTime.time= detail->createTime.time;
		createTime.millitm = detail->createTime.millitm;
		string alarmSeverityName = detail->severityname;
		string assetName = detail->assetname;
		string description = detail->description;
		string value = detail->value;
		string alarmMmsState = detail->mms;
		string alarmDssState = detail->decisionSupportState;
		string alarmAvlState = detail->avalanceState;
		string operatorName = detail->operatorName;
		string alarmComment = detail->alarmComment;  
		string alarmId = detail->alarmID;
		string alarmAck = detail->alarmAckedBy;
		string alarmStatus = detail->alarmStatus;

		unsigned long subsystemKey = detail->subsystemKey;
		long physicalSubsystemKey = detail->physicalSubsystemKey;
		unsigned long locationKey = detail->locationKey;
		unsigned long severityKey = detail->severityKey;
		unsigned long eventTypeKey = detail->eventTypeKey;
		unsigned long alarmTypeKey = detail->alarmTypeKey;
		unsigned long operatorKey = detail->operatorKey;
		unsigned long eventLevel = detail->eventLevel;
		string sessionKey = detail->sessionID;
		unsigned long sessionLocation = detail->sessionLocation;
		long profileId = detail->profileID;
		unsigned long actionId = detail->actionID;
		unsigned long entityKey = detail->entitykey;
		string avlalarmheadid = detail->avlheadID;
		unsigned long systemKey = detail->systemKey;
		string eventTypeName = detail->eventTypeName;
		string operationMode = detail->operatorMode;

		TA_Base_Core::CombinedEventData * data = new TA_Base_Core::CombinedEventData(eventID,eventType,0,
			createTime,alarmSeverityName,assetName,description,value,alarmMmsState,alarmDssState,
			alarmAvlState,operatorName,alarmComment,alarmId,alarmAck,alarmStatus, 
			sourceTable, subsystemKey, physicalSubsystemKey, locationKey, severityKey, eventTypeKey, alarmTypeKey, 
			operatorKey, eventLevel, sessionKey, sessionLocation, profileId, actionId, entityKey, 
			avlalarmheadid, systemKey, eventTypeName,operationMode );

		return data;

	}



};