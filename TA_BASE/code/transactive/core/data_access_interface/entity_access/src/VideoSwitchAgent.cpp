/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoSwitchAgent.cpp $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/05/14 23:14:46 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * VideoSwitchAgent provides access to VideoSwitchAgent entity data and a specific framework
  * for VideoSwitchAgent entity parameters.
  *
  */

#pragma warning( disable: 4786 )

#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
        const std::string VideoSwitchAgent::ENTITY_TYPE = "VideoSwitchAgent";
        const std::string VideoSwitchAgent::SWITCH_CONNECT_TIMEOUT = "SwitchConnectTimeout";
        const std::string VideoSwitchAgent::MAX_SWITCH_RESPONSE_TIME = "MaxSwitchResponseTime";
        const std::string VideoSwitchAgent::SWITCH_POLL_INTERVAL = "SwitchPollInterval";
        const std::string VideoSwitchAgent::LOW_PRIORITY_STATE_VALIDATION_CYCLE = "LowPriorityStateValidationCycle";
        const std::string VideoSwitchAgent::HIGH_PRIORITY_STATE_VALIDATION_CYCLE = "HighPriorityStateValidationCycle";
		const std::string VideoSwitchAgent::ASSET_NAME = "AssetName";

		const std::string VideoSwitchAgent::CCTV_SERVER_IP_ADDRESS = "CCTVServerIPAddress";
		const std::string VideoSwitchAgent::CCTV_SERVER_PORT = "CCTVServerPort";
		const std::string VideoSwitchAgent::CCTV_SERVER_USER_NAME = "CCTVServerUserName";
		const std::string VideoSwitchAgent::CCTV_SERVER_PASSWORD = "CCTVServerPassword";

    //
    // VideoSwitchAgent
    //
    VideoSwitchAgent::VideoSwitchAgent()
        : EntityData()
    {
        // Do nothing.
    }


    //
    // VideoSwitchAgent
    //
    VideoSwitchAgent::VideoSwitchAgent( unsigned long key ) 
        : EntityData( key, getStaticType() )
    {
    }


    //
    // getSwitchConnectTimeout
    //
    unsigned long VideoSwitchAgent::getSwitchConnectTimeout()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(SWITCH_CONNECT_TIMEOUT);
		unsigned long switchConnectTimeout = 0;
		if (!value.empty())
		{
            switchConnectTimeout = EntityHelper::getUnsignedLongData(value);
		}
        return switchConnectTimeout;
    }

    
    //
    // getMaxSwitchResponseTime
    //
    unsigned long VideoSwitchAgent::getMaxSwitchResponseTime()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(MAX_SWITCH_RESPONSE_TIME);
		unsigned long maxSwitchResponseTime = 0;
		if (!value.empty())
		{
            maxSwitchResponseTime = EntityHelper::getUnsignedLongData(value);
		}
        return maxSwitchResponseTime;
    }


    //
    // getActiveSwitchPollInterval
    //
    unsigned long VideoSwitchAgent::getSwitchPollInterval()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(SWITCH_POLL_INTERVAL);
		unsigned long switchPollInterval = 0;
		if (!value.empty())
		{
            switchPollInterval = EntityHelper::getUnsignedLongData(value);
		}
        return switchPollInterval;
    }


    //
    // getLowPriorityStateValidationCycle
    //
    unsigned long VideoSwitchAgent::getLowPriorityStateValidationCycle()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(LOW_PRIORITY_STATE_VALIDATION_CYCLE);
		unsigned long lowPriorityStateValidationCycle = 0;
		if (!value.empty())
		{
            lowPriorityStateValidationCycle = EntityHelper::getUnsignedLongData(value);
		}
        return lowPriorityStateValidationCycle;
    }


    //
    // getHighPriorityStateValidationCycle
    //
    unsigned long VideoSwitchAgent::getHighPriorityStateValidationCycle()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(HIGH_PRIORITY_STATE_VALIDATION_CYCLE);
		unsigned long highPriorityStateValidationCycle = 0;
		if (!value.empty())
		{
            highPriorityStateValidationCycle = EntityHelper::getUnsignedLongData(value);
		}
        return highPriorityStateValidationCycle;
    }

	std::string VideoSwitchAgent::getAssetName()
	{
		// Get the parameters for this entity
		return getHelper()->getParameter(ASSET_NAME);
	}
    
	std::string VideoSwitchAgent::getCCTVServerIPAddress()
	{
		return getHelper()->getParameter(CCTV_SERVER_IP_ADDRESS);
	}

	unsigned long VideoSwitchAgent::getCCTVServerPort()
	{
		std::string value = getHelper()->getParameter(CCTV_SERVER_PORT);
		unsigned long port = 0;
		if (!value.empty())
		{
            port = EntityHelper::getUnsignedLongData(value);
		}
        return port;
	}

	std::string VideoSwitchAgent::getCCTVServerUserName()
	{
		return getHelper()->getParameter(CCTV_SERVER_USER_NAME);
	}

	std::string VideoSwitchAgent::getCCTVServerPassword()
	{
		return getHelper()->getParameter(CCTV_SERVER_PASSWORD);
	}
    ///////////////////////////////////////////////////////////////////////
    //
    // IEntityData Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // getStaticType
    //
    std::string VideoSwitchAgent::getStaticType()
    {
        return ENTITY_TYPE;      // This should be the only place this is defined.
    }


    //
    // getType
    //
    std::string VideoSwitchAgent::getType()
    {
        return getStaticType();
    }

   
    //
    // clone
    //
    IEntityData* VideoSwitchAgent::clone( unsigned long key )
    {
        return new VideoSwitchAgent( key );
    }
    
    
    //
    // invalidate
    //
    void VideoSwitchAgent::invalidate()
    {
        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        getHelper()->invalidate();
    }

} // TA_Base_Core
