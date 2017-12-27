/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/EventAgentImpl.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "app/event/event_agent/src/EventAgentImpl.h"
#include "app/event/event_agent/src/PropagateActorLocal.h"
#include "app/event/event_agent/src/PropagateActorCentral.h"
#include "app/event/event_agent/src/DbEventRetriever.h"
#include "app/event/event_agent/src/EventCache.h"
#include "app/event/event_agent/src/LocalRunParamDefinitions.h"
//#include "app/event/event_agent/src/ThreadRWGuard.h"
#include "app/event/event_agent/src/EventDataProcessor.h"

#include "core/event/src/EventUtilities.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;


namespace TA_Base_App
{
	/**
      * Constructor
      */
	EventAgentImpl::EventAgentImpl()
		: m_pPropagateActor(0)
		, m_bStopDataLoading(false)
	{
		FUNCTION_ENTRY("EventAgentImpl::submitEvent");
		
		FUNCTION_EXIT;
	}

    /**
     * Destructor
     */
	EventAgentImpl::~EventAgentImpl()
	{
		FUNCTION_ENTRY("EventAgentImpl::submitEvent");
		m_bStopDataLoading = true;
		DbEventRetriever::getInstance().releaseInstance();
		
		if (0 != m_pPropagateActor)
		{
			delete m_pPropagateActor;
			m_pPropagateActor = 0;
		}
		
		FUNCTION_EXIT;
	}

	/**
     * submitEvent
	 */
	void EventAgentImpl::submitEvent(const ::TA_Base_Core::EventInfoCorbaDef& p_eventData)
	{
		FUNCTION_ENTRY("EventAgentImpl::submitEvent");
		EnsureInControlState();
		
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received event with ID = %s", p_eventData.eventDetail.eventID.in() );
		
		TA_Base_Core::EventItem* pEventItem = EventDataProcessor::getInstance().processEventInfo(p_eventData);
		m_pPropagateActor->pushEvent(pEventItem);
	
		FUNCTION_EXIT;
	}

	/**
     * submitEvents
	 */
	void EventAgentImpl::submitEvents(const ::TA_Base_Core::EventInfoSequence& p_eventDataSeq)
	{
		FUNCTION_ENTRY("EventAgentImpl::submitEvents");
		EnsureInControlState();
		
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "received events size = %u from local event source", p_eventDataSeq.length());

		// Process events
		TA_Base_Core::T_EventItemList eventList;
		EventDataProcessor::getInstance().processEventInfo(p_eventDataSeq, eventList);

		m_pPropagateActor->push(eventList);

		FUNCTION_EXIT;
	}
	
	/**
     * submitEventsEx
	 */
	void EventAgentImpl::submitEventsEx(const ::TA_Base_Core::EvDataPkgCorbaDef& p_eventDataSeq, ::CORBA::ULong p_ulSourceLoc)
	{
		FUNCTION_ENTRY("EventAgentImpl::submitEvents");
		EnsureInControlState();
		
		// Process events
		TA_Base_Core::T_EventItemList eventList;
		
		TA_Base_Core::EventUtilities::UnpackPackage(&p_eventDataSeq, eventList);
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "received events size = %u from location %u", eventList.size(), p_ulSourceLoc);

		m_pPropagateActor->push(eventList);

		FUNCTION_EXIT;
	}
	
	/**
     * getAllEvents
	 */
	EvDataPkgCorbaDef* EventAgentImpl::getAllEvents(const char* sessionID)
	{
		FUNCTION_ENTRY("EventAgentImpl::getAllEvents");
		EnsureInControlState();
		
		EvDataPkgCorbaDef* pRetVal = NULL;

		pRetVal = EventCache::getInstance().getAllEvents(sessionID);

		FUNCTION_EXIT;
		return pRetVal;
	}

	/**
     * getLatestEvents
	 */
	TA_Base_Core::EvDataPkgCorbaDef* EventAgentImpl::getLatestEvents(const char* eventID, const char* sessionID)
	{
		FUNCTION_ENTRY("EventAgentImpl::getLatestEvents");
		EnsureInControlState();
		
		EvDataPkgCorbaDef* pRetVal = NULL;

		pRetVal = EventCache::getInstance().getLatestEvents(eventID, sessionID);

		FUNCTION_EXIT;
		return pRetVal;
	}
	
	/**
     * getRelatedEvents
	 */
	TA_Base_Core::EvDataPkgCorbaDef* EventAgentImpl::getRelatedEvents(const char* alarmID, const char* sessionID)
	{
		FUNCTION_ENTRY("EventAgentImpl::getRelatedEvents");
		EnsureInControlState();
		
		EvDataPkgCorbaDef* pRetVal = NULL;

		pRetVal = EventCache::getInstance().getRelatedEvents(alarmID, sessionID);

		FUNCTION_EXIT;
		return pRetVal;
	}

	/**
      * getSyncData
      */
	EventSyncCorbaDef* EventAgentImpl::getSyncData(const char* p_eventID)
	{
		FUNCTION_ENTRY("EventAgentImpl::getSyncData");
		EnsureInControlState();
		
		EventSyncCorbaDef* pRetVal = NULL;

		pRetVal = EventCache::getInstance().getSyncData(std::string(p_eventID));

		FUNCTION_EXIT;
		return pRetVal;
	}

	/**
	 * initialize
	 */
	void EventAgentImpl::initialize()
	{
		FUNCTION_ENTRY("initialize");
		//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);
		ThreadGuard guard(m_threadLock);

		unsigned long ulMyLocation = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
		if (ulMyLocation == TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey())
		{
			m_pPropagateActor = new PropagateActorCentral();
			//m_pPropagateActor = new PropagateActorLocal();
		}
		else
		{
			m_pPropagateActor = new PropagateActorLocal();
		}

		activateServantWithName(RunParams::getInstance().get(RPARAM_ENTITYNAME));

// 		//Load data from database
// 		DbEventRetriever::getInstance().LoadData();
// 
// 		//Question: do we need to wait until database loading is complete???
// 		while (!DbEventRetriever::getInstance().hasDataLoaded())
// 		{
// 			TA_Base_Core::Thread::sleep(100);
// 		}

		FUNCTION_EXIT;
	}


	/**
	 * StartStateProcess
	 */
	void EventAgentImpl::changeStateProcess(const std::string& oldState, const std::string& newState)
	{
		FUNCTION_ENTRY("changeStateProcess");

		// =========== Note===============
		// We stop the current state processes without holding the
		// lock to avoid deadlock during failover from monitor -> control
		_stopCurrentStateProcess(oldState);
		// Stop current state processes

		//ThreadRWGuard guard(m_threadLock, WRITE_ACCESS);
		ThreadGuard guard(m_threadLock);
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Starting new state process begin.");
		if (oldState.compare(RPARAM_STANDBY) == 0 ||
			(oldState.empty() == true && newState.compare(RPARAM_STANDBY) != 0))
		{
			//Cache necessary information for event data processing
			EventDataProcessor::getInstance().initialize();

			//Load data from database
			DbEventRetriever::getInstance().resetDataLoader();
			DbEventRetriever::getInstance().LoadData();

			//Question: do we need to wait until database loading is complete???
			while (!DbEventRetriever::getInstance().hasDataLoaded() && !m_bStopDataLoading)
			{
				TA_Base_Core::Thread::sleep(100);
			}

			if (newState.compare(RPARAM_CONTROL) == 0)
				m_pPropagateActor->StartPropagation();

		}
		else if (newState.compare(RPARAM_CONTROL) == 0)
		{
			m_pPropagateActor->StartPropagation();
			DbEventRetriever::getInstance().startDbRefresh();
		}
		else if (newState.compare(RPARAM_MONITOR) == 0)
		{
			DbEventRetriever::getInstance().startAgentSync();	
		}
		else if (newState.compare(RPARAM_STANDBY) == 0)
		{
			//Todo: unload data in cache
		}
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Starting new state process end.");

		FUNCTION_EXIT;
	}


	/**
	 * _stopCurrentStateProcess
	 */
	void EventAgentImpl::_stopCurrentStateProcess(const std::string& oldState)
	{
		//FUNCTION_ENTRY("changeStateProcess");
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Stopping current state process begin.");

		if (oldState.compare(RPARAM_CONTROL) == 0)
		{
			m_pPropagateActor->StopPropagation();
			DbEventRetriever::getInstance().terminateAndWait();
		}
		else if (oldState.compare(RPARAM_MONITOR) == 0)
		{
			DbEventRetriever::getInstance().terminateAndWait();
		}
		// RPARM 

		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Stopping current state process end.");
		//FUNCTION_EXIT;
	}

	/**
     * EnsureInControlState
	 */
	void EventAgentImpl::EnsureInControlState()
	{
		//FUNCTION_ENTRY("EnsureInControlState");
		if( 0 != TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
		{

			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
				"Event Agent is not in control mode");
			throw(TA_Base_Core::OperationModeException(TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).c_str()));
		}

		//Lock to ensure we process request when agent is not switching from state
		ThreadGuard guard(m_threadLock);

		//FUNCTION_EXIT;
	}

	/**
      * checkStatus
      */
	CORBA::Boolean EventAgentImpl::checkStatus()
	{
		FUNCTION_ENTRY("checkStatus");
		FUNCTION_EXIT;
		return true;
	}

	EvDataPkgCorbaDef* EventAgentImpl::getTopEvents(::CORBA::ULong p_ulEventSize, const char* sessionID)
	{
		FUNCTION_ENTRY("EventAgentImpl::getTopEvents");
		EnsureInControlState();
		
		EvDataPkgCorbaDef* pRetVal = NULL;

		pRetVal = EventCache::getInstance().getTopEvents(p_ulEventSize, sessionID);


		FUNCTION_EXIT;
		return pRetVal;
	}
}
