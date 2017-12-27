//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/SynchronisedVector.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_SYNCHRONISEDVECTOR_H__A52E46F3_2D9F_4B86_829F_FDC234E963FA__INCLUDED_)
#define AFX_SYNCHRONISEDVECTOR_H__A52E46F3_2D9F_4B86_829F_FDC234E963FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // SynchronisedVector

    template <typename ItemType>
    class SynchronisedVector : public TA_Base_Core::NonReEntrantThreadLockable
    {
    // Operations
    public:
        SynchronisedVector();
         ~SynchronisedVector();

        bool empty();
        long size();
        void clear();
        const ItemType &front();
        const ItemType &back();
        const ItemType &operator[](long position);
        void push_back(const ItemType &item);
        void pop_back();
        void erase(long position);
        void reserve(long capacity);
        void getItems(std::vector<ItemType> &items);


    private:
        SynchronisedVector(const SynchronisedVector<ItemType> &synchronisedVector);
        SynchronisedVector &operator=(const SynchronisedVector<ItemType> &synchronisedVector);


    // Attributes
    private:
        std::vector<ItemType> m_vector;
    };

} // namespace TA_Base_App

#include "SynchronisedVector.inl"

#endif // !defined(AFX_SYNCHRONISEDVECTOR_H__A52E46F3_2D9F_4B86_829F_FDC234E963FA__INCLUDED_)
