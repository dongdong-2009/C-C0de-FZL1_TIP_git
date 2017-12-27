/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ISCSRealTimeAgentWorker.h $
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
#pragma once
#include "app/event/event_viewer/src/FilterCriteria.h"
#include "app/event/event_viewer/src/IEventWorkObserver.h"
#include "app/event/event_viewer/src/ISCSRealTimeAgentCache.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "core/event/src/EventItem.h"
#include "ISCSWorkerHelper.h"


using namespace TA_Base_Core;
using namespace std;
namespace TA_Base_App{

	enum AgentStatus
	{
		UnKnown,
		UnHealthy,
		Healthy
	};

	class ISCSRealTimeAgentWorker : public TA_Base_Core::Thread
	{
	public:
		ISCSRealTimeAgentWorker(TA_Base_App::IEventWorkObserver * observer);
		~ISCSRealTimeAgentWorker(void);

		void run();

		void terminate();

		bool ISCSRealTimeAgentWorker::checkAgentStatus();
		AgentStatus ISCSRealTimeAgentWorker::getStatus();
		void addNewTask(long taskID, FilterCriteria filter);
		bool isStatusSwictched();
		bool ISCSRealTimeAgentWorker::isPaused();
		void ISCSRealTimeAgentWorker::pause();
	private:

		void checkStatusFromAgent();

		void ISCSRealTimeAgentWorker::loadRefreshList();
		void ISCSRealTimeAgentWorker::updataFailedStatus(bool byLoadAll);
		void ISCSRealTimeAgentWorker::prepare4NewTask();
		
		
		bool ISCSRealTimeAgentWorker::shouldStopThisTask();
		
		bool ISCSRealTimeAgentWorker::newTaskHasArrived();
		void ISCSRealTimeAgentWorker::loadAllFromAgent();
		bool m_needToTerminate;
		bool m_isPaused;  //itself: read, Manager: write
		AgentStatus m_status; //itself: read/write ; Manager thread: read
		std::string m_sessionID;
		IEventWorkObserver * m_wokerObserver;

		long m_nextTaskID; //itself: read, Manager: write
		FilterCriteria m_nextFilter;  //itself: read, Manager: write
		TA_Base_Core::NonReEntrantThreadLockable m_nextTaskLock;  //this lock is for (m_nextTaskID and m_nextFilter together)

		//set to true by itself when(getLatest fail or checkStatus succeed)), 
		//set to false by manager(ISCSRealTimeWorker) after read
		bool m_isStatusSwitched ; 
		TA_Base_Core::NonReEntrantThreadLockable m_isStatusSwitchedLock;

		// true means it's in refresh mode, false means it's in status check mode
		//used only by itself
		bool m_isRefreshMode;

		long m_currentTaskID; //used only by itself		
		FilterCriteria m_currentFilter; //used only by itself
		string m_eventID; //used only by itself
		int m_failedTimes;


		unsigned long m_refreshRateAgent;//Default value: 200(ms)
		int m_timeOutTimes; //0~ 3, every time if agent timeout, don't start a new task. Just Update m_refreshRateAgent to m_refreshRateAgent*8, until 3 times ,  when recover, reset it to 200ms.
		static const int REFRESH_RATE_INCREASE_RATE ;  // 8
		static const int REFRESH_RATE_INCREASE_MAXTIME; // 3
		
		unsigned long m_eventSize; //20000

		ISCSRealTimeAgentCache * m_cacheThread;

		bool m_isFilterNeverMet;
		bool m_disableAgentCheck;
	};
	
};