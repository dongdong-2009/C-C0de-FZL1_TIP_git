/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSRealTimeDBWorker.cpp $
  * @author 
  * @version $Revision: 
  *
  * Last modification: $DateTime:
  * Last modified by: 
  * 
  * ISCSRealTimeDBWorker is a thread for for loading events from DB 
  *
  */
#include "stdafx.h"

#include "ISCSRealTimeDBWorker.h"
#include "app/event/event_viewer/src/ISCSWorkerHelper.h"
#include "app/event/event_viewer/src/EventHandlerManager.h"
#include "app/event/event_viewer/src/ConstantItems.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/T6AccessFactory.h"



namespace TA_Base_App{
	ISCSRealTimeDBWorker::ISCSRealTimeDBWorker(IEventWorkObserver* observer)
	{
		m_needTerminate = false;
		m_isPaused = true;
		m_currentTaksID=0;
		m_nextTaskID =0;

		m_workerObserver = observer;
		m_eventID = "";
		m_checkRights =  EventHandlerManager::getRightsCheck();
		// set refresh Rate:
		m_refreshRateDB = EventHandlerManager::getRefreshRateDB();
		m_refreshRateAgent = EventHandlerManager::getRefreshRateAgent();

		m_isFilterNeverMet = false;
	}

	ISCSRealTimeDBWorker::~ISCSRealTimeDBWorker(void)
	{
	}

	void ISCSRealTimeDBWorker::pause()
	{
		m_isPaused =true;
	}

	bool ISCSRealTimeDBWorker::isPaused()
	{
		return m_isPaused;
	}
	//save the new task(id+filter) temporary, and do not write the current task(id+filter).
	void ISCSRealTimeDBWorker::addNewTask(long taskID, FilterCriteria filter)
	{
		{
			TA_Base_Core::ThreadGuard guard(m_nextTaskLock);
			m_nextTaskID = taskID;
			m_nextFilter = filter;
		}

		m_isPaused = false;
		m_needRefresh = false;
	}
	void ISCSRealTimeDBWorker::getNewTask()
	{
		TA_Base_Core::ThreadGuard guard(m_nextTaskLock);
		m_currentTaksID = m_nextTaskID;
		m_currentFilter = m_nextFilter;
		m_eventID = "";
	}
	void ISCSRealTimeDBWorker::setAutoRefresh()
	{
		m_needRefresh = true;
	}
	bool ISCSRealTimeDBWorker::isAutoRefresh()
	{
		return m_needRefresh;
	}
	
	void ISCSRealTimeDBWorker::terminate()
	{
		m_needTerminate=true;
	}

	void ISCSRealTimeDBWorker::loadRefreshList()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Task(%d)ISCS Real-time DB worker start loading new refresh events.");
		std::vector<TA_Base_Core::ICombinedEventData*> events;	
		try
		{		
			events = TA_Base_Core::CombinedEventAccessFactory::getInstance().getDesignatedEventsSinceLastLoad(m_checkRights);
			submitEvents(events, false);
			updateEventID(); 
		}	
		catch(...)
		{
			// If there's a problem (any problem), notify the user, then return 0.
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"Database load was not successful");
			submitEvents(events, false, TA_Base_App::Reconnect);
		}		
	}

	bool ISCSRealTimeDBWorker::newTaskHasArrived()
	{
		TA_Base_Core::ThreadGuard guard(m_nextTaskLock);
		if(m_nextTaskID>m_currentTaksID)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	

	void ISCSRealTimeDBWorker::run()
	{

		while(!m_needTerminate)
		{
			if(!m_isPaused)   //has task to do
			{
				if(newTaskHasArrived())
				{
					getNewTask();
					loadBacklog();  //load backlog, update m_eventID, set m_needLoad to false /*todo: test*/
				}
				else if(m_needRefresh && !m_isFilterNeverMet)
				{
					loadRefreshList(); //load refresh events
					
					//m_refreshRateDB=5000ms ,it's a long time so we need to divide it into small spans(200ms) to make it more sensitive to termination.
					for(int i=0; !m_needTerminate && !m_isPaused && i<m_refreshRateDB/200; i++)
					{
						sleep(200); 
					}
				}
				else 
				{
					sleep(m_refreshRateAgent);
				}
			}
			else //task invalid,need to wait for new task
			{
				sleep(EventViewerConstants::WORKER_REFRESH_TASK_PERIOD);
			}
		}
	}

	//check whether need to return in the middle of a long function
	//should return quickly when :
	//1.thread terminated 2.m_nextTaskID>m_currentTaskID (new realtime task comes) 3. paused(new historical task start)
	bool ISCSRealTimeDBWorker::shouldStopThisTask()
	{
		TA_Base_Core::ThreadGuard guard(m_nextTaskLock);
		if(!m_needTerminate && !m_isPaused && m_nextTaskID==m_currentTaksID)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void ISCSRealTimeDBWorker::notifySatusReady()
	{
		m_workerObserver->statusReadyNotification(TA_Base_Core::ISCSCategory,m_currentTaksID ); //notify GUI to show status "Ready"
	}

	void ISCSRealTimeDBWorker::loadBacklog()
	{
		std::vector<TA_Base_Core::ICombinedEventData*> events;
		m_isFilterNeverMet = false;

		if( ISCSWorkerHelper::iscsWillNeverMeetFilter(m_currentFilter))
		{
			m_isFilterNeverMet = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Real-time backlog loading stops, get 0 events. ISCS events will never meet this filter.",m_currentTaksID);

			//submit empty events
			submitEvents(events, true);
			notifySatusReady();
			return ;
		}

		
		TA_Base_Core::CombinedEventAccessFactory::EventFilter filter;
		ISCSWorkerHelper::populatePublicFilter(m_currentFilter, filter);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Real-time backlog loading starts.", m_currentTaksID);
		
		try{
			unsigned long queryEndPkey = TA_Base_Core::CombinedEventAccessFactory::getInstance().getMaxEventPKeyNEventId().maxPkey;
			unsigned long queryStartPkey=0;
			std::string firstPartSqlCache="";
			unsigned long hasLoadedSize = 0;
			unsigned long maxSize = EventHandlerManager::getMaxISCSEvents();
			int firstBatchReturn = 0;

			for(int i=0; i<3; i++)  // divide loading into 3(maximum) phrase , each phrase query in REALTIME_LOAD_BATCH events.
			{
				if(shouldStopThisTask())
				{
					return;
				}

				if(queryEndPkey == 0)
				{
					queryEndPkey =  TA_Base_Core::CombinedEventAccessFactory::getInstance().getPkeySequenceMaxValue();
					if(queryEndPkey==0) break;
				}
				//load first 110 and submit:
				queryStartPkey = (queryEndPkey > EventViewerConstants::REALTIME_LOAD_BATCH) ? (queryEndPkey - EventViewerConstants::REALTIME_LOAD_BATCH):0 ;
				events = TA_Base_Core::CombinedEventAccessFactory::getInstance().
					loadDesignatedEventsInASection(queryStartPkey,queryEndPkey, firstPartSqlCache, m_checkRights, filter, maxSize -hasLoadedSize);
				firstBatchReturn = events.size();
				submitEvents(events, true);
				hasLoadedSize+= events.size();

				if(hasLoadedSize>=maxSize) //has loaded max size of events just by "first" return events(even without "getNext")
				{
					break;
				}else if(firstBatchReturn>0) // hasn't reach max size of events by the "first" return events by sql
				{	
					//load "nextEents" and submit:
					int nextEventsTotal = loadNSubmitNextEvents();
					hasLoadedSize += nextEventsTotal; 
					if(hasLoadedSize>=maxSize) // if max size reached or there's no more events to be loaded
					{
						break;
					}			
				}
				queryEndPkey = (queryEndPkey > EventViewerConstants::REALTIME_LOAD_BATCH)? (queryEndPkey - EventViewerConstants::REALTIME_LOAD_BATCH): 0;  
				
			}
			
			// loading t6 rights cache: loading rights cache should be executed after the first events returned in order to show events quickly.
			// But it should be executed before the last SQL(below), because that sql takes time. 
			// if the last sql running before t6 loading, 
			// t6 loading will be locked until the that query return. 
			// In that case, both DB worker and agent worker(need rights cache) can not be finished loading in time.
			if(m_checkRights)
			{
				TA_Base_Core::T6AccessFactory::getInstance().retrieveRightsByProfileId(filter.profiles[0]);
			}
			
			if(!shouldStopThisTask())
			{
				if(hasLoadedSize<maxSize )
				{
					events = TA_Base_Core::CombinedEventAccessFactory::getInstance().
						loadDesignatedEventsInASection( 0, queryEndPkey, firstPartSqlCache, m_checkRights, filter, maxSize -hasLoadedSize);
					firstBatchReturn = events.size();
					hasLoadedSize+= events.size();
					submitEvents(events, false);
					if(firstBatchReturn>0)
					{
						loadNSubmitNextEvents();
					}
					//loadLastEventsByTime() ; //just for testing, the last sql (where pkey<***) will spent a lot of time. so we try to load by time(30min) (where pkey<*** and createtime>***)
				}
				notifySatusReady();
				updateEventID();
				return; // has a bug: when seq meets the max
			}
			else
			{
				return ;
			}
			
		}
		catch(...)
		{
			// If there's a problem (any problem), notify the user, then return 0.
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"Database load was not successful");
			submitEvents(events, false, TA_Base_App::Reconnect);
		}	
	}

	void ISCSRealTimeDBWorker::updateEventID()
	{
		if(!shouldStopThisTask())
		{
			m_eventID = TA_Base_Core::CombinedEventAccessFactory::getInstance().getLastEventID();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Task(%d)ISCS Real-time DB worker,eventID updated to %s",m_currentTaksID,m_eventID.c_str());
		}
	}
	//merge(by eventID) & submit
	void ISCSRealTimeDBWorker::submitEvents(std::vector<TA_Base_Core::ICombinedEventData*> &events,bool firstRefresh,  ActionCommand command)
	{
		/*EvenNotification called for First time load of event for new task, so that GUI event list is cleared*/
		if(! shouldStopThisTask() && (events.size()>0 || firstRefresh))
		{
			m_workerObserver->mergeEventFromDB(events);
			if(!shouldStopThisTask())
			{
				m_workerObserver->EventNotification(TA_Base_Core::ISCSCategory, events, command,m_currentTaksID);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Task(%d)ISCS Real-time DB worker, submit %d events.",m_currentTaksID,events.size());
			}
		}

	}

	int ISCSRealTimeDBWorker::loadNSubmitNextEvents()
	{	
		std::vector<TA_Base_Core::ICombinedEventData*> events;
		//collect all the rest events together and notify observer:
		std::vector<TA_Base_Core::ICombinedEventData*> restEvents;
		
		do{	
			if(shouldStopThisTask())
			{
				return 0;
			}
			events = TA_Base_Core::CombinedEventAccessFactory::getInstance().getNextEvents();
			restEvents.reserve(restEvents.size() + events.size()); 
			restEvents.insert(restEvents.end(), events.begin(), events.end());	
		}while (!events.empty());

		submitEvents(restEvents, false);
		return restEvents.size();	
	}

};