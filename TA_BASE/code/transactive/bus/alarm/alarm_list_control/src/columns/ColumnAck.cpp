/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnAck.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the Acknowledge Column
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAck.h"
#include "bus/alarm/alarm_store_library/src/LogAlarmItem.h"
#include "core/utilities/src/DebugUtil.h"
 

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    const char* ColumnAck::ALARM_ACK = "Yes";
    const char* ColumnAck::ALARM_NOT_ACK = "No";

    ColumnAck::ColumnAck(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
    {
    }

	const std::string ColumnAck::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
	{
		try
		{
			if (strcmp(pAlarmWrap->getItemString(ulIndex, ALARM_ACKNOWLEDGE_BY).c_str(), "") != 0)
			{
				return ALARM_NOT_ACK;
			}
			else
			{
				return ALARM_ACK;
			}
		}
		catch(...)
		{ 
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "wrong");
		}
		return "";
    }


} //End of TA_Base_Bus

