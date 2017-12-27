/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IDatabase.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IDatabase is an interface to the object that maintains a database connection. The object
  * represented by this interface will be specific to a database schema, and will execute
  * sql queries and procedures.
  */

#if !defined(IDatabase_4566FC34_4671_4140_B430_365285D733FB__INCLUDED_)
#define IDatabase_4566FC34_4671_4140_B430_365285D733FB__INCLUDED_

#include <vector>
#include <string>

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/sqldef.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/database/src/CommonType.h"

namespace TA_Base_Core
{
	class SQLTypeAdapter;

class IDatabase
{

    public:
        /**
		 * connect
		 *
		 * This method creates a connection to the database.
		 *
		 * @exception DatabaseException This exception will be thrown under the following circumstances:
         *            1) The connection string in RunParams (normally recieved from the 
         *               Process Manager) is invalid
         *            2) The database does not successfully open
         *
		 */
        virtual void connect( const std::string& connectionString ) =0;

        /**
		 * disconnect
		 * 
		 * This method closes the connection to the database.
         *
		 */
        virtual void disconnect() =0;

        /**
		 * cleanQuery
		 * 
		 * This method closes the current query and close the opening cursor.
         *
		 */
		virtual void cleanQuery() = 0;

        /**
		 * executeQuery
		 *
		 * This method carries out the specified query and stores the retrieved data in an 
         * IData object that is returned to the client. The first method takes a row
		 * count which must be <= getMaxRowCount() the second method simply calls the first
		 * with numRows set to getMaxRowCount().
		 *
		 * @param sql The sql query to execute. This query must not be null.
		 * @param columnNames This is the vector of column names that will be used to store 
         * the retrieved data. This vector must not be empty.
		 * @param numRows This is an optional parameter. It is the number of rows to store in 
         * the IData object. By default this is set to 500. 
		 *
		 * @return The IData object pointer returned contains the data retrieved from the database.
		 * NB: It is the responsibility of the *client* to delete the IData object.
		 *
         * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
         *
		 * pre: sql.size() > 0; 
         *      connect() MUST be called prior to this method   
		 */

		virtual IData* executeQuery(const SQLStatement& rSqlObj, const std::vector<std::string>& columnNames, 
			const unsigned long numRows) =0;
		virtual IData* executeQuery(const SQLStatement& rSqlObj, const std::vector<std::string>& columnNames) =0;

        /**
		 * executeModification
		 *
		 * This method carries out the specified query. Unlike executeQuery(), this method will not
		 * store any returned data, and as such should be used for SQL INSERT, DELETE, or UPDATE 
		 * statements
		 *
		 * @param sql The sql query to execute. This query must not be null.
		 *
		 * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
		 * pre: sql.size() > 0; 
         *      connect() MUST be called prior to this method
		 */
		virtual void executeModification(const SQLStatement& rSqlObj) =0;

        /**
         * executeProcedure
         *
         * Execute the specified procedure and stores the resultant data in an IData 
         * compatible object that is returned to the client.
         * 
         * @param sql The sql query to execute. This query must not be null.
		 * @param columnNames This is the vector of column names that will be used to store 
         * the retrieved data. This vector must not be empty.
		 * @param numRows This is an optional parameter. It is the number of rows to store in 
         * the IData object. By default this is set to 500. 
         *
		 * @return The IData object pointer returned contains the data retrieved from the database.
		 * NB: It is the responsibility of the *client* to delete the IData object.
		 *
         * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the procedure fails.
         *
         *
		 * pre: sql.size() > 0; 
         *      connect() MUST be called prior to this method   
         */
		virtual IData* executeProcedure(const SQLStatement& rSqlObj, const std::vector<std::string>& columnNames, 
			const unsigned long numRows = 500) =0;

        /**
         * executeProcedure
         * 
         * This method executes the speicifed procedure, but ignores any return data. 
         * It is for use with procedures that either return no data, or for which the
         * return data is not important.
         *
         * @param sql The sql query to execute. This query must not be null.
		 *
		 * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
		 * pre: sql.size() > 0; 
         *      connect() MUST be called prior to this method
         */
		virtual void executeProcedure(const SQLStatement& rSqlObj) =0;


        /**
		 * moreData
		 *
		 * This method determines if there is any more data to be retrieved for the current
		 * query, and creates an IData object if necessary.
		 *
		 * @param returnData This is a pointer to an IData object. It MUST be a null pointer, 
		 * as this method will overwrite it (if not, a memory leak will occur).
		 *
		 * @returns true iff there was data to return, and the IData pointer has been instantiated.
		 * If it returns false, then the IData pointer passed in will not be changed.
		 * If it returns true, then the IData pointer passed in points to a new IData object
         * containing the next ammount of data to be loaded.
		 *
         * @exception DatabaseException This exception will be thrown under the following circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
		 * pre: returnData == NULL
         *      connect() MUST be called prior to this method
		 */
        virtual bool moreData(IData*& returnData) =0;


		/**
		 * readBLOB
		 *
		 * This method reads a single row from a single BLOB (Binary Large Object) field into
		 * the vector parameter.
		 *
		 * @param fieldName		the name of the BLOB field to read from
		 * @param tableName		the name of the table that the field is a part of
		 * @param whereClause	SQL clause to specify the exact row to read from
		 * @param data			the BLOB data will be read into this vector
		 *
         * @exception DatabaseException This exception will be thrown under the following circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
		 */
		virtual void readBLOB( const std::string& fieldName, const std::string& tableName,
							   const std::string& whereClause, std::vector<unsigned char>& data ) = 0;


		/**
		 * writeBLOB
		 *
		 * This method updates a single BLOB (Binary Large Object) field in a single row from
		 * the vector parameter.
		 *
		 * @param fieldName		the name of the BLOB field to write to
		 * @param tableName		the name of the table that the field is a part of
		 * @param whereClause	SQL clause to specify the exact row to write to
		 * @param data			the BLOB data will be read from this vector
		 *
         * @exception DatabaseException This exception will be thrown under the following circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
		 */
		virtual void writeBLOB( const std::string& fieldName, const std::string& tableName,
							    const std::string& whereClause, std::vector<unsigned char>& data ) = 0;


        /**
         * beginTransaction
         *
         * This method sets the marker for the start of a transaction. All queries, procedures
         * and modifications from this point until either commitTransaction() or 
         * rollbackTransaction() are called form part of the new transaction.
         * If beginTransaction() is not called prior to running a procedure or modification
         * the call will be committed immediately upon success (this is the default behaviour).
         *
         * @exception DatabaseException Thrown if the transaction cannot be started
         *
         * pre: A transaction cannot already be in progress
         */
        virtual void beginTransaction() =0;

        /**
         * commitTransaction
         *
         * This method commits all changes made since the last call to beginTransaction(). 
         * Until this method is called, none of the changes will be visible to other
         * applications.
         * If the application is terminated or an error occurs (e.g. server disconnection)
         * prior to commitTransaction() being called, a default rollback will be performed.
         *
         * @exception DatabaseException Thrown if the commit cannot be performed, or does not
         *            complete sucessfully.
         *
         * pre: A transaction must currently be in progress
         */
        virtual void commitTransaction() =0;

        /**
         * rollbackTransaction
         *
         * This method "undoes" all changes made since the last call to beginTransaction().
         * Once this method is called, all changes will be dropped from the database, and it
         * will revert to the state as when beginTransaction() was called.
         * If the application is terminated or an error occurs (e.g. server disconnection)
         * prior to rollbackTransaction() being called, a rollback will still be automatically
         * performed by the database.
         *
         * @exception DatabaseException Thrown if the rollback cannot be performed, or does not
         *            complete sucessfully.
         *
         * pre: A transaction must currently be in progress
         */
        virtual void rollbackTransaction() =0;

        /**
         * escapeInsertString
         *
         * Parses a string for insertion into the database, and returns it with any special 
         * characters escaped.
         * Note: 1) This method assumes that the string passed in has NOT had any escaping
         * already applied. If it does, the items will be escaped a second time.
         *       2) This method should be static, but it is not possible to define a static
         * method on an interface.
         *
         * @param theString The string to parse
         *
         * @return The input string with escape characters added where and if necessary.
         */
        virtual std::string escapeInsertString(const std::string& theString) =0;

        /**
         * escapeQueryString
         *
         * Parses a string for use in a SELECT query, and returns it with any special 
         * characters escaped.
         * Note: 1) This method assumes that the string passed in has NOT had any escaping
         * already applied. If it does, the items will be escaped a second time.
         *       2) This method should be static, but it is not possible to define a static
         * method on an interface.
         *
         * @param theString The string to parse
         *
         * @return The string with escape characters added where and if necessary.
         */
        virtual std::string escapeQueryString(const std::string& theString) =0;

		//Mintao++ Distributed DB
		/**
		  * escapeAQSQLString
		  * 
		  * Parses a string for use in a special AQ and returns it with any special 
		  * characters escaped. (use for Distributed DB)
		  * @return std::string 
		  * @param : const std::string& theString
		  * 
		  * @exception <exceptions> Optional
		  */
		virtual std::string escapeAQSQLString(const std::string& theString) = 0;

        /**
	      * getNextPkey
	      * 
	      * get next pkey for insert sql script 
	      * @return int 
	      * @param : const std::string& str, it is sequence name
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual std::string getNextPkey(const std::string& strSeqName) = 0;
		
		/**
		 * GetDbServerType
		 * 
		 * This method get the database type that currently in used.
         *
		 */
		virtual int getDbServerType() = 0;

		/**
		 * setDataTypeAction
		 * 
		 * This method set the datatype and action for the current thread, 
		 * each thread may has different datatype and action for each access.
         *
		 */
		virtual void setDataTypeAction(EDataTypes dataType, EDataActions dataAction) = 0;

		// Declare the all the overloads of prepareSQLStatement()
		PURE_SQL_FUNCS(void, prepareSQLStatement);

        virtual ~IDatabase() {};
    };
} //close namespace TA_Base_Core

#endif // !defined(IDatabase_4566FC34_4671_4140_B430_365285D733FB__INCLUDED_)
