/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/ITSIGroupAccessFactory.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ITSIGroupAccessFactory is a singleton that is used to retrieve radio login group objects either from the
  * database or newly created. All radio login group objects returned will adhere to the IITSIGroup interface.
  * Data is primarily retrieved from the RA_ITSI_GROUP table
  *
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/radio/src/ITSIGroupAccessFactory.h"
#include "core/data_access_interface/radio/src/ITSIGroup.h"
#include "core/data_access_interface/radio/src/ConfigITSIGroup.h"
#include "core/data_access_interface/radio/src/IConfigITSIGroup.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    ITSIGroupAccessFactory::ITSIGroupAccessFactory()
    {
    }


    ITSIGroupAccessFactory& ITSIGroupAccessFactory::getInstance()
    {
        static ITSIGroupAccessFactory factory;
        return factory;
    }


    IITSIGroup* ITSIGroupAccessFactory::getITSIGroup( const unsigned long ITSIGroupKey, const bool readWrite /*false*/ )
    {
        FUNCTION_ENTRY("getITSIGroup");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);

        // create the SQL string to retrieve the name and key of the ITSIGroup
        // this is a check to ensure a ITSIGroup with the specified key actually exists.
        /*std::ostringstream sql;
        sql << "select RAIGRO_ID from RA_ITSI_GROUP where RAIGRO_ID = " << ITSIGroupKey;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_SELECT_29003, ITSIGroupKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_SELECT_29003, ITSIGroupKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "KEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for key = " << ITSIGroupKey;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for key = " << ITSIGroupKey;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        FUNCTION_EXIT;

        if (readWrite)
        {
            return new ConfigITSIGroup( ITSIGroupKey );
        }

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		return new ITSIGroup( ITSIGroupKey );
    }


    std::vector<IITSIGroup*> ITSIGroupAccessFactory::getAllITSIGroups(const bool readWrite /*false*/)
    {
        FUNCTION_ENTRY("getAllITSIGroups");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);

		// create the SQL string to retrieve all the locations of the alarm
       /* std::ostringstream sql;
        sql << "select RAIGRO_ID, NAME from RA_ITSI_GROUP";*/
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_ITSI_GROUP_SELECT_29002);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_ITSI_GROUP_SELECT_29002);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn( "PKEY" );
        std::string nameColumn( "NAME" );
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(nameColumn);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
        // Loop for each row returned
        std::vector< IITSIGroup* > ITSIGroups;

 	    if (data->getNumRows() == 0) // No alarm rules found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return ITSIGroups;
        }

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
						// create new config profile group if readWrite is enabled
                        ITSIGroups.push_back( new ConfigITSIGroup(data->getUnsignedLongData( i, keyColumn )) );
                    }
                    else
                    {
						// create read only profile group if readWrite is disabled
                        ITSIGroups.push_back( new ITSIGroup( data->getUnsignedLongData( i, keyColumn ) ) );
                    }
                }
                catch (TA_Base_Core::DataException&)
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
        while (databaseConnection->moreData(data));

        FUNCTION_EXIT;
		return ITSIGroups;
    }

    IConfigITSIGroup* ITSIGroupAccessFactory::createITSIGroup()
    {            
        return new ConfigITSIGroup();    
    }

	IConfigITSIGroup* ITSIGroupAccessFactory::copyITSIGroup(const IConfigITSIGroup* itsiGpToCopy)
    {
        FUNCTION_ENTRY("copyITSIGroup");

        TA_ASSERT(itsiGpToCopy !=  NULL, "The ITSI group to copy was NULL");
        
        const ConfigITSIGroup* cast = dynamic_cast<const ConfigITSIGroup*>(itsiGpToCopy);

        TA_ASSERT(cast != NULL, "ITSI group passed could not be converted into a ConfigITSIGroup");

        FUNCTION_EXIT;

        return new ConfigITSIGroup(*cast);
    }
	
} // closes TA_Base_Core


