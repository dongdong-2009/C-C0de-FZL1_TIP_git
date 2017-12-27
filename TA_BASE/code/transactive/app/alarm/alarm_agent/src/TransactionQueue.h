/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File$
  * @author:  Ross Tucker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * <description>
  *
  */
#if !defined(TRANSACTIONQUEUE)
#define TRANSACTIONQUEUE

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include <deque>

namespace TA_Base_App
{
    // CL-21508
    class AlarmQueueWrapper;

    class TransactionQueue : public virtual TA_Base_Core::Thread
    {
    public:

        // CL-21508 begin
//         typedef std::deque<TA_Base_Core::AlarmAgentQueueItemCorbaDef*> UpdateQueue;
        typedef std::deque<AlarmQueueWrapper*> UpdateQueue;
        // CL-21508 end

        /**
          * Constructor
          */
        TransactionQueue();

        /**
          * Destructor
          */
        virtual ~TransactionQueue();

        /**
          * push
          *
          * This method enables clients to push a new IQueueItem onto
          * our queue
          *
          * @param A pointer to the IQueueItem to be pushed
          */
        // CL-21508 begin
//         virtual void push( TA_Base_Core::AlarmAgentQueueItemCorbaDef* p_item );
        virtual void push( AlarmQueueWrapper* pWrapper );
        // CL-21508 end

        /**
          * getCurrentQueue
          *
          * Returns a snapshot of the current queue content.
          *
          * @return A sequence of AlarmAgentQueueItemCorbaDef objects.
          */
        virtual TA_Base_Core::AlarmAgentQueueItemSequence* getCurrentQueue();

        /**
          * clear
          *
          * This method clears the queue
          */
        virtual void clear();

        /**
          * run
          *
          * Overides the pure virtual run method declared in Thread
          */
        virtual void run() = 0;

        /**
          * terminate
          *
          * Overides the pure virtual terminate method declared in Thread
          */
        virtual void terminate() = 0;

        /**
          * removeQueueItem
          *
          * This method is called when the agent is in Monitor mode to allow
          * queue synchronisation. It results in the queue item being deleted from
          * the queue.
          *
          * @param An AlarmAgentQueueItemUpdate
          */
        //virtual void removeQueueItem(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item) = 0;

		virtual void MoveToWriteQueue();


		virtual void remove(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item);
	

		 

    private:

		
		void cleanUpdateQeueu();
		void cleanWriteQueue();

		bool removeUpdateQueue(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item);
		bool removeWriteQueue(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item);
        // The queue
        UpdateQueue m_updateQueue;


		UpdateQueue m_writeQueue;

    protected:

        // Thread lock for the cache
        TA_Base_Core::NonReEntrantThreadLockable m_lock;
		 TA_Base_Core::NonReEntrantThreadLockable m_lockWriteQueue;
		TA_Base_Core::Semaphore m_queueSemaphore;

        bool m_terminated;

        /**
          * getQueue
          *
          * This method returns a reference to the Queue.
          */
        UpdateQueue& getWriteQueue();

		 /**
          * popTop
          *
          * This method returns a pointer to the top AlarmQueueWrapper of the Queue.
          */
		AlarmQueueWrapper* popTopWriteQueue();

		void pushTopWriteQueue(AlarmQueueWrapper* pWrapper );

		/**
          * isEmptyWriteQueue
          *
          * This method returns true if Queue is empty.
          */
		bool isEmptyWriteQueue();

		/**
          * size
          *
          * This method returns the size of Queue.
          */
		size_t size();
        
    };
}

#endif // !defined(TRANSACTIONQUEUE)
