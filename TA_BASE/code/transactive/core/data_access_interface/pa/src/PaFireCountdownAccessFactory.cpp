/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaFireCountdownAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaFireCountdownAccessFactory is a singleton that is used to retrieve PaSysParam objects either from the
  * database or newly created. All PaSysParam objects returned will adhere to the IPaSysParam interface.
  */



#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/PaFireCountdownAccessFactory.h"


namespace TA_Base_Core
{
    PaFireCountdownAccessFactory* PaFireCountdownAccessFactory::m_instance = NULL;

    const char* const PA_FIRE_COUNTDOWN_TABLE = "PA_FIRE_COUNTDOWN";
    const char* const PA_STATION_TABLE = "PA_STATION";
    const char* const COL_PA_STATION_LOCATION_KEY = "PA_STATION.LOCATIONKEY";
    const char* const COL_LOCATION_KEY = "LOCATION_KEY";
    const char* const COL_SATION_ID = "PASTAT_ID";
    const char* const COL_STN_TIMER_NAME = "STN_TIMER_NAME";
    const char* const COL_OCC_TIMER_NAME = "OCC_TIMER_NAME";
    const char* const COL_STN_ABORT_NAME = "STN_ABORT_NAME";
    const char* const COL_OCC_ABORT_NAME = "OCC_ABORT_NAME";

    PaFireCountdownAccessFactory& PaFireCountdownAccessFactory::getInstance()
    {
        if( NULL == m_instance )
        {
            m_instance = new PaFireCountdownAccessFactory();
        }
        return *m_instance;
    }

    void PaFireCountdownAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }

    PaFireCountdownAccessFactory::PaFireCountdownAccessFactory()
    {
    }

    PaFireCountdownAccessFactory::~PaFireCountdownAccessFactory()
    {
    }

    std::map<std::string, unsigned short> PaFireCountdownAccessFactory::getStnPaFireTimers( unsigned long ulLocationKey )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string
        std::ostringstream ossSql;
		/*
        ossSql << "select " << PA_STATION_TABLE << "." << COL_SATION_ID << ", " 
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_STN_TIMER_NAME << ", " 
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_OCC_TIMER_NAME
            << " from " << PA_FIRE_COUNTDOWN_TABLE << ", " << PA_STATION_TABLE
            << " where " 
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_LOCATION_KEY << " = " << ulLocationKey 
            << " and "  
            << COL_PA_STATION_LOCATION_KEY << " = "
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_LOCATION_KEY
            << " and "
            << COL_PA_STATION_LOCATION_KEY << " = " << ulLocationKey;
		*/

//		std::string strSql = databaseConnection->prepareSQLStatement(PA_FIRE_COUNTDOWN_SELECT_STCFIRETIME_75001,ulLocationKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_FIRE_COUNTDOWN_SELECT_STCFIRETIME_75001,ulLocationKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(COL_SATION_ID);
        columnNames.push_back(COL_STN_TIMER_NAME);
        columnNames.push_back(COL_OCC_TIMER_NAME);

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


        std::map<std::string, unsigned short> mapPaFireTimers;
        std::string strStnTimerName = "";
        unsigned short usStationId = 0u;
        for ( unsigned long ulLoop = 0; ulLoop < data->getNumRows(); ++ulLoop )
        {
            usStationId = static_cast<unsigned short>(data->getUnsignedLongData( ulLoop, COL_SATION_ID ));
            strStnTimerName = data->getStringData( ulLoop, COL_STN_TIMER_NAME ).c_str();
            mapPaFireTimers[strStnTimerName] = usStationId;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return mapPaFireTimers;
    }

    std::map<std::string, unsigned short> PaFireCountdownAccessFactory::getOccPaFireTimers()
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string
//         std::ostringstream ossSql;
//         ossSql << "select " << PA_STATION_TABLE << "." << COL_SATION_ID << ", " 
//             << PA_FIRE_COUNTDOWN_TABLE << "." << COL_STN_TIMER_NAME << ", " 
//             << PA_FIRE_COUNTDOWN_TABLE << "." << COL_OCC_TIMER_NAME
//             << " from " << PA_FIRE_COUNTDOWN_TABLE << ", " << PA_STATION_TABLE
//             << " where " 
//             << COL_PA_STATION_LOCATION_KEY << " = "
//             << PA_FIRE_COUNTDOWN_TABLE << "." << COL_LOCATION_KEY;

//		std::string strSql = databaseConnection->prepareSQLStatement(PA_FIRE_COUNTDOWN_SELECT_OCCFIRETIME_75002);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_FIRE_COUNTDOWN_SELECT_OCCFIRETIME_75002);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(COL_SATION_ID);
        columnNames.push_back(COL_STN_TIMER_NAME);
        columnNames.push_back(COL_OCC_TIMER_NAME);

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
            sprintf(reasonMessage, "No data found");			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }

        std::map<std::string, unsigned short> mapPaFireTimers;
        std::string strOccTimerName = "";
        unsigned short usStationId = 0u;
        for ( unsigned long ulLoop = 0; ulLoop < data->getNumRows(); ++ulLoop )
        {
            usStationId = static_cast<unsigned short>(data->getUnsignedLongData( ulLoop, COL_SATION_ID ));
            strOccTimerName = data->getStringData( ulLoop, COL_OCC_TIMER_NAME ).c_str();
            mapPaFireTimers[strOccTimerName] = usStationId;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return mapPaFireTimers;
    }

    std::map<std::string, unsigned short> PaFireCountdownAccessFactory::getStnPaFireAborts( unsigned long ulLocationKey )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string
		/*
        std::ostringstream ossSql;
        ossSql << "select " << PA_STATION_TABLE << "." << COL_SATION_ID << ", " 
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_STN_ABORT_NAME << ", " 
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_OCC_ABORT_NAME
            << " from " << PA_FIRE_COUNTDOWN_TABLE << ", " << PA_STATION_TABLE
            << " where " 
            << COL_PA_STATION_LOCATION_KEY << " = "
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_LOCATION_KEY
            << " and "
            << COL_PA_STATION_LOCATION_KEY << " = " << ulLocationKey;
		*/


//		std::string strSql = databaseConnection->prepareSQLStatement(PA_FIRE_COUNTDOWN_SELECT_STCFIREABORT_75003,ulLocationKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_FIRE_COUNTDOWN_SELECT_STCFIREABORT_75003,ulLocationKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(COL_SATION_ID);
        columnNames.push_back(COL_STN_ABORT_NAME);
        columnNames.push_back(COL_OCC_ABORT_NAME);

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
            sprintf(reasonMessage, "No data found");			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }

        std::map<std::string, unsigned short> mapPaFireAborts;
        std::string strStnAbortName = "";
        unsigned short usStationId = 0u;
        for ( unsigned long ulLoop = 0; ulLoop < data->getNumRows(); ++ulLoop )
        {
            usStationId = static_cast<unsigned short>(data->getUnsignedLongData( ulLoop, COL_SATION_ID ));
            strStnAbortName = data->getStringData( ulLoop, COL_STN_ABORT_NAME ).c_str();
            mapPaFireAborts[strStnAbortName] = usStationId;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return mapPaFireAborts;
    }

    std::map<std::string, unsigned short> PaFireCountdownAccessFactory::getOccPaFireAborts()
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string
		/*
        std::ostringstream ossSql;
        ossSql << "select " << PA_STATION_TABLE << "." << COL_SATION_ID << ", " 
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_STN_ABORT_NAME << ", " 
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_OCC_ABORT_NAME
            << " from " << PA_FIRE_COUNTDOWN_TABLE << ", " << PA_STATION_TABLE
            << " where " 
            << COL_PA_STATION_LOCATION_KEY << " = "
            << PA_FIRE_COUNTDOWN_TABLE << "." << COL_LOCATION_KEY;
		*/
//        std::string strSql = databaseConnection->prepareSQLStatement(PA_FIRE_COUNTDOWN_SELECT_OCCFIREABORT_75004);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_FIRE_COUNTDOWN_SELECT_OCCFIREABORT_75004);
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(COL_SATION_ID);
        columnNames.push_back(COL_STN_ABORT_NAME);
        columnNames.push_back(COL_OCC_ABORT_NAME);

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
            sprintf(reasonMessage, "No data found");			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }

        std::map<std::string, unsigned short> mapPaFireAborts;
        std::string strOccAbortName = "";
        unsigned short usStationId = 0u;
        for ( unsigned long ulLoop = 0; ulLoop < data->getNumRows(); ++ulLoop )
        {
            usStationId = static_cast<unsigned short>(data->getUnsignedLongData( ulLoop, COL_SATION_ID ));
            strOccAbortName = data->getStringData( ulLoop, COL_OCC_ABORT_NAME ).c_str();
            mapPaFireAborts[strOccAbortName] = usStationId;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        return mapPaFireAborts;
    }

} // closes TA_Base_Core


