/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/SingletonQueue.h $
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

#ifndef SINGLETON_QUEUE_H
#define SINGLETON_QUEUE_H

#include <queue>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"


const unsigned long MAX_SINGLETON_QUEUE_SIZE = 1000000;

template<class ITEM> class SingletonQueue
{

public:
		
    

	/**
  	 * SingletonQueue::getInstance()
  	 *
  	 * Returns a pointer to the single instance
	 * of SingeltonQueue, creating if necessary.
	 *
	 * @return 	a pointer to a SingeltonQueue
  	 *
  	 */
	static SingletonQueue<ITEM> * getInstance()
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
				m_singleton = new SingletonQueue<ITEM>();
			}
		}
		++m_count;
		return m_singleton;
	}	

	static void removeInstance( SingletonQueue<ITEM> * removeMe )
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
			}
			removeMe = NULL;
		}
	}


	bool isEmpty() 
    { 
   		//
		// Synch this as the operation is unlikely to be atomic
		//
        TA_THREADGUARD( m_queueLock );
        return m_queue.empty(); 
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

	void insert( ITEM * newItem )
	{
		//
		// Synch this to prevent multiple inserts occurring
		// simultaneously or  insert/getNext's on the same
		// object
		//
		TA_THREADGUARD( m_queueLock );
        if (m_queue.size() == m_maxQueueSize)
        {
            // Reached out maximum capacity so we have to 
            // exercise damage control by dumping the oldest
            // items
            ITEM* oldestItem = m_queue.front();
			m_queue.pop();
            if (m_canDeleteItem)
            {
                delete oldestItem;
            }
        }
		m_queue.push( newItem );
	}
		

	ITEM * getNext() 
	{
		//
		// Synch this to prevent multiple inserts occurring
		// simultaneously or  insert/getNext's on the same
		// object
		//
		TA_THREADGUARD( m_queueLock );

		if ( m_queue.empty() )
        {
            return NULL;
        }
		
		ITEM * nextItem = m_queue.front();
		m_queue.pop();
		return nextItem;
	}		

	
private:

    SingletonQueue<ITEM>() : m_maxQueueSize(MAX_SINGLETON_QUEUE_SIZE) {}

	~SingletonQueue<ITEM>() 
	{
        //
        // Empty the queue
        //
        // Thread guarded in removeInstance
        while( ! m_queue.empty() )
        {
        	ITEM * nextItem = m_queue.front();
            m_queue.pop();
            if (m_canDeleteItem)
            {
                delete nextItem;
            }
        }
	}

	std::queue<ITEM *>	                        m_queue;

	static SingletonQueue<ITEM> *				m_singleton;
	static int									m_count;
	static TA_Base_Core::NonReEntrantThreadLockable	m_queueLock;
    unsigned long                               m_maxQueueSize;
    bool                                        m_canDeleteItem;
};

template<typename T> SingletonQueue<T>* SingletonQueue<T>::m_singleton = NULL;
template<typename T> int SingletonQueue<T>::m_count = 0;
template<typename T> TA_Base_Core::NonReEntrantThreadLockable SingletonQueue<T>::m_queueLock;

#endif
