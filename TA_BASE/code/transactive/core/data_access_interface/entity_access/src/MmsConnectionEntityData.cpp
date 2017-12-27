/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MmsConnectionEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of MmsConnectiontEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MmsConnectiontEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string MmsConnectiontEntityData::ENTITY_TYPE             = "MmsConnection";
	const std::string MmsConnectiontEntityData::PING_PERIOD_SEC = "PingPeriodInSeconds";
	const std::string MmsConnectiontEntityData::MSG_CHECK_PERIOD_SEC = "CheckForMessagePeriodInSeconds";
	const std::string MmsConnectiontEntityData::TIMEOUT_TENTH_SEC = "TimeoutInTenthSeconds";
	const std::string MmsConnectiontEntityData::GLOBAL_MMS_INHIBIT = "GlobalMmsInhibit";
	const std::string MmsConnectiontEntityData::QUEUE_OBJECT_NAME = "QueueObjectName";
	const std::string MmsConnectiontEntityData::QUEUE_MANAGER_NAME = "QueueManagerName";

    MmsConnectiontEntityData::MmsConnectiontEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {
    }

    MmsConnectiontEntityData::~MmsConnectiontEntityData()
    { 
    }

    IEntityData* MmsConnectiontEntityData::clone(unsigned long key)
    {
        return new MmsConnectiontEntityData(key);        
    }

    void MmsConnectiontEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    unsigned int MmsConnectiontEntityData::getPingPeriodSeconds()
    {
        unsigned int pingPeriodSeconds = 0;
		std::string value = getHelper()->getParameter(PING_PERIOD_SEC);
		if (!value.empty())
		{
			pingPeriodSeconds = EntityHelper::getIntegerData(value);
		}
        return pingPeriodSeconds;
    }

    unsigned int MmsConnectiontEntityData::getMessageCheckSeconds()
    {
		unsigned int messageCheckPeriodSecs = 0;
		std::string value = getHelper()->getParameter(MSG_CHECK_PERIOD_SEC);
		if (!value.empty())
		{
			messageCheckPeriodSecs = EntityHelper::getIntegerData(value);
		}

        return messageCheckPeriodSecs;
    }

    unsigned int MmsConnectiontEntityData::getTimeoutTenthSecs()
    {
        unsigned int serverTimeoutTenthSecs = 10;

		std::string value = getHelper()->getParameter(TIMEOUT_TENTH_SEC);
		if (!value.empty())
		{
			serverTimeoutTenthSecs = EntityHelper::getIntegerData(value);
		}

        return serverTimeoutTenthSecs;
    }

	bool MmsConnectiontEntityData::getGlobalMmsInhibit()
    {
        bool globalMmsInhibit = false;
		std::string value = getHelper()->getParameter(GLOBAL_MMS_INHIBIT);
		if (!value.empty())
		{
			globalMmsInhibit = EntityHelper::getBooleanData(value);
		}

        return globalMmsInhibit;
    }

    void MmsConnectiontEntityData::setGlobalMmsInhibit(bool applied)
    {
        // Set the value in the database.

        TA_ASSERT( 0 != getHelper(), "The EntityHelper pointer is NULL" );
        std::map< std::string, std::string > paramMap;

		std::string value = "1";
		if(!applied) value = "0";
        paramMap[ GLOBAL_MMS_INHIBIT ] = value;
        getHelper()->writeParameters( paramMap );
    }

    std::string MmsConnectiontEntityData::getQueueObjectName()
    {
		return getHelper()->getParameter(QUEUE_OBJECT_NAME);
    }

    std::string MmsConnectiontEntityData::getQueueManagerName()
    {
		return getHelper()->getParameter(QUEUE_MANAGER_NAME);
    }

    std::string MmsConnectiontEntityData::getType()
    {
        return getStaticType();
    }
	
    std::string MmsConnectiontEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
}
