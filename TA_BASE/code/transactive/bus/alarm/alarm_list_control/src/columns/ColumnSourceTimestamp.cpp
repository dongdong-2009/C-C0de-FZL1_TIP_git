/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnSourceTimestamp.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class provides all the information for the source timestamp column.
 */
#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSourceTimestamp.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "bus/alarm/alarm_common/src/AlarmConstants.h"

#include <sys/timeb.h>
#include "core/utilities/src/DateFormat.h" // TD12474 
#include "core/utilities/src/DebugUtil.h"
#include "bus/alarm/alarm_store_library/src/LogAlarmItem.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_Bus
{
    ColumnSourceTimestamp::ColumnSourceTimestamp(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
	   //m_formatStr("%d-%b-%y %H:%M:%S") // TD12474
    {
		// TD12474 ++
		TA_Base_Core::DateFormat dateFormat; 
		//m_formatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat1b).c_str(); 
		m_formatStr = "%Y/%m/%d %H:%M:%S"; 
		// ++ TD12474
    }

	const char* ColumnSourceTimestamp::timeFromater(unsigned long tm)
	{
		CTime time(tm);
		// liuyang++
// 		CString str = time.Format("%d/%m/%Y-%H:%M:%S");
// 		strcpy(m_timebuf, str.GetBuffer(sizeof(m_timebuf)));
		// use m_formatStr
		CString str = time.Format(m_formatStr.GetBuffer(0));
		strncpy(m_timebuf, str.GetBuffer(0), sizeof(m_timebuf) - 1);
		// ++liuyang
		return m_timebuf;
	}

	const std::string ColumnSourceTimestamp::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
	{
		unsigned long tm = 0;
		unsigned long mi = 0;
		try
		{
			tm = pAlarmWrap->getItemNumber(ulIndex, TRANSACTIVETIME_TIME);
            mi = pAlarmWrap->getItemNumber(ulIndex, TRANSACTIVETIME_MILI);
		}
		catch(...)
		{ 
 			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "wrong");

			return "";
		}

		if (tm == 0 && mi == 0) return "";
		
		return timeFromater(tm);
		
        /*timeb sourceTime;
		
		//make it both compatible to c830 and krtc requirements
		if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_USESOURCETIME.c_str()))
        {
			sourceTime.time = alarm->sourceTime.time;
			sourceTime.millitm = alarm->sourceTime.millitm;
		}
		else
		{
			sourceTime.time = alarm->displayTime.time;
			sourceTime.millitm = alarm->displayTime.millitm;	
		}


        if (sourceTime.time > 0)
        {
            CString time(COleDateTime(sourceTime.time).Format(m_formatStr.operator LPCTSTR()));
            //time += ".%03d";
            CString value;
            if (TA_Base_Core::RunParams::getInstance().isSet(AlarmConstants::RPARAM_MILLISECOND.c_str()))
            {
                time += ".%03d";
			    value.Format(time,sourceTime.millitm);
            }
            else // No milliseconds
            {
			    value.Format(time);
            }			

            return value;
        }
        else
        {
            return "";
        }*/
    }

    /**
     * setTimeFormat
     *
     * @param const CString& - The format string as per the strftime command
     */
    void ColumnSourceTimestamp::setTimeFormat(const CString& formatStr)
	{
		TA_ASSERT( !formatStr.IsEmpty(), "Received empty time format string." );
		m_formatStr = formatStr;
	}


} //End of TA_Base_Bus

