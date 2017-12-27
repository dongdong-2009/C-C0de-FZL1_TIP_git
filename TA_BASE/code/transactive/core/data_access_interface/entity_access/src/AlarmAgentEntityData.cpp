/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmAgentEntityData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/AlarmAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"

namespace TA_Base_Core
{
	
    const std::string AlarmAgentEntityData::ENTITY_TYPE = "AlarmAgent";
    const std::string AlarmAgentEntityData::PRINTER_NAME = "PrinterName";
    const std::string AlarmAgentEntityData::PRINTER_HOST = "PrinterHost";
    const std::string AlarmAgentEntityData::PRINTER_PORT = "PrinterPort";
    const std::string AlarmAgentEntityData::NUMBER_OF_ALARMS_TO_TEST = "NumberOfAlarmsToTest";
    const std::string AlarmAgentEntityData::USER_ID = "UserId";
    const std::string AlarmAgentEntityData::PROFILE_ID = "ProfileId";
    const std::string AlarmAgentEntityData::WORK_STATION_ID = "WorkStationId";
    const std::string AlarmAgentEntityData::PASSWORD = "Password";
    const std::string AlarmAgentEntityData::IS_PRINTING_ENABLED = "IsPrintingEnabled";
    const std::string AlarmAgentEntityData::IS_FEDERATION_ENABLED = "IsFederationEnabled";
    const std::string AlarmAgentEntityData::SUPPRESSION_RULE_LIFETIME = "SuppressionRuleLifetime";
	const std::string AlarmAgentEntityData::ASSET_NAME = "AssetName";
	//TD16491++
	const std::string AlarmAgentEntityData::MMS_SUBMITTER_TYPE = "MmsSubmitterType";
	const std::string AlarmAgentEntityData::USE_SOURCE_TIME = "UseSourceTime";
	//++TD16491

    AlarmAgentEntityData::AlarmAgentEntityData()
    {}

    AlarmAgentEntityData::AlarmAgentEntityData( unsigned long key ) :
		EntityData( key, getStaticType() )
    {
    }

    void AlarmAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

    AlarmAgentEntityData::~AlarmAgentEntityData()
    {
    }

    bool AlarmAgentEntityData::isFederationEnabled()
    {
		bool federationEnabled = false;
		try
		{
			std::string value = getHelper()->getParameter(IS_FEDERATION_ENABLED);
			if (!value.empty())
			{
				federationEnabled = EntityHelper::getBooleanData(value);
			}
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

        return federationEnabled;
    }

    bool AlarmAgentEntityData::isPrintingEnabled()
    {
		bool printingEnabled = false;
		try
		{
			std::string value = getHelper()->getParameter(IS_PRINTING_ENABLED);
			if (!value.empty())
			{
				printingEnabled = EntityHelper::getBooleanData(value);
			}
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

        return printingEnabled;
    }

    std::string AlarmAgentEntityData::getType()
    {
        return getStaticType();
    }

    std::string AlarmAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    std::string AlarmAgentEntityData::getPrinterName()
    {
		std::string printerName("");
		try
		{
			return getHelper()->getParameter(PRINTER_NAME);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
		return printerName;
    }

    std::string AlarmAgentEntityData::getPrinterHost()
    {
		std::string printerHost("");
		try
		{
			return getHelper()->getParameter(PRINTER_HOST);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
		return printerHost;
    }

    std::string AlarmAgentEntityData::getPrinterPort()
    {
		std::string printerPort("");
		try
		{
			return getHelper()->getParameter(PRINTER_PORT);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
		return printerPort;

    }

    unsigned int AlarmAgentEntityData::getNumberOfAlarmsToTest()
    {
		unsigned int numberOfAlarmsToTest = 10;
		try
		{
			std::string value = getHelper()->getParameter(NUMBER_OF_ALARMS_TO_TEST);
			numberOfAlarmsToTest = EntityHelper::getUnsignedLongData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
        return numberOfAlarmsToTest;
    }

    unsigned int AlarmAgentEntityData::getSuppressionRuleLifetime()
    {
		unsigned int suppressionRuleLifetime = 600;
		try
		{
			std::string value = getHelper()->getParameter(SUPPRESSION_RULE_LIFETIME);
			suppressionRuleLifetime = EntityHelper::getUnsignedLongData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

        return suppressionRuleLifetime;
    }

    unsigned long AlarmAgentEntityData::getUserId()
    {
		unsigned long userId = 0;
		try
		{
			std::string value = getHelper()->getParameter(USER_ID);
			userId = EntityHelper::getUnsignedLongData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
		
        return userId;
    }

    unsigned long AlarmAgentEntityData::getProfileId()
    {
		unsigned long profileId = 0;
		try
		{
			std::string value = getHelper()->getParameter(PROFILE_ID);
			profileId = EntityHelper::getUnsignedLongData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

        return profileId;
    }

    unsigned long AlarmAgentEntityData::getWorkStationId()
    {
		unsigned long workStationId = 0;
		try
		{
			std::string value = getHelper()->getParameter(WORK_STATION_ID);
			workStationId = EntityHelper::getUnsignedLongData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}


        return workStationId;
    }

    std::string AlarmAgentEntityData::getPassword()
    {
		std::string password("");
		try
		{
			return getHelper()->getParameter(PASSWORD);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return password;
    }
	
	std::string AlarmAgentEntityData::getAssetName()
	{
		std::string assetName("");

		try
		{
			return getHelper()->getParameter(ASSET_NAME);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return assetName;

	}

	//TD16491++
	std::string AlarmAgentEntityData::getMmsSubmitterType()
	{
		std::string mmsSubmitterType("");
		try
		{
			return getHelper()->getParameter(MMS_SUBMITTER_TYPE);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}
		
		return mmsSubmitterType;
	}

	bool AlarmAgentEntityData::getUseSourceTime()
	{
		bool bUseSourcetime = false;
		try
		{
			std::string value = getHelper()->getParameter(USE_SOURCE_TIME);
			bUseSourcetime = EntityHelper::getBooleanData(value);
		}
		catch(DataException&)
		{
			// If this (optional) parameter isn't set, a DataException is thrown and
			// default value will be used
		}

		return bUseSourcetime;
	}

	//TD16491++
    TA_Base_Core::IEntityData* AlarmAgentEntityData::clone(unsigned long key)
    {
        return( new AlarmAgentEntityData(key) );
    }

}
