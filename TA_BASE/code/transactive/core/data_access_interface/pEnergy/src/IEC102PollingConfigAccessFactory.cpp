#include "IEC102PollingConfigAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "ace/OS.h"
#include <iomanip>
#include <sstream>
#include <time.h>
#include <algorithm>
#include <ctime>

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::IDatabase;

namespace TA_Base_Core
{
	TA_Base_Core::NonReEntrantThreadLockable PollingConfigAccessFactory::m_nonReEntreanLock;
	PollingConfigAccessFactory* PollingConfigAccessFactory::m_self = NULL;

	/* The constroctor is private method */
	PollingConfigAccessFactory::PollingConfigAccessFactory()
	{
	}

	/* if IEC102AccessFactory has not exist, create one to return else return existing one. */
	PollingConfigAccessFactory& PollingConfigAccessFactory::getInstance()
	{
        TA_Base_Core::ThreadGuard guard( m_nonReEntreanLock );
		if (m_self == NULL)
		{
			m_self = new PollingConfigAccessFactory();
		}
		return *m_self;
	}

	std::vector< IEC102PollingConfig* > PollingConfigAccessFactory::getStartTime( unsigned long equipmentKey )
	{
		FUNCTION_ENTRY("getStartTime");
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read); // access database, read rights.
		SQLStatement sql;
		databaseConnection->prepareSQLStatement( sql, PENERGYPOLLINGCONFIG_SELECT_190001, equipmentKey ); // prepare sql statment base on sqlcode.txt
		std::string unitKey = "PENERGYUNIT_KEY"; // query result for equipment key will store in this column name
		std::string startTime = "START_TIME"; // start time query result store in this column name 
		std::string pollingTypeKey = "POLLING_TYPE"; // polling type key result will store in this column name
		std::string pollingTypeName = "NAME"; // polling type name result will store in this column name
		std::vector< std::string > columnNames;
		columnNames.push_back( unitKey );
		columnNames.push_back( startTime );
		columnNames.push_back( pollingTypeKey );
		columnNames.push_back( pollingTypeName );

		std::auto_ptr<IData> data(databaseConnection->executeQuery(sql, columnNames));
		if (data->getNumRows()) // if no data
		{
			std::string reasonMessage = "Could not retrieve IEC102 start time, return 0";
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, reasonMessage.c_str());
			TA_THROW(DataException(reasonMessage.c_str(), DataException::NO_VALUE, ""));
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Get IEC102 start time successful.");
		std::vector< IEC102PollingConfig* > result; // return pointer of data set
		for ( int i = 0; i < data->getNumRows(); i++ )
		{
			IEC102PollingConfig* iec102PollingConfig = new IEC102PollingConfig(); // store data in heap, else the data will be destoryed when exit the function 
			iec102PollingConfig->m_PEnergyUnitKey = data->getUnsignedLongData(i, unitKey);
            iec102PollingConfig->m_startTime = data->getDateData(i, startTime);
			iec102PollingConfig->m_pollingTypeKey = data->getUnsignedLongData(i, pollingTypeKey);
			iec102PollingConfig->m_typeName = data->getStringData(i, pollingTypeName);
			
			result.push_back(iec102PollingConfig);
		}
		return result;
		FUNCTION_EXIT;
	}

	std::vector< IEC102PollingConfig* > PollingConfigAccessFactory::getAllStartTime()
	{
		FUNCTION_ENTRY("getAllStartTime");
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read); // access database, read rights.
		SQLStatement sql;
		databaseConnection->prepareSQLStatement( sql, PENERGYPOLLINGCONFIG_SELECT_190003 ); // prepare sql statment base on sqlcode.txt
		std::string unitKey = "PENERGYUNIT_KEY"; // query result for equipment key will store in this column name
		std::string startTime = "START_TIME"; // start time query result store in this column name 
		std::string pollingTypeKey = "POLLING_TYPE"; // polling type key result will store in this column name
		std::string pollingTypeName = "NAME"; // polling type name result will store in this column name
		std::vector< std::string > columnNames;
		columnNames.push_back( unitKey );
		columnNames.push_back( startTime );
		columnNames.push_back( pollingTypeKey );
		columnNames.push_back( pollingTypeName );

		std::auto_ptr<IData> data(databaseConnection->executeQuery(sql, columnNames));
		if (data->getNumRows())
		{
			std::string reasonMessage = "Could not retrieve IEC102 all start time, return 0";
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, reasonMessage.c_str());
			TA_THROW(DataException(reasonMessage.c_str(), DataException::NO_VALUE, ""));
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Get IEC102 all start time successful.");
		std::vector< IEC102PollingConfig* > result;
		for ( int i = 0; i < data->getNumRows(); i++ )
		{
			IEC102PollingConfig* iec102PollingConfig = new IEC102PollingConfig(); // store data in heap, else the data will be destoryed when exit the function 
			iec102PollingConfig->m_PEnergyUnitKey = data->getUnsignedLongData(i, unitKey);
            iec102PollingConfig->m_startTime = data->getDateData(i, startTime);
			iec102PollingConfig->m_pollingTypeKey = data->getUnsignedLongData(i, pollingTypeKey);
			iec102PollingConfig->m_typeName = data->getStringData(i, pollingTypeName);
			
			result.push_back(iec102PollingConfig);
		}
		return result;
		FUNCTION_EXIT;
	}

	void PollingConfigAccessFactory::setStartTime( unsigned long equipmentKey, unsigned long pollingTypeKey, time_t input_time )
	{
		FUNCTION_ENTRY("setStartTime");

		/* start convert timt_t to string format */
		struct tm* timeinfo;
		timeinfo = localtime(&input_time);
		std::stringstream ss_time;
		ss_time << (timeinfo->tm_year + 1900) << std::setfill('0') << std::setw(2) << (timeinfo->tm_mon + 1) << std::setfill('0') << std::setw(2) << timeinfo->tm_mday << std::setfill('0') << std::setw(2) << timeinfo->tm_hour << std::setfill('0') << std::setw(2) << timeinfo->tm_min << std::setfill('0') << std::setw(2) << timeinfo->tm_sec;
		std::string str_time;
		ss_time >> str_time;
		/* end convert timt_t to string format */
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Convert in put time %s to string format %s", input_time, str_time.c_str());

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write); // access database, write rights
		SQLStatement sql;
		databaseConnection->prepareSQLStatement( sql, PENERGYPOLLINGCONFIG_UPDATE_190002, str_time, equipmentKey, pollingTypeKey ); // prepare sql statment base on sqlcode.txt
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "The update statement will be performed: %s", sql.strCommon.c_str());
		databaseConnection->executeModification(sql); // perform update action in database

		FUNCTION_EXIT;
	}

}	// namespace TA_Base_Core
