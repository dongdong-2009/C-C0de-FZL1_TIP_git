/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSHistoricalWorker.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ISCSHistoricalWorker is ...
  *
  */

#include "ISCSHistoricalWorker.h"
#include "ISCSWorkerHelper.h"
#include "EventHandlerManager.h"
#include "ConstantItems.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_App{


	ISCSHistoricalWorker::ISCSHistoricalWorker(IEventWorkObserver* observer):IEventWorker(observer)
	{
	}



	ISCSHistoricalWorker::~ISCSHistoricalWorker()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "~ISCSRealTimeWorker() finished.");
	}


	// before this function called. m_status was just set as InSearching
	void ISCSHistoricalWorker::doCurrentTask()
	{
		// start loading step :
		load(m_filter, m_curTaskSeqID);  // after this function, m_status will be set as FinishSearch or EndButNotFinishSearch

	}
	

	// This function will set status to FinisheSearch(if all the (backlog)event has been loaded) 
	// or EndButNotFinishSearch( this task is stopped before all the (backlog) events loaded )
	void ISCSHistoricalWorker::load( FilterCriteria filterCriteria, long taskID )
	{
		std::vector<TA_Base_Core::ICombinedEventData*> events;

		// for some situation that will load nothing:
		// e.g. User selected ATS system but no ISCS system, or user selected ATS subsystem but no ISCS subystem
		if ( (m_filter.SystemNameEqualOr.size()!=0 || m_filter.SubsystemNameEqualOr.size() != 0) &&
			m_filter.appSubsystemIds.size() ==0 &&  m_filter.physSubsystemIds.size()==0 )
		{
			// here means loading step finished!
			setStatus(FinishSearch);
			m_workerObserver->EventNotification(TA_Base_Core::ISCSCategory, events, TA_Base_App::NoCommand,taskID, true);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Historical loading stops, get 0 events.",taskID);
			return;
		}


		TA_Base_Core::CombinedEventAccessFactory::EventFilter filter;
		ISCSWorkerHelper::populatePublicFilter(filterCriteria, filter);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Historical loading starts.", taskID);
		try{
			bool enableVisibilityRule = EventHandlerManager::getRightsCheck();

			if( filterCriteria.searchDirection == TA_Base_App::Backward )
			{
				if(filterCriteria.pkeyBoundary_ISCS == 0)
				{
					// normal historical query:
					events = TA_Base_Core::CombinedEventAccessFactory::getInstance().
						loadDesignatedEvents(enableVisibilityRule, filter, "", EventHandlerManager::getMaxISCSEvents());

				}else
				{
					// previous page query:
					events = TA_Base_Core::CombinedEventAccessFactory::getInstance().
						loadDesignatedEvents(enableVisibilityRule, filter, "", EventHandlerManager::getMaxISCSEvents(), filterCriteria.pkeyBoundary_ISCS, false);
				}
			}
			else 
			{
				// next page query:
				events = TA_Base_Core::CombinedEventAccessFactory::getInstance().
					loadDesignatedEvents(enableVisibilityRule, filter, "", EventHandlerManager::getMaxISCSEvents(), filterCriteria.pkeyBoundary_ISCS, true);
			}

			if(!m_needToTerminate && !IsThisTaskStopped(taskID)){

				m_workerObserver->EventNotification(TA_Base_Core::ISCSCategory, events, TA_Base_App::NoCommand,taskID);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Historical loading, get first %d events.",taskID,events.size());

				// then, collect all the rest events together and notify observer:
				std::vector<TA_Base_Core::ICombinedEventData*> restEvents;
				while (!events.empty())
				{
					if(!m_needToTerminate && !IsThisTaskStopped(taskID))
					{
						events = TA_Base_Core::CombinedEventAccessFactory::getInstance().getNextEvents();
						restEvents.insert(restEvents.end(), events.begin(), events.end());
					}
					else
					{
						setStatus(EndButNotFinishSearch);
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Historical loading stops.(Loading end but not finished)",taskID);
						return;
					}

				}

				if( !m_needToTerminate && !IsThisTaskStopped(taskID) ){
					// here means loading step finished!
					setStatus(FinishSearch);

					m_workerObserver->EventNotification(TA_Base_Core::ISCSCategory, restEvents, TA_Base_App::NoCommand,taskID, true);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Historical loading stops, get rest %d events.",taskID,restEvents.size());

				}
				else
				{
					setStatus(EndButNotFinishSearch);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Historical loading stops(Loading end but not finished).",taskID);
					return;
				}

			}
			else
			{
				setStatus(EndButNotFinishSearch);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ISCS Historical loading stops(Loading end but not finished).",taskID);
				return;
			}

		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"Database load was not successful");
			m_workerObserver->EventNotification(TA_Base_Core::ISCSCategory, events, TA_Base_App::Reconnect, taskID);
			setStatus(EndButNotFinishSearch);  
		}	
	}

	/*void ISCSHistoricalWorker::terminate()
	{
		m_needToTerminate = true;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ISCSHistoricalWorker terminated");
	}*/
};
