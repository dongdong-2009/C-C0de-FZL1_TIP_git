/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/OCIAccess.cpp $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* OCIAccess is a singleton that manages all database connections
* using the ORACLE OCI interface.
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
#include "core/database/src/QueryAdapter.h"
#include "core/database/src/OCIAccess.h"
#include "core/database/src/OCIConnection.h"
#include "core/database/src/ConnectionReaper.h"

#include <boost/lexical_cast.hpp>


namespace TA_Base_Core
{
	const int OCIAccess::NUM_RETRIES = 3;
	
	using TA_Base_Core::ThreadGuard;
	using TA_Base_Core::DebugUtil;
	
	// Definition of static member variables.
	TA_Base_Core::ReEntrantThreadLockable OCIAccess::m_singletonLock;
	OCIAccess* OCIAccess::m_instance = NULL;
	
	
	////////////////////////////////////////////////////////////////////////////////
	//                                                                            //
	//                         Construction/Destruction                           //
	//                                                                            //
	////////////////////////////////////////////////////////////////////////////////
	
	
	/**
	* getInstance
	* 
	* OCIAccess is a singleton, so this method returns a reference to the active
	* OCIAccess object and creates a new OCIAccess if one does not exist already.
	*
	* @return  a reference to the active OCIAccess singleton object
	*/
	OCIAccess& OCIAccess::getInstance()
	{
		// TD11793: The OCIAccess instance was being deleted while it was still in use
		// with the old singleton implementation. Using a lock and a heap bound object
		// should avoid this situation.
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		
		if ( NULL == m_instance )
		{
			m_instance = new OCIAccess();
		}
		
		return *m_instance;
	}
	
	
	/**
	* removeInstance
	* 
	* OCIAccess is a singleton, so it wont delete itself
	* this should be called in the cleanup of your app (called by generic gui and generic agent)
	*
	*/
	void OCIAccess::removeInstance()
	{
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = 0;
		}
	}
	
	
	/**
	* OCIAccess
	* 
	* Sole constructor.  This class is a singleton, hence the constructor
	* is private.  Use getInstance() to obtain a reference to the OCIAccess
	* object.
	*/
	OCIAccess::OCIAccess()
        : m_reaper(new ConnectionReaper()),
          m_queryRetryInterval( 0 )
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
		
		m_reaper->start();//for testing
		
	}
	
	
	/**
	* ~OCIAccess
	* 
	* Sole destructor that deletes all of the OCIConnection objects.
	*/
	OCIAccess::~OCIAccess()
	{
		// Each OCIConnection object in the map must be destructed individually.
		
		ThreadGuard guard( m_lock );
		
		LOG( SourceInfo, DebugUtil::GenericLog, 
			DebugUtil::DebugDebug, "Destroying connections" );
		std::map<const std::string, OCIConnection*>::iterator it; 
		for( it = m_connections.begin(); it != m_connections.end(); it++ )
		{
			delete it->second;
		} 
		
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
	bool OCIAccess::createConnection( const std::string& connectionStr, 
		bool isAutoReconnect )
	{
		std::vector<std::string> connDetails;
		parseConnectStr( connectionStr, connDetails, ':' );
		
		TA_ASSERT( 4 == connDetails.size(), "Invalid connection string" );
		
		return createConnection( connectionStr, connDetails[0], connDetails[1], connDetails[2], connDetails[3], isAutoReconnect );
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
	bool OCIAccess::createConnection( const std::string& connectionStr, 
		const std::string& databaseName, 
		const std::string& userName,
		const std::string& password,
		const std::string& hostName,
		bool isAutoReconnect )
	{
        if (connectionStr.empty() || databaseName.empty() || userName.empty() || password.empty())
        {
            LOG5( SourceInfo, DebugUtil::DebugWarn, "can not create a new oracle connection for connectionStr[%s], databaseName[%s], userName[%s], password[%s], hostName[%s]",
                connectionStr.c_str(), databaseName.c_str(), userName.c_str(), password.c_str(), hostName.c_str() );
            return false;
        }
		// Only one thread should be able to set up a new connection at once,
		// because we don't want the same connection set up twice.
		
		ThreadGuard guard( m_lockConnection );
		
		OCIConnection* connection;
		
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
			connection = new OCIConnection( connectionStr, databaseName, userName, 
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
			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "can not open oracle connection %s ", connectionStr.c_str() );
			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "error is %s:", e.getReason().c_str());
			return false;
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "can not create a new oracle connection %s" , connectionStr.c_str() );
			return false;
		}
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
	void OCIAccess::open( const std::string& connectionStr )
	{
		OCIConnection* connection = getConnection( connectionStr );
		
		connection->open();
		connection->decrementExecutionCount();
	}
	
	bool OCIAccess::isOpen( const std::string& connectionStr )
	{
		OCIConnection* connection = getConnection( connectionStr );
		connection->decrementExecutionCount();
		return connection->isOpen();
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
	void OCIAccess::close( const std::string& connectionStr )
	{ 
		if ( !connectionExists(connectionStr) )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Tried to close an invalid connection:" );
			LOGMORE( SourceInfo, connectionStr.c_str() );
			return;        // Return true, because this connection is closed!
		}
		
		OCIConnection* connection = getConnection( connectionStr );
		connection->decrementExecutionCount();		
		connection->close();

	}
	
	bool OCIAccess::connectionExists( const std::string& connectionStr ) const
	{
		// Thread guard this method so that the iterator cannot be 
		// invalidated if new connections are added while the map is
		// being searched.
		
		ThreadGuard guard( m_lock );
		
		// Find the OCIConnection object with the given connection string.
		
		std::map<const std::string, OCIConnection*>::const_iterator it;
		
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
	void OCIAccess::closeAll( const std::string& connectionStr )
	{
		if ( !connectionExists(connectionStr) )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Tried to close an invalid connection: %s",
				connectionStr.c_str() );
			return;
		}
		
		OCIConnection* connection = getConnection( connectionStr );
		
		connection->closeAll();
		connection->decrementExecutionCount();
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
	//ocl::OraQuery* OCIAccess::exec( const std::string& connectionStr, 
    QueryAdapter* OCIAccess::exec( const std::string& connectionStr, 
		const std::string& statement, int numRows )
	{
		ThreadGuard guard( m_lock );

		OCIConnection* connection = NULL;
		
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
		
        QueryAdapter* query = NULL;
		//ocl::OraQuery *query = NULL;
		while ( !execOk && retry < nTrytimes )
		{
			// Execute the statement.
			const int prefetch = numRows;
			try
			{
				connection->exec(statement, query, true, prefetch);
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
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec: %s", statement.c_str() );
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
					std::ostringstream osMsg;
					osMsg << "Status:" << e.getStatus() << " Summary:" << e.getSummary() 
					      << " Reason:" << e.getReason() <<" NativeCode: "<< e.getNativeCode() << std::endl; 
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "\n\t[database error]: %s" , osMsg.str().c_str());
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
		
		// yanrong++
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Save query pair query=%p, connectionStr=%s, before insertion map size=%d", query, connectionStr.c_str(), m_queryMap.size());
		//m_queryMap[query] = connectionStr;
		// ++yanrong
		
		return query;
	}

	// yanrong++
	//bool OCIAccess::cleanUpQuery( ocl::OraQuery*& query )
	//{
	//	FUNCTION_ENTRY( "cleanUpQuery" );
	//	ThreadGuard guard( m_lock );
	//	if( query != NULL )
	//	{
	//		OraQueryMap::iterator iter = m_queryMap.find(query);
	//		if( iter != m_queryMap.end() )
	//		{
	//			OCIConnection* connection = getConnection( iter->second.c_str() );
	//			connection->notifyCleanUpQuery();
	//			m_queryMap.erase(iter);
	//		}

	//		OCIConnection::cleanUpQuery(query);
	//	}

	//	FUNCTION_EXIT;
	//	return true;
	//}
	// ++yanrong
	
	
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
	void OCIAccess::exec( const std::string& connectionStr, const std::string& statement )
	{
		ThreadGuard guard( m_lock );
		OCIConnection* connection =  NULL;
		
		// If the exec fails the first time, try again NUM_RETRIES times.  The
		// exec may succeed on successive calls if, for example, the database
		// was in the middle of automatically reconnecting last time, but is now back up and running.
		
		connection = getConnection( connectionStr );
		
		int retry = 0;
		bool execOk = false;
		
		while ( !execOk && retry < NUM_RETRIES )
		{    
            QueryAdapter* query = NULL;
			//ocl::OraQuery *query = NULL;
			try
			{
				connection->exec(statement, query, false);
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
				
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec: %s" , statement.c_str() );
				
				if ( retry < NUM_RETRIES )
				{
					std::ostringstream osMsg;
					osMsg << "Status:" << e.getStatus() << " Summary:" << e.getSummary() 
						  << " Reason:" << e.getReason() << " NativeCode: " << e.getNativeCode() << std::endl; 
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "\n\t[database error]: %s" , osMsg.str().c_str());
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
	}
	
	void OCIAccess::readBLOB( const std::string& connectionStr, const std::string& fieldName, const std::string& tableName,
		const std::string& whereClause, std::vector<unsigned char>& data )
	{
		
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
		
		OCIConnection* connection = getConnection( connectionStr );
		
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
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to get clob '%s'");
		}else
		{
			data = dataArray.at(0);
		}
		
	}
	
	
	void OCIAccess::writeBLOB( const std::string& connectionStr, const std::string& fieldName, const std::string& tableName,
						  const std::string& whereClause, std::vector<unsigned char>& data )
	{
		
		TA_ASSERT( fieldName.length() > 0, "The name of the BLOB field has not been specified" );
		TA_ASSERT( tableName.length() > 0, "The name of the BLOB table has not been specified" );
		
		// Format a SELECT statement
		std::stringstream selectStatement;
		
		selectStatement << "SELECT " << fieldName << " FROM " << tableName;
		if ( whereClause.length() > 0 )
		{
			selectStatement << " WHERE " << whereClause;
		}
		selectStatement << " FOR UPDATE";
		
		int numRows = 1;
		int numColumns = 1;
		
		OCIConnection* connection = getConnection( connectionStr );
		
		int retry = 0;
		bool execOk = false;
		
		while( !execOk && retry < NUM_RETRIES )
		{
			try
			{
				connection->updateBLOB(selectStatement.str(), fieldName, data);
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
	OCIConnection* OCIAccess::getConnection( const std::string& connectionStr )
	{
		// Thread guard this method so that the iterator cannot be 
		// invalidated if new connections are added while the map is
		// being searched.
		
		ThreadGuard guard( m_lock );
		
		// Find the OCIConnection object with the given connection string.
		
		std::map<const std::string, OCIConnection*>::iterator it;
		it = m_connections.find( connectionStr );
		
		if( it == m_connections.end() )
		{
			std::string assertMsg("Invalid connection: ");
			assertMsg += connectionStr;
			
			TA_ASSERT( false, assertMsg.c_str() );
		}
		
		// Increment the count of the number of threads using this connection
		// right now, then return the connection.
		
		it->second->incrementExecutionCount();
		return it->second;
	}
	
	
	/** 
	* refreshConnections
	*
	* All database connections maintained by this object are regenerated.  The existing
	* connections are cleaned up by the ConnectionReaper thread once they are no longer
	* being used.
	*/
	void OCIAccess::refreshConnections()
	{
		
		// Don't allow threads to get or create connections while the connections
		// are being updated.  Threads executing on existing connections that have
		// already called getConnection() will not be locked out of their execution.
		
		ThreadGuard guard( m_lock );
		
		std::map<const std::string, OCIConnection*>::iterator it = m_connections.begin();
		while ( it != m_connections.end() )
		{
			try
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Refreshing connections: %s",  it->first.c_str());
				// remember the old connection
				OCIConnection* oldConnection = it->second;
				
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
	void OCIAccess::parseConnectStr( const std::string& str,                  // IN
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

    void OCIAccess::cleanQuery( const std::string& strConnStr, QueryAdapter*& pQuery )
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

        OCIConnection* pConnection = getConnection( strConnStr );
        if ( NULL == pConnection )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can't't get connect by connection string: %s", strConnStr.c_str() );
            return;
        }

        pConnection->cleanQuery( pQuery );

        pConnection->decrementExecutionCount();
    }
}
