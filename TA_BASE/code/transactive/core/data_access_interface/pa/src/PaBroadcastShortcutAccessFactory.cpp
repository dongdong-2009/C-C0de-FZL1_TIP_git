/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaBroadcastShortcutAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * PaBroadcastShortcutAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */
#pragma warning(disable:4786 4290)

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/PaBroadcastShortcut.h"
#include "core/data_access_interface/pa/src/ConfigPaBroadcastShortcut.h"
#include "core/data_access_interface/pa/src/PaBroadcastShortcutAccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    PaBroadcastShortcutAccessFactory* PaBroadcastShortcutAccessFactory::m_instance = 0;

    PaBroadcastShortcutAccessFactory& PaBroadcastShortcutAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaBroadcastShortcutAccessFactory();
        }
        return *m_instance;
    }


    void PaBroadcastShortcutAccessFactory::removeInstance( )
    {
	    if ( m_instance != 0 )
	    {
		    delete m_instance;
		    m_instance = 0;
	    }
    }
    
    IPaBroadcastShortcut* PaBroadcastShortcutAccessFactory::getPaBroadcastShortcutByLocationKeyAndShortcutNo(
        unsigned long locationKey, unsigned long ulShortcutNo, bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the PaDvaMessage
        // this is a check to ensure an PaDvaMessage with the specified
        // PAZONE_ID actually exists.
        /*std::ostringstream sql;
        sql << "select " << KEY_COL << ", "
            << LOCATION_KEY_COL << ", "
            << SHORTCUT_NO_COL << ", "
            << DVA_MSG_KEY_COL
            << " from " << PA_BROADCAST_SHORTCUT_TABLE
            << " where " << SHORTCUT_NO_COL << " = " << ulShortcutNo
            << " and " << LOCATION_KEY_COL << " = " << locationKey;*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PA_DVA_MSG_SHORTCUT_CONFIG_SELECT_107001, ulShortcutNo, locationKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(LOCATION_KEY_COL);
        columnNames.push_back(SHORTCUT_NO_COL);
        columnNames.push_back(DVA_MSG_KEY_COL);
        
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
            sprintf(reasonMessage, "No data found for locationKey = %lu and ShortcutNo = %lu",locationKey, ulShortcutNo);			
            TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;
            
            char reasonMessage[256];
            sprintf(reasonMessage, "More than one entry found for locationKey = %lu and ShortcutNo = %lu",locationKey, ulShortcutNo);		
            TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        
        // Create the PaDvaMessage object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaBroadcastShortcut* thePaBroadcastShortcut;
        
        // decide which PaDvaMessage object to return
        if (readWrite) // need a config PaDvaMessage
        {
            thePaBroadcastShortcut = new ConfigPaBroadcastShortcut(0, *data);
        }
        else // need a standard PaDvaMessage
        {
            thePaBroadcastShortcut = new PaBroadcastShortcut(0, *data);
        }

        delete data;
        data = NULL;
        
        // Return the PaDvaMessage pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaBroadcastShortcut;
    }

    IPaBroadcastShortcut* PaBroadcastShortcutAccessFactory::getPaBroadcastShortcut(const unsigned long key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the PaDvaMessage
        // this is a check to ensure an PaDvaMessage with the specified
        // PAZONE_ID actually exists.        
        /*std::ostringstream sql;
        sql << "select " << KEY_COL 
            << " from " << PA_BROADCAST_SHORTCUT_TABLE
            << " where " << KEY_COL << " = " << key; */ 

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PA_DVA_MSG_SHORTCUT_CONFIG_SELECT_107002, key);


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
        IPaBroadcastShortcut* thePaBroadcastShortcut;
        
        // decide which PaDvaMessage object to return
        if (readWrite) // need a config PaDvaMessage
        {
            thePaBroadcastShortcut = new ConfigPaBroadcastShortcut(key);
        }
        else // need a standard PaDvaMessage
        {
            thePaBroadcastShortcut = new PaBroadcastShortcut(key);
        }

        // Return the PaDvaMessage pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaBroadcastShortcut;
    }


    IPaBroadcastShortcuts PaBroadcastShortcutAccessFactory::getAllPaBroadcastShortcuts(const bool readWrite )
    {
        return getPaBroadcastShortcutsReturnedFromStatement("", readWrite);
    }


    IPaBroadcastShortcuts PaBroadcastShortcutAccessFactory::getPaBroadcastShortcutsByLocationKey(unsigned long locationKey, const bool readWrite )
    {
        std::ostringstream sql;
        sql << LOCATION_KEY_COL << " = " << locationKey;

        return getPaBroadcastShortcutsReturnedFromStatement(sql.str(), readWrite);   
    }

    IPaBroadcastShortcuts PaBroadcastShortcutAccessFactory::getPaBroadcastShortcutsReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
        DatabaseQueryHelper::QueryData result;
        
        result.primaryKeyCol = KEY_COL;
        result.tableName = PA_BROADCAST_SHORTCUT_TABLE;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATION_KEY_COL);
        result.columnNames.push_back(SHORTCUT_NO_COL);
        result.columnNames.push_back(DVA_MSG_KEY_COL);
        result.columnNames.push_back(DATE_CREATED_COL);
        result.columnNames.push_back(DATE_MODIFIED_COL);
        
        DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement, result.columnNames, 
                                                        result.tableName);

        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaBroadcastShortcuts, ConfigPaBroadcastShortcut, PaBroadcastShortcut>
                    (result, whereClause, readWrite);
    }

    IConfigPaBroadcastShortcut* PaBroadcastShortcutAccessFactory::createPaBroadcastShortcut()
    {
        return new ConfigPaBroadcastShortcut();    
    }

    IConfigPaBroadcastShortcut* PaBroadcastShortcutAccessFactory::copyPaBroadcastShortcut(const IConfigPaBroadcastShortcut* paBroadcastShortcutToCopy)
    {
        FUNCTION_ENTRY("copyPaBroadcastShortcut");

        TA_ASSERT(paBroadcastShortcutToCopy !=  NULL, "The PaBroadcastShortcut to copy was NULL");
        
        const ConfigPaBroadcastShortcut* cast = dynamic_cast<const ConfigPaBroadcastShortcut*>(paBroadcastShortcutToCopy);

        TA_ASSERT(cast != NULL, "PaBroadcastShortcut passed could not be converted into a ConfigPaBroadcastShortcut");

        FUNCTION_EXIT;
        return new ConfigPaBroadcastShortcut(*cast);

    }

} // closes TA_IRS_Core


