/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/SuppressionRuleData.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * SuppressionRuleData is an implementation of AlarmRuleData.
  */

#ifndef SUPPRESSION_RULE_DATA_H
#define SUPPRESSION_RULE_DATA_H

#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleHelper.h"

namespace TA_Base_Core
{
    class SuppressionRuleData : public AlarmRuleData
    {

    public:

        // The string representing this rule type
        static const std::string RULE_TYPE;

        /**
         * Destructor
         */
        virtual ~SuppressionRuleData();


        /**
         * Constructor
         *
         * Construct an Alarm Rule class based around a key, this will read
		 * the data from the database and throw a DatabaseException
		 * if not successful
         *
         * @param suppressionRuleHelper The ptr of this suppressionRuleHelper
         */
        SuppressionRuleData( SuppressionRuleHelper* suppressionRuleHelper );

        /**
         * Constructor
         *
         * Construct an Alarm Rule class based around a key, this will read
		 * the data from the database and throw a DatabaseException
		 * if not successful
         *
         * @param key The key of this Alarm Rule in the database
         */
        SuppressionRuleData( const unsigned long key );

        
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
        SuppressionRuleData(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger);


        /** 
         * Constructor
         *
         * Construct a new alarm rule.
         *
         * @param entityTypeKey         The key of the entity's type.
         * @param entityKey             The entity's key.
         * @param alarmType             The key of the alarm type.
         * @param ruleTrigger           The trigger for this alarm rule.
         * @param createdBySessionKey  The key of the session under which the rule was created.
         *
         */
        SuppressionRuleData(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
            const std::string& createdBySessionKey);


		SuppressionRuleData(
			unsigned long entityTypeKey,
			unsigned long entityKey,
			unsigned long alarmTypeKey,
			const std::string& ruleTrigger,
			const std::string& createdBySessionKey,
			const std::string& assetName,
			const std::string& description,
			const std::string& value);

        /** 
          * SuppressionRuleData
          *
          * Creates a copy of the given Alarm Rule
          *
          * @param theRule  the rule to copy
          */
        SuppressionRuleData(
            const SuppressionRuleData& theRule);


        /** 
          * SuppressionRuleData
          *
          * Creates a new suppression rule with blank fields.
          *
          */
        SuppressionRuleData();

        /**
         *  Prepends the RULE_TYPE to the getName method of the parent class.
         */
        virtual std::string getName();

		// Optional variables for the suppression of system alarms
		// The variables will be empty string if not provided
		// All GET functions should check for empty string before using
		const std::string getValue();
		const std::string getAssetName();
		const std::string getOperatorName();

		
		void setOperatorName(std::string name);
		const unsigned long getProfileKey();
		void setProfileKey(unsigned long key);

		const int getRemovable() { return m_removable; };

		void setValue(std::string value);
		void setAssetName(std::string assetName);
		void setRemovable(int isRemovable) { m_removable = isRemovable; };

private:
        // Unused methods
		SuppressionRuleData& operator=(const SuppressionRuleData&);
		
		SuppressionRuleHelper* m_suppressionRuleHelper;
		int m_removable;
    };
} // closes TA_Base_Core

#endif // SUPPRESSION_RULE_DATA_H
