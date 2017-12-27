/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciConnection.h $
 * @author:  Robert Stagg
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * This object is NOT thread safe. Each thread should get it's own. Do not try to
 * share them.
 */

#if !defined(EA_9E4D288D_CDC0_44d5_8136_2F05327DA117__INCLUDED_)
#define EA_9E4D288D_CDC0_44d5_8136_2F05327DA117__INCLUDED_


#include "boost/smart_ptr.hpp"
#include "core/data_access_interface/src/IDatabase.h"

using namespace boost;

using oracle::occi::Connection;

class OcciConnectionPool;
class OcciStatement;


class OcciConnection : public TA_Base_Core::IDatabase
{

public:
	/**
	 *
	 * @param poll
	 */
    OcciConnection(OcciConnectionPool* pool, Connection* connection);

	~OcciConnection();

	/**
	 * not implemented here.
	 */
	virtual void connect(const std::string& connectionString);

	/**
	 * returns the connection to the pool and self destructs
	 */
	virtual void disconnect();

	/**
	 * Creates and returns a statment handle
	 */
	shared_ptr<OcciStatement> prepare(const std::string& sql);

	/**
	 * Starts a new transaction
	 */
	void beginTransaction();

	/**
	 * Commits the current transactions and starts a new one
	 */
	void commitTransaction();

	/**
	 * rolls back the current transaction and starts a new one
	 */
	void rollbackTransaction();

	/**
	 * set transaction mode
	 * true is every sql commits on completion
	 * false is commit is required (default)
	 *
	 * @param autoCommit     true is every sql commits on completion
	 * false is commit is required (default)
	 */
	void setAutoCommit(bool autoCommit);

	/**
	 * Sets a flag so the connection knows that a commit/rollback is required
	 */
	void commitRequired();

	/**
	 * Returns true if the connection is still valid
	 */
	bool isValid();

	/**
	 * stores the connection from the connection pool
	 */
	void setConnection();

	/**
	 * not implemented here.
	 *
	 * @param sql
	 */
	virtual void executeModification(const std::string& sql);

	/**
	 * not implemented here.
	 *
	 * @param sql
	 * @param columnNames
	 * @param numRows
	 */
	virtual TA_Base_Core::IData* executeQuery(const std::string& sql, const std::vector<std::string>& columnNames, const unsigned long numRows);

	/**
	 * not implemented here.
	 *
	 * @param sql
	 * @param columnNames
	 */
	virtual TA_Base_Core::IData* executeQuery(const std::string& sql, const std::vector<std::string>& columnNames);

	/**
	 * not implemented here.
	 *
	 * @param sql
	 * @param columnNames
	 * @param numRows
	 */
	virtual TA_Base_Core::IData* executeProcedure(const std::string& sql, const std::vector<std::string>& columnNames, const unsigned long numRows = 500);

	/**
	 * not implemented here.
	 *
	 * @param sql
	 */
	virtual void executeProcedure(const std::string& sql);

	/**
	 * not implemented here.
	 *
	 * @param returnData
	 */
	virtual bool moreData(TA_Base_Core::IData*& returnData);

	/**
	 * Takes the passed in string and returns a new string with all special charters escaped
	 *
	 * @param theString
	 */
	virtual std::string escapeInsertString(const std::string& theString);

	/**
	 * Takes the passed in string and returns a new string with all special charters escaped
	 *
	 * @param theString
	 */
	virtual std::string escapeQueryString(const std::string& theString);

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
    virtual std::string escapeAQSQLString(const std::string& theString);
	/**
	 * not implemented here.
	 *
	 * @param fieldName
	 * @param tableName
	 * @param whereClause
	 * @param data
	 */
	virtual void readBLOB(const std::string& fieldName, const std::string& tableName, const std::string& whereClause, std::vector<unsigned char>& data);

	/**
	 * not implemented here.
	 *
	 * @param fieldName
	 * @param tableName
	 * @param whereClause
	 * @param data
	 */
	virtual void writeBLOB(const std::string& fieldName, const std::string& tableName, const std::string& whereClause, std::vector<unsigned char>& data);


private:
	/**
	 * the pointer to the pool this connection came from
	 */
	OcciConnectionPool* m_connectionPool;

	/**
	 * the connection retrieved from the connection pool
	 */
	Connection* m_connection;

	/**
	 * true if transactions are disabled
	 */
	bool m_autoCommitEnabled;

	/**
	 * holds the list of statement handles currently allocated to this connection
	 */
    typedef std::list<shared_ptr<OcciStatement> > StatementList;
	StatementList m_statements;

	/**
	 * true - a commit is required
	 * false - not commit required
	 */
	bool m_commitRequired;


};
#endif // !defined(EA_9E4D288D_CDC0_44d5_8136_2F05327DA117__INCLUDED_)