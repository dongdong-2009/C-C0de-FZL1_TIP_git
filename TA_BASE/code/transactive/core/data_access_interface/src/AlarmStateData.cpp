/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmStateData.cpp $
 * @author:  Xiangmei.Lu
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2009/12/29 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * AlarmStateData is an implementation of IAlarmTypeData. It holds the data specific to a Alarm
 * state entry in the database.
 */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "core/data_access_interface/src/AlarmStateData.h"


namespace TA_Base_Core
{


	unsigned long AlarmStateData::getKey()
	{
		return m_key;
	}
	unsigned long AlarmStateData::getTypeKey()
	{
		return m_typeKey;
	}

	std::string AlarmStateData::getName()
	{
		return m_name;
	}

	std::string AlarmStateData::getDisplayName()
	{
		return m_displayName;
	}

} // closes TA_Base_Core

