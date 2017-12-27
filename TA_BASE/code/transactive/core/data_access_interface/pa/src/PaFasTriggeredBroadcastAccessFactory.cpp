/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/pa/src/PaFasTriggeredBroadcastAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * PaFasTriggeredBroadcastAccessFactory is a singleton that is used to retrieve PaFasTriggeredBroadcast objects either from the
  * database or newly created. All PaFasTriggeredBroadcast objects returned will adhere to the IPaFasTriggeredBroadcast interface.
  */
#pragma warning(disable:4786 4290)


#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcastAccessFactory.h"

#include "core/data_access_interface/pa/src/PaFasTriggeredBroadcast.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaFasTriggeredBroadcast.h"

#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    PaFasTriggeredBroadcastAccessFactory* PaFasTriggeredBroadcastAccessFactory::m_instance = 0;

    static const std::string KEY_COL_NAME = "PAFASBR_ID";
    static const std::string PA_FAS_TRIG_BCAST_TABLE_NAME = "PA_FAS_TRIGGERED_BROADCAST";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    PaFasTriggeredBroadcastAccessFactory& PaFasTriggeredBroadcastAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaFasTriggeredBroadcastAccessFactory();
        }
        return *m_instance;
    }


    void PaFasTriggeredBroadcastAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaFasTriggeredBroadcast* PaFasTriggeredBroadcastAccessFactory::getPaFasTriggeredBroadcast
    (
        const unsigned long key,
        const bool readWrite
    )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Pa_Cd, Read);

        // create the SQL string to retrieve the name of the PaFasTriggeredBroadcast
        // this is a check to ensure an PaFasTriggeredBroadcast with the specified
        // PAZONE_ID actually exists.        
       /* std::ostringstream sql;
        sql << "select " << KEY_COL_NAME 
            << " from " << PA_FAS_TRIG_BCAST_TABLE_NAME
            << " where " << KEY_COL_NAME << " = " << key; */ 

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PA_FAS_TRIGGERED_BROADCAST_SELECT_108001, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL_NAME);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql, columnNames);
        
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

        // Create the PaFasTriggeredBroadcast object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaFasTriggeredBroadcast* thePaFasTriggeredBroadcast;
        
        // decide which PaFasTriggeredBroadcast object to return
        if (readWrite) // need a config PaFasTriggeredBroadcast
        {
            thePaFasTriggeredBroadcast = new ConfigPaFasTriggeredBroadcast(key);
        }
        else // need a standard PaFasTriggeredBroadcast
        {
            thePaFasTriggeredBroadcast = new PaFasTriggeredBroadcast(key);
        }

        // Return the PaFasTriggeredBroadcast pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaFasTriggeredBroadcast;
    }


    IPaFasTriggeredBroadcasts PaFasTriggeredBroadcastAccessFactory::getAllPaFasTriggeredBroadcasts( const bool readWrite)
    {
        return getPaFasTriggeredBroadcastsReturnedFromStatement("", readWrite);
    }


    IPaFasTriggeredBroadcasts PaFasTriggeredBroadcastAccessFactory::getPaFasTriggeredBroadcastsByLocationKey
    (
     unsigned long locationKey, 
     const bool readWrite 
    )
    {
        std::ostringstream sql;
        sql << LOCATIONKEY_COL << " = " << locationKey;

        return getPaFasTriggeredBroadcastsReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaFasTriggeredBroadcasts PaFasTriggeredBroadcastAccessFactory::getPaFasTriggeredBroadcastsReturnedFromStatement
    (
     const std::string& whereClause,
     const bool readWrite
    )
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaFasTriggeredBroadcasts, ConfigPaFasTriggeredBroadcast, PaFasTriggeredBroadcast>
                        (PaFasTriggeredBroadcastHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaFasTriggeredBroadcast* PaFasTriggeredBroadcastAccessFactory::createPaFasTriggeredBroadcast()
    {            
        return new ConfigPaFasTriggeredBroadcast();    
    }


    IConfigPaFasTriggeredBroadcast* PaFasTriggeredBroadcastAccessFactory::copyPaFasTriggeredBroadcast(const IConfigPaFasTriggeredBroadcast* PaFasTriggeredBroadcastToCopy)
    {
        FUNCTION_ENTRY("copyPaFasTriggeredBroadcast");

        TA_ASSERT(PaFasTriggeredBroadcastToCopy !=  NULL, "The PaFasTriggeredBroadcast to copy was NULL");
        
        const ConfigPaFasTriggeredBroadcast* cast = dynamic_cast<const ConfigPaFasTriggeredBroadcast*>(PaFasTriggeredBroadcastToCopy);

        TA_ASSERT(cast != NULL, "PaFasTriggeredBroadcast passed could not be converted into a ConfigPaFasTriggeredBroadcast");

        FUNCTION_EXIT;
        return new ConfigPaFasTriggeredBroadcast(*cast);

    }
} // closes TA_Base_Core


