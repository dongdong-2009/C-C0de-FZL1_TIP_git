/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/IEventWorker.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * IEventWorker is ...
  *
  */


#include "IEventWorker.h"
#include "EventHandlerManager.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "ConstantItems.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_App{

	IEventWorker::IEventWorker(IEventWorkObserver * observer){
		m_nextTaskSeqID = 0;
		m_curTaskSeqID = 0;

		// For ATSRealTimeWorker, "FinishSearch" means the backlog records which is requested lately is finished.
		// "InSearching" means, from a new subscription started ,till the backlog records (must be the last request) returned
		m_status = FinishSearch;
		m_workerObserver = observer;
		m_needToTerminate = false;

	}

	IEventWorker::~IEventWorker(){

	}

	void IEventWorker::run()
	{
		while( !m_needToTerminate ){
			try
			{

				int workStatus = getNextTaskToCurrent ( );

				if ( workStatus ==1 )  // means this worker has got a new task to do
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

	}


	SearchStatus IEventWorker::getStatus(){
		TA_Base_Core::ThreadGuard guard2(m_CurrentStatus_lock);
		return  m_status;
	}

	void IEventWorker::setStatus (SearchStatus status)
	{
		TA_Base_Core::ThreadGuard guard(m_CurrentStatus_lock);
		m_status = status;
	}

	void IEventWorker::addWork(FilterCriteria filter, long seq_ID)
	{
		TA_Base_Core::ThreadGuard guard1(m_nextFilter_lock);
		TA_Base_Core::ThreadGuard guard2(m_nextID_lock);
		m_nextFilter = filter;
        m_nextTaskSeqID = seq_ID;
		
	}

	int IEventWorker::getNextTaskToCurrent( )
	{

			TA_Base_Core::ThreadGuard guard2(m_nextFilter_lock);
			TA_Base_Core::ThreadGuard guard3(m_nextID_lock);


			if( m_nextTaskSeqID>m_curTaskSeqID && !IsThisTaskStopped(m_nextTaskSeqID)) 
			{
				m_curTaskSeqID = m_nextTaskSeqID;
				m_filter = m_nextFilter;  // should be value copy!
				return 1;
			}else if ( m_nextTaskSeqID>0 && m_nextTaskSeqID == m_curTaskSeqID  && IsThisTaskStopped(m_curTaskSeqID) )
			{
				return -1;
			}else
			{
				return 0;
			}
	}



	bool IEventWorker::IsThisTaskStopped( long taskID){

		return m_workerObserver->IsThisTaskStopped(taskID);
	}

	void IEventWorker::terminate()
	{
		m_needToTerminate = true;
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Worker Thread terminated");
	}
};