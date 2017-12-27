//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/IActivePlanObserver.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_IACTIVEPLANOBSERVER_H__78A644EC_1A99_4FE3_9953_442BA878D5FC__INCLUDED_)
#define AFX_IACTIVEPLANOBSERVER_H__78A644EC_1A99_4FE3_9953_442BA878D5FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // IActivePlanObserver Interface

    class IActivePlanObserver
    {
    public:
        virtual void activeStateChanged(const TA_Base_Core::ActivePlanDetail &activePlanDetail) = 0;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_IACTIVEPLANOBSERVER_H__78A644EC_1A99_4FE3_9953_442BA878D5FC__INCLUDED_)
