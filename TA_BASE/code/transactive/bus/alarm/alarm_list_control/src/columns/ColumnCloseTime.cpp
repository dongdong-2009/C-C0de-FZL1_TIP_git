/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnCloseTime.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the close time column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnCloseTime.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

namespace TA_Base_Bus
{
    ColumnCloseTime::ColumnCloseTime(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
	{
    }


    CString ColumnCloseTime::getItemText(TA_Base_Core::AlarmDetailCorbaDef* alarm, DatabaseCache& databaseCache)
    {
        if (alarm->closeTime == 0)
        {
            return "";
        }
        else
        {
            time_t closeTime = alarm->closeTime;
            if (closeTime > 0)
            {
				// TD12474 ++
                //return COleDateTime(closeTime).Format(DATE_TIME_FORMAT);
				TA_Base_Core::DateFormat dateFormat; 
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 				
				return COleDateTime(closeTime).Format(dateFormatStr.c_str());
				// ++ TD12474 
            }
            else
            {
                return "";
            }
        }
    }


} //End of TA_Base_Bus

