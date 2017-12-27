// This is the main DLL file.
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
	// Constructor
	GenaClientSource::GenaClientSource()
		: m_pkey(0)
		, m_entitykey(0)
		, m_serverURL("")
		, m_maxRetry(0)
		, m_timeOut(0)
		, m_keepAlive(true)
		, m_isNew(true)
		, m_isValidData(false)
		, m_isEntityKeyModified(false)
		, m_isServerURLModified(false)
		, m_isMaxRetryModified(false)
		, m_isTimeOutModified(false)
		, m_isKeepAliveModified(false)
	{
		FUNCTION_ENTRY("Constructor");

		FUNCTION_EXIT;
	}

	GenaClientSource::GenaClientSource(const unsigned long& pkey)
		: m_pkey(pkey)
		, m_entitykey(0)
		, m_serverURL("")
		, m_maxRetry(0)
		, m_timeOut(0)
		, m_keepAlive(true)
		, m_isNew(false)
		, m_isValidData(false)
		, m_isEntityKeyModified(false)
		, m_isServerURLModified(false)
		, m_isMaxRetryModified(false)
		, m_isTimeOutModified(false)
		, m_isKeepAliveModified(false)
	{
		FUNCTION_ENTRY("Constructor");

		FUNCTION_EXIT;
	}

	GenaClientSource::GenaClientSource(const unsigned long row, TA_Base_Core::IData& data)
	{
		FUNCTION_ENTRY("GenaClientSource");
		reloadInternal(row, data);
		FUNCTION_EXIT;
	}

	// Destructor
	GenaClientSource::~GenaClientSource()
	{
		FUNCTION_ENTRY("Destructor");

		FUNCTION_EXIT;
	}

	// Pkey
	void GenaClientSource::setPkey(const unsigned long& pkey)
	{
		FUNCTION_ENTRY("setPkey");
		TA_ASSERT(true == m_isNew,"Cannot set the key for an existing Client Source");
		m_pkey = pkey;

		FUNCTION_EXIT;
	}

	unsigned long GenaClientSource::getPkey()
	{
		FUNCTION_ENTRY("getPkey");
		TA_ASSERT(0 != m_pkey, "A newly created alarm must be written to the database before the key can be retrieved");		
		
		FUNCTION_EXIT;
		return(m_pkey);
	}

	//Entitykey
	void GenaClientSource::setEntityKey(const unsigned long& entityKey)
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

	unsigned long GenaClientSource::getEntityKey()
	{
		FUNCTION_ENTRY("getEntityKey");
		reload();

		FUNCTION_EXIT;
		return m_entitykey;
	}

	//ServerURL
	void GenaClientSource::setServerURL(const std::string& serverURL)
	{
		FUNCTION_ENTRY("setServerURL");
		reload();

		if (m_serverURL != serverURL)
		{
			m_serverURL = serverURL;
			m_isServerURLModified = true;
		}
		FUNCTION_EXIT;
	}

	std::string GenaClientSource::getServerURL()
	{
		FUNCTION_ENTRY("getServerURL");
		reload();

		FUNCTION_EXIT;
		return m_serverURL;
	}

	//MaxRetry
	void GenaClientSource::setMaxRetry(const unsigned long& maxRetry)
	{
		FUNCTION_ENTRY("setMaxRetry");
		reload();

		if (m_maxRetry != maxRetry)
		{
			m_maxRetry = maxRetry;
			m_isMaxRetryModified = true;
		}
		FUNCTION_EXIT;
	}

	unsigned long GenaClientSource::getMaxRetry()
	{
		FUNCTION_ENTRY("getMaxRetry");
		reload();

		FUNCTION_EXIT;
		return m_maxRetry;
	}

	//TimeOut
	void GenaClientSource::setTimeOut(const unsigned long& timeOut)
	{
		FUNCTION_ENTRY("setTimeOut");
		reload();

		if (m_timeOut != timeOut)
		{
			m_timeOut = timeOut;
			m_isTimeOutModified = true;
		}
		FUNCTION_EXIT;
	}

	unsigned long GenaClientSource::getTimeOut()
	{
		FUNCTION_ENTRY("getTimeOut");
		reload();

		FUNCTION_EXIT;
		return m_timeOut;
	}

	//KeepAlive
	void GenaClientSource::setKeepAlive(const bool& keepAlive)
	{
		FUNCTION_ENTRY("setKeepAlive");
		reload();

		if ((m_keepAlive==0?false:true) != keepAlive)
		{
			m_keepAlive = keepAlive?1:0;
			m_isKeepAliveModified = true;
		}
		FUNCTION_EXIT;
	}

	bool GenaClientSource::getKeepAlive()
	{
		FUNCTION_ENTRY("getKeepAlive");
		reload();

		FUNCTION_EXIT;
		return m_keepAlive==0?false:true;
	}

	void GenaClientSource::invalidate()
	{
		FUNCTION_ENTRY("invalidate");
		m_isValidData = false;

		FUNCTION_EXIT;
	}

	void GenaClientSource::applyChanges()
	{
		FUNCTION_ENTRY("applyChanges");

		FUNCTION_EXIT;
	}

	void GenaClientSource::reloadInternal(const unsigned long rowIdx, TA_Base_Core::IData& data)
	{
		FUNCTION_ENTRY("reloadInternal");

		std::string pkeyColumn = "a.PKEY";
		std::string entityKeyColumn = "a.ENTITYKEY";
		std::string serverURLColumn = "a.SERVERURL";
		std::string maxretryColumn = "a.MAXRETRY";
		std::string timeOutColumn = "a.TIMEOUT";
		std::string keepAliveColumn = "a.KEEPALIVE";

		// Assign the data as appropriate to the member variables.
		// These calls can throw DataExceptions, as documented in the comment of this method.
		m_pkey = data.getUnsignedLongData(rowIdx, pkeyColumn);
		m_entitykey = data.getUnsignedLongData(rowIdx,entityKeyColumn);
		m_serverURL = data.getStringData(rowIdx,serverURLColumn);
		m_maxRetry = data.getUnsignedLongData(rowIdx,maxretryColumn);
		m_timeOut = data.getUnsignedLongData(rowIdx,timeOutColumn);
		m_keepAlive = data.getUnsignedLongData(rowIdx,keepAliveColumn);

		// Need to record that we now have valid data
		m_isValidData = true;

		m_isEntityKeyModified = false;
		m_isServerURLModified = false;
		m_isMaxRetryModified = false;
		m_isTimeOutModified = false;
		m_isKeepAliveModified = false;

		FUNCTION_EXIT;
	}
	void GenaClientSource::reload()
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
		databaseConnection->prepareSQLStatement(sql, GENA_CLIENT_SOURCE_SELECT_170001, m_pkey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string pkeyColumn = "a.PKEY";
		std::string entityKeyColumn = "a.ENTITYKEY";
        std::string serverURLColumn = "a.SERVERURL";
        std::string maxretryColumn = "a.MAXRETRY";
        std::string timeOutColumn = "a.TIMEOUT";
        std::string keepAliveColumn = "a.KEEPALIVE";
        
        std::vector<std::string> columnNames;
		columnNames.push_back(pkeyColumn);
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(serverURLColumn);
        columnNames.push_back(maxretryColumn);
        columnNames.push_back(timeOutColumn);
        columnNames.push_back(keepAliveColumn);
        
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

	void GenaClientSource::createNew()
	{
		FUNCTION_ENTRY("createNew");
        // Now we can get on with writing the entity to the database.
        // Convert non-string elements to char*'s
		char entityKeyString[50];
		char maxRetryString[4];
		char timeOutString[50];
        char keepAliveString[2];
        
		sprintf(entityKeyString, "%lu", m_entitykey);
		sprintf(maxRetryString, "%d", m_maxRetry);
		sprintf(timeOutString, "%lu", m_timeOut);
        sprintf(keepAliveString, "%d", m_keepAlive);
        
        // Get the database connection
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Signalling_Cd, Write);
        
		SQLStatement formatSQL;
		databaseConnection->prepareSQLStatement(formatSQL, GENA_CLIENT_SOURCE_INSERT_170002,
			getLocalDatabaseName(), m_pkey, std::string(entityKeyString), databaseConnection->escapeAQSQLString(m_serverURL),
			std::string(maxRetryString), std::string(timeOutString), std::string(keepAliveString));
		
        databaseConnection->executeModification(formatSQL);

		m_isEntityKeyModified = false;
		m_isServerURLModified = false;
		m_isMaxRetryModified = false;
		m_isTimeOutModified = false;
		m_isKeepAliveModified = false;
		
		m_isNew = false;
		m_isValidData = true;
        
		FUNCTION_EXIT;
	}

	void GenaClientSource::updateExisting()
	{
		FUNCTION_ENTRY("updateExisting");
		// TD #11720
		// make sure at least one field has actually been modified before we make the SQL query
		if( m_isEntityKeyModified || m_isServerURLModified || m_isMaxRetryModified || 
		    m_isTimeOutModified || m_isKeepAliveModified )
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

			// ServerURL
			if(m_isServerURLModified)
			{
				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("SERVERURL"), databaseConnection->escapeAQSQLString(m_serverURL), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// MAXRETRY
			if(m_isMaxRetryModified)
			{
				char maxRetryString[4];
				sprintf(maxRetryString, "%lu", m_maxRetry);
				
				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("MAXRETRY"), std::string(maxRetryString), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// TIMEOUT
			if(m_isTimeOutModified)
			{
		        // Convert non-string elements to char*'s
				char timeOutString[50];
				sprintf(timeOutString, "%lu", m_timeOut);
				
				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("TIMEOUT"), std::string(timeOutString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// AVL_STATUS
			if (m_isKeepAliveModified)
			{
				char keepAliveString[2];
				sprintf(keepAliveString, "%d", m_keepAlive);

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("KEEPALIVE"), std::string(keepAliveString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}
			
			databaseConnection->prepareSQLStatement(strSql, GENA_CLIENT_SOURCE_UPDATE_170003, getLocalDatabaseName(), sqlParams, m_pkey);
			databaseConnection->executeModification(strSql);

			m_isEntityKeyModified = false;
			m_isServerURLModified = false;
			m_isMaxRetryModified = false;
			m_isTimeOutModified = false;
			m_isKeepAliveModified = false;
		}

		FUNCTION_EXIT;
	}

	std::string GenaClientSource::getLocalDatabaseName()
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
