/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnAckTime.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the Acknowldge time column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAckTime.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

namespace TA_Base_Bus
{
    ColumnAckTime::ColumnAckTime(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
    {
    }


    CString ColumnAckTime::getItemText(TA_Base_Core::AlarmDetailCorbaDef* alarm, DatabaseCache& databaseCache)
    {
        std::string ackedBy = alarm->alarmAcknowledgedBy;
		if(ackedBy.empty())
        {
            return "";
        }
        else
        {
            time_t ackTime = alarm->ackTime;
            if (ackTime  > 0)
            {
				// TD12474 ++
                //return COleDateTime(ackTime).Format(DATE_TIME_FORMAT);
				TA_Base_Core::DateFormat dateFormat;
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 
				return COleDateTime(ackTime).Format(dateFormatStr.c_str());
				// ++ TD12474
            }
            else
            {
                return "";
            }
        }
    }


} //End of TA_Base_Bus

