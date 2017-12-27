/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ECSStationColourAccessFactory.cpp $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * ECSStationColourAccessFactory is a singleton that is used to retrieve ECSStationColourData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/ECSStationColourAccessFactory.h"
#include "core/data_access_interface/src/ConfigECSStationColourData.h"
#include "core/data_access_interface/src/ECSStationColourData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    ECSStationColourAccessFactory* ECSStationColourAccessFactory::m_instance = 0;

    ECSStationColourAccessFactory& ECSStationColourAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new ECSStationColourAccessFactory();
        }
        return *m_instance;
    }
	
    void ECSStationColourAccessFactory::getECSStationColourBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IECSStationColourData*>& ecsStationColours, const bool readWrite)
    {
		// get a connection to the database
        //IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);
		TA_ASSERT(NULL != pDatabase, "Database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("ECSCOL_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("CURRENT_ECS_STATION_COLOUR");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery( sql, columnNames );

		// Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
				try
				{
					if (readWrite)
					{
						ecsStationColours.push_back( new ConfigECSStationColourData(i, *data) );
					}
					else
					{
						ecsStationColours.push_back( new ECSStationColourData(i, *data) );
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


    IECSStationColourData* ECSStationColourAccessFactory::getStationByKey(const unsigned long key,const bool readWrite)
    {
		FUNCTION_ENTRY("getStationByKey");

		// get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_COLOUR_Oracle_SELECT_14051, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_COLOUR_SELECT_14051, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_COLOUR_SELECT_14051, key);
//		TA_ASSERT(strSql.size() > 0, "ECSStationColourAccessFactory::getStationByKey(): Prepare SQL Statement error");

		// create the SQL string to retrieve all the severities of the alarm
        /*std::ostringstream sql;
        sql << "select ECSCOL_ID, NAME, CURRENT_ECS_STATION_COLOUR,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from EC_STATION_COLOUR where ECSCOL_ID = " << key;*/
		
		std::vector<IECSStationColourData*> ecsStationColours;
		getECSStationColourBySql(databaseConnection, strSql, ecsStationColours, readWrite);

		if ( 0 == ecsStationColours.size())
        {
            std::ostringstream message;
			message << "No data found for EcsStationColour with key " << key;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );

        }

		TA_ASSERT(1 == ecsStationColours.size(), "EcsStationColours key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return ecsStationColours[0];
    }


    std::vector<IECSStationColourData*> ECSStationColourAccessFactory::getAllECSStationColours(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllECSStationColours");

		// get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EC_STATION_COLOUR_Oracle_SELECT_14052);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EC_STATION_COLOUR_SELECT_14052);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EC_STATION_COLOUR_SELECT_14052);
//		TA_ASSERT(strSql.size() > 0, "ECSStationColourAccessFactory::getAllECSStationColours(): Prepare SQL Statement error");

		// create the SQL string to retrieve all the severities of the alarm
       /* std::ostringstream sql;
        sql << "select ECSCOL_ID, NAME, CURRENT_ECS_STATION_COLOUR,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " from EC_STATION_COLOUR order by ECSCOL_ID";*/

		std::vector<IECSStationColourData*> ecsStationColours;
		getECSStationColourBySql(databaseConnection, strSql, ecsStationColours, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return ecsStationColours;
    }

} //closes TA_Base_Core


