/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/IPlanDssDpRule.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IPlanDssDpRule is an interface to a PlanDssDpRule object.  It allows the PlanDssDpRule object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#if !defined(IPlanDssDpRule_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPlanDssDpRule_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IPlanDssDpRule : public virtual TA_Base_Core::IItem
    {
    public:
        virtual ~IPlanDssDpRule() {};

        virtual unsigned long getKey() = 0;
        virtual unsigned long getLocationKey() = 0;
        virtual unsigned long getTriggerEntityKey() = 0;
        virtual unsigned long getActivePlanKey() = 0;
        virtual unsigned long getMinimumInterval() = 0;
        virtual unsigned long getTolerantTimeForActive() = 0;
        virtual unsigned long getValidStartTimeInDay() = 0;
        virtual unsigned long getValidEndTimeInDay() = 0;
        virtual bool getValidSunday() = 0;
        virtual bool getValidMonday() = 0;
        virtual bool getValidTuesday() = 0;
        virtual bool getValidWednesday() = 0;
        virtual bool getValidThursday() = 0;
        virtual bool getValidFriday() = 0;
        virtual bool getValidSaturday() = 0;
        virtual bool getActivateValue() = 0;
        virtual bool getNeedDeactivate() = 0;
        virtual bool getAllowMultiple() = 0;
        virtual bool getEnable() = 0;
        virtual int getDenyMultipleStrategy() = 0;
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPlanDssDpRule*> IPlanDssDpRules;
    typedef IPlanDssDpRules::iterator    IPlanDssDpRulesIt;

} //close namespace TA_Base_Core


#endif // !defined(IPlanDssDpRule_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
