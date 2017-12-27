/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.cpp $
  * @author:  Chris DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  *SystemStatusAgentEntityData proves read-only access to the agent's parameters
  *
  */

#include "core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <sstream>

namespace TA_Base_Core
{

    const std::string SystemStatusAgentEntityData::ENTITY_TYPE                      = "SystemStatusAgent";
	const std::string SystemStatusAgentEntityData::PING_RATE_PARAM = "PingRateSeconds";	
	const std::string SystemStatusAgentEntityData::PING_TIMEOUT_PARAM = "PingTimeoutSeconds";
	const std::string SystemStatusAgentEntityData::PING_RETRIES_PARAM = "PingRetries";

	const std::string SystemStatusAgentEntityData::TA_POLL_RATE_PARAM = "TaPollingRate";

	const std::string SystemStatusAgentEntityData::SYS_CON_PORT_PARAM = "SysConPort";
	const std::string SystemStatusAgentEntityData::SYS_CON_TIMEOUT_PARAM = "SysConTimeoutSeconds";

	const std::string SystemStatusAgentEntityData::CONTROL_STATION_PORT_PARAM = "ControlStationPort";
	const std::string SystemStatusAgentEntityData::CONTROL_STATION_TIMEOUT_PARAM = "ControlStationTimeoutSeconds";

	const std::string SystemStatusAgentEntityData::DB_SERVER_TIMEOUT_PARAM = "DbServerTimeoutSeconds";

	const std::string SystemStatusAgentEntityData::ASSET_NAME_PARAM = "AssetName";
    
    const std::string SystemStatusAgentEntityData::IGNORE_MISSING_PING_PARAM = "IgnoreMissingPingPoints";
	

	SystemStatusAgentEntityData::SystemStatusAgentEntityData(unsigned long key)
		:   EntityData(key, getStaticType())
	{
	}

	SystemStatusAgentEntityData::~SystemStatusAgentEntityData()
	{
	}

	std::string SystemStatusAgentEntityData::getType()
	{
		return getStaticType();
	}

	std::string SystemStatusAgentEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}

    IEntityData* SystemStatusAgentEntityData::clone ( unsigned long key )
    {
        return new SystemStatusAgentEntityData( key );
    }

	int SystemStatusAgentEntityData::getIntParamValue(const std::string& paramName)
	{
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(paramName);
		int intValue = 0;
		if (!value.empty())
		{
            intValue = EntityHelper::getIntegerData(value);
		}

        return intValue;
	}


	std::string SystemStatusAgentEntityData::getStringParamValue(const std::string& paramName)
	{
		return getHelper()->getParameter(paramName);
	}

    
    bool SystemStatusAgentEntityData::getBoolParamValue(const std::string& paramName)
	{
		std::string value("");
		value = getHelper()->getParameter(paramName);

        return getHelper()->getBooleanData(value);
	}
    

    void SystemStatusAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //end namespace TA_Base_Core

