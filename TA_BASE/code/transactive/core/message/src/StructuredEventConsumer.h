#if !defined(StructuredEventConsumer_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
#define StructuredEventConsumer_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/StructuredEventConsumer.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:12:25 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * The StructuredEventConsumer is used to receive events from an event channel.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include <vector>
#include "core/message/src/TA_CosNotifyChannel.h"
#include "core/message/src/DispatchingQueue.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

// TODO: Move SubscriptionFilter into a new header/cpp, then remove these #includes
#include "core/message/src/SubscriptionManager.h"

// PRIVATE_TO_PACKAGE

#include "COS/CosNotification.hh"
#include <boost/shared_ptr.hpp>


namespace TA_Base_Core
{
    // The StructuredEventConsumer is used to receive events from multiple redundant 
    // event channels (serviceAddr, channelName).  It is for LOCAL use as it uses the
    // default consumer admin.

    // The primary features are:
    // - maintains connections to multiple event channels as a SequencePushConsumer.  
    //   Events can be received on ANY of these channels.  This is intended to provide
    //   redundancy.  Note that the StructuredEventConsumer does NOT require the channels
    //   to be related at all (eg. the channels don't have to be redundant).
    // - sets up a filter on the server side (ie. at the EventChannel) that only forwards
    //   an event to the StructuredEventConsumer if it satisfies the filter's criteria.
    // - pushes events that are received on the client side into a queue so that delivery
    //   of events by the EventChannel (and flushing of it's queues) is not interrupted 
    //   by slow processing of events by the client.  
    // - events on the queue are pushed to handlers by a separate thread (providing the
    //   events satisfy the client-side filter criteria, if there is one).

    // The components used by the StructuredEventConsumer are as follows:
    // ----       CNCA::EventChannel ===> CNF::Filter
    // | |                |                     |
    // | |                v                     |
    // | --    CosNotifyChannelConsumer   RefCountedConstraintSet 
    // |             \    |    /
    // |              \   |   /     receives on ALL connections
    // |               v  v  v         
    // |--     StructuredEventConsumer
    // |                  |                  
    // |                  v       multiple threads
    // |            BatchingQueue  
    // |                  |       single thread
    // |                  v
    // | --       SECQueueHandler       
    // | |                |
    // | |                v
    // | |          SeqNumFilter
    // | |                |  
    // | |                v              
    // | | --    DomainNameDispatcher     
    // | | |              |                
    // | | |              v 
    // | | | --   TypeNameDispatcher
    // | | | |            |     client side filter (IMessageFilter)
    // | | | |            v
    // --------    IEventHandler   (eg. AbstractMessageSubscriber, SpecialisedMessageSubscriber)                

    // TODO LPP: Why is it necessary to use the DomainNameDispatcher or TypeNameDispatcher
    // if each StructuredEventConsumer has a matching set of subscriptions?  Probably need
    // to understand the matching algorithm better. Could be related to the segmentation of 
    // entity keys.

    class SECQueueHandler;

	class StructuredEventConsumer : public IEventHandler // for receiving events from the CosNotifyChannelConsumer
	{
	public:
        // adminProvider maps from an event channel (provided by onChannelAvailable) to the
        // ConsumerAdmin object needed for connecting to the event channel.
		StructuredEventConsumer(/*takes*/ IAdminProvider* adminProvider );
		virtual ~StructuredEventConsumer();

        // SUBSCRIPTION MANAGEMENT INTERFACE
        // Register a subscriber with this StructuredEventConsumer.  The handler will be called 
        // back when a message satisfying the SubscriptionFilter is received.
        // false is returned if the subscriber is not successfully attached - this will happen
        // if 'filter' is not compatible with any subscriptions that have already been attached.
        // handler - The IEventHandler that should be called when a message satisfying the filter
        //           criteria for the subscription is dispatched.  Must not be NULL.  DO NOT call 
        //           detachSubscriber() from within the handler.
		// filter  - The criteria to evaluate a message against.  Must not be NULL.  NOTE that all 
        //           of the event types for a given filter must have the same domain_name d (or *).

        // TODO LPP: Specify precisely what 'compatible' means.  What restrictions exist on 
        // SubscriptionFilters that are added to the SAME StructuredEventConsumer?
		bool attachSubscriberIfCompatible( IEventHandler* handler, SubscriptionFilter* filter );

		// Remove the message subscriber from this StructuredEventConsumer.  Note that the subscriber
        // is identified by the filter rather than the handler (since a given handler may be used for
        // multiple subscriptions).
		// true is returned if the subscriber was successfully removed (false will be returned if 
        // a subscriber with that filter has not been attached).
        // filter  - The filter associated with the subscriber that is being detached. 
		bool detachSubscriber( SubscriptionFilter* filter );

        // returns true if the consumer has one or more subscribers, false otherwise.
		bool hasSubscribers() const;

        // CONNECTION MANAGEMENT INTERFACE
        // Connections are maintained to the event channels that are added using addConnection().
        // Events can be received on any of these channels.  A connection is identified by it's
        // serviceAddr ("host:port").
        // NOTE that removeDeadConnection() doesn't try to disconnect cleanly from the event channel.
        // It should be called when the connection is known to have failed.
		bool removeDeadConnection( const std::string& serviceAddr );
		bool addConnection( const std::string& serviceAddr,
                            const CosNotifyChannelAdmin::EventChannel_ptr channel );

        // Disconnects the StructuredEventConsumer cleanly from all valid (ie. active) connections.
        // Should be called before destruction.
		void disconnectAllEventChannels();

        // Implementation of IEventHandler
    	virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events);

	protected:
        // NOT CLONEABLE
		StructuredEventConsumer( const StructuredEventConsumer& theStructuredEventConsumer);

        // THREAD SAFETY:
        // We use a single lock to protect both connection and subscription management state,
        // because there are several methods that need to access both (although this does
        // harm concurrency of the methods that only need to access part of the state).  
		mutable TA_Base_Core::NonReEntrantThreadLockable m_lock;

        // CONNECTION MANAGEMENT
	    struct ConsumerChannelConnection
	    {
            CosNotifyChannelConsumer m_channelConsumer;
            RefCountedConstraintSet m_constraintSet;
	    };

        // serviceAddr -> ConsumerChannelConnection
		typedef std::map<std::string, ConsumerChannelConnection*> ChannelConnections;
		ChannelConnections m_connections;

        // SUBSCRIPTION MANAGEMENT
        // Store the list of subscription filters, used when a connection is added (so all of the 
        // ProxySupplier filters can be set up) and to allow hasSubscribers() to determine whether
        // any subscribers are attached.  There is exactly one entry for every subscription
        // added with attachSubscriberIfCompatible().
        std::vector<SubscriptionFilter*> m_subscriptions;

        // Returns true if the filter's ConstraintExp is compatible with the StructuredEventConsumer's
        // existing constraints.
		bool matchConstraint( SubscriptionFilter* filter ) const;
		
		// Adds the constraint expressions associated with the subscription filters (ie. m_subscriptions)
        // to the given constraint set (and therefore to the Filter associated with m_channelConsumer).
        // This should only be called when a connection is initially added.
        // false is returned if the constraints were not successfully added.
		bool addAllExpressionsToConstraintSet(RefCountedConstraintSet& constraintSet);

        // EVENT DISPATCHING
        // BatchingQueue<CosNotification::StructuredEvent> m_queue;
        boost::shared_ptr< BatchingQueue<CosNotification::StructuredEvent> > m_queue;

        // m_handler is the IQueueHandler<T> for m_queue.  It dispatches events one-at-a-time 
        // to the relevant subscribers.
        std::auto_ptr<SECQueueHandler> m_handler;

        std::auto_ptr<IAdminProvider> m_adminProvider;
	};

}; // TA_Base_Core

#endif // !defined(StructuredEventConsumer_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
