/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/MMSRuleHelper.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Provides an interface to Alarm Rules of type MMS rule in the database.
  *
  */

#ifndef MMS_RULE_HELPER_H
#define MMS_RULE_HELPER_H


#include <string>
#include <vector>


#include "core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h"

namespace TA_Base_Core
{

    class MMSRuleHelper : public AlarmRuleHelper
    {

    public:


        /** 
          * MMSRuleHelper
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param ruleType       The rule type associated with the alarm rule.
          * @param ruleTrigger    The trigger for this alarm rule.
          *
          */
        MMSRuleHelper(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger);



        /** 
          * SuppressionRuleHelper
          *
          * Creates a new suppression rules with blank fields.
          *
          */
        MMSRuleHelper();


        /** 
          * MMSRuleHelper
          *
          * Creates a MMS Alarm Rule from the database.
          *
          * @param pkey  the primary key of the suppression alarm rule.
          */
        MMSRuleHelper(unsigned long pkey);


        /** 
          * MMSRuleHelper
          *
          * Creates a copy of the given object.
          *
          * @param original  the object that will be copied.
          */
        MMSRuleHelper(
            MMSRuleHelper& original);



        /** 
          * ~MMSRuleHelper
          *
          * Destructor
          *
          */
        virtual ~MMSRuleHelper();


        /** 
          * setRuleTrigger
          *
          * This is overridden to assert if anything other than
          * IAlarmRuleData::RULE_TRIGGER_SUBMIT is set.
          *
          * @param trigger  the trigger
          */
        virtual void setRuleTrigger(const std::string& trigger);


    private:
        const MMSRuleHelper& operator=(const MMSRuleHelper&);
    };


}// TA_Base_Core

#endif // MMS_RULE_HELPER_H
