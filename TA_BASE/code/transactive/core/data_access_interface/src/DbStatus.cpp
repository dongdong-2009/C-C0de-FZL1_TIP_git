/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DbStatus.cpp $
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

#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/data_access_interface/src/DbStatus.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Core
{
    DbStatus* DbStatus::m_instance = 0;
    NonReEntrantThreadLockable DbStatus::m_instanceLock;

    DbStatus::DbStatus()
    {
    }


    DbStatus::~DbStatus()
    {
		//RunParams::getInstance().deregisterRunParamUser(this);
    }


    DbStatus& DbStatus::getInstance()
    {
        ThreadGuard guard( m_instanceLock );
        if( m_instance == 0 )
        {
            m_instance = new DbStatus();
        }            
        return *m_instance;
    }

	void DbStatus::removeInstance()
	{
        ThreadGuard guard( m_instanceLock );
        if( m_instance != NULL )
        {
            delete m_instance;
			m_instance = NULL;
        }     
	}

    bool DbStatus::isAvailable(const std::string& databaseId)
    {
        FUNCTION_ENTRY("isAvailable");		

	    // Check first if we know about this database.
        try
        {
            EDbState dbstate = getDbState(databaseId);

            // we do know about this database 
            if ( dbstate == Offline )
            {
                // it's known to be bad
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Return status of %s offline", databaseId.c_str());
                return false;
            }

	        // it's not marked bad so return true.
            FUNCTION_EXIT;
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Return status of %s online", databaseId.c_str());
            return true;
        }
        catch (...)
        {
	        // If not, do we know about this database.
            std::string pointname ( RPARAM_DBPREFIX + databaseId);

			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Not in map yet checking if system controller has set runtime parameter %s",pointname.c_str());

            if ( RunParams::getInstance().isSet(pointname.c_str()) )
            {
                EDbState state = RunParams::getInstance().get(pointname.c_str()).compare(RPARAM_DBONLINE)==0 ? Online :Offline;
                addDbState(databaseId, state);
                RunParams::getInstance().registerRunParamUser(this, pointname.c_str());

                if ( state == Online)
                {
                    FUNCTION_EXIT;
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "System controller runtime parameter %s is set to online",pointname.c_str());
                    return true;
                }
                else
                {
                    FUNCTION_EXIT;
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "System controller runtime parameter %s is set to offline",pointname.c_str());
                    return false;
                }
            }
            else // No not yet :(
            {
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "System controller has not yet is set %s default setting to offline",pointname.c_str());

                // add point to runparmas and our own list of db's
                RunParams::getInstance().set(pointname.c_str(),RPARAM_DBUNKNOWN);
                //addDbState(databaseId, Online);
				addDbState(databaseId, Offline);

				RunParams::getInstance().registerRunParamUser(this, pointname.c_str());
                FUNCTION_EXIT;
                //return true;
				return false;
            }
        }
        // should never get here but if we do,
        FUNCTION_EXIT;
        return true;
    }

    void DbStatus::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");
        std::string dbId (name);
        dbId.erase(0, name.find_first_of("_") + 1);

        if (value.compare(RPARAM_DBONLINE) == 0 )
        {
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Runtime param change detected, updating state for dbid %s to online",dbId.c_str() );

            updateDbState(dbId, Online);
        }
        else
        {
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "	Runtime param change detected, updating state for dbid %s to offline",dbId.c_str() );

            updateDbState(dbId, Offline);
        }

        FUNCTION_EXIT;
    }


    EDbState DbStatus::getDbState(const std::string& dbId)
    {
        FUNCTION_ENTRY("getDbState");
        ThreadReadGuard guard( m_listLock );
        DbList::iterator db = m_dbList.find( dbId );
        if ( db == m_dbList.end() )
        {
            throw 1; // make real exception
        }

        FUNCTION_EXIT;
        return db->second;
    }
	
    void DbStatus::addDbState(const std::string& dbId, const EDbState dbState)
    {
        FUNCTION_ENTRY("addDbState");
        ThreadGuard guard( m_listLock );
        m_dbList.insert( std::make_pair( dbId, dbState ));

        FUNCTION_EXIT;
    }

    void DbStatus::updateDbState(const std::string dbId, const EDbState dbState)
    {
        FUNCTION_ENTRY("updateDbState");
        ThreadGuard guard( m_listLock );
        DbList::iterator db = m_dbList.find( dbId );
        if ( db == m_dbList.end() )
        {
            TA_ASSERT(false, "Update called on unknown database");
        }
        db->second = dbState;

        FUNCTION_EXIT;
    }
} // namespace
