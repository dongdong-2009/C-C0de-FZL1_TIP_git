/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/IEventWorker.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * IEventWorker is ...
  *
  */

#if !defined(EA_71A81AFE_5618_4f47_B5D0_98F57D826EE6__INCLUDED_)
#define EA_71A81AFE_5618_4f47_B5D0_98F57D826EE6__INCLUDED_

#include "SearchStatus.h"
#include "FilterCriteria.h"
#include "core/threads/src/Thread.h"
#include "IEventWorkObserver.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

using namespace TA_Base_App;

namespace TA_Base_App{


	class IEventWorker : public TA_Base_Core::Thread
	{

	public:
		IEventWorker(IEventWorkObserver * observer);
		virtual ~IEventWorker();

		virtual void run();

		//do the task setting in m_curTaskSeqID and m_filter, will be called after current task info has been setted.
		// also include updating the status to FinishSearch or EndButNotFinished
		virtual void doCurrentTask()=0;
	
		SearchStatus getStatus();
	
		void addWork(FilterCriteria filter, long seq_ID) ;

		// if there is a waiting task ( check m_nextTaskSeqID, if it is bigger than m_curTaskSeqID), 
		// return 1 and update m_curTaskSeqID with m_nextTaskSeqID and update m_filter with m_nextFilter; 
        // else if there is no new task to do for this worker but neet to stop current task 
		// (e.g. a new task with different mode(real-time/Hist)has arrived to ANOTHER worker, then need to stop the current worker's task),return -1
		// else if there is no new taks to do for this workder, and can continue with the current task, then return 0
		int getNextTaskToCurrent( );


		/* This will try to terminate the thread */
		virtual void terminate();

		bool IsThisTaskStopped( long taskID);

		long m_nextTaskSeqID;
		FilterCriteria m_nextFilter; 

		
	protected:
		void setStatus (SearchStatus status);
		long m_curTaskSeqID;
		SearchStatus m_status;  // doesn't need lock for this variable 
		FilterCriteria m_filter; // doesn't need lock for this variable 
		IEventWorkObserver * m_workerObserver;
		bool m_needToTerminate;
		


		TA_Base_Core::NonReEntrantThreadLockable m_CurrentStatus_lock;
		TA_Base_Core::NonReEntrantThreadLockable m_nextFilter_lock;
		TA_Base_Core::NonReEntrantThreadLockable m_nextID_lock;


		
	};
};

#endif // !defined(EA_71A81AFE_5618_4f47_B5D0_98F57D826EE6__INCLUDED_)
