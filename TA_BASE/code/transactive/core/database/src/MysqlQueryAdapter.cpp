//////////////////////////////////////////////////////////////////////
///		@file		MysqlQueryAdapter.h
///		@author		zhilin,ouyang
///		@date		2010-11-24 14:44:41
///
///		@brief	    Implementation of the interface IQueryAdapter. 
///					use mysql database server to implement the query  
///                 operations.
//////////////////////////////////////////////////////////////////////
#include <time.h>
//#include "core/mcl/cppconn/sqlstring.h"
#include "driver/mysql_public_iface.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/DBException.h"
#include "core/database/src/MysqlQueryAdapter.h"

NS_BEGIN(TA_Base_Core)

MysqlQueryAdapter::MysqlQueryAdapter( sql::ResultSet* pQuery ) : 
m_pQuery( pQuery ),
m_bConnectionClosed( false ),
m_tmLatestActionTime( 0 )
{
    TA_ASSERT( NULL != m_pQuery, "Can't initialize with NULL pointer of mysql::ResultSet" );
    _UpdateLatestActionTime();
}


MysqlQueryAdapter::~MysqlQueryAdapter()
{
    cleanQuery();
}

bool MysqlQueryAdapter::hasMore()
{
    ThreadGuard guardQuery( m_queryLock );

    _UpdateLatestActionTime();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    return (!m_pQuery->isAfterLast());
}

void MysqlQueryAdapter::fetchInitial( int numColumns, Buffer& data, long lMaxRows )
{
    ThreadGuard guardQuery( m_queryLock );

    _UpdateLatestActionTime();
    data.clear();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    try
    {
        if ( m_pQuery->rowsCount() > 0 )
        {
            m_pQuery->first();
            _FillDataBuffer(numColumns, data, lMaxRows);
        }
    }
    catch( sql::SQLException & e )
    {
        DBException db( "MCL_ERROR", e.getSQLStateCStr(), e.getErrorCode() );
        TA_THROW( db );
    }
    catch( DBException & e )
    {
        std::ostringstream osMsg;
		osMsg << "Status:" << e.getStatus() << " Summary:" << e.getSummary() 
			  << " Reason:" << e.getReason() <<" NativeCode: "<< e.getNativeCode() << std::endl;  
        LOG_EXCEPTION_CATCH(SourceInfo, "Database exception: ", osMsg.str().c_str());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
    }
}

void MysqlQueryAdapter::fetchMore( int numColumns, Buffer& data, long lMaxRows )
{
    TA_ASSERT( numColumns <= FETCH_MAX_COLS                , "Number of columns is out of range");
    TA_ASSERT( (lMaxRows > 0) && (lMaxRows <= FETCH_MAX_ROWS), "Number of rows is out of range"   );

    ThreadGuard guardQuery( m_queryLock );

    _UpdateLatestActionTime();
    data.clear();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    try
    {
        if ( !m_pQuery->rowsCount() )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "no rows returns from SQL");
            return;
        }

        if ( m_pQuery->getRow() + lMaxRows <= m_pQuery->rowsCount() )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "{MCL}: current record no is %d; and get batch record count %d after this round.", m_pQuery->getRow(), m_pQuery->rowsCount());
            _FillDataBuffer( numColumns, data, lMaxRows );
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "{MCL}: current record no is %d; and batch record count, %d after this round", m_pQuery->getRow(), m_pQuery->rowsCount());
            _FillDataBuffer( numColumns, data, m_pQuery->rowsCount() - m_pQuery->getRow() + 1 ); //1-based numbering mechnism. be cafeful
        }
    }
    catch( sql::SQLException & e )
    {
        DBException db("MYSQL_ERROR", e.getSQLStateCStr(), e.getErrorCode());
        TA_THROW( db );
    }
	catch(const DBException& e)
	{
		std::ostringstream osMsg;
		osMsg << "Status:" << e.getStatus() << " Summary:" << e.getSummary() 
			<< " Reason:" << e.getReason() <<" NativeCode: "<< e.getNativeCode() << std::endl; 
		LOG_EXCEPTION_CATCH(SourceInfo, "Database exception: ", osMsg.str().c_str());
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
    }
}

void MysqlQueryAdapter::_FillDataBuffer( int numColumns, Buffer& data, long lMaxRows )
{
    ThreadGuard guardQuery( m_queryLock );

    data.clear();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

	sql::ResultSetMetaData* pRsMetaData = m_pQuery->getMetaData() ; // use of auto_prt, need not delete
	TA_ASSERT(NULL != pRsMetaData, "get result set MetaData pointer failed");
    int nFieldNumber = pRsMetaData->getColumnCount();
    TA_ASSERT(numColumns <= nFieldNumber, "column number is bigger than the SQL returns");

    try
    {
        long lOffset = 0;
        while( !m_pQuery->isAfterLast() && ( lOffset < lMaxRows ))
        {
            Row dataRow;
            for ( int nColLoop = 0; nColLoop < numColumns; ++nColLoop )
            {
				std::string strFieldName = pRsMetaData->getColumnName(nColLoop + 1);  // 1-based 
                std::string strValue = m_pQuery->getString(nColLoop + 1);
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
                    "SQL field name:[%s]; value:[%s]", strFieldName.c_str(), strValue.c_str());
                dataRow.push_back(strValue);
            }
            data.push_back(dataRow);
            m_pQuery->next();
            ++lOffset;
        }
    }
    catch( sql::SQLException & e )
    {
        TA_THROW( DBException ("MYSQL_ERROR", e.getSQLStateCStr(), e.getErrorCode()) );
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
    }
}

bool MysqlQueryAdapter::cleanQuery()
{
    FUNCTION_ENTRY("cleanQuery");

    ThreadGuard guardQuery( m_queryLock );

    if ( NULL == m_pQuery )
    {
        FUNCTION_EXIT;
        return false;
    }

    try
    {
        if ( !m_pQuery->isClosed() )
        {
            m_pQuery->close();
        }
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        // Continue to delete query, it must be deleted.
    }

    try
    {
		if (NULL != m_pQuery)
		{
			delete m_pQuery;
            m_pQuery = NULL;
		}        
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        TA_ASSERT( false, expWhat.what() );
        return false;
    }

    FUNCTION_EXIT;
    return true;
}

bool MysqlQueryAdapter::isActive( unsigned long ulMaxTimeoutInSecs )
{
    FUNCTION_ENTRY("isActive");

    ThreadGuard guardQuery( m_queryLock );

    if ( NULL == m_pQuery )
    {
        FUNCTION_EXIT;
        return false;
    }
    time_t tmNoneActiveTime = time( NULL ) - m_tmLatestActionTime;
    if ( ulMaxTimeoutInSecs <= static_cast<unsigned long>(tmNoneActiveTime) )
    {
        FUNCTION_EXIT;
        return false;
    }

    FUNCTION_EXIT;
    return (!m_pQuery->isLast());
}

void MysqlQueryAdapter::_UpdateLatestActionTime()
{
    // update latest action time in public member function
    m_tmLatestActionTime = time( NULL );
}

NS_END(TA_Base_Core)