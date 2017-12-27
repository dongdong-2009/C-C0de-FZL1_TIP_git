/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/SubsystemAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 09:59:59 $
  * Last modified by:  $Author: Ouyang $
  * 
  * SubsystemAccessFactory is a singleton that is used to retrieve Subsystem objects either from the
  * database or newly created. All Subsystem objects returned will adhear to the ISubsystem interface.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/SubsystemAccessFactory.h"

#include "core/data_access_interface/src/ConfigSubsystem.h"
#include "core/data_access_interface/src/Subsystem.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	SubsystemAccessFactory* SubsystemAccessFactory::m_instance = 0;

    SubsystemAccessFactory& SubsystemAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new SubsystemAccessFactory();
        }
        return *m_instance;
    }

    void SubsystemAccessFactory::getSubsystemBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<ISubsystem*>& subsystems, const bool readWrite)
    {
        FUNCTION_ENTRY("getSubsystemBySql");

        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		TA_ASSERT(NULL != pDatabase, "database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
		columnNames.push_back("DISPLAY_NAME");
        columnNames.push_back("IS_PHYSICAL");
        columnNames.push_back("EXCLUSIVE_CONTROL");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
		columnNames.push_back("SYSTEM_KEY");

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
                    if (readWrite)
                    {

                        if (0 == data->getUnsignedLongData( i, columnNames[0] ))
                        {
                            // if we have the subsystem where the key is 0 then we are not interested in it
                            // The user cannot configure this particular location
                            continue;
                        }

                        subsystems.push_back( new ConfigSubsystem(i, *data) );
                    }

                    else
                    {
                        subsystems.push_back( new Subsystem(i, *data) );
                    }
                }
				//data exception
                catch (const TA_Base_Core::DataException&)
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

        FUNCTION_EXIT;
    }


    std::vector<ISubsystem*> SubsystemAccessFactory::getAllSubsystems(const bool readWrite)
    {
        FUNCTION_ENTRY("getAllSubsystems");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_Oracle_SELECT_18051);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18051);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18051);
//		TA_ASSERT(strSql.size() > 0, "SubsystemAccessFactory::getAllSubsystems(): Prepare SQL Statement error");

        // create the SQL string to retrieve all the Subsystems of the alarm
        /*std::ostringstream sql;
        sql << "select PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL, "
			<< "TO_CHAR(SUBSYSTEM.DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
			<< "TO_CHAR(SUBSYSTEM.DATE_CREATED,'YYYYMMDDHH24MISS'),SYSTEM_KEY "
			<< "from SUBSYSTEM";*/

		std::vector<ISubsystem*> subsystems;
		getSubsystemBySql(databaseConnection, strSql, subsystems, readWrite);

        FUNCTION_EXIT;
		return subsystems;
    }


    std::vector<ISubsystem*> SubsystemAccessFactory::getAllPhysicalSubsystems( const bool readWrite/*=false*/)
    {
        FUNCTION_ENTRY("getAllPhysicalSubsystems");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_Oracle_SELECT_18052);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18052);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18052);
//		TA_ASSERT(strSql.size() > 0, "SubsystemAccessFactory::getAllPhysicalSubsystems(): Prepare SQL Statement error");

        // create the SQL string to retrieve all the Subsystems of the alarm
		/*std::ostringstream sql;
        sql << "select PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL, "
			<< "TO_CHAR(SUBSYSTEM.DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
			<< "TO_CHAR(SUBSYSTEM.DATE_CREATED,'YYYYMMDDHH24MISS'),SYSTEM_KEY "
			<< "from SUBSYSTEM where IS_PHYSICAL = 1";*/

		std::vector<ISubsystem*> subsystems;
		getSubsystemBySql(databaseConnection, strSql, subsystems, readWrite);

        FUNCTION_EXIT;
		return subsystems;
    }

    ISubsystem* SubsystemAccessFactory::getSubsystemByKey( const unsigned long key, const bool readWrite )
    {
        FUNCTION_ENTRY("getSubsystemByKey");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_Oracle_SELECT_18053, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18053, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18053, key);
//		TA_ASSERT(strSql.size() > 0, "SubsystemAccessFactory::getSubsystemByKey(): Prepare SQL Statement error");

        // create the SQL string to retrieve all the Subsystems of the alarm
		/*std::ostringstream sql;
        sql << "select PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL, "
			<< "TO_CHAR(SUBSYSTEM.DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
			<< "TO_CHAR(SUBSYSTEM.DATE_CREATED,'YYYYMMDDHH24MISS'),SYSTEM_KEY "
			<< "from SUBSYSTEM where PKEY = " << key;*/

		std::vector<ISubsystem*> subsystems;
		getSubsystemBySql(databaseConnection, strSql, subsystems, readWrite);

        if ( 0 == subsystems.size())
        {
            std::ostringstream message;
			message << "No data found for Subsystem with Key " << key;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );
           
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
		
		TA_ASSERT(1 == subsystems.size(), "Subsystem key unique constraint violated");

        FUNCTION_EXIT;
		return subsystems[0];
    }

	std::vector<ISubsystem*> SubsystemAccessFactory::getSubsystemBySystemKeys( std::vector<unsigned long> keys, const bool readWrite )
	{
		FUNCTION_ENTRY("getSubsystemBySystemKeys");

		std::vector<ISubsystem*> subsystems;

		if(keys.size()==0)
		{
			return subsystems;
		}

		// create the SQL string to retrieve all the Subsystems of the alarm
		std::ostringstream sql;
// 		sql << "select PKEY, NAME, IS_PHYSICAL, EXCLUSIVE_CONTROL, "
// 			<< "TO_CHAR(SUBSYSTEM.DATE_MODIFIED,'YYYYMMDDHH24MISS'), "
// 			<< "TO_CHAR(SUBSYSTEM.DATE_CREATED,'YYYYMMDDHH24MISS'),SYSTEM_KEY "
// 			<< "from SUBSYSTEM where system_key in ( " ;
		for(int i=0; i<keys.size(); i++)
		{
			if(i>0)
			{
				sql<<",";
			}
			sql<<keys[i];
		}
 
// 		sql<<")";
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		 ;
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_DELETE_18402, sql.str().c_str());
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_DELETE_18402, sql.str().c_str());
//		TA_ASSERT(strSql.size() > 0, "SubsystemAccessFactory::getSubsystemBySystemKeys(): Prepare SQL Statement error");
 
		getSubsystemBySql(databaseConnection, strSql, subsystems, readWrite);


		FUNCTION_EXIT;
		return subsystems;
	}

    std::vector<ISubsystem*> SubsystemAccessFactory::getSubsystemsByRegion( unsigned long regionKey, const bool readWrite )
    {
        FUNCTION_ENTRY("getSubsystemsAtLocation");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_Oracle_SELECT_18054, regionKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18054, regionKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18054, regionKey);
//		TA_ASSERT(strSql.size() > 0, "SubsystemAccessFactory::getSubsystemsByRegion(): Prepare SQL Statement error");

        // create the SQL string to retrieve all the Subsystems of the alarm
       /* std::ostringstream sql;
		sql << " select a.PKEY, a.NAME, a.IS_PHYSICAL, a.EXCLUSIVE_CONTROL,"
			<< " TO_CHAR(a.DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
		    << " TO_CHAR(a.DATE_CREATED,'YYYYMMDDHH24MISS'),SYSTEM_KEY"
			<< " from SUBSYSTEM a, SE_REG_SUB_V b"
		   	<< " where a.PKEY = b.PHYSICAL_SUBSYSTEM_KEY and b.SEREGI_ID = " << regionKey;*/

		std::vector<ISubsystem*> subsystems;
		getSubsystemBySql(databaseConnection, strSql, subsystems, readWrite);

        FUNCTION_EXIT;
		return subsystems;
    }


    IConfigSubsystem* SubsystemAccessFactory::createSubsystem()
    {
        return new ConfigSubsystem();
    }


    IConfigSubsystem* SubsystemAccessFactory::copySubsystem(
        const IConfigSubsystem* subsystemToCopy)
    {
        FUNCTION_ENTRY("copySubsystem");

        TA_ASSERT(subsystemToCopy !=  NULL, "The subsystem to copy was NULL");
        
        const ConfigSubsystem* cast =
            dynamic_cast<const ConfigSubsystem*>(subsystemToCopy);

        TA_ASSERT(cast != NULL,
                  "Subsystem passed could not be converted into a "
                  "ConfigSubsystem");

        FUNCTION_EXIT;
        return new ConfigSubsystem(*cast);
    }



} // closes TA_Base_Core


