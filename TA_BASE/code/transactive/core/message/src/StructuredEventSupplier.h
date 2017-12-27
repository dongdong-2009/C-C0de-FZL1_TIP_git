#if !defined(StructuredEventSupplier_05031D82_E79E_491e_9DF2_148D09783CDF__INCLUDED_)
#define StructuredEventSupplier_05031D82_E79E_491e_9DF2_148D09783CDF__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/StructuredEventSupplier.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * The StructuredEventSupplier is used to publish events to an event channel.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <memory>

#include "core/utilities/src/TAAssert.h"
#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"
#include "core/message/src/DispatchingQueue.h"
#include "core/message/src/TA_CosNotifyChannel.h"
#include "core/message/src/EventChannelUser.h"

namespace TA_Base_Core
{
    class TimetagedStructuredEvent
    {
    public:
        TimetagedStructuredEvent( CosNotification::StructuredEvent* struct_event, unsigned long timeout)
            : first(struct_event), second(timeout) {};
        ~TimetagedStructuredEvent() { delete first; };

        CosNotification::StructuredEvent* first;
        unsigned long second;
    };

    //typedef std::pair<CosNotification::StructuredEvent*, unsigned long> TimetagedStructuredEvent;
    // The RedundantEventSupplier is a helper class that is used to push events
    // onto multiple redundant event channels.  Events are pushed onto ALL channels
    // where there are consumers attached that are interested in receiving the
    // events, and the channel is known to be active (which means that consumers
    // that receive from multiple redundant channels may need to remove duplicate
    // messages).

    // The primary features are:
    // - connection to multiple *redundant* event channels as a structured event PushSupplier.
    //   It will publish events to ALL channels that are valid.
    // - only sending an event where a subscriber of the EventChannel has registered
    //   interest in the (domain_name, type_name).
    // - forwarding subscription_change() notifications to a client-provided handler.

    // The components used by the RedundantEventSupplier are as follows:

    // --    RedundantEventSupplier
    // |             / | \
    // |            /  |  \      dispatches to ALL valid connections
    // |           v   v   v
    // |     SubscribedEventFilter    <=================> client-provided ISubChangeHandler
    // |               |                       |
    // |               |                       | subscription_change() notifications
    // |               v                       |
    // | -- CosNotifyChannelSupplier ===> SubChangeHandlerFan
    // | |             |
    // | |             v
    // ----    CNCA::EventChannel

    // TODO LPP: The need for a client-provided ISubChangeHandler suggests that the client
    // should also provide an IEventHandler assembly, which would allow the client to assemble
    // the pipeline itself.  This would be more like the IEventHandler pipeline design as
    // originally proposed.  This all needs further thought.

    class RedundantEventSupplier
    {
    public:
        // The destructor will attempt to disconnect cleanly from all channels
        // (all channels should be valid, because removeChannel() removes the channel
        // from m_connections).
        RedundantEventSupplier();
        ~RedundantEventSupplier();

        // true is returned if the batch was dispatched successfully to AT LEAST ONE
        // consumer channel.
        bool publishEvents(const std::vector<TA_Base_Core::TimetagedStructuredEvent*>& batch);

        // The client should call these methods when an event channel's validity changes.
        // 'serviceAddr' is used as a key to identify the channel.
        // 'subChangeHandler' will be notified when the EventChannel sends subscription_change()
        // (the SubscribedEventFilter will also be notified).  subChangeHandler can be NULL.
        bool addChannel( const std::string& serviceAddr,
                         const CosNotifyChannelAdmin::SupplierAdmin_ptr supplierAdmin,
                         /*takes*/ ISubChangeHandler* subChangeHandler );

        // removeChannel() can be called with a serviceAddr that hasn't been added.
        void removeChannel( const std::string& serviceAddr );

    private:
        // Maintain information about each event channel added with addChannel.
        // Each connection needs it's own filter because subscription_change notifications must be
        // independently tracked.
        struct SupplierChannelConnection
        {
            CosNotifyChannelSupplier m_channelSupplier;
            SubscribedEventFilter m_filter;
            std::auto_ptr<ISubChangeHandler> m_subChangeHandler;   // client-provided handler
            SubChangeHandlerFan m_fan;      // fans to m_filter and m_subChangeHandler (if not NULL)
        };

        // Copy all events in 'batch' that are subscribed to (ie. those that satisfy
        // the SubscribedEventFilter 'filter') into 'filteredBatch'.
        void filterSubscribedEvents( const std::vector<TA_Base_Core::TimetagedStructuredEvent*>& batch,
                                     std::vector<const CosNotification::StructuredEvent*>& filteredBatch,
                                     const SubscribedEventFilter& filter );

        // serviceAddr -> SupplierChannelConnection
        typedef std::map<std::string, SupplierChannelConnection*> ChannelConnections;
        ChannelConnections m_connections;
        TA_Base_Core::NonReEntrantThreadLockable m_connectionsLock;

		int m_countNoConn;//++RenQiang, no connection counter used in publishEvents()
    };


    struct ISubChangeHandlerFactory
    {
        // Must not return NULL.
        virtual /*gives*/ ISubChangeHandler* createInstance() = 0;
        virtual ~ISubChangeHandlerFactory() {}
    };

    // Allows the RedundantEventSupplier to be used with the StructuredEventSupplier
    // (as the IQueueHandler<T>).  Also implements IChannelObserver so that notifications
    // from the ChannelLocatorConnectionMgr can be used to connect and disconnect event
    // channels dynamically.
    class SESQueueHandler : public IQueueHandler<TA_Base_Core::TimetagedStructuredEvent>,
                            public IChannelObserver
    {
    public:
        // Sets the given (channelName, locationKey) so that the handler can be attached to
        // the ChannelLocatorConnectionMgr.  Events will be dispatched to ALL of the valid
        // EventChannels for the given (channelName, locationKey). NOTE that SESQueueHandler
        // doesn't actually do the attach - the client must do this.
        // adminProvider maps from an event channel (provided by onChannelAvailable) to the
        // SupplierAdmin object needed for connecting to the event channel.
        // 'subChangeHandlerFactory' will be used to create a new ISubChangeHandler instance
        // each time a new channel becomes available (the instance is passed directly to
        // RedundantEventSupplier::addChannel).  Can be NULL.
        SESQueueHandler( const std::string& channelName,
                         unsigned long locationKey,
                         /*takes*/ IAdminProvider* adminProvider,
                         /*takes*/ ISubChangeHandlerFactory* subChangeHandlerFactory );

        // Implementation of IQueueHandler<CosNotification::StructuredEvent>
        virtual bool onDispatchObjects(const std::vector<TA_Base_Core::TimetagedStructuredEvent*>& batch);

        // Implementation of IChannelObserver
        // (we will receive notifications for (channelName, locationKey) )
        virtual void onChannelUnavailable( const std::string& serviceAddr );
        virtual bool onChannelAvailable( const std::string& serviceAddr,
                                         const CosNotifyChannelAdmin::EventChannel_ptr channel,
                                         const TA_Base_Core::IChannelLocator_ptr channelLocator );

    private:
        RedundantEventSupplier m_redundantSupplier;
        std::auto_ptr<IAdminProvider> m_adminProvider;
        std::auto_ptr<ISubChangeHandlerFactory> m_subChangeHandlerFactory;
    };


    // The primary features of the StructuredEventSupplier are:
    // - adds a sequence number to the header of the event (eg. this allows the consumer
    //   to discard duplicates received due to redundant sending, as all copies will have
    //   the same sequence number).
    // - pushes events onto a queue, later forwarding the queued events to an arbitrary
    //   IQueueHandler<T> asynchronously in batches (periodically or when the queue is full).
    // - only removes events from the queue if the IQueueHandler<T> successfully processed them.
    // - RPARAM_BATCHSIZE and RPARAM_PACINGINTERVAL set the batching properties of the queue.

    // The StructuredEventSupplier is a convenience for clients.  Most of the behaviour
    // will be provided by QueueHandler<T> implementations.

    // The components used by the StructuredEventSupplier are as follows:
    //
    // --     StructuredEventSupplier   <--- clients call publishEvent()
    // |                 |
    // |                 v
    // |          SeqNumAppender
    // |                 |
    // |                 v       multiple threads
    // |           BatchingQueue
    // |                 |       single thread
    // |                 v
    // --         IQueueHandler<T>

    class StructuredEventSupplier
    {
    public:
        StructuredEventSupplier(/*takes*/ IQueueHandler<TA_Base_Core::TimetagedStructuredEvent>* handler);

        // Terminates the BatchingQueue thread.  Clients can do their termination work
        // in the IQueueHandler's destructor.
        ~StructuredEventSupplier();

        // The events are placed onto a queue and are pushed to the queue handler later,
        // periodically or when the queue becomes full - the events will only be removed
        // from the queue if they are all successfully processed by the queue handler .
        // This method is thread-safe.  A sequence number is appended to the event header.
        void publishEvents( /*takes*/ std::vector<TA_Base_Core::TimetagedStructuredEvent*>& events );

        // Helper for pushing a single event onto the queue.
        void publishEvent( /*takes*/ CosNotification::StructuredEvent& event, unsigned long expiredTime = 0 );

    private:
        // NOT CLONEABLE
        StructuredEventSupplier(const StructuredEventSupplier& theStructuredEventSupplier);

        // We are responsible for deleting the IQueueHandler<T>.
        BatchingQueue<TA_Base_Core::TimetagedStructuredEvent> m_queue;
        std::auto_ptr<IQueueHandler<TA_Base_Core::TimetagedStructuredEvent> > m_handler;

        SeqNumAppender m_seqNumAppender;
    };

}; // TA_Base_Core

#endif // !defined(StructuredEventSupplier_05031D82_E79E_491e_9DF2_148D09783CDF__INCLUDED_)
