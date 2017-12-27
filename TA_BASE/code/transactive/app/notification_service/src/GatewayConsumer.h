#ifndef GATEWAY_CONSUMER_H
#define GATEWAY_CONSUMER_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/GatewayConsumer.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/12/17 15:24:44 $
  * Last modified by:  $Author: shurui.liu $
  * 
  */

// TODO LPP: Rename file to FederationForwarder.h

#include "core/message/src/StructuredEventSupplier.h"
#include "core/corba/IDL/src/IFederatableEventChannel.hh"


namespace TA_Base_App
{
    // The FederationForwarder is used by the supplier-side NS.  It connects an
    // event channel on the (local) supplier side to multiple redundant event channels
    // on the (remote) consumer side.

    // The primary features are:
    // - connection to a single event channel (on the local supplier side) as a PushConsumer.
    //   The local event channel is not monitored for availability (it is assumed to be
    //   always available as it is in-process).
    // - connection to multiple redundant event channels (on the remote consumer side) using
    //   the StructuredEventSupplier. The redundant channels must all have the same
    //   (channelName, consumerLocationKey).
    // - listens for subscription_change() notifications from the remote consumer side event
    //   channels and modifies filters (there is a separate filter corresponding to each remote
    //   consumer) on the local supplier event channel so that these notifications will be
    //   propagated to suppliers of the local channel.

    // The components used by the FederationForwarder are as follows:
    //
    // ----               CNCA::EventChannel ===> CNF::Filter             (local) supplier event channel
    // | |                        |                    |
    // | |                        v                    |
    // | --            CosNotifyChannelConsumer   RefCountedConstraintSet
    // |                          |                           ^ ^ ^
    // |                          |                          /  |  \
    // |                          |                         /   |   \
    // |                          v                             |
    // |--               FederationForwarder           FFSubChangeHandler ----- EventTypeSet
    // |                          |                             ^
    // |                          |                             |
    // |                          v                             |
    // | --            StructuredEventSupplier                  |
    // | |                        |                             |
    // | |                        v                             |
    // | |                 SeqNumAppender                       |
    // | |                        |                             |
    // | |                        v       multiple threads      |
    // | |                  BatchingQueue                       |
    // | |                        |       single thread         |
    // | |                        v                             |
    // | | --              SESQueueHandler       <========*     |    *<=== ChannelLocatorConnectionMgr notifications
    // | | |                      |   \                         |
    // | | |                      |    - FederatedAdminProvider |
    // | | |                      v                             |
    // | | | --        RedundantEventSupplier                   |
    // | | | |                  / | \       dispatches to ALL   |
    // | | | |                 /  |  \      valid connections   |
    // | | | |                v   v   v                         |
    // | | | |          SubscribedEventFilter    <=============>
    // | | | |                    |                 | SubChangeHandlerFan
    // | | | |                    v                 |
    // | | | | --      CosNotifyChannelSupplier  ==== subscription_change() notifications
    // | | | | |                  |
    // | | | | |                  v
    // ----------         CNCA::EventChannel                              (remote) consumer event channel

    class FederationForwarder : public TA_Base_Core::IEventHandler // For receiving events from the CosNotifyChannelConsumer
    {
    public:
        // The FederationForwarder subscribes to ChannelLocatorConnectionMgr notifications
        // for consumer IChannelLocators with the given (channelName, consumerLocationKey).
        FederationForwarder( const std::string& channelName,
                             unsigned long consumerLocationKey,
                             TA_Base_Core::IFederatableEventChannel_ptr localChannel );

        // Disconnects federation if still connected.
        virtual ~FederationForwarder();

        // A reference count is provided so that multiple clients at the same remote
        // location can share the federated connection (refer to the IChannelLocator
        // interface for details of how federation is established).
        // The FederationForwarder is initialised with a reference count of 0.
        // The connection will be established when the reference count transitions
        // from 0 -> 1, and terminated when it transitions from 1 -> 0.
        void ref();
        void unref();

        // Implementation of IEventHandler
        virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events);

    private:
        // When federation is established, the CosNotifyChannelConsumer is connected to the local
        // event channel, and the StructuredEventSupplier is connected to all of the remote event
        // channels for the (channelName, consumerLocationKey) specified in the ctor.
        // When federation is no longer required, we disconnect the consumer from the local event
        // channel, and disconnect the supplier from the remote channels by destroying it.
        // NOTE: Caller must acquire m_lock before calling these methods.
        void connectFederation();
        void disconnectFederation();

        // The FederationForwarder attaches to a single event channel on the local supplier side.
        // This is set in the ctor and must always be available.
        TA_Base_Core::IFederatableEventChannel_var m_localChannel;
        std::string m_channelName;
        unsigned long m_consumerLocationKey;

        int m_refCount;
        TA_Base_Core::NonReEntrantThreadLockable m_refLock;

        // Protects m_channelConsumer, m_constraintSet
        TA_Base_Core::NonReEntrantThreadLockable m_consumerLock;

        // The consumer will only be attached (ie. non-nil) to the event channel (m_localChannel)
        // when federation is established (ie. ref count > 0).
        TA_Base_Core::CosNotifyChannelConsumer m_channelConsumer;
        TA_Base_Core::RefCountedConstraintSet m_constraintSet;

        // Protects m_supplier, m_handler (it is necessary to use a separate lock to protect
        // consumer and supplier members in order to avoid deadlock).
        TA_Base_Core::NonReEntrantThreadLockable m_supplierLock;

        // m_supplier forwards events to multiple redundant event channels (on the remote consumer side).
        // Some of the StructuredEventSupplier behaviour is not strictly necessary, such as adding a
        // sequence number to each event.  The SubscribedEventFilter might be useful since the
        // multiple channels are unlikely to all have exactly the same subscriptions at all times.

        // TODO LPP: We destroy m_supplier each time the ref count transitions from 1 -> 0.  This is
        // because StructuredEventSupplier doesn't provide a method for flushing the queue, and
        // SESQueueHandler doesn't provide a way of disconnecting from all channels and then
        // reconnecting to them.
        TA_Base_Core::StructuredEventSupplier* m_supplier;

        // We need to attach and detach the SESQueueHandler to the ChannelLocatorConnectionMgr
        // (note that m_supplier /*takes*/ the handler, so we don't have to explicitly delete it).
        TA_Base_Core::SESQueueHandler* m_handler;
    };

}; // TA_Base_Core

#endif
