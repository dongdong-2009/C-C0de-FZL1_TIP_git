/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnTransActiveTimestamp.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the transactive timestamp column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnTransActiveTimestamp.h"
#include "core/utilities/src/TAAssert.h"
#include <sys/timeb.h>
#include "core/utilities/src/DateFormat.h" // TD12474 azenitha

namespace TA_Base_Bus
{
    ColumnTransActiveTimestamp::ColumnTransActiveTimestamp(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
	   //m_formatStr("%d-%b-%y %H:%M:%S") // TD12474
    {
		// TD12474 ++
		TA_Base_Core::DateFormat dateFormat; 
		m_formatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4).c_str(); 
		// ++ TD12474
    }


    CString ColumnTransActiveTimestamp::getItemText(TA_Base_Core::AlarmDetailCorbaDef* alarm, DatabaseCache& databaseCache)
    {
        timeb transactiveTime;
        transactiveTime.time = alarm->transactiveTime.time;
        transactiveTime.millitm = alarm->transactiveTime.millitm;
        if (transactiveTime.time > 0)
        {
            CString value;
            CString time(COleDateTime(transactiveTime.time).Format(m_formatStr.operator LPCTSTR()));
            time += ".%03d";
       
            value.Format(time,transactiveTime.millitm);

            return value;
        }
        else
        {
            return "";
        }
    }

    /**
     * setTimeFormat
     *
     * @param const CString& - The format string as per the strftime command
     */
    void ColumnTransActiveTimestamp::setTimeFormat(const CString& formatStr)
	{
		TA_ASSERT( !formatStr.IsEmpty(), "Received empty time format string." );
		m_formatStr = formatStr;
	}


} //End of TA_Base_Bus

