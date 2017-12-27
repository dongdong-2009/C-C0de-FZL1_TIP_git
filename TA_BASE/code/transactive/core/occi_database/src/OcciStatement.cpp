/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciStatement.cpp $
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

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "OcciConnection.h"
#include "OcciStatement.h"
#include "OcciSqlExceptionHandler.h"

using namespace oracle::occi;

OcciStatement::OcciStatement(Statement* statementHandle, OcciConnection* connectionHandle)
    : m_statement(statementHandle),
      m_connection(connectionHandle),
      m_result(Statement::PREPARED),
      m_resultSet(0)
{
    FUNCTION_ENTRY("OcciStatement");

    FUNCTION_EXIT;
}

OcciStatement::~OcciStatement()
{
    FUNCTION_ENTRY("~OcciStatement");

	//Ensure oracle statement pointer is null
	//(freed)
	//As it requires the Connection pointer
	//we can't do it from here so just assert
	//if it's not.
    TA_ASSERT( 0 == m_statement, "Statement pointer not freed" );
    FUNCTION_EXIT;
}

const void OcciStatement::bindParam(const unsigned int position, const long value, bool isValid)
{
    FUNCTION_ENTRY("bindParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if (isValid)
        {
            m_statement->setInt(position, static_cast<int>(value));
        }
        else
        {
            m_statement->setNull(position, OCCIINT);
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

const void OcciStatement::bindParam(const unsigned int position, const int value, bool isValid)
{
    FUNCTION_ENTRY("bindParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if (isValid)
        {
            m_statement->setInt(position, value);
        }
        else
        {
            m_statement->setNull(position, OCCIINT);
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

const void OcciStatement::bindParam(const unsigned int position, const double value, bool isValid)
{
    FUNCTION_ENTRY("bindParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if (isValid)
        {
            m_statement->setDouble(position, value);
        }
        else
        {
            m_statement->setNull(position, OCCIDOUBLE);
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

const void OcciStatement::bindParam(const unsigned int position, const short value, bool isValid)
{
    FUNCTION_ENTRY("bindParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if (isValid)
        {
            m_statement->setInt(position, static_cast<int>(value));
        }
        else
        {
            m_statement->setNull(position, OCCIINT);
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

const void OcciStatement::bindParam(const unsigned int position, const unsigned int value, bool isValid)
{
    FUNCTION_ENTRY("bindParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if (isValid)
        {
            m_statement->setUInt(position, value);
        }
        else
        {
            m_statement->setNull(position, OCCIUNSIGNED_INT);
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

const void OcciStatement::bindParam(const unsigned int position, const unsigned long value, bool isValid)
{
    FUNCTION_ENTRY("bindParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if (isValid)
        {
            m_statement->setUInt(position, static_cast<unsigned int>(value));
        }
        else
        {
            m_statement->setNull(position, OCCIUNSIGNED_INT);
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

const void OcciStatement::bindParam(const unsigned int position, const unsigned short value, bool isValid)
{
    FUNCTION_ENTRY("bindParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if (isValid)
        {
            m_statement->setInt(position, static_cast<unsigned int>(value));
        }
        else
        {
            m_statement->setNull(position, OCCIUNSIGNED_INT);
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

const void OcciStatement::bindParam(const unsigned int position, const std::string& value, bool isValid)
{
    FUNCTION_ENTRY("bindParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if (isValid)
        {
            m_statement->setString(position, value);
        }
        else
        {
            m_statement->setNull(position, OCCI_SQLT_CHR);
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

const bool OcciStatement::readParam(const unsigned int position, int& value)
{
    FUNCTION_ENTRY("readParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        value = m_resultSet->getInt(position);
    }
    catch (SQLException& ex)
    {
        // NULL value?
        if ( ex.getErrorCode() == OcciSqlExceptionHandler::COLUMN_NULL_ERROR )
        {
            FUNCTION_EXIT;
            return false;
        }

        // nope. off to the error handler with you
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return true;
}

const bool OcciStatement::readParam(const unsigned int position, long& value)
{
    FUNCTION_ENTRY("readParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        value = static_cast<long>( m_resultSet->getInt(position) );
    }
    catch (SQLException& ex)
    {
        // NULL value?
        if ( ex.getErrorCode() == OcciSqlExceptionHandler::COLUMN_NULL_ERROR )
        {
            FUNCTION_EXIT;
            return false;
        }

        // nope. off to the error handler with you
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return true;
}

const bool OcciStatement::readParam(const unsigned int position, double& value)
{
    FUNCTION_ENTRY("readParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        value = m_resultSet->getDouble(position);
    }
    catch (SQLException& ex)
    {
        // NULL value?
        if ( ex.getErrorCode() == OcciSqlExceptionHandler::COLUMN_NULL_ERROR )
        {
            FUNCTION_EXIT;
            return false;
        }

        // nope. off to the error handler with you
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return true;
}

const bool OcciStatement::readParam(const unsigned int position, short& value)
{
    FUNCTION_ENTRY("readParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        value = static_cast<short>( m_resultSet->getInt(position) );
    }
    catch (SQLException& ex)
    {
        // NULL value?
        if ( ex.getErrorCode() == OcciSqlExceptionHandler::COLUMN_NULL_ERROR )
        {
            FUNCTION_EXIT;
            return false;
        }

        // nope. off to the error handler with you
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return true;
}

const bool OcciStatement::readParam(const unsigned int position, unsigned int& value)
{
    FUNCTION_ENTRY("readParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        value = m_resultSet->getUInt(position);
    }
    catch (SQLException& ex)
    {
        // NULL value?
        if ( ex.getErrorCode() == OcciSqlExceptionHandler::COLUMN_NULL_ERROR )
        {
            FUNCTION_EXIT;
            return false;
        }

        // nope. off to the error handler with you
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return true;
}

const bool OcciStatement::readParam(const unsigned int position, unsigned long& value)
{
    FUNCTION_ENTRY("readParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        value = static_cast<unsigned long>( m_resultSet->getUInt(position) );
    }
    catch (SQLException& ex)
    {
        // NULL value?
        if ( ex.getErrorCode() == OcciSqlExceptionHandler::COLUMN_NULL_ERROR )
        {
            FUNCTION_EXIT;
            return false;
        }

        // nope. off to the error handler with you
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return true;
}

const bool OcciStatement::readParam(const unsigned int position, unsigned short& value)
{
    FUNCTION_ENTRY("readParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        value = static_cast<unsigned short>( m_resultSet->getUInt(position) );
    }
    catch (SQLException& ex)
    {
        // NULL value?
        if ( ex.getErrorCode() == OcciSqlExceptionHandler::COLUMN_NULL_ERROR )
        {
            FUNCTION_EXIT;
            return false;
        }

        // nope. off to the error handler with you
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return true;
}

const bool OcciStatement::readParam(const unsigned int position, std::string& value)
{
    FUNCTION_ENTRY("readParam");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        value = m_resultSet->getString(position);
    }
    catch (SQLException& ex)
    {
        // NULL value?
        if ( ex.getErrorCode() == OcciSqlExceptionHandler::COLUMN_NULL_ERROR )
        {
            FUNCTION_EXIT;
            return false;
        }

        // nope. off to the error handler with you
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return true;
}

void OcciStatement::executeQuery()
{
    FUNCTION_ENTRY("executeQuery");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        m_result = m_statement->execute();
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

unsigned int OcciStatement::executeUpdate()
{
    FUNCTION_ENTRY("executeUpdate");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    unsigned int rowsAffected = 0;
    try
    {
        rowsAffected = m_statement->executeUpdate();
        m_connection->commitRequired();
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
    return rowsAffected;
}

bool OcciStatement::moreData()
{
    FUNCTION_ENTRY("moreData");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        if ( m_resultSet->next() )
        {
            FUNCTION_EXIT;
            return true;
        }
    }
    catch(SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }
    FUNCTION_EXIT;
    return false;

}

void OcciStatement::setAutoCommit(bool autoCommit)
{
    FUNCTION_ENTRY("setAutoCommit");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        m_statement->setAutoCommit(autoCommit);
    }
    catch (SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}
void OcciStatement::handleError(SQLException& ex)
{
    FUNCTION_ENTRY("handleError");

    FUNCTION_EXIT;
}

void OcciStatement::freeStatement(Connection* connection)
{
    FUNCTION_ENTRY("freeStatement");

    TA_ASSERT(0 != m_statement, "Statement method called after statement freed");

    try
    {
        connection->terminateStatement(m_statement);
        m_statement = 0;
    }
    catch (SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}
