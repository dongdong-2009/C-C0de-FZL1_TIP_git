/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/ModbusServerPointAccessFactory.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2017/03/30 18:41:26 $
  * Last modified by:  $Author: yong.liu $
  * 
  * 
  *
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ModbusServerPointAccessFactory.h"
#include "core/data_access_interface/src/ModbusServerPoint.h"
#include "core/data_access_interface/src/IModbusServerPoint.h"
#include "core/data_access_interface/src/ModbusServerAnaloguePoint.h"
#include "core/data_access_interface/src/IModbusServerAnaloguePoint.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	void ModbusServerPointAccessFactory::getAllModbusServerPoint(std::vector<IModbusServerPoint*>& ModbusServerPoints)
    {
        FUNCTION_ENTRY("getAllModbusServerPoint");
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, MODBUS_SERVICE_CONFIG_SELECT_001);

		//std::string sql = "";
		// The table name and structure are changed (3/4/16) and the SQL is modified accordingly.
		//sql = "SELECT en.agentkey, p.address, p.entity_key, p.port, p.pkey FROM MODBUS_SERVICE_CONFIG p, entity en WHERE p.entity_key = en.pkey";
		//sql = "select mb.agentkey, m2.address ,mb.entitykey, mb.datalength, m2.port, m2.pkey, mb.name, mb.address entity_address"
		//		" from entity_datalength mb, MODBUS_SERVICE_CONFIG m2 where mb.entitykey = m2.entity_key";
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ModbusServerPoint::ENTITY_AGENTKEY);
		columnNames.push_back(ModbusServerPoint::MODBUS_POINT_ADDRESS);
		columnNames.push_back(ModbusServerPoint::MODBUS_POINT_ENTITY_KEY);
		columnNames.push_back(ModbusServerPoint::MODBUS_POINT_DATALENGTH);
		columnNames.push_back(ModbusServerPoint::MODBUS_POINT_PORT);
		columnNames.push_back(ModbusServerPoint::MODBUS_POINT_PKEY);
		columnNames.push_back(ModbusServerPoint::MODBUS_POINT_NAME);
		//This address is from entity table, required for tii datapoints related to SIG
		columnNames.push_back(ModbusServerPoint::MODBUS_POINT_ENTITY_ADDRESS);

		// Get a connection to the database
		//IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					ModbusServerPoints.push_back( new ModbusServerPoint( i, *data) );
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }

			delete data;
			data = NULL;
        }
        while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
    }

	void ModbusServerPointAccessFactory::getAllSubscribedAgents(std::vector<unsigned long>& vAgentKeys)
    {
        FUNCTION_ENTRY("getAllSubscribedAgents");
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, MODBUS_SERVICE_CONFIG_SELECT_002);

		// The table name and structure are changed (3/4/16) and the SQL is modified accordingly.
		//sql = "SELECT distinct en.agentkey FROM MODBUS_SERVICE_CONFIG p, entity en WHERE p.entity_key = en.pkey";
		
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ModbusServerPoint::ENTITY_AGENTKEY);
		
		IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();

			if (0 == numRows) // No entry found with the specified pkey
			{
				// clean up the pointer
				delete data;
				data = NULL;

				std::ostringstream reasonMessage;
				reasonMessage << "No data found for distinct agentKeys in table MODBUS_SERVICE_CONFIG."; 			
				TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
			}

			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					unsigned long agentKey = data->getUnsignedLongData(i, ModbusServerPoint::ENTITY_AGENTKEY);
					vAgentKeys.push_back( agentKey );
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }

			delete data;
			data = NULL;
        }
        while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
    }

	void ModbusServerPointAccessFactory::getAllAnalogueData(std::vector<IModbusServerAnaloguePoint*>& objModbusServerAnaloguePoint)
    {
        FUNCTION_ENTRY("getAllAnalogueData");
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, MODBUS_SERVICE_CONFIG_SELECT_003);

		//std::string sql = "";
		///*
		//sql = "select ENTITY_KEY,EngineeringLimitHigh,EngineeringLimitLow,RawLimitHigh,RawLimitLow,Precision, "
		//	"((RawLimitHigh-RawLimitLow)/ (EngineeringLimitHigh-EngineeringLimitLow) )ENGGCONSTANT "
		//		" from ANALOGUE_DATAPOINT_CONFIG where ENTITY_KEY in (select entity_key from MODBUS_SERVICE_CONFIG)";
		//*/
		//sql = "select ENTITY_KEY,EngineeringLimitHigh,EngineeringLimitLow,RawLimitHigh,RawLimitLow,Precision"
		//		" from ANALOGUE_DATAPOINT_CONFIG where ENTITY_KEY in (select entity_key from MODBUS_SERVICE_CONFIG)";
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ModbusServerAnaloguePoint::ENTITY_KEY);
		columnNames.push_back(ModbusServerAnaloguePoint::ENGINEERINGLIMITHIGH);
		columnNames.push_back(ModbusServerAnaloguePoint::ENGINEERINGLIMITLOW);
		columnNames.push_back(ModbusServerAnaloguePoint::RAWLIMITHIGH);
		columnNames.push_back(ModbusServerAnaloguePoint::RAWLIMITLOW);
		columnNames.push_back(ModbusServerAnaloguePoint::PRECISION);
		//columnNames.push_back(ModbusServerAnaloguePoint::ENGGCONSTANT);

		IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					objModbusServerAnaloguePoint.push_back( new ModbusServerAnaloguePoint( i, *data) );
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }

			delete data;
			data = NULL;
        }
        while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
    }

} // closes TA_Base_Core
