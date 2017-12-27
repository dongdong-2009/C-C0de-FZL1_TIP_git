/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/CommonStructures.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  *
  */


#ifndef COMMON_STRUCTURES_WED_04_02_2004_H
#define COMMON_STRUCTURES_WED_04_02_2004_H

#include <string>
#include <vector>

namespace TA_Base_Bus
{
    struct SAlarmDetails
    {
        std::string uuid;       // Unique identifier for the alarm
        bool isSelected;        // Indicates whether the alarm is selected

        bool operator==(const SAlarmDetails& rhs) const
        {
            if (rhs.uuid == this->uuid)
            {
                return true;
            }
            return false;
        }
    };


    typedef std::vector<SAlarmDetails> AlarmList;    

}
#endif COMMON_STRUCTURES_WED_04_02_2004_H
