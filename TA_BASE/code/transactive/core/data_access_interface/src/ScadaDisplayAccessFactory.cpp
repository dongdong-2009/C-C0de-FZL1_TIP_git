/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ScadaDisplayAccessFactory.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * ScadaDisplayAccessFactory is a singleton that is used to retrieve display objects either from the
 * database or newly created. All display objects returned will adhear to the IScadaDisplay interface.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"

#include "core/data_access_interface/src/ScadaDisplay.h"
#include "core/data_access_interface/src/ConfigScadaDisplay.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    ScadaDisplayAccessFactory* ScadaDisplayAccessFactory::s_instance = 0;


    ScadaDisplayAccessFactory& ScadaDisplayAccessFactory::getInstance()
    {
        if( s_instance == 0 )
        {
            s_instance = new ScadaDisplayAccessFactory();
        }
        return *s_instance;
    }


    void ScadaDisplayAccessFactory::getScadaDisplayBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IScadaDisplay*>& displayPointers, const bool readWrite)
    {
        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
		TA_ASSERT(NULL != pDatabase, "database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SCSSCH_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("PATH");
        columnNames.push_back("LOCATION");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql, columnNames);

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {
				if (readWrite) // need a config display
                {
                    displayPointers.push_back( new ConfigScadaDisplay(i, *data) );
                }
                else // need a standard display
                {
                    displayPointers.push_back( new ScadaDisplay(i, *data) );
                }
            }

            delete data;
            data = NULL;
        }
        while( pDatabase->moreData(data) );
    }    


    IScadaDisplay* ScadaDisplayAccessFactory::getScadaDisplay(const unsigned long key)
    {
		FUNCTION_ENTRY("getScadaDisplay( key )");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_Oracle_SELECT_20051, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20051, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20051, key);
//		TA_ASSERT(strSql.size() > 0, "ScadaDisplayAccessFactory::getScadaDisplay(): Prepare SQL Statement error");

        // create the SQL string to retrieve the name of the display
        // this also doubles as a check to ensure an display with the specified
        // PKEY actually exists.
        /*std::ostringstream sql;
		sql << " select SCSSCH_ID, NAME, PATH, COLOCA_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SC_SCADA_SCHEMATIC WHERE SCSSCH_ID ="
			<< key;*/

		std::vector<IScadaDisplay*> displayPointers;
		getScadaDisplayBySql(databaseConnection, strSql, displayPointers, false);

        if ( 0 == displayPointers.size())
        {
            std::ostringstream message;
			message << "No data found for ScadaDisplay with Key " << key;
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
		
		TA_ASSERT(1 == displayPointers.size(), "DisplayPointer key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return displayPointers[0];
    }


    IScadaDisplay* ScadaDisplayAccessFactory::getScadaDisplay(const std::string& name)
    {
		FUNCTION_ENTRY("getScadaDisplay( name )");
		
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_Oracle_SELECT_20052,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20052,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20052,
			databaseConnection->escapeQueryString(name));
//		TA_ASSERT(strSql.size() > 0, "ScadaDisplayAccessFactory::getScadaDisplay(): Prepare SQL Statement error");

        // create the SQL string to retrieve the name of the display
        // this also doubles as a check to ensure an display with the specified
        // PKEY actually exists.
        /*std::ostringstream sql;
		sql << " select SCSSCH_ID, NAME, PATH, COLOCA_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SC_SCADA_SCHEMATIC WHERE NAME = '"
			<< databaseConnection->escapeQueryString(name) << "'";*/

		std::vector<IScadaDisplay*> displayPointers;
		getScadaDisplayBySql(databaseConnection, strSql, displayPointers, false);

        if ( 0 == displayPointers.size())
        {
            std::ostringstream message;
			message << "No data found for ScadaDisplay with Name " << name;
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

		TA_ASSERT(1 == displayPointers.size(), "DisplayPointer key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return displayPointers[0];
    }


    std::vector<IScadaDisplay*> ScadaDisplayAccessFactory::getScadaDisplaysForLocation( const unsigned long locationKey )
    {
		FUNCTION_ENTRY("getScadaDisplaysForLocation");

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_Oracle_SELECT_20053,locationKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20053,locationKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20053,locationKey);
//		TA_ASSERT(strSql.size() > 0, "ScadaDisplayAccessFactory::getScadaDisplaysForLocation(): Prepare SQL Statement error");

        // create the SQL string to retrieve the guis
        /*std::ostringstream sql;
		sql << " select SCSSCH_ID, NAME, PATH, COLOCA_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SC_SCADA_SCHEMATIC WHERE COLOCA_ID = " << locationKey;*/

		std::vector<IScadaDisplay*> displayPointers;
		getScadaDisplayBySql(databaseConnection, strSql, displayPointers, false);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return displayPointers;
    }


    std::vector<IScadaDisplay*> ScadaDisplayAccessFactory::getAllScadaDisplays( const bool readWrite)
    {
		FUNCTION_ENTRY("getAllScadaDisplays");

		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_SCADA_SCHEMATIC_Oracle_SELECT_20054);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_SCADA_SCHEMATIC_SELECT_20054);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_SCADA_SCHEMATIC_SELECT_20054);
//		TA_ASSERT(strSql.size() > 0, "ScadaDisplayAccessFactory::getAllScadaDisplays(): Prepare SQL Statement error");

        /*std::ostringstream sql;
		sql << " select SCSSCH_ID, NAME, PATH, COLOCA_ID,"
			<< " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SC_SCADA_SCHEMATIC";*/

		std::vector<IScadaDisplay*> displayPointers;
		getScadaDisplayBySql(databaseConnection, strSql, displayPointers, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return displayPointers;
    }
    
    
    IConfigScadaDisplay* ScadaDisplayAccessFactory::createScadaDisplay()
    {            
        return new ConfigScadaDisplay();    
    }


    IConfigScadaDisplay* ScadaDisplayAccessFactory::copyScadaDisplay(const IConfigScadaDisplay* displayToCopy)
    {
        FUNCTION_ENTRY("copyScadaDisplay");

        TA_ASSERT(displayToCopy !=  NULL, "The display to copy was NULL");
        
        const ConfigScadaDisplay* cast = dynamic_cast<const ConfigScadaDisplay*>(displayToCopy);

        TA_ASSERT(cast != NULL, "ScadaDisplay passed could not be converted into a ConfigScadaDisplay");

        FUNCTION_EXIT;
        return new ConfigScadaDisplay(*cast);

    }


} // closes TA_Base_Core


