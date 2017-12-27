#include <time.h>

#include "core/ocl/src/ocl.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/DBException.h"

#include "core/database/src/OCIQueryAdapter.h"

namespace TA_Base_Core
{

// const time_t QUERY_TIME_OUT_IN_SECS = 7200; // two hours

OCIQueryAdapter::OCIQueryAdapter( ocl::OraQuery* pQuery ) : 
m_pQuery( pQuery ),
m_bConnectionClosed( false ),
m_tmLatestActionTime( 0 )
{
    TA_ASSERT( NULL != m_pQuery, "Can't initialize with NULL pointer of ocl::OraQuery" );
    updateLatestActionTime();
}

OCIQueryAdapter::~OCIQueryAdapter()
{
    cleanQuery();
}

bool OCIQueryAdapter::hasMore()
{
    ThreadGuard guardQuery( m_queryLock );

    updateLatestActionTime();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    return (!m_pQuery->isEOF());
}

void OCIQueryAdapter::fetchInitial( int numColumns, Buffer& data, long lMaxRows )
{
    ThreadGuard guardQuery( m_queryLock );

    updateLatestActionTime();
    data.clear();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    try
    {
        if ( m_pQuery->isRowsReturn() )
        {
            m_pQuery->moveTo(1);
            fillDataBuffer(numColumns, data, lMaxRows);
        }
    }
    catch( ocl::OraException & e )
    {
        DBException db( "OCI_ERROR", e.message().c_str(), e.errorCode() );
        TA_THROW( db );
    }
    catch( const DBException & e )
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

void OCIQueryAdapter::fetchMore( int numColumns, Buffer& data, long lMaxRows )
{
    TA_ASSERT( numColumns <= FETCH_MAX_COLS                , "Number of columns is out of range");
    TA_ASSERT( (lMaxRows > 0) && (lMaxRows <= FETCH_MAX_ROWS), "Number of rows is out of range"   );

    ThreadGuard guardQuery( m_queryLock );

    updateLatestActionTime();
    data.clear();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    try
    {
        if (! m_pQuery->isRowsReturn())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "no rows returns from SQL");
            return;
        }

        if ( m_pQuery->recordNo() + lMaxRows <= m_pQuery->recordCount() )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "{OCL}: current record no is %d; and get batch record count %d after this round.", m_pQuery->recordNo(), m_pQuery->recordCount());
            fillDataBuffer( numColumns, data, lMaxRows );
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "{OCL}: current record no is %d; and batch record count, %d after this round", m_pQuery->recordNo(), m_pQuery->recordCount());
            fillDataBuffer( numColumns, data, m_pQuery->recordCount() - m_pQuery->recordNo() + 1 ); //1-based numbering mechnism. be cafeful
        }
    }
    catch( ocl::OraException & e )
    {
        DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
        TA_THROW( db );
    }
	catch( const DBException & e )
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

void OCIQueryAdapter::fillDataBuffer( int numColumns, Buffer& data, long lMaxRows )
{
    ThreadGuard guardQuery( m_queryLock );

    data.clear();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    int nFieldNumber = m_pQuery->fieldCount();
    TA_ASSERT(numColumns <= nFieldNumber, "column number is bigger than the SQL returns");

    try
    {
        long lOffset = 0;
        while( !m_pQuery->isEOF() && ( lOffset < lMaxRows ))
        {
            Row dataRow;
            for ( int nColLoop = 0; nColLoop < numColumns; ++nColLoop )
            {
                ocl::CRString strFieldName = m_pQuery->field(nColLoop).name();
                std::string strValue = m_pQuery->field(nColLoop).getString();
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
                    "SQL field name:[%s]; value:[%s]", strFieldName.c_str(), strValue.c_str());
                dataRow.push_back(strValue);
            }
            data.push_back(dataRow);
            m_pQuery->next();
            ++lOffset;
        }
    }
    catch( ocl::OraException & e )
    {
        DBException db("OCI_ERROR", e.message().c_str(), e.errorCode());
        TA_THROW( db );
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
    }
}

bool OCIQueryAdapter::cleanQuery()
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
        if ( m_pQuery->isActive() )
        {
            m_pQuery->close();
            m_pQuery->unprepare();
        }
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        // Continue to delete query, it must be deleted.
    }

    try
    {
        delete m_pQuery;
        m_pQuery = NULL;
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

bool OCIQueryAdapter::isActive( unsigned long ulMaxTimeoutInSecs )
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
    return (!m_pQuery->isEOF());
}

void OCIQueryAdapter::updateLatestActionTime()
{
    // update latest action time in public member function
    m_tmLatestActionTime = time( NULL );
}

}