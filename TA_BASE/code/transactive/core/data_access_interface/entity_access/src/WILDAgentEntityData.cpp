/* WILDAgentEntityData
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/WILDAgentEntityData.cpp $
 * @author:  <Andy Siow>
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Implementation file
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/WILDAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	
    const std::string WILDAgentEntityData::ENTITY_TYPE                      = "WILDAgent";
    const std::string WILDAgentEntityData::CORRELATION_THRESHOLD = "CorrelationThresholdInSecs";
	const std::string WILDAgentEntityData::ATS_WILD_QUEUE_SIZE = "ATSWILDQueueSize";
	const std::string WILDAgentEntityData::WILD_DETAILS_TOKEN = "WILDDetailsToken";
	const std::string WILDAgentEntityData::CORRELATION_THRESHOLD_TIMEOUT = "CorrelationTimeOutSecs";
	const std::string WILDAgentEntityData::ASSET_NAME = "AssetName";

    WILDAgentEntityData::WILDAgentEntityData() 
    {}

    WILDAgentEntityData::WILDAgentEntityData( unsigned long key ) :
		EntityData( key, getStaticType() )
    {
    }

    void WILDAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

    WILDAgentEntityData::~WILDAgentEntityData()
    {
    }

    std::string WILDAgentEntityData::getType()
    {
        return getStaticType();
    }

    std::string WILDAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    unsigned long WILDAgentEntityData::getCorrelationThresholdTime()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(CORRELATION_THRESHOLD);
		unsigned long correlationThresholdSecs = 0;
		if (!value.empty())
		{
            correlationThresholdSecs = EntityHelper::getUnsignedLongData(value);
		}
        return correlationThresholdSecs;
    }


	unsigned int WILDAgentEntityData::getATSWILDQueueSize()
    {
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(ATS_WILD_QUEUE_SIZE);
		unsigned int atsWILDQueueSize = 0;
		if (!value.empty())
		{
            atsWILDQueueSize = static_cast< unsigned int >(EntityHelper::getUnsignedLongData(value));
		}

        return atsWILDQueueSize;
    }


	long WILDAgentEntityData::getCorrelationThresholdTimeoutSecs()
	{
		// Get the parameters for this entity
		std::string value("");
		value = getHelper()->getParameter(CORRELATION_THRESHOLD_TIMEOUT);
		long correlationThresholdTimeoutSecs = 0;
		if (!value.empty())
		{
            correlationThresholdTimeoutSecs = static_cast< long >(EntityHelper::getUnsignedLongData(value));
		}

        return correlationThresholdTimeoutSecs;
	}


	std::string WILDAgentEntityData::getWILDDetailsToken()
	{
		// Get the parameters for this entity
		return getHelper()->getParameter(WILD_DETAILS_TOKEN);
	}


	std::string WILDAgentEntityData::getAssetName()
	{
		// Get the parameters for this entity
		return getHelper()->getParameter(ASSET_NAME);
	}


    TA_Base_Core::IEntityData* WILDAgentEntityData::clone(unsigned long key)
    {
        return( new WILDAgentEntityData(key) );
    }

}

