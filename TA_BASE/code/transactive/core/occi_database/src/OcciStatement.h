/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciStatement.h $
 * @author:  Robert Stagg
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 */

#if !defined(EA_3EF3DC76_6005_4ade_A212_57B9AE2D8EBB__INCLUDED_)
#define EA_3EF3DC76_6005_4ade_A212_57B9AE2D8EBB__INCLUDED_


#include "boost/smart_ptr.hpp"
#include "core/data_access_interface/src/IDbStatement.h"

using oracle::occi::Connection;
using oracle::occi::Statement;
using oracle::occi::ResultSet;
using oracle::occi::SQLException;
using namespace boost;

class OcciConnection;

class OcciStatement : public IDbStatement
{

public:
	/**
	 *
	 * @param statementHandle
	 */
	OcciStatement(Statement* statementHandle, OcciConnection* connectionHandle);

	virtual ~OcciStatement();

	/**
	 * Bind a long value to a location in the SQL.
	 *
	 * @param position    Position of parameter in the SQL to bind the value to. Given as base 1
	 * @param value    value to bind
	 * @param isValid  true to bind the value, false is set it to NULL
	 */
	virtual const void bindParam(const unsigned int position, const long value, bool isValid = true);

	/**
	 * Bind an int value to a location in the SQL.
	 *
	 * @param position    Position of parameter in the SQL to bind the value to. Given as base 1
	 * @param value    value to bind
	 */
	virtual const void bindParam(const unsigned int position, const int value, bool isValid = true);

	/**
	 * Bind a double value to a location in the SQL.
	 *
	 * @param position    Position of parameter in the SQL to bind the value to. Given as base 1
	 * @param value    value to bind
	 */
	virtual const void bindParam(const unsigned int position, const double value, bool isValid = true);

	/**
	 * Bind a short value to a location in the SQL.
	 *
	 * @param position    Position of parameter in the SQL to bind the value to. Given as base 1
	 * @param value    value
	 */
	virtual const void bindParam(const unsigned int position, const short value, bool isValid = true);

	/**
	 * Bind an unsigned int  value to a location in the SQL.
	 *
	 * @param position    Position of parameter in the SQL to bind the value to. Given as base 1
	 * @param value    value to bind
	 */
	virtual const void bindParam(const unsigned int position, const unsigned int value, bool isValid = true);

	/**
	 * Bind an unsigned long value to a location in the SQL.
	 *
	 * @param position    Position of parameter in the SQL to bind the value to. Given as base 1
	 * @param value    value to bind
	 */
	virtual const void bindParam(const unsigned int position, const unsigned long value, bool isValid = true);

	/**
	 * Bind an unsigned short value to a location in the SQL.
	 *
	 * @param position    Position of parameter in the SQL to bind the value to. Given as base 1
	 * @param value    value to bind
	 */
	virtual const void bindParam(const unsigned int position, const unsigned short value, bool isValid = true);

	/**
	 * Bind a string to a location in the SQL.
	 *
	 * @param position    Position of parameter in the SQL to bind the value to. Given as base 1
	 * @param value    value to bind
	 */
	virtual const void bindParam(const unsigned int position, const std::string& value, bool isValid = true);

	/**
	 * Fills in an int location with the data from the current row in the result set.  Returns true if
	 * data was copied. Returns false if the data was null. Throws DataException if there is an error
	 *
	 * @param position    Position in the current row the data is expected. Given as base 1
	 * @param value    location of value to update. Will be cleared if data is NULL
	 */
	virtual const bool readParam(const unsigned int position, int& value);

	/**
	 * Fills in a data location with the data from the current row in the result set.  Returns true if
	 * data was copied. Returns false if the data was null. Throws DataException if there is an error
	 *
	 * @param position    Position in the current row the data is expected. Given as base 1
	 * @param value    location of value to update. Will be cleared if data is NULL
	 */
	virtual const bool readParam(const unsigned int position, long& value);

	/**
	 * Fills in a data location with the data from the current row in the result set.  Returns true if
	 * data was copied. Returns false if the data was null. Throws DataException if there is an error
	 *
	 * @param position    Position in the current row the data is expected. Given as base 1
	 * @param value    location of value to update. Will be cleared if data is NULL
	 */
	virtual const bool readParam(const unsigned int position, double& value);

	/**
	 * Fills in a data location with the data from the current row in the result set.  Returns true if
	 * data was copied. Returns false if the data was null. Throws DataException if there is an error
	 *
	 * @param position    Position in the current row the data is expected. Given as base 1
	 * @param value    location of value to update. Will be cleared if data is NULL
	 */
	virtual const bool readParam(const unsigned int position, short& value);

	/**
	 * Fills in a data location with the data from the current row in the result set.  Returns true if
	 * data was copied. Returns false if the data was null. Throws DataException if there is an error
	 *
	 * @param position    Position in the current row the data is expected. Given as base 1
	 * @param value    location of value to update. Will be cleared if data is NULL
	 */
	virtual const bool readParam(const unsigned int position, unsigned int& value);

	/**
	 * Fills in a data location with the data from the current row in the result set.  Returns true if
	 * data was copied. Returns false if the data was null. Throws DataException if there is an error
	 *
	 * @param position    Position in the current row the data is expected. Given as base 1
	 * @param value    location of value to update. Will be cleared if data is NULL
	 */
	virtual const bool readParam(const unsigned int position, unsigned long& value);

	/**
	 * Fills in a data location with the data from the current row in the result set.  Returns true if
	 * data was copied. Returns false if the data was null. Throws DataException if there is an error
	 *
	 * @param position    Position in the current row the data is expected. Given as base 1
	 * @param value    location of value to update. Will be cleared if data is NULL
	 */
	virtual const bool readParam(const unsigned int position, unsigned short& value);

	/**
	 * Fills in a data location with the data from the current row in the result set.  Returns true if
	 * data was copied. Returns false if the data was null. Throws DataException if there is an error
	 *
	 * @param position    Position in the current row the data is expected. Given as base 1
	 * @param value    location of value to update. Will be cleared if data is NULL
	 */
	virtual const bool readParam(const unsigned int position, std::string& value);

	/**
	 * Runs a SELECT SQL statetment. Returns a pointer to the result set Throws DatabaseException if
	 * something goes wrong
	 */
	virtual void executeQuery();

	/**
	 * Runs a non SELECT SQL. Returns the number of rows affected Throws DatabaseException on error
	 */
	virtual unsigned int executeUpdate();

	/**
	 * moreData will point to the next row of data returned from a select. Execute will set the internal
	 * pointer to the 0 row. Thus moreData must be called before any readXxx() functions.  Returns true if
	 * there is more data available. Retuns false if there is no more data.
	 */
	virtual bool moreData();

	/**
	 * frees the Oracle Statement object
	 *
	 * @param connection    pointer to the oracle connection that owns this statement
	 */
	void freeStatement(Connection* connection);

	/**
	 * set transaction mode
	 * true is every sql commits on completion
	 * false is commit is required (default)
	 * this is normaly called fron the OcciConnection class
	 * Client code should be calling setAutoCommit on OcciConnection
	 * not this class.
	 *
	 * @param autoCommit     true is every sql commits on completion
	 * false is commit is required (default)
	 */
	void setAutoCommit(bool autoCommit);


private:
	/**
	 * Processes an Oracle error and trys to recover
	 *
	 * @param ex
	 */
	void handleError(SQLException& ex);

private:

    ResultSet* m_resultSet;
	Statement* m_statement;
    Statement::Status m_result;
    OcciConnection* m_connection;

};
#endif // !defined(EA_3EF3DC76_6005_4ade_A212_57B9AE2D8EBB__INCLUDED_)