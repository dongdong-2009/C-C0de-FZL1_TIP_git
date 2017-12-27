/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/PowerDemandAccessFactory.cpp $
  * @author:  wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PowerDemandAccessFactory is a singleton that is used to retrieve or set power demand data
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include <sstream>


#include "ace/OS.h"

#include "core/data_access_interface/src/PowerDemandAccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{
    PowerDemandAccessFactory* PowerDemandAccessFactory::s_instance = 0;


    PowerDemandAccessFactory& PowerDemandAccessFactory::getInstance()
    {
        if( s_instance == 0 )
        {
            s_instance = new PowerDemandAccessFactory();
        }
        return *s_instance;
    }


	void PowerDemandAccessFactory::getPowerDemandDataPointNames(std::set<std::string>& powerDemandPoints)
    {
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read); 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), pwr_dmd_30min_table_STD_SELECT_19001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PWR_DMD_30MIN_TABLE_SELECT_19001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PWR_DMD_30MIN_TABLE_SELECT_19001);
//		TA_ASSERT(strSql.size() > 0, "PowerDemandAccessFactory::getPowerDemandDataPointNames(): Prepare SQL Statement error");

        // create the SQL string to retrieve the unique name
        /*std::ostringstream sql;
        sql << "SELECT DISTINCT name FROM entity_v, pwr_dmd_30min_table WHERE pkey = entitykey";*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for power demand data point names"; 			
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

		do
        {
            try
            {
                for (unsigned int i = 0; i < data->getNumRows(); ++i)
                {
                    // Retrieve the name from the returned data.
					std::string name = data->getStringData(i,nameColumn);

					if ( 0 != name.compare("") )
					{
						powerDemandPoints.insert(name.c_str());
					}
                }
            }
			catch( const DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );

				// delete data object
				delete data; 
				data = NULL;
				TA_THROW(e);
			}
            
            // Now that we're finished with the IData object, we need to delete it.
            // This clears it ready for the next lot if necessary.
            delete data;
            data = NULL;

        }while( databaseConnection->moreData(data) );

    }


	void PowerDemandAccessFactory::getPowerDemandData(PowerDemandData& powerDemandData)
	{
		// meter name should not be empty and interval > 0
		time_t DemandDataInterval = powerDemandData.getInterval();
		std::string meterDpName = powerDemandData.getMeterName();

		if ( 0 == meterDpName.compare("") || DemandDataInterval <= 0 )
		{
			return;
		}

		// convert the time interval specified by the powerDemandData parameter
		// to the corresponding SOURCEDATE and INT values
		struct tm localTime;
		ACE_OS::localtime_r( &DemandDataInterval, &localTime );

		unsigned long interval = ( (localTime.tm_hour * 60 + localTime.tm_min) / 30) + 1;


		// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read); 	 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), pwr_dmd_30min_table_Oracle_SELECT_19051,
//		std::string strSql  = databaseConnection->prepareSQLStatement(PWR_DMD_30MIN_TABLE_SELECT_19051,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PWR_DMD_30MIN_TABLE_SELECT_19051,
			localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday, interval, meterDpName);
//		TA_ASSERT(strSql.size() > 0, "PowerDemandAccessFactory::getPowerDemandData(): Prepare SQL Statement error");

        // create the SQL string to retrieve the unique name
		/*std::ostringstream sql;
		sql << "SELECT demand, edited_demand FROM pwr_dmd_30min_table WHERE sourcedate = to_date('"
			<< localTime.tm_year + 1900
			<< "/"
			<< localTime.tm_mon + 1
			<< "/"
			<< localTime.tm_mday
			<< "', 'yyyy/mm/dd')"
			<< " AND int="
			<< interval
			<< " AND entitykey=(SELECT pkey FROM entity_v WHERE name='"
			<< meterDpName
			<< "')";*/

		// Set up the columnNames vector to be passed to executeQuery()
        std::string demandColumn = "DEMAND";
        std::string editedDemandColumn = "EDITED_DEMAND";
        std::vector<std::string> columnNames;
        columnNames.push_back(demandColumn);
        columnNames.push_back(editedDemandColumn);

		//std::string sqlCmd = sql.str().c_str();
      
		// Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

		if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for power demand data point names"; 			
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

		
        do
        {
            try
            {
				unsigned long recordedValue = 0;
				unsigned long updatedValue = 0;

                for (unsigned int i = 0; i < data->getNumRows(); ++i)
                {
                    // Retrieve the key from the returned data.
					recordedValue = data->getUnsignedLongData(i,demandColumn);

					if ( false == data->isNull(i,editedDemandColumn) )
					{
						updatedValue = data->getUnsignedLongData(i, editedDemandColumn);
					}
					else if (recordedValue != 0 && updatedValue != recordedValue)
					{
						updatedValue = recordedValue;
					}

					powerDemandData.setRecordedValue(recordedValue);
					powerDemandData.setUpdatedValue(updatedValue);		
                }
            }
			catch( const DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );

				// delete data object
				delete data; 
				data = NULL;

				TA_THROW(e);
			}
            
            // Now that we're finished with the IData object, we need to delete it.
            // This clears it ready for the next lot if necessary.
            delete data;
            data = NULL;

        }while( databaseConnection->moreData(data) );
		
	}


	void PowerDemandAccessFactory::setPowerDemandData(const PowerDemandData& powerDemandData)
	{
		// meter name should not be empty and interval > 0
		time_t DemandDataInterval = powerDemandData.getInterval();
		std::string meterDpName = powerDemandData.getMeterName();

		if ( 0 == meterDpName.compare("") || DemandDataInterval <= 0 )
		{
			return;
		}

		// convert the time interval specified by the powerDemandData parameter
		// to the corresponding SOURCEDATE and INT values
		struct tm localTime;
		ACE_OS::localtime_r( &DemandDataInterval, &localTime );

		unsigned long interval = ( (localTime.tm_hour * 60 + localTime.tm_min) / 30) + 1;

		// get session ID
		std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
		
		// get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write); 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), pwr_dmd_30min_table_Oracle_UPDATE_19201,
//		std::string strSql  = databaseConnection->prepareSQLStatement(PWR_DMD_30MIN_TABLE_UPDATE_19201,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PWR_DMD_30MIN_TABLE_UPDATE_19201,
			powerDemandData.getUpdatedValue(),sessionId,localTime.tm_year + 1900,localTime.tm_mon + 1,localTime.tm_mday,interval,meterDpName);
//		TA_ASSERT(strSql.size() > 0, "PowerDemandAccessFactory::setPowerDemandData(): Prepare SQL Statement error");

		/*std::ostringstream sql;

		sql << "UPDATE pwr_dmd_30min_table SET edited_demand="
			<< powerDemandData.getUpdatedValue()
			<< ", modified_by='"
			<< sessionId
			<< "'"
			<< ", date_modified=SYSDATE "
			<< "WHERE sourcedate=to_date('"
			<< localTime.tm_year + 1900
			<< "/"
			<< localTime.tm_mon + 1
			<< "/"
			<< localTime.tm_mday
			<< "', 'yyyy/mm/dd') AND int="
			<< interval
			<< " AND entitykey = (SELECT pkey FROM entity_v WHERE name='"
			<< meterDpName
			<< "')";*/

		//LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PowerDemandAccessFactory::setPowerDemandData() - sql is %s", strSql.c_str());
						
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
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PowerDemandAccessFactory::setPowerDemandData() - sql is %s",
			strExSql.c_str());


		databaseConnection->executeModification(strSql);

	}


} // closes TA_Base_Core


