//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/response_plans/plan_manager/src/SynchronisedMap.inl,v $
// @author:  Bart Golab
// @version: $Revision: 1.1.2.7.22.1 $
//
// Last modification: $Date: 2004/12/07 07:55:20 $
// Last modified by:  $Author: benr $
//
// <description>

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_App
{
    template <typename KeyType, typename ItemType>
    SynchronisedMap<KeyType, ItemType>::SynchronisedMap()
    {
    }


    template <typename KeyType, typename ItemType>
    SynchronisedMap<KeyType, ItemType>::~SynchronisedMap()
    {
    }

 
    template <typename KeyType, typename ItemType>
    bool SynchronisedMap<KeyType, ItemType>::empty()
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        return m_map.empty();
    }


    template <typename KeyType, typename ItemType>
    long SynchronisedMap<KeyType, ItemType>::size()
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        return m_map.size();
    }

   
    template <typename KeyType, typename ItemType>
    void SynchronisedMap<KeyType, ItemType>::clear()
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        m_map.clear();
    }


    template <typename KeyType, typename ItemType>
    void SynchronisedMap<KeyType, ItemType>::insert(const KeyType key, ItemType item)
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        m_map.insert(std::make_pair(key, item));
    }


    template <typename KeyType, typename ItemType>
    void SynchronisedMap<KeyType, ItemType>::erase(const KeyType &key)
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        m_map.erase(key);
    }        


    template <typename KeyType, typename ItemType>
    bool SynchronisedMap<KeyType, ItemType>::find(const KeyType &key)
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        std::map<KeyType, ItemType>::const_iterator iter = m_map.find(key);

        return (iter != m_map.end());
    }


    template <typename KeyType, typename ItemType>
    ItemType &SynchronisedMap<KeyType, ItemType>::operator[](const KeyType &key)
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        std::map<KeyType, ItemType>::const_iterator iter = m_map.find(key);
        TA_ASSERT(iter != m_map.end(), "No item in map for the given key");

        return (ItemType &) iter->second;
    }


    template <typename KeyType, typename ItemType>
    ItemType &SynchronisedMap<KeyType, ItemType>::front()
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        TA_ASSERT(!m_map.empty(), "Map is empty");
       
        return (ItemType &) m_map.begin()->second;
    }


    template <typename KeyType, typename ItemType>
    void SynchronisedMap<KeyType, ItemType>::getKeys(std::vector<KeyType> &keys)
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        keys.clear();
        keys.reserve(m_map.size());

        for (std::map<KeyType, ItemType>::const_iterator iter = m_map.begin(); iter != m_map.end(); iter++)
        {
            keys.push_back(iter->first);
        }
    }


    template <typename KeyType, typename ItemType>
    void SynchronisedMap<KeyType, ItemType>::getItems(std::vector<ItemType> &items)
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        items.clear();
        items.reserve(m_map.size());

        for (std::map<KeyType, ItemType>::const_iterator iter = m_map.begin(); iter != m_map.end(); iter++)
        {
            items.push_back(iter->second);
        }
    }

    template <typename KeyType, typename ItemType>
    bool TA_Base_App::SynchronisedMap<KeyType, ItemType>::find( const KeyType &refInput, ItemType& refOutput )
    {
        TA_Base_Core::ThreadGuard mapLock(*this);

        std::map<KeyType, ItemType>::const_iterator iter = m_map.find(refInput);

        if (m_map.end() == iter)
        {
            return false;
        }

        refOutput = iter->second;
        return true;
    }

} // namespace TA_Base_App

