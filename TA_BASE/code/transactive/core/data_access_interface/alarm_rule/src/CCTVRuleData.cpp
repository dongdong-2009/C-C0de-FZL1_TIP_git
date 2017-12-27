/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/CCTVRuleData.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "core/data_access_interface/alarm_rule/src/CCTVRuleData.h"

namespace TA_Base_Core
{
    const std::string CCTVRuleData::RULE_TYPE = "CCTV";

    
    CCTVRuleData::~CCTVRuleData()
    {
    }


    CCTVRuleData::CCTVRuleData(
        const unsigned long key)
        :
        AlarmRuleData( new CCTVRuleHelper(key) )
    {
    }


    CCTVRuleData::CCTVRuleData()
        :
        AlarmRuleData(new CCTVRuleHelper())
    {
    }


    CCTVRuleData::CCTVRuleData(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
        :
        AlarmRuleData(
            new CCTVRuleHelper(
                entityTypeKey, entityKey, alarmTypeKey, ruleTrigger) )
    {
    }


    CCTVRuleData::CCTVRuleData(
        const CCTVRuleData& theRule)
        :
        AlarmRuleData(
            new CCTVRuleHelper(
                dynamic_cast<CCTVRuleHelper&>( theRule.getHelper() )))
    {
    }


    std::string CCTVRuleData::getName()
    {
        return RULE_TYPE + " " + AlarmRuleData::getName();
    }

} // closes TA_Base_Core
