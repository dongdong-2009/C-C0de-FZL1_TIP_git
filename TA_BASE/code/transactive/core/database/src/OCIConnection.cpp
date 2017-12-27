/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/OCIConnection.cpp $
* @author:  Ripple
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2015/01/22 12:58:28 $
* Last modified by:  $Author: Ouyang $
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

#include "core/ocl/src/ocl.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/OCIQueryAdapter.h"
#include "core/database/src/OCIConnection.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
//#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::Thread;
//TD18251, jianghp, to replace the OCI with OCL
using namespace ocl;

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
	OCIConnection::OCIConnection( const std::string& connectionStr, 
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
		createConnection();
	}
	
	
	/**
	* ~OCIConnection
	* 
	* Sole standard destructor.  Frees the handles allocated in the constructor.
	*/
	OCIConnection::~OCIConnection()
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
				m_connection->close();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in destruction function.");
				delete m_connection;
				m_connection = NULL;
			}
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DBException", e.getSummary().c_str() );
		}
		catch(ocl::CRException &e)
		{
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DBException:OCL_ERROR", e.message().c_str() );
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
	void OCIConnection::createConnection(void)
	{
		try
		{
			if (m_connection != NULL)
			{
				m_connection->close();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in createConnection() function.");
                delete m_connection;
				m_connection = NULL;
			}	
			if (m_connection == NULL)
			{
				OraEnvironment::setInitMode(OraEnvironment::imObject);
				OraEnvironment::setInitMode(OraEnvironment::imThreaded);
				OraEnvironment::setThreaded(true);
				m_connection = new ocl::OraConnection();
                /*has duplicated transaction error;
				std::string strUUID = TA_Uuid::generateUUID();
				char uuid[128];
				int nLength = 0;
				for (int i = 0; i < strUUID.length(); i++)
				{
					if (strUUID[i] != '-')
					{
						uuid[i] = strUUID[i];
						nLength++;
					}
				}
				m_tran = new ocl::OraTransaction(uuid, nLength);*/

			}
			m_connection->setConnectMode(cmNormal);
			m_connection->setUsername(m_userName); 
			m_connection->setPassword(m_password); 
			m_connection->setServer(m_databaseName); 
		}
		catch(ocl::OraException & e)
		{
			DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
			throw db;
		}
		catch(ocl::CRException &e)
		{
			DBException db("OCL_ERROR", e.message().c_str(),  0);
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
	std::string OCIConnection::getConnectionString() const
	{
		return m_connectionStr;
	}
	
	bool OCIConnection::isOpen()
	{
		FUNCTION_ENTRY("isOpen");
		ThreadGuard guard( m_lock );
		FUNCTION_EXIT;

		return m_connection->isConnected();
	}

	void OCIConnection::open()
	{
		FUNCTION_ENTRY("open");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Fetching thread guard.");
		ThreadGuard guard( m_lock );
		
		if ( m_connection->isConnected())
		{
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Connection Exists.");
			m_usageCount++;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Current usage count is %d, use established connection", m_usageCount);
			FUNCTION_EXIT;
			return;
		}
		
		try
		{
			//m_tran->setTimeout(m_queryTimeout);	
			//m_connection->setTransaction(m_tran);
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Connecting to DB.");
			m_connection->open();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Current usage count is %d, use new connection", m_usageCount);
			m_usageCount = 1;
		}
		catch(ocl::OraException & e)
		{
			DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
			throw db;
		}
		catch(ocl::CRException &e)
		{
			DBException db("OCL_ERROR", e.message().c_str(), 0);
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
	void OCIConnection::close()
	{
		FUNCTION_ENTRY("close");
		ThreadGuard guard( m_lock );
		TA_ASSERT(m_connection != NULL, "connection is NULL!");

		if (! m_connection->isConnected())
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "OCIConnection::close(): Database is already closed" );
			FUNCTION_EXIT;
			return;
		}
		
		if ( m_usageCount > 0 )
		{
			//TD18251, jianghp, to replace the OCI with OCL
			//deleteStatement( false );
			m_usageCount--;
		}
		
		//m_isOpen = false;
		if ( 0 == m_usageCount )
		{
			m_connection->close();
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
	void OCIConnection::closeAll()
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
	void OCIConnection::exec( const SQLStatement& rSqlObj, IQueryAdapter*& pIQueryAdapter, bool isQuery /*= false*/, int prefetch /*= 10*/)
	{
		FUNCTION_ENTRY("exec");
		//TA_ASSERT(query == NULL, "query should be NULL!");

		//delete first, and create another new.
		//if (query != NULL)
		//{
		//    if (query->isActive())
		//    {
		//        query->close();
		//        query->unprepare();
		//    }
		//    delete query;
		//    query = NULL;
		//}

		if ( NULL != pIQueryAdapter )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "pOCIQueryAdapter should not be other value except NULL" );
			cleanQuery( pIQueryAdapter );
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

		// If the database is not already open, open it.
		if (!m_connection->isConnected())
		{
			open();
		}

		OraQuery* query = NULL;
		try
		{
			query = new OraQuery(); // Wrap it to OCIQueryAdapter
			query->setConnection(*m_connection);
			if (isQuery) //select *
			{
				query->setCached(false);  // noncached mode
				query->setFetchRows(prefetch);
				query->setSQL(strSql);
				//				if (! m_connection->inTransaction()) // returns false 
				//				{
				//					m_connection->startTransaction();    // begins new transaction  
				//				}

				query->prepare(); 
				query->open();   // query result rows
			}
			else //like the insert command
			{
				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "The SQL modification to execute is: %s", strSql.c_str());
				query->setCommandType(ocl::ctSQL);
				query->setCommandText(strSql);
				m_connection->startTransaction();  //  start the transaction must after seting the command text.
				query->execute(); 
				query->close();
				if (m_connection->inTransaction()) // returns true 
				{
					m_connection->commit();          // ends current transaction 
				}
				else
				{
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "[ocl-error], transaction is broken" );
				}

				delete query;
				query = NULL;
			}
		}
		catch(ocl::OraException & e)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec SQLID: %s, SQL: %s", rSqlObj.strSQLID.c_str(), strSql.c_str() );
			DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
			delete query; query = NULL;  //it will cause cursor leak if don't delete it.
			throw db;
		}
		catch(ocl::CRException &e)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec SQLID: %s, SQL: %s", rSqlObj.strSQLID.c_str(), strSql.c_str() );
			DBException db("OCL_ERROR", e.message().c_str(), 0);
			delete query; query = NULL;
			throw db;
		}
		catch(...)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to exec SQLID: %s, SQL: %s", rSqlObj.strSQLID.c_str(), strSql.c_str() );
			DBException db("OCIConnection_ERROR", "unknown database exception", 0);
			delete query; query = NULL;
			throw db;
		}	

		//just save the pointer, here
		if ( NULL != query )
		{
			ThreadGuard guard( m_queryLock );
			pIQueryAdapter = new OCIQueryAdapter( query );
			m_mapActiveQuery[pIQueryAdapter] = QUERY_EXIST;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "OCIQueryAdapter: 0x%p is created for connection: 0x%p", pIQueryAdapter, this );
			//m_currentQuery = query;
		}

		FUNCTION_EXIT;
	}
	
	void OCIConnection::fetchBLOB(const std::string &strSQL, 
		const std::string &colomnName, 
		DataClobArray& data )
	{
		FUNCTION_ENTRY("fetchBLOB");
		try
		{
			if (!m_connection->isConnected())
			{
				open();
			}
			
			ocl::OraQuery query(*m_connection);
			query.setCached(false);  // noncached mode
			query.setFetchRows(10);  // think whether it's enough?
			query.setSQL(strSQL);
			query.prepare(); 
			query.open();   // query result rows
			query.moveTo(1); //one-based recNo.
			while(!query.isEOF())
			{
				CRString  strData = "";
				switch(query.field(colomnName).dataType()) 
				{
				case ocl::dtBlob:
					{
						ocl::OraLob *blob = query.field(colomnName).getBlob();
						strData = blob->getString();
					}
					break;
				case ocl::dtClob:
					{
						ocl::OraLob *clob = query.field(colomnName).getClob();
						strData = clob->getString();
					}
					break;
				default:
					TA_ASSERT(false, "unknow data type");
				}
				DataClob dataclob;
                std::copy(strData.begin(), strData.end(), std::back_inserter(dataclob));

				data.push_back(dataclob);
				query.next();
			}
			query.close();
			query.unprepare(); 
		}
		catch( ocl::OraException & e )
		{
			DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
			throw db;
		}
		catch(ocl::CRException &e)
		{
			DBException db("OCL_ERROR", e.message().c_str(), 0);
			throw db;
		}	
		catch(...)
		{
			DBException db("OCIConnection_ERROR", "unknown database exception", 0);
			throw db;
		}	
		
		FUNCTION_EXIT;
	}
	
	
	void OCIConnection::updateBLOB(
		const std::string &strTableName, 
		const std::string &colomnName,
		const std::vector<unsigned char>& data, 
		const std::string &strWhereClause
		)
	{
		FUNCTION_ENTRY("updateBLOB");
		std::stringstream selectStatement;
		try
		{
			
			if (!m_connection->isConnected())
			{
				open();
			}
			
			// Format a SELECT statement
			selectStatement << "SELECT " << colomnName << " FROM " << strTableName;
			if ( strWhereClause.length() > 0 )
			{
				selectStatement << " WHERE " << strWhereClause;
			}
			selectStatement << " FOR UPDATE";
			
			ocl::OraQuery query(*m_connection);
			//query.setCached(true);  // cached mode
			query.setCommandType(ocl::ctSQL);
			query.setCommandText(selectStatement.str());
			m_connection->startTransaction();
			query.execute();
			switch(query.field(colomnName).dataType()) //
			{
			case ocl::dtBlob:
				{
					ocl::OraLob *blob = query.field(colomnName).getBlob();
                    std::ostringstream output;
                    std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(output));
                    blob->setChars(output.str().data(), data.size());
					blob->writeLob();
				}
				break;
			case ocl::dtClob:
				{
					ocl::OraLob *clob = query.field(colomnName).getClob();
                    std::ostringstream output;
                    std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(output));
                    clob->setChars(output.str().data(), data.size());
					clob->writeLob();
				}
				break;
			default:
				TA_ASSERT(false, "unknow lob data type");
			}
			query.close();
			if (m_connection->inTransaction()) // returns true 
			{
				m_connection->commit();          // ends current transaction 
			}else{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "[ocl-error], transaction is broken" );
			}			
		}
		catch(  ocl::OraException & e)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to exec SQL statement '%s'",
				selectStatement.str().c_str() );

			DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
			throw db;
		}
		catch(ocl::CRException &e)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to exec SQL statement '%s'",
				selectStatement.str().c_str() );

			DBException db("OCL_ERROR", e.message().c_str(), 0);
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
	* incrementExecutionCount
	*
	* Increment the count of threads that are using this connection right now.
	* Note: The count does not include threads that have this connection open, 
	* but are not executing on it right now.
	*/
	void OCIConnection::incrementExecutionCount()
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
	void OCIConnection::decrementExecutionCount()
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
	
// 	bool OCIConnection::hasMore() const
// 	{
	//	if (m_currentQuery == NULL)
	//	{
	//		return false;
	//	}
// 		return (!m_currentQuery->isEOF());
// 	}
// 
// 	bool OCIConnection::isActive() const
// 	{
	//	if (m_currentQuery == NULL)
	//	{
	//		return false;
	//	}
	//	return m_currentQuery->isActive();
// 	}
// 
// 	void OCIConnection::cleanUpQuery(ocl::OraQuery*& query)
// 	{
	//	if (query!=NULL)
	//	{
	//		query->close();
	//		query->unprepare();
	//		delete query;
	//		query = NULL;
	//	}
// 	}

    bool OCIConnection::isAnyQueryActive( unsigned long ulMaxTimeoutInSecs )
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

    void OCIConnection::cleanQuery( IQueryAdapter*& pQuery )
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

	void OCIConnection::_GetSQL(std::string& strSql, const SQLStatement& rSqlObj)
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
}
