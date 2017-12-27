//////////////////////////////////////////////////////////////////////
///		@file		IDBConnector.h
///		@author		zhilin,ouyang
///		@date		2012-01-12 14:44:41
///
///		@brief	    Implementation of the Class CDatabaseImpl 
///                 such as MySQL and Oracle etc. for the database 
///                 synchronization module.
///                 
//////////////////////////////////////////////////////////////////////
#include "DatabaseImpl.h"
#include "AQCommonData.h"
#include "DbDataImpl.h"
#include "Message.h"
#include "MysqlConnection.h"
#include "MysqlQueryAdapter.h"
#include "OracleConnection.h"
#include "OracleQueryAdapter.h"

#include "DBException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/exceptions/src/SQLException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "UtilityFun.h"

NS_BEGIN(TA_AQ_Lib)

using TA_Base_Core::DebugUtil;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DbConnectionFailed;

CDatabaseImpl::CDatabaseImpl(void)
: m_pDbConnection(NULL)
, m_pQueryAdapter(NULL)
, m_numRows(FETCH_MAX_ROWS)
, m_bAutoCommit(false)
{

}

CDatabaseImpl::CDatabaseImpl(const std::string& strConnection)
: m_pDbConnection(NULL)
, m_pQueryAdapter(NULL)
, m_numRows(FETCH_MAX_ROWS)
, m_bAutoCommit(false)
{
	_ParseDbConStr(strConnection);
}

int CDatabaseImpl::setDbConStr(const std::string& strConStr)
{
	return _ParseDbConStr(strConStr);
}
 
void CDatabaseImpl::connect()
{
	FUNCTION_ENTRY("connect");	

    if ( NULL == m_pDbConnection ) 
	{
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "CDatabaseImpl::connect(): Database is: %s", m_strSchema.c_str());	
		try
		{
			switch ( m_nDbType )
			{
			case enumMysqlDb:
				m_pDbConnection = new CMysqlConnection(m_strSchema, m_strUserName, m_strPwd, m_strHostName);
				if (m_pDbConnection)
					m_bAutoCommit = m_pDbConnection->getAutoCommit();
				break;
			case enumOracleDb:
				m_pDbConnection = new COracleConnection(m_strSchema, m_strUserName, m_strPwd, m_strHostName);

				break;
			}
		}
		catch( const CDBException& e )
		{            
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );
			FUNCTION_EXIT;
			TA_THROW( e );
		}
	}
    
	FUNCTION_EXIT;
}


void CDatabaseImpl::disconnect()
{
	FUNCTION_ENTRY("disconnect");
	if (NULL != m_pDbConnection)
	{
		try
		{
			m_pDbConnection->close();
		}
		catch( const CDBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );
		}

		DEF_DELETE(m_pDbConnection);
	}
	FUNCTION_EXIT;
}

CDatabaseImpl::~CDatabaseImpl()
{
	FUNCTION_ENTRY("~CDatabaseImpl");

	cleanQuery();
	disconnect();
	
	m_columnNames.clear();

	FUNCTION_EXIT;
}

void CDatabaseImpl::cleanQuery()
{
	FUNCTION_ENTRY("cleanQuery");
	if(m_pDbConnection != NULL)
	{
		try
		{
			m_pDbConnection->cleanQuery(m_pQueryAdapter);
		}
		catch ( ... )
		{
			LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DBException", "database cleanQuery failure");

		}
	}	
	FUNCTION_EXIT;
}

IDbData* CDatabaseImpl::executeQuery(const std::string& sql,const std::vector<std::string>& columnNames)
{
	return( executeQuery( sql, columnNames, FETCH_MAX_ROWS ) );
}

IDbData* CDatabaseImpl::executeQuery(const std::string& sql,const std::vector<std::string>& columnNames,
									  const u_long numRows )
{
	FUNCTION_ENTRY("executeQuery");

    // Assert the pre-conditions on this function
	TA_ASSERT(0 < sql.size(),"SQL statement passed in is zero size");
	TA_ASSERT(0 < numRows, "numRows is 0 - this is not a sensible number");
    TA_ASSERT((columnNames.size() <= FETCH_MAX_COLS), "number of columns is greater than the max fetch columns");
	TA_ASSERT(0 < columnNames.size(), "columnNames is zero size");
	TA_ASSERT(NULL != m_pDbConnection, "Database pointer is null."); 

	LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugSQL,
		"Begin to execute the SQL: %s", sql.c_str());

	// Create a buffer in which to store the retrieved data
	Buffer data;
	try
	{
		_TryToOpenDb();

		cleanQuery();

		m_pDbConnection->exec( sql, m_pQueryAdapter, true, numRows );

		if ( m_pQueryAdapter )
		{
			m_pQueryAdapter->fetchInitial( columnNames.size(), data, numRows );
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is not got" );
		}
	}
	catch( const CDBException& e )
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );
		if ( m_pQueryAdapter )
			m_pQueryAdapter->cleanQuery();		
		TA_THROW( DbConnectionFailed(e.getReason()) );
        
    }
	catch(const DatabaseException& e)
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", e.what() );
		if ( m_pQueryAdapter )
			m_pQueryAdapter->cleanQuery();
		TA_THROW( e );
	}

	// Set the member variables to match the passed-in variables
	m_columnNames = columnNames;
	

	// Place the data we just loaded into an instance of TA_Base_Core::SimpleDbData (which IS A IData)
	CDbDataImpl* idata = new CDbDataImpl(m_columnNames, data);
	m_numRows = idata->getNumRows();

	LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugSQL, "End execute the SQL");

	FUNCTION_EXIT;
	return idata; // Return a pointer to the new IData object
}



void CDatabaseImpl::executeModification(const std::string& sql)
{
	FUNCTION_ENTRY("executeModification");

	/*LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugSQL,
		"Begin to execute the modification SQL: %s", sql.c_str());*/

	// Assert the pre-conditions on this function
	TA_ASSERT(0 < sql.size(),"SQL statement passed in is zero size");
	TA_ASSERT(NULL != m_pDbConnection, "Database pointer is null."); // connect has not been called		

	try
	{
		IQueryAdapter* pQuery = NULL;
		_TryToOpenDb();

		m_pDbConnection->exec( sql, pQuery, false );

	}
	catch( const CDBException& e )
	{
		//LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );
		throw( CDBException("Mysql_ERROR", e.getReason().c_str(), e.getNativeCode()) );
	}
	catch(const DatabaseException& e)
	{
		// if we catch anything in here, it means the db in used no long available
		// so just record the log, next time the application getDatabase will try another db
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", e.what() );
		TA_THROW( DatabaseException(e.what()) );
	}

	/*LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugSQL, "End execute the modification SQL");*/

	FUNCTION_EXIT;
}


void CDatabaseImpl::executeProcedure(const std::string& sql)
{
	// Oracle procedures are executed in the same manner as standard queries - therefore we use
	// the same code
	executeModification(sql);
}


bool CDatabaseImpl::moreData(IDbData*& returnData)
{
	FUNCTION_ENTRY("moreData");

	// The IData pointer MUST be null when passed to this method. If it is not, a memory leak 
	// will result. This is tested by the assert.
	TA_ASSERT(NULL != m_pDbConnection, defErrMsg(Err_Invalid_Ptr));	
	if (! m_pDbConnection->isOpen())
	{
		try
		{
			_TryToOpenDb();
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Database should be opened while requesting more data,something wrong may occurred");
		}			
		catch (DatabaseException& )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
				"Error Code: %d, Message: %s",Err_DbException, defErrMsg(Err_DbException));
			return false;
		}
		catch(...)
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,  defErrMsg(Err_Unknown));
			return false;
		}
		
	}

	//TA_ASSERT(NULL != m_pQueryAdapter, defErrMsg(Err_Invalid_Ptr));
	if(!m_pQueryAdapter)  return false;
	if (m_pQueryAdapter->hasMore()) // There is data to read
	{
		Buffer data;
		try
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDatabase, "CDatabaseImpl::moreData calling FetchMore...");
			m_pQueryAdapter->fetchMore( m_columnNames.size(), data, FETCH_MAX_ROWS );
		}
		catch( const CDBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );
			TA_THROW(DatabaseException(e.getReason().c_str()) );
		}

        // Check if there was actually more data to retrieve
        if(data.size() == 0)
        {
			FUNCTION_EXIT;
            return false;
        }

		returnData = new CDbDataImpl(m_columnNames, data);
		FUNCTION_EXIT;
		return true;
	}
	else
	{
		m_pQueryAdapter->cleanQuery();
	}

	FUNCTION_EXIT;
	return false; // There was no more data to read
}


void CDatabaseImpl::readBLOB( const std::string& fieldName, const std::string& tableName, 
								 const std::string& whereClause, std::vector<unsigned char>& data )
{
	TA_ASSERT( m_pDbConnection != NULL, "Database pointer has not been initialised" );
	TA_ASSERT( fieldName.length() > 0, "The BLOB field name has not been specified" );
	TA_ASSERT( tableName.length() > 0, "The BLOB table name has not been specified" );
	
	_TryToOpenDb();
	
	try
	{
		data.clear();

		// Format a SELECT statement
		std::stringstream selectStatement;

		selectStatement << "SELECT " << fieldName << " FROM " << tableName;
		if ( whereClause.length() > 0 )
		{
			selectStatement << " WHERE " << whereClause;
		}

		DataClobArray dataArray;
		m_pDbConnection->fetchBLOB(selectStatement.str(), fieldName, dataArray);

	}
	catch( const CDBException& e )
	{
        // this db has gone bad, try another one
        disconnect();
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to read BLOB filed \
			%s from table %s with clause %s", fieldName.c_str(), tableName.c_str(), whereClause.c_str() );
	}
}	


void CDatabaseImpl::writeBLOB( const std::string& fieldName, const std::string& tableName, 
								  const std::string& whereClause, std::vector<unsigned char>& data )
{
	TA_ASSERT( m_pDbConnection != NULL, "Database pointer has not been initialised" );
	TA_ASSERT( fieldName.length() > 0, "The BLOB field name has not been specified" );
	TA_ASSERT( tableName.length() > 0, "The BLOB table name has not been specified" );
	
	_TryToOpenDb();

	try
	{
		m_pDbConnection->updateBLOB( tableName, fieldName, data, whereClause );
	}
	catch( const CDBException& e )
	{
        // this db has gone bad, try another one
        disconnect();
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Failed to write BLOB filed \
			%s to table %s with clause %s", fieldName.c_str(), tableName.c_str(), whereClause.c_str() );
	}
}	


//Mintao++: use for Distributed DB
std::string CDatabaseImpl::escapeAQSQLString(const std::string& theString)
{
    return (escapeInsertString(escapeInsertString(theString)));         
}

int CDatabaseImpl::getDbServerType()
{
	return _GetDbServerType();
}

int  CDatabaseImpl::getDBServerID()
{
	int nFunRes = 0;
	if ( m_pDbConnection )
	{
		nFunRes = m_pDbConnection->aqGetSrvID();
	}
	else
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Error Code: %d; Message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr) );
	}
	return nFunRes;
}

int CDatabaseImpl::_GetDbServerType()
{
	return m_nDbType;
}

void CDatabaseImpl::prepareSQLStatement(std::string& strSQL, const SQLTypeAdapter& arg0)
{
	return prepareSQLStatement(strSQL, SQLVarParms() << _GetDbServerType() << arg0);
}

void CDatabaseImpl::prepareSQLStatement(std::string& strSQL, SQLVarParms& varParms)
{
	FUNCTION_ENTRY("prepareSQLStatement");
	try
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugSQL, "Begin prepare SQL statement...");
	/*	std::string strSQL;
		SQLVarParms  sqlVarParam(varParms);*/
		strSQL.clear();
		CSQLCode::getInstance().buildSQLStatement(varParms, strSQL);		

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugSQL, "End prepare SQL statement.");
		//return strSQL;
	}
	catch (BadParamCount* e)
	{
		SQLCodeException SQLException(e->what());
		throw SQLException;
	}
	catch (BadIndex* e)
	{
		SQLCodeException SQLException(e->what());
		throw SQLException;
	}
	catch (...)
	{
		SQLCodeException SQLException("Unknown SQLCode exception");
		throw SQLException;
	}
	FUNCTION_EXIT;
}	

std::string CDatabaseImpl::escapeInsertString(const std::string& theString)
{
    // create a new string to hold the parsed string
    std::string escapedString = theString;
    size_t pos = escapedString.find_first_of("'");
    while (DEF_RET_FAILED != pos)
    {
        char charToInsert = escapedString.at(pos);
        escapedString.insert(pos,&charToInsert, 1);
		pos = escapedString.find_first_of("'", pos + 2);
    }
    return escapedString;
}

std::string CDatabaseImpl::escapeQueryString(const std::string& theString)
{
    return escapeInsertString(theString);
}


void CDatabaseImpl::beginTransaction()
{
	if ( m_pDbConnection && m_bAutoCommit )
	{
		m_pDbConnection->commit();              // commit previous transaction in case of hang
		m_pDbConnection->setAutoCommit(false);
	}
}


void CDatabaseImpl::commitTransaction()
{
	if ( m_pDbConnection )
	{
		m_pDbConnection->commit();
		m_pDbConnection->setAutoCommit(m_bAutoCommit);
	}
	else
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Error Code: %d; Message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr) );
	}
}

void CDatabaseImpl::rollbackTransaction()
{
	if ( m_pDbConnection )
	{
		m_pDbConnection->rollback();
		m_pDbConnection->setAutoCommit(m_bAutoCommit);
	}
	else
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Error Code: %d; Message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr) );
	}
}

void CDatabaseImpl::_TryToOpenDb()
{
	if( m_pDbConnection && false == m_pDbConnection->isOpen() )
	{
		try
		{
			m_pDbConnection->open();
		}
		catch( const CDBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::CDBException", e.getSummary().c_str() );
			if (e.getNativeCode() == INVALID_USERNAME_PASSWORD)
			{
				TA_THROW(e);
			}else
			{
				TA_THROW( DatabaseException(e.getReason().c_str()) );
			}
		}
	}
}

// MySQL:TRA_OCC:IT271350_5:IT271350_5:192.168.0.1;192.168.0.2
int  CDatabaseImpl::_ParseDbConStr(const std::string& strDbCon)
{
	int nRet = Ret_Success;
	size_t uSize = 0;
	char* pDelimPos = NULL;
	DEF_ERROR_CHECK(!strDbCon.empty());
	StrVecT  vecDbConStr;

	uSize = strDbCon.size();
	std::string comp;
	for (size_t i = 0; i < uSize; i++)
	{
		if (strDbCon[i] == defConfig_DbConStrTag)
		{
			vecDbConStr.push_back(comp);
			comp.clear();
		}
		else
		{
			comp += strDbCon[i];
		}
	}
	vecDbConStr.push_back(comp);
	TA_ASSERT(vecDbConStr.size() >= MIN_DBCONNECTION_STR_SIZE, defErrMsg(Err_DbConnectionStr_Invalid));
	nRet = _ParseDbType(vecDbConStr[0]);
	DEF_ERROR_CHECK(!nRet);
	m_strSchema = vecDbConStr[1];
	m_strUserName = vecDbConStr[2];
	m_strPwd = vecDbConStr[3];
	if (vecDbConStr.size() > MIN_DBCONNECTION_STR_SIZE)
	{
		m_strHostName = vecDbConStr[4];
	}

	/*uSize = strlen(g_system_variables.m_strLocalDbName);
	if (!uSize) 
		memcpy(g_system_variables.m_strLocalDbName, m_strSchema.c_str(), m_strSchema.size());*/

	return nRet;
}




int  CDatabaseImpl::_ParseDbType(const std::string& strDbName)
{
	int nRet = Ret_Success;
	

	if(CUtilityFun::stricmp_(strDbName.c_str(), defDefault_Config_DbOracle) == 0) //case insensitive
	{
		m_nDbType = static_cast<int>(enumOracleDb);
	}
	else if (CUtilityFun::stricmp_(strDbName.c_str(), defDefault_Config_DbMySQL) == 0)
	{
		m_nDbType = static_cast<int>(enumMysqlDb);
	}
	else   
	{    // should never reach here, if reach here, then raise a exception
		LOG_GENERIC( SourceInfo, 
			TA_Base_Core::DebugUtil::DebugError,
			"the DB type string in config file is wrong or not support currently(should be: Oracle or MySQL): %s", strDbName.c_str() );
		TA_ASSERT(false, "DB Type string error");  // make a exception deliberately
	}

	return nRet;
}



int CDatabaseImpl::enqMsg( CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt )
{
	int nFunRes = 0;

	if (NULL != m_pDbConnection)
	{
		nFunRes = m_pDbConnection->enqMsg(pEnqMsg, pEnqOpt);
	}
	else
	{
		nFunRes = -1;
	}

	return nFunRes;
}


int CDatabaseImpl::enqMsg( std::list<CMessage*>& EnqMsgList, PMAQ_ENQ_OPT pEnqOpt )
{
	int nFunRes = 0;

	if (NULL != m_pDbConnection)
	{
		nFunRes = m_pDbConnection->enqMsg(EnqMsgList, pEnqOpt);
	}
	else
	{
		nFunRes = -1;
	}

	return nFunRes;
}

int CDatabaseImpl::deqMsg( const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg )
{
	int nFunRes = 0;

	if (NULL != m_pDbConnection)
	{
		nFunRes = m_pDbConnection->deqMsg(strQName, pDeqMsg, rLstMsg);
	}
	else
	{
		nFunRes = -1;
	}

	return nFunRes;
}

bool CDatabaseImpl::listen(const std::string& strQName, u_int  nSubID)
{
	bool bFunRes = false;

	if (NULL != m_pDbConnection)
	{
		bFunRes = m_pDbConnection->listen(strQName, nSubID);
	}	

	return bFunRes;
}

bool CDatabaseImpl::listen(const std::string& strQName, const std::string& strSubName)
{
	bool bFunRes = true;

	if (NULL != m_pDbConnection)
	{
		bFunRes = m_pDbConnection->listen(strQName, strSubName);
	}	

	return bFunRes;
}

int CDatabaseImpl::aqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& nSeqIDPair)
{
	int nFunRes = 0;
	if ( m_pDbConnection )
	{
		nFunRes = m_pDbConnection->aqCommit(strQName, nSubID, strConsumer, nSeqIDPair);
	}
	else
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Error Code: %d; Message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr) );
	}
	return nFunRes;
}

int CDatabaseImpl::aqCommit(const std::string& strQName, const std::string& strMsgID)
{
	int nFunRes = 0;
	if ( m_pDbConnection )
	{
		nFunRes = m_pDbConnection->aqCommit(strQName, strMsgID);
	}
	else
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Error Code: %d; Message: %s", Err_Invalid_Ptr, defErrMsg(Err_Invalid_Ptr) );
	}
	return nFunRes;
}

NS_END(TA_AQ_Lib)
