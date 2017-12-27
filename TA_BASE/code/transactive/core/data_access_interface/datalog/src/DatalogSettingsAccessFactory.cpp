/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogSettingsAccessFactory.cpp $
* @author: 
* @version: $Revision:  $
*
* Last modification: $DateTime:  $
* Last modified by:  $Author:  $
*
*
*
*/

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>

#include "DatalogSettingsAccessFactory.h"
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

	//columns for Datalog_ConfigSettings
	const std::string COLUMN_PKEY( "PKEY" );
	const std::string COLUMN_LOCATION_KEY( "LOCATION_KEY" );
	const std::string COLUMN_CONFIG_TYPE( "CONFIG_TYPE" );
	const std::string COLUMN_DP_TABLE_NAME( "DP_TABLE_NAME" );
	const std::string COLUMN_DP_GROUP_TABLE_NAME( "DP_GROUP_TABLE_NAME" );
	const std::string COLUMN_DP_LOG_TABLE_NAME( "DP_LOG_TABLE_NAME" );
	const std::string COLUMN_VERSION_NUM( "VERSION_NUM" );
	const std::string COLUMN_DP_VIEW_TABLE_NAME ("DP_VIEW_TABLE_NAME" );


	// columns for view Datalog_DP_DPGROUP_TREND_View or Datalog_DP_DPGROUP_Report_View
	const std::string COLUMN_DP_PKEY( "DP_PKEY" );
	const std::string COLUMN_ENTITY_KEY( "ENTITY_KEY" );
	const std::string COLUMN_DP_NAME( "DP_NAME" );
	const std::string COLUMN_DP_ENABLED( "DP_ENABLED" );
	const std::string COLUMN_DP_GRP_KEY( "DP_GRP_KEY" );
	const std::string COLUMN_DP_GRP_NAME( "DP_GRP_NAME" );
	const std::string COLUMN_DP_GRP_DESC( "DP_GRP_DESC" );
	const std::string COLUMN_SAMPLE_INTERVAL( "SAMPLE_INTERVAL" );
	const std::string COLUMN_INTERVAL_TYPE( "INTERVAL_TYPE" );
	const std::string COLUMN_START_TIME( "START_TIME" );
	const std::string COLUMN_DP_GRP_ENABLED( "DP_GRP_ENABLED" );
	const std::string COLUMN_DELTA_VALUE( "DELTA_VALUE" );
	//const std::string COLUMN_PKEY( "LOCATION_KEY" );

	const std::string COLUMN_ENTITY_LOCATIONKEY( "LOCATION_KEY" );

	const std::string DB_DATA_TRENDING ("Trending");   
	const std::string DB_DATA_REPORT ("Report");


};

namespace TA_Base_Core
{
	DatalogSettingsAccessFactory* DatalogSettingsAccessFactory::m_instance = NULL;
	NonReEntrantThreadLockable DatalogSettingsAccessFactory::m_singletonLock;

	DatalogSettingsAccessFactory& DatalogSettingsAccessFactory::getInstance()
	{
		ThreadGuard guard(m_singletonLock);
		// Create a new instance if it doesn't exist.
		if ( 0 == m_instance )
		{
			m_instance = new DatalogSettingsAccessFactory();
		}

		return *m_instance;
	}

	std::vector<DatalogConfigSettingsData *> DatalogSettingsAccessFactory::getDatalogConfigSettingsData (unsigned long locationKey )
	{
		FUNCTION_ENTRY("getDatalogConfigSettingsData");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		//
		/*std::ostringstream sql;
		sql << "SELECT d.Pkey,d.Location_Key, d.Config_Type, d.DP_Table_Name, ";
		sql << "d.DP_Group_Table_Name, d.DP_Log_Table_Name, d.Version_Num,d.DP_View_Table_Name FROM Datalog_ConfigSettings d ";
		sql << "WHERE Location_Key = " << locationKey<<" ";*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, DATALOG_CONFIGSETTINGS_SELECT_82001,locationKey);

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_LOCATION_KEY);
		columnNames.push_back( COLUMN_CONFIG_TYPE );
		columnNames.push_back( COLUMN_DP_TABLE_NAME);
		columnNames.push_back( COLUMN_DP_GROUP_TABLE_NAME );
		columnNames.push_back( COLUMN_DP_LOG_TABLE_NAME);
		columnNames.push_back( COLUMN_VERSION_NUM);
		columnNames.push_back( COLUMN_DP_VIEW_TABLE_NAME);

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading configSetting data: %s", sql.str().c_str());

		IData* data = databaseConnection->executeQuery( sql, columnNames ) ;

		std::vector<DatalogConfigSettingsData*> configSettings;

		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					configSettings.push_back( createDatalogConfigSettingsData( i, data ) );

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
		return configSettings;

	}

	std::vector<DatalogConfigSettingsData *> DatalogSettingsAccessFactory::getDatalogConfigSettingsData ()
	{
		FUNCTION_ENTRY("getDatalogConfigSettingsData");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		//
		/*std::ostringstream sql;
		sql << "SELECT d.Pkey,d.Location_Key, d.Config_Type, d.DP_Table_Name, ";
		sql << "d.DP_Group_Table_Name, d.DP_Log_Table_Name, d.Version_Num,d.DP_View_Table_Name FROM Datalog_ConfigSettings d ";
		sql << "WHERE Location_Key = " << locationKey<<" ";*/

		int locationKey = 0; //for all location

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, DATALOG_CONFIGSETTINGS_SELECT_82001,locationKey);

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_LOCATION_KEY);
		columnNames.push_back( COLUMN_CONFIG_TYPE );
		columnNames.push_back( COLUMN_DP_TABLE_NAME);
		columnNames.push_back( COLUMN_DP_GROUP_TABLE_NAME );
		columnNames.push_back( COLUMN_DP_LOG_TABLE_NAME);
		columnNames.push_back( COLUMN_VERSION_NUM);
		columnNames.push_back( COLUMN_DP_VIEW_TABLE_NAME);

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading configSetting data: %s", sql.str().c_str());

		IData* data = databaseConnection->executeQuery( sql, columnNames ) ;

		std::vector<DatalogConfigSettingsData*> configSettings;

		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					configSettings.push_back( createDatalogConfigSettingsData( i, data ) );

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
		return configSettings;

	}

	std::vector<DatalogConfigSettingsData *> DatalogSettingsAccessFactory::getNewDatalogConfigSettingsData(unsigned long locationKey, std::vector<unsigned long> existedPkeys)
	{
		FUNCTION_ENTRY("getDatalogConfigSettingsData");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Check if the alarm rule is actually in the database
		/*std::ostringstream sql;
		sql << "SELECT d.Pkey,d.Location_Key, d.Config_Type, d.DP_Table_Name, ";
		sql << "d.DP_Group_Table_Name, d.DP_Log_Table_Name, d.Version_Num,d.DP_View_Table_Name FROM Datalog_ConfigSettings d ";
		sql << "WHERE Location_Key = " << locationKey<<" ";*/


		/*if(existedPkeys.size()>0)
		{
		sql << " and pkey not in (";

		for ( int i=0; i<existedPkeys.size(); i++)
		{
		sql << existedPkeys[i];
		if(i != existedPkeys.size() -1)
		{
		sql<< ",";
		}
		}

		sql << ")";
		}*/
		SQLStatement sql;
		if(existedPkeys.size()>0)
		{	
			std::ostringstream sqlpkeys;
			for ( int i=0; i<existedPkeys.size(); i++)
			{
				sqlpkeys << existedPkeys[i];
				if(i != existedPkeys.size() -1)
				{
					sqlpkeys<< ",";
				}
			}
			std::string Pkeys;
			Pkeys = sqlpkeys.str();
			databaseConnection->prepareSQLStatement(sql, DATALOG_CONFIGSETTINGS_SELECT_82002,locationKey,Pkeys);
		}
		else
		{
			databaseConnection->prepareSQLStatement(sql, DATALOG_CONFIGSETTINGS_SELECT_82001,locationKey);
		}

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_LOCATION_KEY);
		columnNames.push_back( COLUMN_CONFIG_TYPE );
		columnNames.push_back( COLUMN_DP_TABLE_NAME);
		columnNames.push_back( COLUMN_DP_GROUP_TABLE_NAME );
		columnNames.push_back( COLUMN_DP_LOG_TABLE_NAME);
		columnNames.push_back( COLUMN_VERSION_NUM);
		columnNames.push_back( COLUMN_DP_VIEW_TABLE_NAME);

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading configSetting data: %s", sql.str().c_str());

		IData* data = databaseConnection->executeQuery( sql, columnNames ) ;

		std::vector<DatalogConfigSettingsData*> configSettings;

		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					configSettings.push_back( createDatalogConfigSettingsData( i, data ) );

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
		return configSettings;

	}

	// table Name : DATALOG_DP_DPGROUP_TREND_VIEW, DATALOG_DP_DPGROUP_REPORT_VIEW
	std::vector<DatalogDPData*> DatalogSettingsAccessFactory::getDatalogDPData ( unsigned long locationKey, std::string tableName)
	{
		FUNCTION_ENTRY("getDatalogConfigSettingsData");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Check if the alarm rule is actually in the database
		/*std::ostringstream sql;
		sql << "SELECT DP_KEY, ENTITY_KEY, DP_NAME, DP_ENABLED, DP_GRP_KEY, ";
		sql << "DP_GRP_NAME, DP_GRP_DESC, SAMPLE_INTERVAL, INTERVAL_TYPE, TO_CHAR(START_TIME,'YYYYMMDDHH24MISS'), ";
		sql << "DP_GRP_ENABLED, DELTA_VALUE, LOCATION_KEY from " ;
		sql << tableName;
		sql << " where location_key = "<< locationKey ;
		sql << " AND DP_ENABLED = 'Y' AND DP_GRP_ENABLED = 'Y' ";*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, DATALOG_CONFIGSETTINGS_SELECT_82004,tableName,locationKey);

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_DP_PKEY );
		columnNames.push_back( COLUMN_ENTITY_KEY );
		columnNames.push_back( COLUMN_DP_NAME );
		columnNames.push_back( COLUMN_DP_ENABLED );
		columnNames.push_back( COLUMN_DP_GRP_KEY );
		columnNames.push_back( COLUMN_DP_GRP_NAME );
		columnNames.push_back( COLUMN_DP_GRP_DESC );
		columnNames.push_back( COLUMN_SAMPLE_INTERVAL );
		columnNames.push_back( COLUMN_INTERVAL_TYPE );
		columnNames.push_back( COLUMN_START_TIME );
		columnNames.push_back( COLUMN_DP_GRP_ENABLED );
		columnNames.push_back( COLUMN_DELTA_VALUE );
		columnNames.push_back( COLUMN_ENTITY_LOCATIONKEY );

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading DP data: %s", sql.str().c_str());

		IData* data = databaseConnection->executeQuery(sql,columnNames);
		std::vector<DatalogDPData*> loadedDP;

		do
		{
			// Loop through and create all the objects
			for(unsigned int i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					loadedDP.push_back( createDatalogDPData( i, data ) );

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

		return loadedDP;

	}

	DatalogConfigSettingsData* DatalogSettingsAccessFactory::createDatalogConfigSettingsData( unsigned long row, IData* data )
	{
		// Get the pkey for the event.
		unsigned long pkey = data->getUnsignedLongData( row, COLUMN_PKEY );
		unsigned long locKey = data->getUnsignedLongData( row, COLUMN_LOCATION_KEY );

		std::string strConfigType = data->getStringData( row, COLUMN_CONFIG_TYPE );
		ConfigType configType;
		if( DB_DATA_TRENDING == strConfigType )
		{
			configType = TA_Base_Core::Trending;
		}else
		{
			configType = TA_Base_Core::Report;
		}

		std::string dpTableName = data->getStringData( row, COLUMN_DP_TABLE_NAME );
		std::string dpGrpTableName = data->getStringData( row, COLUMN_DP_GROUP_TABLE_NAME );
		std::string dpLogTableName = data->getStringData( row, COLUMN_DP_LOG_TABLE_NAME );
		unsigned long version_Num = data->getUnsignedLongData( row, COLUMN_VERSION_NUM );
		std::string dpViewTableName = data->getStringData( row, COLUMN_DP_VIEW_TABLE_NAME );

		return new DatalogConfigSettingsData(pkey,locKey,configType,dpTableName, dpGrpTableName, dpLogTableName, version_Num, dpViewTableName);
	}

	DatalogDPData* DatalogSettingsAccessFactory::createDatalogDPData( unsigned long row, IData* data )
	{

		unsigned long dp_key = data->getUnsignedLongData(row, COLUMN_DP_PKEY);
		unsigned long entiy_key = data->getUnsignedLongData(row, COLUMN_ENTITY_KEY);
		std::string dp_name = data->getStringData(row, COLUMN_DP_NAME);
		std::string str_dp_enabled = data->getStringData(row, COLUMN_DP_ENABLED);
		bool dp_enabled = ("Y"== str_dp_enabled ? true:false);

		unsigned long dp_grp_key = data->getUnsignedLongData(row, COLUMN_DP_GRP_KEY);
		std::string dp_grp_name = data->getStringData( row, COLUMN_DP_GRP_NAME);
		std::string dp_grp_desc = data->getStringData( row, COLUMN_DP_GRP_DESC);
		unsigned long sample_interval = data->getUnsignedLongData( row, COLUMN_SAMPLE_INTERVAL);
		std::string strIntervalType = data->getStringData(row, COLUMN_INTERVAL_TYPE);

		IntervalType interval_type ;
		if ( "S" == strIntervalType)
		{
			interval_type = TA_Base_Core::Second;
		}
		else if( "MI" == strIntervalType)
		{
			interval_type = TA_Base_Core::Minute;
		}
		else if ( "H" == strIntervalType)
		{
			interval_type = TA_Base_Core::Hour;
		}
		else if ( "D" == strIntervalType)
		{
			interval_type = TA_Base_Core::Day;
		}
		else /*if ( "M" == strIntervalType)*/
		{
			interval_type = TA_Base_Core::Month;
		}

		time_t startTime = data->getDateData(row, COLUMN_START_TIME);
		std::string str_dp_grp_enabled = data->getStringData(row, COLUMN_DP_GRP_ENABLED);
		bool dp_grp_enabled = ( "Y" == str_dp_grp_enabled ? true:false);
		double delta_value = data->getRealData(row, COLUMN_DELTA_VALUE);
		unsigned long location_key = data->getUnsignedLongData(row, COLUMN_ENTITY_LOCATIONKEY);

		return new DatalogDPData(entiy_key,dp_name,dp_enabled,dp_grp_key, dp_grp_name, 
			dp_grp_desc, sample_interval, interval_type, startTime, dp_grp_enabled, delta_value, location_key,dp_key);

	}


	bool DatalogSettingsAccessFactory::loadVersionNumByKey( unsigned long pkey, unsigned long &newVersion)
	{
		FUNCTION_ENTRY("loadVersionNum");

		// Get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Check if the alarm rule is actually in the database
		/*std::ostringstream sql;
		sql << "SELECT Version_Num FROM Datalog_ConfigSettings ";
		sql << "WHERE pkey = " << pkey ;*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, DATALOG_CONFIGSETTINGS_SELECT_82003, pkey);

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_VERSION_NUM);

		IData * data = databaseConnection->executeQuery( sql, columnNames ) ;

		int nRows = data->getNumRows();

		if (nRows == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Can't find DatalogConfigsetting with pkey: %ld.", pkey);
			FUNCTION_EXIT;
			return false;
		}
		if (nRows > 1)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "More than one DatalogConfigsetting found with same pkey: %ld", pkey);
			FUNCTION_EXIT;
			return false;
		}

		newVersion = data->getUnsignedLongData(0, COLUMN_VERSION_NUM);
		delete data;
		data = NULL;

		FUNCTION_EXIT;
		return true;

	}


};
