/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AutomaticAcknowledgementRuleData.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * AutomaticAcknowledgementRuleData is an implementation of
  * IAutomaticAcknowledgementRuleData.
  */

#ifndef AUTOMATIC_ACKNOWLEDGEMENT_RULE_DATA_H
#define AUTOMATIC_ACKNOWLEDGEMENT_RULE_DATA_H

#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AutomaticAcknowledgementRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AutomaticAcknowledgementRuleHelper.h"

namespace TA_Base_Core
{
    class AutomaticAcknowledgementRuleData : public AlarmRuleData
    {

    public:

        // The string representing this rule type
        static const std::string RULE_TYPE;


        /**
         * Destructor
         */
        virtual ~AutomaticAcknowledgementRuleData();



        /** 
          * AutomaticAcknowledgementRuleData
          *
          * Constructs a new Automatic Acknowledgement Rule with blank fields.
          *
          */
        AutomaticAcknowledgementRuleData();


        /**
         * Constructor
         *
         * Construct an Alarm Rule class based around a key, this will read
		 * the data from the database and throw a DatabaseException
		 * if not successful
         *
         * @param key The key of this Alarm Rule in the database
         */
        AutomaticAcknowledgementRuleData( const unsigned long key );

        
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
        AutomaticAcknowledgementRuleData(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger);


        /** 
          * AutomaticAcknowledgementRuleData
          *
          * Creates a copy of the given Alarm Rule
          *
          * @param theRule  the rule to copy
          */
        AutomaticAcknowledgementRuleData(
            const AutomaticAcknowledgementRuleData& theRule);


        /**
         *  Prepends the RULE_TYPE to the getName method of the parent class.
         */
        virtual std::string getName();

	
private:
        // Unused methods
		AutomaticAcknowledgementRuleData& operator=(const AutomaticAcknowledgementRuleData&);

    };

} // closes TA_Base_Core

#endif // AUTOMATIC_ACKNOWLEDGEMENT_RULE_DATA_H
