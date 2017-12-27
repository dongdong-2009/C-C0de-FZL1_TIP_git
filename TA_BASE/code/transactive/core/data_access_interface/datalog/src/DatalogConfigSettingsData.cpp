/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogConfigSettingsData.cpp $
 * @author:  
 * @version: $Revision:  $
 *
 * Last modification: $DateTime: $
 * Last modified by:  $Author: $
 *
 * IAlarmData is an interface to the Alarm table. It provides both read and write access
 * for agents and user applicaitons.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "DatalogConfigSettingsData.h"


namespace TA_Base_Core
{
	unsigned long DatalogConfigSettingsData::getPkey()
	{
		return m_pkey;
	}  

	unsigned long DatalogConfigSettingsData::getLocationKey()
	{
		return m_locationKey;
	}

	ConfigType DatalogConfigSettingsData::getConfigType()
	{
		return m_configType;
	}

	std::string DatalogConfigSettingsData::getDPTableName()
	{
		return m_dpTableName;
	}

	std::string DatalogConfigSettingsData::getDPGrpTableName()
	{
		return m_dpGrpTableName;
	}

	std::string DatalogConfigSettingsData::getDPLogTableName()
	{
		return m_dpLogTableName;
	}

	unsigned long DatalogConfigSettingsData::getVersionNum()
	{
		return m_versionNum;
	}
	std::string DatalogConfigSettingsData::getDPViewTableName()
	{
		return m_dpViewTableName;
	}

} // closes TA_Base_Core
