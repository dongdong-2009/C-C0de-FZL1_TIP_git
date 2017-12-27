//////////////////////////////////////////////////////////////////////
///		@file		MysqlConnection.h
///		@author		zhilin,ouyang
///		@date		2010-11-25 15:44:41
///
///		@brief	    Implementation of the interface IDbConnection. 
///					use mysql database server to implement the   
///                 database connection operations.
//////////////////////////////////////////////////////////////////////
#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H
#include "IDbConnection.h"

NS_BEGIN(sql)
class Driver;
class Statement;
class Connection;
NS_END(sql)

NS_BEGIN(TA_Base_Core)

class IQueryAdapter;   // forward declare

class MysqlConnection : public IDbConnection
{
public:
	/**
	* MysqlConnection
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
	MysqlConnection(
		sql::Driver* pDriver,
		const std::string& connectionStr, 
		const std::string& databaseName, 
		const std::string& userName, 
		const std::string& password, 
		const std::string& hostName = "" 
		);

	/**
	* ~MysqlConnection
	* 
	* Sole standard destructor. Frees the handles allocated in the constructor.
	*/
	~MysqlConnection();

	// Indicates that there may be more data			
    bool isAnyQueryActive( unsigned long ulMaxTimeoutInSecs );

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
	void exec( const SQLStatement& rSqlObj, IQueryAdapter*& pIQueryAdapter, bool isQuery = false, int prefetch = 10);
		
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
	* @param    strSQL  the SQL format like something as below:
	                    INSERT INTO BLOBTEST(blobdata) VALUES (?)
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
//	bool isConnectionInUse();

	//static void cleanUpQuery(ocl::OraQuery*& Query);
    void cleanQuery( IQueryAdapter*& pQuery );

private:

	/**
	* createConnection
	* 
	* creates the underlying framework to support a session
	*/
	void createConnection();	
	void _GetSQL(std::string& strSql, const SQLStatement& rSqlObj);
	
	sql::Driver*     m_pDriver;
	sql::Connection* m_pConnection;
//	sql::Statement*  m_pStatment;
};

NS_END(TA_Base_Core)

#endif /// MYSQLCONNECTION_H
