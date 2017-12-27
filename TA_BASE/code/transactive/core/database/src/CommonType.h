

#ifndef _COMMON_TYPE_H
#define _COMMON_TYPE_H

#include <string>
#include <vector>
#include <map>

namespace TA_Base_Core
{
    // Data will be retrieved in Rows into a Buffer.
    typedef std::vector<std::string> Row;
    typedef std::vector<Row> Buffer;
	
	typedef std::vector<unsigned char> DataClob;
	typedef std::vector<DataClob> DataClobArray;

	//const unsigned long MAX_FIELDSIZE  = 4000;
	const unsigned long FETCH_MAX_ROWS = 110;//maybe 200 is too large for first startup, change it to 100
	const unsigned long FETCH_MAX_COLS = 100; 
	const unsigned int  CONN_STR_PARAM_COUNT = 7;     // the number of Db connection string parameters
	const unsigned int  CONN_STR_PARAM_MIN_COUNT = 5; // the least number of the Db connection string parameters
	const unsigned long DEFMAXLOGLEN = 2000;       // the max number of characters for log module

	enum DB_ERR_CODE
	{
		ER_MYSQL_CANNT_CONNECT		 = 2003, // Can't connect to MySQL server 
		ER_MYSQL_GONE_AWAY			 = 2006, // MySQL server has gone away
		ER_MYSQL_READ_ONLY           = 1290, // MySQL server running in read only mode

		ER_ORA_OCI_SESSION_KILLED    = 28,
		ER_ORA_OCI_NOTLOGGEDON       = 1012,
		ER_ORA_IMMIDATE_SHUTDOWN     = 1089, // immediate shutdown in progress - no operations are permitted
		ER_ORA_INITORSHUTDOWN        = 1033, // ORACLE initialization or shutdown in progress
		ER_ORA_NOT_AVALIABLE         = 1034, // ORACLE not available
		ER_ORA_NOT_CONNECTED         = 3114,
		ER_ORA_OCI_NO_INTERFACE      = 3121,
		ER_ORA_TNS_CLOSE             = 12537, // TNS:connection closed
		ER_ORA_TNS_PACKET_WRITER_FAILURE    = 12571, /*TNS:packet writer failure*/
	};

	typedef struct st_sql_tag
	{
		int nSQLID;                                // the SQL ID map in the sqlcode.txt
		std::string strSQLID;                      // the SQL ID map in the sqlcode.txt
		std::string strCommon;                     // the common SQL which can run at all supported db
		std::string strOracle;                     // the oracle SQL statement
		std::string strMySQL;                      // the mysql SQL statement

        //#ifdef TDS_DEVELOP
		bool bIsTdsAQ;                             // for TDS AQ Statement check flag
		std::string strTDSOracle;                  // for TDS AQ Oracle statement
		std::string strTDSMySQL;                   // for TDS AQ MySQL statement
        //#endif
	}SQLStatement, *PSQLStatement;

	enum DB_CON_STR_ELEMENTS
	{
		CON_STR_DBType              = 0,
		CON_STR_SchemaName,
		CON_STR_UserName,
		CON_STR_Passwd,
		CON_STR_Hostname,
        CON_SIZE
	};
}

#define  NS_BEGIN(name) namespace name {
#define  NS_END(name)   };

#endif