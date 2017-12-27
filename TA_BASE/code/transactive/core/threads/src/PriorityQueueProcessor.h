/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/PriorityQueueProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * The PriorityQueueProcessor thread is responsible for processing queue items
  * using a pure virtual method provided by the subclass
  *
  */

#ifndef PRIORITYQUEUEPROCESSOR_H
#define PRIORITYQUEUEPROCESSOR_H

#include "core/threads/src/Thread.h"
#include "core/threads/src/QueueItem.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include <queue>
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{

	//
	//  PriorityQueueProcessor
	//
	template<class ITEM> class PriorityQueueProcessor : public Thread
	{

	private:

		typedef typename std::priority_queue< PriorityQueueItem<ITEM> > ItemQueue;

	public:
    
		PriorityQueueProcessor<ITEM> (unsigned long maxQueueSize = MAX_QUEUEPROCESSOR_QUEUE_SIZE) 
		:
		m_semaphore(0),
		m_maxQueueSize(maxQueueSize),
		m_keepRunning(true),
		m_logStats(false)
		{

			if ( true == RunParams::getInstance().isSet ( "LogStatistics" ) )
			{
				m_logStats = true;
			}
		};

		~PriorityQueueProcessor<ITEM> () 
		{
			//TD13275
			//To ensure the thread is terminated before destruction.
			TA_ASSERT(THREAD_STATE_RUNNING != getCurrentState(), "PriorityQueueProcessor thread not terminated before destruction");
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
				PriorityQueueItem<ITEM> oldestItem = m_queue.top();
				m_queue.pop();
			}

			PriorityQueueItem<ITEM> newPtr( newItem );
			m_queue.push( newPtr );

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
			//f
			TA_THREADGUARD( m_queueLock );
			
			ItemQueue newQueue;

			while ( ! m_queue.empty() )
			{
				PriorityQueueItem<ITEM> currentItem = m_queue.top();
				m_queue.pop();
				newQueue.push(currentItem);

				itemExists = itemExists || (currentItem == newItem);
			}

			m_queue = newQueue;

			if (false == itemExists)
			{
				PriorityQueueItem<ITEM> newPtr( newItem );
				m_queue.push( newPtr );

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

			ItemQueue newQueue;

			while ( ! m_queue.empty() )
			{
				PriorityQueueItem<ITEM> currentItem = m_queue.top();
				m_queue.pop();

				if (false == currentItem.isReplacedBy(newItem))
				{
					newQueue.push(currentItem);
				}
			}

			PriorityQueueItem<ITEM> newPtr( newItem );
			newQueue.push( newPtr );

			m_queue = newQueue;

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

		protected:

		//
		// Thread derived functions to start/stop the
		// PriorityQueueProcessor thread
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
		 * PriorityQueueProcessor::processEvent()
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
			
			nextItem = m_queue.top().getItemPtr();
			m_queue.pop();
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
