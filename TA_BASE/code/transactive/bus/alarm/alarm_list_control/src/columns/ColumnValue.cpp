/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnValue.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class provides all the information for the value column.
 */

#include <algorithm>

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnValue.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/message/src/MessageConfig.h"

namespace TA_Base_Bus
{
    // Helper class used to find a matching name
	class HasName  
	{
		public:
			HasName(const std::string& name)
				:m_name(name)
			{
            }

			bool operator () (TA_Base_Core::NameValuePair* nameValue)
			{
				return (nameValue->name == m_name);
			}

		private:
			std::string m_name;
	};

    
    ColumnValue::ColumnValue(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
    {
    }

	const std::string ColumnValue::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
    {
		// The value will be: "AlarmType Value"
		/*
        CString text = alarm->alarmValue;

		//TD 13146 Check for the temperature and '?' and replace '?' with '?
		CString alarmDesc = alarm->alarmDescription;
		alarmDesc.MakeUpper();
		if (text.Find("?C", 0) >= 0 && alarmDesc.Find("TEMPERATURE", 0) >= 0)
		{
			text.Replace(_T('?'), _T('°''));
		}

        return text;
		*/
		try
		{
			return pAlarmWrap->getItemString(ulIndex, ALARM_VALUE).c_str();
		}
		catch(...)
		{ 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "wrong");
			
		}
		return "";		
    }


} //End of TA_Base_Bus

