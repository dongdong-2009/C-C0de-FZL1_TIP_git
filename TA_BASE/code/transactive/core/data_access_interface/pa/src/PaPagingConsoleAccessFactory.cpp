/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaPagingConsoleAccessFactory.cpp $
  * @author jian.huang
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaPagingConsoleAccessFactory is a singleton that is used to retrieve PaPagingConsole objects either from the
  * database or newly created.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/PaPagingConsoleAccessFactory.h"

namespace TA_Base_Core
{
    PaPagingConsoleAccessFactory* PaPagingConsoleAccessFactory::m_instance = NULL;
    const char* const PA_PAGING_CONSOLE_TABLE = "PA_PAGING_CONSOLE";
    const char* const COL_LOCATION_KEY = "LOCATION_KEY";
    const char* const COL_PAGING_CONSOLE_NAME = "PAGING_CONSOLE_NAME";
    const char* const COL_PAGING_CONSOLE_ID = "PAGING_CONSOLE_ID";

    PaPagingConsoleAccessFactory& TA_Base_Core::PaPagingConsoleAccessFactory::getInstance()
    {
        if( NULL == m_instance )
        {
            m_instance = new PaPagingConsoleAccessFactory();
        }
        return *m_instance;
    }

    void TA_Base_Core::PaPagingConsoleAccessFactory::removeInstance()
    {
        if ( m_instance != NULL )
        {
            delete m_instance;
            m_instance = NULL;
        }
    }

    std::map<unsigned short, std::string> TA_Base_Core::PaPagingConsoleAccessFactory::getPaPagingConsolesByLocationKey( unsigned long ulLocationKey )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string
		/*
        std::ostringstream ossSql;
        ossSql << "select " << COL_PAGING_CONSOLE_ID << ", " << COL_PAGING_CONSOLE_NAME
            << " from " << PA_PAGING_CONSOLE_TABLE
            << " where " 
            << COL_LOCATION_KEY << " = " << ulLocationKey;
		*/
//		std::string strSql = databaseConnection->prepareSQLStatement(PA_PAGING_CONSOLE_SELECT_76001,ulLocationKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_PAGING_CONSOLE_SELECT_76001,ulLocationKey);
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(COL_PAGING_CONSOLE_ID);
        columnNames.push_back(COL_PAGING_CONSOLE_NAME);

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

        std::map<unsigned short, std::string> mapPaPagingConsoles;
        for ( unsigned long ulLoop = 0; ulLoop < data->getNumRows(); ++ulLoop )
        {
            mapPaPagingConsoles[data->getUnsignedLongData( ulLoop, COL_PAGING_CONSOLE_ID )] = 
                data->getStringData( ulLoop, COL_PAGING_CONSOLE_NAME );
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return mapPaPagingConsoles;
    }

    PaPagingConsoleAccessFactory::PaPagingConsoleAccessFactory()
    {

    }

    PaPagingConsoleAccessFactory::~PaPagingConsoleAccessFactory()
    {

    }

}