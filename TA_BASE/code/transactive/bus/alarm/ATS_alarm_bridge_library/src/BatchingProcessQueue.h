// BatchingProcessQueue.h: interface for the BatchingProcessQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATCHINGPROCESSQUEUE_H__E0172882_6C18_4CE1_84B6_C3B5062D8005__INCLUDED_)
#define AFX_BATCHINGPROCESSQUEUE_H__E0172882_6C18_4CE1_84B6_C3B5062D8005__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#pragma warning(disable:4541)
#endif // defined (_MSC_VER)

 
#include <deque>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "core/corba/src/CorbaUtil.h"
 

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
	template <typename T>
	class BatchingProcessQueue  : public TA_Base_Core::Thread
	{
	public:
		BatchingProcessQueue();
		virtual ~BatchingProcessQueue();
		void setBatchingProperties(
		unsigned int batchSize,                   
		unsigned int batchSucceededIntervalMs,
		unsigned int batchFailedIntervalMs);
		
		virtual bool onDispatchObjects(std::vector< boost::shared_ptr<T> >& batch)=0;
		// Objects in 'objs' must not be NULL.
		void pushObject(/*takes*/ boost::shared_ptr<T>& obj);
		
		// Implementation of TA_Base_Core::Thread
		virtual void run();
		virtual void terminate();
	private:
		// true if the thread is running.
		volatile bool m_running;
	 
		
        // Hold all the received objects to be dispatched to the handler.
        // Objects in the queue are deleted after they have been dispatched.
		typedef std::deque< boost::shared_ptr<T> > Queue_t;
		Queue_t m_queue;

		// Called to dispatch and remove up to m_batchSize items from the front of the queue.
        // false is returned if the IQueueHandler<T> returned false.
		bool dispatchBatch();
        
		// Used to synchronise access to the queue
		TA_Base_Core::NonReEntrantThreadLockable m_queueLock;
		
        // Condition for timed waiting on the queue
		TA_Base_Core::Condition m_queueCondition;
		
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
		BatchingProcessQueue<T>::BatchingProcessQueue() : m_running( false ), 
		m_batchSize(0),
		m_batchSucceededIntervalMs(0),
		m_batchFailedIntervalMs(0),
		m_failed(false)
	{
		
	}

    template <class T>
	BatchingProcessQueue<T>::~BatchingProcessQueue()
	{
		// Ensure that terminateAndWait() was called.
		TA_Assert(!m_running); 
		
		TA_Base_Core::ThreadGuard guard(m_queueLock);
		
		// Delete all pending objects in the queue
		LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"BatchingQueue<T>::~BatchingQueue(): %s has %d objects that have not been dispatched", 
			gPtrToStr(this).c_str(), m_queue.size() );		
// 		for (typename Queue_t::iterator iter = m_queue.begin(); iter != m_queue.end(); ++iter)
// 		{
// 			delete *iter;
// 		}
        m_queue.clear();
		
	}


	template <class T>
		void BatchingProcessQueue<T>::setBatchingProperties(
        unsigned int batchSize, 
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
		void BatchingProcessQueue<T>::pushObject(/*takes*/ boost::shared_ptr<T>& obj)
    {
        //TA_Assert(objs.size() > 0);
		
		TA_Base_Core::ThreadGuard guard(m_queueLock);
		m_queue.push_back(obj);
        // MSVC6 limitation: The template method template<class InIt> void insert(iterator it, InIt first, InIt last)
        // is not available, so we have to use a slightly more convoluted notation.
        // According to the documentation, deque<T> iterators are invalidated when elements are inserted.
        //m_queue.resize(m_queue.size() + 1);
        //typename Queue_t::iterator iter = m_queue.end() //- objs.size();
        //std::copy(objs.begin(), objs.end(), iter);
		
        // The wait is signalled if the queue becomes large enough AND we are not currently 
        // in the FAILED state.
        if (!m_failed && m_queue.size() >= m_batchSize)
        {
            ThreadGuard guard(m_queueCondition.getLinkedThreadLockable());
			m_queueCondition.signal();
        }
    }

    template <class T>
    void BatchingProcessQueue<T>::run()
    {
        TA_Assert(m_batchSize != 0);

	    LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "BatchingQueue<T>::run(): %s begin to run", gPtrToStr(this).c_str() );
        
        m_running = true;
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
					TA_Base_Core::ThreadGuard guard( m_queueLock );
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
						TA_Base_Core::ThreadGuard guard( m_queueLock );
						queueSize = m_queue.size();
						m_failed = false;
					}
					else
					{
						// The dispatch failed.  We will wait up to m_batchFailedIntervalMs, then try again.
						TA_Base_Core::ThreadGuard guard(m_queueLock);
						m_failed = true;
						LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
							"BatchingQueue<T>::run(): %s, dispatchBatch failed", gPtrToStr(this).c_str() );
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
		catch(...)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"BatchingQueue<T>::run has exception: %s ", gPtrToStr(this).c_str() );
		}

	    LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "BatchingQueue<T>::run(): %s stop running", gPtrToStr(this).c_str() );
    }

    template <class T>
    void BatchingProcessQueue<T>::terminate()
    {
        ThreadGuard guard(m_queueCondition.getLinkedThreadLockable());

	    // The thread has been terminated
	    m_running = false;
	    m_queueCondition.signal();
    }
    
	 template <class T>
    bool BatchingProcessQueue<T>::dispatchBatch()
    {
        // Copy up to m_batchSize items from the queue
		std::vector<boost::shared_ptr<T> > batch;
        batch.reserve(m_batchSize);

        int queueSize = 0;
		{
			TA_Base_Core::ThreadGuard guard( m_queueLock );

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
	    LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub, 
            "BatchingQueue<T>::dispatchBatch: %s dispatching %d of %d objects on queue", 
            gPtrToStr(this).c_str(), batch.size(), queueSize );

        bool success = false;        
            // This try-catch block is an aid for debugging.  Without it, the application might
            // terminate without warning if exceptions are thrown (because the exception will
            // propagate out of the Threads run() method).
        try
        {
            // Exceptions must not propagate out of IQueueHandler<T>::onDispatchObjects.
    		success = onDispatchObjects(batch);
        } 
        catch (std::exception& e)
        {
			std::string msg( "BatchingQueue<T>::dispatchBatch(): " );
			msg.append( e.what() );
			LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
            
        }
        catch (...)
        {
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "BatchingQueue<T>::dispatchBatch(): Unknown exception" );
             
        }
		 
		 

        if (success)
        {
			//TD17994,jianghp
			ThreadGuard guard( m_queueLock );			
            // Truncate the dispatched objects from the front of the queue
            m_queue.erase(m_queue.begin(), m_queue.begin() + batch.size());
        }
        else
        {
	        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "BatchingQueue<T>::dispatchBatch: %s FAILED to dispatch objects as handler returned false", 
                gPtrToStr(this).c_str() );
        }

        return success;
    }


}

#endif // !defined(AFX_BATCHINGPROCESSQUEUE_H__E0172882_6C18_4CE1_84B6_C3B5062D8005__INCLUDED_)
