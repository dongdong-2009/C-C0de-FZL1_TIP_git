
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
	// Constructor
	GenaTopicData::GenaTopicData()
		: m_pkey(0)
		, m_entitykey(0)
		, m_subscribeURL("")
		, m_topic("")
		, m_notification("")
		, m_isNew(true)
		, m_isValidData(false)
		, m_isEntityKeyModified(false)
		, m_isSubscribeURLModified(false)
		, m_isTopicModified(false)
		, m_isNotificationModified(false)
	{
		FUNCTION_ENTRY("Constructor");

		FUNCTION_EXIT;
	}

	GenaTopicData::GenaTopicData(const unsigned long& pkey)
		: m_pkey(pkey)
		, m_entitykey(0)
		, m_subscribeURL("")
		, m_topic("")
		, m_notification("")
		, m_isNew(false)
		, m_isValidData(false)
		, m_isEntityKeyModified(false)
		, m_isSubscribeURLModified(false)
		, m_isTopicModified(false)
		, m_isNotificationModified(false)
	{
		FUNCTION_ENTRY("Constructor");

		FUNCTION_EXIT;
	}

	GenaTopicData::GenaTopicData(const unsigned long row, TA_Base_Core::IData& data)
	{
		FUNCTION_ENTRY("GenaTopicData");
		reloadInternal(row, data);
		FUNCTION_EXIT;
	}

	// Destructor
	GenaTopicData::~GenaTopicData()
	{
		FUNCTION_ENTRY("Destructor");

		FUNCTION_EXIT;
	}

	// Pkey
	void GenaTopicData::setKey(const unsigned long& pkey)
	{
		FUNCTION_ENTRY("setPkey");
		TA_ASSERT(true == m_isNew,"Cannot set the key for an existing Client Source");
		m_pkey = pkey;

		FUNCTION_EXIT;
	}

	unsigned long GenaTopicData::getKey()
	{
		FUNCTION_ENTRY("getPkey");
		TA_ASSERT(0 != m_pkey, "A newly created alarm must be written to the database before the key can be retrieved");		

		FUNCTION_EXIT;
		return(m_pkey);
	}

	//Entitykey
	void GenaTopicData::setEntityKey(const unsigned long& entityKey)
	{
		FUNCTION_ENTRY("setEntityKey");
		reload();

		if (m_entitykey != entityKey)
		{
			m_entitykey = entityKey;
			m_isEntityKeyModified = true;
		}

		FUNCTION_EXIT;
	}

	unsigned long GenaTopicData::getEntityKey()
	{
		FUNCTION_ENTRY("getEntityKey");
		reload();

		FUNCTION_EXIT;
		return m_entitykey;
	}

	//ServerURL
	void GenaTopicData::setSubscribeURL(const std::string& subscribeURL)
	{
		FUNCTION_ENTRY("setSubscribeURL");
		reload();

		if (m_subscribeURL != subscribeURL)
		{
			m_subscribeURL = subscribeURL;
			m_isSubscribeURLModified = true;
		}
		FUNCTION_EXIT;
	}

	std::string GenaTopicData::getSubscribeURL()
	{
		FUNCTION_ENTRY("getSubscribeURL");
		reload();

		FUNCTION_EXIT;
		return m_subscribeURL;
	}

	//Topic
	void GenaTopicData::setTopic(const std::string& topic)
	{
		FUNCTION_ENTRY("setTopic");
		reload();

		if (m_topic != topic)
		{
			m_topic = topic;
			m_isTopicModified = true;
		}
		FUNCTION_EXIT;
	}

	std::string GenaTopicData::getTopic()
	{
		FUNCTION_ENTRY("getTopic");
		reload();

		FUNCTION_EXIT;
		return m_topic;
	}

	void GenaTopicData::setNotification(const std::string& notification)
	{
		FUNCTION_ENTRY("setNotification");
		reload();

		if (m_notification != notification)
		{
			m_notification = notification;
			m_isNotificationModified =true;
		}
		FUNCTION_EXIT;
		
	}

	std::string GenaTopicData::getNotification()
	{
		FUNCTION_ENTRY("getNotification");
		reload();

		FUNCTION_EXIT;
		return m_notification;
	}


	void GenaTopicData::invalidate()
	{
		FUNCTION_ENTRY("invalidate");
		m_isValidData = false;

		FUNCTION_EXIT;
	}

	void GenaTopicData::applyChanges()
	{
		FUNCTION_ENTRY("applyChanges");

		FUNCTION_EXIT;
	}

	void GenaTopicData::reloadInternal(const unsigned long rowIdx, TA_Base_Core::IData& data)
	{
		FUNCTION_ENTRY("reloadInternal");

		std::string pkeyColumn = "a.PKEY";
		std::string entityKeyColumn = "a.ENTITYKEY";
		std::string topicColumn = "a.TOPIC";
		std::string subscribeURLColumn = "a.SUBSCRIBEURL";
		std::string notificationColumn = "a.NOTIFICATION";
		
		// Assign the data as appropriate to the member variables.
		// These calls can throw DataExceptions, as documented in the comment of this method.
		m_pkey = data.getUnsignedLongData(rowIdx, pkeyColumn);
		m_entitykey = data.getUnsignedLongData(rowIdx,entityKeyColumn);
		m_topic = data.getStringData(rowIdx,topicColumn);
		m_subscribeURL = data.getStringData(rowIdx,subscribeURLColumn);
		m_notification = data.getStringData(rowIdx,notificationColumn);
	
		// Need to record that we now have valid data
		m_isValidData = true;

		m_isEntityKeyModified = false;
		m_isSubscribeURLModified = false;
		m_isTopicModified = false;
		
		FUNCTION_EXIT;
	}
	void GenaTopicData::reload()
	{
		FUNCTION_ENTRY("reload");
		if (m_isNew || (!m_isNew && m_isValidData))
		{	// no need to reload
			return; 
		}

		TA_ASSERT(0 != m_pkey, "This Client Source has not yet been written to the database, therefore it cannot be reloaded");

		// get a connection to the database
		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Read);

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, GENA_CLIENT_TOPIC_SELECT_180001, m_pkey);

		// Set up the columnNames vector to be passed to executeQuery()
		std::string pkeyColumn = "a.PKEY";
		std::string entityKeyColumn = "a.ENTITYKEY";
		std::string topicColumn = "a.TOPIC";
		std::string subscribeURLColumn = "a.SUBSCRIBEURL";
		std::string notificationColumn = "a.NOTIFICATION";

		std::vector<std::string> columnNames;
		columnNames.push_back(pkeyColumn);
		columnNames.push_back(entityKeyColumn);
		columnNames.push_back(topicColumn);
		columnNames.push_back(subscribeURLColumn);
		columnNames.push_back(notificationColumn);
		
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery(sql,columnNames);

		// Need to bring in the DataException
		using TA_Base_Core::DataException;

		if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified alarm_id
		{
			delete data;
			data = NULL;
			char reasonMessage[256];
			sprintf(reasonMessage, "No Client Source found for pkey = %d", m_pkey);
			throw DataException(reasonMessage,DataException::NO_VALUE,"");
		}
		else if (1 < data->getNumRows()) // More than one entry found for the alarm_id
		{
			delete data;
			data = NULL;
			char reasonMessage[256];
			sprintf(reasonMessage, "More than one Client Source found for pkey = %d", m_pkey);
			throw DataException(reasonMessage,DataException::NOT_UNIQUE,"");
		}

		reloadInternal(0, *data);

		// Now that we're done with the IData object, it's our responsibility to delete it
		delete data;
		data = NULL;

		FUNCTION_EXIT;
	}

	void GenaTopicData::createNew()
	{
		FUNCTION_ENTRY("createNew");
		// Now we can get on with writing the entity to the database.
		// Convert non-string elements to char*'s
		char entityKeyString[50];
		sprintf(entityKeyString, "%lu", m_entitykey);
		
		// Get the database connection
		IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Write);

		SQLStatement formatSQL;
		databaseConnection->prepareSQLStatement(formatSQL, GENA_CLIENT_TOPIC_INSERT_180002,
			getLocalDatabaseName(), m_pkey, std::string(entityKeyString), databaseConnection->escapeAQSQLString(m_subscribeURL),
			databaseConnection->escapeAQSQLString(m_topic), databaseConnection->escapeAQSQLString(m_notification));

		databaseConnection->executeModification(formatSQL);

		m_isEntityKeyModified = false;
		m_isSubscribeURLModified = false;
		m_isTopicModified = false;
		m_isNotificationModified = false;

		m_isNew = false;
		m_isValidData = true;

		FUNCTION_EXIT;
	}

	void GenaTopicData::updateExisting()
	{
		FUNCTION_ENTRY("updateExisting");
		// TD #11720
		// make sure at least one field has actually been modified before we make the SQL query
		if( m_isEntityKeyModified || m_isSubscribeURLModified || m_isTopicModified )
		{
			// Get the database connection
			IDatabase* databaseConnection =
				TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Write);

			bool firstField = true;
			std::ostringstream formatSQL;

			SQLStatement strSql;
			SQLTypeAdapter sqlParams;

			// Entity
			if(m_isEntityKeyModified)
			{
				char entitkyKeyString[50];
				sprintf(entitkyKeyString, "%lu", m_entitykey);

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("ENTITYKEY"), std::string(entitkyKeyString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// SUBSCRIBEURL
			if(m_isSubscribeURLModified)
			{
				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("SUBSCRIBEURL"), databaseConnection->escapeAQSQLString(m_subscribeURL), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// TOPIC
			if(m_isTopicModified)
			{
				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("TOPIC"), databaseConnection->escapeAQSQLString(m_topic), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			if (m_isNotificationModified)
			{
				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("NOTIFICATION"), databaseConnection->escapeAQSQLString(m_notification), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			
			databaseConnection->prepareSQLStatement(strSql, GENA_CLIENT_TOPIC_UPDATE_180003, getLocalDatabaseName(), sqlParams, m_pkey);
			databaseConnection->executeModification(strSql);

			m_isEntityKeyModified = false;
			m_isSubscribeURLModified = false;
			m_isTopicModified = false;
			m_isNotificationModified = false;
		}

		FUNCTION_EXIT;
	}

	std::string GenaTopicData::getLocalDatabaseName()
	{
		if (m_localDatabase.empty())
		{
			// have to find out what is the local db name
			// this should be the first in the connection list so grab it
			if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
			{
				throw DatabaseException("db-connection-file not set");
			}
			try
			{
				std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Alarm_Sd, Write);
			}
			catch(...) // most likely a file not found error
			{
				throw DatabaseException("Unable to find database connection infomation");
			}

		}
		return m_localDatabase;
	}
}
