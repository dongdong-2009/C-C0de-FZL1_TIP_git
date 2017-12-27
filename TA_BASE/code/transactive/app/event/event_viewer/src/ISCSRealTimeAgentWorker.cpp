/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSRealTimeAgentWorker.cpp $
  * @author 
  * @version $Revision: 
  *
  * Last modification: $DateTime:
  * Last modified by: 
  * 
  * ISCSRealTimeAgentWorker is a thread for for loading events from event agent 
  * or checking agent status when after function fail. 
  *
  */


#include "ISCSRealTimeAgentWorker.h"
#include "app/event/event_viewer/src/IEventWorkObserver.h"
#include "app/event/event_viewer/src/FilterCriteria.h"
#include "app/event/event_viewer/src/ConstantItems.h"
#include "app/event/event_viewer/src/EventHandlerManager.h"

#include "core/event/src/EventHelper.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/event/src/EventHelperException.h"

#define RPARAM_DISABLEAGENTCHENCK "DisableAgentCheck"


namespace TA_Base_App{

	const int ISCSRealTimeAgentWorker::REFRESH_RATE_INCREASE_RATE = 8;  
	const int ISCSRealTimeAgentWorker::REFRESH_RATE_INCREASE_MAXTIME =3; 

	ISCSRealTimeAgentWorker::ISCSRealTimeAgentWorker(IEventWorkObserver* observer)
	{

		m_needToTerminate = false;
		m_isPaused = true;
		m_currentTaskID=0;
		m_nextTaskID =0;
		m_eventID = "";
		m_status=AgentStatus::UnKnown;
		m_isStatusSwitched= false;
		m_failedTimes = 0;
		m_wokerObserver = observer;
		m_sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) ;
		m_refreshRateAgent = EventHandlerManager::getRefreshRateAgent();
		m_timeOutTimes = 0;
		m_eventSize = EventHandlerManager::getMaxISCSEvents();
		m_isFilterNeverMet = false;
		m_disableAgentCheck = TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DISABLEAGENTCHENCK);

		m_cacheThread = new ISCSRealTimeAgentCache(observer);
		m_cacheThread->setInterval(m_refreshRateAgent);
		m_cacheThread->start();

		//init EventHelper
		try
		{
			//get location key
			unsigned long locationKey =0;
			std::string strLocationKey = (const std::string&)TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
			std::istringstream strStream(strLocationKey);
			strStream >> locationKey;
			//LOG ( SourceInfo, DebugUtil::DebugInfo, "Get Location Key: %lu", locationKey);?
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get Location Key: %lu", locationKey);
			TA_Base_Core::EventHelper::getInstance().init(locationKey);
		}
		catch ( ... )
		{
			LOG(SourceInfo,TA_Base_Core::DebugUtil::ExceptionCatch,"Exception caught in ISCSRealTimeAgentWorker constructor." ); 
		}
	}

	AgentStatus ISCSRealTimeAgentWorker::getStatus()
	{
		return m_status;
	}
	ISCSRealTimeAgentWorker::~ISCSRealTimeAgentWorker(void)
	{
		TA_Base_Core::EventHelper::releaseInstance();
		m_cacheThread->terminateAndWait();
		delete m_cacheThread;
		m_cacheThread = NULL;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "~ISCSRealTimeAgentWorker finished.");
	}

	void ISCSRealTimeAgentWorker::terminate()
	{
		m_needToTerminate = true;
		m_cacheThread->clearList();
		m_cacheThread->terminate();
	}

	void ISCSRealTimeAgentWorker::pause()
	{
		m_isPaused =true;
		m_cacheThread->clearList();
	}
	bool ISCSRealTimeAgentWorker::isPaused()
	{
		return m_isPaused;
	}
	//this function is called by manager thread
	void ISCSRealTimeAgentWorker::addNewTask(long taskID, FilterCriteria  filter)
	{
		{
			TA_Base_Core::ThreadGuard guard(m_nextTaskLock);
			m_nextTaskID = taskID;
			m_nextFilter = filter;
		}

		m_isPaused = false;

		TA_Base_Core::ThreadGuard guard2(m_isStatusSwitchedLock); 
		m_isStatusSwitched= false;
		
	}

	// this is called by manager thread(RealTimeEventWorker)
	//return m_isStatusSwitched and reset m_isStatusSwitched to false
	bool ISCSRealTimeAgentWorker::isStatusSwictched()  
	{
		TA_Base_Core::ThreadGuard guard(m_isStatusSwitchedLock); 
		bool isStatusSwitched= m_isStatusSwitched;
		m_isStatusSwitched = false;

		return isStatusSwitched;
	}

	void ISCSRealTimeAgentWorker::checkStatusFromAgent()
	{
		//check status from Agent, 
		bool status= false;
		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Before calling eventHelper checkStatus()");
			status = TA_Base_Core::EventHelper::getInstance().checkStatus();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End calling eventHelper checkStatus() - successed.");
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught when call eventHelper checkStatus().");
		}
		if(status)//if fail, do nothing
		{
			m_status=AgentStatus::Healthy;

			TA_Base_Core::ThreadGuard guard(m_isStatusSwitchedLock); 
			m_isStatusSwitched=true;
			
			m_isRefreshMode=true;
		}
	}


	void ISCSRealTimeAgentWorker::prepare4NewTask()
	{
		m_eventID = "";
		{
			TA_Base_Core::ThreadGuard guard(m_nextTaskLock);
			m_currentTaskID = m_nextTaskID;
			m_currentFilter = m_nextFilter;
		}

		m_cacheThread->setNewTask(m_currentTaskID, m_currentFilter);

		//If Ats system or AtsSubsystem is chosen, stop ISCS Real-time Agent loading.
		m_isFilterNeverMet = false;

		if( ISCSWorkerHelper::iscsWillNeverMeetFilter(m_currentFilter))
		{
			m_isFilterNeverMet = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Real-time Agent loading stops, get 0 events. ISCS events will never meet this filter.",m_currentTaskID);
			
			//Update Status Bar 
			m_wokerObserver->statusReadyNotification(TA_Base_Core::ISCSCategory, m_currentTaskID);
		}
		

	}


	//This function is for checking in the middle of a long function to return quickly.
	bool ISCSRealTimeAgentWorker::shouldStopThisTask()
	{
		TA_Base_Core::ThreadGuard guard(m_nextTaskLock);
		if(m_needToTerminate || m_isPaused || m_nextTaskID>m_currentTaskID)
		{
			return true;
		}
		else
		{
			return false;
		}
		
	}

	bool ISCSRealTimeAgentWorker::newTaskHasArrived()
	{
		TA_Base_Core::ThreadGuard guard(m_nextTaskLock);
		if(m_nextTaskID>m_currentTaskID)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void ISCSRealTimeAgentWorker::run()
	{
		while(!m_needToTerminate)
		{
			if(!m_isPaused)  //has task to do
			{
				//has new task came
				if(newTaskHasArrived()) 
				{
					prepare4NewTask();
					//no need to sleep here
				}
				else if(m_isRefreshMode )
				{
					if(!m_isFilterNeverMet)
					{
						//use m_eventId , if m_eventId=="", call getAllEvents and update m_status(Healthy or Unhealthy)
						//if m_eventID !="", call getLatestEvents, if failed,updata m_status&m_isStatusSwitched
						if(""!=m_eventID)
						{
							loadRefreshList();
						}else
						{
							loadAllFromAgent();
						}
					}
					sleep(m_refreshRateAgent);
				}
				else 
				{
					if(!m_disableAgentCheck)
					{
						checkStatusFromAgent();
					}

					sleep(EventViewerConstants::AGENT_STATUS_CHECK_INTERVAL);
					
				}
			}
			else //has no realTime Task now, should wait for new task
			{
				//when task is paused(user start a historical task), set the status to UnKnown.
				//We let the worker thread write m_status by itself instead of by manager thread. 
				if(m_status!= AgentStatus::UnKnown) 
				{
					m_status = AgentStatus::UnKnown;
					m_isRefreshMode = true;  //reset to Refresh mode so next time when new task comes try refresh first.
				}
				Thread::sleep( EventViewerConstants::WORKER_REFRESH_TASK_PERIOD); 
			}
		}
	}


	void ISCSRealTimeAgentWorker::loadRefreshList()
	{
		FUNCTION_ENTRY("loadRefreshList");
		std::vector<TA_Base_Core::ICombinedEventData*> events;

		TA_Base_Core::T_EventItemList eventItemList;

		try{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Before calling event agent getLatestEvents(),eventID=%s, sessionID=%s ", m_eventID.c_str(), m_sessionID.c_str());
			TA_Base_Core::EventHelper::getInstance().getLatestEvents(m_eventID, m_sessionID, eventItemList);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End calling event agent getLatestEvents() - successed, return %d events", eventItemList.size());

			//if last time is corba timeout, reset it to normal now.
			if(m_timeOutTimes>0)
			{
				m_timeOutTimes =0;
				m_refreshRateAgent = EventHandlerManager::getRefreshRateAgent();
			}

			if(eventItemList.size()>0)
			{
				int size = eventItemList.size();
				m_eventID = eventItemList.back()->eventID; //after get the event, update eventID.
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Task(%d)ISCS Agent, Get %d events from agent. Record the latest EventId for next time refresh: %s",
					m_currentTaskID,size, m_eventID.c_str());

				m_cacheThread->pushEventsToCache(eventItemList);
			}

			return;

		}
		catch(TA_Base_Core::EventHelperException ex)
		{
			TA_Base_Core::EventHelperException::ERROR_CODE eCode = ex.getErrorCode();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught EventHelperException, Error code is %d", eCode);

			if(eCode == TA_Base_Core::EventHelperException::ERR_ID_NOT_FOUND)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Event with id: (%s) can't been found in Agent. Begin to reload all Events from Event Agent.", m_eventID.c_str());
				if(shouldStopThisTask())
				{
					return;	
				}else 
				{
					//loadAllFromAgent();    //when eventID not found in agent, we reload from DB.(start a new realtime task)
					m_isPaused = true;
					m_wokerObserver->reload(); //m_workerObserver is a EventHandler, this will start a new task
					return;
				}
			}
			//if catch timeout exception: don't start a new task. Just Update m_refreshRateAgent to m_refreshRateAgent*8, 
			//until 3 times *8,  when recover, reset it to 200ms.
			if(eCode == TA_Base_Core::EventHelperException::ERR_TIMEOUT)
			{
				if(m_timeOutTimes < REFRESH_RATE_INCREASE_MAXTIME)
				{
					m_refreshRateAgent = m_refreshRateAgent* REFRESH_RATE_INCREASE_RATE;
					m_timeOutTimes++;
				}
				return;
			}
		}
		catch(...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught:Caught General Exception" );
		}
		m_wokerObserver->EventNotification(TA_Base_Core::ISCSCategory, events, TA_Base_App::Reconnect, m_currentTaskID);		
		updataFailedStatus(false);
	}

	void ISCSRealTimeAgentWorker::updataFailedStatus(bool byLoadAll)
	{
		m_failedTimes ++;
		// if 3 continuous times failed, we consider agent is down and need status-checking
		if(m_failedTimes >=3)
		{
			m_failedTimes = 0;
			ThreadGuard guard(m_isStatusSwitchedLock);
			m_status = AgentStatus::UnHealthy;
			if(!byLoadAll)
			{
				m_isStatusSwitched = true;
			}
			m_isRefreshMode = false;
		}
	}


	
	void ISCSRealTimeAgentWorker::loadAllFromAgent( )
	{
		FUNCTION_ENTRY("loadAllFromAgent");
		std::vector<TA_Base_Core::ICombinedEventData*> events;
		try
		{
			TA_Base_Core::T_EventItemList  eventItemList;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Before calling eventHelper getTopEvents()");
			TA_Base_Core::EventHelper::getInstance().getTopEvents(m_eventSize, m_sessionID,eventItemList );
			//TA_Base_Core::EventHelper::getInstance().getAllEvents(m_sessionID,eventItemList );
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End calling eventHelper getTopEvents() - successed.");
			
			//if last time is corba timeout, reset it to normal now.
			if(m_timeOutTimes>0)
			{
				m_timeOutTimes =0;
				m_refreshRateAgent = EventHandlerManager::getRefreshRateAgent();
			}

			m_status = AgentStatus::Healthy;
			//if no exception throw, means the eventID is found in the agent. Then we don't have to merge the events
			if(eventItemList.size()>0)
			{
				//after get the event, update eventID.
				m_eventID = eventItemList.back()->eventID; //get last event's eventID as the latest one
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Task(%d)ISCS Agent,Get %d events from agent. Record the latest EventId for next time refresh: %s", 
					m_currentTaskID, eventItemList.size(),m_eventID.c_str());

				m_cacheThread->pushEventsToCache(eventItemList);

			}
			else
			{
				m_wokerObserver->statusReadyNotification(TA_Base_Core::ISCSCategory, m_currentTaskID);
			}
			
			return;
			
		}
		catch(TA_Base_Core::EventHelperException ex)
		{
			//m_status = AgentStatus::UnHealthy;  //update status to unHealthy only after 3 times failed. (means DB worker will start refresh after agent failed 3 times "getTopEvents"
			TA_Base_Core::EventHelperException::ERROR_CODE eCode = ex.getErrorCode();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "error code is %d", eCode);

			//if catch timeout exception: don't start a new task. Just Update m_refreshRateAgent to m_refreshRateAgent*8, 
			//until 3 times *8,  when recover, reset it to 200ms.
			if(eCode == TA_Base_Core::EventHelperException::ERR_TIMEOUT)
			{
				m_status = AgentStatus::Healthy;
				if(m_timeOutTimes<REFRESH_RATE_INCREASE_MAXTIME)
				{
					m_refreshRateAgent = m_refreshRateAgent * REFRESH_RATE_INCREASE_RATE;
					m_timeOutTimes++;
				}
				
				return;
			}
		}
		catch(...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught:Caught General Exception" );
		}
		m_wokerObserver->EventNotification(TA_Base_Core::ISCSCategory, events, TA_Base_App::Reconnect, m_currentTaskID);
		updataFailedStatus(true);
	}


}