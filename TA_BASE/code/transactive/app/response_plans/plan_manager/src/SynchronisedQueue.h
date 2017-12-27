/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/SynchronisedQueue.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Thread-safe queue. Hides iterator access.
  *
  */

#if !defined(AFX_SYNCHRONISEDQUEUE_H__8E9F63F9_5F20_449F_857A_0866F6907453__INCLUDED_)
#define AFX_SYNCHRONISEDQUEUE_H__8E9F63F9_5F20_449F_857A_0866F6907453__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // SynchronisedQueue

    template <typename ItemType>
    class SynchronisedQueue : public TA_Base_Core::NonReEntrantThreadLockable
    {
    // Operations
    public:
        SynchronisedQueue();
         ~SynchronisedQueue();

        bool empty();
        long size();
        void clear();
        const ItemType& front();
        void pop_front();
        void push_back(const ItemType &item);
        void erase(const ItemType &item);
        void getItems(std::vector<ItemType> &items);


    private:
        SynchronisedQueue(const SynchronisedQueue<ItemType> &synchronisedQueue);
        SynchronisedQueue &operator=(const SynchronisedQueue<ItemType> &synchronisedQueue);


    // Attributes
    private:
        std::deque<ItemType> m_queue;
    };

} // namespace TA_Base_App

#include "SynchronisedQueue.inl"

#endif // !defined(AFX_SYNCHRONISEDQUEUE_H__8E9F63F9_5F20_449F_857A_0866F6907453__INCLUDED_)
