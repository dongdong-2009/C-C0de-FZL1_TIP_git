#ifndef CONNECTION_THREAD_H
#define CONNECTION_THREAD_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/ConnectionThread.h $
  * @author:  Lachlan Partington
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
#include <vector>
#include <string>
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"
#include "core/message/src/ConnectionChecker.h"

// TODO LPP: Rename this file to ChannelLocatorConnection

// PRIVATE_TO_PACKAGE

namespace TA_Base_Core
{
	class IChannelObserver;

    // Allows observers to receive notifications when a particular IChannelLocator instance 
    // at address "corbaloc::<host>:<port>/ChannelLocator" becomes available (valid)
    // or unavailable (invalid).

    // We fan notifications of IChannelLocator availability out to observers that
    // are interested in EventChannel availability (where a given channel locator
    // can host multiple event channels).

    // The IChannelLocator is available if the object reference 
    // "corbaloc::<host>:<port>/ChannelLocator" can be resolved, narrowed to
    // IChannelLocator, and the methods getChannels() and getID() can be called.

    // NOTE: It is assumed that all of the EventChannels belonging to the IChannelLocator
    // are valid if the IChannelLocator is available.  The availability of EventChannel 
    // instances (or their components) is not checked (this is one reason why 
    // implementors of IChannelObserver may need to return false from onChannelAvailable().

    // NOTE: This worker thread must be started and stopped explicitly by the client.

    // TODO LPP: The state needed for correctly handling the threading in ChannelLocatorConnection 
    // is quite complicated.  This class should be reviewed with the aim of simplifying it.

	class ChannelLocatorConnection : public TA_Base_Core::Thread 
	{
	public:
        // serviceAddr is the "host:port" that the IChannelLocator instance that we are
        // monitoring can be accessed on.
		ChannelLocatorConnection( const std::string& serviceAddr );

        // All observers must be detached before the destructor is called.
		virtual ~ChannelLocatorConnection();

        // Sets the state of the connection to UNAVAILABLE.  This forces the IChannelLocator to
        // be re-resolved.  onChannelUnavailable() will only be called if the state was AVAILABLE.
		void invalidate();

        // Returns true if the IChannelLocator was available the last time availability was checked.
		bool isAvailable();

        // Attach the IChannelObserver so it receives notifications of channel availability 
        // periodically.  
		void attach( IChannelObserver* obs );

        // Detach the IChannelObserver so it no longer receives notifications of channel 
        // availability.
		void detach( IChannelObserver* obs );

		// Implementation of Thread
		virtual void run();			
		virtual void terminate();

	private:
        // CHANNEL LOCATOR RESOLUTION

        // "host:port" of the IChannelLocator instance that will be resolved
		std::string m_serviceAddr;

        // Guards m_channelLocator, m_channelNameToEventChannelMap, m_agentID, m_pendingInvalidate
        // NOTE: The run() method is the only one that is allowed to MODIFY these members,
        // so we don't need to acquire a lock in run() in order to GET their values, only 
        // to SET them.
		mutable TA_Base_Core::NonReEntrantThreadLockable m_resolveLock;

        // The connection to the channel locator (eg. NotificationServiceAgent).  This also
        // indicates the last known availability state: 
        // - nil() indicates that the IChannelLocator wasn't resolved successfully 
        //   the last time it was checked.  The current state is UNAVAILABLE.
        // - non-nil() indicates that the IChannelLocator was resolved successfully.
        //   The current state is AVAILABLE.
		TA_Base_Core::IChannelLocator_var m_channelLocator;

        // Channels provided by the IChannelLocator that is being monitored.  NOTE that the
        // channel names in this map (but not the EventChannel_var) are also used AFTER the 
        // IChannelLocator becomes unreachable.
        // channel name -> Corba event channel
		typedef std::map<std::string, CosNotifyChannelAdmin::EventChannel_var> ChannelNameToEventChannelMap;
		ChannelNameToEventChannelMap m_channelNameToEventChannelMap;

        // Used to ensure that the same agent servant that was active when m_channelLocator was
        // resolved is still active. If the agent has been restarted, all of the event channels' 
        // state will have been reset.  Proxy consumers and suppliers, filters, etc will have 
        // to be set up again, so we treat the channel locator as unreachable in this case.
        // ONLY valid if m_channelLocator is not nil.
		std::string m_agentID;

        // Set to true when invalidate() is called.  Invalidation will occur during the next
        // iteration through the run() loop.  
        volatile bool m_pendingInvalidate;

        // true if the thread is running, setting to false requests the thread to shut down.
		volatile bool m_running;
        volatile bool m_terminated;

        // Condition for timed waiting and signalling
		Condition m_condition;

        // Returns true if channelLocator is currently AVAILABLE (ie. the same IChannelLocator
        // with the agent ID 'prevAgentID' is still reachable).
        bool isChannelLocatorReachable( const std::string& corbalocUrl,     // used for debug logging
                                        IChannelLocator_ptr channelLocator, 
                                        const std::string& prevAgentID ) const;

        // Attempts to resolve the IChannelLocator instance at corbalocUrl.  false will be 
        // returned if the channel locator can't be resolved (none of the out args will be
        // valid).  No exceptions will be propagated.
	    bool resolveChannelLocator( const std::string& corbalocUrl,
                                    IChannelLocator_out channelLocator,                              // out
                                    ChannelNameToEventChannelMap& channelNameToEventChannelMap,      // out
                                    std::string& agentID ) const;                                    // out

        // Returns the names of channels in 'map'.
        static std::string getChannelNamesStr(ChannelNameToEventChannelMap& map);


        // CHANNEL OBSERVER NOTIFICATION

        // true if observers have been added to the queue (attached) since the last merge.  If true,
        // the channel locator state will be checked immediately and notifications dispatched.
        bool m_attachPending;

        // DEADLOCK AVOIDANCE: 
        // Deadlock is possible whenever there are two locks that different threads try to
        // acquire in different orders.
        // We have to decouple attach() and detach() from dispatchNotifications() so that
        // IChannelObserver implementations can make (possibly remote) calls to attach()
        // and detach() from a different thread without there being a risk of deadlock.
        
        // Calls onChannelUnavailable() on all observers that have previously been sent the
        // onChannelAvailable() notification if m_channelLocator is nil(), otherwise calls 
        // onChannelAvailable() on all observers where m_haveValidated is false.
        // Updates m_haveValidated.
        // Must only be called by run() (unless m_resolveLock is acquired).

        // TODO LPP: It is possible for the observer to receive onChannelUnavailable() BEFORE
        // onChannelAvailable() if the IChannelLocator is available at the time it is attached,
        // but then becomes unavailable before dispatchNotifications() is called.
        void dispatchNotifications();

        // attach() and detach() add 'commands' to m_obsChangeQueue which the run() thread 
        // processes periodically and merges changes in FIFO order into m_channelToObserverDetailMap
        // by calling mergeObserverChanges().
        void mergeObserverChanges();
        
        struct ObserverChangeDetail
        {
            ObserverChangeDetail(IChannelObserver* obs, bool hasAttached);

            IChannelObserver* m_observer;

            // true if attach() was called, false if detach() was called
            bool m_hasAttached;

            // Particularly necessary when detach() is called, because the observer may have been
            // deleted by the time we remove it from m_channelToObserverDetailMap and we wouldn't
            // be able to dereference it to determine the channel name.  We have to obtain the
            // channel name when we know the observer is still valid.
            std::string m_channelName;
        };

        typedef std::vector<ObserverChangeDetail> ObsChangeQueue;
        ObsChangeQueue m_obsChangeQueue;

        struct ObserverDetail
        {
            ObserverDetail(IChannelObserver* obs)
                : m_observer(obs), m_haveValidated(false), m_haveDetached(false) {}

            IChannelObserver* m_observer;

            // Relevant only if the channel locator is available.  IChannelObserver::onChannelAvailable() 
            // will only be called if m_haveValidated is false.
            bool m_haveValidated;

            // It is reasonable that a client that detaches an observer would NOT expect that observer
            // to receive any more notifications.  We have to be careful when dispatching notifications
            // that we don't send notifications to observers that have been detached, and that we don't
            // return from detach() while a notification is being dispatched to the observer being 
            // detached.  NOTE that this can probably lead to a deadlock if the observer detaches ITSELF 
            // in it's own handler (but from another thread).
            // TODO LPP: This is all very complicated.  There must be an easier way.

            // true if this observer has been detached (can be set by detach() during dispatchNotifications).
            // NOTE that it will be false if the observer has been attached AND detached during a single
            // call to mergeObserverChanges.
            bool m_haveDetached;

            // Protects m_haveDetached
            // TODO LPP: We shouldn't have to have a separate lock for every observer.
    		NonReEntrantThreadLockable m_detachLock;
        };

        // The elements in m_channelToObserverDetailMap are only added or removed by the run() thread,
        // but the m_haveDetached member of ObserverDetail can be modified by detach().
        // A given IChannelObserver will only appear once in this map.
        // channel name -> (IChannelObserver*, need to validate)
		typedef std::multimap<std::string, ObserverDetail*> ChannelNameToObserverDetailMap;
		ChannelNameToObserverDetailMap m_channelToObserverDetailMap;

        // Returns an iterator to the entry in m_channelToObserverDetailMap with m_observer == obs.
        // Note that obs is not dereferenced (because it may have been destroyed already).
        ChannelNameToObserverDetailMap::iterator findObserver(IChannelObserver* obs, const std::string& channelName);

        // Protects m_obsChangeQueue and m_channelToObserverDetailMap
		TA_Base_Core::NonReEntrantThreadLockable m_observersLock;

        mutable bool m_resolvedLastTime;
	};

}; // TA_Base_Core

#endif // !CONNECTION_THREAD_H
