/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/PaAtsTriggeredBroadcastAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PaAtsTriggeredBroadcastAccessFactory is a singleton that is used to retrieve PaAtsTriggeredBroadcast objects either from the
  * database or newly created. All PaAtsTriggeredBroadcast objects returned will adhere to the IPaAtsTriggeredBroadcast interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcastAccessFactory.h"

#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcast.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaAtsTriggeredBroadcast.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaAtsTriggeredBroadcastAccessFactory* PaAtsTriggeredBroadcastAccessFactory::m_instance = 0;

    static const std::string KEY_COL_NAME = "PAATBR_ID";
    static const std::string PA_ATS_TRIG_BCAST_TABLE_NAME = "PA_ATS_TRIGGERED_BROADCAST";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";
    
    static const std::string ATS_PLATFORM_TABLE = "AT_PLATFORM";
    static const std::string PLATFORM_KEY_COL = "ATPLAT_ID";
    static const std::string PLATFORM_ID_COL = "PLATFORM_ID";
    static const std::string PLATFORM_NAME_COL = "PLATFORM_NAME";
    static const std::string PLATFORM_STATION_ID_COL = "STATION_ID";

    PaAtsTriggeredBroadcastAccessFactory& PaAtsTriggeredBroadcastAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaAtsTriggeredBroadcastAccessFactory();
        }
        return *m_instance;
    }


    void PaAtsTriggeredBroadcastAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaAtsTriggeredBroadcast* PaAtsTriggeredBroadcastAccessFactory::getPaAtsTriggeredBroadcast
    (
        const unsigned long key,
        const bool readWrite
    )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string to retrieve the name of the PaAtsTriggeredBroadcast
        // this is a check to ensure an PaAtsTriggeredBroadcast with the specified
        // PAZONE_ID actually exists.        
       /* std::ostringstream sql;
        sql << "select " << KEY_COL_NAME 
            << " from " << PA_ATS_TRIG_BCAST_TABLE_NAME
            << " where " << KEY_COL_NAME << " = " << key;  */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), PA_ATS_TRIGGERED_BROADCAST_STD_SELECT_45001, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(PA_ATS_TRIGGERED_BROADCAST_SELECT_45001, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PA_ATS_TRIGGERED_BROADCAST_SELECT_45001, key);

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

        // Create the PaAtsTriggeredBroadcast object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaAtsTriggeredBroadcast* thePaAtsTriggeredBroadcast;
        
        // decide which PaAtsTriggeredBroadcast object to return
        if (readWrite) // need a config PaAtsTriggeredBroadcast
        {
            thePaAtsTriggeredBroadcast = new ConfigPaAtsTriggeredBroadcast(key);
        }
        else // need a standard PaAtsTriggeredBroadcast
        {
            thePaAtsTriggeredBroadcast = new PaAtsTriggeredBroadcast(key);
        }

        // Return the PaAtsTriggeredBroadcast pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaAtsTriggeredBroadcast;
    }


    IPaAtsTriggeredBroadcasts PaAtsTriggeredBroadcastAccessFactory::getAllPaAtsTriggeredBroadcasts( const bool readWrite)
    {
        return getPaAtsTriggeredBroadcastsReturnedFromStatement("", readWrite);
    }


    IPaAtsTriggeredBroadcasts PaAtsTriggeredBroadcastAccessFactory::getPaAtsTriggeredBroadcastsByLocationKey
    (
     unsigned long locationKey, 
     const bool readWrite 
    )
    {
        std::ostringstream sql;
        sql << LOCATIONKEY_COL << " = " << locationKey;

        return getPaAtsTriggeredBroadcastsReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaAtsTriggeredBroadcasts PaAtsTriggeredBroadcastAccessFactory::getPaAtsTriggeredBroadcastsReturnedFromStatement
    (
     const std::string& whereClause,
     const bool readWrite
    )
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaAtsTriggeredBroadcasts, ConfigPaAtsTriggeredBroadcast, PaAtsTriggeredBroadcast>
                        (PaAtsTriggeredBroadcastHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaAtsTriggeredBroadcast* PaAtsTriggeredBroadcastAccessFactory::createPaAtsTriggeredBroadcast()
    {            
        return new ConfigPaAtsTriggeredBroadcast();    
    }


    IConfigPaAtsTriggeredBroadcast* PaAtsTriggeredBroadcastAccessFactory::copyPaAtsTriggeredBroadcast(const IConfigPaAtsTriggeredBroadcast* paAtsTriggeredBroadcastToCopy)
    {
        FUNCTION_ENTRY("copyPaAtsTriggeredBroadcast");

        TA_ASSERT(paAtsTriggeredBroadcastToCopy !=  NULL, "The PaAtsTriggeredBroadcast to copy was NULL");
        
        const ConfigPaAtsTriggeredBroadcast* cast = dynamic_cast<const ConfigPaAtsTriggeredBroadcast*>(paAtsTriggeredBroadcastToCopy);

        TA_ASSERT(cast != NULL, "PaAtsTriggeredBroadcast passed could not be converted into a ConfigPaAtsTriggeredBroadcast");

        FUNCTION_EXIT;
        return new ConfigPaAtsTriggeredBroadcast(*cast);

    }
        
    PaAtsTriggeredBroadcastAccessFactory::PlatformIds 
    PaAtsTriggeredBroadcastAccessFactory::enumeratePlatformIds()
    {
        std::vector<std::string> columnNames;
        columnNames.push_back(PLATFORM_KEY_COL);
        columnNames.push_back(PLATFORM_ID_COL);
        columnNames.push_back(PLATFORM_NAME_COL);
        columnNames.push_back(PLATFORM_STATION_ID_COL);

        /*std::string query = TA_Base_Core::DatabaseQueryHelper::createBasicSelectStatement(
                                        columnNames, ATS_PLATFORM_TABLE, false);*/

		SQLStatement sqlStatement;
		TA_Base_Core::DatabaseQueryHelper::createBasicSelectStatement(sqlStatement,
			columnNames, ATS_PLATFORM_TABLE, false);
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);
        //std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(query, columnNames));
     

		//waitforcheck delete by lin
		/*SQLStatement strSql;
		strSql.strCommon = query;*/
		std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sqlStatement, columnNames));


     
        PlatformIds result;
          
        if (data.get() == 0)
        {
            // Return empty set in the event of no rows
            return result;
        }
        
        // Fill in resulting structures
        for (unsigned int i = 0; i < data->getNumRows(); ++i)
        {
            PlatformId msg;

            msg.pKey            = data->getUnsignedLongData(i, PLATFORM_KEY_COL);
            msg.platformId      = data->getUnsignedLongData(i, PLATFORM_ID_COL);

            msg.platformName    = data->getStringData(i, PLATFORM_NAME_COL);
            msg.stationId       = data->getUnsignedLongData(i, PLATFORM_STATION_ID_COL);  

            result.push_back(msg);      
        }

        return result;
    }
} // closes TA_Base_Core


