/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/QueueProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/04/15 19:15:46 $
  * Last modified by:  $Author: limin.zhu $
  *
  * The QueueProcessor thread is responsible for processing queue items
  * using a pure virtual method provided by the subclass
  *
  */

#ifndef QUEUEPROCESSOR_H
#define QUEUEPROCESSOR_H

#include "core/threads/src/Thread.h"
#include "core/threads/src/QueueItem.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <deque>
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
	template<class ITEM> class QueueProcessor : public Thread
	{

	private:

		typedef typename std::deque< QueueItem<ITEM> > ItemQueue;

	public:
    
		QueueProcessor<ITEM> (unsigned long maxQueueSize = MAX_QUEUEPROCESSOR_QUEUE_SIZE, bool canDeleteItem = true, bool logStats = false) 
		:
		m_semaphore(0),
		m_maxQueueSize(maxQueueSize),
		m_keepRunning(true),
		m_logStats(logStats)
		{
		};

		~QueueProcessor<ITEM> () 
		{
			//TD13275
			//To ensure the thread is terminated before destruction.
			TA_ASSERT(THREAD_STATE_RUNNING != getCurrentState(), "QueueProcessor thread not terminated before destruction");
		};

		void insert( boost::shared_ptr<ITEM>& newItem )
		{
			//
			// Synch this to prevent multiple inserts occurring
			// simultaneously or  insert/getNext's on the same
			// object
			//
			// The size() operation is unlikely to be atomic
			//
			TA_THREADGUARD( m_queueLock );

			unsigned long queueSize = m_queue.size();
			if (queueSize == m_maxQueueSize)
			{
				// Reached out maximum capacity so we have to 
				// exercise damage control by dumping the oldest
				// items
				QueueItem<ITEM> oldestItem = m_queue.front();
				m_queue.pop_front();
			}
			QueueItem<ITEM> newPtr( newItem );
			m_queue.push_back( newPtr );

			m_semaphore.post();

			if ( m_logStats )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"Queue Processor size %u", queueSize );
			}
		}

		void insertUnique( boost::shared_ptr<ITEM>& newItem )
		{
			bool itemExists = false;

			//
			// Synch this to prevent multiple inserts occurring
			// simultaneously or  insert/getNext's on the same
			// object
			//
			TA_THREADGUARD( m_queueLock );

			if ( ! m_queue.empty() )
			{
				// see below - this compares the items pointed to by the pointers
				// not the pointer itself
				itemExists = ( std::find(m_queue.begin(), m_queue.end(), newItem)
							   != m_queue.end() );
			}

			if (!itemExists)
			{
				QueueItem<ITEM> newPtr( newItem );
				m_queue.push_back( newPtr );

				m_semaphore.post();

				if ( m_logStats )
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
						"Queue Processor size %u", m_queue.size() );
				}
			}
			else if ( m_logStats )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"Queue Processor duplicate item not queued");
			}
		}

		void insertReplace( boost::shared_ptr<ITEM>& newItem )
		{
			//
			// Synch this to prevent multiple inserts occurring
			// simultaneously or  insert/getNext's on the same
			// object
			//
			TA_THREADGUARD( m_queueLock );

			if ( ! m_queue.empty() )
			{
				typename ItemQueue::iterator itr = m_queue.begin();
				
				while (itr != m_queue.end())
				{
					if (true == itr->isReplacedBy(newItem))
					{
						itr = m_queue.erase(itr);
					}
					else
					{
						++itr;
					}
				}
			}

			QueueItem<ITEM> newPtr( newItem );
			m_queue.push_back( newPtr );

			m_semaphore.post();

			if ( m_logStats )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"Queue Processor size %u", m_queue.size() );
			}
		}

		unsigned long getQueueSize()
		{
			return m_queue.size();
		}

		void clearQueue()
		{
			TA_THREADGUARD( m_queueLock );
			m_queue.clear();
		}
		protected:

		//
		// Thread derived functions to start/stop the
		// QueueProcessor thread
		//
		virtual void run() 
		{
			//15120. if m_keepRunning is not set to true, the processor can not support restart.
			m_keepRunning = true; 
			
			if ( m_logStats )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"Queue Processor %u started", getId() );
			}

			while ( m_keepRunning )
			{
				// This call blocks until there is an event to process
				boost::shared_ptr<ITEM> newEvent = blockForItem();

				if (newEvent.get() != NULL)
				{
					processEvent( newEvent );

					if ( m_logStats )
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
							"Queue Processor size %u", m_queue.size() );
					}
				}
			}

			if ( m_logStats )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"Queue Processor %u finished", getId() );
			}
		};

		virtual void terminate()
		{
			m_keepRunning = false;
			unBlockQueue();
		};

		/**
		 * QueueProcessor::processEvent()
		 *
		 * Adds the new event request to the current bundle
		 *
		 * @param newEvent	the request for the new event
		 *
		 */
		virtual void processEvent( boost::shared_ptr<ITEM> newEvent ) = 0;
		
		boost::shared_ptr<ITEM> blockForItem() 
		{
   			m_semaphore.wait();

			boost::shared_ptr<ITEM> nextItem ((ITEM*) NULL);

			//
			// Synch this to prevent multiple inserts occurring
			// simultaneously or  insert/getNext's on the same
			// object
			//
			// The empty() operation is unlikely to be atomic
			//
			TA_THREADGUARD( m_queueLock );

			if ( m_queue.empty() )
			{
				return nextItem;
			}
			
			nextItem = m_queue.front().getItemPtr();
			m_queue.pop_front();
			return nextItem;
		}		

		void unBlockQueue()
		{
			m_semaphore.post();
		}


	private:

		ItemQueue										m_queue;
		NonReEntrantThreadLockable						m_queueLock;
		Semaphore										m_semaphore;
		unsigned long									m_maxQueueSize;
		bool											m_keepRunning;
		bool											m_logStats;
	};
};

#endif
