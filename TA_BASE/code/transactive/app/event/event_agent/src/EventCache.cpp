/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/EventCache.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "app/event/event_agent/src/EventCache.h"
#include "app/event/event_agent/src/LocalRunParamDefinitions.h"
//#include "app/event/event_agent/src/ThreadRWGuard.h"

#include "core/event/IDL/src/EventAgentCorbaDef.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/uuid/src/TAuuid.h" //Only for testing
#include <functional> 

#include "core/event/src/EventSerialize.h"
#include "core/event/src/EventZlibDeflate.h"
#include "core/event/src/EventUtilities.h"

using namespace TA_Base_Core;

#define DEFAULT_MAXIMUM_CACHE 100000
#define QUERY_LIMIT 20000

namespace TA_Base_App
{
	EventCache* EventCache::m_pInstance = NULL;
	TA_Base_Core::ReEntrantThreadLockable EventCache::m_singletonLock;

	EventCache::EventCache()
		: m_ulMaxCacheSize(0)
		, m_ulLastestDbSeq(0)
	{
		FUNCTION_ENTRY("EventCache");
		
		unsigned long ulMaxCache = DEFAULT_MAXIMUM_CACHE;
		if (TA_Base_Core::RunParams::getInstance().isSet(TA_EventAgent::MAX_CACHE_SIZE.c_str()))
		{
			ulMaxCache = atol(TA_Base_Core::RunParams::getInstance().get(TA_EventAgent::MAX_CACHE_SIZE.c_str()).c_str());
			if (ulMaxCache == 0)
			{
				ulMaxCache = DEFAULT_MAXIMUM_CACHE;
			}
		}
		m_ulMaxCacheSize = ulMaxCache;
		FUNCTION_EXIT;
	}

	EventCache::~EventCache()
	{
		FUNCTION_ENTRY("~EventCache");
		deleteAllCacheData();


		FUNCTION_EXIT;
	}

	EventCache& EventCache::getInstance()
	{
		if (0 != m_pInstance)
			return *m_pInstance;
		
		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		if (0==m_pInstance)
		{
			m_pInstance = new EventCache();
		}

		return *m_pInstance;
		
	}

	void EventCache::releaseInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		if (0 != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance=0;
		}
	}

	void EventCache::deleteAllCacheData()
	{
		FUNCTION_ENTRY("deleteAllCacheData");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);
		try
		{
			if(!m_evCacheData.empty())
			{
				std::for_each(m_evCacheData.begin(), m_evCacheData.end(), 
					std::bind1st(std::mem_fun1_t<int, EventCache, TA_Base_Core::EventItem*>
					(&EventCache::deleteItem), this));
			}

			m_evCacheData.clear();
		}
		catch (...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while try to clean container");
		}		

		FUNCTION_EXIT;
	}

	int EventCache::deleteItem(TA_Base_Core::EventItem* pEventData)
	{
		delete pEventData;
		pEventData = NULL;
		return 0;
	}

	void EventCache::initialize()
	{
		FUNCTION_ENTRY("initialize");
		//Todo: Need implementation

		FUNCTION_EXIT;
	}


	void EventCache::insertEvents(TA_Base_Core::EventSequence* pEvents)
	{
		FUNCTION_ENTRY("insertEvents");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);
		unsigned long ulSize = pEvents->length();
		for (unsigned long ulIdx=0; ulIdx < ulSize; ulIdx++)
		{
			_insertInternal(&((*pEvents)[ulIdx]));
		}

		FUNCTION_EXIT;
	}

	void EventCache::insertEvents(TA_Base_Core::EvDataPkgCorbaDef* pEvents)
	{
		FUNCTION_ENTRY("insertEvents");
		T_EventItemList eventList;
		TA_Base_Core::EventUtilities::UnpackPackage(pEvents, eventList);
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "insertEvents: event count %d", eventList.size());
		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);
			//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);
			T_EventItemList::iterator itr = eventList.begin();
			for (itr; itr!=eventList.end(); itr++)
			{
				_insertInternal(*itr);
			}
		}
		eventList.clear();

		FUNCTION_EXIT;
	}

	void EventCache::insertEvents(std::vector < TA_Base_Core::EventItem* >& pEvents)
	{
		FUNCTION_ENTRY("insertEvents");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);
		std::vector < TA_Base_Core::EventItem* >::iterator itr = pEvents.begin();
		for (itr; itr!= pEvents.end(); itr++)
		{
			TA_Base_Core::EventItem* pEventData = *itr;
			
			_pushToCache(pEventData);

		}
		FUNCTION_EXIT;
	}

	void EventCache::insertEvent(TA_Base_Core::EventDetailCorbaDef* pEvent)
	{
		FUNCTION_ENTRY("insertEvent");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);
		_insertInternal(pEvent);

		FUNCTION_EXIT;
	}

	TA_Base_Core::EvDataPkgCorbaDef* EventCache::getLatestEvents(const std::string& eventID, const std::string& sessionID )
	{
		FUNCTION_ENTRY("getLastestEvents");
		//TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock, READ_ACCESS);
		TA_Base_Core::EvDataPkgCorbaDef* pRetVal = NULL;

		pRetVal = _getLatestEventsInternal(eventID, sessionID);

		FUNCTION_EXIT;
		return pRetVal;
	}

	TA_Base_Core::EvDataPkgCorbaDef* EventCache::getAllEvents(const std::string& sessionID )
	{
		FUNCTION_ENTRY("getAllEvents");
		TA_Base_Core::EvDataPkgCorbaDef* pRetVal = NULL;
		EvDataPkg* pTempDataPkg;


		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);

			//Get all events
			EvCacheData_by_evSeqNum& indexBySeqNum = boost::multi_index::get<SORT_SEQ_NUMBER>(m_evCacheData);

			unsigned long idx = 0;
			T_EventItemList tempList;
			EvCacheData_by_evSeqNum::iterator itr = indexBySeqNum.begin();
			for(itr; itr!=indexBySeqNum.end(); itr++)
			{
				tempList.push_back(*itr);
				idx++;
				if (idx >= QUERY_LIMIT)
				{
					break;
				}
			}

			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retrieve all events count %lu", tempList.size());
			std::auto_ptr<EventSerialize> pObjSerialize(new EventSerialize(tempList));
			pTempDataPkg = pObjSerialize->processEventEx();
		}

		std::auto_ptr<EventZlibDeflate> pObjDeflate( new EventZlibDeflate(pTempDataPkg) );
		std::auto_ptr<EvDataPkg> pData( pObjDeflate->processEventEx());

		pRetVal = new TA_Base_Core::EvDataPkgCorbaDef();
		pRetVal->ulDataSize = pData->ulDataLen;
		pRetVal->eventDetail.length(pData->ulCompressLen);
		CORBA::Octet* pszData = pRetVal->eventDetail.get_buffer();
		memcpy((char*)pszData, pData->pszData, pData->ulCompressLen);

		FUNCTION_EXIT;
		return pRetVal;
	}

	TA_Base_Core::EvDataPkgCorbaDef* EventCache::getTopEvents(unsigned long p_ulEventSize, const char* sessionID)
	{
		FUNCTION_ENTRY("getTopEvents");
		TA_Base_Core::EvDataPkgCorbaDef* pRetVal = NULL;
		EvDataPkg* pTempDataPkg;


		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);

			//Get all events
			EvCacheData_by_evSeqNum& indexBySeqNum = boost::multi_index::get<SORT_SEQ_NUMBER>(m_evCacheData);

			long idx = 0;
			if (indexBySeqNum.size() > p_ulEventSize)
				idx = p_ulEventSize -1;
			else
				idx = indexBySeqNum.size() -1;

			T_EventItemList tempList;
			EvCacheData_by_evSeqNum::reverse_iterator itr = indexBySeqNum.rbegin();
			for(itr; itr!=indexBySeqNum.rend(); itr++)
			{
				tempList.push_front(*itr);
				idx--;
				if (idx < 0)
				{
					break;
				}
			}
			
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get top events count %lu", tempList.size());
			std::auto_ptr<EventSerialize> pObjSerialize(new EventSerialize(tempList));
			pTempDataPkg = pObjSerialize->processEventEx();
		}
		
		std::auto_ptr<EventZlibDeflate> pObjDeflate( new EventZlibDeflate(pTempDataPkg) );
		std::auto_ptr<EvDataPkg> pData( pObjDeflate->processEventEx() );

		pRetVal = new TA_Base_Core::EvDataPkgCorbaDef();
		pRetVal->ulDataSize = pData->ulDataLen;
		pRetVal->eventDetail.length(pData->ulCompressLen);
		CORBA::Octet* pszData = pRetVal->eventDetail.get_buffer();
		memcpy((char*)pszData, pData->pszData, pData->ulCompressLen);

		FUNCTION_EXIT;
		return pRetVal;
	}

	TA_Base_Core::EvDataPkgCorbaDef* EventCache::getRelatedEvents(const std::string& alarmID, const std::string& sessionID)
	{
		FUNCTION_ENTRY("getRelatedEvents");
		TA_Base_Core::EvDataPkgCorbaDef* pRetVal = new TA_Base_Core::EvDataPkgCorbaDef();

		//Todo: Need implementation

		FUNCTION_EXIT;
		return pRetVal;
	}

	TA_Base_Core::EventSyncCorbaDef* EventCache::getSyncData(const std::string& p_eventID)
	{
		FUNCTION_ENTRY("getSyncData");
		TA_Base_Core::EventSyncCorbaDef* pRetVal = new TA_Base_Core::EventSyncCorbaDef;
		EvDataPkg* pTempDataPkg;

		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);

			EvCacheData_by_eventID& indexByID = boost::multi_index::get<SORT_EVENT_ID>(m_evCacheData);
			EvCacheData_by_eventID::iterator itFind = indexByID.find(const_cast<char*>(p_eventID.c_str()));

			if (itFind == indexByID.end())
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find event ID %s", p_eventID.c_str());
				TA_Base_Core::EventAgentCorbaDef::EventIDNotFoundException ae;
				ae.reason = CORBA::string_dup("Event ID not found.");
				throw(ae);
			}

			TA_Base_Core::EventItem* pEventData = (*itFind);

			//Get the latest events
			EvCacheData_by_evSeqNum& indexBySeqNum = boost::multi_index::get<SORT_SEQ_NUMBER>(m_evCacheData);
			EvCacheData_by_evSeqNum::iterator itr = indexBySeqNum.find(pEventData->eventSequence);

			// Advance 1 to exclude the event with the parameter ID
			itr++;

			unsigned long idx = 0;
			T_EventItemList tempList;
			for(itr; itr!=indexBySeqNum.end(); itr++)
			{
				tempList.push_back(*itr);
				idx++;
				if (idx >= QUERY_LIMIT)
				{
					break;
				}
			}

			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retrieve events count %lu", tempList.size());
			std::auto_ptr<EventSerialize> pObjSerialize(new EventSerialize(tempList));
			pTempDataPkg = pObjSerialize->processEventEx();

			//Latest database sequence number
			pRetVal->lastestDbSeqNumber = m_ulLastestDbSeq;
		}

		std::auto_ptr<EventZlibDeflate> pObjDeflate( new EventZlibDeflate(pTempDataPkg) );
		std::auto_ptr<EvDataPkg> pData( pObjDeflate->processEventEx());

		pRetVal->eventDetailSeq.ulDataSize = pData->ulDataLen;
		pRetVal->eventDetailSeq.eventDetail.length(pData->ulCompressLen);
		CORBA::Octet* pszData = pRetVal->eventDetailSeq.eventDetail.get_buffer();
		memcpy((char*)pszData, pData->pszData, pData->ulCompressLen);

		FUNCTION_EXIT;
		return pRetVal;
	}

	void EventCache::proccessSyncData(TA_Base_Core::EventSyncCorbaDef* pSyncData)
	{
		FUNCTION_ENTRY("proccessSyncData");
		T_EventItemList eventList;
		TA_Base_Core::EventUtilities::UnpackPackage(&(pSyncData->eventDetailSeq), eventList);
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "insertEvents: event count %d", eventList.size());
		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);
			//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);
			m_ulLastestDbSeq = pSyncData->lastestDbSeqNumber;

			T_EventItemList::iterator itr = eventList.begin();
			for (itr; itr!=eventList.end(); itr++)
			{
				_insertInternal(*itr);
			}
		}
		eventList.clear();

		FUNCTION_EXIT;
	}

	void EventCache::proccessDbData(std::list<TA_Base_Core::ICombinedEventData*>& events, unsigned long ulLatestDbSeq)
	{
		FUNCTION_ENTRY("proccessDbData");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);

		// Set the latest database sequence number
		m_ulLastestDbSeq = ulLatestDbSeq;
		// use reverse iterator as the first element is the latest event.
		std::list<TA_Base_Core::ICombinedEventData*>::reverse_iterator itr = events.rbegin();
		for( itr; itr != events.rend(); itr++ )
		{
			try
			{
				std::auto_ptr<TA_Base_Core::ICombinedEventData> tempEvent(*itr);
				EvCacheData_by_eventID& indexByID = boost::multi_index::get<SORT_EVENT_ID>(m_evCacheData);
				EvCacheData_by_eventID::iterator itFind = indexByID.find( const_cast<char*>(tempEvent->getEventID().c_str()));

				if (indexByID.end() != itFind)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Event ID %s already exist.",  tempEvent->getEventID().c_str());
					continue;
				}

				//Create new event data
				TA_Base_Core::EventItem* pEventData = NULL;
				pEventData = _dbDataToEventData(tempEvent.get());
				if (0 == pEventData)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to convert corba event to event data with ID", (*itr)->getEventID().c_str());
					continue;
				}

				_pushToCache(pEventData);
			}
			catch( ... )
			{
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
					"Caught an exception while trying to load alarm" );
			}
		}

		events.clear();
		FUNCTION_EXIT;
	}

	std::string EventCache::getLatestEventID()
	{
		FUNCTION_ENTRY("getLastestMgsSeqNumber");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock, READ_ACCESS);
		std::string strRetVal;
		
		EvCacheData_by_evSeqNum& indexBySeqNum = boost::multi_index::get<SORT_SEQ_NUMBER>(m_evCacheData);
		EvCacheData_by_evSeqNum::reverse_iterator itr = indexBySeqNum.rbegin();

		if (itr!=indexBySeqNum.rend())
		{
			TA_Base_Core::EventItem* ptempData = (*itr);
			strRetVal = ptempData->eventID;

		}
		
		FUNCTION_EXIT;
		return strRetVal;
	}

	unsigned long EventCache::getLatestDbSeqNumber()
	{
		FUNCTION_ENTRY("getLastestDbSeqNumber");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock,READ_ACCESS);
		unsigned long ulRetVal;
		
		ulRetVal = m_ulLastestDbSeq;

		FUNCTION_EXIT;
		return ulRetVal;
	}

	bool EventCache::findEvent(const std::string& strEventID)
	{
		FUNCTION_ENTRY("findEvent");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//ThreadRWGuard guard(m_threadLock,READ_ACCESS);
		bool bRetVal = false;

		EvCacheData_by_eventID& indexByID = boost::multi_index::get<SORT_EVENT_ID>(m_evCacheData);
		EvCacheData_by_eventID::iterator itFind = indexByID.find(const_cast<char*>(strEventID.c_str()));

		if (indexByID.end() != itFind)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Event ID %s already exist.", strEventID.c_str());
			bRetVal = true;
		}
		FUNCTION_EXIT;
		return bRetVal;
	}

	/************************************************************************/
	/* Private Methods                                                      */
	/************************************************************************/
	void EventCache::_insertInternal(TA_Base_Core::EventDetailCorbaDef* pEvent)
	{
		FUNCTION_ENTRY("_insertInternal");
		
		EvCacheData_by_eventID& indexByID = boost::multi_index::get<SORT_EVENT_ID>(m_evCacheData);
		EvCacheData_by_eventID::iterator itFind = indexByID.find(const_cast<char*>(pEvent->eventID.in()));

		if (indexByID.end() != itFind)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Event ID %s already exist.", pEvent->eventID.in());
			return;
		}

		//Create new event data
		TA_Base_Core::EventItem* pEventData = NULL;
		pEventData = _corbaDefToEventData(pEvent);
		if (0 == pEventData)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to convert corba event to event data with ID", pEvent->eventID.in());
			return;
		}

		_pushToCache(pEventData);

		FUNCTION_EXIT;
	}

	void EventCache::_insertInternal(TA_Base_Core::EventItem* pEventData)
	{
		FUNCTION_ENTRY("_insertInternal");

		EvCacheData_by_eventID& indexByID = boost::multi_index::get<SORT_EVENT_ID>(m_evCacheData);
		EvCacheData_by_eventID::iterator itFind = indexByID.find(pEventData->eventID);
		
		if (indexByID.end() != itFind)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Event ID %s already exist.", pEventData->eventID);
			delete pEventData;
			pEventData = 0;
			return;
		}
		
		_pushToCache(pEventData);

		FUNCTION_EXIT;
	}

	TA_Base_Core::EvDataPkgCorbaDef* EventCache::_getLatestEventsInternal(const std::string& eventID, const std::string& sessionID)
	{
		FUNCTION_ENTRY("_getLatestEventsInternal");
		TA_Base_Core::EvDataPkgCorbaDef* pRetVal = NULL;
		EvDataPkg* pTempDataPkg;

		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);
			
			EvCacheData_by_eventID& indexByID = boost::multi_index::get<SORT_EVENT_ID>(m_evCacheData);
			EvCacheData_by_eventID::iterator itFind = indexByID.find(const_cast<char*>(eventID.c_str()));

			if (itFind == indexByID.end())
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find event ID %s", eventID.c_str());
				TA_Base_Core::EventAgentCorbaDef::EventIDNotFoundException ae;
				ae.reason = CORBA::string_dup("Event ID not found.");
				throw(ae);
			}

			TA_Base_Core::EventItem* pEventData = (*itFind);

			//Get the latest events
			EvCacheData_by_evSeqNum& indexBySeqNum = boost::multi_index::get<SORT_SEQ_NUMBER>(m_evCacheData);
			EvCacheData_by_evSeqNum::iterator itr = indexBySeqNum.find(pEventData->eventSequence);

			// Advance 1 to exclude the event with the parameter ID
			itr++;

			unsigned long idx = 0;
			T_EventItemList tempList;
			for(itr; itr!=indexBySeqNum.end(); itr++)
			{
				tempList.push_back(*itr);
				idx++;
				if (idx >= QUERY_LIMIT)
				{
					break;
				}
			}
			
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retrieve events count %lu", tempList.size());
			std::auto_ptr<EventSerialize> pObjSerialize(new EventSerialize(tempList));
			pTempDataPkg = pObjSerialize->processEventEx();
		}
		
		std::auto_ptr<EventZlibDeflate> pObjDeflate( new EventZlibDeflate(pTempDataPkg) );
		std::auto_ptr<EvDataPkg> pData( pObjDeflate->processEventEx());

		pRetVal = new TA_Base_Core::EvDataPkgCorbaDef();
		pRetVal->ulDataSize = pData->ulDataLen;
		pRetVal->eventDetail.length(pData->ulCompressLen);
		CORBA::Octet* pszData = pRetVal->eventDetail.get_buffer();
		memcpy((char*)pszData, pData->pszData, pData->ulCompressLen);


		FUNCTION_EXIT;
		return pRetVal;
	}

	const EvMgsSequenceNumber& EventCache::_getNextMsgSeqNum()
	{
		FUNCTION_ENTRY("_getNextMsgSeqNum");
		m_evMsgSeqNumber.ulLowOrder++;

		if( m_evMsgSeqNumber.ulLowOrder == ULONG_MAX )
		{
			m_evMsgSeqNumber.ulHighOrder++;
			m_evMsgSeqNumber.ulLowOrder = 0;
		}

		FUNCTION_EXIT;
		return m_evMsgSeqNumber;
	}

	TA_Base_Core::EventItem* EventCache::_corbaDefToEventData(TA_Base_Core::EventDetailCorbaDef* pEvent)
	{
		FUNCTION_ENTRY("_corbaDefToEventData");

		TA_Base_Core::EventItem* pRetVal = NULL;

		try
		{
			pRetVal = new TA_Base_Core::EventItem(
				pEvent->eventID.in(),
				pEvent->sourceTable.in(),
				TA_Base_Core::MillisecondTimeEx(pEvent->createTime.time, 
					pEvent->createTime.millitm),
				pEvent->subsystemKey,
				pEvent->physicalSubsystemKey,
				pEvent->locationKey,
				pEvent->severityKey,
				pEvent->severityname.in(),
				pEvent->assetname.in(),
				pEvent->description.in(),
				pEvent->eventTypeKey,
				pEvent->eventTypeName.in(),
				pEvent->alarmID.in(),
				pEvent->alarmTypeKey,
				pEvent->value.in(),
				pEvent->mms.in(),
				pEvent->decisionSupportState.in(),
				pEvent->avalanceState.in(),
				pEvent->operatorKey,
				pEvent->operatorName.in(),
				pEvent->alarmComment.in(),
				pEvent->eventLevel,
				pEvent->alarmAckedBy.in(),
				pEvent->alarmStatus.in(),
				pEvent->sessionID.in(),
				pEvent->sessionLocation,
				pEvent->profileID,
				pEvent->actionID,
				pEvent->operatorMode.in(),
				pEvent->entitykey,
				pEvent->avlheadID.in(),
				pEvent->systemKey,
				TA_Base_Core::EvMgsSequenceNumber(pEvent->eventSequence.ulHighOrder, 
					pEvent->eventSequence.ulLowOrder));
		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception occurred while trying to convert corba type to event data type.");
		}
		
		return pRetVal;
		FUNCTION_EXIT;
	}

	TA_Base_Core::EventItem* EventCache::_dbDataToEventData(TA_Base_Core::ICombinedEventData* pEventData)
	{
		FUNCTION_ENTRY("_eventDataToEventMessage");
		
		TA_Base_Core::EventItem* pRetVal = NULL;

		try
		{
			pRetVal = new TA_Base_Core::EventItem(
				pEventData->getEventID().c_str(),
				pEventData->getSourceTable().c_str(),
				TA_Base_Core::MillisecondTimeEx(pEventData->getCreateTime().time, 
				pEventData->getCreateTime().millitm),
				pEventData->getSubsystemKey(),
				pEventData->getPhysicalSubsystemKey(),
				pEventData->getLocationKey(),
				pEventData->getSeverityKey(),
				pEventData->getAlarmSeverityName().c_str(),
				pEventData->getAssetName().c_str(),
				pEventData->getDescription().c_str(),
				pEventData->getEventTypeKey(),
				pEventData->getEventTypeName().c_str(),
				pEventData->getAlarmId().c_str(),
				pEventData->getAlarmTypeKey(),
				pEventData->getValue().c_str(),
				pEventData->getAlarmMmsState().c_str(),
				pEventData->getAlarmDssState().c_str(),
				pEventData->getAlarmAvlState().c_str(),
				pEventData->getOperatorKey(),
				pEventData->getOperatorName().c_str(),
				pEventData->getAlarmComment().c_str(),
				pEventData->getEventLevel(),
				pEventData->getAlarmAck().c_str(),
				pEventData->getAlarmStatus().c_str(),
				pEventData->getSessionKey().c_str(),
				pEventData->getSessionLocation(),
				pEventData->getProfileId(),
				pEventData->getActionId(),
				pEventData->getOperationMode().c_str(),
				pEventData->getEntityKey(),
				pEventData->getAvlAlarmHeadId().c_str(),
				pEventData->getSystemKey(),
				TA_Base_Core::EvMgsSequenceNumber(0, 0));

		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception occurred while trying to convert ICombinedEventData type to event data type.");
		}

		FUNCTION_EXIT;
		return pRetVal;
	}

	void EventCache::_eventDataToCorbaDef(TA_Base_Core::EventItem* pEventData, TA_Base_Core::EventDetailCorbaDef& refRetVal)
	{
		FUNCTION_ENTRY("_eventDataToCorbaDef");

		refRetVal.eventID = CORBA::string_dup(pEventData->eventID);
		refRetVal.sourceTable = CORBA::string_dup(pEventData->sourceTable);
		refRetVal.createTime.time = pEventData->createTime.time;
		refRetVal.createTime.millitm = pEventData->createTime.millitm;
		refRetVal.subsystemKey = pEventData->subsystemKey; 
		refRetVal.physicalSubsystemKey = pEventData->physicalSubsystemKey;
		refRetVal.locationKey = pEventData->locationKey;
		refRetVal.severityKey = pEventData->severityKey;
		refRetVal.severityname = CORBA::string_dup(pEventData->severityname);
		refRetVal.assetname = CORBA::string_dup(pEventData->assetname);
		refRetVal.description = CORBA::string_dup(pEventData->description);
		refRetVal.eventTypeKey = pEventData->eventTypeKey;
		refRetVal.eventTypeName = CORBA::string_dup(pEventData->eventTypeName);
		refRetVal.alarmID = CORBA::string_dup(pEventData->alarmID);
		refRetVal.alarmTypeKey = pEventData->alarmTypeKey;
		refRetVal.value = CORBA::string_dup(pEventData->value);
		refRetVal.mms = CORBA::string_dup(pEventData->mms);
		refRetVal.decisionSupportState = CORBA::string_dup(pEventData->decisionSupportState);
		refRetVal.avalanceState = CORBA::string_dup(pEventData->avalanceState);
		refRetVal.operatorKey = pEventData->operatorKey;
		refRetVal.operatorName = CORBA::string_dup(pEventData->operatorName);
		refRetVal.alarmComment = CORBA::string_dup(pEventData->alarmComment);
		refRetVal.eventLevel = pEventData->eventLevel;
		refRetVal.alarmAckedBy = CORBA::string_dup(pEventData->alarmAckedBy);
		refRetVal.alarmStatus = CORBA::string_dup(pEventData->alarmStatus);
		refRetVal.sessionID = CORBA::string_dup(pEventData->sessionID);
		refRetVal.sessionLocation = pEventData->sessionLocation;
		refRetVal.profileID = pEventData->profileID;
		refRetVal.actionID = pEventData->actionID;
		refRetVal.operatorMode = CORBA::string_dup(pEventData->operatorMode);
		refRetVal.entitykey = pEventData->entitykey;
		refRetVal.avlheadID = CORBA::string_dup(pEventData->avlheadID);
		refRetVal.systemKey = pEventData->systemKey;

		FUNCTION_EXIT;
	}

	void EventCache::_pushToCache(TA_Base_Core::EventItem* pEventData)
	{
		FUNCTION_ENTRY("_pushToCache");

		// Delete the oldest event
		if (m_evCacheData.size() >= m_ulMaxCacheSize)
		{
			EvCacheData_by_evSeqNum& indexBySeqNum = boost::multi_index::get<SORT_SEQ_NUMBER>(m_evCacheData);
			EvCacheData_by_evSeqNum::iterator itDel = indexBySeqNum.begin();
			
			TA_Base_Core::EventItem* pDelData = (*itDel);
			if (0 != pDelData)
			{
				//LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Maximum cache capacity reach, delete oldest event data with ID %s", pDelData->eventID.c_str());

				delete pDelData;
				pDelData = NULL;
			}
			indexBySeqNum.erase(itDel);
		}		
		//Assign sequence number
		pEventData->eventSequence = _getNextMsgSeqNum();
		//Insert into cache
		std::pair<EvCacheData::iterator,bool> pRetVal = m_evCacheData.insert(pEventData);
		if (pRetVal.second == false)
		{
			//Duplicate ID
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Event ID %s already exist.", pEventData->eventID);	
			delete pEventData;
			pEventData = NULL;
		}
// 		else
// 			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Insert event into cache. size %lu, event ID %s", m_evCacheData.size(), pEventData->eventID);

		FUNCTION_EXIT;
	}

}
