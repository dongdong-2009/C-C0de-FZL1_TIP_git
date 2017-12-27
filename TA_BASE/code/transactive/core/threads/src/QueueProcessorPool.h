/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/QueueProcessorPool.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Kind of like a thread pool. Except items can be unique.
  * Items with the same ID or key are always assigned to the same thread
  * this preserves order of execution.
  * so its a thread pool, where the queue is maintained in each thread rather
  * than one queue for all threads.
  */

#if !defined(QUEUEPROCESSORPOOL_H)
#define QUEUEPROCESSORPOOL_H


#include <vector>
#include "core/threads/src/QueueProcessor.h"
#include "core/threads/src/PriorityQueueProcessor.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    template<class ITEM> class QueueProcessorPoolCallback
    {
    public:

		virtual ~QueueProcessorPoolCallback() {};

		virtual void queueProcessorPoolCallback( boost::shared_ptr<ITEM> item ) = 0;
    };

//    template<class ITEM> class QueueProcessorWorker;

    template<class ITEM, class PROCESSORWORKER> class QueueProcessorPool
	{
		public:
			
            QueueProcessorPool(int numberOfWorkers, QueueProcessorPoolCallback<ITEM>& callback, bool startWorkers = true)
                : m_numWorkers(numberOfWorkers)
            {
                for (int i = 0; i < m_numWorkers; ++i)
                {
                    m_workers.push_back(new PROCESSORWORKER(callback));
                }
			
                if ( true == startWorkers )
                {
                    startProcessing();
                }
            }

			virtual ~QueueProcessorPool()
            {
                // clean up workers
                for ( typename std::vector<PROCESSORWORKER*>::iterator iter = m_workers.begin();
                      iter != m_workers.end(); ++iter )
                {
                    (*iter)->terminateAndWait(); //TD13275
                    delete (*iter);
                }
                m_workers.clear();
            }
        
        
            /**
             * This will start the queue processors running
             * so items can be processed
             */
            void startProcessing()
            {
				for ( typename std::vector< PROCESSORWORKER* >::iterator iter = m_workers.begin();
                      iter != m_workers.end(); ++iter )
                {
                    (*iter)->start();
                }
            }
           
            
            /**
             * This will stop the queue processors
             */    
            void stopProcessing()
            {
                for ( typename std::vector<PROCESSORWORKER*>::iterator iter = m_workers.begin();
                      iter != m_workers.end(); ++iter )
                {
                    (*iter)->terminateAndWait();
                }
            }
        

            /**
		     * queueItem()
		     *
		     * adds the item for processing. The itemId is used
             * to allocate to the appropriate thread (itemId % m_numWorkers)
		     */
            void queueItem( unsigned long itemId, boost::shared_ptr<ITEM>& item )
            {
                // add it to the queue to be processed
                if (NULL != item.get())
                {
                    // pick a worker
                    int worker = itemId % m_numWorkers;
                    m_workers[worker]->insert(item);
                }
            }

            /**
		     * queueItem()
		     *
		     * adds the item for processing (uniquely). The itemId is used
             * to allocate to the appropriate thread (itemId % m_numWorkers)
		     */
            void queueItemUnique( unsigned long itemId, boost::shared_ptr<ITEM>& item )
            {
                // add it to the queue to be processed
                if (NULL != item.get())
                {
                    // pick a worker
                    int worker = itemId % m_numWorkers;
                    m_workers[worker]->insertUnique(item);
                }
            }

			/**
		     * queueItemReplace()
		     *
		     * adds the item for processing (replacing all previous requests). The itemId is used
             * to allocate to the appropriate thread (itemId % m_numWorkers)
		     */
            void queueItemReplace( unsigned long itemId, boost::shared_ptr<ITEM>& item )
            {
                // add it to the queue to be processed
                if (NULL != item.get())
                {
                    // pick a worker
                    int worker = itemId % m_numWorkers;
                    m_workers[worker]->insertReplace(item);
                }
            }

			std::vector<unsigned long> getQueueSizes()
			{
				std::vector<unsigned long> sizes;
				typename std::vector<PROCESSORWORKER*>::iterator iter;
                for (iter = m_workers.begin(); iter != m_workers.end(); ++iter)
                {
                    sizes.push_back((*iter)->getQueueSize());
                }

				return sizes;
			}


		private:
            QueueProcessorPool();

            // Worker threads
            int                                         m_numWorkers;
            std::vector< PROCESSORWORKER* >				m_workers;
	};

    template<class ITEM> class QueueProcessorWorker : public QueueProcessor<ITEM>
    {
        public:
			QueueProcessorWorker(QueueProcessorPoolCallback<ITEM>& callback)
              : m_callback(callback)
            {}

            virtual ~QueueProcessorWorker() {}

        protected:

            /**
		     * processItem()
		     *
		     * called by QueueProcessor for items to process
		     */
			void processEvent( boost::shared_ptr<ITEM> newItem )
            {
                m_callback.queueProcessorPoolCallback(newItem);
            }

        private:
            QueueProcessorWorker();

            QueueProcessorPoolCallback<ITEM>& m_callback;
    };

	template<class ITEM> class PriorityQueueProcessorWorker : public PriorityQueueProcessor<ITEM>
    {
        public:
			PriorityQueueProcessorWorker(QueueProcessorPoolCallback<ITEM>& callback)
              : m_callback(callback)
            {}

            virtual ~PriorityQueueProcessorWorker() {}

        protected:

            /**
		     * processItem()
		     *
		     * called by QueueProcessor for items to process
		     */
		    void processEvent( boost::shared_ptr<ITEM> newItem )
            {
                m_callback.queueProcessorPoolCallback(newItem);
            }

        private:
            PriorityQueueProcessorWorker();

            QueueProcessorPoolCallback<ITEM>& m_callback;
    };

}
#endif // !defined(QUEUEPROCESSORPOOL_H)
