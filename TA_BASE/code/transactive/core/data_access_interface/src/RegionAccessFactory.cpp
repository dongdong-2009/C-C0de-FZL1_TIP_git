/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/RegionAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RegionAccessFactory is a singleton that is used to retrieve Region objects either from the
  * database or newly created. All Region objects returned will adhear to the IRegion interface.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/data_access_interface/src/RegionAccessFactory.h"
#include "core/data_access_interface/src/Region.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	RegionAccessFactory* RegionAccessFactory::m_instance = 0;

    RegionAccessFactory& RegionAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new RegionAccessFactory();
        }
        return *m_instance;
    }

    void RegionAccessFactory::getRegionBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector< IRegion* >& regions, const bool readWrite)
    {
        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
		TA_ASSERT(0 != pDatabase, "the database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEREGI_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
		columnNames.push_back("DISPLAY_NAME");
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

                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    regions.push_back( new Region( i, *data ) );
				}
				//data exception
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

    std::vector<IRegion*> RegionAccessFactory::getAllRegions(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllRegions");

		TA_ASSERT( false==readWrite, "A configurable version of the region object has not been implemented" );
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// create the SQL string to retrieve all the regions of the alarm
       /* std::ostringstream sql;
        sql << "select SEREGI_ID, NAME, DESCRIPTION, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SE_REGION order by NAME";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_REGION_Oracle_SELECT_8551);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_REGION_SELECT_8551);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_REGION_SELECT_8551);
//		TA_ASSERT(sql.size() > 0, "getAllRegions: Get SQL Statement error");

		std::vector< IRegion* > regions;
		getRegionBySql(databaseConnection, sql, regions, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return regions;
    }

	IRegion* RegionAccessFactory::getRegionByKey( const unsigned long regionKey, const bool readWrite )
	{
		FUNCTION_ENTRY("getRegionByKey");

		TA_ASSERT( false==readWrite, "A configurable version of the region object has not been implemented" );
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// create the SQL string to retrieve all the regions of the alarm
        /*std::ostringstream sql;
        sql << "select SEREGI_ID, NAME, DESCRIPTION, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SE_REGION where SEREGI_ID = " << regionKey;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			SE_REGION_Oracle_SELECT_8552, regionKey);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_REGION_SELECT_8552, regionKey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_REGION_SELECT_8552, regionKey);
//		TA_ASSERT(sql.size() > 0, "getRegionByKey: Get SQL Statement error");

		std::vector< IRegion* > regions;
		getRegionBySql(databaseConnection, sql, regions, readWrite);

        if ( 0 == regions.size())
        {
            std::ostringstream message;
			message << "No data found for Region with Key " << regionKey;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );
            
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
		
		TA_ASSERT(1 == regions.size(), "Region key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return regions[0];
	}

	std::vector<IRegion*> RegionAccessFactory::getRegionsByLocation( const unsigned long locationKey )
	{
		FUNCTION_ENTRY("getRegionsByLocation");

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// create the SQL string to retrieve all the regions of the alarm
        /*std::ostringstream sql;
		sql << " SELECT b.SEREGI_ID, b.NAME, b.DESCRIPTION,"
			<< " TO_CHAR(b.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(b.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " FROM SE_LOC_REG_V a, SE_REGION b WHERE a.SEREGI_ID=b.SEREGI_ID"
			<< " and a.LOCATIONKEY = " << locationKey;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), 			SE_REGION_Oracle_SELECT_8553, locationKey);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_REGION_SELECT_8553, locationKey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_REGION_SELECT_8553, locationKey);

		std::vector< IRegion* > regions;
		getRegionBySql(databaseConnection, sql, regions, true);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return regions;		
	}

	std::vector<IRegion*> RegionAccessFactory::getRegionsByOperator( const unsigned long operatorKey )
	{
		FUNCTION_ENTRY("getRegionsByOperator");

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

		// create the SQL string to retrieve all the regions of the alarm
       /* std::ostringstream sql;
		sql << " SELECT b.SEREGI_ID, b.NAME, b.DESCRIPTION,"
			<< " TO_CHAR(b.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(b.DATE_CREATED,'YYYYMMDDHH24MISS')"
			<< " FROM SE_OPERATOR_REGIONS a, SE_REGION b WHERE a.SEREGI_ID=b.SEREGI_ID"
			<< " and a.operatorkey = " << operatorKey;*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),			SE_REGION_Oracle_SELECT_8554, operatorKey);
//		std::string sql  = databaseConnection->prepareSQLStatement(SE_REGION_SELECT_8554, operatorKey);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, SE_REGION_SELECT_8554, operatorKey);
//		TA_ASSERT(sql.size() > 0, "getRegionsByOperator: Get SQL Statement error");

		std::vector< IRegion* > regions;
		getRegionBySql(databaseConnection, sql, regions, true);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return regions;	
	}

} // closes TA_Base_Core


