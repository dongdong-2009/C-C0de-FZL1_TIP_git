/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event_processing/src/EventDataProcessor.cpp $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by:  $Author: 
 * 
 * EventDataProcessor is a thread for 1).retreiving all the data for event 2)processing some data in event
 *
 */

#include "app/event/event_agent/src/EventDataProcessor.h"
#include "core/event/src/EventUtilities.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/Profile.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/Operator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/AlarmSeverityData.h"
#include "core/event_processing/src/IEvent.h"


using namespace std;
using namespace TA_Base_Core::EventConstants;
using namespace TA_Base_Core;

namespace TA_Base_App
{

	EventDataProcessor* EventDataProcessor::m_instance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable EventDataProcessor::m_singletonLock;

	EventDataProcessor& EventDataProcessor::getInstance()
	{
		ThreadGuard guard(m_singletonLock);
		// Create a new instance if it doesn't exist.
		if ( 0 == m_instance )
		{
			m_instance = new EventDataProcessor();
		}

		return *m_instance;
	}

	void EventDataProcessor::releaseInstance()
	{
		ThreadGuard guard(m_singletonLock);
		if ( m_instance != 0)
		{
			delete m_instance;
			m_instance = NULL;
		}
		return;
	}

	EventDataProcessor::EventDataProcessor(void)
		:m_myLocation(0)
		,m_OCCLocationKey(0)
		,m_alarmSeverityLoaded(false)
		,m_bInitialized(false)
	{

	}

	void EventDataProcessor::initialize()
	{
		ThreadGuard guard(m_threadLock);
		if (!m_bInitialized)
		{
			initOperatorData();
			initLocationKeys();
			InitAlarmSeverity();

			m_authLib = new TA_Base_Bus::AuthenticationLibrary();
			m_bInitialized = true;
		}
	}


	void EventDataProcessor::initLocationKeys()
	{
		//SELECT pkey from LOCATION where name = 'OCC';  
		m_OCCLocationKey = LocationAccessFactory::getInstance().getOccLocationKey();
		m_myLocation = strtoul( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);
	}

	EventDataProcessor::~EventDataProcessor(void)
	{
		releaseOperatorCache();
		releaseAlarmSeverityCache();

		T_EntityMap::iterator itr = m_entityCache.begin();
		for (itr; itr!=m_entityCache.end();itr++)
		{
			delete itr->second;
		}
		m_entityCache.clear();
	}

	void EventDataProcessor::releaseOperatorCache()
	{
		T_OperatorMap::iterator it;
		for( it = m_operatorMap.begin(); it != m_operatorMap.end(); it++ )
		{
			delete it->second;
			it->second = NULL;
		}
	}

	TA_Base_Core::EventItem* EventDataProcessor::processEventInfo(const TA_Base_Core::EventInfoCorbaDef& eventInfo)
	{
		FUNCTION_ENTRY("processEventInfo");
		FUNCTION_EXIT;
		return processEventInternal(eventInfo);
	}

	void EventDataProcessor::processEventInfo(const TA_Base_Core::EventInfoSequence& eventInfos, TA_Base_Core::T_EventItemList& retVal)
	{
		FUNCTION_ENTRY("processEventInfo");
		unsigned long idx=0;
		for (idx; idx < eventInfos.length(); idx++)
		{
			retVal.push_back(processEventInternal(eventInfos[idx]));
		}

		FUNCTION_EXIT;
	}

	TA_Base_Core::EventItem* EventDataProcessor::processEventInternal(const TA_Base_Core::EventInfoCorbaDef& eventInfo)
	{
		FUNCTION_ENTRY("processEventInternal");
		TA_Base_Core::EventItem* pEvent = TA_Base_Core::EventUtilities::EventCorbaToEventItem(eventInfo.eventDetail);
		
		if (TA_Base_Core::EventConstants::SOURCE_TABLE_EVENT == pEvent->sourceTable)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Begin processing event, eventId= %s ", eventInfo.eventDetail.eventID.in() );
			unsigned long entityLocationKey = pEvent->locationKey;
			//entity  data
			if(eventInfo.isEntityInfoValid == false)
			{
				IEntityData* entityData = getEntityData(pEvent->entitykey);
				if (0 != entityData)
				{
					pEvent->subsystemKey = entityData->getSubsystem();
					pEvent->physicalSubsystemKey = entityData->getPhysicalSubsystem();  //unsigned long-> long
					entityLocationKey =  entityData->getLocation();
				}
				else
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to retrieve entity data for entity key %d", pEvent->entitykey);
			}

			//session data : if sessionId is invalid
			if(eventInfo.isSessionInfoValid == false )
			{
				updateSessionInfo(pEvent);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Session Id is valid and get session info by sessionID.");
			}

			updateOperator(pEvent); 
			
			unsigned long locKey = pEvent->locationKey;
			if(std::string("") == pEvent->alarmID)  //for non-alarm event
			{
				locKey = processLocKeyForEvent( m_myLocation, entityLocationKey);
			}
			else
			{
				locKey = processLocKeyForAlarm(m_myLocation , entityLocationKey);  //sourceLocationKey from RPARAM_LOCATIONKEY
				assignNewValue(pEvent->severityname, (getSeverityNameByKey(pEvent->severityKey)).c_str(), EV_SEVERITY_NAME_SIZE);
			}
			pEvent->locationKey = locKey;

			if ( 0 < pEvent->physicalSubsystemKey)  
			{ 
				pEvent->subsystemKey = pEvent->physicalSubsystemKey; 
			}
			else
			{ 
				pEvent->physicalSubsystemKey = -1;
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Finished processing event, eventId= %s ", eventInfo.eventDetail.eventID.in() );
		}

		FUNCTION_EXIT;
		return pEvent;
	}

	void EventDataProcessor::updateSessionInfo(TA_Base_Core::EventItem* pEvent)
	{
		try 
		{
			if(m_authLib != 0)
			{
				std::string sessionID = pEvent->sessionID;
				TA_Base_Bus::SessionInfo sessionInfo = m_authLib->getSessionInfo(sessionID, sessionID);

				pEvent->operatorKey = sessionInfo.UserId;
				pEvent->profileID = sessionInfo.ProfileId[0];
				pEvent->sessionLocation = sessionInfo.WorkstationId;			//console key
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Authentication library not properly initialized.");
				throw;
			}	

		}
		catch(...)
		{
			// Log the catch, and re-throw as an AuthenticationAgentException
			LOG_EXCEPTION_CATCH(SourceInfo, "...",
				"Unknown exception thrown while retrieving session information");
		}
	}

	unsigned long EventDataProcessor::processLocKeyForEvent(unsigned long sourceLocKey, unsigned long etyLocationKey)  
	{
		if (etyLocationKey != 0) 
		{ 
			return etyLocationKey; 
		}
		else if(sourceLocKey != 0) 
		{
			return sourceLocKey;
		}

		return 0;
	}

	unsigned long EventDataProcessor::processLocKeyForAlarm(unsigned long sourceLocKey, unsigned long etyLocationKey)  // can alarm provide the processed location key and put it into param "entityLocation"?
	{
		if (etyLocationKey != 0) 
		{ 
			return etyLocationKey; 
		}
		else if(sourceLocKey != 0) 
		{
			return sourceLocKey;
		}
		 
		return m_OCCLocationKey;
	}

	void EventDataProcessor::updateOperator(TA_Base_Core::EventItem* pEvent)
	{
		//get opearatorName by operatorKey , from cache:
		T_OperatorMap::iterator it ;
		it = m_operatorMap.find(pEvent->operatorKey);
		if(it == m_operatorMap.end()) //can't find prifile by id in cache.
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Operator with ID %lu can't be found in cache.", pEvent->operatorKey);
		}
		else
		{
			TA_Base_Core::Operator* theOpertor = static_cast<TA_Base_Core::Operator*>(it->second);
			if(theOpertor->isSystemOperator()) 
			{ 
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Is_system is true, so we set operatorName and profileID to default value.");
				assignNewValue(pEvent->operatorName, "", EV_OPERATOR_NAME_SIZE);
				pEvent->profileID = -1;
			}
			else
			{
				assignNewValue(pEvent->operatorName, theOpertor->getName().c_str(), EV_OPERATOR_NAME_SIZE-1);
			}
		}
	}

	void EventDataProcessor::initOperatorData()
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
			m_operatorMap.insert(T_OperatorMap::value_type( tempKey, theOperator  ) );

		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Operator cache size : %lu ", (unsigned long)m_operatorMap.size() );
	}


	void EventDataProcessor::InitAlarmSeverity()
	{
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
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "about to insert alarm Severity : %lu into cache", pKey );
				m_alarmSeverityMap.insert(T_AlarmSeverityMap::value_type( pKey, alarmSeverity ) );
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Alarm Severity cache size : %lu ", (unsigned long)m_alarmSeverityMap.size() );
			m_alarmSeverityLoaded = true;
		}
	}

	std::string EventDataProcessor::getSeverityNameByKey(unsigned long severityKey)
	{
		T_AlarmSeverityMap::iterator it ;
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

	void EventDataProcessor::releaseAlarmSeverityCache()
	{
		if(m_alarmSeverityLoaded)
		{
			T_AlarmSeverityMap::iterator it;
			for( it = m_alarmSeverityMap.begin(); it != m_alarmSeverityMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
		}
	}

	TA_Base_Core::IEntityData* EventDataProcessor::getEntityData(unsigned long ulEntityKey)
	{
		T_EntityMap::iterator itFind = m_entityCache.find(ulEntityKey);
		if (itFind!=m_entityCache.end())
		{
			return itFind->second;
		}
		
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Entity data not found in the cache with pkey = %u", ulEntityKey);
		IEntityData* pEntityData = NULL;
		pEntityData = EntityAccessFactory::getInstance().getEntity(ulEntityKey, true);  //"true": will not retrieve entity parameter info.
		if (0 != pEntityData)
		{
			m_entityCache[ulEntityKey] = pEntityData;
		}

		return pEntityData;
	}

	void EventDataProcessor::assignNewValue(char* pszDest, const char* pszSource, size_t maxBuffer)
	{
		size_t cpLen = strlen(pszSource) >= maxBuffer ? maxBuffer - 1 : strlen(pszSource);
		memcpy(pszDest, pszSource, cpLen);
		pszDest[cpLen] = 0;

	}
}
