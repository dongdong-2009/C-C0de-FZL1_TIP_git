/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnSubsystem.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the subsystem column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSubsystem.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/ISubsystem.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    ColumnSubsystem::ColumnSubsystem(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
    {
    }


    const std::string ColumnSubsystem::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
    {
		m_strSubSystem = "";
		try
		{
			unsigned long ulSubSystem = pAlarmWrap->getItemNumber(ulIndex, SUBSYSTEM_KEY);
			if (ulSubSystem != 0)
			{
				TA_Base_Core::ISubsystem* pSubSystemData = TA_Base_Core::SubsystemAccessFactory::getInstance().getSubsystemByKey(ulSubSystem);

				if (NULL != pSubSystemData)
				{
					m_strSubSystem = pSubSystemData->getName();
				}
			}
		}
		catch(...)
		{ 
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "wrong");			
		}

		return m_strSubSystem;

    }


} //End of TA_Base_Bus

