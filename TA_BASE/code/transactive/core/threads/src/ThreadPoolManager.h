/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:     $File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/ThreadPoolManager.h $
  * @author:    HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  * ThreadPoolManager provides a single interface for scheduling of asynchronous operations,
  * which are represented by instances of IWorkItem interface.  For the current design, ThreadPoolManager
  * creates and manages ten (10) threaded workers to execute the registered asynchronous operations.
  *
  */

#ifndef THREAD_POOL_MANAGER_H
#define THREAD_POOL_MANAGER_H

#include "core/threads/src/IThreadPoolWorkerCallback.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"
#include "core/threads/src/IWorkItem.h"
#include <vector>

namespace TA_Base_Core
{
    class ThreadedWorker;
};

namespace TA_Base_Core
{
    class ThreadPoolManager : public Thread, public IThreadPoolWorkerCallback
    {
    public:

        ThreadPoolManager(unsigned long numThreads = 30, unsigned long upperLimit = 30);

        virtual ~ThreadPoolManager();

        //
        // Operations specific to ThreadPoolManager
        //

        /**
          *
          * queueWorkItem
          *
          * Queue the specified IWorkItem for processing by a thread in the thread pool
          * and return immediately
          *
          * @param    workItem    The work item to be processed by a thread
          *
          */
        void queueWorkItem(IWorkItemPtr workItem);
        void queueWorkItems(IWorkItemPtr item1,
                            IWorkItemPtr item2 = IWorkItemPtr(),
                            IWorkItemPtr item3 = IWorkItemPtr(),
                            IWorkItemPtr item4 = IWorkItemPtr(),
                            IWorkItemPtr item5 = IWorkItemPtr());

        /**
          *
          * removeWorkItem
          *
          * Remove the specified IWorkItem so that it is no longer processed by a thread
          * in the thread pool.  This will block until the specified IWorkItem is successfully
          * removed from the ThreadPool.
          *
          * @param    workItem    The work item to be processed by a thread
          *
          */
        void removeWorkItem(IWorkItemPtr workItem);

        /**
          * getOriginalSize
          *
          * @return the original pool size
          */
        unsigned long getOriginalSize();

        /**
          * getNumberOfThreads
          *
          * @return the current thread pool size
          */
        unsigned long getNumberOfThreads();

        /**
          * getUpperLimit
          *
          * @return the maximum size the pool can be
          */
        unsigned long getUpperLimit();

        /**
          * setUpperLimit
          *
          * @return the maximum size the pool can be
          */
        void setUpperLimit(unsigned long upperLimit);

        /* IThreadPoolWorkerCallback implementation
         * called by worker threads */

        /**
          *
          * workerFree
          *
          * This is called by a worker when it becomes free
          *
          */
        virtual void workerFree();

        void cleanUp();

        /**
          *
          * The following methods provides statistics
          */
        long getQueueSize(void);

        void waitForAll();
        void waitForAll(size_t timeout);
        void waitForAny();

    protected:

        /**
          *
          * processWorkItem
          *
          * Obtain the work item, from the begining of the list, and pass to
          * the ThreadedWorkers for processing
          *
          */
        void processWorkItem();

        /**
          *
          * getNonBusyWorker
          *
          * Return a pointer to a threaded worker that is not currently
          * any work.  NULL pointer is returned if all available workers
          * are busy.
          *
          */
        ThreadedWorker* getNonBusyWorker();
        size_t getBusyCount();

        //
        // Operations required by Thread
        //

        virtual void run();
        virtual void terminate();

    private:

        ThreadPoolManager(const ThreadPoolManager & theInstance);
        ThreadPoolManager & operator= (const ThreadPoolManager & rhs);

    protected:

        Semaphore                               m_semaphore;
        ReEntrantThreadLockable                 m_workItemListLock;
        volatile bool                           m_threadRunning;

        std::vector < IWorkItemPtr >            m_workItemList;
        std::vector < ThreadedWorker* >        m_threadWorkerList;

        unsigned long                           m_originalSize;
        volatile unsigned long                  m_currentSize;
        unsigned long                           m_upperLimit;
        volatile bool                           m_isThdAvail;   //zn++
    };
};

#endif
