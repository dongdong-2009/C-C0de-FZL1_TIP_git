/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/MMSRuleHelper.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Provides an interface to Alarm Rules of type MMS rule in the database.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/alarm_rule/src/MMSRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/MMSRuleData.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    MMSRuleHelper::MMSRuleHelper(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger)
        :
        AlarmRuleHelper(
            entityTypeKey,
            entityKey,
            alarmTypeKey,
            MMSRuleData::RULE_TYPE,
            ruleTrigger)
    {
    }


    MMSRuleHelper::MMSRuleHelper()
        :
        AlarmRuleHelper(
            MMSRuleData::RULE_TYPE,
            IAlarmRuleData::RULE_TRIGGER_SUBMIT)
    {
    }


    MMSRuleHelper::MMSRuleHelper(unsigned long pkey)
        :
        AlarmRuleHelper(pkey)
    {
    }


    MMSRuleHelper::MMSRuleHelper(
        MMSRuleHelper& original)
        :
        AlarmRuleHelper(
            original.getEntityTypeKey(),
            original.getEntityKey(),
            original.getAlarmType(),
            original.getRuleType(),
            original.getRuleTrigger())
    {
    }


    MMSRuleHelper::~MMSRuleHelper()
    {
    }



    void MMSRuleHelper::setRuleTrigger(const std::string& trigger)
    {
        TA_ASSERT(trigger == IAlarmRuleData::RULE_TRIGGER_SUBMIT,
                  "MMS jobs can only be initiated when an alarm is submitted");
    }



}// TA_Base_Core
