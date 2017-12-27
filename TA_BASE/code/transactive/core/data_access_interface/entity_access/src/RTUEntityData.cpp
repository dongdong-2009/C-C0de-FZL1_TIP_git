/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RTUEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2015/01/23 19:22:26 $
  * Last modified by: $Author: haijun.li $
  * 
  * RTUEntityData implements the interface IRTUEntityData to provide read-only access
  * to RTU entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"

namespace TA_Base_Core
{
    
    const std::string RTUEntityData::ENTITY_TYPE                        = "RTU";
    const std::string RTUEntityData::INSERVICE = "InService";
    const std::string RTUEntityData::SCANTIME = "ScanTimeMSecs";
    const std::string RTUEntityData::STATIONNAME = "StationName";
	const std::string RTUEntityData::RTU_SLAVE_ID = "SlaveID";
	const std::string RTUEntityData::RTU_STATUS_TABLE_START_ADDRESS = "RTUStatusTableStartAddress";
	const std::string RTUEntityData::RTU_STATUS_TABLE_END_ADDRESS = "RTUStatusTableEndAddress";
	const std::string RTUEntityData::RTU_STATUS_OFFSET_ADDRESS = "RTUStatusTableOffsetAddress";
	const std::string RTUEntityData::RTU_STATUS_TABLE_SERVICE_NUMBER = "RTUStatusTableServiceNumber";
	const std::string RTUEntityData::RTU_COMMAND_TABLE_START_ADDRESS = "RTUCommandTableStartAddress";
	const std::string RTUEntityData::RTU_COMMAND_TABLE_END_ADDRESS = "RTUCommandTableEndAddress";
	const std::string RTUEntityData::RTU_COMMAND_TABLE_SERVICE_NUMBER = "RTUCommandTableServiceNumber";
	const std::string RTUEntityData::RTU_POLLING_TABLE_START_ADDRESS = "RTUPollingTableStartAddress";
	const std::string RTUEntityData::RTU_POLLING_TABLE_END_ADDRESS = "RTUPollingTableEndAddress";
	const std::string RTUEntityData::RTU_POLLING_TABLE_SERVICE_NUMBER = "RTUPollingTableServiceNumber";
    const std::string RTUEntityData::TCPNODELAY = "TcpNoDelay";
	const std::string RTUEntityData::COMMS_ERROR_RETRIES = "CommsErrorRetries";
	const std::string RTUEntityData::POLL_TIMEOUT = "PollTimeout";
	const std::string RTUEntityData::COMMAND_TIMEOUT = "CommandTimeout";
	const std::string RTUEntityData::POLL_ALL_DATA_TIME_IN_SEC = "PollAllDataTimeInSec";
	const std::string RTUEntityData::POLL_KWH_DATA_TIME_IN_SEC = "PollKwhDataTimeInSec";
	const std::string RTUEntityData::DATA_LOST_DURABLE_TIME_IN_SEC = "DataLostDurableTimeInSec";
	const std::string RTUEntityData::UPDATE_LOST_PERIOD_IN_SEC = "UpdateLostPeriodInSec";
	const std::string RTUEntityData::RAISE_ALARM_COMM_ERROR = "RaiseAlarmCommError";

	//TD10586
	const std::string RTUEntityData::RTU_FTP_PORT = "RTUFtpPort";
	const std::string RTUEntityData::RTU_FTP_USERNAME = "RTUFtpUsername";
	const std::string RTUEntityData::RTU_FTP_PASSWORD = "RTUFtpPassword";
	const std::string RTUEntityData::RTU_CONFIGFILE_PATH = "RTUConfigFilePath";
	const std::string RTUEntityData::PMS_CONFIGFILE_PATH = "PMSConfigFilePath";
    
	//Using for IEC 104 Protocol 
	const std::string RTUEntityData::RTUSECONDADDRESS = "SecondAddress";
	
	const std::string RTUEntityData::RTU_GROUP_ID = "RTUGroupID";

	RTUEntityData::RTUEntityData()
    {
    }

	
    RTUEntityData::RTUEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }


    RTUEntityData::~RTUEntityData()
    {
    }

          
	std::string RTUEntityData::getType()
	{
		return getStaticType();
	}


    std::string RTUEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    IEntityData* RTUEntityData::clone ( unsigned long key )
    {
        return new RTUEntityData ( key );
    }


    void RTUEntityData::invalidate()
    {
        getHelper()->invalidate();
    }


	//TD10586
	unsigned int RTUEntityData::getRTUFtpPort()
	{
		return getIntegerData( RTU_FTP_PORT );
	}

	std::string RTUEntityData::getRTUFtpUsername()
	{
		return getStringData ( RTU_FTP_USERNAME );
	}


	std::string RTUEntityData::getRTUFtpPassword()
	{
		return getStringData ( RTU_FTP_PASSWORD );
	}


	std::string RTUEntityData::getRTUConfigFilePath()
	{
		return getStringData ( RTU_CONFIGFILE_PATH );
	}


	std::string RTUEntityData::getPMSConfigFilePath()
	{
		return getStringData ( PMS_CONFIGFILE_PATH );
	}


    bool RTUEntityData::getInServiceState()
	{
        return getBooleanData ( INSERVICE );
	}


    int RTUEntityData::getScanTimeMSecs()
	{
		int scantime = getIntegerData ( SCANTIME );

		// if not defined or has been set to 0
		if ( 0 == scantime )
		{
			// default to 250 milli seconds
			scantime = 250;
		}

		return scantime;
	}


	unsigned long RTUEntityData::getRTUStatusTableStartAddress()
	{
		return getUnsignedLongData ( RTU_STATUS_TABLE_START_ADDRESS );
	}


	unsigned long RTUEntityData::getRTUStatusTableEndAddress()
	{
		return getUnsignedLongData ( RTU_STATUS_TABLE_END_ADDRESS );
	}


	unsigned long RTUEntityData::getRTUStatusOffsetAddress()
	{
		return getUnsignedLongData ( RTU_STATUS_OFFSET_ADDRESS );
	}


	const std::string RTUEntityData::getRTUStatusTableServiceNumber()
	{
		return getStringData ( RTU_STATUS_TABLE_SERVICE_NUMBER );
	}


	unsigned long RTUEntityData::getRTUCommandTableStartAddress()
	{
		return getUnsignedLongData ( RTU_COMMAND_TABLE_START_ADDRESS );
	}


	unsigned long RTUEntityData::getRTUCommandTableEndAddress()
	{
		return getUnsignedLongData ( RTU_COMMAND_TABLE_END_ADDRESS );
	}


	const std::string RTUEntityData::getRTUCommandTableServiceNumber()
	{
		return getStringData ( RTU_COMMAND_TABLE_SERVICE_NUMBER );
	}


	unsigned long RTUEntityData::getRTUPollingTableStartAddress()
	{
		return getUnsignedLongData ( RTU_POLLING_TABLE_START_ADDRESS );
	}


	unsigned long RTUEntityData::getRTUPollingTableEndAddress()
	{
		return getUnsignedLongData ( RTU_POLLING_TABLE_END_ADDRESS );
	}


	const std::string RTUEntityData::getRTUPollingTableServiceNumber()
	{
		return getStringData ( RTU_POLLING_TABLE_SERVICE_NUMBER );
	}


	int RTUEntityData::getSlaveID()
	{
		return getIntegerData ( RTU_SLAVE_ID );
	}


    bool RTUEntityData::getTcpNoDelay()
    {
		return getBooleanData ( TCPNODELAY );
    }


	int RTUEntityData::getCommsErrorRetries()
	{
		return getIntegerData ( COMMS_ERROR_RETRIES );
	}

	int RTUEntityData::getPollTimeout()
	{
		return getIntegerData ( POLL_TIMEOUT );
	}

	int RTUEntityData::getCommandTimeout()
	{
		return getIntegerData ( COMMAND_TIMEOUT );
	}

	int RTUEntityData::getPollAllDataTimeInSec()
	{
		return getIntegerData(POLL_ALL_DATA_TIME_IN_SEC);
	}
	

	int RTUEntityData::getPollKwhDataTimeInSec()
	{
		return getIntegerData(POLL_KWH_DATA_TIME_IN_SEC);
	}

	int RTUEntityData::getDataLostDurableTimeInSec()
	{
		return getIntegerData(DATA_LOST_DURABLE_TIME_IN_SEC);
	}

	int RTUEntityData::getUpdateLostPeriodInSec()
	{
		return getIntegerData(UPDATE_LOST_PERIOD_IN_SEC);
	}

	std::string RTUEntityData::getStringData ( const std::string & parameterName )
	{
		return getHelper()->getParameter(parameterName);
	}


	bool RTUEntityData::getBooleanData ( const std::string & parameterName )
	{
		bool data = false;
		
		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getBooleanData(stringValue);
		}

		return data;
	}


	int RTUEntityData::getIntegerData ( const std::string & parameterName )
	{
		int data = 0;
		
		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getIntegerData(stringValue);
		}
		
		return data;
	}


	unsigned long RTUEntityData::getUnsignedLongData ( const std::string & parameterName )
	{
		unsigned long data = 0;

		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getUnsignedLongData(stringValue);
		}
		
		return data;
	}


	double RTUEntityData::getRealData ( const std::string & parameterName )
	{
		double data = 0.0;

		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getRealData(stringValue);
		}
		
		return data;
	}

    bool RTUEntityData::isRaiseAlarmWhenCommError()
	{
		return getBooleanData ( RAISE_ALARM_COMM_ERROR );
	}
	
	//Using for IEC104 Protocol 
	std::string RTUEntityData::getSecondAddress()
	{
		return getStringData ( RTUSECONDADDRESS );
	}

	int RTUEntityData::getGroupID()
	{
		return getIntegerData ( RTU_GROUP_ID ); 
	}

} //close namespace TA_Base_Core
