/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/ReceiveEventCache.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "app/event/event_agent/src/ReceiveEventCache.h"
#include "app/event/event_agent/src/EventCache.h"
#include "app/event/event_agent/src/LocalRunParamDefinitions.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#define DEFAULT_MAX_RECEIVECACHE 100000
#define DEFAULT_CACHE_MOVE_INTERVAL 100

using namespace TA_Base_Core;

namespace TA_Base_App
{
	ReceiveEventCache::ReceiveEventCache()
		: m_bTerminate(false)
		, m_ulMaxCacheSize(0)
	{
		FUNCTION_ENTRY("ReceiveEventCache");
		unsigned long ulMaxCache = DEFAULT_MAX_RECEIVECACHE;
		if (TA_Base_Core::RunParams::getInstance().isSet(TA_EventAgent::MAX_CACHE_SIZE.c_str()))
		{
			ulMaxCache = atol(TA_Base_Core::RunParams::getInstance().get(TA_EventAgent::MAX_CACHE_SIZE.c_str()).c_str());
			if (ulMaxCache == 0)
			{
				ulMaxCache = DEFAULT_MAX_RECEIVECACHE;
			}
		}
		m_ulMaxCacheSize = ulMaxCache;

		FUNCTION_EXIT;
	}

	ReceiveEventCache::~ReceiveEventCache()
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();

		TA_Base_Core::ThreadGuard guard(m_threadLock);
		while(m_recEventsQueue.size() != 0)
		{
			TA_Base_Core::EventItem* pTemp = m_recEventsQueue.front();
			delete pTemp;
			pTemp = NULL;

			m_recEventsQueue.pop();
		}

		FUNCTION_EXIT;
	}

	void ReceiveEventCache::run()
	{
		FUNCTION_ENTRY("ReceiveEventCache::run");
		while(!m_bTerminate)
		{
			T_vecEventData eventSeq;
			_getQueueItem(eventSeq);
			if (0 != eventSeq.size())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Moving events from Received cache to EventCache size=%u", eventSeq.size());
				EventCache::getInstance().insertEvents(eventSeq);
				
			}
	
			TA_Base_Core::Thread::sleep(DEFAULT_CACHE_MOVE_INTERVAL);
		}

		FUNCTION_EXIT;
	}

	void ReceiveEventCache::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_bTerminate = true;

		FUNCTION_EXIT;
	}

	void ReceiveEventCache::pushEvent(::TA_Base_Core::EventItem* p_eventData)
	{
		FUNCTION_ENTRY("pushEvent");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		//TA_Base_Core::EventItem* pEventData = _corbaDefToEventData(p_eventData);
		if (m_recEventsQueue.size() >= m_ulMaxCacheSize)
		{
			TA_Base_Core::EventItem *pDetail = m_recEventsQueue.front();
			delete pDetail;
			pDetail = NULL;
			
			m_recEventsQueue.pop();
		}
		
		m_recEventsQueue.push(p_eventData);
		FUNCTION_EXIT;
	}

	void ReceiveEventCache::pushEvents(TA_Base_Core::T_EventItemList& pEvents)
	{
		FUNCTION_ENTRY("pushEvents");
		// Insert into internal cache
		//unsigned long ulSize = pEvents.size();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "push Events into cache size=%u", pEvents.size());
		TA_Base_Core::T_EventItemList::iterator itr = pEvents.begin();
		for (itr; itr!=pEvents.end(); ++itr)
		{
			TA_Base_Core::EventItem* pEventData = (*itr);
			{
				TA_Base_Core::ThreadGuard guard(m_threadLock);
				if (m_recEventsQueue.size() >= m_ulMaxCacheSize)
				{
					TA_Base_Core::EventItem* pDetail = m_recEventsQueue.front();
					delete pDetail;
					pDetail = NULL;

					m_recEventsQueue.pop();
				}

				m_recEventsQueue.push(pEventData);
			}
		}

		FUNCTION_EXIT;
	}

	unsigned long ReceiveEventCache::_getQueueSize()
	{
		FUNCTION_ENTRY("_getQueueSize");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		FUNCTION_EXIT;
		return m_recEventsQueue.size();
	}

	void ReceiveEventCache::_getQueueItem(T_vecEventData& pEvents)
	{
		FUNCTION_ENTRY("_getQueueItem");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		
		if (m_recEventsQueue.size() != 0)
		{
			//Resize container
			pEvents.reserve(m_recEventsQueue.size());
			
			while (m_recEventsQueue.size() != 0)
			{
				pEvents.push_back(m_recEventsQueue.front());
				m_recEventsQueue.pop();
			}
		}

		FUNCTION_EXIT;
	}

	TA_Base_Core::EventItem* ReceiveEventCache::_corbaDefToEventData(const TA_Base_Core::EventDetailCorbaDef& p_Event)
	{
		FUNCTION_ENTRY("_corbaDefToEventData");

		TA_Base_Core::EventItem* pRetVal = NULL;

		try
		{
			pRetVal = new TA_Base_Core::EventItem(
				p_Event.eventID.in(),
				p_Event.sourceTable.in(),
				TA_Base_Core::MillisecondTimeEx(p_Event.createTime.time, 
				p_Event.createTime.millitm),
				p_Event.subsystemKey,
				p_Event.physicalSubsystemKey,
				p_Event.locationKey,
				p_Event.severityKey,
				p_Event.severityname.in(),
				p_Event.assetname.in(),
				p_Event.description.in(),
				p_Event.eventTypeKey,
				p_Event.eventTypeName.in(),
				p_Event.alarmID.in(),
				p_Event.alarmTypeKey,
				p_Event.value.in(),
				p_Event.mms.in(),
				p_Event.decisionSupportState.in(),
				p_Event.avalanceState.in(),
				p_Event.operatorKey,
				p_Event.operatorName.in(),
				p_Event.alarmComment.in(),
				p_Event.eventLevel,
				p_Event.alarmAckedBy.in(),
				p_Event.alarmStatus.in(),
				p_Event.sessionID.in(),
				p_Event.sessionLocation,
				p_Event.profileID,
				p_Event.actionID,
				p_Event.operatorMode.in(),
				p_Event.entitykey,
				p_Event.avlheadID.in(),
				p_Event.systemKey,
				TA_Base_Core::EvMgsSequenceNumber(p_Event.eventSequence.ulHighOrder, 
				p_Event.eventSequence.ulLowOrder));

		}
		catch(...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception occurred while trying to convert corba type to event data type.");
		}

		return pRetVal;
		FUNCTION_EXIT;
	}
}
