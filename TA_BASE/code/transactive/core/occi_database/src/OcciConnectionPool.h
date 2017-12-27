/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciConnectionPool.h $
 * @author:  R.Stagg
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 */

#if !defined(EA_F440FE34_FEB9_404f_BBF6_DAAF0E61881A__INCLUDED_)
#define EA_F440FE34_FEB9_404f_BBF6_DAAF0E61881A__INCLUDED_

#include <vector>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/DebugUtil.h"

#include "OcciConnection.h"

using oracle::occi::Environment;
using oracle::occi::Connection;
using oracle::occi::ConnectionPool;
using namespace boost;

class OcciConnectionPool
{

public:
	/**
	 * OCIConnectionPool Sole constructor that stores the database connection details and  creates and
	 * manages the pool. Allocates connections from the pool to perform transactions with the database.
	 * Creates the environment required.
	 *
	 * @param connectionStr    databaseName:userName:password:hostName
	 * @param databaseName    the name of the database to connect to
	 * @param userName    the name of the user for the session
	 * @param password    the name of the password for the session
	 * @param hostName    the name of the host that the database is on
	 */
	OcciConnectionPool(const std::string& databaseName, const std::string& userName, const std::string& password);

	/**
	 * ~OCIConnection  Sole standard destructor. Deletes the connection pool and releases the environment
	 */
	~OcciConnectionPool();

	/**
	 * returns a connection from the connection pool. Creates the pool if it doesn't already exist.
	 */
	shared_ptr<OcciConnection> open(Environment* env);

	/**
	 * frees all connections and closes the pool
	 */
	void disconnect(Environment* env);

	/**
	 * The max number of connections in the pool
	 */
	int getMaxConnections();

	/**
	 * The number of connections in the pool currently in use
	 */
	int getCurrentConnections();

	/**
	 * The max number of connections in the pool
	 *
	 * @param newVal
	 */
	void setMaxConnections(int newVal);

	/**
	 * Returns the connection to the pool.
	 * The connection must have cleaned itself up before calling this
	 *
	 * @param connection
	 */
	void releaseConnection(OcciConnection* occiConnection, Connection* connection);

private:
	/**
	 * The name of the database this pool is connected to
	 */
	const std::string m_databaseName;

	/**
	 * The name of the schema this pool is connected to
	 */
	const std::string m_userName;

	/**
	 * The password used to connect
	 */
	const std::string m_password;

	/**
	 * the connection pool held by this object
	 */
    ConnectionPool* m_pool;

	/**
	 * The max number of connections in the pool
	 */
	static const int DEFAULT_MAX_CONNECTIONS;

	/**
	 * The min number of connections in the pool
	 */
	static const int DEFAULT_MIN_CONNECTIONS;

	/**
	 * The number of connections in the pool in increase by
	 */
	static const int DEFAULT_INCR_CONNECTIONS;

	/**
	 * The list of currently allocated connections
	 */
    typedef std::vector<shared_ptr<OcciConnection> > Connections;
	Connections m_connections;

	/**
	 * The lock to guard pool creation
	 */
    TA_Base_Core::NonReEntrantThreadLockable m_poolGuard;

	/**
	 * The lock to guard access to the open connections list
	 */
    TA_Base_Core::NonReEntrantThreadLockable m_connectionsGuard;

};
#endif // !defined(EA_F440FE34_FEB9_404f_BBF6_DAAF0E61881A__INCLUDED_)