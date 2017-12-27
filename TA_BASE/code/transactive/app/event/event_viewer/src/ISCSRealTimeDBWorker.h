/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSRealTimeDBWorker.h $
  * @author 
  * @version $Revision: 
  *
  * Last modification: $DateTime:
  * Last modified by: 
  * 
  * ISCSRealTimeDBWorker is a thread for for loading events from DB 
  *
  */
#pragma once
#include "app/event/event_viewer/src/IEventWorkObserver.h"
#include "app/event/event_viewer/src/FilterCriteria.h"
#include "app/event/event_viewer/src/ActionCommand.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/src/CombinedEventAccessFactory.h"

namespace TA_Base_App{
	class ISCSRealTimeDBWorker : public TA_Base_Core::Thread
	{
	public:
		ISCSRealTimeDBWorker(IEventWorkObserver* observer);
		~ISCSRealTimeDBWorker(void);

		void run();
		void terminate();
		void addNewTask(long taskID, FilterCriteria filter);
		int loadNSubmitNextEvents();

		/*Parameter <firstRefresh> added to identify the first time load of events in a new task*/
		void ISCSRealTimeDBWorker::submitEvents(std::vector<TA_Base_Core::ICombinedEventData*> &events,bool firstRefresh, ActionCommand command = TA_Base_App::NoCommand);
		bool ISCSRealTimeDBWorker::shouldStopThisTask();
		void ISCSRealTimeDBWorker::updateEventID();
		void ISCSRealTimeDBWorker::loadRefreshList();
		bool ISCSRealTimeDBWorker::isAutoRefresh();
		void ISCSRealTimeDBWorker::setAutoRefresh();
		void ISCSRealTimeDBWorker::pause();
		bool ISCSRealTimeDBWorker::newTaskHasArrived();
		bool ISCSRealTimeDBWorker::isPaused();
		void ISCSRealTimeDBWorker::getNewTask();
		void ISCSRealTimeDBWorker::loadBacklog();
		void ISCSRealTimeDBWorker::notifySatusReady();
	private:
		bool m_needTerminate; 
		bool m_isPaused;    //read by itself, write by RealTimeEventWorker
		bool m_needRefresh; //read by itself, write by RealTimeEventWorker

		long m_currentTaksID;  //used only by itself		
		FilterCriteria m_currentFilter;//used only by itself		

		long m_nextTaskID; //itself: read, Manager: write
		FilterCriteria m_nextFilter; //itself: read, Manager: write
		TA_Base_Core::NonReEntrantThreadLockable m_nextTaskLock;

		std::string m_eventID;
		IEventWorkObserver* m_workerObserver;
		bool m_checkRights;
		unsigned long m_refreshRateDB;
		unsigned long m_refreshRateAgent;
		
		bool m_isFilterNeverMet;
	};
};