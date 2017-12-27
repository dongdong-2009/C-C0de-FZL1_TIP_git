/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/EMSAgentEntityData.cpp $
  * @author Dhanshri
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * EMSAgentEntityData implements the interface IEMSAgentEntityData to provide read-only access
  * to EMSAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/EMSAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{

    const std::string EMSAgentEntityData::ENTITY_TYPE                            = "EMSAgent";
	const std::string EMSAgentEntityData::OVERLAPREQUEST                         = "OverlapModbusRequest";
	const std::string EMSAgentEntityData::INTERVAL_BETWN_MODBUSTRX               = "IntervalBetweenModbusTransaction";
    const std::string EMSAgentEntityData::STATIONNAME                            = "StationName";
	const std::string EMSAgentEntityData::MAX_MODBUS_LEN						 = "MaxModbusLength";
	const std::string EMSAgentEntityData::ASSET_NAME                             = "AssetName";
	const std::string EMSAgentEntityData::STD_MODBUS_LEN_LMT			         = "StandardModbusLengthLimit"; //TD16169
	const std::string EMSAgentEntityData::POLL_TIMEOUT 							 = "PollTimeout";
	
	const std::string EMSAgentEntityData::POLLING_DI_START_ADDRESS = "PollingDiStartAddress";
	const std::string EMSAgentEntityData::POLLING_DI_END_ADDRESS = "PollingDiEndAddress";
	const std::string EMSAgentEntityData::POLLING_DI_TIMEOUT = "PollingDiTimeout";
	const std::string EMSAgentEntityData::POLLING_DI_TYPE = "PollingDiType";
	
	const std::string EMSAgentEntityData::POLLING_AI_START_ADDRESS = "PollingAiStartAddress";
	const std::string EMSAgentEntityData::POLLING_AI_END_ADDRESS = "PollingAiEndAddress";
	const std::string EMSAgentEntityData::POLLING_AI_TIMEOUT = "PollingAiTimeout";
	const std::string EMSAgentEntityData::POLLING_AI_TYPE = "PollingAiType";
	
	const std::string EMSAgentEntityData::POLLING_MI_START_ADDRESS = "PollingMiStartAddress";
	const std::string EMSAgentEntityData::POLLING_MI_END_ADDRESS = "PollingMiEndAddress";
	const std::string EMSAgentEntityData::POLLING_MI_TIMEOUT = "PollingMiTimeout";
	const std::string EMSAgentEntityData::POLLING_MI_TYPE = "PollingMiType";
	
	const std::string EMSAgentEntityData::DP_PROCESSORS_START_ADDRESSES = "DpProcessorsStartAddresses";
	const std::string EMSAgentEntityData::DP_PROCESSORS_END_ADDRESSES = "DpProcessorsEndAddresses";
	const std::string EMSAgentEntityData::DP_PROCESSORS_ANA_DP_UPD_INTERVL = "AnalogueDpUpdateInterval";
	
	EMSAgentEntityData::EMSAgentEntityData()
    {
    }

	
    EMSAgentEntityData::EMSAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }

    EMSAgentEntityData::~EMSAgentEntityData()
    {
    }


    /**
     * getScanTimeMSecs
     *
     * Returns the scan time in msecs for EMSAgent entity
     *
     * @return The integer value presenting the scan time in msecs
     */
    const int EMSAgentEntityData::getIntervalBetweenModbusTransaction()
	{
		int iIntervalBetweenModbusTransaction = 1000;
		std::string value = getHelper()->getParameter(INTERVAL_BETWN_MODBUSTRX);
		if (!value.empty())
		{
			iIntervalBetweenModbusTransaction = EntityHelper::getIntegerData(value);
		}
		return iIntervalBetweenModbusTransaction;
	}


	const bool EMSAgentEntityData::getOverlapModbusRequest()
	{
		bool isOverlapModbusRequest = false;
		std::string value = getHelper()->getParameter(OVERLAPREQUEST);
		if (!value.empty())
		{
			isOverlapModbusRequest = EntityHelper::getBooleanData(value);
		}
		return isOverlapModbusRequest;
	}

	const bool EMSAgentEntityData::getStandardModbusLengthLimit()
	{
		bool isStandardModbusLengthLimit = false;
		std::string value = getHelper()->getParameter(STD_MODBUS_LEN_LMT);
		if (!value.empty())
		{
			isStandardModbusLengthLimit = EntityHelper::getBooleanData(value);
		}
		return isStandardModbusLengthLimit;
	}
	
	const int EMSAgentEntityData::getPollTimeout()
	{
		int iPollTO = 2000;
		std::string value = getHelper()->getParameter(POLL_TIMEOUT);
		if (!value.empty())
		{
			iPollTO  = EntityHelper::getIntegerData(value);
		}
		return iPollTO;
	}
	
    const std::string EMSAgentEntityData::getStationName()
	{
        return getHelper()->getParameter(STATIONNAME);
	}


    const std::string EMSAgentEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}


	const int EMSAgentEntityData::getMaxModbusLength()
	{
		int iMaxModbusLength = 124;
		std::string value = getHelper()->getParameter(MAX_MODBUS_LEN);
		if(!value.empty())
		{
			iMaxModbusLength = EntityHelper::getIntegerData(value);
		}

        return iMaxModbusLength;
	}


	std::string EMSAgentEntityData::getType()
	{
		return getStaticType();
	}

    const std::string EMSAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


	std::string EMSAgentEntityData::getPollingDiStartAddress()
	{
		return getHelper()->getParameter( POLLING_DI_START_ADDRESS );
	}

	std::string EMSAgentEntityData::getPollingAiStartAddress()
	{
		return getHelper()->getParameter( POLLING_AI_START_ADDRESS );
	}
	
	std::string EMSAgentEntityData::getPollingMiStartAddress()
	{
		return getHelper()->getParameter( POLLING_MI_START_ADDRESS );
	}

	std::string EMSAgentEntityData::getPollingDiEndAddress()
	{
		return getHelper()->getParameter( POLLING_DI_END_ADDRESS );
	}

	std::string EMSAgentEntityData::getPollingAiEndAddress()
	{
		return getHelper()->getParameter( POLLING_AI_END_ADDRESS );
	}

	std::string EMSAgentEntityData::getPollingMiEndAddress()
	{
		return getHelper()->getParameter( POLLING_MI_END_ADDRESS );
	}


	int EMSAgentEntityData::getPollingDiTimeOut()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_DI_TIMEOUT);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int EMSAgentEntityData::getPollingAiTimeOut()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_AI_TIMEOUT);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int EMSAgentEntityData::getPollingMiTimeOut()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_MI_TIMEOUT);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int EMSAgentEntityData::getPollingDiType()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_DI_TYPE);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int EMSAgentEntityData::getPollingAiType()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_AI_TYPE);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int EMSAgentEntityData::getPollingMiType()
	{
			int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_MI_TYPE);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}
	
	std::string EMSAgentEntityData::getDPProcessorsStartAddresses()
	{
		return getHelper()->getParameter( DP_PROCESSORS_START_ADDRESSES );			
	}
	
	std::string EMSAgentEntityData::getDPProcessorsEndAddresses()
	{
		return getHelper()->getParameter( DP_PROCESSORS_END_ADDRESSES );			
	}
		
	int EMSAgentEntityData::getAnalogueDpUpdateInterval()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(DP_PROCESSORS_ANA_DP_UPD_INTERVL);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}
	
    IEntityData* EMSAgentEntityData::clone ( unsigned long key )
    {
        return new EMSAgentEntityData ( key );
    }

    void EMSAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
