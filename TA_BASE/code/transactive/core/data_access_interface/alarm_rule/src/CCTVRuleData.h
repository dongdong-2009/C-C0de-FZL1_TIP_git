/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/CCTVRuleData.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#ifndef CCTV_RULE_DATA_H
#define CCTV_RULE_DATA_H

#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/CCTVRuleHelper.h"

namespace TA_Base_Core
{
    class CCTVRuleData : public AlarmRuleData
    {

    public:

        // The string representing this rule type
        static const std::string RULE_TYPE;


        /**
         * Destructor
         */
        virtual ~CCTVRuleData();


        /**
         * Constructor
         *
         * Construct an Alarm Rule class based around a key, this will read
		 * the data from the database and throw a DatabaseException
		 * if not successful
         *
         * @param key The key of this Alarm Rule in the database
         */
        CCTVRuleData( const unsigned long key );

        
        /**
         * Constructor
         *
         * Construct a new alarm rule.
         *
         * @param entityTypeKey  The key of the entity's type.
         * @param entityKey      The entity's key.
         * @param alarmType      The key of the alarm type.
         * @param ruleTrigger    The trigger for this alarm rule.
         */
        CCTVRuleData(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger);


        /** 
          * CCTVRuleData
          *
          * Creates a copy of the given Alarm Rule
          *
          * @param theRule  the rule to copy
          */
        CCTVRuleData(
            const CCTVRuleData& theRule);


        /** 
          * SuppressionRuleData
          *
          * Creates a new suppression rule with blank fields.
          *
          */
        CCTVRuleData();


        /**
         *  Prepends the RULE_TYPE to the getName method of the parent class.
         */
        virtual std::string getName();


private:
        // Unused methods
		CCTVRuleData& operator=(const CCTVRuleData&);

    };
} // closes TA_Base_Core

#endif // SUPPRESSION_RULE_DATA_H
