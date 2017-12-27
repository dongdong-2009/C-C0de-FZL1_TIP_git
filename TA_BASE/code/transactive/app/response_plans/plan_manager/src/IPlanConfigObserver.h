//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/IPlanConfigObserver.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_IPLANCONFIGOBSERVER_H__B345D221_022E_4F86_9F7A_2508ACAB3415__INCLUDED_)
#define AFX_IPLANCONFIGOBSERVER_H__B345D221_022E_4F86_9F7A_2508ACAB3415__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // IPlanConfigObserver Interface

    class IPlanConfigObserver
    {
    public:
        virtual void planConfigChanged(const TA_Base_Core::PlanConfigUpdateData &planConfigData) = 0;
        virtual void planTreeNodeDeleted(const TA_Base_Core::PlanConfigUpdateData &planConfigData) = 0;

        virtual ~IPlanConfigObserver() {};
    };

} // namespace TA_Base_App

#endif // !defined(AFX_IPLANCONFIGOBSERVER_H__B345D221_022E_4F86_9F7A_2508ACAB3415__INCLUDED_)
