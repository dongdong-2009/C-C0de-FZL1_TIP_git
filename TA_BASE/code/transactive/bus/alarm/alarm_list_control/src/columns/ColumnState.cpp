/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnState.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the State column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnState.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/alarm/alarm_store_library/src/LogAlarmItem.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    const char* ColumnState::ALARM_OPEN = "Open";
    const char* ColumnState::ALARM_CLOSED = "Closed";

    ColumnState::ColumnState(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
    {
    }

	const std::string ColumnState::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
	{
		try
		{
			int nState = pAlarmWrap->getItemNumber(ulIndex, STATE);
// 			if (nState == 0)
// 			{
// 				return "";
// 			}

			if (nState == TA_Base_Core::AlarmOpen)
			{
				return ALARM_OPEN;
			}
			else
			{
				return ALARM_CLOSED;
			}
		}
		catch(...)
		{ 
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "wrong");

		}
		return "";
    }


} //End of TA_Base_Bus

