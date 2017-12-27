/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/ThreadWorker.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__THREAD_WORKER_H_
#define SNMP__THREAD_WORKER_H_

#include "IWorkerPool.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Bus
{
	/** implement the IThreadWorker interface and Thread's run function.
     *  
     */
	class ThreadWorker : public TA_Base_Core::Thread, public IThreadWorker
	{
	public:

		/** constructor
         *
         * @param workPool: my WorkerPool.
         */
		ThreadWorker(IWorkerPool* workPool);
		virtual ~ThreadWorker();

		/** Thread's main function.
         *
         * @param 
		 * @return
         */
		virtual void run();

		/** when thread terminate call back, terminate is called by terminateAndWait.
		 * @return void:
         */
		virtual void terminate();

		bool assignTask(IWorkerTask* task);

	private:

        ThreadWorker();
		void executeTask();

		volatile bool	m_threadRunning;
		IWorkerTask*	m_task;
		IWorkerPool*		m_workPool;

		TA_Base_Core::Semaphore		m_semaphore;
		TA_Base_Core::ReEntrantThreadLockable m_lock;
	};
};

#endif
