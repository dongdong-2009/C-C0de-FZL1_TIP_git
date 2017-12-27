/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/IConfigPlanDssDpRule.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IConfigPlanDssDpRule - This is the interface supported by objects that allow configuration of
  * PlanDssDpRules. It provides methods for retrieving, settings, and writing data for an PlanDssDpRule object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPlanDssDpRule_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPlanDssDpRule_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/plans/src/IPlanDssDpRule.h"

namespace TA_Base_Core
{

    class IConfigPlanDssDpRule : public virtual TA_Base_Core::IConfigItem, public virtual IPlanDssDpRule
    {

    public:
        virtual ~IConfigPlanDssDpRule() {};

        virtual void setLocationKey(unsigned long key) = 0;
        virtual void setTriggerEntityKey(const unsigned long ulTriggerEntityKey) = 0;
        virtual void setActivePlanKey(const unsigned long ulActivePlanKey) = 0;
        virtual void setMinimumInterval(const unsigned long ulMinimumInterval) = 0;
        virtual void setTolerantTimeForActive(const unsigned long ulTolerantTimeForActive) = 0;

        virtual void setValidStartTimeInDay(const unsigned long ulValidStartTimeInDay) = 0;
        virtual void setValidEndTimeInDay(const unsigned long ulValidEndTimeInDay) = 0;

        virtual void setValidSunday(const bool bValidSunday) = 0;
        virtual void setValidMonday(const bool bValidMonday) = 0;
        virtual void setValidTuesday(const bool bValidTuesday) = 0;
        virtual void setValidWednesday(const bool bValidWednesday) = 0;
        virtual void setValidThursday(const bool bValidThursday) = 0;
        virtual void setValidFriday(const bool bValidFriday) = 0;
        virtual void setValidSaturday(const bool bValidSaturday) = 0;

        virtual void setActivateValue(const bool bActivateValue) = 0;
        virtual void setNeedDeactivate(const bool bNeedDeactivate) = 0;
        virtual void setAllowMultiple(const bool bAllowMultiple) = 0;
        virtual void setEnable(const bool bEnable) = 0;
        virtual void setDenyMultipleStrategy(const int nDenyMultipleStrategy) = 0;

        virtual std::string getLocationName() = 0;
        virtual std::string getTriggerEntityName() = 0;
        virtual std::string getActivePlanName() = 0;

        /**
         * getDateCreated
         *
         * Returns the date created for this PlanDssDpRule
         *
         * @return The date created for this PlanDssDpRule as a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual unsigned long getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this PlanDssDpRule.
         *
         * @return The date modified for this PlanDssDpRule as a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual unsigned long getDateModified() =0;

        
        
        /**
         * deleteThisObject
         *
         * Removes this PlanDssDpRule from the database. 
         * The calling application MUST then delete this PlanDssDpRule object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PlanDssDpRule was initially loaded from the database
         *      OR     - writePlanDssDpRuleData() has already been called
         *      This PlanDssDpRule has not been deleted
         */
        virtual void deleteThisObject() = 0;

    };
} //close namespace TA_Base_Core
#endif // !defined(IConfigPlanDssDpRule_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
