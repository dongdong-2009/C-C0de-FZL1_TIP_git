/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:     $File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/ThreadPoolManager.cpp $
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

#include "core/threads/src/ThreadPoolManager.h"
#include "core/threads/src/ThreadedWorker.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include <sstream>

namespace TA_Base_Core
{
    ThreadPoolManager::ThreadPoolManager(unsigned long numThreads, unsigned long upperLimit)
        : m_semaphore(0), //zn++
          m_originalSize(numThreads),
          m_currentSize(numThreads),
          m_upperLimit(upperLimit),
          m_isThdAvail(true) //zn++
    {
        m_workItemList.clear();
        m_threadWorkerList.clear();

        for (unsigned long i = 1; i <= numThreads; i++)
        {
            ThreadedWorker* newWorker = 0;
            newWorker = new ThreadedWorker(*this);

            // start up background thread of the workers
            newWorker->start();

            // save to internal list for later usage
            m_threadWorkerList.push_back(newWorker);
        }

        // start up the thread for background task
        m_threadRunning = true;
        start();
    }

    ThreadPoolManager::~ThreadPoolManager()
    {
        cleanUp();
    }

    void ThreadPoolManager::cleanUp()
    {
        //
        // stop the background task if the flag indicates that the thread has been
        // started
        //

        if (THREAD_STATE_RUNNING == getCurrentState())
        {
            terminateAndWait();
        }

        //
        // clean up internal lists
        //

        std::vector < ThreadedWorker* >::iterator threadedWorkerItr;

        for (threadedWorkerItr = m_threadWorkerList.begin();
                threadedWorkerItr != m_threadWorkerList.end();
                threadedWorkerItr++)
        {
            // stop the background task
            (*threadedWorkerItr)->terminateAndWait();

            // then delete the worker
            delete (*threadedWorkerItr);
        }

        m_threadWorkerList.clear();

        m_workItemList.clear();

        m_currentSize = 0;
    }

    void ThreadPoolManager::terminate()
    {
        // set the flag to indicate the thread is not supposed to be running anymore
        m_threadRunning = false;

        // notify the running thread to break the thread loop
        m_semaphore.post();
    }

    void ThreadPoolManager::run()
    {
        // wait here until m_semaphore.post() is called
        m_semaphore.wait();

        while (true == m_threadRunning)
        {
            // Catch all exceptions to prevent run() from dying pre-maturely
            try
            {
                // process the queued work item now
                processWorkItem();
            }
            catch (const TransactiveException & te)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
            }
            catch (const std::exception & e)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", e.what());
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught unknown exception in ThreadPoolManager::run");
            }

            // wait here until m_semaphore.post() is called
            m_semaphore.wait();
        }
    }

    void ThreadPoolManager::queueWorkItem(IWorkItemPtr workItem)
    {
        if (true == m_threadRunning)
        {
            // stop multiple threads accessing the internal list
            TA_THREADGUARD(m_workItemListLock);

            bool found = false;

            // for each of the work items currently in the list, find if we already
            // have the specified work item in the list, which has not been processed yet
            std::vector < IWorkItemPtr >::iterator workItemItr;

            for (workItemItr = m_workItemList.begin();
                    workItemItr != m_workItemList.end();
                    workItemItr++)
            {
                // if found a match, set the flag and exit loop
                if ((*workItemItr) == workItem)
                {
                    found = true;
                    break;
                }
            }

            // if found none
            if (false == found)
            {
                // put the new work item to the end of the list for processing
                m_workItemList.push_back(workItem);

                // notify the running thread to process the new work item
                m_semaphore.post();
            }

            // else do nothing
        }

        // else do nothing
    }

    void ThreadPoolManager::queueWorkItems(IWorkItemPtr item1, IWorkItemPtr item2, IWorkItemPtr item3, IWorkItemPtr item4, IWorkItemPtr item5)
    {
        queueWorkItem(item1);

        if (item2) { queueWorkItem(item2); }

        if (item3) { queueWorkItem(item3); }

        if (item4) { queueWorkItem(item4); }

        if (item5) { queueWorkItem(item5); }
    }

    void ThreadPoolManager::removeWorkItem(IWorkItemPtr workItem)
    {
        // stop multiple threads accessing the internal list
        TA_THREADGUARD(m_workItemListLock);

        bool found = false;

        // for each of the work items currently in the list, find if we already
        // had the specified work item in the list, which has not been processed yet
        std::vector < IWorkItemPtr >::iterator workItemItr;

        for (workItemItr = m_workItemList.begin();
                workItemItr != m_workItemList.end();
                workItemItr++)
        {
            // if found a match, set the flag and exit loop
            if ((*workItemItr) == workItem)
            {
                found = true;
                break;
            }
        }

        // if found one
        if (true == found)
        {
            // remove the work item from the list
            m_workItemList.erase(workItemItr);
        }

        // else do nothing
    }

    void ThreadPoolManager::processWorkItem()
    {
        // stop multiple threads accessing the internal list
        TA_THREADGUARD(m_workItemListLock);

        if (!m_isThdAvail) return;

        // only process if the list if not empty
        if (false == m_workItemList.empty())
        {
            // for each of the work item in the list
            std::vector < IWorkItemPtr >::iterator workItemItr = m_workItemList.begin();

            while (workItemItr != m_workItemList.end())
            {
                // get a non busy worker
                ThreadedWorker* threadedWorker = 0;
                threadedWorker = getNonBusyWorker();

                // if there is one
                if (0 != threadedWorker)
                {
                    // pass the scheduled callback to the threaded worker
                    threadedWorker->executeWorkItem(*workItemItr);

                    // remove the work item from the list and move onto the next
                    workItemItr = m_workItemList.erase(workItemItr);
                }
                else
                {
                    std::stringstream ss;
                    ss << "There are no threaded workers available to perform any of the queued work items.  Total number of queued work items = " << m_workItemList.size();
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str());
                    m_isThdAvail = false;

                    // exit loop
                    break;
                }
            }
        }

        // else do nothing

        /*
        {
            std::stringstream ss;
            ss << "End of processWorkItem.  Total number of queued work items = " << m_workItemList.size();
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, ss.str().c_str() );
        }
        */
    }

    void ThreadPoolManager::workerFree()
    {
        // a worker has become free, so signal the semaphore to unblock
        // so it can assign any remaining work items to any free threads
        {
            TA_THREADGUARD(m_workItemListLock);
            m_isThdAvail = true;
        }
        m_semaphore.post();
    }

    long ThreadPoolManager::getQueueSize()
    {
        TA_THREADGUARD(m_workItemListLock);
        return (long)m_workItemList.size();
    }

    unsigned long ThreadPoolManager::getOriginalSize()
    {
        return m_originalSize;
    }

    unsigned long ThreadPoolManager::getNumberOfThreads()
    {
        return m_currentSize;
    }

    unsigned long ThreadPoolManager::getUpperLimit()
    {
        return m_upperLimit;
    }

    void ThreadPoolManager::setUpperLimit(unsigned long upperLimit)
    {
        m_upperLimit = upperLimit;
    }

    ThreadedWorker* ThreadPoolManager::getNonBusyWorker()
    {
        ThreadedWorker* nonBusyWorker = 0;

        // for each of the available workers
        std::vector < ThreadedWorker* >::iterator itr;

        for (itr = m_threadWorkerList.begin();
                itr != m_threadWorkerList.end();
                itr++)
        {
            // if one of the worker is not busy
            if (false == (*itr)->getIsThreadWorkerBusy())
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Free thread found, current thread pool size is %d.", m_currentSize);
                // get handle to this worker and exit loop
                nonBusyWorker = (*itr);
                return nonBusyWorker;
            }
        }

        // check to see if the pool can grow...
        if (m_currentSize < m_upperLimit && nonBusyWorker == NULL)
        {
            ThreadedWorker* newWorker = 0;
            newWorker = new ThreadedWorker(*this);

            // start up background thread of the workers
            newWorker->start();

            // save to internal list for later usage
            m_threadWorkerList.push_back(newWorker);

            nonBusyWorker = newWorker;

            m_currentSize = m_threadWorkerList.size();
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "All threads are busy, new thread created, current thread pool size is %d.", m_currentSize);
        }

        return nonBusyWorker;
    }

    void ThreadPoolManager::waitForAll()
    {
        while (getQueueSize() || getBusyCount())
        {
            sleep(10);
        }
    }

    void ThreadPoolManager::waitForAll(size_t timeout)
    {
        for (size_t i = 0, STEP = 10; i < timeout; i += STEP)
        {
            if ((0 == getQueueSize() && 0 == getBusyCount()))
            {
                return;
            }

            sleep(STEP);
        }

        m_workItemList.clear();
        m_threadWorkerList.clear();
    }

    void ThreadPoolManager::waitForAny()
    {
        //TODO
        waitForAll();
    }

    size_t ThreadPoolManager::getBusyCount()
    {
        size_t count = 0;

        for (size_t i = 0; i < m_threadWorkerList.size(); ++i)
        {
            if (m_threadWorkerList[i]->getIsThreadWorkerBusy())
            {
                count++;
            }
        }

        return count;
    }
}
