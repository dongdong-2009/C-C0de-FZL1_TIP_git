/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/BlockingSingletonQueue.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *	Template for a singleton queue object. Example 
  *	implementations are DataPointWriteQueue and 
  *	DataPointAlarmQueue.
  *
  *
  */

#ifndef BLOCKING_SINGLETON_QUEUE_H
#define BLOCKING_SINGLETON_QUEUE_H

#include <deque>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/utilities/src/DebugUtil.h"


const unsigned long MAX_BLOCKING_SINGLETON_QUEUE_SIZE = 1000000;

template<class ITEM> class BlockingSingletonQueue
{

public:
		

	/**
  	 * BlockingSingletonQueue::getInstance()
  	 *
  	 * Returns a pointer to the single instance
	 * of SingeltonQueue, creating if necessary.
	 *
	 * @return 	a pointer to a SingeltonQueue
  	 *
  	 */
	static BlockingSingletonQueue<ITEM> * getInstance()
	{
		if ( m_singleton == NULL )
		{
			//
			// Double checking to prevent multiple threads 
			// creating multiple instances
			//
			TA_THREADGUARD( m_queueLock );
			if ( m_singleton == NULL )
			{
				m_singleton = new BlockingSingletonQueue<ITEM>();
				m_semaphore = new TA_Base_Core::Semaphore(0);
			}
		}
		++m_count;
		return m_singleton;
	}	

	static void removeInstance( BlockingSingletonQueue<ITEM> * removeMe )
	{
		if ( removeMe == m_singleton )
		{
			if ( --m_count == 0 )
			{
				// 
				// Guard this to prevent multiple threads atempting
				// to delete/create simultaneously
				//
				TA_THREADGUARD( m_queueLock );
				if ( m_singleton != NULL )
				{
					delete m_singleton;
					m_singleton = NULL;
				}
				if ( m_semaphore != NULL )
				{
					delete m_semaphore;
					m_semaphore = NULL;
				}
			}
			removeMe = NULL;
		}
	}

    void setMaxQueueSize(unsigned long maxQueueSize ) 
    {
  		//
		// Synch this as the operation is unlikely to be atomic
		//
        TA_THREADGUARD( m_queueLock );
        m_maxQueueSize = maxQueueSize; 
    }

    void setCanDeleteItem(bool canDeleteItem) 
    {
  		//
		// Synching not required as only one bit is affected and 
        // so assignment is atomic
		//
        
        m_canDeleteItem = canDeleteItem; 
    }


	unsigned int getSize() 
    { 
        //
		// Synch this as the operation is unlikely to be atomic
		//
        TA_THREADGUARD( m_queueLock );
        return m_queue.size(); 
    }


	bool itemExists( ITEM * checkItem ) 
	{
        bool itemExists(false);
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
            return itemExists;
        }
        
        // see below - this compares the items pointed to by the pointers
        // not the pointer itself
        itemExists = ( std::find(m_queue.begin(), m_queue.end(), checkItem)
                       != m_queue.end() );

		return itemExists;
	}		

    void insert( ITEM * newItem )
	{
		//
		// Synch this to prevent multiple inserts occurring
		// simultaneously or  insert/getNext's on the same
		// object
		//
        // The size() operation is unlikely to be atomic
        //
        TA_THREADGUARD( m_queueLock );

        if (m_queue.size() == m_maxQueueSize)
        {
            // Reached out maximum capacity so we have to 
            // exercise damage control by dumping the oldest
            // items
            ItemPtr<ITEM*> oldestItem = m_queue.front();
			m_queue.pop_front();
            if (m_canDeleteItem)
            {
                delete oldestItem.ptr;
            }
        }
		m_queue.push_back( newItem );

        m_semaphore->post();
	}

	void insertUnique( ITEM * newItem )
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
			ItemPtr<ITEM*> newPtr( newItem );
			m_queue.push_back( newPtr );

			m_semaphore->post();
		}
		else if (m_canDeleteItem)
		{
			delete newItem;
		}
	}
	
    ITEM * blockForItem() 
	{
   		m_semaphore->wait();

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
            return NULL;
        }
		
		ItemPtr<ITEM*> nextItem = m_queue.front();
		m_queue.pop_front();
		return nextItem.ptr;
	}		

	void unBlockQueue()
	{
		m_semaphore->post();
	}

	
private:

	BlockingSingletonQueue<ITEM>() 
     : m_maxQueueSize(MAX_BLOCKING_SINGLETON_QUEUE_SIZE),
       m_canDeleteItem(true) 
    {}

	~BlockingSingletonQueue<ITEM>() 
	{
        //
        // Empty the queue
        //
        while( ! m_queue.empty() )
        {
            ItemPtr<ITEM*> nextItem = m_queue.front();
            m_queue.pop_front();
            if (m_canDeleteItem)
            {
                delete nextItem.ptr;
            }
        }
	}

    // the queue holds pointers
    // we want to insert unique, we dont want to insert unique
    // pointers, but unique items, so we must define a structure so
    // an operator== that compares items rather than pointers can be used
    template<class POINTER> struct ItemPtr
    {
        POINTER ptr;

        ItemPtr ( POINTER itemPtr )
        {
            ptr = itemPtr;
        }

        bool operator== ( const POINTER& rhs )
        {
            return *ptr == *rhs;
        }

        bool operator== ( const ItemPtr& rhs )
        {
            return *ptr == *(rhs.ptr);
        }
    };

	std::deque< ItemPtr<ITEM *> >	                m_queue;

	static BlockingSingletonQueue<ITEM>*            m_singleton;
	static int                                      m_count;
	static TA_Base_Core::NonReEntrantThreadLockable m_queueLock;
	static TA_Base_Core::Semaphore*                 m_semaphore;
    unsigned long                                   m_maxQueueSize;
    bool                                            m_canDeleteItem;

};

template<typename T> BlockingSingletonQueue<T>* BlockingSingletonQueue<T>::m_singleton = NULL;
template<typename T> int BlockingSingletonQueue<T>::m_count = 0;
template<typename T> TA_Base_Core::NonReEntrantThreadLockable BlockingSingletonQueue<T>::m_queueLock;
template<typename T> TA_Base_Core::Semaphore* BlockingSingletonQueue<T>::m_semaphore = NULL;

#endif
