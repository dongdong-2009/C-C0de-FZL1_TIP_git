
#include "OracleQueryAdapter.h"

#include <time.h>

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/DBException.h"

#include "DbCommonData.h"


#include "ocilib.h"

NS_BEGIN(TA_AQ_Lib)

COracleQueryAdapter::COracleQueryAdapter( OCI_Statement* pStatement )
{
	TA_ASSERT( NULL != pStatement, "Can't initialize with NULL pointer of ocl::OraQuery" );

	m_pStatement = pStatement;

	m_pResultset = OCI_GetResultset(m_pStatement);

	m_bConnectionClosed = false;
	m_tmLatestActionTime = 0;

	_UpdateLatestActionTime();
}

COracleQueryAdapter::~COracleQueryAdapter()
{
	cleanQuery();
}

bool COracleQueryAdapter::hasMore()
{
	bool bHasMore = true; 

	_UpdateLatestActionTime();	
	
	return bHasMore;
	
}

void COracleQueryAdapter::fetchInitial( int numColumns, Buffer& data, long lMaxRows )
{
	TA_Base_Core::ThreadGuard guardQuery(m_queryLock);


	_UpdateLatestActionTime();
	data.clear();

	if ( NULL == m_pResultset )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
		TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
	}

	try
	{

		_FillDataBuffer(numColumns, data, lMaxRows);
	
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
	}
}

void COracleQueryAdapter::fetchMore( int numColumns, Buffer& data, long lMaxRows )
{
	TA_ASSERT( numColumns <= FETCH_MAX_COLS                , "Number of columns is out of range");
	TA_ASSERT( (lMaxRows > 0) && (lMaxRows <= FETCH_MAX_ROWS), "Number of rows is out of range"   );

	TA_Base_Core::ThreadGuard guardQuery(m_queryLock);

	_UpdateLatestActionTime();
	data.clear();

	if ( NULL == m_pResultset )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
		TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
	}

	try
	{
		_FillDataBuffer( numColumns, data, lMaxRows );

	} 
	catch( ... )
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
	}
}

void COracleQueryAdapter::_FillDataBuffer( int numColumns, Buffer& data, long lMaxRows )
{
	int nFetchNext = 0;
	int nRowNumber = 0;
	int nFieldNumber = 0;
	const char* pGetStr = NULL;
	TA_Base_Core::ThreadGuard guardQuery(m_queryLock);

	OCI_Column *col = NULL;
	std::string strColumnName;
	std::string strValue;
	unsigned int nColumnType;
	OCI_Date*  date = NULL;
	OCI_Timestamp* timeStampVauleGet = NULL; 

	data.clear();

	if ( NULL == m_pResultset )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
		TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
	}
	
	/* get resultset row count */
	//OCI_FetchLast(m_pResultset);
	//nRowNumber = OCI_GetRowCount(m_pResultset);//hang
	/* go to row 1 */
	//OCI_FetchFirst(m_pResultset);

	nFieldNumber = OCI_GetColumnCount(m_pResultset); //lie
	
	//TA_ASSERT(numColumns <= nFieldNumber, "column number is bigger than the SQL returns");
	
	if (numColumns > nFieldNumber)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"column number is bigger than the SQL returns, [numColumns=%d] [nFieldNumber=%d]",
			numColumns, nFieldNumber);
		return;
	}
	try
	{
		long lOffset = 0;
		while( (nFetchNext = OCI_FetchNext(m_pResultset)) && ( lOffset < lMaxRows ))
		{
			nRowNumber = OCI_GetRowCount(m_pResultset);//hang

			Row dataRow;
			for ( int nColLoop = 0; nColLoop < numColumns; ++nColLoop )
			{	
				strValue.clear();
				col = OCI_GetColumn(m_pResultset, nColLoop+1);
				pGetStr = NULL;
			    pGetStr = OCI_GetColumnName(col);
				if (NULL != pGetStr)
				{
					strColumnName = pGetStr;
				}
				
				nColumnType = OCI_GetColumnType(col);
				switch (nColumnType)
				{
				case OCI_CDT_TEXT:
					{
						//dtext *
						pGetStr = NULL;
						pGetStr = OCI_GetString(m_pResultset, nColLoop+1);
						if (NULL != pGetStr)
						{
							strValue = pGetStr;
						}
						break;
					}
				case OCI_CDT_NUMERIC:
					{
						//: short, int, long long, double
						pGetStr = NULL;
						pGetStr = OCI_GetString(m_pResultset, nColLoop+1);
						if (NULL != pGetStr)
						{
							strValue = pGetStr;
						}
						break;
					}
				case OCI_CDT_DATETIME:							 
					{
						//OCI_Date *
						date = NULL;
						date = OCI_GetDate(m_pResultset, nColLoop+1);
						if (NULL != date)
						{
							char* pValueTmp = new char[256];
							memset(pValueTmp, 0, 256);
							OCI_DateToText(date, MT("YYYY-MM-DD HH24:MI:SS"), 256, pValueTmp);
							strValue = pValueTmp;
							delete[] pValueTmp;
							pValueTmp = NULL;
						}

						break;
					}
				case OCI_CDT_TIMESTAMP:
					{
						//OCI_Date *
						timeStampVauleGet = NULL;
						timeStampVauleGet = OCI_GetTimestamp(m_pResultset, nColLoop+1);
						if (NULL != timeStampVauleGet)
						{
							char* pValueTmp = new char[256];
							memset(pValueTmp, 0, 256);						
							OCI_TimestampToText(timeStampVauleGet, MT("YYYYMMDDHH24MMSSFF3"), 256, pValueTmp, 3);
							strValue = pValueTmp;
							delete[] pValueTmp;
							pValueTmp = NULL;
						}

						break;
					}	
				default:
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
						"Warn! OCILIB Select not support now! [strColumnName=%s][nColumnType=%d]",
						 strColumnName.c_str(), nColumnType);				

					break;
				}//switch
				
				  

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
					"SQL field name:[%s]; value:[%s]", strColumnName.c_str(), strValue.c_str());				
				dataRow.push_back(strValue);
			}
			data.push_back(dataRow);
			++lOffset;
		}
	}
	catch( ... )
	{
	}
	
	
	pGetStr = NULL;
}

bool COracleQueryAdapter::cleanQuery()
{
	bool   bCleanQueryRes = true;
	int    nRes = 0;
	
	FUNCTION_ENTRY("cleanQuery");

	TA_Base_Core::ThreadGuard guardQuery(m_queryLock);

	if ( NULL == m_pResultset ||  NULL == m_pStatement )
	{
		FUNCTION_EXIT;
		return false;
	}

	m_pResultset = NULL;
	
	bCleanQueryRes = true;
	nRes = OCI_StatementFree(m_pStatement);
	if ( 1 > nRes )
	{
		bCleanQueryRes = false;
	}
	if (false == bCleanQueryRes)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"error! OCI_StatementFree()"); 
	}
	
	m_pStatement = NULL;

	FUNCTION_EXIT;
	return bCleanQueryRes;
}

bool COracleQueryAdapter::isActive()
{
	FUNCTION_ENTRY("isActive");

	

	FUNCTION_EXIT;
	return true;
}

void COracleQueryAdapter::_UpdateLatestActionTime()
{
	// update latest action time in public member function
	m_tmLatestActionTime = time( NULL );
}

//}

NS_END(TA_AQ_Lib)




