/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/WorkerPool.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SNMP__WORKER_POOL_H_
#define SNMP__WORKER_POOL_H_

#include <list>
#include <map>
#include <sys/timeb.h>

#include "ThreadWorker.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

#include "core/synchronisation/src/Condition.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
	/** implement the IWorkerPool interface.
     * this class is a thread pool class.
     */
	class WorkerPool : public TA_Base_Core::Thread, public IWorkerPool
	{
	public:
		/** constructor a thread pool.
         *
         * @param numThreads: initial workers.
		 * @param upperLimit: most workers.
         */
		WorkerPool(unsigned long numThreads = 10, unsigned long upperLimit = 30);
		virtual ~WorkerPool();

		/** queue a task to the pool.
         *
         * @param task: new task to queue.
		 * @return void:
         */
		void queueTask (IWorkerTask* task);
        virtual void finishedTask(IThreadWorker* worker, IWorkerTask* task);
		
		/** set the service state, true: will process tasks, false: do nothing.
         *
         * @param work: service state.
		 * @return
         */
		void setServiceState(bool work){ m_bInService = work;};

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

	private:
		
		WorkerPool ( const WorkerPool & theInstance );
		WorkerPool & operator= ( const WorkerPool & rhs );

		void scanIdleTasks();
		void processTasks();
		ThreadWorker* getIdleWorker();

		void cleanUp();

	private:

		volatile bool				m_threadRunning;
		volatile bool				m_bInService;
		Semaphore					m_semaphore;
		ReEntrantThreadLockable		m_workersLock;
		ReEntrantThreadLockable		m_tasksLock;

		std::list<ThreadWorker*>	m_idleWorkers;
		std::list<ThreadWorker*>	m_allWorkers;
		std::list<IWorkerTask*>		m_idleTasks;
		std::list<IWorkerTask*>		m_toExecuteTasks;

		std::map<IWorkerTask*, bool>	m_allTasks;

        unsigned long               m_originalSize;
        volatile unsigned long      m_currentSize;
        unsigned long               m_upperLimit;

		timeb						m_lastAllTaskFinished;
	};
};

#endif
