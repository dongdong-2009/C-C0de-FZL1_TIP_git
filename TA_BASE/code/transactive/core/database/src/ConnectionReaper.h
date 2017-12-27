/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/database/src/ConnectionReaper.h $
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

#ifndef CONNECTION_REAPER_H
#define CONNECTION_REAPER_H

#include <vector>
#include <queue>
#include "IDbConnection.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"

namespace TA_Base_Core
{
	
	class ConnectionReaper : public TA_Base_Core::Thread
	{
	public:
		
    /**
	* ConnectionReaper
	*
	* Standard constructor that determines how often the connections
	* should be reaped from the RunParams. The default interval is
	* 120 minutes.
		*/
		
		ConnectionReaper();
		
		
		/** 
		* ~ConnectionReaper
		*
		* Destructor that deletes any left over OCIConnection objects from 
		* the vector of old connections.
		*/
		
		~ConnectionReaper();
		
		
		/**
		* run
		*
		* Cycles through the connections in its vector and deletes the ones
		* that are not being used.
		*/
		
		virtual void run();
		
		
		/**
		* terminate
		*
		* Waits until an appropriate break and then stop running the thread.
		*/
		
		virtual void terminate();
		
		
		/**
		* reapConnection
		*
		* Queues connection so that it can be added to the vector
		* of connections that the reaper is monitoring for deletion.
		* The connection is queued so that it can be added to the vector at
		* an appropriate time.
		*
		* @param connection   an old OCIConnection that should be deleted
		*                     once its execution count is zero - i.e. once
		*                     it is no longer being used.
		*/
		
		void reapConnection( IDbConnection* connection );
	
	private:
		
    /**
	* monitorConnections
	*
	* Moves the connections from the vector of waiting connections
	* to the vector of monitored connections.
		*/
		
		void monitorConnections();
		
		
		/**
		* deleteUnusedConnections
		*
		* Deletes any monitored connections that are not being used
		* any more or have more data to be fetched, but are not the
		* latest version of the connection.
		*/
		
		void deleteUnusedConnections();
		
		
		// These are the connections that are being monitored.
		
		std::vector< IDbConnection* > m_monitoredConnections;
		
		// These are the connections that should be added to the 
		// vector of monitored connections in the next pass.  This
		// vector is treated like a queue, however a queue cannot be
		// used because we need to traverse through it.
		
		std::vector< IDbConnection* >  m_waitingQueue;
		
		// This value determines how often the database connections 
		// should be reaped (refreshed).  It is in minutes.
		
		unsigned long m_reapIntervalInMins;
		
		// These locks provide thread safety for adding/removing items
		// from the vectors of monitored and waiting connections.
		
		TA_Base_Core::ReEntrantThreadLockable m_monitoredLock;
		TA_Base_Core::ReEntrantThreadLockable m_waitingLock;
		
		// Used to pause the thread execution so that it can be terminated.
		
		bool m_isTerminating;
		TA_Base_Core::Condition m_conditionTimer;
};


}
#endif // #define CONNECTION_REAPER_HHH
