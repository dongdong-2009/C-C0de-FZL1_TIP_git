/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmSeverityAccessFactory.cpp $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * AlarmSeverityAccessFactory is a singleton that is used to retrieve AlarmSeverityData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/AlarmSeverityData.h"
#include "core/data_access_interface/src/ConfigAlarmSeverityData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    AlarmSeverityAccessFactory* AlarmSeverityAccessFactory::m_instance = 0;

    AlarmSeverityAccessFactory& AlarmSeverityAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new AlarmSeverityAccessFactory();
        }
        return *m_instance;
    }

	void AlarmSeverityAccessFactory::freeInstance()
	{
		if (0 != m_instance)
		{
			delete m_instance;
			m_instance = 0;
		}
	}

    void AlarmSeverityAccessFactory::getAlarmSeverityBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAlarmSeverityData*>& alarmSeverities, const bool readWrite)
    {
        // get a connection to the database
        //IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);
		TA_ASSERT(NULL != pDatabase, "the database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("OPEN_ACKED_FG_COLOUR1");
        columnNames.push_back("OPEN_ACKED_FG_COLOUR2");
        columnNames.push_back("OPEN_ACKED_BG_COLOUR1");
        columnNames.push_back("OPEN_ACKED_BG_COLOUR2");
        columnNames.push_back("OPEN_UNACKED_FG_COLOUR1");
        columnNames.push_back("OPEN_UNACKED_FG_COLOUR2");
        columnNames.push_back("OPEN_UNACKED_BG_COLOUR1");
        columnNames.push_back("OPEN_UNACKED_BG_COLOUR2");
        columnNames.push_back("CLOSED_UNACKED_FG_COLOUR1");
        columnNames.push_back("CLOSED_UNACKED_FG_COLOUR2");
        columnNames.push_back("CLOSED_UNACKED_BG_COLOUR1");
        columnNames.push_back("CLOSED_UNACKED_BG_COLOUR2");
        columnNames.push_back("SEL_OPEN_ACKED_FG_COLOUR1");
        columnNames.push_back("SEL_OPEN_ACKED_FG_COLOUR2");
        columnNames.push_back("SEL_OPEN_ACKED_BG_COLOUR1");
        columnNames.push_back("SEL_OPEN_ACKED_BG_COLOUR2");
        columnNames.push_back("SEL_OPEN_UNACKED_FG_COLOUR1");
        columnNames.push_back("SEL_OPEN_UNACKED_FG_COLOUR2");
        columnNames.push_back("SEL_OPEN_UNACKED_BG_COLOUR1");
        columnNames.push_back("SEL_OPEN_UNACKED_BG_COLOUR2");
        columnNames.push_back("SEL_CLOSED_UNACKED_FG_COLOUR1");
        columnNames.push_back("SEL_CLOSED_UNACKED_FG_COLOUR2");
        columnNames.push_back("SEL_CLOSED_UNACKED_BG_COLOUR1");
        columnNames.push_back("SEL_CLOSED_UNACKED_BG_COLOUR2");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql, columnNames);

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
				try
				{
					if (readWrite)
					{
						alarmSeverities.push_back( new ConfigAlarmSeverityData(i, *data) );
					}
					else
					{
						alarmSeverities.push_back( new AlarmSeverityData(i, *data) );
					}
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
        while ( pDatabase->moreData(data) );
    }


    IAlarmSeverityData* AlarmSeverityAccessFactory::getSeverityByKey(const unsigned long key, const bool readWrite)
    {
        FUNCTION_ENTRY("getSeverityByKey");

		// get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);

		// create the SQL string to retrieve all the severities of the alarm
        /*std::ostringstream sql;
		sql << " select PKEY, NAME,"
			<< " OPEN_ACKED_FG_COLOUR1, OPEN_ACKED_FG_COLOUR2, OPEN_ACKED_BG_COLOUR1, OPEN_ACKED_BG_COLOUR2,"
			<< " OPEN_UNACKED_FG_COLOUR1, OPEN_UNACKED_FG_COLOUR2, OPEN_UNACKED_BG_COLOUR1, OPEN_UNACKED_BG_COLOUR2,"
			<< " CLOSED_UNACKED_FG_COLOUR1, CLOSED_UNACKED_FG_COLOUR2, CLOSED_UNACKED_BG_COLOUR1,"
			<< " CLOSED_UNACKED_BG_COLOUR2, SEL_OPEN_ACKED_FG_COLOUR1, SEL_OPEN_ACKED_FG_COLOUR2,"
			<< " SEL_OPEN_ACKED_BG_COLOUR1, SEL_OPEN_ACKED_BG_COLOUR2, SEL_OPEN_UNACKED_FG_COLOUR1,"
			<< " SEL_OPEN_UNACKED_FG_COLOUR2, SEL_OPEN_UNACKED_BG_COLOUR1, SEL_OPEN_UNACKED_BG_COLOUR2,"
			<< " SEL_CLOSED_UNACKED_FG_COLOUR1, SEL_CLOSED_UNACKED_FG_COLOUR2, SEL_CLOSED_UNACKED_BG_COLOUR1,"
			<< " SEL_CLOSED_UNACKED_BG_COLOUR2,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from ALARMSEVERITY where PKEY = " << key;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMSEVERITY_Oracle_SELECT_10551, key);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMSEVERITY_SELECT_10551, key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMSEVERITY_SELECT_10551, key);
//		TA_ASSERT(sql.size() > 0, "getSeverityByKey: Get SQL Statement error");

		// Perform SQL here
		std::vector< IAlarmSeverityData* > alarmSeverities;
		getAlarmSeverityBySql(databaseConnection, sql, alarmSeverities, readWrite);

        if ( 0 == alarmSeverities.size())
        {
            std::ostringstream message;
			message << "No data found for AlarmSeverity with key " << key;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );
			  
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
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );

        }
		
		TA_ASSERT(1 == alarmSeverities.size(), "Action key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;

		return alarmSeverities[0];
    }

    std::vector<IAlarmSeverityData*> AlarmSeverityAccessFactory::getAllAlarmSeverities(const bool readWrite)
    {
        FUNCTION_ENTRY("getAllAlarmSeverities");
		// get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);

		// create the SQL string to retrieve all the severities of the alarm
        /*std::ostringstream sql;
		sql << " select PKEY, NAME,"
			<< " OPEN_ACKED_FG_COLOUR1, OPEN_ACKED_FG_COLOUR2, OPEN_ACKED_BG_COLOUR1, OPEN_ACKED_BG_COLOUR2,"
			<< " OPEN_UNACKED_FG_COLOUR1, OPEN_UNACKED_FG_COLOUR2, OPEN_UNACKED_BG_COLOUR1, OPEN_UNACKED_BG_COLOUR2,"
			<< " CLOSED_UNACKED_FG_COLOUR1, CLOSED_UNACKED_FG_COLOUR2, CLOSED_UNACKED_BG_COLOUR1,"
			<< " CLOSED_UNACKED_BG_COLOUR2, SEL_OPEN_ACKED_FG_COLOUR1, SEL_OPEN_ACKED_FG_COLOUR2,"
			<< " SEL_OPEN_ACKED_BG_COLOUR1, SEL_OPEN_ACKED_BG_COLOUR2, SEL_OPEN_UNACKED_FG_COLOUR1,"
			<< " SEL_OPEN_UNACKED_FG_COLOUR2, SEL_OPEN_UNACKED_BG_COLOUR1, SEL_OPEN_UNACKED_BG_COLOUR2,"
			<< " SEL_CLOSED_UNACKED_FG_COLOUR1, SEL_CLOSED_UNACKED_FG_COLOUR2, SEL_CLOSED_UNACKED_BG_COLOUR1,"
			<< " SEL_CLOSED_UNACKED_BG_COLOUR2,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from ALARMSEVERITY order by PKEY";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMSEVERITY_Oracle_SELECT_10552);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMSEVERITY_SELECT_10552);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMSEVERITY_SELECT_10552);
//		TA_ASSERT(sql.size() > 0, "getAllAlarmSeverities: Get SQL Statement error");
  
		// Perform SQL here
		std::vector< IAlarmSeverityData* > alarmSeverities;
		getAlarmSeverityBySql(databaseConnection, sql, alarmSeverities, readWrite);

        // Return the pointer. The class that receives this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return alarmSeverities;
    }

} //closes TA_Base_Core


