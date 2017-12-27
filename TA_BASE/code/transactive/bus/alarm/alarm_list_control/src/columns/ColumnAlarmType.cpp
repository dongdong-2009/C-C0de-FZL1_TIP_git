/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnAlarmType.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the alarm type column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAlarmType.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
namespace TA_Base_Bus
{
    ColumnAlarmType::ColumnAlarmType(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
	{
    }


    const std::string ColumnAlarmType::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
    {
		unsigned long ulMessageTypeKey = 0;

		try
		{
			ulMessageTypeKey = pAlarmWrap->getItemNumber(ulIndex, MESSAGE_TYPE_KEY);

			TA_Base_Core::IAlarmTypeData* alarmTypeData = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey( ulMessageTypeKey, false);
			std::string itemText( alarmTypeData->getName().c_str());
			delete alarmTypeData;
			return itemText;
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "wrong");
		}

		return "";
    }


} //End of TA_Base_Bus

