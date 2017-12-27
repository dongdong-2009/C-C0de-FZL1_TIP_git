/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaSysParamAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaSysParamAccessFactory is a singleton that is used to retrieve PaSysParam objects either from the
  * database or newly created. All PaSysParam objects returned will adhere to the IPaSysParam interface.
  */



#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/PaSysParamAccessFactory.h"


namespace TA_Base_Core
{
    PaSysParamAccessFactory* PaSysParamAccessFactory::m_instance = NULL;

    const char* const KEY_COL_NAME = "PASTAT_ID";
    const char* const PA_HARDWARE_PARAMETER_TABLE = "PA_HARDWARE_PARAMETER";
    const char* const COL_LOCATION_KEY = "LOCATION_KEY";
    const char* const COL_PARAMETER_NAME = "PARAMETER_NAME";
    const char* const COL_PARAMETER_VALUE = "PARAMETER_VALUE";

    PaSysParamAccessFactory& PaSysParamAccessFactory::getInstance()
    {
        if( NULL == m_instance )
        {
            m_instance = new PaSysParamAccessFactory();
        }
        return *m_instance;
    }

    void PaSysParamAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }

    PaSysParamAccessFactory::PaSysParamAccessFactory()
    {
    }

    PaSysParamAccessFactory::~PaSysParamAccessFactory()
    {
    }

    std::string PaSysParamAccessFactory::getPaSysParamValue( 
        const unsigned long ulLocationKey, const std::string& strParameterName )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string
		/*
        std::ostringstream ossSql;
        ossSql << "select " << COL_PARAMETER_VALUE 
            << " from " << PA_HARDWARE_PARAMETER_TABLE
            << " where " 
            << COL_LOCATION_KEY << " = " << ulLocationKey
            << " and "
            << COL_PARAMETER_NAME << " = '" 
            << databaseConnection->escapeQueryString(strParameterName) << "'";
		*/


//		std::string strSql = databaseConnection->prepareSQLStatement(PA_HARDWARE_PARAMETER_SELECT_77001,ulLocationKey,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_HARDWARE_PARAMETER_SELECT_77001,ulLocationKey,
			 databaseConnection->escapeQueryString(strParameterName));

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(COL_PARAMETER_VALUE);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
            sprintf(reasonMessage, "No data found for location key = %lu",ulLocationKey);			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
            sprintf(reasonMessage, "More than one entry found for location key = %lu",ulLocationKey);			
            TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        std::string strValue = data->getStringData( 0, COL_PARAMETER_VALUE );

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return strValue;
    }

    void PaSysParamAccessFactory::setPaSysParamValue( const unsigned long ulLocationKey, 
        const std::string& strParamName, const std::string& strParamValue )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Write);

        // create the SQL string
		/*
        std::ostringstream ossSql;
        ossSql << "update " << PA_HARDWARE_PARAMETER_TABLE
            << " set " << COL_PARAMETER_VALUE << " = '"
            << databaseConnection->escapeInsertString(strParamValue) << "'"
            << " where " 
            << COL_LOCATION_KEY << " = " << ulLocationKey
            << " and "
            << COL_PARAMETER_NAME << " = '" 
            << databaseConnection->escapeQueryString(strParamName) << "'";
        */
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_HARDWARE_PARAMETER_UPDATE_77002, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_HARDWARE_PARAMETER_UPDATE_77002,
			databaseConnection->escapeInsertString(strParamValue), ulLocationKey,
			databaseConnection->escapeQueryString(strParamName));
        // Execute the modification
        databaseConnection->executeModification(strSql);
    }

    std::map<std::string, std::string> PaSysParamAccessFactory::getPaSysParamsByLocationKey( unsigned long ulLocationKey )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string
		/*
        std::ostringstream ossSql;
        ossSql << "select " << COL_PARAMETER_NAME << ", " << COL_PARAMETER_VALUE
            << " from " << PA_HARDWARE_PARAMETER_TABLE
            << " where " 
            << COL_LOCATION_KEY << " = " << ulLocationKey;
		*/

//		std::string strSql = databaseConnection->prepareSQLStatement(PA_HARDWARE_PARAMETER_SELECT_77003,ulLocationKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_HARDWARE_PARAMETER_SELECT_77003,ulLocationKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(COL_PARAMETER_NAME);
        columnNames.push_back(COL_PARAMETER_VALUE);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
            sprintf(reasonMessage, "No data found for location key = %lu",ulLocationKey);			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }

        std::string strName = "";
        std::string strValue = "";
        std::map<std::string, std::string> mapPaSysParams;
        for ( unsigned long ulLoop = 0; ulLoop < data->getNumRows(); ++ulLoop )
        {
            strName = data->getStringData( ulLoop, COL_PARAMETER_NAME );
            strValue = data->getStringData( ulLoop, COL_PARAMETER_VALUE );
            mapPaSysParams.insert( std::map<std::string, std::string>::value_type(strName, strValue) );
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return mapPaSysParams;
    }
} // closes TA_Base_Core


