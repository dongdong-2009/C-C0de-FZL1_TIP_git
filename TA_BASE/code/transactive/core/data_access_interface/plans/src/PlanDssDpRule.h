 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanDssDpRule.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PlanDssDpRule is an implementation of IPlanDssDpRule. It holds the data specific to an PlanDssDpRule entry
  * in the database, and allows read-only access to that data.
  *
  */


#if !defined(PlanDssDpRule_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define PlanDssDpRule_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/plans/src/IPlanDssDpRule.h"

namespace TA_Base_Core
{
    class IData;
    class PlanDssDpRuleHelper;


    class PlanDssDpRule : public IPlanDssDpRule
    {

    public:



        /**
         * PlanDssDpRule (constructor)
         *
         * Construct an PlanDssDpRule class based around a key.
         *
         * @param key The key of this PlanDssDpRule in the database
         */
        PlanDssDpRule(const unsigned long key);

        /**
         * PlanDssDpRule (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PlanDssDpRuleHelper::PlanDssDpRuleHelper(IData*)
         *
         */
        PlanDssDpRule(unsigned long row, IData& data);

        virtual ~PlanDssDpRule();

        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual unsigned long getTriggerEntityKey();
        virtual unsigned long getActivePlanKey();
        virtual unsigned long getMinimumInterval();
        virtual unsigned long getTolerantTimeForActive();
        virtual unsigned long getValidStartTimeInDay();
        virtual unsigned long getValidEndTimeInDay();
        virtual bool getValidSunday();
        virtual bool getValidMonday();
        virtual bool getValidTuesday();
        virtual bool getValidWednesday();
        virtual bool getValidThursday();
        virtual bool getValidFriday();
        virtual bool getValidSaturday();
        virtual bool getActivateValue();
        virtual bool getNeedDeactivate();
        virtual bool getAllowMultiple();
        virtual bool getEnable();
        virtual int getDenyMultipleStrategy();

        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  
         * For the PA DVA Message Version this is 
         * just the key in a string format e.g. "PA Dva Message Version 1".
         *
         * @return The key description for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * invalidate
         *
         * Make the data contained by this PlanDssDpRule as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment PlanDssDpRule not used so it is made private
		PlanDssDpRule& operator=(const PlanDssDpRule &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PlanDssDpRuleHelper
        // has only been written for ConfigLocation objects and will not copy the PlanDssDpRule helper
        // for a read-only object (ie this one).
        PlanDssDpRule( const PlanDssDpRule& thePlanDssDpRule);  

        PlanDssDpRuleHelper* m_PlanDssDpRuleHelper;
    };
} // closes TA_Base_Core

#endif // !defined(PlanDssDpRule_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
