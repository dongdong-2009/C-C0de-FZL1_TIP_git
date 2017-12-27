/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/SuppressionRuleData.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * SuppressionRuleData is an implementation of ISuppressionRuleData.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"

namespace TA_Base_Core
{
    const std::string SuppressionRuleData::RULE_TYPE = "SuppressionByType";

    SuppressionRuleData::~SuppressionRuleData()
    {
		// No need to deleted the helper as it will be deleted in AlarmRuleData
		/*if(m_suppressionRuleHelper != NULL)
		{
			delete m_suppressionRuleHelper;
			m_suppressionRuleHelper = NULL;
		}*/
    }

    SuppressionRuleData::SuppressionRuleData(SuppressionRuleHelper* suppressionRuleHelper)
        :
        AlarmRuleData( m_suppressionRuleHelper = suppressionRuleHelper )
    {
    }

    SuppressionRuleData::SuppressionRuleData(
        const unsigned long key)
        :
        AlarmRuleData( m_suppressionRuleHelper = new SuppressionRuleHelper(key) )
    {
    }


    SuppressionRuleData::SuppressionRuleData()
        :
        AlarmRuleData(m_suppressionRuleHelper = new SuppressionRuleHelper())
    {
    }


    SuppressionRuleData::SuppressionRuleData(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
        :
        AlarmRuleData( m_suppressionRuleHelper = 
            new SuppressionRuleHelper(
                entityTypeKey, entityKey, alarmTypeKey, ruleTrigger) )
    {
    }


    SuppressionRuleData::SuppressionRuleData(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
        const std::string& createdBySessionKey)
        :
        AlarmRuleData( m_suppressionRuleHelper = 
            new SuppressionRuleHelper(
                entityTypeKey, entityKey, alarmTypeKey, ruleTrigger, createdBySessionKey) )
    {
    }

	SuppressionRuleData::SuppressionRuleData(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
        const std::string& createdBySessionKey,
		const std::string& assetName,
		const std::string& description,
		const std::string& value)
        :
        AlarmRuleData( m_suppressionRuleHelper = 
            new SuppressionRuleHelper(
                entityTypeKey, entityKey, alarmTypeKey, ruleTrigger, description, createdBySessionKey, assetName, value) )
    {
    }

    SuppressionRuleData::SuppressionRuleData(
        const SuppressionRuleData& theRule)
        :
        AlarmRuleData( m_suppressionRuleHelper =
            new SuppressionRuleHelper(
                dynamic_cast<SuppressionRuleHelper&>( theRule.getHelper() )))
    {
    }


    std::string SuppressionRuleData::getName()
    {
        return RULE_TYPE + " " + AlarmRuleData::getName();
    }

	const std::string SuppressionRuleData::getValue()
	{
		return m_suppressionRuleHelper->getValue();
	}

	const std::string SuppressionRuleData::getAssetName()
	{
		return m_suppressionRuleHelper->getAssetName();
	}

	void SuppressionRuleData::setValue(std::string value)
	{
		m_suppressionRuleHelper->setValue(value);
	}

	void SuppressionRuleData::setAssetName(std::string assetName)
	{
		m_suppressionRuleHelper->setAssetName(assetName);
	}

	const std::string SuppressionRuleData::getOperatorName()
	{
		return m_suppressionRuleHelper->getOperatorName();
	}

	void SuppressionRuleData::setOperatorName(std::string name)
	{
	    m_suppressionRuleHelper->setCreatedByOperatorName(name);
	}
	const unsigned long SuppressionRuleData::getProfileKey()
	{
		return m_suppressionRuleHelper->getCreatedByProfileKey();
	}
	 void SuppressionRuleData::setProfileKey(unsigned long key)
	 {
		m_suppressionRuleHelper->setCreatedByProfileKey(key);
	 }

} // closes TA_Base_Core
