/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/PropagateActorLocal.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "app/event/event_agent/src/PropagateActorLocal.h"
#include "app/event/event_agent/src/EventCache.h"
#include "app/event/event_agent/src/LocalRunParamDefinitions.h"


#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EventAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/event/IDL/src/EventAgentCorbaDef.h"
#include "core/event/src/EventUtilities.h"
#include "core/event/src/EventSerialize.h"
#include "core/event/src/EventZlibDeflate.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#define DEFAULT_PROPAGATE_INTERVAL 100
#define DEFAULT_MAX_PROPAGATE_ITEM 500
#define DEFAULT_MAX_PROPAGETE_CACHE 100000

using namespace TA_Base_Core;

namespace TA_Base_App
{

	/**
	 * Constructor
	 */
	PropagateActorLocal::PropagateActorLocal()
		:m_bTerminate(false)
		,m_ulEvSubmitInterval(0)
		,m_ulMyLocation(0)
		,m_ulMaxCacheSize(0)
		,m_ulEventBatchSize(0)
	{
		FUNCTION_ENTRY("PropagateActorLocal");
	
		// Set Occ Event Agent 
		std::vector<std::string> vecAgentNames = EntityAccessFactory::getInstance().getAgentNamesOfTypeAtLocation(
			EventAgentEntityData::getStaticType(),
			LocationAccessFactory::getInstance().getOccLocationKey()
			);

		if (vecAgentNames.size() > 1)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There is more than one Event agent servant at location %li", LocationAccessFactory::getInstance().getOccLocationKey() );
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Event propagation to location %li will be disabled.", LocationAccessFactory::getInstance().getOccLocationKey());
			return;
		}  

		std::string strOccEventAgent = vecAgentNames[0];
		m_eventAgentNameObj.setEntityName(strOccEventAgent, true);
		
		// Set Propagate Interval
		unsigned long ulInterval = DEFAULT_PROPAGATE_INTERVAL;
		if (TA_Base_Core::RunParams::getInstance().isSet(TA_EventAgent::EVENT_SUBMIT_INTERVAL.c_str()))
		{
			ulInterval = atol(TA_Base_Core::RunParams::getInstance().get(TA_EventAgent::EVENT_SUBMIT_INTERVAL.c_str()).c_str());
			if (ulInterval == 0)
			{
				ulInterval = DEFAULT_PROPAGATE_INTERVAL;
			}
		}
		m_ulEvSubmitInterval = ulInterval;

		unsigned long ulMaxCache = DEFAULT_MAX_PROPAGETE_CACHE;
		if (TA_Base_Core::RunParams::getInstance().isSet(TA_EventAgent::MAX_CACHE_SIZE.c_str()))
		{
			ulMaxCache = atol(TA_Base_Core::RunParams::getInstance().get(TA_EventAgent::MAX_CACHE_SIZE.c_str()).c_str());
			if (ulMaxCache == 0)
			{
				ulMaxCache = DEFAULT_MAX_PROPAGETE_CACHE;
			}
		}
		m_ulMaxCacheSize = ulMaxCache;

		unsigned long ulBatchSize = DEFAULT_MAX_PROPAGATE_ITEM;
		if (TA_Base_Core::RunParams::getInstance().isSet(TA_EventAgent::PROPAGATE_BATCH_SIZE.c_str()))
		{
			ulMaxCache = atol(TA_Base_Core::RunParams::getInstance().get(TA_EventAgent::PROPAGATE_BATCH_SIZE.c_str()).c_str());
			if (ulMaxCache == 0)
			{
				ulBatchSize = DEFAULT_MAX_PROPAGATE_ITEM;
			}
		}
		m_ulEventBatchSize = ulBatchSize;

		// Get this agents location
		m_ulMyLocation = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());

		m_receiveCache.start();
		FUNCTION_EXIT;
	}
	
	/**
	 * Destructor
	 */
	PropagateActorLocal::~PropagateActorLocal()
	{
		FUNCTION_ENTRY("PropagateActorLocal");
		m_receiveCache.terminateAndWait();

		terminateAndWait();

		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//Deallocate all objects in the queue
		unsigned long ulSize = m_eventQueue.size();
		for(unsigned long ulIdx=0; ulIdx < ulSize; ulIdx++)
		{
			TA_Base_Core::EventItem* pEventData = m_eventQueue.front();
			delete pEventData;
			pEventData = 0;
			
			m_eventQueue.pop_front();
		}

		FUNCTION_EXIT;
	}

    /**
      * run
      */
	void PropagateActorLocal::run()
	{
		FUNCTION_ENTRY("run");


		while(!m_bTerminate)
		{
			m_conditionLock.timedWait(m_ulEvSubmitInterval);
			if (m_bTerminate)
				break;

			T_EventItemList cachedData;
			bool bSuccessful = false;
			_getCachedData(cachedData);

			try
			{
				if (cachedData.size()>0)
				{
					TA_Base_Core::EvDataPkgCorbaDef packageData;
					std::auto_ptr<EventSerialize> pObjSerialize(new EventSerialize(cachedData));
					std::auto_ptr<EventZlibDeflate> pObjDeflate(new EventZlibDeflate(pObjSerialize.get()));
					std::auto_ptr<EvDataPkg> pData (pObjDeflate->processEventEx());

					packageData.ulDataSize = pData->ulDataLen;
					packageData.eventDetail.length(pData->ulCompressLen);
					CORBA::Octet* pszData = packageData.eventDetail.get_buffer();
					memcpy((char*)pszData, pData->pszData, pData->ulCompressLen);

					//Submit these events to central Event Agent
					CORBA_CALL(m_eventAgentNameObj, 
						submitEventsEx, 
						(packageData, m_ulMyLocation));
				}
				
				//Release EventItem
				T_EventItemList::iterator delIter= cachedData.begin();
				for (delIter; delIter!=cachedData.end(); delIter++)
				{
					delete (*delIter);
				}
				cachedData.clear();

				bSuccessful = true;
			}
			catch(const CORBA::SystemException& ex)
			{
				LOG0( SourceInfo, 
					TA_Base_Core::DebugUtil::DebugError, "caught EventAgent exception ");
			}
			catch( const TA_Base_Core::ObjectResolutionException& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
					TA_Base_Core::DebugUtil::DebugError, "failed to resolve agent ", ex.what());
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
					TA_Base_Core::DebugUtil::DebugError, "caught unknown corba exception ", ex._name());
			}
			catch( ... )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
					TA_Base_Core::DebugUtil::DebugError, "caught unknown exception");
			}	
			//If we fail to submit then push it back into the queue
			if (!bSuccessful)
				_pushBackToCache(cachedData);
			
			//TA_Base_Core::Thread::sleep(m_ulEvSubmitInterval);
		}

		FUNCTION_EXIT;
	}

    /**
      * terminate
      */
	void PropagateActorLocal::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_bTerminate = true;
		m_conditionLock.signal();

		FUNCTION_EXIT;
	}

	/**
	 * push
	 */
	void PropagateActorLocal::push(TA_Base_Core::T_EventItemList& pEvents)
	{
		FUNCTION_ENTRY("push");

		m_receiveCache.pushEvents(pEvents);
	
		// Insert into internal cache
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		TA_Base_Core::T_EventItemList::iterator itr = pEvents.begin();
		for (itr; itr!=pEvents.end(); itr++)
		{
			TA_Base_Core::EventItem* pEventData = new TA_Base_Core::EventItem(*(*itr));
			if (m_eventQueue.size() >= m_ulMaxCacheSize)
			{
				//Delete the oldest event in the cache
				TA_Base_Core::EventItem* temp = m_eventQueue.front();
				delete temp;
				temp=NULL;

				m_eventQueue.pop_front();
			}
			
			m_eventQueue.push_back(pEventData);
			if (m_eventQueue.size() >= m_ulEventBatchSize)
			{
				//Signal propagate event
				m_conditionLock.signal();
			}
		}

		FUNCTION_EXIT;
	}

	/**
	 * pushEvent
	 */
	void PropagateActorLocal::pushEvent(TA_Base_Core::EventItem* pEvent)
	{
		FUNCTION_ENTRY("push");
		TA_ASSERT(0 != pEvent, "pEvents must be a valid pointer.");
	
		// Insert into event cache
		m_receiveCache.pushEvent(pEvent);

		// Insert into internal cache
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		TA_Base_Core::EventItem* pEventData = new TA_Base_Core::EventItem(*pEvent);
		
		if (m_eventQueue.size() >= m_ulMaxCacheSize)
		{
			//Delete the oldest event in the cache
			TA_Base_Core::EventItem* temp = m_eventQueue.front();
			delete temp;
			temp=NULL;

			m_eventQueue.pop_front();
		}
		else if (m_eventQueue.size() >= m_ulEventBatchSize)
		{
			//Signal propagate event
			m_conditionLock.signal();
		}

		m_eventQueue.push_back(pEventData);
		
		FUNCTION_EXIT;
	}

	/**
	 * _getCachedData
	 */
	void PropagateActorLocal::_getCachedData(TA_Base_Core::T_EventItemList& cachedData)
	{
		FUNCTION_ENTRY("_getCachedData");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		//Determine the size of the queue
		int nSize = 0;
		if (m_eventQueue.size() > DEFAULT_MAX_PROPAGATE_ITEM)
			nSize = DEFAULT_MAX_PROPAGATE_ITEM;
		else
			nSize = m_eventQueue.size();

		for (int idx=0; idx<nSize; idx++)
		{
			TA_Base_Core::EventItem* pEventData = m_eventQueue.front();
			cachedData.push_back(pEventData);
			m_eventQueue.pop_front();
		}

		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Event size to propagate %d", cachedData.size());
		FUNCTION_EXIT;
	}

	/**
	 * _pushBackToCache
	 */
	void PropagateActorLocal::_pushBackToCache(TA_Base_Core::T_EventItemList& cachedData)
	{
		FUNCTION_ENTRY("_pushBackToCache");

		// Insert into internal cache
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		int nSize = cachedData.size();

		T_EventItemList::reverse_iterator itr = cachedData.rbegin();
		for (itr; itr!=cachedData.rend(); itr++)
		{
			TA_Base_Core::EventItem* pEventData = *itr;
			if (m_eventQueue.size() >= m_ulMaxCacheSize)
			{
				//Delete the oldest event in the cache
				delete pEventData;
				pEventData=NULL;
				//m_eventQueue.pop_front();
			}
			else
				m_eventQueue.push_front(pEventData);
		}

		FUNCTION_EXIT;
	}

	/**
	 * StartPropagation
	 */
	void PropagateActorLocal::StartPropagation()
	{
		FUNCTION_ENTRY("StartPropagation");
		m_bTerminate = false;
		start();
		FUNCTION_EXIT;
	}

	/**
	 * StopPropagation
	 */
	void PropagateActorLocal::StopPropagation()
	{
		FUNCTION_ENTRY("StopPropagation");
		terminateAndWait();
		FUNCTION_EXIT;
	}
}