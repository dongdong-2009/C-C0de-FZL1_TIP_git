//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ViewUpdate.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_VIEWUPDATE_H__E94B98A5_33F8_4C5F_B7E0_EF5F50459ABB__INCLUDED_)
#define AFX_VIEWUPDATE_H__E94B98A5_33F8_4C5F_B7E0_EF5F50459ABB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CViewUpdate

    template <typename UpdateDataType>
    class CViewUpdate : public CObject
    {
    // Operations
    public:
        CViewUpdate(UpdateDataType updateData);
         virtual ~CViewUpdate();

        UpdateDataType GetUpdateData() const;

    private:
        CViewUpdate();
        CViewUpdate(const CViewUpdate<UpdateDataType> &viewUpdate);
        CViewUpdate &operator=(const CViewUpdate<UpdateDataType> &viewUpdate);


    // Attributes
    private:
        UpdateDataType m_updateData;
    };

} // namespace TA_Base_App

#include "ViewUpdate.inl"

#endif // !defined(AFX_VIEWUPDATE_H__E94B98A5_33F8_4C5F_B7E0_EF5F50459ABB__INCLUDED_)
