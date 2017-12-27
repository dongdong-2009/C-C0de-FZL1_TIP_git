//////////////////////////////////////////////////////////////////////
///		@file		MysqlQueryAdapter.h
///		@author		zhilin,ouyang
///		@date		2010-11-24 14:44:41
///
///		@brief	    Implementation of the interface IQueryAdapter. 
///					use mysql database server to implement the query  
///                 operations.
//////////////////////////////////////////////////////////////////////
#include "driver/mysql_public_iface.h"
#include "MysqlQueryAdapter.h"
#include "DBException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"

NS_BEGIN(TA_AQ_Lib)

CMysqlQueryAdapter::CMysqlQueryAdapter( sql::ResultSet* pQuery ) : 
m_pQuery( pQuery ),
m_bConnectionClosed( false ),
m_tmLatestActionTime( 0 )
{
    TA_ASSERT( NULL != m_pQuery, "Can't initialize with NULL pointer of mysql::ResultSet" );
}


CMysqlQueryAdapter::~CMysqlQueryAdapter()
{
    cleanQuery();
}

bool CMysqlQueryAdapter::hasMore()
{
	TA_Base_Core::ThreadGuard guardQuery( m_queryLock );

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    return (!m_pQuery->isAfterLast());
}

void CMysqlQueryAdapter::fetchInitial( int numColumns, Buffer& data, long lMaxRows )
{
	TA_Base_Core::ThreadGuard guardQuery( m_queryLock );

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
    catch( sql::SQLException& e )
    {
        CDBException db( "MCL_ERROR", e.getSQLStateCStr(), e.getErrorCode() );
        TA_THROW( db );
    }
    catch( CDBException& e )
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

void CMysqlQueryAdapter::fetchMore( int numColumns, Buffer& data, long lMaxRows )
{
    TA_ASSERT( numColumns <= FETCH_MAX_COLS, "Number of columns is out of range");
    TA_ASSERT( (lMaxRows > 0) && (lMaxRows <= FETCH_MAX_ROWS), "Number of rows is out of range"   );

    TA_Base_Core::ThreadGuard guardQuery( m_queryLock );

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
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "no rows returns from SQL");
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
    catch( sql::SQLException& e )
    {
        CDBException db("MYSQL_ERROR", e.getSQLStateCStr(), e.getErrorCode());
        TA_THROW( db );
    }
	catch(const CDBException& e)
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

void CMysqlQueryAdapter::_FillDataBuffer( int numColumns, Buffer& data, long lMaxRows )
{
    TA_Base_Core::ThreadGuard guardQuery( m_queryLock );

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
    catch( sql::SQLException& e )
    {
        TA_THROW( CDBException ("MYSQL_ERROR", e.getSQLStateCStr(), e.getErrorCode()) );
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
    }
}

bool CMysqlQueryAdapter::cleanQuery()
{
    FUNCTION_ENTRY("cleanQuery");

    TA_Base_Core::ThreadGuard guardQuery( m_queryLock );

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

bool CMysqlQueryAdapter::isActive()
{
    FUNCTION_ENTRY("isActive");

    TA_Base_Core::ThreadGuard guardQuery( m_queryLock );

    if ( NULL == m_pQuery )
    {
        FUNCTION_EXIT;
        return false;
    }
    
    return (!m_pQuery->isClosed());
}


NS_END(TA_AQ_Lib)














