/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/response_plans/plan_manager/src/Attic/SynchronisedQueue.inl,v $
  * @author:  Bart Golab
  * @version: $Revision: 1.1.6.2 $
  *
  * Last modification: $Date: 2005/06/01 08:54:04 $
  * Last modified by:  $Author: bartg $
  *
  * Thread-safe queue. Hides iterator access.
  *
  */

#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_App
{
    template <typename ItemType>
    SynchronisedQueue<ItemType>::SynchronisedQueue()
    {
    }


    template <typename ItemType>
    SynchronisedQueue<ItemType>::~SynchronisedQueue()
    {
    }

 
    template <typename ItemType>
    bool SynchronisedQueue<ItemType>::empty()
    {
        TA_Base_Core::ThreadGuard queueLock(*this);

        return m_queue.empty();
    }


    template <typename ItemType>
    long SynchronisedQueue<ItemType>::size()
    {
        TA_Base_Core::ThreadGuard queueLock(*this);

        return m_queue.size();
    }


    template <typename ItemType>
    void SynchronisedQueue<ItemType>::clear()
    {
        TA_Base_Core::ThreadGuard queueLock(*this);

        m_queue.clear();
    }


    template <typename ItemType>
    const ItemType &SynchronisedQueue<ItemType>::front()
    {
        TA_Base_Core::ThreadGuard queueLock(*this);

        return m_queue.front();
    }


    template <typename ItemType>
    void SynchronisedQueue<ItemType>::pop_front()
    {
        TA_Base_Core::ThreadGuard queueLock(*this);

        m_queue.pop_front();
    }


    template <typename ItemType>
    void SynchronisedQueue<ItemType>::push_back(const ItemType &item)
    {
        TA_Base_Core::ThreadGuard queueLock(*this);

        m_queue.push_back(item);
    }       

    
    template <typename ItemType>
    void SynchronisedQueue<ItemType>::erase(const ItemType &item)
    {
        TA_Base_Core::ThreadGuard queueLock(*this);

        for (std::deque<ItemType>::iterator iter = m_queue.begin(); iter != m_queue.end(); iter++)
        {
            if (*iter == item)
            {
                m_queue.erase(iter);
                return;
            }
        }
    }       


    template <typename ItemType>
    void SynchronisedQueue<ItemType>::getItems(std::vector<ItemType> &items)
    {
        TA_Base_Core::ThreadGuard queueLock(*this);

        items.reserve(m_queue.size());
        items.insert(items.begin(), m_queue.begin(), m_queue.end());
    }

} // namespace TA_Base_App

