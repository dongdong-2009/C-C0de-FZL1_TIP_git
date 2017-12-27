/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaStationAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaStationAccessFactory is a singleton that is used to retrieve PaStation objects either from the
  * database or newly created. All PaStation objects returned will adhere to the IPaStation interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa/src/PaStationAccessFactory.h"

#include "core/data_access_interface/pa/src/PaStation.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaStation.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaStationAccessFactory* PaStationAccessFactory::m_instance = 0;

    static const std::string KEY_COL_NAME = "PASTAT_ID";
    static const std::string PA_STATION_TABLE = "PA_STATION";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";


    PaStationAccessFactory& PaStationAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaStationAccessFactory();
        }
        return *m_instance;
    }


    void PaStationAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaStation* PaStationAccessFactory::getPaStation(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string to retrieve the name of the PaStation
        // this is a check to ensure an PaStation with the specified
        // PAZONE_ID actually exists.        
       /* std::ostringstream sql;
        sql << "select " << KEY_COL_NAME 
            << " from " << PA_STATION_TABLE
            << " where " << KEY_COL_NAME << " = " << key; */ 
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_STATION_STD_SELECT_43501, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_STATION_SELECT_43501, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_STATION_SELECT_43501, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL_NAME);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "No data found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "More than one entry found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the PaStation object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaStation* thePaStation;
        
        // decide which PaStation object to return
        if (readWrite) // need a config PaStation
        {
            thePaStation = new ConfigPaStation(key);
        }
        else // need a standard PaStation
        {
            thePaStation = new PaStation(key);
        }

        // Return the PaStation pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaStation;
    }


    IPaStations PaStationAccessFactory::getAllPaStations(const bool readWrite )
    {
        return getPaStationsReturnedFromStatement("", readWrite);
    }


    IPaStations PaStationAccessFactory::getPaStationsByLocationKey(unsigned long locationKey, const bool readWrite )
    {
        std::ostringstream sql;
        sql << LOCATIONKEY_COL << " = " << locationKey;

        return getPaStationsReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaStations PaStationAccessFactory::getPaStationsReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaStations, ConfigPaStation, PaStation>
                    (PaStationHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaStation* PaStationAccessFactory::createPaStation()
    {            
        return new ConfigPaStation();    
    }


    IConfigPaStation* PaStationAccessFactory::copyPaStation(const IConfigPaStation* paStationToCopy)
    {
        FUNCTION_ENTRY("copyPaStation");

        TA_ASSERT(paStationToCopy !=  NULL, "The PaStation to copy was NULL");
        
        const ConfigPaStation* cast = dynamic_cast<const ConfigPaStation*>(paStationToCopy);

        TA_ASSERT(cast != NULL, "PaStation passed could not be converted into a ConfigPaStation");

        FUNCTION_EXIT;
        return new ConfigPaStation(*cast);

    }

} // closes TA_Base_Core


