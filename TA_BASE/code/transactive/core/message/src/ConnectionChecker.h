#ifndef CONNECTION_CHECKER_H
#define CONNECTION_CHECKER_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/ConnectionChecker.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <map>
#include <string>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"

// TODO LPP: Rename this file to ChannelLocatorConnectionMgr

namespace TA_Base_Core
{
	class IChannelObserver;
	class ChannelLocatorConnection;

    // Monitors IChannelLocator instances periodically (referred to here as 'connections'),
    // notifying attached observers when instances become available (valid) or unavailable 
    // (invalid).
    // 'Connections' are identified by a serviceAddr "host:port" that is used to form a
    // corba object reference of the form "corbaloc::<host>:<port>/ChannelLocator".

    // The channel locator framework sends notifications when the availability of ANY of the
    // IChannelLocator instances for a given (channelName, locationKey) change.

	class ChannelLocatorConnectionMgr 
	{
	public:
		virtual ~ChannelLocatorConnectionMgr();

        // Get the one and only ChannelLocatorConnectionMgr. The first call to this method will
        // call gAddNotifyHostConnections().
		static ChannelLocatorConnectionMgr& getInstance();

		/**
		* removeInstance
		*
		* Remove the one and only ChannelLocatorConnectionMgr.  This MUST be called before
        * shutdown so that all ChannelLocatorConnection threads are stopped.  For example,
        * this prevents pure virtual function calls occurring when the threads try to log
        * after the logger's static thread guard has been cleaned up.
		*/
		static void removeInstance();

        // Force a particular IChannelLocator instance to be re-resolved.
		void invalidateConnection( const std::string& serviceAddr );

        // Adds an IChannelLocator instance to the list that the ChannelLocatorConnectionMgr
        // monitors.  All observers that have attached for a given (channelName, locationKey)
        // will be notified when the availability of any IChannelLocator which hosts that 
        // channelName and has that locationKey changes.  Note that location is a virtual
        // concept - two IChannelLocator instances can be at the same physical location
        // (ie. on the same host) but have different locationKeys.

        // addConnection() can be called multiple times with the same serviceAddr, but the same
        // locationKey must be specified each time.
        // serviceAddr must NOT be empty.
		void addConnection( const std::string& serviceAddr, unsigned long locationKey );

        // Attaches an IChannelObserver to all ChannelLocatorConnections with the given locationKey
        // so that it receives notifications when any connection with the given (channelName, locationKey) 
        // becomes valid (available) or invalid.
        // TODO LPP: We should require that all observers have detached before the 
        // ChannelLocatorConnectionMgr is destroyed.
        // TODO LPP: What happens if attach() is called with an IChannelObserver that is already attached?
		void attach(IChannelObserver* obs);
		void detach(IChannelObserver* obs);

        void shutdownInProgress() { s_shutdownInProgress = true; }

	private:

        // Private as this is a singleton
		ChannelLocatorConnectionMgr();
		
        // NOT CLONEABLE or ASSIGNABLE
		ChannelLocatorConnectionMgr( const ChannelLocatorConnectionMgr& );
		ChannelLocatorConnectionMgr& operator=(const ChannelLocatorConnectionMgr& );

        struct ConnectionDetail
        {
            ConnectionDetail(ChannelLocatorConnection* connection, unsigned int locationKey)
                : m_connection(connection), m_locationKey(locationKey) {}

            ChannelLocatorConnection* m_connection;
            unsigned int m_locationKey;
        };

        // serviceAddr -> (ChannelLocatorConnection*, locationKey of IChannelLocator)
		typedef std::map<std::string, ConnectionDetail> ConnectionsMap;
		ConnectionsMap m_connectionsMap;
		TA_Base_Core::ReEntrantThreadLockable m_connectionsLock;  // Guard m_connectionsMap

		// Store all the channel observers for a particular location.  This allows ChannelObservers
        // to be attached to ChannelLocatorConnections efficiently when addConnection() is called.
        // Each observer will be added once to this map.
        // location key of IChannelObserver -> IChannelObserver
		typedef std::multimap<unsigned long, IChannelObserver*> LocationToObservers;
		LocationToObservers m_locationToObservers;
		TA_Base_Core::NonReEntrantThreadLockable m_locationsLock;    // Guard m_locationToObservers
		
		static ChannelLocatorConnectionMgr* m_theClassInstance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;  // Guard m_theClassInstance
        static volatile bool s_shutdownInProgress;
	};


    // Parse RPARAM_NOTIFYHOSTS and monitor connections to the IChannelLocator instances on 
    // those hosts by calling addConnection().  The hosts are assumed to be for the local
    // location (ie. gGetLocalLocationKey()). For example, if there are two IChannelLocator 
    // instances listening on ports "localhost:9997" and "localhost:9998", --notify-hosts 
    // would be "localhost:9997, localhost:9998".    
    void gAddNotifyHostConnections(ChannelLocatorConnectionMgr* mgr);

}; // TA_Base_Core

#endif // !CONNECTION_CHECKER_H
