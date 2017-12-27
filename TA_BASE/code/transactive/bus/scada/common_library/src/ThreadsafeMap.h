/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/ThreadsafeMap.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * The ThreadsafeMap holds shared pointers and ensures all read/write
  * access is safe
  *
  */

#ifndef THREADSAFEMAP_H
#define THREADSAFEMAP_H

#include <map>
#include "boost/shared_ptr.hpp"

#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	template<class Key, class T> class ThreadsafeMap
	{
	public:
		typedef typename std::vector< boost::shared_ptr<T> > MapVector;

	private:

		typedef typename std::map < Key, boost::shared_ptr<T> > InternalMap;
        typedef typename std::map < Key, boost::shared_ptr<T> >::iterator InternalMapIterator;

	public:
    
		ThreadsafeMap<Key,T>() {};

		virtual ~ThreadsafeMap<Key,T>() {};

		boost::shared_ptr<T> insert(Key key, boost::shared_ptr<T> value)
		{
			TA_THREADGUARD(m_mapLock);

			InternalMapIterator itr = m_map.find(key);

			if ( itr == m_map.end())
			{
				m_map[key] = value;

				return value;
			}
            
            return itr->second;
		}

		boost::shared_ptr<T> get(Key key)
		{
			TA_THREADREADGUARD(m_mapLock);

			InternalMapIterator itr = m_map.find(key);

			if ( itr != m_map.end())
			{
				return itr->second;
			}

			return boost::shared_ptr<T>((T*)NULL);
		}

		MapVector getAll()
		{
			MapVector returnVal;

			TA_THREADREADGUARD(m_mapLock);

			for (InternalMapIterator itr = m_map.begin(); itr != m_map.end(); itr++)
			{
				returnVal.push_back(itr->second);
			}

			return returnVal;
		}

		void erase (Key key)
		{
			TA_THREADGUARD(m_mapLock);

			InternalMapIterator itr = m_map.find(key);

			if ( itr != m_map.end())
			{
				m_map.erase(itr);
			}
		}

		void clear()
		{
			TA_THREADGUARD(m_mapLock);
			m_map.clear();
		}
		
	private:

		InternalMap							  m_map;
		TA_Base_Core::ReadWriteThreadLockable m_mapLock;
	};
}

#endif
