//////////////////////////////////////////////////////////////////////
///		@file		DbCommonData.h
///		@author		zhilin,ouyang
///		@date		2012-01-12 17:44:41
///
///		@brief	    to define some common use data or structure which 
///                 used in the database wrapper.
//////////////////////////////////////////////////////////////////////
#ifndef  DBCOMMONDATA_H
#define  DBCOMMONDATA_H
#include "AQCommonDef.h"
#include <vector>
#include <string>

NS_BEGIN(TA_AQ_Lib)

// Data will be retrieved in Rows into a Buffer.
typedef std::vector<std::string> Row;
typedef std::vector<Row> Buffer;	

typedef std::vector<u_char> DataClob;
typedef std::vector<DataClob> DataClobArray;

const long QUERY_EXIST = 1;
const u_long FETCH_MAX_ROWS = 1000;  // think if this is enough
const u_long FETCH_MAX_COLS = 100;
const long INVALID_USERNAME_PASSWORD = 1017;
const size_t MIN_DBCONNECTION_STR_SIZE = 4;
const u_int  MAX_RETRY_TIMES = 3;
const u_int  DB_CONNECT_TIMEOUT = 100;
const u_int  DB_SWITCH_TIMEOUT  = 5 * 1000;

enum Months_Code
{
	JANUARY         = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

NS_END(TA_AQ_Lib)


#endif // DBCOMMONDATA_H



























