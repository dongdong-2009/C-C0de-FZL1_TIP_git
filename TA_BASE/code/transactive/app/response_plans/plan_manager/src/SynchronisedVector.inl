//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/response_plans/plan_manager/src/SynchronisedVector.inl,v $
// @author:  Bart Golab
// @version: $Revision: 1.1.2.3.26.1 $
//
// Last modification: $Date: 2004/12/07 07:55:21 $
// Last modified by:  $Author: benr $
//
// <description>

#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_App
{
    template <typename ItemType>
    SynchronisedVector<ItemType>::SynchronisedVector()
    {
    }


    template <typename ItemType>
    SynchronisedVector<ItemType>::~SynchronisedVector()
    {
    }

 
    template <typename ItemType>
    bool SynchronisedVector<ItemType>::empty()
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        return m_vector.empty();
    }


    template <typename ItemType>
    long SynchronisedVector<ItemType>::size()
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        return m_vector.size();
    }


    template <typename ItemType>
    void SynchronisedVector<ItemType>::clear()
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        m_vector.clear();
    }


    template <typename ItemType>
    const ItemType &SynchronisedVector<ItemType>::front()
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        return m_vector.front();
    }


    template <typename ItemType>
    const ItemType &SynchronisedVector<ItemType>::back()
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        return m_vector.back();
    }


    template <typename ItemType>
    const ItemType &SynchronisedVector<ItemType>::operator[](long position)
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        return m_vector[position];
    }


    template <typename ItemType>
    void SynchronisedVector<ItemType>::push_back(const ItemType &item)
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        m_vector.push_back(item);
    }


    template <typename ItemType>
    void SynchronisedVector<ItemType>::pop_back()
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        m_vector.pop_back();
    }        


    template <typename ItemType>
    void SynchronisedVector<ItemType>::erase(long position)
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        std::vector<ItemType>::iterator iter = m_vector.begin() + position;

        m_vector.erase(iter);
    }        


    template <typename ItemType>
    void SynchronisedVector<ItemType>::reserve(long capacity)
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        m_vector.reserve(capacity);
    }


    template <typename ItemType>
    void SynchronisedVector<ItemType>::getItems(std::vector<ItemType> &items)
    {
        TA_Base_Core::ThreadGuard vectorLock(*this);

        items.reserve(m_vector.size());
        items.insert(items.begin(), m_vector.begin(), m_vector.end());
    }

} // namespace TA_Base_App

