
#include "GenaClientSourceAccessFactory.h"
#include "GenaClientSource.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
    GenaClientSourceAccessFactory* GenaClientSourceAccessFactory::m_instance = 0;

    GenaClientSourceAccessFactory& GenaClientSourceAccessFactory::getInstance()
    {
		FUNCTION_ENTRY("getInstance");
        if( m_instance == 0 )
        {
            m_instance = new GenaClientSourceAccessFactory();
        }
		FUNCTION_EXIT;
        return *m_instance;
    }

	void GenaClientSourceAccessFactory::freeInstance()
	{
		FUNCTION_ENTRY("freeInstance");
		if (0 != m_instance)
		{
			delete m_instance;
			m_instance = 0;
		}

		FUNCTION_EXIT;
	}

	void GenaClientSourceAccessFactory::runQueryForClientSource(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IGenaClientSource*>& clientSources)
	{
		FUNCTION_ENTRY("runQueryForClientSource");
        TA_ASSERT(NULL != pDatabase , "the Database pointer is NULL");

		// Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("a.PKEY");
        columnNames.push_back("a.ENTITYKEY");
		columnNames.push_back("a.SERVERURL");
        columnNames.push_back("a.MAXRETRY");
        columnNames.push_back("a.TIMEOUT");
        columnNames.push_back("a.KEEPALIVE");
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql,columnNames);
        
        do
        {
		    // Loop through and create all the objects
		    for(unsigned int i = 0; i < data->getNumRows(); i++)
		    {
				try
				{
					clientSources.push_back(new GenaClientSource(i, *data));
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
        while ( pDatabase->moreData(data) );

		FUNCTION_EXIT;
    }

    
    IGenaClientSource* GenaClientSourceAccessFactory::getGenaClientSource(const unsigned long& key)
    {
        FUNCTION_ENTRY("getGenaClientSource");

		// get a connection to the database and retrieve the DB server type
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Read);		
	   
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, GENA_CLIENT_SOURCE_SELECT_170001, key);

		std::vector<IGenaClientSource*> clientSources;
		runQueryForClientSource(databaseConnection, sql, clientSources);

        if ( 0 == clientSources.size())
        {
            std::ostringstream message;
			message << "No data found for GenaClientSource with key " << key;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

 			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, "ERROR" ) );

        }

		TA_ASSERT(1 == clientSources.size(), "GenaClientSource key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return clientSources[0];
    }
	
    std::vector<IGenaClientSource*> GenaClientSourceAccessFactory::getClientSourceForEntity(unsigned long p_entityKey)
	{	
		FUNCTION_ENTRY("getClientSourceForEntity");

		// get a connection to the database and retrieve the DB server type
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Read);		

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, GENA_CLIENT_SOURCE_SELECT_170004, p_entityKey);

		std::vector<IGenaClientSource*> clientSources;
		runQueryForClientSource(databaseConnection, sql, clientSources);

		if ( 0 == clientSources.size())
		{
			std::ostringstream message;
			message << "No data found for GenaClientSource with entitykey " << p_entityKey;
			// TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, "ERROR" ) );

		}

		FUNCTION_EXIT;
		return clientSources;
	}

	std::vector<IGenaClientSource*> GenaClientSourceAccessFactory::getClientSourceForLocation(unsigned long p_locationKey)
	{
		FUNCTION_ENTRY("getClientSourceForLocation");

		// get a connection to the database and retrieve the DB server type
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Read);		

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, GENA_CLIENT_SOURCE_SELECT_170005, p_locationKey);

		std::vector<IGenaClientSource*> clientSources;
		runQueryForClientSource(databaseConnection, sql, clientSources);

		if ( 0 == clientSources.size())
		{
			std::ostringstream message;
			message << "No data found for GenaClientSource with locationkey " << p_locationKey;
			// TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, "ERROR" ) );

		}

		FUNCTION_EXIT;
		return clientSources;
	}

	IGenaClientSource* GenaClientSourceAccessFactory::createClientSource()
	{
		FUNCTION_ENTRY("createClientSource");
		FUNCTION_EXIT;
		return new GenaClientSource();
	}
} //TA_Base_Core
