/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/core/database/src/DbServerLocator.cpp $
* @author:  Ripple
* @version: $Revision: #3 $
*
* Last modification: $DateTime: 2010/11/25 09:29:55 $
* Last modified by:  $Author: zhilin.ouyang $
*
* DbServerLocator is a singleton that locate the DB server according
* to the DB server in used.
*/

#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#include <sstream>
#include <time.h>
#include <algorithm>
#include "core/ocl/src/ocl.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/IQueryAdapter.h"
#include "core/database/src/DbServerLocator.h"
#include "core/database/src/OCIConnection.h"
#include "core/database/src/OCIOTLConnection.h"
#include "core/database/src/ODBCOTLConnection.h"
#include "core/database/src/MysqlConnection.h"
#include "core/database/src/ConnectionReaper.h"
#include "cppconn/driver.h"
#include <boost/algorithm/string.hpp>

#include <boost/lexical_cast.hpp>

#define  defOracleDBName          "Oracle"
#define  defMysqlDBName           "Mysql"
#define  defMAXLOGMESGSIZE         2000

namespace TA_Base_Core
{
	const int DbServerLocator::NUM_RETRIES = 3;
	
	using TA_Base_Core::ThreadGuard;
	using TA_Base_Core::DebugUtil;
	
	// Definition of static member variables.
	TA_Base_Core::ReEntrantThreadLockable DbServerLocator::m_singletonLock;
	DbServerLocator* DbServerLocator::m_instance = NULL;
	
	
	////////////////////////////////////////////////////////////////////////////////
	//                                                                            //
	//                         Construction/Destruction                           //
	//                                                                            //
	////////////////////////////////////////////////////////////////////////////////
	
	
	/**
	* getInstance
	* 
	* DbServerLocator is a singleton, so this method returns a reference to the active
	* DbServerLocator object and creates a new DbServerLocator if one does not exist already.
	*
	* @return  a reference to the active DbServerLocator singleton object
	*/
	DbServerLocator& DbServerLocator::getInstance()
	{
		// TD11793: The DbServerLocator instance was being deleted while it was still in use
		// with the old singleton implementation. Using a lock and a heap bound object
		// should avoid this situation.
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		
		if ( NULL == m_instance )
		{
			m_instance = new DbServerLocator();
		}
		
		return *m_instance;
	}
	
	
	/**
	* removeInstance
	* 
	* DbServerLocator is a singleton, so it wont delete itself
	* this should be called in the cleanup of your app (called by generic gui and generic agent)
	*
	*/
	void DbServerLocator::removeInstance()
	{
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = 0;
		}
	}
	
	
	/**
	* DbServerLocator
	* 
	* Sole constructor.  This class is a singleton, hence the constructor
	* is private.  Use getInstance() to obtain a reference to the DbServerLocator
	* object.
	*/
	DbServerLocator::DbServerLocator()
        : m_reaper(new ConnectionReaper()),
          m_queryRetryInterval( 0 )
	//	  m_enumDbType(enumOracleDb)  // default for Oracle DB
	{
		// Construct a ConnectionReaper thread.  It will be responsible
		// for cleaning up connections that are being refreshed and 
		// the existing connections when auto-reconnect occurs.
		
		LOG(SourceInfo, DebugUtil::GenericLog, 
			DebugUtil::DebugDatabase, "Starting Connection Reaper" );
		
		//added by hongzhi, solve thread hang when setting runparam
		if ( true == RunParams::getInstance().isSet("QueryRetryInterval") )
		{
            std::string queryRetryInterval = RunParams::getInstance().get("QueryRetryInterval");
            try
            {
                m_queryRetryInterval = boost::lexical_cast<unsigned long>( queryRetryInterval.c_str() );
            }
            catch( boost::bad_lexical_cast& )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Bad Runparam value for QueryRetryInterval: %s. Failed to parse to an unsigned long",
                             queryRetryInterval.c_str() );
            }
            
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                         "QueryRetryInterval is set as %lu second",
                         m_queryRetryInterval );
		}
		m_pDriver = (sql::Driver*)get_driver_instance();  // for mysql db connection
		m_reaper->start();//for testing
		
	}
	
	
	/**
	* ~DbServerLocator
	* 
	* Sole destructor that deletes all of the OCIConnection objects.
	*/
	DbServerLocator::~DbServerLocator()
	{
		// Each OCIConnection object in the map must be destructed individually.
		
		ThreadGuard guard( m_lock );
		
		LOG( SourceInfo, DebugUtil::GenericLog, 
			DebugUtil::DebugDebug, "Destroying connections" );
		std::map<const std::string, IDbConnection*>::iterator it; 
		for( it = m_connections.begin(); it != m_connections.end(); it++ )
		{
			delete it->second;
		} 

		m_connections.clear();
		
		// Terminate the connection reaper.
		
		if ( m_reaper.get() != NULL )
		{
			m_reaper->terminateAndWait();
		}
	}
	
	
	/**
	* createConnection
	*
	* Sets up an OCIConnection for the given database including a session for
	* the specified user.  However, open() must be called to start the 
	* session before exec() can be used.
	*
	* In the event of auto-reconnect, a connection to the new database will be made
	* even if the old connection is still in use.  All new queries will be 
	* directed to through the new connection.
	*
	* Use this version of createConnection if the connectionStr hostName,
	* databaseName, userName and password are not already separated out.
	*
	* @param connectionStr     the string that identifies the database:
	*                          databaseName:userName:password:hostName
	* @param isAutoReconnect     the database is being automatically reconnected, so create
	*                          a new connection even if one already exists
	* @return true if the connection was set up successfully, false otherwise.
	*/
	bool DbServerLocator::createConnection( const std::string& connectionStr, 
		bool isAutoReconnect )
	{
		std::vector<std::string> connDetails;
		parseConnectStr( connectionStr, connDetails, ':' );
		
		TA_ASSERT( CONN_STR_PARAM_MIN_COUNT == connDetails.size(), "Invalid connection string" );		
		
		return createConnection( connectionStr, connDetails[CON_STR_DBType], connDetails[CON_STR_SchemaName], connDetails[CON_STR_UserName], connDetails[CON_STR_Passwd], connDetails[CON_STR_Hostname], isAutoReconnect );
	}
	
	
	/**
	* createConnection
	*
	* Sets up an OCIConnection for the given database including a session for
	* the specified user.  However, open() must be called to start the 
	* session before exec() can be used.
	*
	* In the event of auto-reconnect, a connection to the new database will be made
	* even if the old connection is still in use.  All new queries will be 
	* directed to through the new connection.
	*
	* Use this version of createConnection if the connectionStr hostName,
	* databaseName, userName and password are already separated out for efficiency.
	*
	* @param connectionStr     the string that identifies the database:
	*                          databaseName:userName:password:hostName
	* @param databaseName      the name of the database to connect to
	* @param userName          the name of the user for the session
	* @param password          the name of the password for the session
	* @param hostName          the name of the host that the database is on
	* @param isAutoReconnect     the database is being automatically reconnected, so create
	*                          a new connection even if one already exists
	* @return true if the connection was set up successfully, false otherwise.
	*/
	bool DbServerLocator::createConnection( const std::string& connectionStr, 
		const std::string& strDbType,
		const std::string& databaseName, 
		const std::string& userName,
		const std::string& password,
		const std::string& hostName,
		bool isAutoReconnect )
	{
        if (connectionStr.empty() || strDbType.empty() || databaseName.empty() || userName.empty() || password.empty())
        {
            LOG6( SourceInfo, DebugUtil::DebugWarn, "can not create a new oracle connection for connectionStr[%s], DbType[%s], databaseName[%s], userName[%s], password[%s], hostName[%s]",
                connectionStr.c_str(), strDbType.c_str(), databaseName.c_str(), userName.c_str(), password.c_str(), hostName.c_str() );
            return false;
        }
		// Only one thread should be able to set up a new connection at once,
		// because we don't want the same connection set up twice.
		
		//ThreadGuard guard( m_lockConnection );
		
		IDbConnection* connection;
		
		// Determine if we already have this connection stored.
		// If it is not automatically reconnecting, an existing connection can be used
		// if one exists.
		
		if ( !isAutoReconnect && connectionExists(connectionStr) )
		{
			return true;
		}
		
		// If it is automatically reconnecting or there is not an existing connection
		// create a new OCIConnection to handle sessions with the given details.
		try{
			connection = createDbConnection( connectionStr, strDbType, databaseName, userName, 
				password, hostName );
			
			// If so, add it to the map of connections referenced by the
			// database connection string.
			
			if ( !isAutoReconnect )
			{
				// If this isAutoReconnect is true, this function is called from the
				// callback function and the lock is already guarded.
				
				ThreadGuard lockGuard( m_lock );
			}
			// Use [] instead of insert so that if this connection is being refreshed,
			// the new connection will replace the old connection object.  The 
			// ConnectionReaper will clean up the old object.
			connection->open();

		}
		catch(DBException &e)		
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "can not open DB connection %s ", connectionStr.c_str() );
			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "error is %s:", e.getReason().c_str());
			TA_THROW( DBException("Database_ERROR", e.getReason(), e.getNativeCode()) )
	//		return false;
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "can not create a new DB connection %s" , connectionStr.c_str() );
			throw;
	//		return false;
		}

		ThreadGuard lockGuard( m_lock );
		m_connections[connectionStr] = connection;
		return true;
	}
	
	
	/**
	* open
	* 
	* Opens the connection to the specified database.  This will actually
	* start a new session with the set up connection to the given database
	* if there is not one already running.
	*
	* @param connectionStr     the string that identifies the database:
	*                          databaseName:userName:password:hostName
	* @return true if the database was opened, false otherwise
	*/
	void DbServerLocator::open( const std::string& connectionStr )
	{
		IDbConnection* connection = getConnection( connectionStr );
		
		connection->open();
		connection->decrementExecutionCount();
	}
	
	bool DbServerLocator::isOpen( const std::string& connectionStr )
	{
		try
		{
		IDbConnection* connection = getConnection( connectionStr );
		connection->decrementExecutionCount();
		return connection->isOpen();
	}
		catch (const DBException& e)
		{
			// in case of the connection is not in the connection map, just return false,and open it later
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "DbServerLocator::isOpen():the DB connection string is: %s, Reason: %s" , 
				connectionStr.c_str(), e.getReason().c_str() );
			return false;
		}		
		
	}
	
	/**
	* close
	* 
	* Closes the connection to the specified database.  This will actually
	* end the existing session with the set up connection to the given database
	* if one is running.
	*
	* @param connectionStr     the string that identifies the database:
	*                          databaseName:userName:password:hostName
	* @return true if the database was closed, false otherwise
	*/
	void DbServerLocator::close( const std::string& connectionStr )
	{ 
		if ( !connectionExists(connectionStr) )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Tried to close an invalid connection:" );
			LOGMORE( SourceInfo, connectionStr.c_str() );
			return;        // Return true, because this connection is closed!
		}
		
		IDbConnection* connection = getConnection( connectionStr );
		connection->decrementExecutionCount();		
		connection->close();
		ThreadGuard guard( m_lock );
		m_connections.erase(connectionStr); // erase the connection in the map when close
	}
	
	bool DbServerLocator::connectionExists( const std::string& connectionStr ) const
	{
		// Thread guard this method so that the iterator cannot be 
		// invalidated if new connections are added while the map is
		// being searched.
		
		ThreadGuard guard( m_lock );
		
		// Find the OCIConnection object with the given connection string.
		
		std::map<const std::string, IDbConnection*>::const_iterator it;
		
		return( m_connections.find( connectionStr ) != m_connections.end() );
	}
	
	/**
	* closeAll
	* 
	* Closes ALL connections to the specified database. 
	*
	* @param connectionStr     the string that identifies the database:
	*                          databaseName:userName:password:hostName
	* @return true if the database was closed, false otherwise
	*/
	void DbServerLocator::closeAll( const std::string& connectionStr )
	{
		if ( !connectionExists(connectionStr) )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Tried to close an invalid connection: %s",
				connectionStr.c_str() );
			return;
		}
		
		IDbConnection* connection = getConnection( connectionStr );
		
		connection->closeAll();
		connection->decrementExecutionCount();
		ThreadGuard guard( m_lock );
		m_connections.clear();  // erase all when close all connections
	}
	
	
	
	/**
	* exec
	*
	* Executes the given statement on the database.  This method
	* handles queries (e.g. "SELECT...").
	*
	* @param connectionStr  the string that identifies the database:
	*                       databaseName:userName:password:hostName
	* @param statement      the statement to execute
	* @param data           a buffer in which to put the retrieved data
	* @param numColumns     the number of columns to retrieve
	* @param numRows        the maximum number of rows to fetch at a time 
	*                       ( The maximum numRows is SimpleDb::MAX_ROWS )
	* @param bindType       the type of binding required
	* @return true if the statement was executed, false otherwise
	*/
	//ocl::OraQuery* DbServerLocator::exec( const std::string& connectionStr, 
	IQueryAdapter* DbServerLocator::exec( const std::string& connectionStr, 
										  const SQLStatement& rSqlObj, int numRows )
	{
		ThreadGuard guard( m_lock );

		IDbConnection* connection = NULL;
		
		connection = getConnection( connectionStr );
		//LOG( DebugUtil::DebugInfo, "Open(), ThreadId: " << omni_thread::self()->id());
/*		unsigned long queryRetryInterval = 0;
		if (RunParams::getInstance().isSet("QueryRetryInterval"))
		{
			queryRetryInterval = atol(RunParams::getInstance().get("QueryRetryInterval").c_str());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "QueryRetryInterval is set as %lu second", queryRetryInterval);
		}
*/		
		unsigned long retry = 0;
		bool execOk = false;
		
		unsigned long nTrytimes = NUM_RETRIES;
		
        IQueryAdapter* query = NULL;
		//ocl::OraQuery *query = NULL;
		while ( !execOk && retry < nTrytimes )
		{
			// Execute the statement.
			/*if ( statement.size() > defMAXLOGMESGSIZE)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,"Begin execute the LargeSQL statement: [[");
				LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, statement.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL, "Begin execute the SQL statement: [[%s]]", statement.c_str());
            }*/

			const int prefetch = numRows;
			try
			{
				connection->exec(rSqlObj, query, true, prefetch);
				execOk = true;
			}
			catch( const DBException& e)
			{
				retry++;
                if ( NULL != query )
                {
                    cleanQuery( connectionStr, query );
				}
				//OCIConnection::cleanUpQuery(query);
				TA_ASSERT(query==NULL, "query should be null");
				//LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec: %s", statement.c_str() );
				if (m_queryRetryInterval != 0)
				{
					if (e.getNativeCode() == 1013)
					{
						TA_Base_Core::Thread::sleep(m_queryRetryInterval * 1000);
					}
				}
				//since the timeout is not set here, we need try more times
				if ( retry < NUM_RETRIES )
				{
					LOGMORE( SourceInfo, "Retrying..." );
					continue;
				}
				else
				{
					connection->decrementExecutionCount();
					std::ostringstream osMsg;
					osMsg << "Status:" << e.getStatus() << " Summary:" << e.getSummary() << " Reason:" << e.getReason() << std::endl; 
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "\n\t[database error]: %s" , osMsg.str().c_str());
					throw e; //cause the agent or application down right now, maybe.
				}
			}
		}
		
		connection->decrementExecutionCount();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL, "End execute the SQL statement");
		
		return query;
	}
	
	
	/**
	* exec
	*
	* Executes the given statement on the database.  This method
	* handles non-queries (e.g."INSERT...").
	*
	* @param connectionStr  the string that identifies the database:
	*                       databaseName:userName:password:hostName
	* @param statement      the statement to execute
	* @return true if the statement was executed, false otherwise
	*/
	void DbServerLocator::exec( const std::string& connectionStr, const SQLStatement& rSqlObj )
	{
		ThreadGuard guard( m_lock );
		IDbConnection* connection =  NULL;

		// If the exec fails the first time, try again NUM_RETRIES times.  The
		// exec may succeed on successive calls if, for example, the database
		// was in the middle of automatically reconnecting last time, but is now back up and running.

		connection = getConnection( connectionStr );

		int retry = 0;
		bool execOk = false;

		while ( !execOk && retry < NUM_RETRIES )
		{    
			IQueryAdapter* query = NULL;
			//ocl::OraQuery *query = NULL;
			/*if ( statement.size() > defMAXLOGMESGSIZE)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,"Begin execute the LargeSQL statement: [[");
				LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, statement.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL, "Begin execute the SQL statement: [[%s]]", statement.c_str());
			}*/

			try
			{
				connection->exec(rSqlObj, query, false);
				//OCIConnection::cleanUpQuery(query);
				if ( NULL != query )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "execute none query command should not return query" );
					cleanQuery( connectionStr, query );
				}

				TA_ASSERT(query==NULL, "query should be null");
				execOk = true;
			}
			catch( const DBException& e)
			{
				retry++;
				//OCIConnection::cleanUpQuery(query);
				if ( NULL != query )
				{
					cleanQuery( connectionStr, query );
				}
				TA_ASSERT(query==NULL, "query should be null");

				//LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec: %s" , statement.c_str() );

				if ( retry < NUM_RETRIES )
				{
					LOGMORE( SourceInfo, "Retrying..." );
					continue;
				}
				else
				{
					connection->decrementExecutionCount();
					std::ostringstream osMsg;
					osMsg << "Status:" << e.getStatus() << " Summary:" << e.getSummary() << " Reason:" << e.getReason() << std::endl; 
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "\n\t[database error]: %s" , osMsg.str().c_str());
					throw e;
				}
			}
		}

		connection->decrementExecutionCount();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL, "End execute the SQL statement");
	}
	
	void DbServerLocator::readBLOB( const std::string& connectionStr, const std::string& fieldName, const std::string& tableName,
		const std::string& whereClause, std::vector<unsigned char>& data )
	{
		//fix bug: multi threads safe problem
		ThreadGuard guard( m_lock );

		
		TA_ASSERT( fieldName.length() > 0, "The name of the BLOB field has not been specified" );
		TA_ASSERT( tableName.length() > 0, "The name of the BLOB table has not been specified" );
		
		// It's important that the data vector is empty
		data.clear();
		
		// Format a SELECT statement
		std::stringstream selectStatement;
		
		selectStatement << "SELECT " << fieldName << " FROM " << tableName;
		if ( whereClause.length() > 0 )
		{
			selectStatement << " WHERE " << whereClause;
		}
		
		int numRows = 1;
		int numColumns = 1;
		
		IDbConnection* connection = getConnection( connectionStr );
		
		int retry = 0;
		bool execOk = false;
		
		DataClobArray dataArray;
		while( !execOk && retry < NUM_RETRIES )
		{
			try
			{
				connection->fetchBLOB(selectStatement.str(), fieldName, dataArray);
				execOk = true;
			}
			catch( const DBException& )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to exec SQL statement '%s'",
					selectStatement.str().c_str() );
				retry++;
				
				if( retry < NUM_RETRIES )
				{
					LOGMORE( SourceInfo, "Retrying..." );
					continue;
				}
				else
				{
					connection->decrementExecutionCount();
					throw;
				}
			}
		}
		
		connection->decrementExecutionCount();
		
		if (dataArray.size() < 1)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to get clob '%s'", fieldName.c_str());
		}else
		{
			data = dataArray.at(0);
		}
		
	}
	
	
	void DbServerLocator::writeBLOB( const std::string& connectionStr, const std::string& fieldName, const std::string& tableName,
						  const std::string& whereClause, std::vector<unsigned char>& data )
	{
		//fix bug: multi threads safe problem
		ThreadGuard guard( m_lock );
		
		TA_ASSERT( fieldName.length() > 0, "The name of the BLOB field has not been specified" );
		TA_ASSERT( tableName.length() > 0, "The name of the BLOB table has not been specified" );
		
		// Format a SELECT statement
		/*std::stringstream selectStatement;
		
		selectStatement << "SELECT " << fieldName << " FROM " << tableName;
		if ( whereClause.length() > 0 )
		{
			selectStatement << " WHERE " << whereClause;
		}
		selectStatement << " FOR UPDATE";*/
		
		int numRows = 1;
		int numColumns = 1;
		
		IDbConnection* connection = getConnection( connectionStr );
		
		int retry = 0;
		bool execOk = false;
		
		while( !execOk && retry < NUM_RETRIES )
		{
			try
			{
				connection->updateBLOB(tableName, fieldName, data, whereClause);
				execOk = true;
			}
			catch( const DBException& )
			{
				/*LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to exec SQL statement '%s'",
					selectStatement.str().c_str() );*/
				retry++;
				
				if( retry < NUM_RETRIES )
				{
					LOGMORE( SourceInfo, "Retrying..." );
					continue;
				}
				else
				{
					connection->decrementExecutionCount();
					throw;
				}
			}
		}
		
		connection->decrementExecutionCount();
	}
	
	
	
	/**
	* getConnection
	* 
	* Returns a pointer to the newest connection with the specified connection string.
	* Increments the count of threads using the connection at this moment.
	* A matching call to OCIConnection::decrementExecutionCount() should be called 
	* every time getConnection() is called (after the connection has been used).
	* This will maintain the correct current usage count.
	*
	* @param connectionStr     the string that identifies the connection:
	*                          databaseName:userName:password:hostName.
	* @return a pointer to the specified connection 
	*/
	IDbConnection* DbServerLocator::getConnection( const std::string& connectionStr )
	{
		// Thread guard this method so that the iterator cannot be 
		// invalidated if new connections are added while the map is
		// being searched.
		
		ThreadGuard guard( m_lock );
		
		// Find the OCIConnection object with the given connection string.
		
		std::map<const std::string, IDbConnection*>::iterator it;
		it = m_connections.find( connectionStr );
		
		if( it == m_connections.end() )
		{
			std::string assertMsg("Invalid connection: ");
			assertMsg += connectionStr;
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to get connection: '%s'", assertMsg.c_str() );
			throw DBException("DB Connection Error", assertMsg.c_str(), -1);

			//TA_ASSERT( false, assertMsg.c_str() );
		}
		
		// Increment the count of the number of threads using this connection
		// right now, then return the connection.
		
		it->second->incrementExecutionCount();
		return it->second;
	}

	/**
	* CreateDbConnection
	*
	* Returns a pointer to the newest connection according to the database type.
	*
	*/
	IDbConnection* DbServerLocator::createDbConnection(
		/*[in]*/ const std::string& connectionStr, 
		/*[in]*/ const std::string& strDbType, 
		/*[in]*/ const std::string& databaseName, 
		/*[in]*/ const std::string& userName,
		/*[in]*/ const std::string& password,
		/*[in]*/ const std::string& hostName
		)
	{
		FUNCTION_ENTRY("DbServerLocator::createDbConnection");


#if (defined(USING_OTL))
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,	"defined USING_OTL");
#endif

#if (defined(OTL_ODBC))
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,	"defined OTL_ODBC");
#endif

#if (defined(OTL_ODBC_UNIX))
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,	"defined OTL_ODBC_UNIX");
#endif

#if (defined(OTL_ORA10G))
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,	"defined OTL_ORA10G");
#endif

#if (defined(OTL_ORA11G))
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,	"defined OTL_ORA11G");
#endif

		IDbConnection* pDbConnection = NULL;		
		try
		{			
			int nDbType = _ParseDBType(strDbType);
			switch (nDbType)
			{
			case enumOracleDb:

#if (defined(USING_OTL))
	#if (defined(OTL_ODBC))
			pDbConnection = new ODBCOTLConnection( connectionStr, databaseName, userName, password, hostName ); //OTL ODBC
	#else
			pDbConnection = new OCIOTLConnection( connectionStr, databaseName, userName, password, hostName ); //OTL OCI
	#endif	
#else
		pDbConnection = new OCIConnection( connectionStr, databaseName, userName, password, hostName ); //OCL
#endif
				
				break;
			case enumMysqlDb:
				TA_ASSERT(NULL != m_pDriver, "the Mysql database Driver is null");
				pDbConnection = new MysqlConnection( m_pDriver, connectionStr, databaseName, userName, 
					password, hostName );
				break;
			default:
				break;
			}
			
			TA_ASSERT(NULL != pDbConnection, "create database connection failed!");				
		}
		catch (const DBException& )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "function: createDbConnection Exception!");
			throw;
		}	
		
		FUNCTION_EXIT;	
		return pDbConnection;
	}
	
	
	/** 
	* refreshConnections
	*
	* All database connections maintained by this object are regenerated.  The existing
	* connections are cleaned up by the ConnectionReaper thread once they are no longer
	* being used.
	*/
	void DbServerLocator::refreshConnections()
	{
		
		// Don't allow threads to get or create connections while the connections
		// are being updated.  Threads executing on existing connections that have
		// already called getConnection() will not be locked out of their execution.
		
		ThreadGuard guard( m_lock );
		
		std::map<const std::string, IDbConnection*>::iterator it = m_connections.begin();
		while ( it != m_connections.end() )
		{
			try
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Refreshing connections: %s",  it->first.c_str());
				// remember the old connection
				IDbConnection* oldConnection = it->second;
				
				// try to make a new connection
				if (createConnection( it->first, true ))
				{
					// Tell the ConnectionReaper to monitor this connection and once it
					// is not being used, delete it.
					m_reaper->reapConnection( oldConnection );
				}
                else
                {
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "failed in create new connections, keep the old connection %s", it->first.c_str() );
				}
			}
			catch( const DBException& )
			{
				// if that didn't work then keep the old one!
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
					"Failed to refresh connection: %s", it->first.c_str() );
				LOGMORE( SourceInfo, "Keeping old connection" );
			}
			
			it++;
		}
	}
	
	
	/**
	* parseConnectStr()
	*
	* Parses the given string, extracting delimiter-separated string components
	* and storing them in a vector.
	*
	* @param str           the string to parse
	* @param components    the vector that will contain the parsed components
	* @param delimiter     the character that identifies where the string should be split.
	*/
	void DbServerLocator::parseConnectStr( const std::string& str,                  // IN
								std::vector<std::string>& components,    // OUT
								char delimiter )                         // IN
	{
		// KT 19/08/2002 : This has been moved from SimpleDb into here so it can be
		// used in auto-reconnect situations.
		
		// Not the most efficient implementation, but pretty intuitive
		components.resize(0);
		std::string comp;
		for (unsigned int i = 0; i < str.size(); i++)
		{
			if (str[i] == delimiter)
			{
				components.push_back(comp);
				comp = "";
			}
			else
			{
				comp += str[i];
			}
		}
		components.push_back(comp);
	}

	/**
	* setDbServerType()
	*
	* Set the database Server that will be used in the system.
	* this method should be invoke before connecting to the target database
	*
	* @param nDbType       the database type. see the database type enumeration defined in the top
	*/
	void DbServerLocator::setDbServerType(int nDbType)
	{
		/*ThreadGuard guard( m_lock );
		switch (nDbType)
		{
		case enumOracleDb:
			m_enumDbType = enumOracleDb;
			break;
		case enumMysqlDb:
			m_enumDbType = enumMysqlDb;
			break;
		case enumMSSvrDb:
			m_enumDbType = enumMSSvrDb;
			break;
		default:
			m_enumDbType = enumOracleDb;
			break;
		}*/
	}

    int DbServerLocator::_ParseDBType(const std::string& strDbType)
    {
        int nRet = 0;

        if (boost::iequals(strDbType, defOracleDBName)) //no case sensitive
        {
            nRet = static_cast<int>(DbServerLocator::enumOracleDb);
        }
        else if (boost::iequals(strDbType, defMysqlDBName))
        {
            nRet = static_cast<int>(DbServerLocator::enumMysqlDb);
        }
        else   
        {    // should never reach here, if reach here, then raise a exception
            LOG_GENERIC( SourceInfo, 
                TA_Base_Core::DebugUtil::DebugError,
                "the DB type string in database config file wrong(should be: Oracle or Mysql): %s", strDbType.c_str() );
            TA_ASSERT(false, "DB Type string error");  // make a exception deliberately
        }

        return nRet;
    }

	/**
	* getDbServerType()
	*
	* get the database Server that in used in the system.
	*
	* @return the database type. see the database type enumeration defined at the top
	*/
	/*int DbServerLocator::getDbServerType()
        {
		return static_cast<int>(m_enumDbType);
	}*/

    void DbServerLocator::cleanQuery( const std::string& strConnStr, IQueryAdapter*& pQuery )
    {
        if ( NULL == pQuery )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "invalid input parameter: NULL pointer of QueryAdapter" );
            return;
        }
        if ( strConnStr.empty() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "invalid input parameter: empty connection string" );
            return;
    }

        IDbConnection* pConnection = getConnection( strConnStr );
        if ( NULL == pConnection )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can't't get connect by connection string: %s", strConnStr.c_str() );
            return;
}

        pConnection->cleanQuery( pQuery );

        pConnection->decrementExecutionCount();
    }
}
