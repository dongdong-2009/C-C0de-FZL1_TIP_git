#include <time.h>

#include "core/otl/src/otlv4.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/database/src/DBException.h"

#include "core/database/src/ODBCOTLQueryAdapter.h"
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <string>     // std::string, std::to_string

namespace TA_Base_Core
{

// const time_t QUERY_TIME_OUT_IN_SECS = 7200; // two hours

ODBCOTLQueryAdapter::ODBCOTLQueryAdapter( otl_stream* pQuery ) : 
m_pQuery( pQuery ),
m_bConnectionClosed( false ),
m_tmLatestActionTime( 0 )
{
    TA_ASSERT( NULL != m_pQuery, "Can't initialize with NULL pointer of ocl::OraQuery" );
    updateLatestActionTime();
}

ODBCOTLQueryAdapter::~ODBCOTLQueryAdapter()
{
    cleanQuery();
}

bool ODBCOTLQueryAdapter::hasMore()
{
    ThreadGuard guardQuery( m_queryLock );

    updateLatestActionTime();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    return (!m_pQuery->eof());
}

void ODBCOTLQueryAdapter::fetchInitial( int numColumns, Buffer& data, long lMaxRows )
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
        if ( m_pQuery->good() )
        {
            fillDataBuffer(numColumns, data, lMaxRows);
        }
    }
	catch(otl_exception& e)
	{
		//error
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s",
			e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);

		//throw DBException
		DBException db("OTL_ERROR", (char*)(e.msg), e.code);
		throw db;
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

void ODBCOTLQueryAdapter::fetchMore( int numColumns, Buffer& data, long lMaxRows )
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
        if (!m_pQuery->good())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "no rows returns from SQL");
            return;
        }

        {
            //LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "{OCL}: current record no is %d; and get batch record count %d after this round.", m_pQuery->recordNo(), m_pQuery->recordCount());
            fillDataBuffer( numColumns, data, lMaxRows );
        }

    }
	catch(otl_exception& e)
	{
		//error
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s",
			e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);

		//throw DBException
		DBException db("OTL_ERROR", (char*)(e.msg), e.code);
		throw db;
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

void ODBCOTLQueryAdapter::fillDataBuffer( int numColumns, Buffer& data, long lMaxRows )
{
    ThreadGuard guardQuery( m_queryLock );

	int nFieldNumber = 0;
	std::string strFieldName;
	std::string strValue;

    data.clear();

    if ( NULL == m_pQuery )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Query is closed by timeout" );
        TA_THROW( TA_Base_Core::DatabaseException("Query is closed by timeout") );
    }

    nFieldNumber = _GetColumnCount(m_pQuery);//
    TA_ASSERT(numColumns <= nFieldNumber, "column number is bigger than the SQL returns");

	int nColumnIndex = 0;
	//otl_stream_read_iterator iterR;
	//iterR.attach(*m_pQuery);
	//iterR.get(nColumnIndex,);
    try
    {
        int nRowIndex = 0;
        while( !m_pQuery->eof() && ( nRowIndex < lMaxRows ))
        {
            Row dataRow;
            for ( int nColumnIndex = 0; nColumnIndex < numColumns; ++nColumnIndex )
            {
				strFieldName = _GetColumnName(m_pQuery,nColumnIndex);//m_pQuery->field(nColLoop).name();
                strValue = _GetColumnValue(m_pQuery, nColumnIndex);//m_pQuery->field(nColLoop).getString();
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, 
					"RowIndex:[%d],ColumnIndex:[%d], SQL field name:[%s]; value:[%s]", nRowIndex, nColumnIndex, strFieldName.c_str(), strValue.c_str());
                dataRow.push_back(strValue);
            }
            data.push_back(dataRow);
            ++nRowIndex;
		}
    }
	catch(otl_exception& e)
	{
		//error
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s",
			e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);

		//throw DBException
		DBException db("OTL_ERROR", (char*)(e.msg), e.code);
		throw db;
	}	
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");
    }
}

bool ODBCOTLQueryAdapter::cleanQuery()
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
		m_pQuery->close();
        delete m_pQuery;
        m_pQuery = NULL;
    }
	catch(otl_exception& e)
	{
		//error
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"OTL_ERROR, errmsg:%s, errcode=%d stm_text:%s, sqlstate:%s, var_info:%s",
			e.msg, e.code, e.sqlstate, e.stm_text, e.var_info);

		//throw DBException
		DBException db("OTL_ERROR", (char*)(e.msg), e.code);
		throw db;
	}
    catch(...)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "OTL_ERROR, UNKNOW_ERROR");
        return false;
    }

    FUNCTION_EXIT;
    return true;
}

bool ODBCOTLQueryAdapter::isActive( unsigned long ulMaxTimeoutInSecs )
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
    //return (!m_pQuery->isEOF());//fix bug: first delete connection then delete query crash 
	return true;
}

void ODBCOTLQueryAdapter::updateLatestActionTime()
{
    // update latest action time in public member function
    m_tmLatestActionTime = time( NULL );
}





//////////////////////////////////////////////////////////////////////////
int ODBCOTLQueryAdapter::_GetColumnCount(otl_stream* pQuery)
{
	otl_column_desc* p_otl_column_desc = NULL;
	int nColumnCount = 0;//nColumnCount;
	p_otl_column_desc = pQuery->describe_select(nColumnCount);
	if(!p_otl_column_desc)
	{
		throw otl_exception(otl_error_msg_21,otl_error_code_21);
	}
	return nColumnCount;

}

std::string ODBCOTLQueryAdapter::_GetColumnName(otl_stream* pQuery, int nColumnIndex)
{
	std::string strValue;
	otl_column_desc* p_otl_column_desc = NULL;
	int nColumnCount = 0;//nColumnCount;
	p_otl_column_desc = pQuery->describe_select(nColumnCount);
	if(!p_otl_column_desc)
	{
		throw otl_exception(otl_error_msg_21,otl_error_code_21);
	}

	const otl_column_desc& curr_var=p_otl_column_desc[nColumnIndex];
	strValue = curr_var.name;
	return strValue;
}

std::string ODBCOTLQueryAdapter::_GetColumnValue(otl_stream* pQuery, int nColumnIndex)
{
	std::string strValue;
	otl_column_desc* p_otl_column_desc = NULL;
	int nColumnCount = 0;//nColumnCount;
	p_otl_column_desc = pQuery->describe_select(nColumnCount);
	if(!p_otl_column_desc)
	{
		throw otl_exception(otl_error_msg_21,otl_error_code_21);
	}

	otl_long_string* p_otl_long_string = NULL;
	char* pszTemp = NULL;
	int nSZTempSize = 256;
	otl_datetime* p_otl_datetime = NULL;
	double varValue_double = 0;
	float varValue_float = 0;			
	int varValue_int = 0;
	unsigned int varValue_unsigned_int = 0;
	short int varValue_short_int = 0;
	long varValue_long = 0;
	std::ostringstream datatime_ss;//



	//for(int nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex)
	{
		const otl_column_desc& curr_var = p_otl_column_desc[nColumnIndex];
		switch(curr_var.otl_var_dbtype)
		{
		case otl_var_char:
			{
				pszTemp = NULL;
				pszTemp = new char[curr_var.dbsize + 1];
				memset(pszTemp, 0, (curr_var.dbsize + 1)*(sizeof(char)) );
				(*pQuery)>>pszTemp;
				strValue = pszTemp;
				delete[] pszTemp;
				pszTemp = NULL;
			}
			break;
		case otl_var_raw:
			{
				p_otl_long_string = new otl_long_string(curr_var.dbsize + 1);
				(*pQuery)>>(*p_otl_long_string);

				strValue.clear();
				strValue.append((char*)(p_otl_long_string->v), p_otl_long_string->len());

				delete p_otl_long_string;
				p_otl_long_string = NULL;
			}
			break;
		case otl_var_double:
			{
				(*pQuery)>>varValue_double;
				strValue = boost::lexical_cast<std::string>(varValue_double);
			}
			break;
		case otl_var_float:
			{
				(*pQuery)>>varValue_float;
				strValue = boost::lexical_cast<std::string>(varValue_float);
			}
			break;
		case otl_var_int:
			{
				(*pQuery)>>varValue_int;
				strValue = boost::lexical_cast<std::string>(varValue_int);
			}
			break;
		case otl_var_unsigned_int:
			{
				(*pQuery)>>varValue_unsigned_int;
				strValue = boost::lexical_cast<std::string>(varValue_unsigned_int);
			}
			break;
		case otl_var_short:
			{
				(*pQuery)>>varValue_short_int;
				strValue = boost::lexical_cast<std::string>(varValue_short_int);
			}
			break;
		case otl_var_long_int:
			{
				(*pQuery)>>varValue_long;
				strValue = boost::lexical_cast<std::string>(varValue_long);
			}
			break;
		case otl_var_timestamp:
		case otl_var_db2time:
		case otl_var_db2date:
		case otl_var_tz_timestamp:
		case otl_var_ltz_timestamp:
			p_otl_datetime = new otl_datetime();
			(*pQuery)>>(*p_otl_datetime);

			datatime_ss<<p_otl_datetime->year<<"-"<<p_otl_datetime->month<<"-"<<p_otl_datetime->day
				<<" "<<p_otl_datetime->hour<<":"<<p_otl_datetime->minute<<":"<<p_otl_datetime->second;//<<"."<<p_otl_datetime->fraction;

			strValue = datatime_ss.str();
			delete p_otl_datetime;
			p_otl_datetime = NULL;
			break;
		case otl_var_varchar_long:
		case otl_var_raw_long:
		case otl_var_clob:
		case otl_var_blob:
			p_otl_long_string = new otl_long_string(pQuery->get_adb_max_long_size());
			(*pQuery)>>(*p_otl_long_string);

			strValue.clear();
			strValue.append((char*)(p_otl_long_string->v), p_otl_long_string->len());
			//std::string std::string(p_otl_long_string->v, p_otl_long_string->v + p_otl_long_string->get_buf_size());

			delete p_otl_long_string;
			p_otl_long_string = NULL;
			break;
#if defined(OTL_BIGINT)
		case otl_var_bigint:
			//out_vars_arr_[nColumnIndex]=OTL_RCAST(unsigned char*,new OTL_BIGINT(0));
			break;
#endif
#if defined(OTL_UBIGINT)
		case otl_var_ubigint:
			//out_vars_arr_[nColumnIndex]=OTL_RCAST(unsigned char*,new OTL_UBIGINT(0));
			break;
#endif
#if defined(OTL_NUMERIC_TYPE_1) && defined(OTL_STR_TO_NUMERIC_TYPE_1) && defined(OTL_NUMERIC_TYPE_1_TO_STR) && defined(OTL_NUMERIC_TYPE_1_ID)
		case otl_var_numeric_type_1:
			//out_vars_arr_[nColumnIndex]=OTL_RCAST(unsigned char*,new OTL_NUMERIC_TYPE_1(0));
			break;
#endif
#if defined(OTL_NUMERIC_TYPE_2) && defined(OTL_STR_TO_NUMERIC_TYPE_2) && defined(OTL_NUMERIC_TYPE_2_TO_STR) && defined(OTL_NUMERIC_TYPE_2_ID)
		case otl_var_numeric_type_2:
			//out_vars_arr_[nColumnIndex]=OTL_RCAST(unsigned char*,new OTL_NUMERIC_TYPE_2(0));
			break;
#endif
#if defined(OTL_NUMERIC_TYPE_3) && defined(OTL_STR_TO_NUMERIC_TYPE_3) && defined(OTL_NUMERIC_TYPE_3_TO_STR) && defined(OTL_NUMERIC_TYPE_3_ID)
		case otl_var_numeric_type_3:
			//out_vars_arr_[nColumnIndex]=OTL_RCAST(unsigned char*,new OTL_NUMERIC_TYPE_3(0));
			break;
#endif
		}//switch
	}//for


	//otl_null();
	if(pQuery->is_null())
	{
		//strValue = "column have not data, NULL";
		strValue.clear();
		return strValue;
	}

	return strValue;
}//_GetColumnValue

}