/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VirtualDataPointAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * VirtualDataPointAgentEntityData provide read-only access
  * to VirtualDataPointAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/VirtualDataPointAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{
    const std::string VirtualDataPointAgentEntityData::ENTITY_TYPE  = "VirtualDataPointAgent";
    const std::string VirtualDataPointAgentEntityData::STATIONNAME  = "StationName";
	const std::string VirtualDataPointAgentEntityData::PROXY_DP_HEALTH_MONITORING_PERIOD_SECS = "ProxyDpMonitoringPeriodSecs";
	const std::string VirtualDataPointAgentEntityData::ASSET_NAME = "AssetName";


	VirtualDataPointAgentEntityData::VirtualDataPointAgentEntityData()
    {
    }


    VirtualDataPointAgentEntityData::VirtualDataPointAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }

    VirtualDataPointAgentEntityData::~VirtualDataPointAgentEntityData()
    {
    }


    const std::string VirtualDataPointAgentEntityData::getStationName()
	{
		// Get the parameters for this entity
		return getHelper()->getParameter(STATIONNAME);
	}


	int VirtualDataPointAgentEntityData::getProxyDpHealthMonitoringPeriodSecs()
	{
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(PROXY_DP_HEALTH_MONITORING_PERIOD_SECS);
		int proxyDpMonitoringPeriodSecs = 10; // default value is 10
		if (!value.empty())
		{
            proxyDpMonitoringPeriodSecs = EntityHelper::getIntegerData(value);
		}
        return proxyDpMonitoringPeriodSecs;
	}


    std::string VirtualDataPointAgentEntityData::getAssetName()
	{
		// Get the parameters for this entity
		return getHelper()->getParameter(ASSET_NAME);
	}


	std::string VirtualDataPointAgentEntityData::getType()
	{
		return getStaticType();
	}


    std::string VirtualDataPointAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    IEntityData* VirtualDataPointAgentEntityData::clone ( unsigned long key )
    {
        return new VirtualDataPointAgentEntityData ( key );
    }


    void VirtualDataPointAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
