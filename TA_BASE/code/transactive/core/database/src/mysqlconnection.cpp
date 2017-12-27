//////////////////////////////////////////////////////////////////////
///		@file		MysqlConnection.cpp
///		@author		zhilin,ouyang
///		@date		2010-11-25 15:44:41
///
///		@brief	    Implementation of the interface IDbConnection. 
///					use mysql database server to implement the   
///                 database connection operations.
//////////////////////////////////////////////////////////////////////
#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#include <algorithm>
#include "driver/mysql_public_iface.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/MysqlQueryAdapter.h"
#include "core/database/src/MysqlConnection.h"
#include "core/database/src/DBException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"


using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::Thread;

NS_BEGIN(TA_Base_Core)

const long QUERY_EXIST = 1;
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                         Construction/Destruction                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/**
* MysqlConnection
* 
* Sole constructor that stores the database connection details and 
* allocates handles to perform transactions with the database.
* Note: this method can be unsuccessful, so checkIsValid() should be 
* called afterwards to check that construction was successful.
* 
* @param connectionStr databaseName:userName:password:hostName
* @param databaseName  the name of the database to connect to
* @param userName      the name of the user for the session
* @param password      the name of the password for the session
* @param hostName      the name of the host that the database is on
*/
MysqlConnection::MysqlConnection( sql::Driver* pDriver,
	const std::string& connectionStr, 
	const std::string& databaseName, 
	const std::string& userName, 
	const std::string& password, 
	const std::string& hostName /*= ""*/ )
	: IDbConnection(
	 connectionStr
	,databaseName
	,userName
	,password
	,hostName)
	,m_pDriver(pDriver)
	,m_pConnection(NULL)
//	,m_pStatment(NULL)
{
	createConnection();
}


/**
* ~MysqlConnection
* 
* Sole standard destructor.  Frees the handles allocated in the constructor.
*/
MysqlConnection::~MysqlConnection()
{
    {
        ThreadGuard guard( m_queryLock );

        for ( std::map<IQueryAdapter*, long>::iterator itLoop = m_mapActiveQuery.begin();
                m_mapActiveQuery.end() != itLoop; ++itLoop )
        {
            try
            {
                if ( NULL == itLoop->first )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of MysqlQueryAdapter" );
                    continue;
                }

                if ( !itLoop->first->cleanQuery() )
                {
                    delete (itLoop->first);
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "MysqlQueryAdapter: 0x%p is deleted by ConnectionReaper in destructor of MysqlConnection", itLoop->first );
                }
                // else leave it to SimpleDb for deleting
            }
            catch (...)
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "cleanQuery: Unknown exception" );
            }
        }

        m_mapActiveQuery.clear();
    }

    // The connection is closed when the server is detached.
	try
	{
		//fix bug: when mysql db server offline, 
		//MysqlConnection::createConnection() will set m_pConnection=NULL, so we need to check m_pConnection's value here
		if (NULL != m_pConnection)
		{
			m_pConnection->close();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in destruction function.");	
			delete m_pConnection;
			m_pConnection = NULL;
		}
	}
	catch( const DBException& e )
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DBException", e.getSummary().c_str() );
	}
	catch(sql::SQLException &e)
	{
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DBException:MCL_ERROR", e.what() );
	}
    catch (...)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "~MysqlConnection(): Unknown exception" );
    }
}


/**
* createConnection
* 
* creates the underlying framework to support a session
*/
void MysqlConnection::createConnection()
{
	std::string charset;
	charset = TA_Base_Core::RunParams::getInstance().get(RPARAM_CHARSET);//"gbk" "utf8" ""
	if (charset.empty())
	{
		charset = "utf8";//default charset utf8
	}

	try
	{		
		if (NULL != m_pConnection)
		{		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection already closed, create or reconnect to database.");		
            delete m_pConnection;
			m_pConnection = NULL;
		}	
		if (NULL == m_pConnection)
		{
 			sql::ConnectOptionsMap connection_properties;
 			connection_properties["hostName"] = m_hostName;
 			connection_properties["userName"] = m_userName;
            connection_properties["password"] = m_password;
			connection_properties["metadataUseInfoSchema"] = true;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"MysqlConnection Charset: %s", charset.c_str());
			//set character_set_results=gbk;//output from server			
			connection_properties["characterSetResults"] = charset;
			//set character_set_client=gbk;//input to server
			connection_properties["OPT_CHARSET_NAME"] = charset;

 
			TA_ASSERT(NULL != m_pDriver, "Mysql database Driver is null");
			m_pConnection = m_pDriver->connect(connection_properties); 
			TA_ASSERT(NULL != m_pConnection, "Cannot connect to the database server.");	

			m_pConnection->setSchema(m_databaseName);
			
			// set this session mode with backslash disabled, same with oracle
			std::auto_ptr< sql::Statement > stmt(m_pConnection->createStatement());
			stmt->execute("set session sql_mode=concat(@@sql_mode,',NO_BACKSLASH_ESCAPES')");
		}				
		
	}
	catch(sql::SQLException & e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"MysqlConnection exception: %s, Error Code: %d, Reason: %s", e.getSQLStateCStr(), e.getErrorCode(), e.what());
		DBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}		
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"MysqlConnection_ERROR: unknown database exception" );
		DBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		throw db;
	}	
}

/**
* getConnectionString
*
* Returns the connection string for this MysqlConnection.
*
* @return std::string  the connection string in the form
*                      databaseName:userName:password:hostName
*/
std::string MysqlConnection::getConnectionString() const
{
	return m_connectionStr;
}

bool MysqlConnection::isOpen()
{
	FUNCTION_ENTRY("isOpen");
	ThreadGuard guard( m_lock );
	try
	{
		FUNCTION_EXIT;
		if (NULL == m_pConnection)
			return false;
		return (!m_pConnection->isClosed());
	}	
	catch(sql::SQLException & e)
	{
		DBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}		
	catch(...)
	{
		DBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		throw db;
	}	
}

void MysqlConnection::open()
{
	FUNCTION_ENTRY("open");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Fetching thread guard.");
	ThreadGuard guard( m_lock );
	
	if ( m_pConnection && !m_pConnection->isClosed())
	{
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Connection Exists.");
		m_usageCount++;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Current usage count is %d, use established connection", m_usageCount);
		FUNCTION_EXIT;
		return;
	}
	
	try  // connection closed, reconnect to the database server
	{
		//m_tran->setTimeout(m_queryTimeout);	
		//m_pConnection->setTransaction(m_tran);
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Connecting to DB.");
		createConnection();		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Current usage count is %d, use new connection", m_usageCount);
		m_usageCount = 1;
	}
	catch(const DBException& e)
	{
		throw e;
	}
	catch(sql::SQLException & e)
	{
		DBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}	
	catch(...)
	{
		DBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		throw db;
	}	
	
	FUNCTION_EXIT;
}


/**
* close
*
* Ends the session with the database if there is one running.
*
* @return true if the session was ended, false otherwise
*/
void MysqlConnection::close()
{
	FUNCTION_ENTRY("close");
	ThreadGuard guard( m_lock );
	if (NULL == m_pConnection || m_pConnection->isClosed())
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "MysqlConnection::close(): Database is already closed" );
		FUNCTION_EXIT;
		return;
	}
	
	if ( m_usageCount > 0 )
	{		
		m_usageCount--;
	}	

	if ( 0 == m_usageCount )
	{
		m_pConnection->close();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in close() function.");
	}
	
	FUNCTION_EXIT;
}


/**
* closeAll
*
* Ends all sessions with the database if there are any running.
*
* @return true if the sessions were ended, false otherwise
*/
void MysqlConnection::closeAll()
{
	FUNCTION_ENTRY("closeAll");
	{
		ThreadGuard guard( m_lock );
		// If m_usageCount is 1, close() will close everything.
		m_usageCount = 1;
	}
	close();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in closeAll() function.");
	FUNCTION_EXIT;
}


/**
* exec
*
* Executes the given statement on the database.  This method
* handles both queries (e.g. "SELECT...") and non-queries (e.g."INSERT...")
*
* @param statement    the statement to execute
* @param isQuery      a bool that is true if the statement is a query
* @param numColumns   the number of columns to retrieve
* @param bindType     the type of binding required
* @return true if the statement was executed, false otherwise
*/
//void MysqlConnection::exec( const std::string& statement, ocl::OraQuery *& query, bool isQuery /*= false*/, int prefetch /*= 10*/)
void MysqlConnection::exec( const SQLStatement& rSqlObj, IQueryAdapter*& pIQueryAdapter, bool isQuery /*= false*/, int prefetch /*= 10*/)
{
	FUNCTION_ENTRY("exec");		

	if ( NULL != pIQueryAdapter )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "pIQueryAdapter should not be other value except NULL" );
		cleanQuery( pIQueryAdapter );
	}
	if (NULL == m_pConnection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Mysql connection is null" );
		TA_THROW( DBException("MysqlConnection_ERROR", "Mysql connection is gone away", ER_MYSQL_GONE_AWAY) );
	}
	std::string strSql;
	_GetSQL(strSql, rSqlObj);

	TA_ASSERT(!strSql.empty(), "SQL statement is empty");
	if ( strSql.size() > DEFMAXLOGLEN)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL,"Begin execute the LargeSQL statement: [[");
		LOGLARGESTRING(SourceInfo, DebugUtil::DebugSQL, strSql.c_str());
	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugSQL, "Begin execute the SQL statement: [[%s]]", strSql.c_str());
	}

	sql::ResultSet* pQuery = NULL;	
	try
	{
		ThreadGuard guard( m_lock );
		// check the database connection first, if it already closed, open it
		if (m_pConnection->isClosed())
		{
			open();
		}		
		std::auto_ptr< sql::Statement > stmt(m_pConnection->createStatement());
		if (isQuery) //select *
		{
			pQuery = stmt->executeQuery(strSql);
		}
		else //like the insert command
		{
			LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "The SQL modification to execute is: %s", strSql.c_str());
			bool bRetCode = stmt->execute(strSql);
			if(bRetCode)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "the statement is not a insert like statement" );
			}
			if ( NULL != pQuery)
			{
				delete pQuery;
				pQuery = NULL;
			}			
		}
	}
	catch(const DBException& e)
	{
		throw e;
	}
	catch(sql::SQLException & e)
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec SQLID: %s, SQL: %s", rSqlObj.strSQLID.c_str(), strSql.c_str() );
		LOG2(SourceInfo, DebugUtil::DebugError, "Reason: %s, ErrorCode: %d", e.what(), e.getErrorCode());
		DBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		delete pQuery; pQuery = NULL;
		throw db;
	}	
	catch(...)
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec SQLID: %s, SQL: %s", rSqlObj.strSQLID.c_str(), strSql.c_str() );
		DBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		delete pQuery; pQuery = NULL;
		throw db;
	}	

	//just save the pointer, here
	if ( NULL != pQuery )
	{
		ThreadGuard guard( m_queryLock );
		pIQueryAdapter = new MysqlQueryAdapter( pQuery );
		m_mapActiveQuery[pIQueryAdapter] = QUERY_EXIST;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "MysqlQueryAdapter: 0x%p is created for connection: 0x%p", pIQueryAdapter, this );
	}

	FUNCTION_EXIT;
}

void MysqlConnection::fetchBLOB(const std::string &strSQL, 
	const std::string &colomnName, 
	DataClobArray& data )
{
	FUNCTION_ENTRY("fetchBLOB");
	try
	{
		if (m_pConnection->isClosed())
		{
			open();
		}
		TA_ASSERT(NULL != m_pConnection, "Connection is NULL");
		std::auto_ptr< sql::PreparedStatement >pstmt(m_pConnection->prepareStatement( strSQL ));

		size_t nIndex       = 0;
		size_t nCount       = 0;
		size_t nColumnIndex = 0;
		sql::ResultSet* pQuery   = NULL;           // this pointer should delete after use
		sql::ResultSetMetaData* pRsetData = NULL;    // this pointer using boost::scoped_ptr internal.
		const size_t nFetchRows      = 10;    // think whether it's enough?		

		pQuery       = pstmt->executeQuery();		
		nColumnIndex = pQuery->findColumn(colomnName);  // get the column index
		pRsetData    = pQuery->getMetaData();
		nCount       = pQuery->rowsCount();
		pQuery->first();  // move to the first row
		while( pQuery && pRsetData && nCount > 0 && !pQuery->isAfterLast() && nIndex < nFetchRows )
		{
			std::string  strData = "";
			switch(pRsetData->getColumnType(nColumnIndex)) 
			{
			case sql::DataType::VARBINARY:
			case sql::DataType::VARCHAR:
			case sql::DataType::LONGVARBINARY:
			case sql::DataType::LONGVARCHAR:
				{
					//fix bug: Blob Column Contain binary Data, opation "std::istream >> std::string " will not get all data.
					//use std::copy to replace opation "std::istream >> std::string "
					std::istream* ssblob = pQuery->getBlob(colomnName);
					std::streambuf* sbuf = ssblob->rdbuf();
					std::copy(std::istreambuf_iterator<char>(sbuf), std::istreambuf_iterator<char>(), back_inserter(strData));			
				}
				break;			
			default:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The Column data type is not a Blob type" );
				TA_THROW( DBException("MysqlConnection_ERROR", "The Column data type is not a Blob type", 0) );
			}

			if (strData.length() > 0)  // have data
			{
				DataClob dataclob;
				std::copy(strData.begin(), strData.end(), std::back_inserter(dataclob));
		    	data.push_back(dataclob);
			}
			pQuery->next(); nIndex++;
		}
		// we finished use it, and need to clean it.
		pQuery->close();
		if (NULL != pQuery)
		{
			delete pQuery;
			pQuery = NULL;
		}
	}
	catch( sql::SQLException & e )
	{
		LOG2(SourceInfo, DebugUtil::DebugError, "Reason: %s, ErrorCode: %d", e.what(), e.getErrorCode());
		DBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}		
	catch(...)
	{
		DBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		throw db;
	}
	
	FUNCTION_EXIT;
}


void MysqlConnection::updateBLOB(
	 const std::string &strTableName, 
	const std::string &colomnName,
	 const std::vector<unsigned char>& data, 
	 const std::string &strWhereClause
	)
{
	FUNCTION_ENTRY("updateBLOB");
	std::stringstream SqlStatement;
	try
	{
		TA_ASSERT(strTableName.length() > 0, "MysqlConnection::updateBLOB::Invalid table name.");
		TA_ASSERT(colomnName.length() > 0, "MysqlConnection::updateBLOB::Invalid ColumnLabel parameter");
		
		if (m_pConnection->isClosed())
		{
			open();
		}

		// modified: trigger MySQL binlog_format to RAW mode for replication issue
		SqlStatement << "UPDATE " << strTableName << " SET " << colomnName << "=  concat(?,ifnull(load_file(null),'')) ";	
		if ( strWhereClause.length() > 0 )
		{
			SqlStatement << " WHERE " << strWhereClause;
		}	
		
		std::auto_ptr< sql::PreparedStatement >pstmt(m_pConnection->prepareStatement( SqlStatement.str() ));
		
		if (data.size() > 0)  // have data to insert
		{
			std::string strBuf(data.begin(), data.end());
			std::istringstream ss;
			ss.str(strBuf);
			pstmt->setBlob(1, &ss);
			
			if (pstmt->execute())
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[mysql-info], Not a update like statement." );
			
	    	pstmt->clearParameters();
	}
						
	}
	catch(  sql::SQLException & e)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to exec SQL statement '%s'",
			SqlStatement.str().c_str() );
		LOG2(SourceInfo, DebugUtil::DebugError, "Reason: %s, ErrorCode: %d", e.what(), e.getErrorCode());

		DBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}		
	catch(...)
	{
		DBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		throw db;
	}	
	
	FUNCTION_EXIT;
}

/**
* incrementExecutionCount
*
* Increment the count of threads that are using this connection right now.
* Note: The count does not include threads that have this connection open, 
* but are not executing on it right now.
*/
void MysqlConnection::incrementExecutionCount()
{
	FUNCTION_ENTRY("incrementExecutionCount");
	ThreadGuard guard( m_countLock );
	m_executionCount++;
	LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "MysqlConnection::incrementExecutionCount(), Execution count: %d" ,m_executionCount );
	FUNCTION_EXIT;
}


/**
* decrementExecutionCount
*
* Decrement the count of threads that are using this connection right now.
* Note: The count does not include threads that have this connection open,
* but are not executing on it right now.
*/
void MysqlConnection::decrementExecutionCount()
{
	FUNCTION_ENTRY("decrementExecutionCount");
	ThreadGuard guard( m_countLock );
	if (m_executionCount > 0)
	{
		m_executionCount--;
	}
	LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "MysqlConnection::decrementExecutionCount(), Execution count: %d" ,m_executionCount );
	FUNCTION_EXIT;
}

bool MysqlConnection::isAnyQueryActive( unsigned long ulMaxTimeoutInSecs )
{
    FUNCTION_ENTRY("isAnyQueryActive");

    ThreadGuard guard( m_queryLock );

    for ( std::map<IQueryAdapter*, long>::iterator itLoop = m_mapActiveQuery.begin(); 
            m_mapActiveQuery.end() != itLoop; ++itLoop )
    {
        if ( NULL == itLoop->first )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of OCIQueryAdapter" );
            continue;
        }

        if ( (itLoop->first)->isActive( ulMaxTimeoutInSecs ))
        {
            FUNCTION_EXIT;
            return true;
        }
    }

    FUNCTION_EXIT;
    return false;
}

void MysqlConnection::cleanQuery( IQueryAdapter*& pQuery )
{
    FUNCTION_ENTRY("cleanQuery");

    if ( NULL == pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid input parameter: NULL pointer of MysqlConnection" );
        return;
    }

    ThreadGuard guard( m_queryLock );

    std::map<IQueryAdapter*, long>::iterator itExistQuery = m_mapActiveQuery.find( pQuery );
    if ( m_mapActiveQuery.end() != itExistQuery )
    {
        if ( NULL == itExistQuery->first )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of MysqlConnection" );
            return;
        }

        // Destructor will clean query
        delete (itExistQuery->first);
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "MysqlConnection: 0x%p is deleted by SimpleDb", pQuery );

        m_mapActiveQuery.erase( itExistQuery );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Specified query is not found in this connection, the old connection has been reaped" );
        if ( !pQuery->cleanQuery() )
        {
            delete pQuery;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "MysqlConnection: 0x%p is deleted by SimpleDb through it has been reaped", pQuery );
        }
        // else leave it to ConnectionReaper for deleting
    }

    // Set pointer to NULL
    pQuery = NULL;

    FUNCTION_EXIT;
}

void  MysqlConnection::_GetSQL(std::string& strSql, const SQLStatement& rSqlObj)
{
	if (!rSqlObj.strCommon.empty())
	{
		strSql = rSqlObj.strCommon;
	}
	else if (!rSqlObj.strMySQL.empty())
	{
		strSql = rSqlObj.strMySQL;
	}
}

NS_END(TA_Base_Core)
