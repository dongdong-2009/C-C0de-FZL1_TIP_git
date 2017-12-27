/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DatabaseFactory.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * DatabaseFactory is a singleton that returns a pointer to an IDatabase object
  * The database object that it returns will always be connected.
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::Thread;

namespace TA_Base_Core
{

    DatabaseFactory* DatabaseFactory::m_instance = 0;
    NonReEntrantThreadLockable DatabaseFactory::m_instanceLock;

    DatabaseFactory::~DatabaseFactory()
    {
        cleanup();  
	}

    DatabaseFactory& DatabaseFactory::getInstance()
    {            
        ThreadGuard guard( m_instanceLock );
        if( m_instance == 0 )
        {
            m_instance = new DatabaseFactory();
        }            
        return *m_instance;
	}

	void DatabaseFactory::removeInstance()
	{
        ThreadGuard guard( m_instanceLock );
        if( m_instance != NULL )
        {
            delete m_instance;
			m_instance = NULL;
        }            
	}

    IDatabase* DatabaseFactory::getDatabase(EDataTypes dataType, EDataActions dataAction)
    {
        FUNCTION_ENTRY("getDatabase");
        // Set up a local variable to point to the database that is either created or found
        IDatabase* theDatabase = NULL;

        ThreadGuard guard( m_getDatabaseLock );

        // get the connection string for this datatype/action
        std::string dbConnection;
        
        // forever loop will exit by exception or when a good db is found
        while(1)
        {
            // If this call fails it will throw something.
            try
            {
                DbConnection::getInstance().getConnectionString(dataType, dataAction, dbConnection);
            }
            catch(DbConnectionFailed&)
            {
                // we exit the forever loop with this throw
                TA_THROW(DatabaseException("Unable to find a working database"));
            }

            // Get the ID of this thread - we want one database connection per thread
            int threadId = Thread::getCurrentThreadId();

            // Look to see if there is already a database connection for this thread
			//<ThreadID: Map<ConnectionString, SimpleDBDatabase>>
            ThreadMap::iterator threadIter ( m_databaseMap.find( threadId ));
            if ( threadIter == m_databaseMap.end() )
            {
                // add new ConnectionMap. Set db* to null untill it's defined
                ConnectionMap cmap;
                cmap.insert( ConnectionMap::value_type( dbConnection, 0 ) );
                threadIter = m_databaseMap.insert(ThreadMap::value_type(threadId, cmap) ).first;
            }

            ConnectionMap::iterator connIter ( threadIter->second.find( dbConnection ) );
            if (connIter == threadIter->second.end())
            {
                connIter = threadIter->second.insert(ConnectionMap::value_type( dbConnection, 0 )).first;
            }
            if (connIter->second != 0 )
            {
                // Then the connection already exists.
	            theDatabase = connIter->second; //TODO: need to set dataType and DataAction
				theDatabase->setDataTypeAction(dataType, dataAction);
            }
            else // Database doesn't exist for this thread, so need to create it.
            {
			    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "New SimpleDbDatabase object created for thread %lu", threadId);
            
	            theDatabase = new TA_Base_Core::SimpleDbDatabase(dataType, dataAction);

                // add to map
                connIter->second = theDatabase;
            }


            // Now that we have a database object, we need to connect it - do this everytime
            // 'cause if the database is alerady connected, nothing will happen, but if it has
            // been inadvertandtly disconnected, then it will be reconnected
            try // The connect line can generate a DatabaseException
            {
                theDatabase->connect( dbConnection );

                // we can also exit the forever loop this (preferred) way
 
                FUNCTION_EXIT;
   		        return theDatabase;
            }
            catch (DatabaseException&) 
            {
                // A DatabaseException was generated. Need to delete the database object for now.
                delete theDatabase;
                threadIter->second.erase(connIter);
                TA_Base_Core::Thread::sleep(100);
                // do not rethrow, just sleep for a while
                // there may be another database we can talk to.
            }
        } // end forever loop
	}


    IDatabase* DatabaseFactory::getDatabase(EDataTypes dataType, EDataActions dataAction, IDatabase *theDatabase )
    {
        FUNCTION_ENTRY("getDatabase");
        ThreadGuard guard( m_getDatabaseLock );

        // get the connection string for this datatype/action
        std::string dbConnection;
        
        // forever loop will exit by exception or when a good db is found
        while(1)
        {
            // If this call fails it will throw something.
            try
            {
                DbConnection::getInstance().getConnectionString(dataType, dataAction, dbConnection);
            }
            catch(DbConnectionFailed&)
            {
                // we exit the forever loop with this throw
                throw(DatabaseException("Unable to find a working database"));
            }

            // The only reason to call this function is when a db connection has gone bad
            // We therfore try to connect every time
            try // The connect line can generate a DatabaseException
            {
                theDatabase->connect( dbConnection );

                // we can also exit the forever loop this (preferred) way
                FUNCTION_EXIT;
    		    return theDatabase;
            }
            catch (DatabaseException&) 
            {
                // do not rethrow.
                // there may be another database we can talk to.
            }
        } // end forever loop
	}


    void DatabaseFactory::cleanup()
    {
        FUNCTION_ENTRY("cleanup");
        TA_Base_Core::ThreadGuard guard( m_getDatabaseLock );

        // set up iterators to step through the database map.
        ThreadMap::iterator threadIter (m_databaseMap.begin());
        ThreadMap::iterator threadIterEnd (m_databaseMap.end());

        // step through member map
        while (threadIter != threadIterEnd)
        {
            ConnectionMap::iterator iter (threadIter->second.begin());
            ConnectionMap::iterator iterEnd (threadIter->second.end());
            while (iter != iterEnd)
            {
                if (iter->second != NULL)
                {
                    delete iter->second;
                    iter->second = NULL;
                }               
                iter++;
            }
            threadIter++;
        }

        FUNCTION_EXIT;
    }

    void DatabaseFactory::delDatabase( EDataTypes dataType, EDataActions dataAction )
    {
        FUNCTION_ENTRY("delDatabase(EDataTypes,EDataActions)");

        IDatabase* pDatabase = NULL;

        {
            ThreadGuard guard( m_getDatabaseLock );

            // get the connection string for this datatype/action
            std::string dbConnection;

            // If this call fails it will throw something.
            try
            {
                DbConnection::getInstance().getConnectionString(dataType, dataAction, dbConnection);
            }
            catch(DbConnectionFailed&)
            {
                // we exit the forever loop with this throw
                TA_THROW(DatabaseException("Unable to find a working database"));
            }

            // Get the ID of this thread - we want one database connection per thread
            int threadId = Thread::getCurrentThreadId();

            // Look to see if there is already a database connection for this thread
            ThreadMap::iterator threadIter = m_databaseMap.find( threadId );
            if ( threadIter == m_databaseMap.end() )
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "No SimpleDbDatabase object was found for thread %lu", threadId);
                TA_THROW(DatabaseException("Unable to find the IDatabase"));
            }

            ConnectionMap::iterator connIter = threadIter->second.find( dbConnection );
            if (connIter == threadIter->second.end())
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "No SimpleDbDatabase object was found for DBconnection string %s", dbConnection.c_str());
                TA_THROW(DatabaseException("Unable to find the IDatabase"));
            }

            pDatabase = connIter->second;
            threadIter->second.erase(connIter);
            if ( threadIter->second.empty() )
            {
                m_databaseMap.erase( threadIter );
            }

            if ( NULL == pDatabase )
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Unexpected  to be deleted NULL pointer of IDatabase");
            }
        }

        if ( NULL != pDatabase )
        {
            delete pDatabase;
            pDatabase = 0;
        }

        FUNCTION_EXIT;
    }

    void DatabaseFactory::delDatabase( IDatabase* pDatabase )
    {
        FUNCTION_ENTRY("delDatabase(IDatabase*)");

        if ( NULL == pDatabase )
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Invalid input parameter that IDatabase is NULL pointer");
            return;
        }

        bool bExist = false;

        {
            ThreadGuard guard( m_getDatabaseLock );

            // set up iterators to step through the database map.
            ThreadMap::iterator threadIter (m_databaseMap.begin());
            ThreadMap::iterator threadIterEnd (m_databaseMap.end());

            // step through member map
            while (threadIter != threadIterEnd)
            {
                ConnectionMap::iterator iter (threadIter->second.begin());
                ConnectionMap::iterator iterEnd (threadIter->second.end());
                while (iter != iterEnd)
                {
                    if (iter->second == pDatabase)
                    {
                        threadIter->second.erase( iter );
                        if ( threadIter->second.empty() )
                        {
                            m_databaseMap.erase( threadIter );
                        }
                        bExist = true;
                        break;
                    }               
                    iter++;
                }

                if ( bExist )
                {
                    break;
                }

                threadIter++;
            }
        }

        if ( bExist )
        {
            delete pDatabase;
            pDatabase = NULL;
        }
        else
        {
            TA_THROW(DatabaseException("Unable to find the IDatabase"));
        }

        FUNCTION_EXIT;
    }

	std::string DatabaseFactory::getDatabaseName(const std::string& strFileName, const EDataTypes dataType, const EDataActions action)
	{
		FUNCTION_ENTRY("getDatabaseName()");
		std::string strDbName;
		DbNameStrMap::const_iterator cIter;

		ThreadGuard guard( m_getDbNameLock );
		cIter = m_DbStrMap.find(strFileName);
		if ( cIter != m_DbStrMap.end() )
		{
			// already in the map
			DbConnectionStrings *pDbConStr = cIter->second;
			DataConnections connections = pDbConStr->getConnectionList(dataType, action);
			if ( connections.size() > 0 ) 
				strDbName = connections[0].first;
			else
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
					"Cannot find the DB name in file: %s", strFileName.c_str());
		}
		else // the db connection file changed.
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Database file changed, fileName: %s", strFileName.c_str());
			DbConnectionStrings *pDbConnStr = new DbConnectionStrings(strFileName);
			DataConnections connections = pDbConnStr->getConnectionList(dataType, action);
			if ( connections.size() > 0 )
				strDbName = connections[0].first;
			else
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
					"Cannot find the DB name in file: %s", strFileName.c_str());

			m_DbStrMap.insert(DbNameStrMap::value_type(strFileName, pDbConnStr));
		}

		FUNCTION_EXIT;
		return strDbName;
	}
} // Closes TA_Base_Core

