/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciConnectionPool.cpp $
 * @author:  R.Stagg
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
#include "core/synchronisation/src/ThreadGuard.h"
#include "OcciConnectionPool.h"
#include "OcciSqlExceptionHandler.h"

using namespace oracle::occi;

const int OcciConnectionPool::DEFAULT_MAX_CONNECTIONS = 10;
const int OcciConnectionPool::DEFAULT_MIN_CONNECTIONS = 5;
const int OcciConnectionPool::DEFAULT_INCR_CONNECTIONS = 1;

OcciConnectionPool::OcciConnectionPool(const std::string& databaseName, const std::string& userName, const std::string& password)
    : m_databaseName( databaseName ),
      m_userName( userName ),
      m_password( password ),
      m_pool( 0 )
{
    FUNCTION_ENTRY("OCIConnectionPool");

    FUNCTION_EXIT;
}

OcciConnectionPool::~OcciConnectionPool()
{
    FUNCTION_ENTRY("~OCIConnectionPool");


    FUNCTION_EXIT;
}

shared_ptr<OcciConnection> OcciConnectionPool::open(Environment* env)
{
    FUNCTION_ENTRY("open");

    {
        TA_Base_Core::ThreadGuard guard(m_poolGuard);
        if ( 0 == m_pool )
        {
            // must create the pool
            try
            {
                m_pool = env->createConnectionPool( m_userName, m_password, m_databaseName,
                                                    DEFAULT_MAX_CONNECTIONS, DEFAULT_MIN_CONNECTIONS, DEFAULT_INCR_CONNECTIONS );
            }
            catch (SQLException& ex)
            {
                OcciSqlExceptionHandler::throwOnError(ex);
            }
        }
    }

    // get a connection from the pool
    Connection* con;
    try
    {
        con = m_pool->createConnection(m_userName, m_password);
    }
    catch (SQLException& ex)
    {
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    // Create a OcciConnection to hold this connection
    shared_ptr<OcciConnection> occiConnection(new OcciConnection(this, con));

    // and store it before returning it
    TA_Base_Core::ThreadGuard guard(m_connectionsGuard);
    m_connections.push_back(occiConnection);

    FUNCTION_EXIT;
    return occiConnection;
}

void OcciConnectionPool::disconnect(Environment* env)
{
    FUNCTION_ENTRY("disconnect");

    TA_Base_Core::ThreadGuard guard(m_poolGuard);
    TA_ASSERT(0 != env, "null pointer passed in");
    TA_ASSERT(0 != m_pool, "Already disconnected");

    // TWEET!! Everyone out of the pool!

    // NOTE: we do not guard the connection list because
    // it is guarded when a connection gets deleted.
    // Because it will change we can not use an iterator.
    while ( m_connections.size() )
    {
        // this will call back onto us (releaseConnection below)
        m_connections[0]->disconnect();
    }

    // everyone has left the pool - time to close
    try
    {
        env->terminateConnectionPool(m_pool);
        m_pool = 0;
    }
    catch (SQLException& ex)
    {
        m_pool = 0;
        OcciSqlExceptionHandler::throwOnError(ex);
    }

    FUNCTION_EXIT;
}

int OcciConnectionPool::getMaxConnections()
{
    FUNCTION_ENTRY("getmaxConnections");

    TA_ASSERT(0 != m_pool, "Not Connected");

    FUNCTION_EXIT;
    return static_cast<int> (m_pool->getMaxConnections());
}

int OcciConnectionPool::getCurrentConnections()
{
    FUNCTION_ENTRY("getCurrentConnections");

    TA_ASSERT(0 != m_pool, "Not Connected");

    FUNCTION_EXIT;
    return static_cast<int> (m_pool->getBusyConnections());
}

void OcciConnectionPool::setMaxConnections(int newVal)
{
    FUNCTION_ENTRY("setMaxConnections");

    TA_ASSERT(0 != m_pool, "Not Connected");

    m_pool->setPoolSize(newVal);

    FUNCTION_EXIT;
}

void OcciConnectionPool::releaseConnection(OcciConnection* occiConnection, Connection* connection)
{
    FUNCTION_ENTRY("releaseConnection");

    TA_ASSERT(0 != m_pool, "Not Connected");

    // remove the OcciConnection from our list
    {
        TA_Base_Core::ThreadGuard guard(m_connectionsGuard);

        Connections::iterator con( m_connections.begin() );
        while (con != m_connections.end() )
        {
            if ( con->get() == occiConnection )
            {
                break;
            }
            con++;
        }
        if ( con != m_connections.end() )
        {
            m_connections.erase( con );

            // return the connection to the pool
            try
            {
                m_pool->terminateConnection(connection);
            }
            catch (SQLException& ex)
            {
                OcciSqlExceptionHandler::throwOnError(ex);
            }
        }
        else // connection already freed
        {
            // Log something (Just wondering if this will happen??)
        }

    }
    FUNCTION_EXIT;
}