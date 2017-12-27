/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogRecordingAccessFactory.cpp $
* @author:  
* @version: $Revision:  $
*
* Last modification: $DateTime: $
* Last modified by:  $Author: $
*
* DatalogRecordingAccessFactory is a singleton that is used by the Data Log Agent to
* record the Trending/Report Data into the database.
*/

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "ace/OS.h"
#include <time.h>

#include "DatalogRecordingAccessFactory.h"
#include "DatalogRecordingData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <sstream>

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IDatabase;


namespace TA_Base_Core
{
	DatalogRecordingAccessFactory* DatalogRecordingAccessFactory::m_instance = NULL;
	NonReEntrantThreadLockable DatalogRecordingAccessFactory::m_singletonLock;

	DatalogRecordingAccessFactory& DatalogRecordingAccessFactory::getInstance()
	{
		ThreadGuard guard(m_singletonLock);
		// Create a new instance if it doesn't exist.
		if ( 0 == m_instance )
		{
			m_instance = new DatalogRecordingAccessFactory();
		}

		return *m_instance;
	}


	bool DatalogRecordingAccessFactory::insertRecord(DatalogRecordingData * record, std::string tableName)
	{

		if (record ==NULL) { return true;}

		struct tm createTime_tm, planLogTime_tm;
		char createTime_str[20] = {0}, planLogTime_str[20] = {0};

		char entityKeyString[50] = {0};
		char valueString[70] = {0};
		char locationKeyString[50] = {0};

		sprintf(entityKeyString, "%lu", record->getEntityKey());
		sprintf(valueString, "%f", record->getValue());
		sprintf(locationKeyString, "%lu", record->getLocationKey());

		createTime_tm = record->getCreateTime();
		sprintf(createTime_str, "%02d/%02d/%04d %02d:%02d:%02d", createTime_tm.tm_mday, createTime_tm.tm_mon + 1,
			createTime_tm.tm_year + 1900, createTime_tm.tm_hour, createTime_tm.tm_min, createTime_tm.tm_sec);

		planLogTime_tm = record->getPlanLogTime();
		sprintf(planLogTime_str, "%02d/%02d/%04d %02d:%02d:%02d", planLogTime_tm.tm_mday, planLogTime_tm.tm_mon + 1,
			planLogTime_tm.tm_year + 1900, planLogTime_tm.tm_hour, planLogTime_tm.tm_min, planLogTime_tm.tm_sec);


		/*std::string formatSQL = "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
		formatSQL += getLocalDatabaseName();
		formatSQL += "','GROUP','";
		formatSQL += "insert into " + tableName;
		formatSQL += "( createtime, entity_key, value, location_key, planlogtime) ";
		formatSQL += "Values( ";
		formatSQL += "to_date(''"; 
		formatSQL += createTime_str ;
		formatSQL += "'', ''dd/mm/yyyy hh24:mi:ss''),";
		formatSQL += std::string(entityKeyString) + ",";
		formatSQL += std::string(valueString) + ",";
		formatSQL += std::string(locationKeyString) + ",";
		formatSQL += "to_date(''" ;
		formatSQL += std::string (planLogTime_str) ;
		formatSQL += "'', ''dd/mm/yyyy hh24:mi:ss'')" ;
		formatSQL += " )'); END;";*/

		SQLStatement formatSQL;
		


		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SQL for insert datalog records : %s", formatSQL.c_str());
		try
		{
			IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);
			
			databaseConnection->prepareSQLStatement(formatSQL, DATALOG_CONFIGSETTINGS_INSERT_82005,
			getLocalDatabaseName(), tableName , createTime_str,std::string(entityKeyString),
			std::string(valueString),std::string(locationKeyString),std::string (planLogTime_str));

			databaseConnection->executeModification(formatSQL);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DatalogRecordingAccessFactory::insertRecord failed.");
			return false;
		}

		return true;

	}


	bool DatalogRecordingAccessFactory::insertRecords(std::vector <DatalogRecordingData *>& records, std::string tableName)
	{
		bool res = true;

		for (int i=0; i<records.size(); i++)
		{
			if (insertRecord( records[i], tableName)== false)
			{
				res = false;
			}
		}
		return res;
	}

	std::string DatalogRecordingAccessFactory::getLocalDatabaseName()
	{
		std::string localDatabase;
		if (localDatabase.empty())
		{
			// have to find out what is the local db name
			// this should be the first in the connection list so grab it
			if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
			{
				throw DatabaseException("db-connection-file not set");
			}
			try
			{
				DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				DataConnections connections = connectionStrings.getConnectionList(Event_Ad, Write);
				localDatabase = connections[0].first;  
			}
			catch(...) // most likely a file not found error
			{
				throw DatabaseException("Unable to find database connection infomation");
			}

		}
		return localDatabase;
	}
};
