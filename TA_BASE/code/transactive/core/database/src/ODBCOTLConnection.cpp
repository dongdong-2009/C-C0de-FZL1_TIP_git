/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/database/src/ODBCOTLConnection.cpp $
* @author:  Ripple
* @version: $Revision: #8 $
*
* Last modification: $DateTime: 2014/06/26 14:18:31 $
* Last modified by:  $Author: weikun.lin $
*
* This class is used by OCIAccess to store database connection 
* information and maintain the associated connection handles.  
* If the database automatically reconnects to a database with the same name, username and 
* password, this class will connect to the new database on the first exec
* failure.
* 
*/

#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#include <algorithm>

#include "core/otl/src/otlv4.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/ODBCOTLQueryAdapter.h"
#include "core/database/src/ODBCOTLConnection.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
//#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::Thread;
//TD18251, jianghp, to replace the OCI with OCL


namespace TA_Base_Core
{
	const long QUERY_EXIST = 1;
	////////////////////////////////////////////////////////////////////////////////
	//                                                                            //
	//                         Construction/Destruction                           //
	//                                                                            //
	////////////////////////////////////////////////////////////////////////////////

	/**
	* OCIConnection
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
	ODBCOTLConnection::ODBCOTLConnection( const std::string& connectionStr, 
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
		,m_connection(NULL)	
	{
		//[User Name]/[Password]@[TNS Alias]
		//"IT271350_5/IT271350_5@TRANSACT"
		m_strOTLConnection = m_userName + "/" + m_password + "@" + m_databaseName;
		m_nAutoCommit = 0;
		createConnection();
	}


	/**
	* ~OCIConnection
	* 
	* Sole standard destructor.  Frees the handles allocated in the constructor.
	*/
	ODBCOTLConnection::~ODBCOTLConnection()
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
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of OCIQueryAdapter" );
						continue;
					}

					if ( !itLoop->first->cleanQuery() )
					{
						delete (itLoop->first);
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "OCIQueryAdapter: 0x%p is deleted by ConnectionReaper in destructor of OCIConnection", itLoop->first );
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
			// prevent null pointer cause crash
			if (NULL != m_connection)
			{
				m_connection->logoff();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in destruction function.");
				delete m_connection;
				m_connection = NULL;
			}
		}
		catch(otl_exception& e)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, 
				"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s", 
				e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);
		}
		catch (...)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "~OCIConnection(): Unknown exception" );
		}
	}


	/**
	* createConnection
	* 
	* creates the underlying framework to support a session
	*/
	void ODBCOTLConnection::createConnection(void)
	{
		int nOTLRes = 0;

		try
		{
			if (m_connection != NULL)
			{
				m_connection->logoff();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in createConnection() function.");
				delete m_connection;
				m_connection = NULL;
			}	
			if (m_connection == NULL)
			{
				int nThreadedMode = 1;
				nOTLRes = otl_connect::otl_initialize(nThreadedMode); // initialize OCI environment
				if (1 != nOTLRes)
				{
					//error
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "otl_connect::otl_initialize error!");
				}
				m_connection = new otl_connect();
			}
			// connect to Oracle and set autocommit value
			m_connection->rlogon(m_strOTLConnection.c_str(), m_nAutoCommit); 
		}
		catch(otl_exception& e)
		{
			//error
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s, strCon=%s",
				e.msg, e.code, e.sqlstate, e.stm_text, e.var_info, m_strOTLConnection.c_str());

			DBException db("OTL_ERROR", (char*)(e.msg), e.code);
			throw db;
		}		
		catch(...)
		{
			DBException db("OCIConnection_ERROR", "unknown database exception", 0);
			throw db;
		}	
	}

	/**
	* getConnectionString
	*
	* Returns the connection string for this OCIConnection.
	*
	* @return std::string  the connection string in the form
	*                      databaseName:userName:password:hostName
	*/
	std::string ODBCOTLConnection::getConnectionString() const
	{
		return m_connectionStr;
	}

	bool ODBCOTLConnection::isOpen()
	{
		FUNCTION_ENTRY("isOpen");
		ThreadGuard guard( m_lock );
		FUNCTION_EXIT;

		bool bConnected = false;

		if (m_connection->connected)
		{
			bConnected = true;
		}
		else
		{
			bConnected = false;
		}

		return bConnected;
	}

	void ODBCOTLConnection::open()
	{
		FUNCTION_ENTRY("open");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Fetching thread guard.");
		ThreadGuard guard( m_lock );

		if (m_connection->connected)
		{
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Connection Exists.");
			m_usageCount++;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Current usage count is %d, use established connection", m_usageCount);
			FUNCTION_EXIT;
			return;
		}

		try
		{
			// connect to Oracle and set autocommit value
			m_connection->rlogon(m_strOTLConnection.c_str(), m_nAutoCommit);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Current usage count is %d, use new connection", m_usageCount);
			m_usageCount = 1;
		}
		catch(otl_exception& e)
		{
			//error
  			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s, strCon=%s",
				e.msg, e.code, e.sqlstate, e.stm_text, e.var_info, m_strOTLConnection.c_str());		
				
			//throw DBException
			DBException db("OTL_ERROR", (char*)(e.msg), e.code);
			throw db;
		}	
		catch(...)
		{
			DBException db("OCIConnection_ERROR", "unknown database exception", 0);
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
	void ODBCOTLConnection::close()
	{
		FUNCTION_ENTRY("close");
		ThreadGuard guard( m_lock );
		TA_ASSERT(m_connection != NULL, "connection is NULL!");

		if (!m_connection->connected)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "OCIConnection::close(): Database is already closed" );
			FUNCTION_EXIT;
			return;
		}

		if ( m_usageCount > 0 )
		{
			m_usageCount--;
		}

		if ( 0 == m_usageCount )
		{
			m_connection->logoff();
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
	void ODBCOTLConnection::closeAll()
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
	//void OCIConnection::exec( const std::string& statement, ocl::OraQuery *& query, bool isQuery /*= false*/, int prefetch /*= 10*/)
	void ODBCOTLConnection::exec( const SQLStatement& rSqlObj, IQueryAdapter*& pIQueryAdapter, bool isQuery /*= false*/, int prefetch /*= 10*/)
	{
		FUNCTION_ENTRY("exec");
		//TA_ASSERT(query == NULL, "query should be NULL!");

		if ( NULL != pIQueryAdapter )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "pOCIQueryAdapter should not be other value except NULL" );
			cleanQuery( pIQueryAdapter );
		}

		std::string strSql;
		_GetSQL(strSql, rSqlObj);

		TA_ASSERT(!strSql.empty(), "SQL statement is empty");
		_LogSQLInfo(SourceInfo, DebugUtil::DebugSQL, "Begin execute the SQL statement: ", strSql);

		// If the database is not already open, open it.
		if (!m_connection->connected)
		{
			open();
		}

		otl_stream* query = NULL;
		long nRowsAffected = 0;
		try
		{
			if (isQuery) //select *
			{
				query = new otl_stream();
				query->open(prefetch, strSql.c_str(), *m_connection);
			}
			else //like the insert command
			{
				m_connection->auto_commit_off();
				nRowsAffected = m_connection->direct_exec(strSql.c_str(), otl_exception::enabled);
				m_connection->commit();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Affected Rows=%d", nRowsAffected);
				query = NULL;
			}
		}
		catch(otl_exception& e)
		{
			// Handle lost Connection and rollback transaction
			_CheckQueryType(isQuery, e);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Failed to exec SQLID: %s, SQL: %s", rSqlObj.strSQLID.c_str(), strSql.c_str());
			//error
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s",
				e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);

			delete query;
			query = NULL;

			//throw DBException
			DBException db("OTL_ERROR", (char*)(e.msg), e.code);
			throw db;;
		}	
	

		//just save the pointer, here
		if ( NULL != query )
		{
			ThreadGuard guard( m_queryLock );
			pIQueryAdapter = new ODBCOTLQueryAdapter( query );
			m_mapActiveQuery[pIQueryAdapter] = QUERY_EXIST;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "OCIQueryAdapter: 0x%p is created for connection: 0x%p", pIQueryAdapter, this );
		}

		FUNCTION_EXIT;
	}

	void ODBCOTLConnection::fetchBLOB(const std::string &strSQL, 
		const std::string &colomnName, 
		DataClobArray& data )
	{
		FUNCTION_ENTRY("fetchBLOB");

		otl_stream query;
		int prefetch = 1;
		int nColumnCount = 0;;
		int nRowIndex = 0;
		int nColumnIndex = 0; 
		std::string strColumnName;
		std::string strColumnValue;
		otl_column_desc* pOTLColumnDescRef = NULL;

		if (!m_connection->connected)
		{
			open();
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "fetchBLOB exec SQL:%s", strSQL.c_str());
		try
		{
			//query->set_lob_stream_mode(true); //no need set the "lob stream mode" flag
			//prefetch (buffer size) needs to be set to 1 in case of BLOB columns
			prefetch = 1;
			query.open(prefetch, strSQL.c_str(), *m_connection);

			pOTLColumnDescRef = query.describe_select(nColumnCount);
			if(!pOTLColumnDescRef)
			{
				throw otl_exception(otl_error_msg_21,otl_error_code_21);
			}

			nRowIndex = 1;
			nColumnIndex = 0;
			const otl_column_desc& curr_var = pOTLColumnDescRef[nColumnIndex];

			//oracle.otl_stream_read_iterator.get 
			while(!query.eof())
			{
				strColumnName.clear();
				strColumnValue.clear();
				strColumnName = curr_var.name;

				otl_long_string otlLongString(query.get_adb_max_long_size());

				switch(curr_var.otl_var_dbtype)
				{
				case otl_var_varchar_long:
				case otl_var_raw_long:
				case otl_var_clob:
				case otl_var_blob:
					query>>otlLongString;

					strColumnValue.clear();
					strColumnValue.append((char*)(otlLongString.v), otlLongString.len());

					break;
				default:
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "fetchBLOB Unsupported data type:%d", curr_var.otl_var_dbtype);
					throw otl_exception(otl_error_msg_14,otl_error_code_14);
				}//switch

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"[nRowIndex=%d][nColumnIndex=%d][strColumnName=%s][strColumnValue=%s]",
					nRowIndex, nColumnIndex, strColumnName.c_str(),strColumnValue.c_str());

				DataClob dataclob;
				std::copy(strColumnValue.begin(), strColumnValue.end(), std::back_inserter(dataclob));
				data.push_back(dataclob);
			}//while

			pOTLColumnDescRef = NULL;
		}
		catch(otl_exception& e)
		{
			// Handle lost Connection
			if (_CheckLostConnection(e.code))
			{
				m_connection->logoff();
			}


			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to exec SQL:%s", strSQL.c_str());

			//error
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s",
				e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);

			//throw DBException
			DBException db("OTL_ERROR", (char*)(e.msg), e.code);
			throw db;
		}


		FUNCTION_EXIT;
	}


	void ODBCOTLConnection::updateBLOB(
		const std::string &strTableName, 
		const std::string &colomnName,
		const std::vector<unsigned char>& data, 
		const std::string &strWhereClause
		)
	{
		FUNCTION_ENTRY("updateBLOB");
		std::stringstream selectStatement;
		std::stringstream updateStatement;
		otl_stream strSelectForUpdate;
		otl_stream strUpdate;
		int nRowsAffected = 0;
		int buffer_size = 0;
		otl_lob_stream lob_stream_tmp;/// LOB stream for reading/writing unlimited number of bytes regardless of the buffer size.
		otl_column_desc* pOTLColumnDescRef = NULL;
		int nColumnCount = 0;
		std::ostringstream ostrTmp;
		otl_long_string otlLongStringData(data.size()+1);// define long string variable

		if (!m_connection->connected)
		{
			open();
		}



		//init data
		//std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(ostrTmp));
		//memcpy(otlLongStringData.v, ostrTmp.str().data(), data.size());
		for (int nIndex = 0; nIndex < data.size(); nIndex++)
		{	
			otlLongStringData[nIndex] = data[nIndex];
		}
		otlLongStringData.set_len(data.size());

		try
		{
			//init sql
			// Format a SELECT statement
			selectStatement<<"SELECT"<<" "<<colomnName<<" "<<"FROM"<<" "<<strTableName<<" ";
			if ( strWhereClause.length() > 0 )
			{
				selectStatement<<" "<<"WHERE"<<" "<<strWhereClause<<" ";
			}
			selectStatement<<" "<<"FOR UPDATE";

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "updateBLOB exec SQL:%s", selectStatement.str().c_str());

			m_connection->auto_commit_off();//start transaction
			strSelectForUpdate.set_lob_stream_mode(true); // set the "lob stream mode" flag
			strSelectForUpdate.open(1, selectStatement.str().c_str(), *m_connection);
			strSelectForUpdate.set_commit(0);
			pOTLColumnDescRef = strSelectForUpdate.describe_select(nColumnCount);
			if(NULL == pOTLColumnDescRef || 1 != nColumnCount)
			{
				pOTLColumnDescRef = NULL;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to exec SQL:%s, ColumnCount:%d, errormsg:%s", 
					selectStatement.str().c_str(), nColumnCount, otl_error_msg_21);
				throw otl_exception(otl_error_msg_21,otl_error_code_21);
			}
			

			//
			const otl_column_desc& curr_var = pOTLColumnDescRef[0];//nColumnIndex=0
			switch(curr_var.otl_var_dbtype)
			{
			case otl_var_varchar_long:			
			case otl_var_raw_long:
			case otl_var_clob:
			case otl_var_blob:
				updateStatement.clear();
				updateStatement<<"UPDATE"<<" "<<strTableName<<" "<<"SET"<<" "<<colomnName<<"=:"<<colomnName<<"<raw_long>"<<" ";
				if ( strWhereClause.length() > 0 )
				{
					updateStatement<<" "<<"WHERE"<<" "<<strWhereClause<<" ";
				}				
				break;
			default:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "fetchBLOB Unsupported data type:%d", curr_var.otl_var_dbtype);
				throw otl_exception(otl_error_msg_14,otl_error_code_14);
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "updateBLOB exec SQL:%s", updateStatement.str().c_str());
			strUpdate.set_lob_stream_mode(true); // set the "lob stream mode" flag
			strUpdate.open(1, updateStatement.str().c_str(), *m_connection);
			strUpdate.set_commit(0);
			strUpdate<<lob_stream_tmp; // Initialize otl_lob_stream by writing it

			lob_stream_tmp.set_len(otlLongStringData.len());//// setting the total size of of the BLOB to be written
			lob_stream_tmp<<otlLongStringData;

			lob_stream_tmp.close(); // closing the otl_lob_stream

			m_connection->commit(); // committing transaction

		}
		catch(otl_exception& e)
		{
			// Handle lost Connection
			if (_CheckLostConnection(e.code))
			{
				m_connection->logoff();
			}
			else
			{
				m_connection->rollback();
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to exec SQL:%s", selectStatement.str().c_str());

			//error
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s",
				e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);

			//throw DBException
			DBException db("OTL_ERROR", (char*)(e.msg), e.code);
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
	void ODBCOTLConnection::incrementExecutionCount()
	{
		FUNCTION_ENTRY("incrementExecutionCount");
		ThreadGuard guard( m_countLock );
		m_executionCount++;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "OCIConnection::incrementExecutionCount(), Execution count: %d" ,m_executionCount );
		FUNCTION_EXIT;
	}


	/**
	* decrementExecutionCount
	*
	* Decrement the count of threads that are using this connection right now.
	* Note: The count does not include threads that have this connection open,
	* but are not executing on it right now.
	*/
	void ODBCOTLConnection::decrementExecutionCount()
	{
		FUNCTION_ENTRY("decrementExecutionCount");
		ThreadGuard guard( m_countLock );
		if (m_executionCount > 0)
		{
			m_executionCount--;
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "OCIConnection::decrementExecutionCount(), Execution count: %d" ,m_executionCount );
		FUNCTION_EXIT;
	}

	bool ODBCOTLConnection::isAnyQueryActive( unsigned long ulMaxTimeoutInSecs )
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

	void ODBCOTLConnection::cleanQuery( IQueryAdapter*& pQuery )
	{
		FUNCTION_ENTRY("cleanQuery");

		if ( NULL == pQuery )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid input parameter: NULL pointer of OCIQueryAdapter" );
			return;
		}

		ThreadGuard guard( m_queryLock );

		std::map<IQueryAdapter*, long>::iterator itExistQuery = m_mapActiveQuery.find( pQuery );
		if ( m_mapActiveQuery.end() != itExistQuery )
		{
			if ( NULL == itExistQuery->first )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of OCIQueryAdapter" );
				return;
			}

			// Destructor will clean query
			delete (itExistQuery->first);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "OCIQueryAdapter: 0x%p is deleted by SimpleDb", pQuery );

			m_mapActiveQuery.erase( itExistQuery );
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Specified query is not found in this connection, the old connection has been reaped" );
			if ( !pQuery->cleanQuery() )
			{
				delete pQuery;
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "OCIQueryAdapter: 0x%p is deleted by SimpleDb through it has been reaped", pQuery );
			}
			// else leave it to ConnectionReaper for deleting
		}

		// Set pointer to NULL
		pQuery = NULL;

		FUNCTION_EXIT;
	}

	void ODBCOTLConnection::_GetSQL(std::string& strSql, const SQLStatement& rSqlObj)
	{
		if (!rSqlObj.strCommon.empty())
		{
			strSql = rSqlObj.strCommon;
		}
		else if (!rSqlObj.strOracle.empty())
		{
			strSql = rSqlObj.strOracle;
		}
	}
	void ODBCOTLConnection::_LogSQLInfo(const char* pszFileName, int nLineNumber, TA_Base_Core::DebugUtil::EDebugLevel nLogLevel,
		const std::string& strLogInfo, const std::string& strSQL)
	{
		std::string strLogStream;
		strLogStream = strLogInfo + strSQL;
		if ( strLogStream.size() > DEFMAXLOGLEN)
		{
			LOGLARGESTRING(pszFileName, nLineNumber, nLogLevel, strLogStream.c_str());
		}
		else
		{
			LOG_GENERIC(pszFileName, nLineNumber, nLogLevel, "%s", strLogStream.c_str());
		}
	}

	bool ODBCOTLConnection::_CheckLostConnection(int nOracleErrorCode)
	{
		bool bLostConnection = false;
		// Handle lost Connection
		if (ER_ORA_NOT_CONNECTED == nOracleErrorCode ||
			ER_ORA_OCI_NOTLOGGEDON == nOracleErrorCode ||
			ER_ORA_OCI_SESSION_KILLED == nOracleErrorCode ||
			ER_ORA_OCI_NO_INTERFACE == nOracleErrorCode ||
			ER_ORA_TNS_PACKET_WRITER_FAILURE == nOracleErrorCode) 
		{
			bLostConnection = true;
		}

		return bLostConnection;
	}

	void ODBCOTLConnection::_CheckQueryType(bool isQuery, otl_exception& e)
	{
		try
		{
			// Handle lost Connection
			if (_CheckLostConnection(e.code))
			{
				m_connection->logoff();
			}
			else
			{
				//in fun exec(), query not start transaction don't need to rollback 
				if (!isQuery)
				{
					//Attempting to roll back a global transaction that is not currently active causes an error
					m_connection->rollback();
				}
			}
		}
		catch (otl_exception& e)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"logoff or rollback OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s",
				e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);

		}
	}


}//namespace TA_Base_Core