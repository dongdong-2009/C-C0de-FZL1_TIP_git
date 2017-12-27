/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/SimpleDb.h $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* <description>
*
*/
//
// SimpleDb.h
//
// Wrapper for the OCIAccess class which (hopefully) takes all the pain
// out of database access. Note that it is probably not very efficient.
//
// Author: Bruce Fountain 24-Jan-2001
//

#ifndef SIMPLEDB_H_INCLUDED
#define SIMPLEDB_H_INCLUDED

#if defined( WIN32 )
#pragma warning ( disable : 4786 )
#endif // defined( WIN32 )

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
#include <string>
#include "CommonType.h"

namespace TA_Base_Core
{
    class IQueryAdapter;

    class SimpleDb
	{
	public:
		const static unsigned long MAX_ROWS;
		const static unsigned long MAX_COLS;
		// You must specify the connection on construction, because this is
		// required by OCIAccess. The connection string is the name assigned
		// to your connection via Net8EasyConfig (eg "TCMS")
		//
		// NOTE: the hostname parameter is optional on Windows, but required
		//       on Solaris
		//
		// Bruce Fountain (21-Mar-2001): actually, I don't think the hostname
		//      is required at all if the listener is configured properly on
		//      solaris
		SimpleDb(const char* pszDbType, const char* connection, const char* user, const char* passwd,
			const char* hostname = "");
		virtual ~SimpleDb();
		
		// A useful method to parse a colon-separated database connection
		// string (eg "TCMS:COM:COM") and return a vector of the component
		// strings
		static void ParseConnectStr(const std::string& str,					// IN
			std::vector<std::string>& components,   // OUT
			char delimiter = ':');                  // IN
		
		// You MUST open the connection before calling Exec(), otherwise you
		// will get an assertion. You can close the connnection yourself or
		// let the destructor do this
		virtual void Open();
		void Close();
		virtual void CloseAll();
		void cleanQuery();
		// All data is represented as strings. You will have to do your own
		// conversions. I said it was simple...

		// A simple statement that returns no data (insert, update, delete...)
		void Exec(const SQLStatement& rSqlObj);
		
		// Exec()
		// Execute a statement which returns data. Succeeds or fails, no further
		// error handling
		// The argument (int maxrows = FETCH_MAX_ROWS) sets the maximum number of rows that 
		// can be returned in the argument Buffer& data. 
		// Note that maxrows must be greater than 0 and less than FETCH_MAX_ROWS+1
		// If Exec is called twice without fetching and storing the results from
		// the first Exec, the data will be overwritten.
		void Exec(const SQLStatement& rSqlObj, int numColumns, Buffer& data, int maxrows = FETCH_MAX_ROWS);
		
		
		// FetchMore()
		//
		// This method can only be called after an Exec() statement which
		// returns data. This method succeeds or fails, no further error handling.
		// The argument (int maxrows = FETCH_MAX_ROWS) sets the maximum number of rows that can 
		//be returned in the argument Buffer& data. 
		//Note that maxrows must be greater than 0 and less than FETCH_MAX_ROWS+1
		void FetchMore(int numColumns, Buffer& data, int maxrows = FETCH_MAX_ROWS);
		
		// Indicates that there may be more data
		bool HasMore() const;
		
		// Do we have an open connection?
		bool IsOpen();
		
		// Get the DB Server type that in used
		int  GetDbServerType();

		// check if the existing connection string is match the current one
		// if match, no need change, otherwise change the connection to the new one
		// during failover
		bool IsNeedChange(const std::string& strCon);
		
		
		/**
		* readBLOB
		*
		* This method reads a single row from a single BLOB (Binary Large Object) field into
		* the vector parameter.
		*
		* @param fieldName     the name of the BLOB field to read from
		* @param tableName     the name of the table that the field is a part of
		* @param whereClause   SQL clause to specify the exact row to read from
		* @param data          the BLOB data will be read into this vector
		* 
		* @return true/false indicating success or failure of the operation
		*
		*/
		void readBLOB( const std::string& fieldName, const std::string& tableName,
			const std::string& whereClause, std::vector<unsigned char>& data );
		
		
			/**
			* writeBLOB
			*
			* This method updates a single BLOB (Binary Large Object) field in a single row from
			* the vector parameter.
			*
			* @param fieldName     the name of the BLOB field to write to
			* @param tableName     the name of the table that the field is a part of
			* @param whereClause   SQL clause to specify the exact row to write to
			* @param data          the BLOB data will be read from this vector
			*
			* @return true/false indicating success or failure of the operation
			*
		*/
		void writeBLOB( const std::string& fieldName, const std::string& tableName,
			const std::string& whereClause, std::vector<unsigned char>& data );
		
			/**
			* getMaxColumns
			*
			* Return the maximum columns that are allowed in a select statement.
			*
			* @return                  maximum allowed columns
		*/
		int getMaxColumns() const
		{
			return FETCH_MAX_COLS;
		}
protected:
	//void fillDataBuffer(int numColumns, ocl::OraQuery *query, Buffer& data, long nMaxRows);
	void parseDBType(const std::string& strDBName);
		
protected:
	
    // Do we have an open connection?
	//ocl::OraQuery *m_currentQuery;
    IQueryAdapter* m_pCurQuery;
	
    std::string m_connectionStr;
	int  m_nDBType;
};


}
#endif // SIMPLEDB_H_INCLUDED


