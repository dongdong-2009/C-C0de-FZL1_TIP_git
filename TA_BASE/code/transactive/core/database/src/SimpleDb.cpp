/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/SimpleDb.cpp $
* @author:  Ripple
* @version: $Revision: #3 $
*
* Last modification: $DateTime: 2015/04/07 14:19:42 $
* Last modified by:  $Author: Ouyang $
*
* <description>
*
*/
//
// SimpleDb.cpp
//
// Wrapper for the DbServerLocator class which (hopefully) takes all the pain
// out of database access. Note that it is probably not very efficient.
//
// Author: Bruce Fountain 24-Jan-2001
//


#include "core/ocl/src/ocl.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"
#include "core/database/src/DbServerLocator.h"
#include "core/database/src/IQueryAdapter.h"
#include "core/database/src/DBException.h"
#include "core/database/src/SimpleDb.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"
#include <boost/algorithm/string.hpp>


#define  defOracleDBName          "Oracle"
#define  defMysqlDBName           "Mysql"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::Thread;

using namespace ocl;

namespace TA_Base_Core
{
	const unsigned long SimpleDb::MAX_ROWS = FETCH_MAX_ROWS;
	const unsigned long SimpleDb::MAX_COLS = FETCH_MAX_COLS;

	SimpleDb::SimpleDb(const char* pszDbType, const char* connection, const char* user, const char* passwd, const char* hostname) : 
	m_pCurQuery(NULL), m_nDBType(DbServerLocator::enumOracleDb)
	{
		FUNCTION_ENTRY("SimpleDb");
		
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
			"SimpleDb::SimpleDb(): dbType=%s, connection=%s, user=%s, passwd=%s, hostname=%s", pszDbType, connection, user, passwd, hostname);
	
		// Create a string from the database connection details.
		// This will be used to identify the OCIConnection object.
		m_connectionStr = pszDbType;
		
		m_connectionStr += ":";
		
		m_connectionStr += connection;
		
		m_connectionStr += ":";
		
		m_connectionStr += user;
		
		m_connectionStr += ":";
		
		m_connectionStr += passwd;
		
		m_connectionStr += ":";
		
		m_connectionStr += hostname;
		
		parseDBType(pszDbType);
		
		// The final parameter is false to indicate that the connection is
		// not being created as part of auto-reconnect.
		//DbServerLocator::getInstance().setDbServerType( m_nDBType );
		DbServerLocator::getInstance().createConnection(
			m_connectionStr, pszDbType, connection, user, passwd, hostname, false );
		
		FUNCTION_EXIT;
	}

	SimpleDb::~SimpleDb()
	{
		FUNCTION_ENTRY("~SimpleDb");
		
		try
		{
			cleanQuery();
			//todo..., think it's not necessary to close so often. 
			//Close(); //TODO: need to implement it in connection class
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DBException", e.getSummary().c_str() );
		}
		
		FUNCTION_EXIT;
	}
	
	void SimpleDb::Open()
	{
		FUNCTION_ENTRY("Open");
		
		DbServerLocator::getInstance().open( m_connectionStr );
		
		FUNCTION_EXIT;
	}
	
	void SimpleDb::Close()
	{
		FUNCTION_ENTRY("Close");

		cleanQuery();
		//Cannot release connection on this class, it should be in charge by OCIAccess;
		//OCIAccess::getInstance().close( m_connectionStr );
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "SimpleDb::SimpleDb(): close database = [%s]", m_connectionStr.c_str());
		
		FUNCTION_EXIT;
	}
	
	void SimpleDb::CloseAll()
	{
		FUNCTION_ENTRY("CloseAll");
		
		cleanQuery();

		DbServerLocator::getInstance().closeAll( m_connectionStr );
		
		FUNCTION_EXIT;
	}
	
	void SimpleDb::cleanQuery()
	{
		FUNCTION_ENTRY( "cleanQuery" );
		//OCIConnection::cleanUpQuery(m_currentQuery);
        if ( NULL != m_pCurQuery )
        {
            DbServerLocator::getInstance().cleanQuery( m_connectionStr, m_pCurQuery );
        }
		FUNCTION_EXIT;
	}	

	void SimpleDb::Exec(const SQLStatement& rSqlObj)
	{
		FUNCTION_ENTRY("Exec");

		//TA_ASSERT( !statement.empty(), "SQL statement is empty" );
		//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "SQL statement:%s", statement);
		try
		{
			// Parse and delimit any % signs so that the log statement below will work.
			/*std::string convertedStatement(statement);
			unsigned int pos = convertedStatement.find('%');
			while ( pos != std::string::npos )
			{
				convertedStatement = convertedStatement.insert(pos,"%");

				pos = convertedStatement.find('%', pos+2);
			}

			LOGMORE( SourceInfo, convertedStatement.c_str());*/

			// Delegate to DbServerLocator
			DbServerLocator::getInstance().exec( m_connectionStr, rSqlObj );
		}
		catch(ocl::OraException & e)
		{
			DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
			throw db;
		}
		catch(DBException & e)
		{
			std::ostringstream osMsg;
			osMsg << "Status:" << e.getStatus() << " Summary:" << e.getSummary() << " Reason:" << e.getReason() << std::endl; 
			LOG_EXCEPTION_CATCH(SourceInfo, "Database exception: ", osMsg.str().c_str());

			int nErrCode = e.getNativeCode();
			if (ER_MYSQL_CANNT_CONNECT == nErrCode ||
				ER_MYSQL_GONE_AWAY == nErrCode ||
				ER_MYSQL_READ_ONLY == nErrCode ||
				ER_ORA_IMMIDATE_SHUTDOWN == nErrCode ||
				ER_ORA_INITORSHUTDOWN == nErrCode ||
				ER_ORA_NOT_AVALIABLE == nErrCode ||
				ER_ORA_TNS_CLOSE == nErrCode)
			{
				// if go here, it means the db gone bad, rethrow and try another one
				throw e;				
			}
			else
			{	
				//modify by shenglong.lin
				//sql error and CE project, throw to outside
				//in CE project, param  EntityName shoule be "EntityName=ConfigurationEditor"
				if (RunParams::getInstance().get(RPARAM_ENTITYNAME).compare("ConfigurationEditor") == 0)
				{
					throw DatabaseException(e.getReason());
				}
			}
		}
		FUNCTION_EXIT;
	}
	
	void SimpleDb::Exec(const SQLStatement& rSqlObj, int numColumns,
		Buffer& data, int maxrows /*=FETCH_MAX_ROWS*/)
	{
		FUNCTION_ENTRY("Exec");

		TA_ASSERT( (maxrows > 0) && (maxrows <= FETCH_MAX_ROWS), "Number of rows is out of range"   );
		//TA_ASSERT( !statement.empty(), "SQL statement is empty" );

		//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "SQL statement:%s", statement);

		try
		{
			data.clear();
			//OCIConnection::cleanUpQuery(m_currentQuery);

			//m_currentQuery = DbServerLocator::getInstance().exec( m_connectionStr, statement, maxrows); 

			cleanQuery();

			m_pCurQuery = DbServerLocator::getInstance().exec( m_connectionStr, rSqlObj, maxrows); 

			if ( NULL == m_pCurQuery )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is not got" );
			}
			else
			{
				m_pCurQuery->fetchInitial( numColumns, data, maxrows );
			}
		}
		catch(ocl::OraException & e)
		{
			DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
			throw db;
		}
		catch(DBException & e)
		{
			std::ostringstream osMsg;
			osMsg << "Status:" << e.getStatus() << " Summary:" << e.getSummary() << " Reason:" << e.getReason() << std::endl; 
			LOG_EXCEPTION_CATCH(SourceInfo, "Database exception: ", osMsg.str().c_str());

			int nErrCode = e.getNativeCode();
			if (ER_MYSQL_CANNT_CONNECT == nErrCode ||
				ER_MYSQL_GONE_AWAY == nErrCode ||
				ER_ORA_IMMIDATE_SHUTDOWN == nErrCode ||
				ER_ORA_INITORSHUTDOWN == nErrCode ||
				ER_ORA_NOT_AVALIABLE == nErrCode ||
				ER_ORA_TNS_CLOSE == nErrCode)
			{
				// if go here, it means the db gone bad, rethrow and try another one
				throw e;
			}			
		}

		FUNCTION_EXIT;
	}
	
	void SimpleDb::FetchMore(int numColumns, Buffer& data, int maxrows /*=FETCH_MAX_ROWS*/ )
	{
		FUNCTION_ENTRY("FetchMore");

        if ( NULL == m_pCurQuery )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
            // Or throw exception?
            return;
        }
        m_pCurQuery->fetchMore( numColumns, data, maxrows );

		FUNCTION_EXIT;
	}
	
	//void SimpleDb::fillDataBuffer(int numColumns, ocl::OraQuery *query, Buffer& data, long nMaxRows)
	//{
	//	TA_ASSERT(query != NULL, "query should not be NULL!");
	//	int nFieldNumber = query->fieldCount();
	//	TA_ASSERT(numColumns <= nFieldNumber, "column number is bigger than the SQL returns");
	//	data.clear();
	//	try
	//	{
	//		int nOffset = 0;
	//		while(!query->isEOF() && (nOffset < nMaxRows) )
	//		{
	//			Row dataRow;
	//			for (int i = 0; i < numColumns; i++)
	//			{
	//				CRString strFieldName = query->field(i).name();
	//				std::string value = query->field(i).getString();
	//				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
	//					"SQL field name:[%s]; value:[%s]", strFieldName.c_str(), value.c_str());
	//				dataRow.push_back(value);
	//			}
	//			data.push_back(dataRow);
	//			query->next();
	//			nOffset++;
	//		}
	//	}
	//	catch(ocl::OraException & e)
	//	{
	//		DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
	//		throw db;
	//	}
	//}
	
	void SimpleDb::readBLOB( const std::string& fieldName, const std::string& tableName,
		const std::string& whereClause, std::vector<unsigned char>& data )
	{
		FUNCTION_ENTRY("readBLOB");
		
		TA_ASSERT( fieldName.length() > 0, "No field name has been specified" );
		TA_ASSERT( tableName.length() > 0, "No table name has been specified" );
		
		DbServerLocator::getInstance().readBLOB( m_connectionStr, fieldName, tableName, whereClause, data );
		
		FUNCTION_EXIT;
	}
	
	
	void SimpleDb::writeBLOB( const std::string& fieldName, const std::string& tableName,
		const std::string& whereClause, std::vector<unsigned char>& data )
	{
		FUNCTION_ENTRY("writeBLOB");
		
		TA_ASSERT( fieldName.length() > 0, "No field name has been specified" );
		TA_ASSERT( tableName.length() > 0, "No table name has been specified" );
		
		DbServerLocator::getInstance().writeBLOB( m_connectionStr, fieldName, tableName, whereClause, data );
		
		FUNCTION_EXIT;
	}
	
	
	void SimpleDb::ParseConnectStr(const std::string& str,                  // IN
		std::vector<std::string>& components,    // OUT
		char delimiter)                          // IN
	{
		DbServerLocator::getInstance().parseConnectStr( str, components, delimiter );		
	}
	
	bool SimpleDb::IsOpen()
	{
		return DbServerLocator::getInstance().isOpen(m_connectionStr);
	}

	int SimpleDb::GetDbServerType()
	{
		return m_nDBType;
	}

	void SimpleDb::parseDBType(const std::string& strDBName)
	{
        if (boost::iequals(strDBName, defOracleDBName)) //no case sensitive
		{
			m_nDBType = static_cast<int>(DbServerLocator::enumOracleDb);
		}
		else if (boost::iequals(strDBName, defMysqlDBName))
		{
			m_nDBType = static_cast<int>(DbServerLocator::enumMysqlDb);
		}
		else   
		{    // should never reach here, if reach here, then raise a exception
			 LOG_GENERIC( SourceInfo, 
				 TA_Base_Core::DebugUtil::DebugError,
				 "the DB type string in database config file wrong(should be: Oracle or Mysql): %s", strDBName.c_str() );
			 TA_ASSERT(false, "DB Type string error");  // make a exception deliberately
		}
	}

	bool SimpleDb::IsNeedChange(const std::string& strCon)
	{
		return strCon.compare(m_connectionStr);
	}
	
	bool SimpleDb::HasMore() const
	{
		//TA_ASSERT(m_currentQuery != NULL, "m_currentQuery should not be NULL");
		//return (!m_currentQuery->isEOF());
        if ( NULL == m_pCurQuery )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
            // Or throw exception?
            return false;
        }

        return m_pCurQuery->hasMore();
	}
}
