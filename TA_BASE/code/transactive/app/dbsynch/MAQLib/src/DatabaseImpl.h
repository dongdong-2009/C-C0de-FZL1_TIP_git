//////////////////////////////////////////////////////////////////////
///		@file		IDBConnector.h
///		@author		zhilin,ouyang
///		@date		2012-01-12 14:44:41
///
///		@brief	    the interface to access various database servers 
///                 such as MySQL and Oracle etc. for the database 
///                 synchronization module.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef DATABASEIMPL_H
#define DATABASEIMPL_H

#include "IDatabase.h"
#include "SQLCode.h"

NS_BEGIN(TA_AQ_Lib)

class IDbConnection;
class IQueryAdapter;
class CMessage;

class CDatabaseImpl : public IDatabase
{
private:
	typedef  std::vector<std::string>                         StrVecT;
	typedef  std::vector<std::string>::const_iterator         StrVeccIterT;


public:    
	CDatabaseImpl(void);
	CDatabaseImpl(const std::string& strConnection);
	virtual ~CDatabaseImpl();

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CDatabaseImpl);

public:

	virtual int  setDbConStr(const std::string& strConStr);

	/**
	 * connect
	 *
	 * This method creates a connection to the database using the underlying IDbConnection interface.
	 *
	 * @the connection string store in the system variables.
     *
	 */
	virtual void connect();	
	virtual void disconnect();
	virtual void cleanQuery();

	/**
	 * executeQuery
	 *
	 * This method carries out the specified query and stores the retrieved data in an IData
	 * object that is returned to the client.
	 *
	 * @param sql The sql query to execute. This query must not be null.
	 * @param columnNames This is the vector of column names that will be used to store the 
	 * retrieved data. This vector must not be empty.
	 * @param numRows This is an optional parameter. It is the number of rows to store in the 
	 * IData object. By default this is set to SimpleDb::MAX_ROWS. For SimpleDb, this number must be between
	 * 1 and SimpleDb::MAX_ROWS
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
     *      0 < numRows <= SimpleDb::MAX_ROWS (the upper limit is a restriction on SimpleDb      
	 */
	virtual IDbData* executeQuery(const std::string& sql, const std::vector<std::string>& columnNames, 
								const u_long numRows );

	virtual IDbData* executeQuery(const std::string& sql, const std::vector<std::string>& columnNames);

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
	virtual void executeModification(const std::string& sql);
   
    /**
     * executeProcedure
     * 
     * This method executes the speicifed procedure using the executeModification() method.
     * This procedure CANNOT take an INOUT or OUT parameter, or the query will fail.
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
	virtual void executeProcedure(const std::string& sql);


	/**
	 * moreData
	 *
	 * This method determines if there is any more data to be retrieved for the current
	 * query, and creates an IData object if necessary.
	 *
	 * @param returnData This is a pointer to an IData object. It MUST be a null pointer, 
	 * as this method will overwrite it (if not, a memory leak will occur). This is confirmed
	 * by the use of a TA_ASSERT.
	 *
	 * @returns true iff there was data to return, and the IData pointer has been instantiated.
	 * If it returns false, then the IData object passed in will not be changed.
	 * If it returns true, then the IData object passed in contains the next lot of data.
	 *
     * @exception DatabaseException This exception will be thrown under the followin circumstances:
     *            1) The database has inadvertantly been closed, and cannot be reopend.
     *            2) The exectuion of the sql query fails.
     *
	 * pre: returnData == NULL
     *      connect() MUST be called prior to this method
	 */
	virtual bool moreData(IDbData*& returnData);

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
     * @exception DatabaseException This exception will be thrown under the following circumstances:
     *            1) The database has inadvertantly been closed, and cannot be reopend.
     *            2) The exectuion of the sql query fails.
     *
     */
    virtual void readBLOB( const std::string& fieldName, const std::string& tableName,
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
     * @exception DatabaseException This exception will be thrown under the following circumstances:
     *            1) The database has inadvertantly been closed, and cannot be reopend.
     *            2) The exectuion of the sql query fails.
     *
     */
    virtual void writeBLOB( const std::string& fieldName, const std::string& tableName,
                            const std::string& whereClause, std::vector<unsigned char>& data );


    /**
     * beginTransaction
     *
     * Implemented to support IDatabase, but SimpleDb does not support transactions, so
     * this will simply assert false.
     */
    virtual void beginTransaction();

    /**
     * commitTransaction
     *
     * Implemented to support IDatabase, but SimpleDb does not support transactions, so
     * this will simply assert false.
     */
    virtual void commitTransaction();

    /**
     * rollbackTransaction
     *
     * Implemented to support IDatabase, but SimpleDb does not support transactions, so
     * this will simply assert false.
     */
    virtual void rollbackTransaction();

    /**
     * escapeInsertString
     *
     * Parses a string for insertion into the database, and returns it with any special 
     * characters escaped.
     * Note: This method assumes that the string passed in has NOT had any escaping
     * already applied. If it does, the items will be escaped a second time.
     *
     * @param theString The string to parse
     *
     * @return The input string with escape characters added where and if necessary.
     */
    std::string escapeInsertString(const std::string& theString);

    /**
     * escapeQueryString
     *
     * Parses a string for use in a SELECT query, and returns it with any special 
     * characters escaped.
     * Note: 1) This method assumes that the string passed in has NOT had any escaping
     * already applied. If it does, the items will be escaped a second time.
     *       2) For the Orcale implementation of SQL, there is no pre-defined escape
     * character for the "_" and "%" characters within "select...like..." statements.
     * Due to this problem, this method will only escape the standard single-quote character
     * as done for insert statements.
     *
     * @param theString The string to parse
     *
     * @return The string with escape characters added where and if necessary.
     */
    std::string escapeQueryString(const std::string& theString);


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
    std::string escapeAQSQLString(const std::string& theString);

	/**
	 * GetDbServerType
	 * 
	 * This method get the database type that currently in used.
     *
	 */
	virtual int getDbServerType();

	/**
	 * getDBServerID
	 * 
	 * This method get the database server id for replication node.
	 * if the db node is formed a cluster, then this method is useless
	 * and should not use this method.
     *
	 */
	virtual int  getDBServerID();

	/**
	* prepareSQLStatement
	*
	* @brief: to prepare the SQL statement, this method accept the
	*         SQLVarParms object which used to construct the SQL
	*         statement into a vector.
	*
	* @param varParms    the SQLVarParms object to construct the 
	*                    SQL statement into a vector
	* Returns the SQL statement as a std::string.
	*/
	void prepareSQLStatement(std::string& rStrSql, SQLVarParms& varParms);

	/**
	* prepareSQLStatement
	*
	* @brief: to prepare the SQL statement, this method accept just
	*         one parameter. such as: select * from table_name. the
	*         parameters will store in a vector and the first element
	*         should always be the SQL statement id which map into the 
	*         sql.dat file.
	*
	* @param  arg0   the SQL statement id which map into
	*                the sql.dat file
	* Returns the SQL statement as a std::string.
	*/ 
	void prepareSQLStatement(std::string& strSQL, const SQLTypeAdapter& arg0);

	// Declare the remaining overloads of PrepareSQLStatement()
	DEF_SQL_PARMS(void, prepareSQLStatement);
	


public:	

	virtual int    enqMsg(CMessage* pEnqMsg, PMAQ_ENQ_OPT pEnqOpt);
	virtual int	   enqMsg( std::list<CMessage*>& EnqMsgList, PMAQ_ENQ_OPT pEnqOpt );

	virtual int    deqMsg(const std::string& strQName, PMAQ_DEQ_OPT pDeqMsg, LstMsgConT& rLstMsg);	
	virtual bool   listen(const std::string& strQName, u_int  nSubID);
	virtual bool   listen(const std::string& strQName, const std::string& strSubName);
	virtual int    aqCommit(const std::string& strQName, int nSubID, const std::string& strConsumer, const nPairT& nSeqIDPair);
	virtual int    aqCommit(const std::string& strQName, const std::string& strMsgID);

	
private:
	void _TryToOpenDb();
	int  _GetDbServerType();
	int  _ParseDbConStr(const std::string& strDbCon);
	int  _ParseDbType(const std::string& strDbName);
	
    IDbConnection*           m_pDbConnection;	// The database connection
	IQueryAdapter*           m_pQueryAdapter;   // the result set of the current connection
	StrVecT                  m_columnNames;		// Local copy of the vector of column names
	u_long                   m_numRows;		    // Number of rows of data to retrieve at a time
	bool                     m_bAutoCommit;     // the current connection auto-commit mode.
};

NS_END(TA_AQ_Lib)

#endif // DATABASEIMPL_H




















