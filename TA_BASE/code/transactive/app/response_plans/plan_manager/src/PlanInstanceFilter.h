//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanInstanceFilter.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Used by active plan instance list to determine which instances to display
// based on the current filtering rules.
//

#if !defined(AFX_PLANINSTANCEFILTER_H__8B5B6E55_9563_46CF_8934_F8684C3565D1__INCLUDED_)
#define AFX_PLANINSTANCEFILTER_H__8B5B6E55_9563_46CF_8934_F8684C3565D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/response_plans/active_plans_display/src/IPlanInstanceFilter.h"


namespace TA_Base_Core
{
    struct ActivePlanId;
}


namespace TA_Base_App
{
    class PlanInstance;


    /////////////////////////////////////////////////////////////////////////////
    // PlanInstanceFilter

    class PlanInstanceFilter : public TA_Base_Bus::IPlanInstanceFilter
    {
    // Construction/destruction
    public:
        PlanInstanceFilter();
        ~PlanInstanceFilter();


    // Operations
    public:
        void setDefaults();
        void setAllOwnersNeeded(bool allOwnersNeeded);
        void setAllLocationsNeeded(bool allLocationsNeeded);

        // IPlanInstaceFilter interface
        bool allOwnersNeeded() const;
        bool allLocationsNeeded() const;
        bool isRelevant(const TA_Base_Core::ActivePlanId& activePlanId);
        bool isRelevant(TA_Base_Core::NodeId plan, const PlanInstanceId& planInstanceId);
        bool isRelevant(const PlanInstanceSharedPtr& planInstance);

    // Attributes
    private:
        bool m_allOwnersNeeded;
        bool m_allLocationsNeeded;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANINSTANCEFILTER_H__8B5B6E55_9563_46CF_8934_F8684C3565D1__INCLUDED_)
