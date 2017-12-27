/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/ThreadedWorker.h $
  * @author:	HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  * ThreadedWorker represents a single threaded opreation and accepts an instance of IWorkItem
  * interface that is passed in by the ThreadPoolSingletonManager.  When the ThreadedWorker detects
  * that it has been assigned an instance of IWorkItem interface, it executes the callback operation
  * defined in the interface and when the execution is finished, it makes itself available to the
  * ThreadPoolSingletonManager to assign the next IWorkItem interface.
  *
  */

#ifndef THREADED_WORKER_H
#define THREADED_WORKER_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

#include "core/threads/src/IWorkItem.h"

namespace TA_Base_Core
{
    class IThreadPoolWorkerCallback;
};

namespace TA_Base_Core
{
	class ThreadedWorker : public Thread
	{
	public:

		ThreadedWorker(IThreadPoolWorkerCallback& callback);
		virtual ~ThreadedWorker();


		//
		// Operations required by Thread
		//


		virtual void run();
		virtual void terminate();


		//
		// Operations specific to ThreadedWorker
		//


		/**
		*
		* executeWorkItem
		*
		* Process the specified work item
		*
		* @param	workItem	The work item to be processed by this thread worker
		*
		*/
		void executeWorkItem ( IWorkItemPtr workItem );


		/**
		*
		* getIsThreadWorkerBusy
		*
		* Returns true to indicate that this thread worker is busy processing the
		* work item currently assigned to it, false otherwise.
		*
		*/
		bool getIsThreadWorkerBusy();


	private:

        ThreadedWorker();

		/**
		*
		* processWorkItem
		*
		* Obtain the work item and start the callback
		*
		*/
		void processWorkItem();


		/**
		*
		* setThreadWorkerBusy
		*
		* Set the internal variable to indicate that this thread worker is busy
		* processing the work item currently assigned to it.
		*
		* @param	indication		True if the thread is busy processing the
		*							work item, false otherwise
		*
		*/
		void setThreadWorkerBusy ( bool indication );

		bool m_threadRunning;
		bool m_threadWorkerBusy;
		ReEntrantThreadLockable m_lock;
		Semaphore m_semaphore;
		IWorkItemPtr m_workItem;
        IThreadPoolWorkerCallback& m_callback;
	};
};

#endif
