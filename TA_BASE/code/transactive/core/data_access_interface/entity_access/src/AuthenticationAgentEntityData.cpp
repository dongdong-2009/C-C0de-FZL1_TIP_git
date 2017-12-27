/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AuthenticationAgentEntityData.cpp $
  * @author Anita Lee	
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/AuthenticationAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string   AuthenticationAgentEntityData::ENTITY_TYPE = "AuthenticationAgent";
	const std::string   AuthenticationAgentEntityData::CHECK_SESSION_FREQUENCY_NAME = "CheckSessionFrequency";
	const std::string   AuthenticationAgentEntityData::TIMEOUT_PERIOD_NAME = "TimeOutPeriod";
    const std::string   AuthenticationAgentEntityData::KEEP_SESSION_ALIVE_HEARTBEAT_NAME = "KeepSessionAliveHeartbeat";
	const std::string	AuthenticationAgentEntityData::ASSET_NAME = "AssetName";
	
    AuthenticationAgentEntityData::AuthenticationAgentEntityData(unsigned long key) : EntityData(key,getStaticType()),
		m_isValidData(false),
		m_checkSessionfrequency(30),
        m_timeoutPeriod(90),
        m_keepSessionAliveHeartbeat(10),
		m_assetName("")
    {
    }

    AuthenticationAgentEntityData::~AuthenticationAgentEntityData()
    { // Nothing to clean up
    }

    IEntityData* AuthenticationAgentEntityData::clone(unsigned long key)
    {
        return new AuthenticationAgentEntityData(key);        
    }

    void AuthenticationAgentEntityData::invalidate()
    {
		TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		getHelper()->invalidate();
        m_isValidData = false;
    }

    std::string AuthenticationAgentEntityData::getType()
    {
        return getStaticType();
    }
	
    std::string AuthenticationAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    unsigned long AuthenticationAgentEntityData::getCheckSessionFrequency()
    {
		std::string value = getHelper()->getParameter(CHECK_SESSION_FREQUENCY_NAME);
		m_checkSessionfrequency = EntityHelper::getUnsignedLongData(value);

        return m_checkSessionfrequency;
    }

    unsigned long AuthenticationAgentEntityData::getTimeOutPeriod()
    {
		std::string value = getHelper()->getParameter(TIMEOUT_PERIOD_NAME);
		m_timeoutPeriod = EntityHelper::getUnsignedLongData(value);

        return m_timeoutPeriod;
    }

    unsigned long AuthenticationAgentEntityData::getKeepSessionAliveHeartbeat()
    {
		std::string value = getHelper()->getParameter(KEEP_SESSION_ALIVE_HEARTBEAT_NAME);
		m_keepSessionAliveHeartbeat = EntityHelper::getUnsignedLongData(value);

        return m_keepSessionAliveHeartbeat;
    }

	std::string AuthenticationAgentEntityData::getAssetName()
    {
		m_assetName = getHelper()->getParameter(ASSET_NAME);
		
        return m_assetName;
    }

} //close namespace TA_Base_Core

