
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/data_access_interface/src/sqlmacrodef.h"
#include <sstream>

#include "ModbusHelperFactory.h"
using namespace TA_Base_Core;

namespace TA_Base_Bus 
{
	NotFoundFEPConfigException::NotFoundFEPConfigException(const std::string& message) throw()
		:TransactiveException(message)
	{
	}

	NotFoundFEPConfigException::~NotFoundFEPConfigException() throw()
	{
	}

	boost::shared_ptr<IModbusHelper> ModbusHelperFactory::createHelper(std::string primaryHost, std::string primaryPort, std::string secondaryHost, std::string secondaryPort,
		std::string slaveAddress, int retries, unsigned long socketTimeout, bool isSTEModbus)
	{
		boost::shared_ptr<IModbusHelper> helper (new ModbusHelper(primaryHost, primaryPort, secondaryHost, secondaryPort,slaveAddress, retries, socketTimeout, isSTEModbus));

		return helper;
	}

	boost::shared_ptr<IModbusHelper> ModbusHelperFactory::createHelper(unsigned long location,EnumSubSystem subsystem)
	{
		//get primary host, primary port, secondary host, secondary port and slave address;
		std::string primaryHost;
		std::string primaryPort;
		std::string secondaryHost;
		std::string secondaryPort;

		int slaveAddress;
		int retries;
		int startAddress;
		unsigned long socketTimeout = 0;
		bool isSTEModbus;

		if(!getConfig(location,subsystem,primaryHost,primaryPort,secondaryHost,secondaryPort,slaveAddress,startAddress,retries,socketTimeout, isSTEModbus))
		{
			//can't get configuration correctly
			std::stringstream ss;
			ss << "Failed to get the FEP configuration for (location:" <<  location << ", subsystem : "   << subsystem << ") ";
			TA_THROW( NotFoundFEPConfigException(ss.str() ) );
		}

		return boost::shared_ptr<IModbusHelper>(new ModbusHelper(primaryHost, primaryPort, secondaryHost, secondaryPort,slaveAddress,startAddress,retries,socketTimeout, isSTEModbus));
	}


	bool ModbusHelperFactory::getConfig(unsigned long location,EnumSubSystem subsys,std::string& primaryHost, std::string& primaryPort, 
		std::string& secondaryHost, std::string& secondaryPort,int& slaveAddress,int& startAddress, int & retries,unsigned long& socketTimeout, bool& isSTEModbus)
	{
		socketTimeout = 0;
		bool result = false;
		try
		{

			std::string primaryFEPAddressColumn = "PRIMARY_FEP_IP";
			std::string primaryFEPPortColumn = "PRIMARY_FEP_PORT";
			std::string secondaryFEPAddressColumn = "SECONDARY_FEP_IP";
			std::string secondaryFEPPortColumn = "SECONDARY_FEP_PORT";
			std::string slaveAddressColumn = "SLAVEADDRESS";
			std::string retriesColumn = "RETRIES";
			std::string startAddressColumn = "STARTADDRESS";
			std::string socketTimeoutColumn = "SOCKET_TIMEOUT";
			std::string isSTEModbusColumn = "IS_STE_MODBUS";

			std::vector<std::string> columnNames;

			columnNames.push_back(primaryFEPAddressColumn);
			columnNames.push_back(primaryFEPPortColumn);

			columnNames.push_back(secondaryFEPAddressColumn);
			columnNames.push_back(secondaryFEPPortColumn);

			columnNames.push_back(slaveAddressColumn);
			columnNames.push_back(startAddressColumn);

			columnNames.push_back(retriesColumn);
			columnNames.push_back(socketTimeoutColumn);	
			columnNames.push_back(isSTEModbusColumn);
			
			std::string subsystem = "";

			switch(subsys)
			{
			case ModbusHelperFactory::PA:
				subsystem = "PA";
				break;
			case ModbusHelperFactory::CCTV:
				//TODO: change the entitynamepattern for CCTV
				//video switch agent
				subsystem = "CCTV";
				break;
			case ModbusHelperFactory::PIDS:
				//TODO: change the entitynamepattern for PIDS
				//TIS
				subsystem = "PIDS";
				break;
			case ModbusHelperFactory::BAS:
				subsystem = "BAS";
				break;
			case ModbusHelperFactory::FAS:
				subsystem = "FAS";
				break;
			case ModbusHelperFactory::SIGNALLING:
				subsystem = "SIGNALLING";
				break;
			}


			/*std::stringstream sqlStream;
			sqlStream << " select  primary_fep_ip, primary_fep_port, secondary_fep_ip,secondary_Fep_port, slave_address, start_address,retries,socket_timeout,is_ste_modbus "
				<< " from fep_config config"
				<< " where config.location_id = " << location
				<< " and config.subsystem = '" << subsystem << "'";
				//<<" ) ";*/

			TA_Base_Core::IDatabase* databaseConnection = 
				TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
			
			TA_Base_Core::SQLStatement sql;
			databaseConnection->prepareSQLStatement(sql, FEP_CONFIG_SELECT_106001, location, subsystem);
			TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);
			
			int rowNo = data->getNumRows();
			if (data != NULL && rowNo >= 1)
			{
				primaryHost = data->getStringData(0,primaryFEPAddressColumn);
				primaryPort = data->getStringData(0,primaryFEPPortColumn);

				secondaryHost = data->getStringData(0,secondaryFEPAddressColumn);
				secondaryPort = data->getStringData(0,secondaryFEPPortColumn);

				slaveAddress = data->getIntegerData(0,slaveAddressColumn);
				startAddress = data->getIntegerData(0,startAddressColumn); 

				retries = data->getIntegerData(0, retriesColumn,3);
				socketTimeout = data->getUnsignedLongData(0, socketTimeoutColumn, 1000);
				isSTEModbus = data->getBooleanData(0, isSTEModbusColumn, true);  
				
				result = true;
			}

			if (data != NULL)
			{
				delete data;
				data = NULL;
			}
			
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknow Error!" );
		}
		return result;
	}

}
