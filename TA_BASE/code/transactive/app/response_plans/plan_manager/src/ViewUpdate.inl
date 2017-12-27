//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/response_plans/plan_manager/src/ViewUpdate.inl,v $
// @author:  Bart Golab
// @version: $Revision: 1.1.2.5.26.1 $
//
// Last modification: $Date: 2004/12/07 07:55:21 $
// Last modified by:  $Author: benr $
//
// <description>

namespace TA_Base_App
{
    template <typename UpdateDataType>
    CViewUpdate<UpdateDataType>::CViewUpdate(UpdateDataType updateData) : m_updateData(updateData)
    {
    }


    template <typename UpdateDataType>
    CViewUpdate<UpdateDataType>::~CViewUpdate()
    {
    }

 
    template <typename UpdateDataType>
    UpdateDataType CViewUpdate<UpdateDataType>::GetUpdateData() const
    {
        return m_updateData;
    }     

} // namespace TA_Base_App

