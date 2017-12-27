/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/CCTVRuleHelper.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Provides an interface to Alarm Rules of type CCTV rule in the database.
  *
  */

#ifndef CCTV_RULE_HELPER_H
#define CCTV_RULE_HELPER_H


#include <string>
#include <vector>


#include "core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h"

namespace TA_Base_Core
{

    class CCTVRuleHelper : public AlarmRuleHelper
    {

    public:


        /** 
          * CCTVRuleHelper
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param ruleType       The rule type associated with the alarm rule.
          * @param ruleTrigger    The trigger for this alarm rule.
          *
          */
        CCTVRuleHelper(
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
        CCTVRuleHelper();


        /** 
          * CCTVRuleHelper
          *
          * Creates a CCTV Alarm Rule from the database.
          *
          * @param pkey  the primary key of the suppression alarm rule.
          */
        CCTVRuleHelper(unsigned long pkey);


        /** 
          * CCTVRuleHelper
          *
          * Creates a copy of the given object.
          *
          * @param original  the object that will be copied.
          */
        CCTVRuleHelper(
            CCTVRuleHelper& original);



        /** 
          * ~CCTVRuleHelper
          *
          * Destructor
          *
          */
        virtual ~CCTVRuleHelper();


    private:
        const CCTVRuleHelper& operator=(const CCTVRuleHelper&);
    };


}// TA_Base_Core

#endif // CCTV_RULE_HELPER_H
