
#if !defined(BackwardsCompatibility_F0FBFCD4_1728_49e8_8FF2_9647110C3E62__INCLUDED_)
#define BackwardsCompatibility_F0FBFCD4_1728_49e8_8FF2_9647110C3E62__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/BackwardsCompatibility.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Allows existing clients to compile without change following a major refactoring
  * of the messaging framework.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/TA_CosNotifyInterfaces.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/message/src/SubscriptionManager.h"
#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/TAAssert.h"


// Roadmap to messaging framework classes
//
// ----------------------------------------------------------------------------------------------------------
// TRANSACTIVE MESSAGING API
//                                                                          
// AbstractMessageSubscriber                                   FederationForwarderMgr     
// SpecialisedMessageSubscriber                                FederationForwarder
//
//                                                             Federator
//
// ----------------------------------------------------------------------------------------------------------
// HIGH-LEVEL API
//                                                                      
// SubscriptionManager           StructuredEventSupplier       EventChannelForwarder                                                   
// FederationHandler             SESQueueHandler               FederatableEventChannel_impl
//                               RedundantEventSupplier       
// StructuredEventConsumer        
// DomainNameDispatcher
// TypeNameDispatcher
//
// SubscriptionFilterSet
// SubscriptionFilter
//
// ----------------------------------------------------------------------------------------------------------
// LOW-LEVEL MESSAGING API
//
// CosProxyKeepAlive              SubChangeHandlerFan          BatchingQueue      
// CosNotifyChannelConsumer       RefCountedConstraintSet      
// CosNotifyChannelSupplier       SeqNumAppender
//                                SeqNumFilter
//                                EventTypeSet
//                                WildcardEventTypeSet
// ChannelLocatorConnectionMgr    SubscribedEventFilter

namespace TA_Base_Core
{
    // The AbstractMessageSubscriber originally implemented IMessageSubscriber, which had methods 
    // for adding and removing subscriptions.  This was so that the subscriptions could be 
    // unsubscribed from the SubscriptionManager and the SubscriptionFilter objects could be
    // deleted.  Clients would inherit from AbstractMessageSubscriber to use this behaviour.
    // However, it was a very confusing approach (as the template pattern often is).  

    // To maintain backwards compatibility, AbstractMessageSubscriber is still provided, but
    // now uses the SubscriptionFilterSet.  Also, the MessageSubscriptionManager has to manage
    // the subscription explicitly - it actually uses the m_subscriptions member directly
    // (clients that use MessageSubscriptionManager to subscribe to messages are REQUIRED to 
    // implement AbstractMessageSubscriber).
    class AbstractMessageSubscriber : public IEventHandler
    {
    public:
        // receiveMessage() is provided for backward compatibility with existing
        // clients.  NOTE that the event_domain and event_type (and all other header 
        // fields) are not available to the implementor of receiveMessage().
        // New clients can override onReceiveMessage() instead - this is preferable 
        // since the message header is available.  NOTE that receiveMessage() won't
        // be called then, so this has to be commented well.  
        virtual void receiveMessage(const CORBA::Any& message) = 0;

        // Implementation of IEventHandler
        virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events)
        {
            // Forward framework call to old method name for each event.
            for (unsigned int i = 0; i < events.size(); ++i)
            {
                TA_Assert(events[i] != NULL);
                gLogSpecificMessage(*events[i], "AbstractMessageSubscriber::onEventsReceived()");
                receiveMessage(events[i]->remainder_of_body);
            }

            return true;
        }

        SubscriptionFilterSet m_subscriptions;    
    };


    // IEventHandler::onEventsReceived() provides the event body as a CORBA::Any, but it
    // is necessary for clients to downcast to whatever type of message they are
    // expecting.  SpecialisedMessageSubscriber provides a simple implementation of
    // onEventsReceived() that does the downcast if possible, and passes the message to
    // the client by calling receiveSpecialisedMessage().
    // SpecialisedMessageSubscriber also ensures that the subscriber only receives a 
    // single message at a time.   

    // TODO LPP: The benefit provided by this class is minimal.  It should be possible
    // to write a template function that returns a pointer to the downcasted type.  
    // There may be some issues though (eg. the lock wouldn't be able to be used,
    // would a returned _ptr be valid).

    // TODO LPP: Does this approach rely on different message types being sent on 
    // distinct channels?  Presumably the subscriber attaches to a specific channel
    // (how is this done?).  
    // It seems inappropriate to subdivide channels based on the C++ 'type' of message,
    // rather than on subsystem etc.  A more general solution would use a polymorphic
    // approach based on factory ids (eg. similar to serialisation).

    template <class PayloadType> 
    class SpecialisedMessageSubscriber : public AbstractMessageSubscriber
    {
    public:
        SpecialisedMessageSubscriber() {}

        // This is overriden by the client to receive the downcasted message (rather than 
        // receiving a CORBA::Any).
        // This version of receiveSpecialisedMessage() is provided for backwards compatibility.
        // NOTE that the event_domain and event_type (and all other header fields) are not 
        // available to the implementor.  Clients should override the two-argument version of
        // receiveSpecialisedMessage() below if they need the header.
        virtual void receiveSpecialisedMessage(const PayloadType& payload) = 0;

        virtual void receiveSpecialisedMessage(const CosNotification::StructuredEvent& event, const PayloadType& payload)
        {
            receiveSpecialisedMessage(payload);
        }

        // Implementation of AbstractMessageSubscriber
        virtual void receiveMessage(const CORBA::Any& message) {TA_Assert(0);} // Dummy implementation is NOT CALLED

        virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events)
        {
            for (unsigned int i = 0; i < events.size(); ++i)
            {
                TA_Assert(events[i] != NULL);

                // Prevent this being called on the same subscriber at the same time
                TA_THREADGUARD( m_lock );

                // extract the specialised message from the Any
                PayloadType* payload = 0;

                if ( ( events[i]->remainder_of_body >>= payload) != 0)
                {
                    // Extraction was successful - pass the message AND specialised payload to the handler
                    receiveSpecialisedMessage(*events[i], *payload);
                }
                else
                {
                    LOG2( SourceInfo, DebugUtil::DebugError,
                        "SpecialisedMessageSubscriber<%s>::onEventsReceived(): Error extracting message body for event: %s",
                        typeid(PayloadType).name(), gStringFromEventHeader(*events[i]).c_str() );
                }
            }

            return true;
        }

    private:
        // NOT CLONEABLE
        SpecialisedMessageSubscriber( const SpecialisedMessageSubscriber& );

        // Thread guard the received message call back
        TA_Base_Core::NonReEntrantThreadLockable m_lock;
    };    


    // MessageSubscriptionManager is used to hide the details of subscribing to specific
    // types of TransActive messages (eg. Comms, Audit, etc).  For example, clients don't 
    // need to directly know about the SubscriptionManager or SubscriptionFilter.  However,
    // it is not particularly elegant that all TransActive message types are exposed in the 
    // one interface.

    // Existing code typically makes calls similar to the following, so it is necessary
    // to provide the MessageSubscriptionManager for backwards compatibility:
    //  MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
    //      TA_Base_Core::AlarmStateUpdate::AlarmAgentStateRequest,
    //      this,
    //      RunParams::getInstance().get(RPARAM_ENTITYNAME));
    //  MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    // Note that the subscriber must be an AbstractMessageSubscriber.  This is so that the
    // MessageSubscriptionManager can add and remove subscriptions from the SubscriptionFilterSet 
    // (the MessageSubscriptionManager and AbstractMessageSubscriber are doing the work that 
    // should possibly be done by the client).

    // TODO LPP: It may be preferable for clients to use a set of global methods that 
    // return SubscriptionFilter objects constructed in appropriate ways or to use 
    // gCreateSubscriptionFilter() directly.  For example:
    //  SubscriptionFilterSet m_siSet;     // would normally be a member variable
    //  SubscriptionFilter* si = gCreateSubscriptionFilterForAudit(context, subscriber);
    //  m_siSet.addSubscription(channelName, si, sourceLocationKey);
    //  m_siSet.removeAllSubscriptions();

    // Use ANY_ENTITY when calling subscribeToCommsMessage() to subscribe to events
    // relating to any entities.
    const unsigned int ANY_ENTITY = 0;
          
    class MessageContext;
    class MessageType;

    class MessageSubscriptionManager
    {
    public:
        // This class is stateless and only has static member functions (it is similar to a 
        // namespace).  A call such as MessageSubscriptionManager::getInstance().subscribeToCommsMessage
        // will not actually use the instance returned by getInstance (because no 'this' pointer
        // is needed).
        // Note that a reference is NOT returned here, so that we can just return a trivial
        // default-constructed instance.
        // TODO LPP: There is one problem with not returning a reference here.  It is not legal to 
        // assign the result of this call (which is a temporary) to a non-const reference variable.
        static MessageSubscriptionManager getInstance()
        {
            return MessageSubscriptionManager();
        }


        // Comms message subscriptions are location-based (the subscription is for events
        // sent by a particular location).  NOTE that subsystemKey is not currently used.
        // entityKey    - the key of the entity from which you want to receive events.
        //                If entityKey == ANY_ENTITY, the subscription is for events from ANY entity.
        // locationKey  - the key of the location of the entity from which you want to receive events.
        //                Must NOT be the constants LOCAL_LOCATION or ANY_LOCATION.

        static void subscribeToCommsMessage( const MessageContext& context,
                                             AbstractMessageSubscriber* subscriber,
                                             unsigned long entityKey,
                                             unsigned long subsystemKey,    // NOT USED
                                             unsigned long locationKey,
                                             const FilterData* filterableData = NULL );

        static void subscribeToCommsMessage( const MessageType& messageType,
                                             AbstractMessageSubscriber* subscriber,
                                             unsigned long entityKey,
                                             unsigned long subsystemKey,    // NOT USED
                                             unsigned long locationKey,
                                             const FilterData* filterableData = NULL );


        // Broadcast Comms message subscriptions are location-independent.  The subscriber
        // will receive events sent by ANY_LOCATION.
        // NOTE: Care should be taken when subscribing for broadcast comms.  Location-independent
        // subscriptions do not scale to large numbers of locations very well.

        static void subscribeToBroadcastCommsMessage( const MessageContext& context,
                                                      AbstractMessageSubscriber* subscriber,
                                                      const FilterData* filterableData = NULL );

        static void subscribeToBroadcastCommsMessage( const MessageType& messageType,
                                                      AbstractMessageSubscriber* subscriber,
                                                      const FilterData* filterableData = NULL);


        // The following message subscriptions are location-based.  The subscriber will 
        // ONLY receive events sent by the LOCAL_LOCATION.
        static void subscribeToAuditMessage( const MessageContext& context, AbstractMessageSubscriber* subscriber );

        static void subscribeToAuditMessage( const MessageType& messageType, AbstractMessageSubscriber* subscriber );

        static void subscribeToConfigUpdateMessage( const MessageContext& context, AbstractMessageSubscriber* subscriber );

        static void subscribeToConfigUpdateMessage( const MessageType& messageType, AbstractMessageSubscriber* subscriber );

        static void subscribeToStateUpdateMessage( const MessageContext& context, 
                                                   AbstractMessageSubscriber* subscriber,
                                                   const std::string& agentName,
                                                   const FilterData* filterableData = NULL );

        static void subscribeToStateUpdateMessage( const MessageType& messageType, 
                                                   AbstractMessageSubscriber* subscriber,
                                                   const std::string& agentName,
                                                   const FilterData* filterableData = NULL );

        static void subscribeToBindingRequestMessage( const MessageType& messageType,
                                                      const std::string& agentName,
                                                      AbstractMessageSubscriber* subscriber);

        static void unsubscribeToMessages( AbstractMessageSubscriber* subscriber );
    };

}; // TA_Base_Core

#endif // !defined(BackwardsCompatibility_F0FBFCD4_1728_49e8_8FF2_9647110C3E62__INCLUDED_)
