/**
  *
  * Source:   $File: //depot/CODE_TA_BASE_DDB/transactive/bus/scada/common_library/src/BlockingSingletonMap.h $
  *
  *	used for heartbeat
  *	
  */

#ifndef BLOCKING_SINGLETON_MAP_H
#define BLOCKING_SINGLETON_MAP_H

#include <map>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/utilities/src/DebugUtil.h"


template<class Key, class ITEM> class BlockingSingletonMap
{
private:
	
	typedef typename std::map < Key, ITEM* > InternalMap;
	typedef typename std::map < Key, ITEM* >::iterator InternalMapIterator;

public:

	static BlockingSingletonMap<Key, ITEM> * getInstance()
	{
		if ( m_singleton == NULL )
		{
			//
			// Double checking to prevent multiple threads 
			// creating multiple instances
			//
			TA_THREADGUARD( m_mapLock );
			if ( m_singleton == NULL )
			{
				m_singleton = new BlockingSingletonMap<Key, ITEM>();
				m_semaphore = new TA_Base_Core::Semaphore(0);
			}
		}
		++m_count;
		return m_singleton;
	}	

	static void removeInstance( BlockingSingletonMap<Key, ITEM> * removeMe )
	{
		if ( removeMe == m_singleton )
		{
			if ( --m_count == 0 )
			{
				// 
				// Guard this to prevent multiple threads atempting
				// to delete/create simultaneously
				//
				TA_THREADGUARD( m_mapLock );
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
        TA_THREADGUARD( m_mapLock );
        return m_map.size(); 
    }

    void insert( Key key, ITEM * newItem )
	{
        TA_THREADGUARD( m_mapLock );
		
		InternalMapIterator itr;
		for( itr = m_map.begin(); itr != m_map.end(); ++itr)
		{
			if(itr->first == key)
			{
				ITEM* oldItem = itr->second;
				itr->second = newItem;
				if(m_canDeleteItem)
				{
					delete oldItem;
					oldItem = NULL;
				}
				m_semaphore->post();	
				return;
			}
		}

		m_map[key] = newItem;

        m_semaphore->post();
	}

    ITEM * blockForItem() 
	{
   		m_semaphore->wait();
	
		TA_THREADGUARD( m_mapLock );
		InternalMapIterator itr = m_map.begin();
		
		if(m_map.end() != itr) 
		{
			ITEM* rtnItem = itr->second;
			m_map.erase(itr);
			return rtnItem;
		}

		return NULL;
	}		

	void unBlockQueue()
	{
		m_semaphore->post();
	}

	
private:

	BlockingSingletonMap< Key,  ITEM>()
	: m_canDeleteItem(true)
	{}

	~BlockingSingletonMap< Key, ITEM>() 
	{
        //
        // Empty the map
        //
		TA_THREADGUARD( m_mapLock );

		if(m_canDeleteItem)
		{
			for(InternalMapIterator itr = m_map.begin(); itr != m_map.end(); ++itr)
			{
				if(itr->second)
				{
					delete itr->second;
					itr->second = NULL;
				}
			}
		}

		m_map.clear();
	}

	InternalMap											m_map;

	static BlockingSingletonMap<Key, ITEM>*				m_singleton;
	static int											m_count;
	static TA_Base_Core::NonReEntrantThreadLockable		m_mapLock;
	static TA_Base_Core::Semaphore*						m_semaphore;
    bool												m_canDeleteItem;
	

};

template<typename K, typename I> BlockingSingletonMap<K, I>* BlockingSingletonMap<K, I>::m_singleton = NULL;
template<typename K, typename I> int BlockingSingletonMap<K, I>::m_count = 0;
template<typename K, typename I> TA_Base_Core::NonReEntrantThreadLockable BlockingSingletonMap<K, I>::m_mapLock;
template<typename K, typename I> TA_Base_Core::Semaphore* BlockingSingletonMap<K, I>::m_semaphore = NULL;

#endif
