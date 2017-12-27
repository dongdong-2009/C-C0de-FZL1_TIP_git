/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/NotificationAgentData.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Implementation of configuration data accesor for the Notification Service Agent
 *
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/NotificationAgentData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    const std::string NotificationAgentData::ENTITY_TYPE            = "NotificationServiceAgent";
    const std::string NotificationAgentData::NOTIFY_SERVICE_PATH = "NotifyServicePath";
    const std::string NotificationAgentData::BROADCAST = "Broadcast";
	const std::string NotificationAgentData::ASSET_NAME = "AssetName";

	NotificationAgentData::NotificationAgentData( unsigned long key )
		: EntityData(key, getStaticType())
	{
	}

	NotificationAgentData::~NotificationAgentData()
	{
		// nothing yet 
	}

	void NotificationAgentData::invalidate()
	{
        getHelper()->invalidate();
	}

	std::string NotificationAgentData::getType()
	{
		return getStaticType();
	}

    std::string NotificationAgentData::getStaticType()
    {
        return ENTITY_TYPE;
    }

	IEntityData* NotificationAgentData::clone( unsigned long key )
	{
		return new NotificationAgentData( key );
	}
	
	bool NotificationAgentData::isBroadcast()
	{
        bool broadcast = false;
		std::string value = getHelper()->getParameter(BROADCAST);
		if (!value.empty())
		{
			broadcast = EntityHelper::getBooleanData(value);
		}	

		return broadcast;
	}
	
	std::string NotificationAgentData::getNotifyServicePath()
	{
		return getHelper()->getParameter(NOTIFY_SERVICE_PATH);
	}

	std::string NotificationAgentData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}

}
