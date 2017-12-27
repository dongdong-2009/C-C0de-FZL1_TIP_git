/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaScheduleBroadcastAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * PaScheduleBroadcastAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */
#pragma warning(disable:4786 4290)

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/PaScheduleBroadcast.h"
#include "core/data_access_interface/pa/src/ConfigPaScheduleBroadcast.h"
#include "core/data_access_interface/pa/src/PaScheduleBroadcastAccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    PaScheduleBroadcastAccessFactory* PaScheduleBroadcastAccessFactory::m_instance = 0;

    PaScheduleBroadcastAccessFactory& PaScheduleBroadcastAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaScheduleBroadcastAccessFactory();
        }
        return *m_instance;
    }

    void PaScheduleBroadcastAccessFactory::removeInstance( )
    {
	    if ( m_instance != 0 )
	    {
		    delete m_instance;
		    m_instance = 0;
	    }
    }

    IPaScheduleBroadcast* PaScheduleBroadcastAccessFactory::getPaScheduleBroadcast(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the PaDvaMessage
        // this is a check to ensure an PaDvaMessage with the specified
        // PAZONE_ID actually exists.        
        /*std::ostringstream sql;
        sql << "select " << KEY_COL 
            << " from " << PA_SCHEDULE_BROADCAST_TABLE
            << " where " << KEY_COL << " = " << key;*/     

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PA_SCHEDULE_BROADCAST_SELECT_110001, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

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

        // Create the PaDvaMessage object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaScheduleBroadcast* thePaScheduleBroadcast;
        
        // decide which PaDvaMessage object to return
        if (readWrite) // need a config PaDvaMessage
        {
            thePaScheduleBroadcast = new ConfigPaScheduleBroadcast(key);
        }
        else // need a standard PaDvaMessage
        {
            thePaScheduleBroadcast = new PaScheduleBroadcast(key);
        }

        // Return the PaDvaMessage pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaScheduleBroadcast;
    }

    IPaScheduleBroadcasts PaScheduleBroadcastAccessFactory::getAllPaScheduleBroadcasts(const bool readWrite )
    {
        return getPaScheduleBroadcastsReturnedFromStatement("", readWrite);
    }

    IPaScheduleBroadcasts PaScheduleBroadcastAccessFactory::getPaScheduleBroadcastsByLocationKey(unsigned long locationKey, bool bRequireEnable, bool readWrite )
    {
        std::ostringstream sql;
        sql << LOCATION_KEY_COL << " = " << locationKey;
        if ( bRequireEnable )
        {
            sql << " and " << PA_SCHEDULE_BROADCAST_IS_ENABLE_COL << " = 1";
        }

        return getPaScheduleBroadcastsReturnedFromStatement(sql.str(), readWrite);   
    }

    IPaScheduleBroadcasts PaScheduleBroadcastAccessFactory::getPaScheduleBroadcastsReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
        DatabaseQueryHelper::QueryData result;
        
        result.primaryKeyCol = KEY_COL;
        result.tableName = PA_SCHEDULE_BROADCAST_TABLE;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATION_KEY_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_SCHEDULE_NAME_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_START_TIME_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_END_TIME_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_INTERVAL_TYPE_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_IS_ENABLE_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_AUTO_DEL_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_DVA_MSG_KEY_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_CYCLIC_TIMES_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_PRIORY_GROUP_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_SOURCE_TYPE_COL);
        result.columnNames.push_back(PA_SCHEDULE_BROADCAST_STATUS_COL);
        
        DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement, result.columnNames, 
                                                        result.tableName, false );

        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaScheduleBroadcasts, ConfigPaScheduleBroadcast, PaScheduleBroadcast>
                    (result, whereClause, readWrite);
    }

    IConfigPaScheduleBroadcast* PaScheduleBroadcastAccessFactory::createPaScheduleBroadcast()
    {
        return new ConfigPaScheduleBroadcast();    
    }

    IConfigPaScheduleBroadcast* PaScheduleBroadcastAccessFactory::copyPaScheduleBroadcast(const IConfigPaScheduleBroadcast* PaScheduleBroadcastToCopy)
    {
        FUNCTION_ENTRY("copyPaScheduleBroadcast");

        TA_ASSERT(PaScheduleBroadcastToCopy !=  NULL, "The PaScheduleBroadcast to copy was NULL");
        
        const ConfigPaScheduleBroadcast* cast = dynamic_cast<const ConfigPaScheduleBroadcast*>(PaScheduleBroadcastToCopy);

        TA_ASSERT(cast != NULL, "PaScheduleBroadcast passed could not be converted into a ConfigPaScheduleBroadcast");

        FUNCTION_EXIT;
        return new ConfigPaScheduleBroadcast(*cast);

    }

} // closes TA_IRS_Core


