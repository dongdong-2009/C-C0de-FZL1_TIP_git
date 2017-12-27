/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/SCADA_development/TA_BASE/transactive/core/threads/src/LFThreadPoolWorker.cpp $
  * @author:	HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2013/05/09 17:18:15 $
  * Last modified by : $Author: builder $
  *
  *
  
  */


#include "./LFThreadPoolWorker.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/threads/src/IThreadPoolWorkerCallback.h"

namespace TA_Base_Core
{

	LFThreadPoolWorker::LFThreadPoolWorker(ILFThreadPoolManager& callback)
	:
	m_threadRunning ( true ),
    m_callback( callback ),
	m_semaphore(0)	//zn++
	{

	}


	LFThreadPoolWorker::~LFThreadPoolWorker()
	{
		//
		// stop the background task if the flag indicates that the thread has been
		// started
		//
		
		if ( THREAD_STATE_RUNNING == getCurrentState() )
		{
			terminateAndWait();
		}

	}


	void LFThreadPoolWorker::terminate()
	{
		// set the flag to indicate the thread is not supposed to be running anymore
		m_threadRunning = false;

		// notify the running thread to break the thread loop
		m_semaphore.post();
	}


	void LFThreadPoolWorker::run()
	{
        // tell the callback it can give some work
        m_callback.workerFree(this);

        // wait here until m_semaphore.post() is called
        m_semaphore.wait();

        while (true == m_threadRunning)
        {
            // process the queued work item now
            doLeaderWorker();

            // wait here until m_semaphore.post() is called
            m_semaphore.wait();
        }
	}


	void LFThreadPoolWorker::changeToLeader()
	{
		m_semaphore.post();
	}


	void LFThreadPoolWorker::doLeaderWorker()
	{
		IThreadPoolExecuteItem* item = m_callback.leaderExecuteItem();
		if (item)
		{
			try
			{
				item->executeWorkItem();
			}
            catch( const TransactiveException & te )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
            }
            catch( const std::exception & e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception in LFThreadPoolManager::run");
            }
		}
        m_callback.workerFree(this);
	}
}
