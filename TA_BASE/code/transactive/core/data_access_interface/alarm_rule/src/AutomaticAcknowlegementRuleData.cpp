/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AutomaticAcknowlegementRuleData.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * AutomaticAcknowledgementRuleData is an implementation of
  * IAutomaticAcknowledgementRuleData.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "core/data_access_interface/alarm_rule/src/AutomaticAcknowledgementRuleData.h"

namespace TA_Base_Core
{
    const std::string AutomaticAcknowledgementRuleData::RULE_TYPE = "AutomaticAcknowledgement";

    
    AutomaticAcknowledgementRuleData::~AutomaticAcknowledgementRuleData()
    {
    }


    AutomaticAcknowledgementRuleData::AutomaticAcknowledgementRuleData()
        :
        AlarmRuleData( new AutomaticAcknowledgementRuleHelper() )
    {
    }


    AutomaticAcknowledgementRuleData::AutomaticAcknowledgementRuleData(
        const unsigned long key)
        :
        AlarmRuleData( new AutomaticAcknowledgementRuleHelper(key) )
    {
    }


    AutomaticAcknowledgementRuleData::AutomaticAcknowledgementRuleData(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
        :
        AlarmRuleData(
            new AutomaticAcknowledgementRuleHelper(
                entityTypeKey, entityKey, alarmTypeKey, ruleTrigger) )
    {
    }


    AutomaticAcknowledgementRuleData::AutomaticAcknowledgementRuleData(
        const AutomaticAcknowledgementRuleData& theRule)
        :
        AlarmRuleData(
            new AutomaticAcknowledgementRuleHelper(
                dynamic_cast<AutomaticAcknowledgementRuleHelper&>( theRule.getHelper() )))
    {
    }


    std::string AutomaticAcknowledgementRuleData::getName()
    {
        return RULE_TYPE + " " + AlarmRuleData::getName();
    }


} // closes TA_Base_Core
