#if !defined(SubscriptionManager_67605B83_BD33_46b1_A08D_57083840C03B__INCLUDED_)
#define SubscriptionManager_67605B83_BD33_46b1_A08D_57083840C03B__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/SubscriptionManager.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Classes related to managing subscriptions.
  * Clients will usually interact with the SubscriptionManager rather than 
  * directly with the StructuredEventConsumer.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <vector>
#include <utility>
#include <map>
#include "COS/CosNotifyFilter.hh"
#include "core/message/src/TA_CosNotifyInterfaces.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class IMessageFilter;

    // The purpose of SubscriptionFilter is to store information related to filtering
    // of events.  Subscribers use it to specify the set of event types that they are
    // interested in receiving.  Both server-side and client-side filtering is supported.
    // The following filter-related data is represented:
    // - sequence of event types of interest and a constraint expression that must be
    //   satisfied by events of those types, which is used by the server-side filter.
    //
    // - IMessageFilter, used to filter events on the client-side if the server-side event
    //   channel filter has passed them.
    //
    // - constraint ID, used for sharing of filter constraints.  If multiple subscriptions
    //   share the same filter criteria, the corresponding filter constraint will be shared
    //   by all of those subscriptions, with a reference count to determine when to remove the
    //   constraint from the filter.  The ID is used to tag SubscriptionFilter objects that
    //   share the same constraint.

    class SubscriptionFilter
    {
    public:

        /**
          * SubscriptionFilter
          *
          * The constructor will construct a SubscriptionFilter object that describes the filtering details
          *
          * @param CosNotification::EventTypeSeq& event_types - can be retrieved using getFilterCriteria().event_types
          * @param std::string& constraintExpr - can be retrieved using getFilterCriteria().constraint_expr
          * @param IMessageFilter* - the client side filter (can be NULL)
          */
        SubscriptionFilter( const CosNotification::EventTypeSeq& event_types,
                            const std::string& constraintExpr,
                            IMessageFilter* clientSideFilter );

        virtual ~SubscriptionFilter();

        // SERVER-SIDE FILTERING

        // Get the constraint expression for this subscriber.  ConstraintExp contains two
        // members, event_types (the sequence of events that are of interest to the subscriber)
        // and constraint_expr (the string expression that each event must also satisfy).
        const CosNotifyFilter::ConstraintExp& getFilterCriteria( ) const
        {
            return m_serverSideFilter;
        }

        // Returns the unique (in this process) ID of this constraint.  The ID of different
        // SubscriptionFilter objects can be compared to determine whether they represent
        // the same server-side filter, without having to compare the constraints (providing
        // setID() has been called by the client on one of the SubscriptionFilter objects
        // when the server side filters match).
        unsigned int getID() const;

        // Set the id of this constraint to be the same as another.
        void setID( unsigned int id )
        {
            m_id = id;
        }

        // TODO LPP: A more direct and clear approach might be to provide a method that
        // sets the constraint id from another SubscriptionFilter if the constraints match.
        // ie. it both compares the filter criteria and sets the constraint id.
        // Ideally, there would not be a need to provide a constraint id.

        // CLIENT-SIDE FILTERING
        // This function will determine whether the given message matches the installed filter
        // criteria if a local (client-side) filter has been set, otherwise will return true.
        bool messageMatchesFilter( const CosNotification::StructuredEvent& event ) const;

     private:
        CosNotifyFilter::ConstraintExp m_serverSideFilter;
        mutable unsigned int m_id;    // 0 == not initialised
        IMessageFilter* m_clientSideFilter;

        // NOT CLONEABLE
        SubscriptionFilter(const SubscriptionFilter&);
        SubscriptionFilter& operator=(const SubscriptionFilter&);
    };


/////////////////////////////////////////////////////////////////////////////////////////

    class SubscriptionManager;


    // This by-value class can be used to manage one or more subscriptions to a SubscriptionManager.
    // It allows the common pattern of subscribing to multiple channels one at a time, then later
    // unsubscribing from all channels at once, to be implemented easily.
    class SubscriptionFilterSet
    {
    public:
        // Uses the SubscriptionManager 'manager' to subscribe, and adds the SubscriptionFilter
        // object 'subFilter' to the set of subscriptions that will later be unsubscribed from.
        void addSubscription( SubscriptionManager* manager,
                              const std::string& channelName,
                              /*takes*/ SubscriptionFilter* subFilter,
                              unsigned long supplierLocationKey,
                              IEventHandler* handler );

        // Unsubscribes all subscriptions that were added by addSubscription, and deletes the
        // SubscriptionFilter objects.  An explicit call to removeAllSubscriptions() is required
        // before the SubscriptionFilterSet is destroyed.
        void removeAllSubscriptions();

        // Checks that all subscriptions have been removed.
        ~SubscriptionFilterSet();

    private:
        // SubscriptionFilterSet Subscription Detail
        struct SFSSubDetail
        {
            SFSSubDetail(SubscriptionFilter* subFilter, SubscriptionManager* manager)
                : m_subFilter(subFilter), m_manager(manager)
            {
            }

            SubscriptionFilter* m_subFilter;
            SubscriptionManager* m_manager;
        };

        std::vector<SFSSubDetail> m_subscriptions;

        TA_Base_Core::NonReEntrantThreadLockable m_subscriptionsLock;

    };


/////////////////////////////////////////////////////////////////////////////////////////

    class StructuredEventConsumer;
    class FederationHandler;

    // LOCATION-BASED AND BROADCAST SUBSCRIPTIONS
    // Location-based subscriptions need to specify which location the events of interest originate
    // from - this allows the local location to federate with the remote event source (note that
    // this is independent of any location filtering specified in the SubscriptionFilter).
    // A consequence of this is that a consumer will only receive events from those locations that
    // it has explicitly subscribed to.

    // A location-independent subscription allows a consumer to receive events from ANY location
    // without having to explicitly subscribe to it.  The framework does this by federating with
    // all locations so events published by a supplier at any of the locations will be forwarded.
    // This mechanism allows suppliers to 'broadcast' events - it doesn't matter which location
    // the event was sent from, it will be forwarded to all consumers that have subscribed for
    // that type of event on the particular event channel.
    // NOTE: Location-independent subscriptions don't scale to large numbers of locations very well.

    // Special values for use as supplierLocationKey in SubscriptionManager::subscribeToMessages():

    // Use as the supplierLocationKey to indicate that events of interest originate from
    // the local location (ie. the same location as the consumer).
    // TODO LPP: LOCAL_LOCATION should probably have the value ULONG_MAX, since it is unsigned long.
    const unsigned long LOCAL_LOCATION = LONG_MAX;

    // Use as the supplierLocationKey to indicate that events of interest can originate
    // from ANY location.  This specifies a LOCATION-INDEPENDENT subscription - all other
    // subscriptions are location-dependent.
    const unsigned long ANY_LOCATION = 0;


    // Manages subscriptions to a set of StructuredEventConsumers (SECs).  Subscribers
    // are assigned to a particular SEC depending on event channel, message source
    // location key, and server-side subscription filtering details. SECs are created
    // and destroyed on demand.
    class SubscriptionManager
    {
    public:
        SubscriptionManager();
        virtual ~SubscriptionManager();

        // Clean up the StructuredEventConsumers.  This MUST be called before the SubscriptionManager
        // is destroyed.
        void cleanUpConsumers();

        /**
        * subscribeToMessages
        *
        * Allows a client to receive messages on a given channel, from a particular source
        * location, satisfying the given server- and client-side filters.
        *
        * @param    channelName         Name of the event channel to subscribe to
        * @param    subFilter           Identifies server- and client-side filters.  This is also used
        *                               as a key for unsubscribing.
        * @param    supplierLocationKey The source of events, used to establish federation with the
        *                               correct event source.  The special values LOCAL_LOCATION and
        *                               ANY_LOCATION can be used.  It is unrelated to the subFilter
        *                               (however, the subFilter must not use a location-based filter
        *                               criteria if ANY_LOCATION is specified, otherwise events will be filtered out).
        * @param    handler             The IEventHandler that should be called when an event satisfying
        *                               all filter criteria for the subscription is dispatched.
        */
        void subscribeToMessages( const std::string& channelName,
                                  SubscriptionFilter* subFilter,
                                  unsigned long supplierLocationKey,
                                  IEventHandler* handler );

        // Allows a client that has subscribed to messages (using subscribeToMessages)
        // to unsubscribe.
        // TODO LPP: Are we required to unsubscribe if we have subscribed?
        void unsubscribeToMessages( SubscriptionFilter* subFilter );

    private:
        // NOT CLONEABLE
        SubscriptionManager( const SubscriptionManager& theSubscriptionManager);

        // StructuredEventConsumer  <===  FederationHandler  <===  ChannelLocatorConnectionMgr notifications
        //          |                             |
        //          v                           calls IChannelLocator::federateIfRemoteSource
        //    IEventHandler

        struct ConsumerInfo
        {
            ConsumerInfo(StructuredEventConsumer* consumer, FederationHandler* fedHandler, unsigned long supplierLocationKey);

            StructuredEventConsumer* m_consumer;
            FederationHandler* m_fedHandler;
            unsigned long m_supplierLocationKey;
        };

        // event channel name -> ConsumerInfo*
        // Each StructuredEventConsumer and FederationHandler will only occur once in this map.
        typedef std::multimap<std::string, ConsumerInfo> ChannelToConsumerMap;
        ChannelToConsumerMap m_channelToConsumerMap;

        // Reverse lookup so that unsubscribeToMessages can remove the subscription
        // from the appropriate StructuredEventConsumer, and delete the StructuredEventConsumer
        // (and the FederationHandler) if there are no other subscribers attached to it -
        // determined by calling SEC::hasSubscribers().
        typedef std::map<SubscriptionFilter*, ConsumerInfo> SubscriptionToConsumerMap;
        SubscriptionToConsumerMap m_subscriptionToConsumerMap;

        TA_Base_Core::NonReEntrantThreadLockable m_subscriberLock;

        // Detaches the given FederationHandler from the ChannelLocatorConnectionMgr and
        // unfederates it if necessary, disconnects the SEC from all valid event
        // channels, and finally terminates/destroys the SEC.
        void unfederateAndDestroyConsumer(const ConsumerInfo& ci);

    }; // SubscriptionManager


}; // TA_Base_Core

#endif // !defined(SubscriptionManager_67605B83_BD33_46b1_A08D_57083840C03B__INCLUDED_)
