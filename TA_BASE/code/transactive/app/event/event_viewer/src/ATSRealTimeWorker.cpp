/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ATSRealTimeWorker.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ATSRealTimeWorker is ...
  *
  */

#include "ATSComWrapper.h"
#include "ATSRealTimeWorker.h"
// #include "ATSEventWrapper.h"
// #include "ATSSimulator.h"
#include "ATSRealTimeObserverManager.h"
#include "EventHandlerManager.h"

#include <vector>
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/utilities/src/DebugUtil.h"
#include "ConstantItems.h"
//#define  ATS_BACK_LOG_LENGTH 500

namespace TA_Base_App{

	unsigned long ATSRealTimeWorker::currentMaxPkey = 0;

	ATSRealTimeWorker::ATSRealTimeWorker(IEventWorkObserver* observer):IEventWorker(observer)
	{
		m_ARunningSubscriptionExist = false;	
	}


	ATSRealTimeWorker::~ATSRealTimeWorker()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "~ATSRealTimeWorker() finished.");

	}

	void ATSRealTimeWorker::stopLastSubscriptionIfNeeded()
	{
		ActionCommand actionCommand;
		//runTask:
		// if there is a running subscription, then need to stop subsription first
		if( m_ARunningSubscriptionExist){
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"Task(%d)ATS Real-time : stopping last ATS Subscription  needed and start.",m_curTaskSeqID);

			actionCommand = ATSComWrapper::getInstanceRealTime()->stopATSSubscription(); //if stop unsuccessfully, will not try again.
			//actionCommand = ATSSimulator::stopATSSubscription(); //if stop unsuccessfully, will not try again.

			setCurrentMaxKey(0);

			if(actionCommand == NoCommand)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Task(%d)ATS Real-time : ATS Subscription successfully stopped.",m_curTaskSeqID);

				m_ARunningSubscriptionExist = false;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Task(%d)ATS Real-time : ATS Subscription unSuccessfully stopped.",m_curTaskSeqID);
				
			}
		}
	}

	void ATSRealTimeWorker::setCurrentMaxKey(unsigned long key)
	{
		currentMaxPkey = key;
	}

	unsigned long ATSRealTimeWorker::getCurrentMaxKey()
	{
		return currentMaxPkey;
	}

	void  ATSRealTimeWorker::startNewSubscription()
	{
		std::vector<TA_Base_Core::ICombinedEventData*> events;
		ActionCommand actionCommand;

		setCurrentMaxKey(0);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ATS Real-time backlog loading starts.",m_curTaskSeqID);
		
		ATSRealTimeObserverManager::getInstance()->setTaskInfo( m_curTaskSeqID, m_filter);
		
		// for some situation that will load nothing:
		// e.g. User selected ISCS system but no ATS system, or user selected ISCS subsystem but no ATS subystem
		if ( m_filter.SubsystemNameEqualOr.size() != 0 &&
			m_filter.SubsystemNameEqualOr.size() == m_filter.appSubsystemIds.size() +  m_filter.physSubsystemIds.size() )
		{
			// here means loading step finished!
			setStatus(FinishSearch);
			m_workerObserver->EventNotification(TA_Base_Core::ATSCategory, events, TA_Base_App::NoCommand,m_curTaskSeqID,true);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ATS Real-time backlog loading, get 0 events.",m_curTaskSeqID);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ATS Real-time backlog loading stops.",m_curTaskSeqID);
			return;
		}

		actionCommand =ATSComWrapper::getInstanceRealTime()->startATSSubscription(
			m_filter,
			EventHandlerManager::getMaxATSEvents(),				
			events);


    	setCurrentMaxKey(events.size());

		// subscribe successfully
		if(actionCommand == TA_Base_App::NoCommand)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ATS Real-time backlog loading ends successfully.",m_curTaskSeqID);
			m_ARunningSubscriptionExist = true; 
			setStatus( FinishSearch);
			if( !m_needToTerminate && !m_workerObserver->IsThisTaskStopped(m_curTaskSeqID) ) 
			{	
				m_workerObserver->EventNotification(TA_Base_Core::ATSCategory, events, TA_Base_App::NoCommand,m_curTaskSeqID ,true);
			}

		}
		//subscribe failed: then subscribe again, but no need stopsubsciption because it's already passed.
		else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ATS Real-time backlog loading ends unsuccessfully.",m_curTaskSeqID);
			m_workerObserver->EventNotification(TA_Base_Core::ATSCategory, events, actionCommand,m_curTaskSeqID );
			return;
		}

	}

	void ATSRealTimeWorker::run()
	{
		ATSComWrapper::getInstanceRealTime();

		while( !m_needToTerminate ){
			try
			{
				int workStatus = getNextTaskToCurrent ();
				if ( workStatus == 1 ) //If there is a new task to do
				{
					setStatus(InSearching);

					doCurrentTask();
				}else if(workStatus == -1 &&  m_ARunningSubscriptionExist)// If there is no new task for this worker, but the current task should be stopped. 
					//(e.g. There is a new task for Historical worker, then this real-time worker should stop current real-time task)
				{
					
					stopLastSubscriptionIfNeeded();

				}
				else if( m_workerObserver->getATSSubscriptionErr() < 0) // get ATSSubscriptionError, should restart the task for both ATS and ISCS
				{
					m_workerObserver->setATSSubscriptionErr(0); // set it back to normal
					stopLastSubscriptionIfNeeded();
					m_workerObserver->ATSSubscriptionErrNotification();
				}
				else  //should continue the current task
				{
					for (long i=0;i< (long)(EventHandlerManager::getRefreshRateDB()*5/1000) && !m_needToTerminate ;i++)
					{
						Thread::sleep( EventViewerConstants::WORKER_REFRESH_TASK_PERIOD); 
					}
					
				}
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught in IEventWorker::run()");
			}
		}


		stopLastSubscriptionIfNeeded();

		ATSComWrapper::freeInstanceRealTime();
	}

	void ATSRealTimeWorker::doCurrentTask()
	{

		stopLastSubscriptionIfNeeded();

		startNewSubscription();

	}


	/*void ATSRealTimeWorker::terminate()
	{
			m_needToTerminate = true;
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ATSRealTimeWorker terminated.");
	}*/
    

};
