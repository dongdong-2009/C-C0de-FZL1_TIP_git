/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Session.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * Session is an implementation of ISession. It holds the data specific to an Session entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/Session.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{
    Session::Session(const std::string& sessionId)
        : m_sessionId(sessionId),
          m_operatorKey( ULONG_MAX ),
          m_consoleKey( ULONG_MAX ),
		  m_profileKey( ULONG_MAX ),
	      m_locationKey( ULONG_MAX ),
		  m_entityKey( ULONG_MAX ),
		  m_displayOnly(false),
		  m_hasEnded(false),
          m_operatorName( "" ),
          m_isValidData( false ),
		  m_isWritten( true ),
		  m_isUpdateOnly(false)
    {
    }


	Session::Session( unsigned long operatorKey, unsigned long consoleKey, unsigned long profileKey,
		unsigned long locationKey, unsigned long entityKey ,bool displayOnly) :
		m_operatorKey( operatorKey ),
		m_consoleKey( consoleKey ),
		m_profileKey( profileKey ),
		m_locationKey(locationKey),
		m_entityKey( entityKey ),
		m_hasEnded(false),
        m_operatorName( "" ),
		m_isValidData( false ),
		m_isWritten( false ),
	    m_isUpdateOnly(false),
		m_displayOnly(displayOnly)
	{
		// Set up the sessionID for database storage
        //xinsong++ sessionID should include location information 
		std::string uuidStr = TA_Base_Core::TA_Uuid::generateUUID();
		m_sessionId = SessionAccessFactory::fillLocInfoToSessionId( locationKey, uuidStr );
	}

	Session::Session( const std::string& sessionID,
		unsigned long operatorKey,
		unsigned long consoleKey,
		unsigned long profileKey,
		unsigned long locationKey,
		unsigned long entityKey,
		bool		  displayOnly,
		bool          hasEnded,
        bool isNew) :
		m_sessionId   (sessionID  ),
		m_operatorKey (operatorKey),
        m_consoleKey  (consoleKey ),
		m_profileKey  (profileKey ),
		m_locationKey (locationKey),
		m_entityKey   (entityKey  ),
		m_displayOnly (displayOnly),
		m_hasEnded    (hasEnded   ),
		m_operatorName( ""        ),
		m_isValidData ( false     ),
		m_isWritten   (!isNew     ),
		m_isUpdateOnly(true)
	{
	}

    Session::~Session()
    {
    }


	std::string Session::getSessionId()
    {
        TA_ASSERT(m_sessionId != "", "The sessionId is not set");
        return m_sessionId;
    }
	
	bool Session::getDisplayMode()
	{
		return m_displayOnly;
	}

    unsigned long Session::getOperatorKey()
    {
		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten && m_operatorKey == ULONG_MAX )
		{
			TA_ASSERT( true, "The Operatorkey must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly )
        {
            reload();
        }

        return m_operatorKey;
    }


    std::string Session::getOperatorName()
    {

        if (m_operatorName == "")
        {
            IOperator* op = NULL;

            try
            {
		        // Get the location object
		        op = OperatorAccessFactory::getInstance().getOperator( getOperatorKey(), false );
		        m_operatorName = op->getName();

		        // Clean up
		        delete op;
            }
			//exception
            catch ( ... )
            {
                if (op != NULL)
                {
                    // Clean up if an exception is thrown
                    delete op;
                }
                throw;
            }
        }

        return m_operatorName;
    }


    unsigned long Session::getConsoleKey()
    {

		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten && m_consoleKey == ULONG_MAX )
		{
			TA_ASSERT( true, "The Consolekey must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly )
        {
            reload();
        }

        return m_consoleKey;
    }


    unsigned long Session::getProfileKey()
    {

		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten && m_profileKey == ULONG_MAX )
		{
			TA_ASSERT( true, "The ProfileKey must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly )
        {
            reload();
        }

        return m_profileKey;
    }

	unsigned long Session::getLocationKey()
    {

		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten && m_locationKey == ULONG_MAX )
		{
			TA_ASSERT( true, "The LocationKey must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly )
        {
            reload();
        }

        return m_locationKey;
    }

	bool Session::getHasEnded()
    {

		if( m_isWritten && !m_isValidData  && !m_isUpdateOnly )
        {
            reload();
        }

        return m_hasEnded;
    }

	void Session::setHasEnded()
	{
		m_isUpdateOnly = true;
		m_hasEnded = true;
	}

    void Session::invalidate()
    {
		m_sessionId    = "";
		m_operatorKey  = ULONG_MAX;
		m_consoleKey   = ULONG_MAX;
		m_profileKey   = ULONG_MAX;
		m_locationKey  = ULONG_MAX;
		m_entityKey    = ULONG_MAX;
		m_hasEnded     = false;
		m_operatorName = "";
        m_isValidData  = false;
    }

	
    std::string Session::getLocalDatabaseName()
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
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Sessions_Ad, Write);

               /* DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Sessions_Ad, Write);
                m_localDatabase = connections[0].first;*/  
            }
			//exception
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

    void Session::applyChanges()
	{
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "Session::applyChanges" );     
		TA_ASSERT( !m_sessionId.empty(), "SessionId not set, can not apply" );

		// NOTE: Sessions can only be created or destroyed - nothing else. 
		// There is no need to handle updating records, only creating new records... UNLESS
		// were are updating the HAS_ENDED column ...

        // Write the session to the database

		// get a connection to the database
			IDatabase* databaseConnection = 
					TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Write);

		if( !m_hasEnded )
		{
			LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				"Writing new session (id: %s) to the database.",
				m_sessionId.c_str());

            /*std::ostringstream formatSQL;
            formatSQL << "BEGIN ";
            formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
	        formatSQL << "insert into TA_SESSION (PKEY, OPERATORKEY, CONSOLEKEY, SEPROF_ID, LOCATI_ID, ENTITY_ID, HAS_ENDED,ISDISPLAYONLY,OPERATOR_OVERRIDE_KEY, SEPROF_OVERRIDE_ID) ";
            formatSQL << "values (''" << m_sessionId << "'',"<< m_operatorKey <<","<< m_consoleKey <<","<< m_profileKey <<","<< m_locationKey <<","<< m_entityKey <<","<< m_hasEnded <<","<<m_displayOnly <<",NULL,NULL)";
            formatSQL << "');";
            formatSQL << "END;";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ta_session_Oracle_INSERT_11351, getLocalDatabaseName(), m_sessionId, 
//			std::string strSql  = databaseConnection->prepareSQLStatement(TA_SESSION_INSERT_11351, getLocalDatabaseName(), m_sessionId, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TA_SESSION_INSERT_11351, getLocalDatabaseName(), m_sessionId,
				m_operatorKey, m_consoleKey, m_profileKey, m_locationKey, m_entityKey, m_hasEnded,m_displayOnly);
				
			// This will throw an exception upon failure - ready to catch??
			databaseConnection->executeModification(strSql);

			// Set the data to be valid - no need to force a reload after a succesful write
			m_isValidData = true;
			m_isWritten = true;
		}

		else
		{
			/*std::ostringstream sql;
			sql << "BEGIN ";
			sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
			sql << "update TA_SESSION set HAS_ENDED = 1 where pkey = ''" << m_sessionId << "''";
            sql << "');";
            sql << "END;";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ta_session_Oracle_UPDATE_11201, getLocalDatabaseName(), m_sessionId);
//			std::string strSql  = databaseConnection->prepareSQLStatement(TA_SESSION_UPDATE_11201, getLocalDatabaseName(), m_sessionId);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TA_SESSION_UPDATE_11201, getLocalDatabaseName(), m_sessionId);
			databaseConnection->executeModification(strSql);
		}

	}


    void Session::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );
 
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sessions_Ad, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        /*char sql[256];
		sprintf(sql,"select OPERATORKEY, CONSOLEKEY, SEPROF_ID, LOCATI_ID, ENTITY_ID, HAS_ENDED,ISDISPLAYONLY,OPERATOR_OVERRIDE_KEY, SEPROF_OVERRIDE_ID from TA_SESSION where PKEY = '%s'",
			m_sessionId.c_str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ta_session_STD_SELECT_11007, m_sessionId);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TA_SESSION_SELECT_11007, m_sessionId);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TA_SESSION_SELECT_11007, m_sessionId);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string operatorColumn = "OPERATORKEY";
        std::string consoleColumn  = "CONSOLEKEY";
        std::string profileColumn  = "SEPROF_ID";
		std::string locationColumn = "LOCATI_ID";
		std::string entityColumn   = "ENTITY_ID";
		std::string hasEndedColumn = "HAS_ENDED";
		std::string displayOnlyColumn = "ISDISPLAYONLY";
		std::string overrideOperatorColumn = "OPERATOR_OVERRIDE_KEY";
		std::string overrideProfileColumn = "SEPROF_OVERRIDE_ID";

        std::vector<std::string> columnNames;
        columnNames.push_back(operatorColumn);
        columnNames.push_back(consoleColumn );
		columnNames.push_back(profileColumn );
		columnNames.push_back(locationColumn);
		columnNames.push_back(entityColumn  );
		columnNames.push_back(hasEndedColumn);
		columnNames.push_back(displayOnlyColumn);
		columnNames.push_back(overrideOperatorColumn);
		columnNames.push_back(overrideProfileColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No session found for pkey = %s", m_sessionId.c_str() );			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one session found with pkey = %s", m_sessionId.c_str() );
			TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_operatorKey = data->getUnsignedLongData(0,operatorColumn);
        m_consoleKey  = data->getUnsignedLongData(0,consoleColumn );
		m_profileKey  = data->getUnsignedLongData(0,profileColumn );
		m_locationKey = data->getUnsignedLongData(0,locationColumn);
		m_entityKey   = data->getUnsignedLongData(0,entityColumn  );
		m_overrideOperatorKey  = data->getUnsignedLongData(0,overrideOperatorColumn,ULONG_MAX );
		m_overrideProfileKey = data->getUnsignedLongData(0,overrideProfileColumn,ULONG_MAX);
		m_hasEnded    = data->getBooleanData     (0,hasEndedColumn);
		m_displayOnly = data->getBooleanData     (0,displayOnlyColumn);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
		m_isWritten = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }

	//chenlei++ CL-21183
	unsigned long Session::getEntityKey()
    {
		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten && m_entityKey == ULONG_MAX )
		{
			TA_ASSERT( true, "The LocationKey must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly )
        {
            reload();
        }
		
        return m_entityKey;
    }
	//++chenlei

	void Session::updateOverrideInfoToDb(  unsigned long userOverrideId, unsigned long profileOverrideId )
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "AuthenticationLibrary::updateOverrideInfoToDb");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Sessions_Ad,TA_Base_Core::Write);
		
		if ( NULL == databaseConnection )
		{
			TA_THROW( TA_Base_Core::DatabaseException("Cannot connect to database") );
		}

// 		std::ostringstream updateSql;
// 		updateSql<<"UPDATE TA_SESSION ";
// 		updateSql<<"SET OPERATOR_OVERRIDE_KEY="<<userOverrideId<<",";
// 		updateSql<<"SEPROF_OVERRIDE_ID="<<profileOverrideId<<" ";
// 		updateSql<<"WHERE PKEY='"<<m_sessionId<<"'";

//		std::string strSql  = databaseConnection->prepareSQLStatement(TA_SESSION_UPDATE_11202,userOverrideId,profileOverrideId, m_sessionId);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TA_SESSION_UPDATE_11202,userOverrideId,profileOverrideId, m_sessionId);


		try
		{
			databaseConnection->executeModification(strSql);
		}
		//database exception
		catch( const TA_Base_Core::DatabaseException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "AuthenticationLibrary::updateOverrideInfoToDb, throw a DatabaseException during execute updateSQL.");
		}
		//data exception
		catch( const TA_Base_Core::DataException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "AuthenticationLibrary::updateOverrideInfoToDb, throw a DataException during execute updateSQL.");
		}

		LOG( SourceInfo, DebugUtil::FunctionExit, "AuthenticationLibrary::updateOverrideInfoToDb");

		m_isWritten = true;
	}

	void Session::updateOverrideInfoToDb(  )
	{
		LOG( SourceInfo, DebugUtil::FunctionEntry, "AuthenticationLibrary::updateOverrideInfoToDb");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Sessions_Ad,TA_Base_Core::Write);
		
		if ( NULL == databaseConnection )
		{
			TA_THROW( TA_Base_Core::DatabaseException("Cannot connect to database") );
		}

// 		std::ostringstream updateSql;
// 		updateSql<<"UPDATE TA_SESSION ";
// 		updateSql<<"SET OPERATOR_OVERRIDE_KEY=NULL, ";
// 		updateSql<<"SEPROF_OVERRIDE_ID=NULL ";
// 		updateSql<<"WHERE PKEY='"<<m_sessionId<<"'";

//		std::string strSql  = databaseConnection->prepareSQLStatement(TA_SESSION_UPDATE_11203, m_sessionId);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TA_SESSION_UPDATE_11203, m_sessionId);

		try
		{
			databaseConnection->executeModification(strSql);
		}
		//database exception
		catch( const TA_Base_Core::DatabaseException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "AuthenticationLibrary::updateOverrideInfoToDb, throw a DatabaseException during execute updateSQL.");
		}
		//data exception
		catch( const TA_Base_Core::DataException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "AuthenticationLibrary::updateOverrideInfoToDb, throw a DataException during execute updateSQL.");
		}

		LOG( SourceInfo, DebugUtil::FunctionExit, "AuthenticationLibrary::updateOverrideInfoToDb");

		m_isWritten = true;
	}

	 unsigned long Session::getOverrideOperatorKey() 
	{
		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten && m_overrideOperatorKey == ULONG_MAX )
		{
			TA_ASSERT( true, "The LocationKey must be set before it can be retrieved" );
		}

		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly )
		{
			reload();
		}

		return m_overrideOperatorKey;
	}
	 unsigned long Session::getOverrideProfileKey() 
	{
		// If the data isn't written & the operatorkey isn't set then we must reload
		if ( ! m_isWritten && m_overrideProfileKey == ULONG_MAX )
		{
			TA_ASSERT( true, "The LocationKey must be set before it can be retrieved" );
		}

		if ( m_isWritten && ! m_isValidData  && !m_isUpdateOnly )
		{
			reload();
		}

		return m_overrideProfileKey;
	}

	 void Session::setDisplayOnly(bool displayOnly)
	 {
		 LOG( SourceInfo, DebugUtil::FunctionEntry, "AuthenticationLibrary::setDisplayOnly");

		 IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Sessions_Ad,TA_Base_Core::Write);
		
		 if ( NULL == databaseConnection )
		 {
			 TA_THROW( TA_Base_Core::DatabaseException("Cannot connect to database") );
		 }
		 m_displayOnly = displayOnly;
// 		 std::ostringstream updateSql;
// 		 updateSql<<"UPDATE TA_SESSION ";
// 		 updateSql<<"SET ISDISPLAYONLY="<<displayOnly<<" ";
// 		 updateSql<<"WHERE PKEY='"<<m_sessionId<<"'";

//		 std::string strSql  = databaseConnection->prepareSQLStatement(TA_SESSION_UPDATE_11204,displayOnly, m_sessionId);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TA_SESSION_UPDATE_11204,displayOnly, m_sessionId);

		 try
		 {
			 databaseConnection->executeModification(strSql);
		 }
		 //database exception
		 catch( const TA_Base_Core::DatabaseException& )
		 {
			 LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "AuthenticationLibrary::setDisplayOnly, throw a DatabaseException during execute updateSQL.");
		 }
		 //data exception
		 catch( const TA_Base_Core::DataException& )
		 {
			 LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "AuthenticationLibrary::setDisplayOnly, throw a DataException during execute updateSQL.");
		 }

		 LOG( SourceInfo, DebugUtil::FunctionExit, "AuthenticationLibrary::setDisplayOnly");

		 m_isWritten = true;
	 }
} // closes TA_Base_Core
