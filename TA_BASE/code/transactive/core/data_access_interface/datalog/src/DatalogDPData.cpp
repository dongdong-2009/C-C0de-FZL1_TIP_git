/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogDPData.cpp $
 * @author: 
 * @version: $Revision: $
 *
 * Last modification: $DateTime: $
 * Last modified by:  $Author: $
 *
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif



#include "DatalogDPData.h"


namespace TA_Base_Core
{


	unsigned long DatalogDPData::getDPKey()
	{
		return m_DP_Key;
	}

	unsigned long DatalogDPData::getEntityKey()
	{
		return m_Entity_Key;
	}

	std::string DatalogDPData::getDPName()
	{
		return m_DP_Name;
	}

	bool DatalogDPData::getDPEnabled()
	{
		return m_DP_Enabled;
	}

	unsigned long DatalogDPData::getDPGrpKey()
	{
		return m_DP_GRP_Key;
	}

	std::string DatalogDPData::getDPGrpName()
	{
		return m_DP_GRP_Name;
	}

	std::string   DatalogDPData::getDPGrpDesc()
	{
		return m_DP_GRP_Desc;
	}

	unsigned long DatalogDPData::getSampleInterval()
	{
		return m_Sample_Interval;
	}

	IntervalType DatalogDPData::getIntervalType()
	{
		return m_Interval_Type;
	}

	time_t	DatalogDPData::getStartTime()
	{
		return m_Start_Time;
	}
	bool   DatalogDPData::getDPGrpEnabled()
	{
		return m_DP_GRP_Enabled;
	}

	double DatalogDPData::getDeltaValue()
	{
		return m_Delta_Value;
	}

	unsigned long DatalogDPData::getLocationKey()
	{
		return m_Location_Key;
	}
	

} // closes TA_Base_Core
