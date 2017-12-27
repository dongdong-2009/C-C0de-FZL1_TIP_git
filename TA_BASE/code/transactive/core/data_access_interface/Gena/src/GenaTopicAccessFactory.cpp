
#include "GenaTopicAccessFactory.h"
#include "GenaTopicData.h"
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
	GenaTopicAccessFactory* GenaTopicAccessFactory::m_instance = 0;

	GenaTopicAccessFactory& GenaTopicAccessFactory::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		if( m_instance == 0 )
		{
			m_instance = new GenaTopicAccessFactory();
		}
		FUNCTION_EXIT;
		return *m_instance;
	}

	void GenaTopicAccessFactory::freeInstance()
	{
		FUNCTION_ENTRY("freeInstance");
		if (0 != m_instance)
		{
			delete m_instance;
			m_instance = 0;
		}

		FUNCTION_EXIT;
	}

	void GenaTopicAccessFactory::runQueryForTopic(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IGenaTopic*>& genaTopics)
	{
		FUNCTION_ENTRY("runQueryForClientSource");
		TA_ASSERT(NULL != pDatabase , "the Database pointer is NULL");

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back("a.PKEY");
		columnNames.push_back("a.ENTITYKEY");
		columnNames.push_back("a.TOPIC");
		columnNames.push_back("a.SUBSCRIBEURL");
		columnNames.push_back("a.NOTIFICATION");

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
					genaTopics.push_back(new GenaTopicData(i, *data));
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


	IGenaTopic* GenaTopicAccessFactory::getGenaGenaTopic(const unsigned long& key)
	{
		FUNCTION_ENTRY("getGenaClientSource");

		// get a connection to the database and retrieve the DB server type
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Read);		

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, GENA_CLIENT_TOPIC_SELECT_180001, key);

		std::vector<IGenaTopic*> genaTopics;
		runQueryForTopic(databaseConnection, sql, genaTopics);

		if ( 0 == genaTopics.size())
		{
			std::ostringstream message;
			message << "No data found for AlarmType with key " << key;
			// TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, "ERROR" ) );

		}

		TA_ASSERT(1 == genaTopics.size(), "Gena Client Topic key unique constraint violated");

		// Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return genaTopics[0];
	}

	std::vector<IGenaTopic*> GenaTopicAccessFactory::getGenaTopicForEntity(unsigned long p_entityKey)
	{
		FUNCTION_ENTRY("getGenaTopicForEntity");

		// get a connection to the database and retrieve the DB server type
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Read);		

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, GENA_CLIENT_TOPIC_SELECT_180004, p_entityKey);

		std::vector<IGenaTopic*> genaTopics;
		runQueryForTopic(databaseConnection, sql, genaTopics);

		if ( 0 == genaTopics.size())
		{
			//std::ostringstream message;
			//message << "No data found for Gena Client Topic with entitykey " << p_entityKey;
			//TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, "ERROR" ) );
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "No data found for Gena Client Topic with entitykey=%d", p_entityKey);
		}

		FUNCTION_EXIT;
		return genaTopics;
	}

	IGenaTopic* GenaTopicAccessFactory::createTopic()
	{
		FUNCTION_ENTRY("createClientSource");
		FUNCTION_EXIT;
		return new GenaTopicData();
	}
} //TA_Base_Core
