//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/IPlanInstanceFilter.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/19 17:43:23 $
// Last modified by:  $Author: CM $
//
// This interface is used to facilitate the filtering of plans in the active plan instance list control.

#if !defined(AFX_IPLANINSTANCEFILTER_H__5A963BAA_A9F3_4420_9E07_EC260C3890C7__INCLUDED_)
#define AFX_IPLANINSTANCEFILTER_H__5A963BAA_A9F3_4420_9E07_EC260C3890C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_Core
{
    struct ActivePlanId;
}

namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // IPlanInstanceFilter Interface

    class IPlanInstanceFilter
    {
    public:
		/** 
		  * isRelevant
		  *
		  * Indicate whether the specified plan instance satisfies the filtering rules based on ownership and location.
		  *
		  */        
		virtual bool isRelevant(const TA_Base_Core::ActivePlanId& activePlanInstance) = 0;

		/** 
		  * allOwnersNeeded
		  *
		  * Indicate whether instances from all operators are included
		  *
		  */        
        virtual bool allOwnersNeeded() const = 0;
     
		/** 
		  * allLocationsNeeded
		  *
		  * Indicate whether the instances from all locations are included
		  *
		  */        
        virtual bool allLocationsNeeded() const = 0;
        
		/** 
		  * Destructor
		  *
		  */
        virtual ~IPlanInstanceFilter() = 0 {};        
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_IPLANINSTANCEFILTER_H__5A963BAA_A9F3_4420_9E07_EC260C3890C7__INCLUDED_)
