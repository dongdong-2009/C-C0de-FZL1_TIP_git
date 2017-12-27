/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/ThreadedWorker.cpp $
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


#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadedWorker.h"

#include "core/threads/src/IThreadPoolWorkerCallback.h"

namespace TA_Base_Core
{

	ThreadedWorker::ThreadedWorker(IThreadPoolWorkerCallback& callback)
	:
	m_threadRunning ( true ),
	m_threadWorkerBusy ( false ),
	m_workItem ( ),
    m_callback( callback ),
	m_semaphore(0)	//zn++
	{

	}


	ThreadedWorker::~ThreadedWorker()
	{
		//
		// stop the background task if the flag indicates that the thread has been
		// started
		//
		
		if ( THREAD_STATE_RUNNING == getCurrentState() )
		{
			terminateAndWait();
		}

		// reset pointer to IWorkItem
		m_workItem.reset();
	}


	void ThreadedWorker::terminate()
	{
		// set the flag to indicate the thread is not supposed to be running anymore
		m_threadRunning = false;

		// notify the running thread to break the thread loop
		m_semaphore.post();
	}


	void ThreadedWorker::run()
	{
        // tell the callback it can give some work
        m_callback.workerFree();

        // wait here until m_semaphore.post() is called
        m_semaphore.wait();

        while (true == m_threadRunning)
        {
            // process the queued work item now
            processWorkItem();

            // wait here until m_semaphore.post() is called
            m_semaphore.wait();
        }
	}


	void ThreadedWorker::executeWorkItem ( IWorkItemPtr workItem )
	{
		// only proceed if the background task is up and running
		if ( true == m_threadRunning )
		{
			// only proceed if specified pointer is valid
			if ( 0 != workItem.get() )
			{
				// set the flag to indicate we are busy processing the scheduled work
				setThreadWorkerBusy ( true );

				// save the work item
				m_workItem = workItem;

				// notify the running thread to process the new work item
				m_semaphore.post();
			}
			// else do nothing
		}
		// else do nothing
	}


	void ThreadedWorker::processWorkItem()
	{
		// only proceed if pointer is valid
		if ( 0 != m_workItem.get() )
		{
			try
			{
				// do the work
				m_workItem->executeWorkItem();
			}
			catch ( ... )
			{
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught an unknown exception");
                // Force a small sleep time
                sleep(20);
			}

            m_workItem.reset();
		}
		// else do nothing

		// set the flag to indicate we have finished processing the scheduled work
		setThreadWorkerBusy ( false );
        // tell the callback it can give more work
        m_callback.workerFree();
	}


	bool ThreadedWorker::getIsThreadWorkerBusy()
	{
		// protect this variable from multiple threads
		TA_THREADGUARD( m_lock );

		return (m_threadWorkerBusy || (THREAD_STATE_RUNNING != getCurrentState()) );
	}


	void ThreadedWorker::setThreadWorkerBusy ( bool indication )
	{
		// protect this variable from multiple threads
		TA_THREADGUARD( m_lock );

		m_threadWorkerBusy = indication;
	}

}
