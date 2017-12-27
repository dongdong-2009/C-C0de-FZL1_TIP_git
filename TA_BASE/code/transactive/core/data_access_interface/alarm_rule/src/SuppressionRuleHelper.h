/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/SuppressionRuleHelper.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Provides an interface to Alarm Rules of type suppression in the database.
  *
  */

#ifndef SUPPRESSION_RULE_HELPER_H
#define SUPPRESSION_RULE_HELPER_H


#include <string>
#include <vector>

#include "core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h"

namespace TA_Base_Core
{

    class SuppressionRuleHelper : public AlarmRuleHelper
    {

    public:


        /** 
          * SuppressionRuleHelper
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param ruleType       The rule type associated with the alarm rule.
          * @param ruleTrigger    The trigger for this alarm rule.
          *
          */
        SuppressionRuleHelper(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger);

        /** 
          * SuppressionRuleHelper
          *
          * @param entityTypeKey        The key of the entity's type.
          * @param entityKey            The entity's key.
          * @param ruleType             The rule type associated with the alarm rule.
          * @param ruleTrigger          The trigger for this alarm rule.
          * @param createdBySessionKey  The key of the session under which the rule was created.
          *
          */
        SuppressionRuleHelper(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
			const std::string& description,
            const std::string& createdBySessionKey ="",
			const std::string& assetName = "",
			const std::string& value = "");

        /** 
          * SuppressionRuleHelper
          *
          * Creates a new suppression rules with blank fields.
          *
          */
        SuppressionRuleHelper();


        /** 
          * SuppressionRuleHelper
          *
          * Creates a Suppression Alarm Rule from the database.
          *
          * @param pkey  the primary key of the suppression alarm rule.
          */
        SuppressionRuleHelper(unsigned long pkey);


        /** 
          * SuppressionRuleHelper
          *
          * Creates a copy of the given object.
          *
          * @param original  the object that will be copied.
          */
        SuppressionRuleHelper(
            SuppressionRuleHelper& original);



        /** 
          * ~SuppressionRuleHelper
          *
          * Destructor
          *
          */
        virtual ~SuppressionRuleHelper();


        /** 
          * setRuleTrigger
          *
          * This is overridden to assert if anything other than
          * IAlarmRuleData::RULE_TRIGGER_SUBMIT is set.
          *
          * @param trigger  the trigger
          */
        virtual void setRuleTrigger(const std::string& trigger);

        /** 
          * createNewAlarmRule
          *
          * This is overridden to check for repeated suppression by type vs. by description
		  * before allowing new suppression rule being created
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if the pkey cannot be retrieved after creating the
          * Alarm Rule.
          *
          */
        virtual void createNewAlarmRule();

		/**
		  * getValue
		  * getAssetName
		  *
		  * These are optional const string variables of the suppression rule
		  * which will be stored in the alarm rule param table, if present
		  * Empty string will be returned if these are not found in the table
		  *
		  */
		// ALARM DESCRIPTION
		const std::string getDescription();
		void setDescription(std::string description);

		// ALARM VALUE
		const std::string getValue();
		void setValue(std::string value);

		// ALARM ASSET NAME obtained from the entity table
		const std::string getAssetName();
		void setAssetName(std::string assetName);

		// Operator name obtained from the session table with the session key provided
		const std::string getOperatorName();


    private:
        const SuppressionRuleHelper& operator=(const SuppressionRuleHelper&);

		// Optional parameter under suppression rule
		static const std::string VALUE;
		static const std::string ASSETNAME;
		// Optional variables for suppression rule data
		// Empty string will be returned if not used
		std::string m_description;
		std::string m_value;
		std::string m_assetName;
		std::string m_operator;

		// indicate the object has loaded the data from DB
		bool m_loaded;
		// this is to load all existing data from the DB
		void loadData();
		// this is to apply any changes made to the parameters
		void changeParameters();
    };


}// TA_Base_Core

#endif // SUPPRESSION_RULE_HELPER_H
