/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/SuppressionByDescriptionRuleData.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * SuppressionByDescriptionRuleData is an implementation of ISuppressionRuleData.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "core/data_access_interface/alarm_rule/src/SuppressionByDescriptionRuleData.h"

namespace TA_Base_Core
{
    const std::string SuppressionByDescriptionRuleData::RULE_TYPE = "SuppressionByDescription";

    SuppressionByDescriptionRuleData::~SuppressionByDescriptionRuleData()
    {
		// No need to deleted the helper as it will be deleted in AlarmRuleData
		/*if(m_suppressionRuleHelper != NULL)
		{
			delete m_suppressionRuleHelper;
			m_suppressionRuleHelper = NULL;
		}*/
    }


    SuppressionByDescriptionRuleData::SuppressionByDescriptionRuleData(const unsigned long key) 
		: SuppressionRuleData( m_suppressionRuleHelper = new SuppressionRuleHelper(key) )
    {
		// set the rule type
		m_suppressionRuleHelper->setRuleType(SuppressionByDescriptionRuleData::RULE_TYPE);
    }


    SuppressionByDescriptionRuleData::SuppressionByDescriptionRuleData()
        : SuppressionRuleData( m_suppressionRuleHelper = new SuppressionRuleHelper() )
    {
		// set the rule type
		m_suppressionRuleHelper->setRuleType(SuppressionByDescriptionRuleData::RULE_TYPE);
    }


	SuppressionByDescriptionRuleData::SuppressionByDescriptionRuleData(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
		const std::string& description,
        const std::string& createdBySessionKey,
		const std::string& assetName,
		const std::string& value)
        : SuppressionRuleData( m_suppressionRuleHelper = 
            new SuppressionRuleHelper(
                entityTypeKey, entityKey, alarmTypeKey, ruleTrigger, description, createdBySessionKey, assetName, value) )
    {
		// set the rule type
		m_suppressionRuleHelper->setRuleType(SuppressionByDescriptionRuleData::RULE_TYPE);
    }

    SuppressionByDescriptionRuleData::SuppressionByDescriptionRuleData(
        const SuppressionByDescriptionRuleData& theRule)
        : SuppressionRuleData ( m_suppressionRuleHelper =
            new SuppressionRuleHelper(
                dynamic_cast<SuppressionRuleHelper&>( theRule.getHelper() )))
    {
		// set the rule type
		m_suppressionRuleHelper->setRuleType(SuppressionByDescriptionRuleData::RULE_TYPE);
    }


    std::string SuppressionByDescriptionRuleData::getName()
    {
        return RULE_TYPE + " " + AlarmRuleData::getName();
    }

} // closes TA_Base_Core