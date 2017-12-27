/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DbConnection.cpp $
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

#include "core/data_access_interface/src/DbConnection.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DbConnectionFailed.h"

#include "core/exceptions/src/DbConnectionNoFile.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/threads/src/Thread.h"

#include <iostream>

namespace TA_Base_Core
{
    DbConnection* DbConnection::m_instance = 0;
    NonReEntrantThreadLockable DbConnection::m_instanceLock;


    DbConnection::DbConnection()
    {
        FUNCTION_ENTRY("DbConnection");
        if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
        {
            throw DbConnectionFailed("db-connection-file not set");
        }
        std::string filename = RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
        try
        {
            DbConnectionStrings connectStrings(filename);
            m_dbMap = connectStrings.getConnectionMap();
        }
        catch (DbConnectionNoFile e)
        {
            throw DbConnectionFailed( e.what() );
        }

    	FUNCTION_EXIT;
    }

    DbConnection::~DbConnection()
    {
    }

    DbConnection& DbConnection::getInstance()
    {
        FUNCTION_ENTRY("getInstance");
        ThreadGuard guard( m_instanceLock );
        if( m_instance == 0 )
        {
            m_instance = new DbConnection();
        }            

    	FUNCTION_EXIT;
        return *m_instance;
    }

	void DbConnection::removeInstance()
	{
        ThreadGuard guard( m_instanceLock );
        if( m_instance != NULL )
        {
            delete m_instance;
			m_instance = NULL;
        }            
	}

    void DbConnection::getConnectionString(const EDataTypes dataType, const EDataActions action, std::string& connectionString)
    {
        FUNCTION_ENTRY("getConnectionString");

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDatabase, "Looking for a database for data type %i, action %i.",
			 dataType, action );

        DatabaseMap::iterator dbIter ( m_dbMap.find( std::make_pair( dataType, action )));
        if ( dbIter == m_dbMap.end() )
        {
            std::string msg ("No connection string defined for ");
            msg += dataType;   // should create a lookup function
            msg += "/";        // to change the enums back into
            msg += action;     // strings.
            throw DbConnectionFailed(msg); 
        }
        std::vector<DataConnection> dbList = dbIter->second;
        /*std::vector<DataConnection>::iterator dbListIter ( dbList.begin() );
        do 
        {
            if ( DbStatus::getInstance().isAvailable( dbListIter->first ) )
            {
                connectionString = dbListIter->second;

            	FUNCTION_EXIT;
                return;
            }
            dbListIter++; // point to next db in list
        }
        while ( dbListIter != dbList.end() );*/

		int retry = 0;

		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"Trying to find working database...");

		while ( retry < 3 )
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"Start Iteration %d",retry);

			std::vector<DataConnection>::iterator dbListIter ( dbList.begin() );
			do 
			{
				if ( DbStatus::getInstance().isAvailable( dbListIter->first ) )
				{
					connectionString = dbListIter->second;
					LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
						"Found connection string: %s", connectionString.c_str());

            		FUNCTION_EXIT;
					return;
				}
				/*LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"Sleeping before next iteration");
				TA_Base_Core::Thread::sleep(100);
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"Sleeping finished for next iteration");*/
				dbListIter++; // point to next db in list
			}
			while ( dbListIter != dbList.end() );
			
			
			TA_Base_Core::Thread::sleep(5000);

			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"End Iteration %d",retry);
			retry++;
		}

        // If we make it here, we failed to find any working db, so 
        throw DbConnectionFailed("No working database found");
    }

} // namespace
