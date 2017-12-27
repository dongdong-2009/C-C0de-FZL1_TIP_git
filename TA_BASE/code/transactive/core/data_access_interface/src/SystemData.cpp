/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemData.cpp $
 * @author:  xiangmei.lu
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2011/01/26 18:31:14 $
 * Last modified by:  $Author: huirong.luo $
 *
 * IAlarmData is an interface to the Alarm table. It provides both read and write access
 * for agents and user applicaitons.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "SystemData.h"


namespace TA_Base_Core
{
	unsigned long SystemData::getPkey()
	{
		return m_pkey;
	}  

	std::string SystemData::getName()
	{
		return m_name;
	}

	bool SystemData::getIsPhysical()
	{
		return m_isPhysical;
	}

	bool SystemData::getIsExclusive()
	{
		return m_isExclusive;
	}

	

} // closes TA_Base_Core
