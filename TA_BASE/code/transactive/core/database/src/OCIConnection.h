/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/OCIConnection.h $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* This class is used by OCIAccess to store database connection 
* information and maintain the associated connection handles.  
* If the database automatically reconnects a database with the same name, username and 
* password, this class will connect to the new database on the first exec
* failure.
* 
*/

#ifndef OCICONNECTION_H
#define OCICONNECTION_H

#ifdef WIN32
#pragma warning(disable: 4290) // C++ exception specification ignored
#endif // WIN32

#include "IDbConnection.h"

namespace ocl
{
	class OraConnection;
	class OraQuery;
	class OraTransaction;
}

namespace TA_Base_Core
{
    class IQueryAdapter;

	//TD18251, jianghp, to replace the OCI with OCL
	class OCIConnection : public IDbConnection
	{
	public:
			
		/**
		* OCIConnection
		* 
		* Sole constructor that stores the database connection details and 
		* allocates handles to perform transactions with the database.
		* Note: this method can be unsuccessful, so checkIsValid() should be 
		* called afterwards to check that construction was successful.
		* 
		* @param connectionStr databaseName:userName:password:hostName
		* @param databaseName  the name of the database to connect to
		* @param userName      the name of the user for the session
		* @param password      the name of the password for the session
		* @param hostName      the name of the host that the database is on
		*/
		OCIConnection( const std::string& connectionStr, 
			const std::string& databaseName, 
			const std::string& userName, 
			const std::string& password, 
			const std::string& hostName = "" );
		
		
			/**
			* ~OCIConnection
			* 
			* Sole standard destructor. Frees the handles allocated in the constructor.
		*/
		~OCIConnection();

		// Indicates that there may be more data
		//bool hasMore() const;		
        bool isAnyQueryActive( unsigned long ulMaxTimeoutInSecs );

		//bool isActive() const;

		/**
		* getConnectionString
		*
		* Returns the connection string for this OCIConnection.
		*
		* @return std::string  the connection string in the form
		*                      databaseName:userName:password:hostName
		*/
		std::string getConnectionString() const;
		
		/**
		* open
		*
		* Starts a new session with the database if there is not one already 
		* running.
		*
		* @exception DBException Thrown if there is an OCI error.
		*/
		void open();
		
		bool isOpen();
		/**
		* close
		*
		* Ends the session with the database if there is one running.
		*
		* @exception DBException Thrown if there is an OCI error.
		*/
		void close();
		
		
		/**
		* closeAll
		*
		* Ends all sessions with the database if there are any running.
		*
		* @exception DBException Thrown if there is an OCI error.
		*/
		void closeAll();
		
		
		/**
		* exec
		*
		* Executes the given statement on the database.  This method
		* handles both queries (e.g. "SELECT...") and non-queries (e.g."INSERT...")
		*
		* @param statement    the statement to execute
		* @param isQuery      a bool that is true if the statement is a query
		* @param numColumns   the number of columns to retrieve
		* @param bindType     the type of binding required
		* @exception DBException Thrown if there is an OCI error.
        */
        //void exec( const std::string& statement, ocl::OraQuery *& query, bool isQuery = false, int prefetch = 10);
		void exec( const SQLStatement& rSqlObj, IQueryAdapter*& pQueryAdapter, bool isQuery = false, int prefetch = 10);
		
		/**
		* fetchBLOB
		*
		* Fetches the BLOB data from the field/row specified by the previous
		* exec(). 
		*
		* @param	data	the BLOB data will be written to this parameter
		* @exception DBException Thrown if there is an OCI error.
		*
		*/
		void fetchBLOB(const std::string &strSQL, const std::string &colomnName, DataClobArray& data );
		
		
		/**
		* updateBLOB
		*
		* Updates the BLOB data from the field/row specified by the previous
		* exec(), with the contents of the vector. 
		*
		* @param	data	the BLOB data will be written from this parameter
		* @exception DBException Thrown if there is an OCI error.
		*
		*/
		void updateBLOB(
			const std::string &strTableName, 
			const std::string &colomnName,
			const std::vector<unsigned char>& data, 
			const std::string &strWhereClause
			);

		/**
		* incrementExecutionCount
		*
		* Increment the count of threads that are using this connection right now.
		* Note: The count does not include threads that have this connection open, 
		* but are not executing on it right now.
		*/
		void incrementExecutionCount();
		
        
		/**
		* decrementExecutionCount
		*
		* Decrement the count of threads that are using this connection right now.
		* Note: The count does not include threads that have this connection open,
		* but are not executing on it right now.
		*/
		void decrementExecutionCount();
		
		/**
		* getExecutionCount
		*
		* Get the count of threads that are using this connection right now.
		* Note: The count does not include threads that have this connection open,
		* but are not executing on it right now.
		*/
		int getExecutionCount() { return m_executionCount; };
		
		/**
		* isConnectionInUse
		*
		* Checks if the connection is still being used.  This involves checking
		* if the execution count is non-zero or if there are any statements 
		* that have more data to be fetched.  Note: this only returns true
		* if there is a thread that is partway through a transaction on this
		* connection - not just if they have an open link to this connection.
		*
		* @return bool     True if the connection is being used at the moment.
		*                  False otherwise.
		*/
//		bool isConnectionInUse();

		//static void cleanUpQuery(ocl::OraQuery*& Query);
        void cleanQuery( IQueryAdapter*& pQuery );
		
protected:
	
	/**
	* createConnection
	* 
	* creates the underlying framework to support a session
	*/
    void createConnection(void);
	void _GetSQL(std::string& strSql, const SQLStatement& rSqlObj);
	
	//TD18251, jianghp, to replace the OCI with OCL
	ocl::OraConnection *m_connection;
		
};

}
#endif // OCICONNECTION_H



