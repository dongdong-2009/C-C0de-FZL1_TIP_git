//////////////////////////////////////////////////////////////////////
///		@file		MysqlConnection.cpp
///		@author		zhilin,ouyang
///		@date		2010-11-25 15:44:41
///
///		@brief	    Implementation of the interface IDbConnection. 
///					use mysql database server to implement the   
///                 database connection operations.
//////////////////////////////////////////////////////////////////////
#include "MysqlConnection.h"
#include "AQCommonData.h"
#include "DBException.h"
#include "MysqlQueryAdapter.h"
#include "Message.h"
#include "MAQCmd.h"
#include "CMysqlEnv.h"

#if defined(_WIN32)
	#include <Windows.h>
	//dwMilliseconds
	#define aq_sleep     Sleep	
#else
	#include <unistd.h>
	//seconds
	#define aq_sleep     sleep		
#endif

#include "driver/mysql_public_iface.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"


NS_BEGIN(TA_AQ_Lib)

using TA_Base_Core::DebugUtil;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                         Construction/Destruction                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/**
* CMysqlConnection
* 
* Sole constructor that stores the database connection details and 
* allocates handles to perform transactions with the database.
* Note: this method can be unsuccessful, so checkIsValid() should be 
* called afterwards to check that construction was successful.
*/
CMysqlConnection::CMysqlConnection(const std::string& strDbName, const std::string& strUserName,
								   const std::string& strPwd, const std::string& strHost)
: IDbConnection(strDbName, strUserName, strPwd, strHost)
, m_pConnection(NULL)
, m_pMAQCmd(NULL)
{
	_ParseDbHostName(strHost);
	_CreateConnection(m_strSchema, m_strUserName, m_strPwd);
	// mysql_init already called in mcl library
	/*if (g_p_db_driver)
	{
		g_p_db_driver->threadInit();
	}*/
	m_pMAQCmd = new CMAQCmd(m_pConnection);
}


/**
* ~MysqlConnection
* 
* Sole standard destructor.  Frees the handles allocated in the constructor.
*/
CMysqlConnection::~CMysqlConnection()
{
    {

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
		m_vecSrvIps.clear();
    }

    // The connection is closed when the server is detached.
	try
	{
		if (m_pConnection && !m_pConnection->isClosed())
		{
			m_pConnection->close();
		}		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in destruction function.");	
		DEF_DELETE(m_pMAQCmd);
        DEF_DELETE(m_pConnection);

		// need put this in each thread when the before terminating
		/*if (g_p_db_driver)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "invoke g_p_db_driver->threadEnd() to close connection thread.");
			g_p_db_driver->threadEnd();
		}*/
	}
	catch( const CDBException& e )
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );
	}
	catch(sql::SQLException&e)
	{
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException:MCL_ERROR", e.what() );
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
void CMysqlConnection::_CreateConnection(const std::string& strDbName, const std::string& strUserName, const std::string& strPwd)
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
		connection_properties["userName"] = strUserName; 
        connection_properties["password"] = strPwd;
		connection_properties["metadataUseInfoSchema"] = true;

		// check the char set run parameter
		if (!CMysqlEnv::getInstance().getConOpt()->m_strCharSet.empty())
		{
			std::string strCharSet = CMysqlEnv::getInstance().getConOpt()->m_strCharSet;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MysqlConnection Charset: %s", strCharSet.c_str());
			//set character_set_results=gbk;//output from server			
			connection_properties["characterSetResults"] = strCharSet;
			//set character_set_client=gbk;//input to server
			connection_properties["OPT_CHARSET_NAME"] = strCharSet;
		}
		
		u_int  nTimes = 0, i = 0;
		bool    bIsOk = false;
		size_t  uSrvIpSize = m_vecSrvIps.size();
		TA_ASSERT(uSrvIpSize, defErrMsg(Err_HostName_notFound));

		while (!bIsOk /*&& i < uSrvIpSize*/)
		{
			try
			{
				nTimes++;
				connection_properties["hostName"] = m_vecSrvIps[i];
				m_pConnection = CMysqlEnv::getInstance().getDriver()->connect(connection_properties); 
				m_pConnection->setSchema(strDbName);

				// set this session mode with backslash options based on the configuration
				if ( CON_BACKSLASH_OFF == CMysqlEnv::getInstance().getConOpt()->m_nConSlashSwitch)
				{
					std::auto_ptr< sql::Statement > stmt(m_pConnection->createStatement());
					stmt->execute("set session sql_mode=concat(@@sql_mode,',NO_BACKSLASH_ESCAPES')");
				}				

				bIsOk = true;
			}	
			catch(sql::SQLException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"MysqlConnection exception: %s, Error Code: %d, Reason: %s", e.getSQLStateCStr(), e.getErrorCode(), e.what());
				if ( nTimes >= MAX_RETRY_TIMES )
				{
					nTimes = 0; 
					if (++i == uSrvIpSize)
					{
						CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
						throw db;
					}						
					//i = ((i < uSrvIpSize - 1) ? ++i : 0);

					aq_sleep(5);
				}
				aq_sleep(1);
				continue;
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"MysqlConnection_ERROR: unknown database exception" );
				if ( nTimes >= MAX_RETRY_TIMES )
				{
					nTimes = 0; 
					if (++i == uSrvIpSize)
					{
						CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
						throw db;
					}
					//i = ((i < uSrvIpSize - 1) ? ++i : 0);

					aq_sleep(5);
				}
				aq_sleep(1);
				continue;
			}
		}	 // end of while		
	}  // end of if			
}

int  CMysqlConnection::_ParseDbHostName(const std::string& strHost)
{
	int nRet = Ret_Success;
	size_t uSize = 0;
	char* pDelimPos = NULL;
	DEF_ERROR_CHECK(!strHost.empty());
	char pDbConStr[2 * defMaxNameLen] = {0};

	uSize = strHost.size();
	TA_ASSERT(uSize > 0, defErrMsg(Err_NoData_inFile));
	memcpy(pDbConStr, strHost.c_str(), uSize);
	pDelimPos = strchr(pDbConStr, defConfig_srcIpDelimit);
	if (!pDelimPos)
	{
		m_vecSrvIps.push_back(pDbConStr);
	}
	else
	{
		char* pBegin = pDbConStr;
		char* pEnd   = pDelimPos;
		pDelimPos++;
		(*pEnd) = 0;
		m_vecSrvIps.push_back(pBegin);
		while (pDelimPos)
		{
			pBegin    = pDelimPos;
			pDelimPos = strchr(pDelimPos, defConfig_srcIpDelimit);
			if ( pDelimPos )
			{
				pEnd = pDelimPos;
				pDelimPos++;
				(*pEnd) = 0;				
			}
			m_vecSrvIps.push_back(pBegin);
		}
	}

	return nRet;
}

bool CMysqlConnection::isOpen()
{
	FUNCTION_ENTRY("isOpen");

	ThreadGuard  guard(m_lockCon);
	try
	{
		FUNCTION_EXIT;
		if (NULL == m_pConnection)
			return false;
		return (!m_pConnection->isClosed());
	}	
	catch(sql::SQLException& e)
	{
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}		
	catch(...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		throw db;
	}	
}

void CMysqlConnection::open()
{
	FUNCTION_ENTRY("open");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: Fetching thread guard.");
	ThreadGuard  guard(m_lockCon);
	if ( m_pConnection && !m_pConnection->isClosed())
	{		
		FUNCTION_EXIT;
		return;
	}
	
	try  // connection closed, reconnect to the database server
	{		
		_CreateConnection(m_strSchema, m_strUserName, m_strPwd);		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: create new connection.");
	}
	catch(sql::SQLException & e)
	{
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}
	catch(const CDBException& e)
	{
		throw e;
	}	
	catch(...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
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
void CMysqlConnection::close()
{
	FUNCTION_ENTRY("close");
	ThreadGuard  guard(m_lockCon);
	if (NULL == m_pConnection || m_pConnection->isClosed())
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "MysqlConnection::close(): Database is already closed" );
		FUNCTION_EXIT;
		return;
	}	
	
	m_pConnection->close();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in close() function.");	
	
	FUNCTION_EXIT;
}


/**
* closeAll
*
* Ends all sessions with the database if there are any running.
*
* @return true if the sessions were ended, false otherwise
*/
void CMysqlConnection::closeAll()
{
	FUNCTION_ENTRY("closeAll");	
	close();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "FAILOVERDBLOG: connection closed in closeAll() function.");
	FUNCTION_EXIT;
}

void CMysqlConnection::setAutoCommit(bool bAutoCommit)
{
	m_pConnection->setAutoCommit( bAutoCommit );
}


bool CMysqlConnection::getAutoCommit()
{
	return m_pConnection->getAutoCommit();
}

void CMysqlConnection::rollback()
{
	m_pConnection->rollback();
}


void CMysqlConnection::commit()
{
	m_pConnection->commit();
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
void CMysqlConnection::exec( const std::string& statement, IQueryAdapter*& pIQueryAdapter, bool isQuery /*= false*/, int prefetch /*= 10*/)
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
		TA_THROW( CDBException("MysqlConnection_ERROR", "Mysql connection is null", 0) );
	}
	
	sql::ResultSet* pQuery = NULL;	
	try
	{
		ThreadGuard  guard(m_lockCon);
		// check the database connection first, if it already closed, open it
		if (m_pConnection->isClosed())
		{
			open();
		}		
		std::auto_ptr< sql::Statement > stmt(m_pConnection->createStatement());
		if (isQuery) //select *
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
				"exec stmt->executeQuery( statement )[statement=%s]", statement.c_str());

			pQuery = stmt->executeQuery(statement);
		}
        else //like the insert command
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugSQL,
				"exec stmt->execute( statement )[statement=%s]", statement.c_str());

			bool bRetCode = stmt->execute(statement);
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
	catch(sql::SQLException& e)
	{
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		DEF_DELETE(pQuery);
		throw db;
	}
	catch(const CDBException& e)
	{
		throw e;
	}
	catch(...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		DEF_DELETE(pQuery);
		throw db;
	}	
	
	//just save the pointer, here
    if ( NULL != pQuery )
    {
        pIQueryAdapter = new CMysqlQueryAdapter( pQuery );
        m_mapActiveQuery[pIQueryAdapter] = QUERY_EXIST;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "MysqlQueryAdapter: 0x%p is created for connection: 0x%p", pIQueryAdapter, this );
    }

	FUNCTION_EXIT;
}

void CMysqlConnection::fetchBLOB(const std::string &strSQL, 
	const std::string &colomnName, 
	DataClobArray& data )
{
	FUNCTION_ENTRY("fetchBLOB");
	try
	{
		ThreadGuard  guard(m_lockCon);
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
					std::istream* ssblob = pQuery->getBlob(colomnName);
					(*ssblob) >> strData;
				}
				break;			
			default:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The Column data type is not a Blob type" );
				TA_THROW( CDBException("MysqlConnection_ERROR", "The Column data type is not a Blob type", 0) );
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
	catch( sql::SQLException& e )
	{
		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}		
	catch(...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		throw db;
	}
	
	FUNCTION_EXIT;
}


void CMysqlConnection::updateBLOB(
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
		
		ThreadGuard  guard(m_lockCon);
		if (m_pConnection->isClosed())
		{
			open();
		}

		SqlStatement << "UPDATE " << strTableName << " SET " << colomnName << "=  ? ";	
		if ( strWhereClause.length() > 0 )
		{
			SqlStatement << " WHERE " << strWhereClause;
		}	
		
		std::auto_ptr< sql::PreparedStatement >pstmt(m_pConnection->prepareStatement( SqlStatement.str() ));
		
		if (data.size() > 0)  // have data to insert
		{
			std::ostringstream output;
			std::copy(data.begin(), data.end(), std::ostreambuf_iterator<char>(output));
			pstmt->setString(1, output.str());
			if (pstmt->execute())
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "[mysql-info], Not a update like statement." );
			
	    	pstmt->clearParameters();
	}
						
	}
	catch(  sql::SQLException& e)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to exec SQL statement '%s'",
			SqlStatement.str().c_str() );

		CDBException db("Mysql_ERROR", e.what(), e.getErrorCode());
		throw db;
	}		
	catch(...)
	{
		CDBException db("MysqlConnection_ERROR", "unknown database exception", 0);
		throw db;
	}	
	
	FUNCTION_EXIT;
}

bool CMysqlConnection::isAnyQueryActive()
{
    FUNCTION_ENTRY("isAnyQueryActive");

    for ( std::map<IQueryAdapter*, long>::iterator itLoop = m_mapActiveQuery.begin(); 
            m_mapActiveQuery.end() != itLoop; ++itLoop )
    {
        if ( NULL == itLoop->first )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer of OCIQueryAdapter" );
            continue;
        }

        if ( (itLoop->first)->isActive())
        {
            FUNCTION_EXIT;
            return true;
        }
    }

    FUNCTION_EXIT;
    return false;
}

void CMysqlConnection::cleanQuery( IQueryAdapter*& pQuery )
{
    FUNCTION_ENTRY("cleanQuery");

    if ( NULL == pQuery )
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "The input query parameter has been deleted: pQuery = NULL" );
        return;
    }

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
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "MysqlConnection: 0x%p is deleted by DatabaseImpl", pQuery );

        m_mapActiveQuery.erase( itExistQuery );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Specified query is not found in this connection, the old connection has been reaped" );
        if ( !pQuery->cleanQuery() )
        {
			DEF_DELETE(pQuery);
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "MysqlConnection: 0x%p is deleted by SimpleDb through it has been reaped", pQuery );
        }
        // else leave it to ConnectionReaper for deleting
    }

    // Set pointer to NULL
    pQuery = NULL;

    FUNCTION_EXIT;
}


int CMysqlConnection::enqMsg(CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt)
{
	FUNCTION_ENTRY("enqMsg");
	int nFunRes = Ret_Success;

	if (NULL == m_pMAQCmd || !m_pConnection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MAQ Command is null" );
		TA_THROW( CDBException("MAQCmd_ERROR", "MAQ Command is null", 0) );
	}

	// check the database connection first, if it already closed, open it
	if (m_pConnection->isClosed())
	{
		open();
	}
	
	nFunRes = m_pMAQCmd->mAQEnq(pEnqMsg, pEnqOpt);

	FUNCTION_EXIT;
	return nFunRes;
}

int	CMysqlConnection::enqMsg( std::list<CMessage*>& EnqMsgList, PMAQ_ENQ_OPT pEnqOpt )
{
	FUNCTION_ENTRY("enqMsg std::list");
	int nFunRes = Ret_Success;

	

	FUNCTION_EXIT;
	return nFunRes;
}


int CMysqlConnection::deqMsg(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg)
{
	FUNCTION_ENTRY("deqMsg");
	int nFunRes = Ret_Success;

	if (NULL == m_pMAQCmd || !m_pConnection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MAQ Command is null" );
		TA_THROW( CDBException("MAQCmd_ERROR", "MAQ Command is null", 0) );
	}

	// check the database connection first, if it already closed, open it
	if (m_pConnection->isClosed())
	{
		open();
	}
	nFunRes = m_pMAQCmd->mAQDeq(strQName, pDeqMsg, rLstMsg);

	FUNCTION_EXIT;
	return nFunRes;
}

bool CMysqlConnection::listen(const std::string& strQName, u_int  nSubID)
{
	FUNCTION_ENTRY("listen");
	bool bFunRes = false;

	if (NULL == m_pMAQCmd || !m_pConnection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MAQ Command is null" );
		TA_THROW( CDBException("MAQCmd_ERROR", "MAQ Command is null", 0) );
	}

	// check the database connection first, if it already closed, open it
	if (m_pConnection->isClosed())
	{
		open();
	}
	bFunRes = m_pMAQCmd->mAQListen(strQName, nSubID);

	FUNCTION_EXIT;
	return bFunRes;
}

bool CMysqlConnection::listen(const std::string& strQName, const std::string& strSubName)
{
	FUNCTION_ENTRY("listen");
	bool bFunRes = false;

	if (NULL == m_pMAQCmd || !m_pConnection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MAQ Command is null" );
		TA_THROW( CDBException("MAQCmd_ERROR", "MAQ Command is null", 0) );
	}

	// check the database connection first, if it already closed, open it
	if (m_pConnection->isClosed())
	{
		open();
	}
	bFunRes = m_pMAQCmd->mAQListen(strQName, strSubName);

	FUNCTION_EXIT;
	return bFunRes;
}

int CMysqlConnection::aqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& rSeqIDPair)
{
	FUNCTION_ENTRY("aqCommit");
	int nFunRes = Ret_Success;

	if (NULL == m_pMAQCmd || !m_pConnection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MAQ Command is null" );
		TA_THROW( CDBException("MAQCmd_ERROR", "MAQ Command is null", 0) );
	}

	// check the database connection first, if it already closed, open it
	if (m_pConnection->isClosed())
	{
		open();
	}
	m_pMAQCmd->mAQDeqCommit(strQName, nSubID, strConsumer, rSeqIDPair);

	FUNCTION_EXIT;
	return nFunRes;

}

int CMysqlConnection::aqCommit(const std::string& strQName, const std::string& strMsgID)
{
	FUNCTION_ENTRY("aqCommit");
	int nFunRes = Ret_Success;

	if (NULL == m_pMAQCmd || !m_pConnection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MAQ Command is null" );
		TA_THROW( CDBException("MAQCmd_ERROR", "MAQ Command is null", 0) );
	}

	// check the database connection first, if it already closed, open it
	if (m_pConnection->isClosed())
	{
		open();
	}
	m_pMAQCmd->mAQDeqCommit(strQName, strMsgID);

	FUNCTION_EXIT;
	return nFunRes;

}

int CMysqlConnection::aqGetSrvID()
{
	FUNCTION_ENTRY("aqGetSrvID");
	int nFunRes = Ret_Success;

	if (NULL == m_pMAQCmd || !m_pConnection)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "MAQ Command is null" );
		TA_THROW( CDBException("MAQCmd_ERROR", "MAQ Command is null", 0) );
	}

	// check the database connection first, if it already closed, open it
	if (m_pConnection->isClosed())
	{
		open();
	}
	nFunRes = m_pMAQCmd->mAQGetSrvID();

	FUNCTION_EXIT;
	return nFunRes;
}

NS_END(TA_AQ_Lib)


