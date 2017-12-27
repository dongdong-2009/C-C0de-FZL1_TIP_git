/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ATSHistoricalWorker.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ATSHistoricalWorker is ...
  *
  */

#include "ATSComWrapper.h"
#include "ATSHistoricalWorker.h"
#include "ConstantItems.h"

namespace TA_Base_App{

	ATSHistoricalWorker::ATSHistoricalWorker(IEventWorkObserver* observer):IEventWorker(observer)
	{
	}

	ATSHistoricalWorker::~ATSHistoricalWorker()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "~ATSHistoricalWorker() finished.");
	}

	void ATSHistoricalWorker::run()
	{
		ATSComWrapper::getInstanceHistorical();

		while( !m_needToTerminate ){
			try
			{
				if (getNextTaskToCurrent ( ) >0)
				{
					setStatus(InSearching);

					doCurrentTask();
				}else
				{
					for (long i=0;i<(long)(EventHandlerManager::getRefreshRateDB()*5/1000) && !m_needToTerminate ;i++)
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

		ATSComWrapper::freeInstanceHistorical();
	}

	void ATSHistoricalWorker::doCurrentTask()
	{
		std::vector<TA_Base_Core::ICombinedEventData*> events;
		ActionCommand actionCommand;

		// start taskId ,ATSHistorical, loading
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ATS Historical loading starts.",m_curTaskSeqID);

		// for some situation that will load nothing:
		// e.g. User selected ISCS system/subsystem but no ATS system/subysstem
		if ( m_filter.SubsystemNameEqualOr.size() != 0 &&
			m_filter.SubsystemNameEqualOr.size() == m_filter.appSubsystemIds.size() +  m_filter.physSubsystemIds.size() )
		{
			// here means loading step finished!
			setStatus(FinishSearch);
			m_workerObserver->EventNotification(TA_Base_Core::ATSCategory, events, TA_Base_App::NoCommand,m_curTaskSeqID,true);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ATS Historical loading, get 0 events.",m_curTaskSeqID);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Task(%d)ATS Historical loading stops.",m_curTaskSeqID);
			return;
		}


		if( m_filter.searchDirection == TA_Base_App::Backward /*&& m_filter.pkeyBoundary_ATS!=""*/ )
		{
			actionCommand = ATSComWrapper::getInstanceHistorical()->doPreviousPageQuery(events, m_filter, EventHandlerManager::getMaxATSEvents());
			
		}
		else if( m_filter.searchDirection == TA_Base_App::Forward )
		{
			actionCommand = ATSComWrapper::getInstanceHistorical()->doNextPageQuery(events, m_filter, EventHandlerManager::getMaxATSEvents());
		}
		
		if (!m_needToTerminate && !IsThisTaskStopped(m_curTaskSeqID))
		{
			if(actionCommand == NoCommand) 	// query successful
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Task(%d)ATS Historical loading finished succeessfully, get total %d events.",m_curTaskSeqID, events.size());
				m_workerObserver->EventNotification(TA_Base_Core::ATSCategory, events, TA_Base_App::NoCommand,m_curTaskSeqID,true);
				setStatus(FinishSearch);
			}
			else   // query unsuccessful
			{		
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Task(%d)ATS Historical loading finished unsucceessfully.",m_curTaskSeqID, events.size());

				m_workerObserver->EventNotification(TA_Base_Core::ATSCategory, events, actionCommand,m_curTaskSeqID);
				setStatus(EndButNotFinishSearch);
				
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"Task(%d)ATS Historical loading ended by interruption.",m_curTaskSeqID, events.size());
			setStatus (EndButNotFinishSearch);	
		}

	}

	/*void ATSHistoricalWorker::terminate()
	{
		m_needToTerminate = true;
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ATSHistoricalWorker terminated.");
	}*/

};
