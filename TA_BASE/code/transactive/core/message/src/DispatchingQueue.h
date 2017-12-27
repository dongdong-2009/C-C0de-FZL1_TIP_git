#if !defined(TA_DispatchingQueue_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_)
#define TA_DispatchingQueue_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/DispatchingQueue.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <deque>
#include <vector>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"
#include "core/utilities/src/DebugSETranslator.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"

// To allow BatchingQueue<T>::dispatchBatch() to catch CORBA::Exceptions for debugging. 
#include "core/corba/src/CorbaUtil.h"


namespace TA_Base_Core
{
    // TODO LPP: Rather than using a templated interface, it may be better to use
    // a functor template argument for the queue classes.
    template <class T>
    struct IQueueHandler
    {
        // onDispatchObjects() should return true if the batch was processed successfully.
        // If false is returned, NONE of the objects will be removed from the queue.
        // Exceptions must NOT be thrown.
        virtual bool onDispatchObjects(const std::vector<T*>& batch) = 0;

        virtual ~IQueueHandler() {}
    };

    // Clients push events onto the BatchingQueue by calling pushObjects, and they are
    // dispatched in BATCHES (in the same order that they were pushed) to a handler by
    // a worker thread.

    // NOTE: This worker thread must be started and stopped explicitly by the client.

    template <class T>
    class BatchingQueue : public TA_Base_Core::Thread
    {
    public:
        BatchingQueue();
        ~BatchingQueue();

        // handler can be NULL, in which case queue items are simply deleted.
        // BatchingQueue does NOT /*take*/ IQueueHandler, so don't forget to delete it.
        void setQueueHandler(IQueueHandler<T>* handler);
        IQueueHandler<T>* getQueueHandler();

        // setBatchingProperties() MUST be called (no default properties are set by the ctor).
        // The dispatching thread sleeps until either:
        // - more than 'batchSize' items have accumulated on the queue
        // - the time period 'batchSucceededIntervalMs' has expired
        // When it wakes up, batches of 'batchSize' items will be dispatched until there are
        // fewer than 'batchSize' items remaining on the queue.  If there were less than
        // 'batchSize' items on the queue and the time period expired, a partial batch will
        // be dispatched.
        // If a batch of items can't be dispatched (ie. IQueueHandler::onDispatchObjects() returns
        // false), the thread will sleep for 'batchFailedIntervalMs' and then try again.  In this
        // case, the thread will not wake up when items are pushed onto the queue.

        // TODO LPP: Would it be more appropriate for the queue to provide a flow control mechanism,
        // so that the IQueueHandler is able to pause dispatching (eg. when it knows it is unable
        // to handle further batches).
        virtual void setBatchingProperties( unsigned int batchSize,
                                            unsigned int batchSucceededIntervalMs,
                                            unsigned int batchFailedIntervalMs );

        // Objects in 'objs' must not be NULL.
        virtual void pushObjects(/*takes*/ const std::vector<T*>& objs);

        // Implementation of TA_Base_Core::Thread
        virtual void run();
        virtual void terminate();

    protected:
        IQueueHandler<T>* m_handler;

        // true if the thread is running.
        volatile bool m_running;
        volatile bool m_terminated;

        // Hold all the received objects to be dispatched to the handler.
        // Objects in the queue are deleted after they have been dispatched.
        typedef std::deque<T*> Queue_t;
        Queue_t m_queue;

        // Called to dispatch and remove up to m_batchSize items from the front of the queue.
        // false is returned if the IQueueHandler<T> returned false.
        bool dispatchBatch();

        // Used to synchronise access to the queue
        TA_Base_Core::NonReEntrantThreadLockable m_queueLock;

        // Condition for timed waiting on the queue
        Condition m_queueCondition;

        // Condition is signalled whenever queue size >= batch size.  Batches of m_batchSize
        // objects will be dispatched until queue size < batch size.
        unsigned int m_batchSize;

        // The maximum time (in ms) between sending of batches.
        unsigned int m_batchSucceededIntervalMs;

        // m_failed will be set if a batch of items can't be dispatched.  The dispatching
        // thread sleeps for m_batchFailedIntervalMs in this case.
        unsigned int m_batchFailedIntervalMs;
        bool m_failed;
    };


    template <class T>
    BatchingQueue<T>::BatchingQueue()
        : m_handler(NULL),
          m_running( false ),
          m_terminated( false ),
          m_batchSize(0),
          m_batchSucceededIntervalMs(0),
          m_batchFailedIntervalMs(0),
          m_failed(false)
    {
    }

    template <class T>
    BatchingQueue<T>::~BatchingQueue()
    {
        // Ensure that terminateAndWait() was called.
        TA_Assert(!m_running);

        TA_THREADGUARD( m_queueLock );

        // Delete all pending objects in the queue
        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BatchingQueue<T>::~BatchingQueue(): %s has %d objects that have not been dispatched",
            gPtrToStr(this).c_str(), m_queue.size() );

        for (typename Queue_t::iterator iter = m_queue.begin(); iter != m_queue.end(); ++iter)
        {
            delete *iter;
        }
    }

    template <class T>
    void BatchingQueue<T>::setQueueHandler(IQueueHandler<T>* handler)
    {
        // TODO LPP: setQueueHandler() is NOT threadsafe.  If it is called with handler == NULL
        // while popFrontOfQueue() is dispatching an event, it is possible that an attempt
        // will be made to dispatch to a NULL handler.
        m_handler = handler;
    }

    template <class T>
    IQueueHandler<T>* BatchingQueue<T>::getQueueHandler()
    {
        return m_handler;
    }

    template <class T>
    void BatchingQueue<T>::setBatchingProperties( unsigned int batchSize,
                                                  unsigned int batchSucceededIntervalMs,
                                                  unsigned int batchFailedIntervalMs )
    {
        // TODO LPP: setBatchingProperties() is NOT THREADSAFE.

        LOG4( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "BatchingQueue<T>::setBatchingProperties(): %s setting batchSize=%d, batchSucceededIntervalMs=%d, batchFailedIntervalMs=%d",
            gPtrToStr(this).c_str(), batchSize, batchSucceededIntervalMs, batchFailedIntervalMs );

        m_batchSize = batchSize;
        m_batchSucceededIntervalMs = batchSucceededIntervalMs;
        m_batchFailedIntervalMs = batchFailedIntervalMs;

        TA_Assert(m_batchSize > 0);

        // TODO LPP: What would a m_batchSucceededIntervalMs interval of 0 do?  It would be useful
        // if it caused an infinite wait.  We wouldn't want m_batchFailedIntervalMs to be 0 though.
        TA_Assert(m_batchSucceededIntervalMs > 0);
        TA_Assert(m_batchFailedIntervalMs > 0);
    }

    template <class T>
    void BatchingQueue<T>::pushObjects(/*takes*/ const std::vector<T*>& objs)
    {
        TA_Assert(objs.size() > 0);

        TA_THREADGUARD( m_queueLock );

        // MSVC6 limitation: The template method template<class InIt> void insert(iterator it, InIt first, InIt last)
        // is not available, so we have to use a slightly more convoluted notation.
        // According to the documentation, deque<T> iterators are invalidated when elements are inserted.
        m_queue.resize(m_queue.size() + objs.size());
        typename Queue_t::iterator iter = m_queue.end() - objs.size();
        std::copy(objs.begin(), objs.end(), iter);

        // The wait is signalled if the queue becomes large enough AND we are not currently
        // in the FAILED state.
        if (!m_failed && m_queue.size() >= m_batchSize)
        {
            ThreadGuard guard(m_queueCondition.getLinkedThreadLockable());
            m_queueCondition.signal();
        }
    }


    // STATE TRANSITIONS:
    //
    //             m_batchSucceededIntervalMs                 m_batchFailedIntervalMs
    //                      expires                                   expires
    //          --------------------------------          -------------------------------
    //         |                                |        |                               |
    //         |             Success            |        |                               |
    //         |    Queue size < m_batchSize    v        v                               |
    //       Sleep <------------------------- Dispatch Batch ------------------------> Sleep
    //         |                                ^   |    ^              FAILED
    //         |                                |   |    |         m_failed = true
    //         |                                |   |    |
    //          --------------------------------     ----
    //                    Item pushed               Success
    //             Queue size >= m_batchSize        Queue size >= m_batchSize

    template <class T>
    void BatchingQueue<T>::run()
    {
        TA_Assert(m_batchSize != 0);

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "BatchingQueue<T>::run(): %s begin to run", gPtrToStr(this).c_str() );

        if ( false == m_terminated )
        {
            m_running = true;
        }

        //xxs add for test, dispose the exception
        try
        {
            while (true)
            {
                // TODO LPP: Do we need to check m_running at various places (eg. before calling
                // dispatchBatch)?
                bool timerExpired = false;
                {
                    ThreadGuard guard(m_queueCondition.getLinkedThreadLockable());
                    if (m_running)
                    {
                        // Note that timedWait() returns 1 if successfully signalled or 0 if time expired.
                        timerExpired = !m_queueCondition.timedWait(m_failed ? m_batchFailedIntervalMs : m_batchSucceededIntervalMs);
                    }
                }

                if ( !m_running )
                {
                    LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "BatchingQueue<T>::run(): %s will be signalled by termniate", gPtrToStr(this).c_str() );
                    // We were signalled by terminate()
                    break;
                }

                // If we are in the FAILED state, we should NOT have been signalled by pushObjects()
                if ((true == m_failed) && (false == timerExpired))
                {
                    LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                            "BatchingQueue<T>::run(): %s, ERROR - we are in the FAILED state, we should NOT have been signalled by pushObjects()", gPtrToStr(this).c_str() );

                    continue;
                }

                unsigned int queueSize = 0;
                {
                    TA_THREADGUARD( m_queueLock );
                    queueSize = m_queue.size();
                }

                // The timedWait() will unblock periodically even if the queue is empty.
                // If it unblocked because we signalled it (ie. the queue size is larger than the batch
                // size), we send  batches of events until there are fewer than m_batchSize objects in
                // the queue.
                // If it unblocked because the timer expired, at least one batch will be sent even if it
                // is smaller than the batch size.
                while ( queueSize > 0 && ( queueSize > m_batchSize || timerExpired ) )
                {
                    // A partial batch will only be sent if the timer expired and there were less than
                    // m_batchSize items on the queue.
                    timerExpired = false;

                    bool success = dispatchBatch();
                    if ( success )
                    {
                        TA_THREADGUARD( m_queueLock );
                        queueSize = m_queue.size();
                        m_failed = false;
                    }
                    else
                    {
                        // The dispatch failed.  We will wait up to m_batchFailedIntervalMs, then try again.
                        TA_THREADGUARD( m_queueLock );

                        if (!m_failed)
                        {
                            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                "BatchingQueue<T>::run(): %s, dispatchBatch failed", gPtrToStr(this).c_str() );
                        }
                        
                        m_failed = true;
                        break;
                    }
                }
            }
        }
        //it seems could not happen
        catch(TransactiveException & e)
        {
            std::string msg( "BatchingQueue<T>::run -- " );
            msg.append( gPtrToStr(this) );
            msg.append( " has TransactiveException:");
            msg.append( e.what() );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
        }
        catch (std::exception& e)
        {
            std::string msg( "BatchingQueue<T>::run -- " );
            msg.append( gPtrToStr(this) );
            msg.append( " has std::exception:");
            msg.append( e.what() );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "BatchingQueue<T>::run -- " );
            msg.append( gPtrToStr(this) );
            msg.append( " has CORBA::exception:");
            msg.append( CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
        }
        catch(...)
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "BatchingQueue<T>::run has exception: %s ", gPtrToStr(this).c_str() );
        }

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "BatchingQueue<T>::run(): %s stop running", gPtrToStr(this).c_str() );
    }

    template <class T>
    void BatchingQueue<T>::terminate()
    {
        ThreadGuard guard(m_queueCondition.getLinkedThreadLockable());

        // The thread has been terminated
        m_running = false;
        m_terminated = true;
        m_queueCondition.signal();
    }

    template <class T>
    bool BatchingQueue<T>::dispatchBatch()
    {
        // Copy up to m_batchSize items from the queue
        std::vector<T*> batch;
        batch.reserve(m_batchSize);

        int queueSize = 0;
        {
            TA_THREADGUARD( m_queueLock );

            TA_Assert(m_queue.size() > 0);
            TA_Assert(m_batchSize > 0);

            for ( typename Queue_t::iterator iter = m_queue.begin(); (iter != m_queue.end()) && (batch.size() < m_batchSize); ++iter )
            {
                batch.push_back(*iter);
            }

            // We access the queue size in the LOG statement below, so we need a snapshot while we have the lock.
            queueSize = m_queue.size();

            TA_Assert(batch.size() > 0);
        }

        // Dispatch to the handler
        LOG4(SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "BatchingQueue<T>::dispatchBatch: %s dispatching %d of %d objects on queue to handler %s",
            gPtrToStr(this).c_str(), batch.size(), queueSize, gPtrToStr(m_handler).c_str() );

        bool success = false;
        if ( m_handler != NULL )
        {
            TA_Assert(m_handler != NULL);

            // This try-catch block is an aid for debugging.  Without it, the application might
            // terminate without warning if exceptions are thrown (because the exception will
            // propagate out of the Threads run() method).
            try
            {
                // Exceptions must not propagate out of IQueueHandler<T>::onDispatchObjects.
                success = m_handler->onDispatchObjects(batch);
            }
            catch (WinStructuredException& e)
            {
                std::ostringstream stm;
                stm << "runThread(): WinStructuredException exceptionCode == " << std::hex << e.m_exceptionCode;
                LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), stm.str().c_str() );

                DebugUtil::getInstance().logStackTrace(gContextFromExceptionInfo(e.m_exceptionInfo));

                // TA_Assert(0);
            }
            catch (std::exception& e)
            {
                std::string msg( "BatchingQueue<T>::dispatchBatch(): " );
                msg.append( e.what() );
                LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
                // TA_Assert(0);
            }
            catch (CORBA::Exception& e)
            {
                std::string msg( "BatchingQueue<T>::dispatchBatch(): " );
                msg.append( CorbaUtil::getInstance().exceptionToString(e) );
                LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
                // TA_Assert(0);
            }
            catch (...)
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "BatchingQueue<T>::dispatchBatch(): Unknown exception" );
                // TA_Assert(0);
            }
        }
        else
        {
            // Events are discarded immediately if there is no handler
            success = true;
        }

        if (success)
        {
            //TD17994,jianghp
            TA_THREADGUARD( m_queueLock );
            //TD17994,jianghp
            // Delete the dispatched objects
            for (typename std::vector<T*>::iterator iter = batch.begin(); iter != batch.end(); ++iter)
            {
                delete *iter;
                //TD17994,jianghp
                *iter = NULL;
                //TD17994,jianghp
            }

            // Truncate the dispatched objects from the front of the queue
            m_queue.erase(m_queue.begin(), m_queue.begin() + batch.size());
        }
        else
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                "BatchingQueue<T>::dispatchBatch: %s FAILED to dispatch objects as handler returned false",
                gPtrToStr(this).c_str() );
        }

        return success;
    }

}; // TA_Base_Core


#endif // !defined(TA_DispatchingQueue_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_)
