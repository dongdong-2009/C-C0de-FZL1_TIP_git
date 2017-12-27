/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciConnection.cpp $
 * @author:  Robert Stagg
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <occi.h>

#include "core/utilities/src/TAAssert.h"

#include "OcciConnectionPool.h"
#include "OcciConnection.h"
#include "OcciStatement.h"
#include "OcciSqlExceptionHandler.h"

OcciConnection::OcciConnection(OcciConnectionPool* pool, Connection* connection)
    : m_connectionPool(pool),
      m_connection(connection),
      m_commitRequired(false),
      m_autoCommitEnabled(false)
{
    FUNCTION_ENTRY("OcciConnection");

    FUNCTION_EXIT;
}

OcciConnection::~OcciConnection()
{
    FUNCTION_ENTRY("~OcciConnection");

	//NOTE: must trap all exceptions because
	//the shared_ptr class says exceptions
	//during deconstructing results in
	//"undefined behaviour", what ever that
	//means.
	//
	//Must clean up the statement list first.
	//
	//Then we can release this connection
	//back to the pool
    if ( m_statements.size() )
    {
        try
        {
            disconnect();
        }
        catch (...)
        {
        }
    }

    FUNCTION_EXIT;
}

void OcciConnection::disconnect()
{
    FUNCTION_ENTRY("disconnect");

    TA_ASSERT(0 != m_connection, "Already disconnected");

	//Commit if required.
    commitTransaction();

	//Ensure all statements are free.
    while ( m_statements.size() )
    {
        try
        {
            (*m_statements.begin())->freeStatement(m_connection);
        }
        catch (...)
        {
            ; // don't really care about errors here
            ; // Just making sure memory doesn't leak
        }
        m_statements.pop_front();
    }

	//Return the connection to the pool by
	//calling releaseConnection on the pool
	//pointer. The pool will then unlink this
	//object which should cause it to die.
    m_connectionPool->releaseConnection(this, m_connection);
    m_connection = 0;
    FUNCTION_EXIT;
}

shared_ptr<OcciStatement> OcciConnection::prepare(const std::string& sql)
{
    FUNCTION_ENTRY("prepare");

    TA_ASSERT(0 != m_connection, "Not connected");

    try
    {
    	//Check the Oracle statement cache for
    	//this statement. If found return it,
    	//otherwise create a new statment handle.
    	Statement* statement = m_connection->createStatement(sql);

        // if autocommit required set it now
        if ( m_autoCommitEnabled )
        {
            statement->setAutoCommit(true);
        }

        // create a new Occi statment to hold it
        shared_ptr<OcciStatement> occiStatement(new OcciStatement(statement, this));

        //add this statment to our list
        m_statements.push_back(occiStatement);

        //return new OCIStatement
        FUNCTION_EXIT;
        return occiStatement;

    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    TA_ASSERT(false, "Should never get here");
}

void OcciConnection::beginTransaction()
{
    FUNCTION_ENTRY("beginTransaction");

	//Can't really 'start' a new transaction
	//so...
	//set autoCommit to false as the user
	//want to control transactions and
	//call commit to ensure any transactions
	//in progress are completed.
	//We do not support nested transactions.
    m_autoCommitEnabled = false;
    commitTransaction();

    FUNCTION_EXIT;
}

void OcciConnection::commitTransaction()
{
    FUNCTION_ENTRY("commitTransaction");

    TA_ASSERT(0 != m_connection, "Not connected");

	//Call commit on the connection
	//Clear the commit pending flag
    try
    {
        m_connection->commit();
    }
    catch (SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }
    m_commitRequired = false;

    FUNCTION_EXIT;
}

void OcciConnection::rollbackTransaction()
{
    FUNCTION_ENTRY("rollbackTransaction");

    TA_ASSERT(0 != m_connection, "Not connected");

	//Call roll back on the connection
	//clear the commit pending flag
    try
    {
        m_connection->rollback();
    }
    catch (SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }
    m_commitRequired = false;

    FUNCTION_EXIT;
}

void OcciConnection::setAutoCommit(bool autoCommit)
{
    FUNCTION_ENTRY("setAutoCommit");

    if ( m_autoCommitEnabled != autoCommit )
    {
        if ( true == autoCommit )
        {
            // do commit now so it doesn't get lost
            commitTransaction();
        }

        // spin through the statment list and set the mode as required
        for( StatementList::iterator it = m_statements.begin(); it != m_statements.end(); it++ )
        {
            (*it)->setAutoCommit(autoCommit);
        }
    }
    m_autoCommitEnabled = autoCommit;

    FUNCTION_EXIT;
}

void OcciConnection::commitRequired()
{
    FUNCTION_ENTRY("commitRequired");

    m_commitRequired = true;

    FUNCTION_EXIT;
}

bool OcciConnection::isValid()
{
    FUNCTION_ENTRY("isValid");

    TA_ASSERT(0 != m_connection, "Not connected");

    // it's not good enough to just return true (although it is tempting)
    // so we do a select from dual to ensure that things have not gone bad
    try
    {
        Statement* st = m_connection->createStatement("select 69 from dual");
        ResultSet* rs = st->executeQuery();
        if ( 69 != rs->getInt(1) )
        {
            // this is not good
            m_connection->terminateStatement(st);
            FUNCTION_EXIT;
            return false;
        }
        m_connection->terminateStatement(st);
    }
    catch (SQLException& )
    {
        // something bad happened
        FUNCTION_EXIT;
        return false;
    }

    FUNCTION_EXIT;
    return true;
}

//Mintao++: use for Distributed DB
std::string OcciConnection::escapeAQSQLString(const std::string& theString)
{
    return (escapeInsertString(escapeInsertString(theString)));         
}
//Mintao++: use for Distributed DB

std::string OcciConnection::escapeInsertString(const std::string& theString)
{
    FUNCTION_ENTRY("escapeInsertString");

    FUNCTION_EXIT;
    return escapeQueryString(theString);
}

std::string OcciConnection::escapeQueryString(const std::string& theString)
{
    FUNCTION_ENTRY("escapeQueryString");

    // create a new string to hold the parsed string
    std::string escapedString = theString;
    int pos = escapedString.find_first_of("'");
    while (-1 != pos)
    {
        char charToInsert = escapedString.at(pos);
        escapedString.insert(pos,&charToInsert, 1);
		pos = escapedString.find_first_of("'", pos + 2);
    }
    FUNCTION_EXIT;
    return escapedString;
}

/**********************************************************************
 *
 * functions below this line should never be called
 *
 * They are included for backward compatibility with the old interface
 *
 **********************************************************************/

void OcciConnection::connect(const std::string& connectionString)
{
    FUNCTION_ENTRY("connect");

    TA_ASSERT(false, "This function should only be called on a OcciDatabase");

    FUNCTION_EXIT;
}

void OcciConnection::executeModification(const std::string& sql)
{
    FUNCTION_ENTRY("executeModification");

	//Should never be called for Occi
	TA_ASSERT(false, "This function should only be called on a statement");

    FUNCTION_EXIT;
}

TA_Base_Core::IData* OcciConnection::executeQuery(const std::string& sql, const std::vector<std::string>& columnNames, const unsigned long numRows)
{
    FUNCTION_ENTRY("executeQuery");

	//Should never be called for Occi
	TA_ASSERT(false, "This function should only be called on a statement");

    FUNCTION_EXIT;
    return 0;
}

TA_Base_Core::IData* OcciConnection::executeQuery(const std::string& sql, const std::vector<std::string>& columnNames)
{
    FUNCTION_ENTRY("executeQuery");

	//Should never be called for Occi
	TA_ASSERT(false, "This function should only be called on a statement");

    FUNCTION_EXIT;
    return 0;
}

TA_Base_Core::IData* OcciConnection::executeProcedure(const std::string& sql, const std::vector<std::string>& columnNames, const unsigned long numRows)
{
    FUNCTION_ENTRY("executeProcedure");

	//Should never be called for Occi
	TA_ASSERT(false, "This function should only be called on a statement");

    FUNCTION_EXIT;
    return 0;
}

void OcciConnection::executeProcedure(const std::string& sql)
{
    FUNCTION_ENTRY("executeProcedure");

	//Should never be called for Occi
	TA_ASSERT(false, "This function should only be called on a statement");

    FUNCTION_EXIT;
}

bool OcciConnection::moreData(TA_Base_Core::IData*& returnData)
{
    FUNCTION_ENTRY("moreData");

	//Should never be called for Occi
	TA_ASSERT(false, "This function should only be called on a statement");

    FUNCTION_EXIT;
    return false;
}

void OcciConnection::readBLOB(const std::string& fieldName, const std::string& tableName, const std::string& whereClause, std::vector<unsigned char>& data)
{
    FUNCTION_ENTRY("readBLOB");

	//Should never be called for Occi
	TA_ASSERT(false, "This function should only be called on a statement");

    FUNCTION_EXIT;
}

void OcciConnection::writeBLOB(const std::string& fieldName, const std::string& tableName, const std::string& whereClause, std::vector<unsigned char>& data)
{
    FUNCTION_ENTRY("writeBLOB");

    //Should never be called for Occi
	TA_ASSERT(false, "This function should only be called on a statement");

    FUNCTION_EXIT;
}