/**
  * The source code in this file is the property of ST Electronics and is not for redistribution
  * in any form.
  *
  *  
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PEnergyUnitEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2015/01/23 19:22:26 $
  * Last modified by: $Author: haijun.li $
  * 
  * PEnergyUnitEntityData provide read-only access to PEnergyUnit entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/PEnergyUnitEntityData.h"

namespace TA_Base_Core
{
    
    const std::string PEnergyUnitEntityData::ENTITY_TYPE = "PEnergyUnit";
    const std::string PEnergyUnitEntityData::PORT = "Port";
    const std::string PEnergyUnitEntityData::REMOTE_ADDRESS = "RemoteAddress";
    const std::string PEnergyUnitEntityData::ENERGY_DATA_START_ADDRESS = "EnergyDataStartAddr";
	const std::string PEnergyUnitEntityData::ENERGY_DATA_END_ADDRESS = "EnergyDataEndAddr";
	const std::string PEnergyUnitEntityData::ENERGY_DATA_POLLING_INSEC = "EnergyDataPollInSec";
	const std::string PEnergyUnitEntityData::ENERGY_DATA_FREQUENCY = "EnergyDataFrequency";
    const std::string PEnergyUnitEntityData::MEASURE_DATA_START_ADDRESS = "MeasureDataStartAddr";
	const std::string PEnergyUnitEntityData::MEASURE_DATA_END_ADDRESS = "MeasureDataEndAddr";
	const std::string PEnergyUnitEntityData::MEASURE_DATA_POLLING_INSEC = "MeasureDataPollInSec";
	const std::string PEnergyUnitEntityData::MEASURE_DATA_FREQUENCY = "MeasureDataFrequency";
	
	const std::string PEnergyUnitEntityData::IEC102_SEND_RETRY_TIMES = "IEC102SendRetryTimes";
	const std::string PEnergyUnitEntityData::IEC102_INVALID_DATA_RETRY_TIMES = "IEC102InvalidDataRetryTimes";
	const std::string PEnergyUnitEntityData::IEC102_MSG_RESPONSE_TIMEOUT_INMS = "IEC102MessageResponseTimeOutInMs";
	

	PEnergyUnitEntityData::PEnergyUnitEntityData()
    {
    }

	
    PEnergyUnitEntityData::PEnergyUnitEntityData ( unsigned long key )
	: EntityData( key, getStaticType() )
    {
    }


    PEnergyUnitEntityData::~PEnergyUnitEntityData()
    {
    }

          
	std::string PEnergyUnitEntityData::getType()
	{
		return getStaticType();
	}


    std::string PEnergyUnitEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    IEntityData* PEnergyUnitEntityData::clone ( unsigned long key )
    {
        return new PEnergyUnitEntityData ( key );
    }


    void PEnergyUnitEntityData::invalidate()
    {
        getHelper()->invalidate();
    }


	int PEnergyUnitEntityData::getPort()
	{
		return getIntegerData( PORT );
	}
	
	int PEnergyUnitEntityData::getRemoteAddress()
	{
		return getIntegerData( REMOTE_ADDRESS );
	}
	
	const std::string PEnergyUnitEntityData::getEnergyDataStartAddr()
	{
		return getStringData ( ENERGY_DATA_START_ADDRESS );
	}
	
	const std::string PEnergyUnitEntityData::getEnergyDataEndAddr()
	{
		return getStringData ( ENERGY_DATA_END_ADDRESS );
	}
	
	int PEnergyUnitEntityData::getEnergyDataPollInSec()
	{
		return getIntegerData( ENERGY_DATA_POLLING_INSEC );
	}
	
	int PEnergyUnitEntityData::getEnergyDataFrequency()
	{
		return getIntegerData( ENERGY_DATA_FREQUENCY );
	}
	
	const std::string PEnergyUnitEntityData::getMeasureDataStartAddr()
	{
		return getStringData ( MEASURE_DATA_START_ADDRESS );
	}
	
	const std::string PEnergyUnitEntityData::getMeasureDataEndAddr()
	{
		return getStringData ( MEASURE_DATA_END_ADDRESS );
	}
	
	int PEnergyUnitEntityData::getMeasureDataPollInSec()
	{
		return getIntegerData( MEASURE_DATA_POLLING_INSEC );
	}
	
	int PEnergyUnitEntityData::getMeasureDataFrequency()
	{
		return getIntegerData( MEASURE_DATA_FREQUENCY );
	}
	
	int PEnergyUnitEntityData::getIEC102SendRetryTimes()
	{
		return getIntegerData( IEC102_SEND_RETRY_TIMES );
	}
	
	int PEnergyUnitEntityData::getIEC102InvalidDataRetryTimes()
	{
		return getIntegerData( IEC102_INVALID_DATA_RETRY_TIMES );
	}

	int PEnergyUnitEntityData::getIEC102MessageResponseTimeOutInMs()	
	{
		return getIntegerData( IEC102_MSG_RESPONSE_TIMEOUT_INMS  );
	}
		
	std::string PEnergyUnitEntityData::getStringData ( const std::string & parameterName )
	{
		return getHelper()->getParameter(parameterName);
	}


	bool PEnergyUnitEntityData::getBooleanData ( const std::string & parameterName )
	{
		bool data = false;
		
		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getBooleanData(stringValue);
		}

		return data;
	}


	int PEnergyUnitEntityData::getIntegerData ( const std::string & parameterName )
	{
		int data = 0;
		
		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getIntegerData(stringValue);
		}
		
		return data;
	}


	unsigned long PEnergyUnitEntityData::getUnsignedLongData ( const std::string & parameterName )
	{
		unsigned long data = 0;

		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getUnsignedLongData(stringValue);
		}
		
		return data;
	}


	double PEnergyUnitEntityData::getRealData ( const std::string & parameterName )
	{
		double data = 0.0;

		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getRealData(stringValue);
		}
		
		return data;
	}

} //close namespace TA_Base_Core
