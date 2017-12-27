/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/SuppressionByDescriptionRuleData.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * SuppressionByDescriptionRuleData is an implementation of SuppressionRuleData.
  */

#ifndef SUPPRESSION_BY_DESCRIPTION_RULE_DATA_H
#define SUPPRESSION_BY_DESCRIPTION_RULE_DATA_H

#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleHelper.h"

namespace TA_Base_Core
{

	class SuppressionByDescriptionRuleData : public SuppressionRuleData 
	{
		public:

			// The string representing this rule type
			static const std::string RULE_TYPE;

			/** 
			  * SuppressionByDescriptionRuleData
			  *
			  * Creates a new suppression rule with blank fields.
			  *
			  */
			SuppressionByDescriptionRuleData();

			virtual ~SuppressionByDescriptionRuleData();

			/**
			 * Constructor
			 *
			 * Construct an Alarm Rule class based around a key, this will read
			 * the data from the database and throw a DatabaseException
			 * if not successful
			 *
			 * @param key The key of this Alarm Rule in the database
			 */
			SuppressionByDescriptionRuleData( const unsigned long key );

        
			/**
			 * Constructor
			 *
			 * Construct a new alarm rule.
			 *
			 * @param entityTypeKey  The key of the entity's type.
			 * @param entityKey      The entity's key.
			 * @param alarmType      The key of the alarm type.
			 * @param ruleTrigger    The trigger for this alarm rule
			 * @param description	 The exact description for this alarm
			 */
			SuppressionByDescriptionRuleData(
				unsigned long entityTypeKey,
				unsigned long entityKey,
				unsigned long alarmTypeKey,
				const std::string& ruleTrigger,
				const std::string& description,
				const std::string& createdBySessionKey,
				const std::string& assetName,
				const std::string& value);

			/** 
			  * SuppressionRuleData
			  *
			  * Creates a copy of the given Alarm Rule
			  *
			  * @param theRule  the rule to copy
			  */
			SuppressionByDescriptionRuleData(
				const SuppressionByDescriptionRuleData& theRule);


			/**
			 *  Prepends the RULE_TYPE to the getName method of the parent class.
			 */
			virtual std::string getName();
		
		private:

			// Unused methods
			SuppressionByDescriptionRuleData& operator=(const SuppressionByDescriptionRuleData&);
			
			SuppressionRuleHelper* m_suppressionRuleHelper;	
	};

} // end namespace

#endif // !defined(SUPPRESSION_BY_DESCRIPTION_RULE_DATA_H)
