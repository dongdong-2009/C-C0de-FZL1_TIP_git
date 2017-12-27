/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnLocation.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the location column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnLocation.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    ColumnLocation::ColumnLocation(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
    {
    }


    const std::string ColumnLocation::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
    {
		m_strLocation = "";
		try
		{
			unsigned long ulLocation = pAlarmWrap->getItemNumber(ulIndex, LOCATION_KEY);
			if (ulLocation != 0)
			{
				TA_Base_Core::ILocation* pLocationData = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(ulLocation);
			
				if (NULL != pLocationData)
				{
					m_strLocation = pLocationData->getName();
				}
			}
		}
		catch(...)
		{ 
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "wrong");			
		}

		return m_strLocation;
    }


} //End of TA_Base_Bus

