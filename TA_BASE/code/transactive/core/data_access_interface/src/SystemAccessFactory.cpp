/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/CombinedEventAccessFactory.cpp $
* @author:  Bradley Cleaver
* @version: $Revision: #6 $
*
* Last modification: $DateTime: 2010/06/30 14:51:45 $
* Last modified by:  $Author: lu.xiangmei $
*
* CombinedEventAccessFactory is a singleton that is used by the Event Viewer to
* retrieve all data required to be presented to the user. It allows filtering and
* sorting to done on the database side, and returns a vector of partially loaded
* data to improve the efficiency.
*
*/

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>

#include "SystemAccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <sstream>

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IDatabase;

// Define all constants in an anonymous namespace.
namespace
{

	//columns for System
	const std::string COLUMN_PKEY( "PKEY" );
	const std::string COLUMN_NAME( "NAME" );
	const std::string COLUMN_IS_PHYSICAL( "IS_PHYSICAL" );
	const std::string COLUMN_EXCLUSIVE_CONTROL( "EXCLUSIVE_CONTROL" );
	


};

namespace TA_Base_Core
{
	SystemAccessFactory * SystemAccessFactory::m_instance = NULL;
	NonReEntrantThreadLockable SystemAccessFactory::m_singletonLock;

	SystemAccessFactory& SystemAccessFactory::getInstance()
	{
		ThreadGuard guard(m_singletonLock);
		// Create a new instance if it doesn't exist.
		if ( 0 == m_instance )
		{
			m_instance = new SystemAccessFactory();
		}

		return *m_instance;
	}

	std::vector<SystemData*> SystemAccessFactory::getAllSystems() 
	{
		FUNCTION_ENTRY("getAllSystems");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

// 		std::ostringstream sql;
// 		sql << "SELECT Pkey,name, is_physical,exclusive_control ";
// 		sql << " FROM System ";

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_NAME);
		columnNames.push_back( COLUMN_IS_PHYSICAL );
		columnNames.push_back( COLUMN_EXCLUSIVE_CONTROL);

//		std::string sql  = databaseConnection->prepareSQLStatement(SYSTEM_SELECT_72001);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SYSTEM_SELECT_72001);
//		TA_ASSERT(sql.size() > 0, "getAlarmStateData: GetSQLStatement error");
	    
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading system data: %s", sql.c_str());
	   
		std::string strExSql;
		int nExDbType= databaseConnection->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
			break;		
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading system data: %s", strExSql.c_str());
		
		IData* data = databaseConnection->executeQuery( sql, columnNames ) ;

		std::vector<SystemData*> systems;

		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					systems.push_back( createSystemData( i, data ) );

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
		while ( databaseConnection->moreData(data) );

		FUNCTION_EXIT;
		return systems;

	}

	SystemData*SystemAccessFactory::createSystemData( unsigned long row, IData* data )
	{
		// Get the pkey for the event.
		unsigned long pkey = data->getUnsignedLongData( row, COLUMN_PKEY );
		std::string name = data->getStringData( row, COLUMN_NAME );

		int is_physical = data->getIntegerData( row, COLUMN_IS_PHYSICAL );
		int exclusive_control = data->getIntegerData(row, COLUMN_EXCLUSIVE_CONTROL);

		return new SystemData(pkey,name,(is_physical==1)?true:false, (exclusive_control ==1)?true:false);
	}

};
