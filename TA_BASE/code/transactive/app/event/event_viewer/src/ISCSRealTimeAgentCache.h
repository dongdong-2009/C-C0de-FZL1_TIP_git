/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSRealTimeAgentCache.h $
  * @author 
  * @version $Revision: 
  *
  * Last modification: $DateTime:
  * Last modified by: 
  * 
  * ISCSRealTimeAgentCache is a thread for for holding data and process data before notifying GUI to show.
  *
  */
#pragma once

#include "app/event/event_viewer/src/IEventWorkObserver.h"
#include "app/event/event_viewer/src/FilterCriteria.h"

#include "core/event/src/EventItem.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/data_access_interface/src/T6AccessFactory.h"

namespace TA_Base_App{

class ISCSRealTimeAgentCache: public TA_Base_Core::Thread
{
	public:
		ISCSRealTimeAgentCache(IEventWorkObserver* observer);
		~ISCSRealTimeAgentCache(void);
		void run();

		void terminate();
		
		void pushEventsToCache(TA_Base_Core::T_EventItemList & events);
		void ISCSRealTimeAgentCache::clearList();
		void ISCSRealTimeAgentCache::setNewTask(long taskID, FilterCriteria filter);
		void ISCSRealTimeAgentCache::setInterval(unsigned long interval);
	private:
		void ISCSRealTimeAgentCache::ProcessEvents();
		
		
		std::vector<TA_Base_Core::ICombinedEventData*> ISCSRealTimeAgentCache::filterEvents();
		void ISCSRealTimeAgentCache::prepareRightsCache();
		bool ISCSRealTimeAgentCache::checkAnEventByRights(TA_Base_Core::T6AccessFactory::SubSystemKeys subKeys, unsigned long actionID);
		bool ISCSRealTimeAgentCache::checkAnEventByAFilterItem(FilterItermToCheck filterItem, TA_Base_Core::EventItem * event);
		bool ISCSRealTimeAgentCache::checkAnEventByFilter(TA_Base_Core::EventItem * event);
		TA_Base_Core::ICombinedEventData* ISCSRealTimeAgentCache::createEventDataFromDetail(TA_Base_Core::EventItem* detail);
		void ISCSRealTimeAgentCache::clearCurrentRightsCache();
		void ISCSRealTimeAgentCache::getRightsCacheFromFactory(unsigned long profileId);
		bool m_needToTerminate;
		TA_Base_Core::T_EventItemList m_events; // the cache, add/delete(when more then 20000) by ISCSRealTimeAgentWorker, delete by itself.
		IEventWorkObserver * m_wokerObserver;
		unsigned long m_cachProcessInterval;

		TA_Base_Core::ReEntrantThreadLockable m_cacheLock;
		long m_currentTaskID; 
		bool m_statusReadySent;
		FilterCriteria m_currentFilter;
		
		bool m_isRightsCheckNeeded;
		bool m_rightCachePrepared;
		unsigned long m_ProfileId4CurrentRightsCache;
		TA_Base_Core::T6AccessFactory::Rights m_rights ;

		
};
}