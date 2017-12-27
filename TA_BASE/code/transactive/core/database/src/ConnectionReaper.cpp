/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/ConnectionReaper.cpp $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
*
* This class is used by OCIAccess to regularly refresh the connections
* to avoid having left over cursors using up the allowed number of cursors.
* This class also monitors old connections after the refresh or auto-reconnect
* and deletes the connections when they are no longer used.
*/

#pragma warning(disable:4786) // disable the identifier truncated warning

#include "ConnectionReaper.h"
#include "DbServerLocator.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

namespace TA_Base_Core
{
/** 
* ConnectionReaper
*
* Standard constructor that determines how often the connections
* should be reaped from the RunParams.  The default interval is
* 120 minutes.
	*/
	ConnectionReaper::ConnectionReaper()
		: m_isTerminating( false )
	{
		m_reapIntervalInMins = 0;
		
		// Set the reap interval to the value specified in the RunParams.
		
		if ( TA_Base_Core::RunParams::getInstance().isSet( "DbReapInterval" ) )
		{
			m_reapIntervalInMins = atoi( TA_Base_Core::RunParams::getInstance().get( "DbReapInterval" ).c_str() );
		}
		
		// The valid range for the reap interval is 10 mins to 10080 mins (7 days).
		// If no value is specified or if the value is out of the given range,
		// set the default reap interval to 120 minutes => 2 hours.
        
		if ( ( m_reapIntervalInMins < 10 ) || ( m_reapIntervalInMins > 10080 ) )
		{    
			m_reapIntervalInMins = 120;
		}
		
		LOG( SourceInfo, DebugUtil::GenericLog, 
			DebugUtil::DebugDatabase, "Database connection reap interval: %d minutes", m_reapIntervalInMins  );
	}
	
	
	/** 
	* ~ConnectionReaper
	*
	* Destructor that deletes any left over OCIConnection objects from 
	* the vector of old connections.
	*/
	
	ConnectionReaper::~ConnectionReaper() 
	{
		// Do nothing.
	}
	
	
	/**
	* run
	*
	* Sleeps for the reap interval, then tells the OCIAccess object to 
	* refresh its connections.  This thread then monitors the old connections
	* and deletes them when they are no longer being used.
	*/
	
	void ConnectionReaper::run()
	{
		while( !m_isTerminating )
		{
            try
            {
                m_conditionTimer.timedWait( m_reapIntervalInMins * 60000 );//60000
			    if (m_isTerminating)
			    {
				    return;
			    }
			    
			    // Tell the OCIAccess object to refresh its connections.
			    // It will add the old connections to the map of connections
			    // to be reaped.
			    
			    DbServerLocator::getInstance().refreshConnections();
			    
			    // Loop until there are no more connections to reap.
			    
			    while ( !m_waitingQueue.empty() )
			    {
				    // Retrieve any connections that should be added to the vector 
				    // of monitored connections.
				    
				    monitorConnections();

				    // Delete any connections that are no longer being used
				    // and removes them from the map.
				    
				    deleteUnusedConnections();
				    std::vector< IDbConnection* >::iterator it = m_monitoredConnections.begin();
				    int i = 1;
				    for (;it != m_monitoredConnections.end(); ++it)
				    {
					    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
						    TA_Base_Core::DebugUtil::DebugInfo, "current connections:[%d],  %s" 
						    , i, (*it)->getConnectionString().c_str() );					
					    i++;
				    }				
                    m_conditionTimer.timedWait( 30000 ); // Sleep for half a minute.
				    
				    if ( m_isTerminating )
				    {
					    return;
				    }
			    }
            }
		    catch( const DBException& e )
		    {
			    LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DBException", e.getSummary().c_str() );
		    }
            catch (...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception", "WtOPCSvr library threw an unexpected exception");
            }
		}
	}
	
	
	/**
	* terminate
	*
	* Waits until an appropriate break and then stop running the thread.
	*/
	
	void ConnectionReaper::terminate()
	{
		// Wait until the locks are freed up.  This just guarantees that
		// the thread doesn't terminate between deleting something and
		// erasing it from the vector or while adding/removing items
		// from the vectors.
		
		// scope to limit guard
		{
			ThreadGuard vectorGuard( m_monitoredLock );
			
			// Delete any left over OCIConnection objects from the vector.
			
			std::vector<IDbConnection*>::iterator it = m_monitoredConnections.begin();
			while ( it != m_monitoredConnections.end() )
			{
				delete *it;
				it++;
			}
			
			m_monitoredConnections.clear();
		}
		
		// Delete any left over OCIConnection objects from the queue.
		{
			ThreadGuard queueGuard( m_waitingLock );
			
			std::vector<IDbConnection*>::iterator it = m_waitingQueue.begin();
			while ( it != m_waitingQueue.end() )
			{
				delete *it;
				it++;
			}
			
			m_waitingQueue.clear();
		}
		
		// wake up!  Time to die!
		m_isTerminating = true;
		m_conditionTimer.signal();
	}
	
	
	/**
	* reapConnection
	*
	* Queues connection so that it can be added to the vector
	* of connections that the reaper is monitoring for deletion.
	* The connection is queued so that this method does not block while
	* the existing connections are being deleted.  This is to guard
	* against the case where the vector of monitored connections is
	* locked and it takes 6 minutes or more to delete a connection.
	* 
	* @param connection   an old OCIConnection that should be deleted
	*                     once its execution count is zero - i.e. once
	*                     it is no longer being used.
	*/
	
	void ConnectionReaper::reapConnection( IDbConnection* connection )
	{
		if ( !connection )
		{
			return;
		}
		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 
			TA_Base_Core::DebugUtil::DebugDebug, "Reaping connection: %s" 
            , connection->getConnectionString().c_str() );
		
		// Don't allow connections to be added to the queue while others
		// are being removed.
		
		ThreadGuard guard( m_waitingLock );
		
		// Note that a vector is being used as a queue, because we
		// need to traverse through it.
		
		m_waitingQueue.push_back( connection );
	}
	
	
	/**
	* monitorConnections
	*
	* Moves the connections from the vector of waiting connections
	* to the vector of monitored connections.
	*/
	
	void ConnectionReaper::monitorConnections()
	{
		ThreadGuard queueGuard( m_waitingLock );
		ThreadGuard vectorGuard( m_monitoredLock );
		
		if ( !m_waitingQueue.empty() )
		{
			m_monitoredConnections.insert( m_monitoredConnections.end(),
				m_waitingQueue.begin(),
				m_waitingQueue.end() );
			
			// Don't call delete for the connections on the waiting vector
			// because they have been passed to the monitored connection vector.
			
			m_waitingQueue.clear();
		}
	}
	
	
	/**
	* deleteUnusedConnections
	*
	* Deletes any monitored connections that are not being used
	* any more or have more data to be fetched, but are not the
	* latest version of the connection.
	*/
	
	void ConnectionReaper::deleteUnusedConnections()
	{
		ThreadGuard guard( m_monitoredLock );
		
		std::string connectionStr;
		std::vector< IDbConnection* >::iterator it = m_monitoredConnections.begin();
		std::vector< IDbConnection* >::iterator find_it;
		while ( it != m_monitoredConnections.end() )
		{
			// If a thread is using the connection, leave it.
			
			if ( (*it)->getExecutionCount() > 0 ) 
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "connection still in use: %s" ,
					(*it)->getConnectionString().c_str());
				it++;
				
				continue;
			}
			
			
			// If the connection has statements that have more data
			// to be fetched and it is the latest version of this
			// connection, leave it.
			if ( (*it)->isAnyQueryActive( m_reapIntervalInMins * 60 ))
			//if ( (*it)->hasMore() )
			{
				// Search the rest of the vector for the latest version
				// of this connection.
				connectionStr = (*it)->getConnectionString();
			
				find_it = it + 1;
				while ( find_it != m_monitoredConnections.end() )
				{
					if ( (*find_it)->getConnectionString() == connectionStr )
					{
						// Found a later version.
						
						break;
					}
					find_it++;
				}
				
				if ( find_it == m_monitoredConnections.end() )
				{
					// This is the latest version of this connection,
					// so leave it.
					
					it++;
					continue;
				}
			}
			
			// This connection is no longer in use or it has expired,
			// so delete it. The delete may take 6 minutes or more.
			
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Deleting connection: %s" ,
                (*it)->getConnectionString().c_str() );
			delete *it;
			*it = NULL;
			it = m_monitoredConnections.erase( it ); // must get return iterator
		}
	}
}
