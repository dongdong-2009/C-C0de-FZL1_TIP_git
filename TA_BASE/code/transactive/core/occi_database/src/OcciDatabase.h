/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/occi_database/src/OcciDatabase.h $
 * @author:  Robert Stagg
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * There should be one instance of this object for each database connection string.
 */

#if !defined(EA_A6851557_E062_40fa_9F02_B77798792A4F__INCLUDED_)
#define EA_A6851557_E062_40fa_9F02_B77798792A4F__INCLUDED_

#include <occi.h>

#include "OcciConnection.h"
#include "OcciConnectionPool.h"

using namespace oracle::occi;
class OcciDatabase
{

public:
	OcciDatabase();

	virtual ~OcciDatabase();

	/**
	 * Connects to the specified database.
	 * Throws DBException if there is a problem that can not be overcome.
	 *
	 * @param user    The schema name
	 * @param pass    The password to for the schema
	 * @param database    The database name to connect to
	 */
	shared_ptr<OcciConnection> connect(const std::string& user, const std::string& pass, const std::string& database);

	/**
	 * Disconnects from the database and cleans up all memory.
	 */
	void disconnect();

private:
	OcciConnectionPool* m_pool;
    Environment* m_env; 

};
#endif // !defined(EA_A6851557_E062_40fa_9F02_B77798792A4F__INCLUDED_)