/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmStateAccessFactory.cpp $
 * @author:  xiangmei.lu
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2009/12/29 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * AlarmTypeAccessFactory is a singleton that is used to retrieve AlarmTypeData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

#pragma warning(disable:4786)

#include "core/data_access_interface/src/AlarmStateAccessFactory.h"
#include "core/data_access_interface/src/AlarmStateData.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace
{

	//columns for AlarmState:
	const std::string COLUMN_PKEY( "PKEY" );
	const std::string COLUMN_NAME( "NAME" );
	const std::string COLUMN_DISPLAYNAME( "DISPLAYNAME" );
	const std::string COLUMN_TYPEKEY( "TYPEKEY" );

};

namespace TA_Base_Core
{
	AlarmStateAccessFactory* AlarmStateAccessFactory::m_instance = NULL;
	NonReEntrantThreadLockable AlarmStateAccessFactory::m_singletonLock;

	AlarmStateAccessFactory& AlarmStateAccessFactory::getInstance()
	{
		ThreadGuard guard(m_singletonLock);
		// Create a new instance if it doesn't exist.
		if ( 0 == m_instance )
		{
			m_instance = new AlarmStateAccessFactory();
		}

		return *m_instance;
	}
	
	void AlarmStateAccessFactory::freeInstance()
	{
		ThreadGuard guard(m_singletonLock);
		if (0 != m_instance)
		{
			delete m_instance;
			m_instance = 0;
		}
	}

	std::vector<AlarmStateData *> AlarmStateAccessFactory::getAlarmStateData ()
	{
		FUNCTION_ENTRY("getAlarmStateData");

// 		// Get a connection to the database
 		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);
// 
// 		// Check if the alarm rule is actually in the database
// 		std::ostringstream sql;
// 		sql << "SELECT Pkey,Name, DisplayName, TypeKey ";
// 		sql << "FROM AlarmState ";
 

//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMSTATE_SELECT_71001);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMSTATE_SELECT_71001);
//		TA_ASSERT(sql.size() > 0, "getAlarmStateData: GetSQLStatement error");


		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_NAME);
		columnNames.push_back( COLUMN_DISPLAYNAME );
		columnNames.push_back( COLUMN_TYPEKEY);

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading alarm state data: %s", sql.c_str());

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
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading alarm state data: %s", strExSql.c_str());

		
 		IData* data = databaseConnection->executeQuery(sql, columnNames);

		if (data->getNumRows() == 0)
		{
			//TA_THROW(DataException("AlarmState not found.",	DataException::NO_VALUE,sql.c_str()));		

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
			TA_THROW(DataException("AlarmState not found.",	DataException::NO_VALUE, strExSql.c_str()));

		}

		std::vector<AlarmStateData*> alarmStates;
		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					alarmStates.push_back( createAlarmStateData( i, data ) );

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
		return alarmStates;

	}



	

	AlarmStateData* AlarmStateAccessFactory::createAlarmStateData( unsigned long row, IData* data )
	{
		// Get the pkey for the event.
		unsigned long pkey = data->getUnsignedLongData( row, COLUMN_PKEY );
		std::string name = data->getStringData(row, COLUMN_NAME);
		std::string displayname = data->getStringData(row, COLUMN_DISPLAYNAME);
		unsigned long typekey = data->getUnsignedLongData( row, COLUMN_TYPEKEY );

		
		return new AlarmStateData(pkey,name,displayname, typekey);
	}



};
