/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/SubscriptionManager.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:12:25 $
  * Last modified by:  $Author: limin.zhu $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/SubscriptionManager.h"
#include "core/message/src/StructuredEventConsumer.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/message/src/EventChannelUser.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/message/src/IMessageSubscriber.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{
    SubscriptionFilter::SubscriptionFilter( const CosNotification::EventTypeSeq& event_types,
                                            const std::string& constraintExpr,
                                            IMessageFilter* filter )
        : m_id(0),
          m_clientSideFilter( filter )
    {
        std::string eventTypesStr = gStringFromEventTypes(event_types);
        LOG4( SourceInfo, DebugUtil::DebugDebug,
            "SubscriptionFilter::SubscriptionFilter(): constructing SubscriptionFilter %s\n"
            " event types=%s\n"
            " constraint_expr=%s\n"
            " clientSideFilter=%s",
            gPtrToStr(this).c_str(), eventTypesStr.c_str(), constraintExpr.c_str(),
            gPtrToStr(m_clientSideFilter).c_str() );

        // initialise the constraint expression
        m_serverSideFilter.event_types = event_types;
        m_serverSideFilter.constraint_expr = constraintExpr.c_str();
    }


    SubscriptionFilter::~SubscriptionFilter()
    {
        delete m_clientSideFilter;
        m_clientSideFilter = NULL;
    }


    unsigned int sGetNextID()
    {
        static TA_Base_Core::ReEntrantThreadLockable s_IDLock;

        TA_THREADGUARD( s_IDLock );

        static unsigned int s_nextID = 0;
        return ++s_nextID;
    }

    unsigned int SubscriptionFilter::getID() const
    {
        if ( 0 == m_id )
        {
            m_id = sGetNextID();
        }

        return m_id;
    }


    bool SubscriptionFilter::messageMatchesFilter( const CosNotification::StructuredEvent& event ) const
    {
        return ( NULL == m_clientSideFilter ) || m_clientSideFilter->match( event );
    }


/////////////////////////////////////////////////////////////////////////////////////////

    void SubscriptionFilterSet::addSubscription( SubscriptionManager* manager,
                                                 const std::string& channelName,
                                                 /*takes*/ SubscriptionFilter* subFilter,
                                                 unsigned long supplierLocationKey,
                                                 IEventHandler* handler )
    {
        TA_Assert(manager != NULL);
        TA_Assert(subFilter != NULL);
        TA_Assert(handler != NULL);

        manager->subscribeToMessages( channelName, subFilter, supplierLocationKey, handler );

        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
                "SubscriptionFilterSet::addSubscription(): %p, channel=%s, supplier location=%d", this, channelName.c_str(), supplierLocationKey );
            TA_THREADGUARD( m_subscriptionsLock );
            m_subscriptions.push_back(SFSSubDetail(subFilter, manager));
        }
    }


    void SubscriptionFilterSet::removeAllSubscriptions()
    {
        LOG2( SourceInfo, DebugUtil::DebugMsg,
             "SubscriptionFilterSet::removeAllSubscriptions(): %s removing %d subscriptions",
             gPtrToStr(this).c_str(), m_subscriptions.size() );

        TA_THREADGUARD( m_subscriptionsLock );

        for ( unsigned int i = 0; i < m_subscriptions.size(); ++i )
        {
            SFSSubDetail& detail = m_subscriptions[i];

            TA_Assert(detail.m_manager != NULL);
            TA_Assert(detail.m_subFilter != NULL);

            detail.m_manager->unsubscribeToMessages(detail.m_subFilter);

            delete detail.m_subFilter;
            detail.m_subFilter = NULL;
        }

        m_subscriptions.clear();
    }


    SubscriptionFilterSet::~SubscriptionFilterSet()
    {
        LOG2( SourceInfo, DebugUtil::DebugDebug,
             "SubscriptionFilterSet::~SubscriptionFilterSet(): %s has %d subscriptions",
             gPtrToStr(this).c_str(), m_subscriptions.size() );

        TA_THREADGUARD( m_subscriptionsLock );

        for ( unsigned int i = 0; i < m_subscriptions.size(); ++i )
        {
            LOG2( SourceInfo, DebugUtil::DebugDebug,
                 "SubscriptionFilter %d: %s", i, gPtrToStr(m_subscriptions[i].m_subFilter).c_str() );
        }

        TA_Assert(m_subscriptions.empty());
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // IChannelLocator::federateIfRemoteSource() needs to be called when a client wants to
    // subscribe to events that originate from a location different to the local location.
    // Rather than add the concept of location and federation to the StructuredEventConsumer,
    // this behaviour is provided by the FederationHandler.

    class FederationHandler : public IChannelObserver
    {
    public:
        // NOTE that the FederationHandler doesn't self-register with the ChannelLocatorConnectionMgr.
        // ChannelLocatorConnectionMgr::attach() must be called by our client.

        // consumer             This handler forwards IChannelObserver notifications onto the
        //                      consumer (ie. onChannelAvailable() and onChannelUnavailable() calls).
        // channelName          Name of the channel that the handler will listen to
        // locationKey          Location key of the IChannelLocator that the handler is connected to.
        //                      Used to determine whether federation is necessary.
        // supplierLocationKey  Location key of message source.  This is only used when
        //                      establishing federation - if the source location key
        //                      is different to the local location key,
        //                      IChannelLocator::federateIfRemoteSource() will be called.

        // TODO LPP: Perhaps the client should determine whether federation is necessary, and use
        // a different handler if it isn't necessary.
        FederationHandler( StructuredEventConsumer& consumer,
                           const std::string& channelName,
                           unsigned long locationKey,
                           unsigned long supplierLocationKey );

        // CLEAN-UP
        // unfederateAllConnections() must be called to unfederate all valid connections.
        // The client must first detach the FederationHandler from the ChannelLocatorConnectionMgr
        // (this ensures that methods won't be called on m_consumer in onChannelAvailable() and
        // onChannelUnavailable() while connections are being unfederated).

        // Unfederates all valid connections between the local and remote IChannelLocators.
        void unfederateAllConnections();

        // Implementation of IChannelObserver
        // (we will receive notifications for (channelName, locationKey) )
        virtual void onChannelUnavailable( const std::string& serviceAddr );
        virtual bool onChannelAvailable( const std::string& serviceAddr,
                                         const CosNotifyChannelAdmin::EventChannel_ptr channel,
                                         const IChannelLocator_ptr channelLocator );

    private:
        StructuredEventConsumer& m_consumer;
        unsigned long m_supplierLocationKey;

        // serviceAddr -> IChannelLocator*
        typedef std::map<std::string, IChannelLocator_var> ChannelLocators;
        ChannelLocators m_channelLocators;

        TA_Base_Core::NonReEntrantThreadLockable m_channelLocatorLock;
    };


    FederationHandler::FederationHandler( StructuredEventConsumer& consumer,
                                          const std::string& channelName,
                                          unsigned long locationKey,
                                          unsigned long supplierLocationKey )
        : IChannelObserver(channelName, locationKey),
          m_consumer(consumer),
          m_supplierLocationKey(supplierLocationKey )
    {
        LOG1( SourceInfo, DebugUtil::DebugMsg,
             "FederationHandler::FederationHandler(): Creating FederationHandler %s",
             gPtrToStr(this).c_str() );
    }


    void FederationHandler::onChannelUnavailable( const std::string& serviceAddr )
    {
        TA_THREADGUARD( m_channelLocatorLock );
        // Note that we may never have validated successfully in onChannelAvailable().
        ChannelLocators::iterator it = m_channelLocators.find( serviceAddr );
        if ( it != m_channelLocators.end() )
        {
            m_consumer.removeDeadConnection(serviceAddr);
            m_channelLocators.erase(it);
        }
    }


    bool FederationHandler::onChannelAvailable( const std::string& serviceAddr,
                                                const CosNotifyChannelAdmin::EventChannel_ptr channel,
                                                const TA_Base_Core::IChannelLocator_ptr channelLocator )
    {
        TA_THREADGUARD( m_channelLocatorLock );
       // Ensure that onChannelAvailable() is not called twice without an intervening call to
        // onInvalidateChannel().
        TA_Assert( m_channelLocators.find(serviceAddr) == m_channelLocators.end() );

        // Although IChannelLocator::federateIfRemoteSource() only federates if the source is actually
        // remote, we can avoid a remote call if we do the test now.
        if ( m_supplierLocationKey != getLocationKey() )
        {
            LOG5(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
                "FederationHandler::onChannelAvailable(): %s calling IChannelLocator::federateIfRemoteSource: "
                "supplierLocationKey=%d, local locationKey=%d, channelName=%s, SEC=%s",
                gPtrToStr(this).c_str(),
                m_supplierLocationKey, getLocationKey(), getChannelName().c_str(), gPtrToStr(&m_consumer).c_str() );

            try
            {
                channelLocator->federateIfRemoteSource( m_supplierLocationKey, getChannelName().c_str() );
            }
            catch(TA_Base_Core::FederationFailed& ff)
            {
                LOG_EXCEPTION_CATCH( SourceInfo,
                    "Federator::onChannelAvailable(): FederationFailed exception while calling federateIfRemoteSource()",
                    TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ff ).c_str() );

                // TODO LPP: Why don't we retry if this happens?
            }
            catch ( const CORBA::Exception& ex )
            {
                std::string mesg( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) );
                mesg += ": exception caught while calling federateIfRemoteSource on local NS";
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", mesg.c_str());

                return false;
            }
        }

        if ( m_consumer.addConnection(serviceAddr, channel) )
        {
            m_channelLocators[serviceAddr] = IChannelLocator::_duplicate(channelLocator);
            return true;
        }
        else
        {
            return false;
        }
    }


    void FederationHandler::unfederateAllConnections()
    {
        // Although IChannelLocator::unfederateIfRemoteSource() only unfederates if the source is actually
        // remote, we can avoid a remote call if we do the test now.
        if ( m_supplierLocationKey != getLocationKey() )
        {
            for ( ChannelLocators::iterator iter = m_channelLocators.begin(); iter != m_channelLocators.end(); ++iter )
            {
                TA_Assert(! CORBA::is_nil( iter->second ) );

                LOG5(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
                    "FederationHandler::unfederateAllConnections(): %s calling IChannelLocator::unfederateIfRemoteSource: "
                    "supplierLocationKey=%d, local locationKey=%d, channelName=%s, SEC=%s",
                    gPtrToStr(this).c_str(),
                    m_supplierLocationKey, getLocationKey(), getChannelName().c_str(), gPtrToStr(&m_consumer).c_str() );

                // TODO LPP: Is it appropriate to suppress the exceptions?
                try
                {
                    iter->second->unfederateIfRemoteSource( m_supplierLocationKey, getChannelName().c_str() );
                }
                catch( const TA_Base_Core::FederationFailed& ff)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo,
                        "Federator::onChannelAvailable(): FederationFailed exception while calling unfederateIfRemoteSource()",
                        TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ff ).c_str() );
                }
                catch ( const CORBA::Exception& ex )
                {
                    std::string mesg( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) );
                    mesg += ": exception caught while calling unfederateIfRemoteSource on local NS";
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", mesg.c_str());
                }
            }
        }
    }


/////////////////////////////////////////////////////////////////////////////////////////

    SubscriptionManager::SubscriptionManager()
    {
    }


    SubscriptionManager::~SubscriptionManager()
    {
        // cleanUpConsumers() must be called before destroying the SubscriptionManager
        TA_Assert(m_channelToConsumerMap.empty());
        TA_Assert(m_subscriptionToConsumerMap.empty());
    }


    void SubscriptionManager::cleanUpConsumers()
    {
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "SubscriptionManager::cleanUpConsumers(): Subscription Manager cleaning up %d StructuredEventConsumers",
            m_channelToConsumerMap.size());

        // TODO LPP: If we required clients to unsubscribe, there would be nothing to do here.

        for ( ChannelToConsumerMap::iterator it = m_channelToConsumerMap.begin(); it != m_channelToConsumerMap.end(); ++it )
        {
            unfederateAndDestroyConsumer(it->second);
        }

        m_channelToConsumerMap.clear();
        m_subscriptionToConsumerMap.clear();
    }


    void SubscriptionManager::unsubscribeToMessages( SubscriptionFilter* subscriber )
    {
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
            "SubscriptionManager::unsubscribeToMessages(): removing subscriber %s", gPtrToStr(subscriber).c_str() );

        TA_ASSERT( NULL != subscriber, "The SubscriptionFilter was null");

        TA_THREADGUARD( m_subscriberLock );

        SubscriptionToConsumerMap::iterator subIter = m_subscriptionToConsumerMap.find(subscriber);
        TA_Assert(subIter != m_subscriptionToConsumerMap.end());

        StructuredEventConsumer* consumer = (*subIter).second.m_consumer;
        TA_Assert(consumer != NULL);

        // Remove this particular subscription
        if ( consumer->detachSubscriber( subscriber ) )
        {
            // If there aren't any subscribers left for that consumer
            if ( ! consumer->hasSubscribers() )
            {
                unfederateAndDestroyConsumer(subIter->second);

                // Remove the consumer from the list of stored consumers
                for ( ChannelToConsumerMap::iterator iter = m_channelToConsumerMap.begin();
                      iter != m_channelToConsumerMap.end(); ++iter )
                {
                    TA_ASSERT( NULL != iter->second.m_consumer,
                        "The consumer was null before removing a registered subscriber");

                    // Remove the consumer as it has been shut-down
                    if ( iter->second.m_consumer == consumer )
                    {
                        m_channelToConsumerMap.erase( iter++ );
                        break;
                    }
                }
            }
        }

        m_subscriptionToConsumerMap.erase(subIter);
    }


    void SubscriptionManager::unfederateAndDestroyConsumer(const SubscriptionManager::ConsumerInfo& ci)
    {
        TA_Assert(ci.m_fedHandler != NULL);
        ChannelLocatorConnectionMgr::getInstance().detach( ci.m_fedHandler );
        ci.m_fedHandler->unfederateAllConnections();
        delete ci.m_fedHandler;

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
            "SubscriptionManager::unfederateAndDestroyConsumer(): SEC %s will be destroyed",
            gPtrToStr(ci.m_consumer).c_str() );

        // Shut down the threads, stop incoming requests and clean up (delete) the consumer
        TA_Assert(ci.m_consumer!= NULL);
        ci.m_consumer->disconnectAllEventChannels();
        delete ci.m_consumer;
    }


    void SubscriptionManager::subscribeToMessages( const std::string& channelName,
                                                   SubscriptionFilter* subFilter,
                                                   unsigned long supplierLocationKey,
                                                   IEventHandler* handler )
    {
        TA_Assert(subFilter != NULL);
        TA_Assert(handler != NULL);

        std::string eventTypesStr = gStringFromEventTypes(subFilter->getFilterCriteria().event_types);
        LOG3( SourceInfo, DebugUtil::DebugDebug,
            "SubscriptionManager::subscribeToMessages(): channelName=%s, subFilter=%s, supplierLocationKey=%d",
            channelName.c_str(), gPtrToStr(subFilter).c_str(), supplierLocationKey );

        // Check to see if we want the local location, or a specified location
        if ( LOCAL_LOCATION == supplierLocationKey )
        {
            // We want to use our own location
            supplierLocationKey = gGetLocalLocationKey();
        }

        TA_THREADGUARD( m_subscriberLock );

        // Find a StructuredEventConsumer for the given channel that was created with
        // matching subscription information.

        // equal_range() returns the earliest element in the map greater than or equal to
        // the find key, and the earliest element in the map greater than the find key.
        // For example, {A, A, A, B, B, B, C, C, C}         1: lower_bound
        //                        ^1       ^2               2: upper_bound
        typedef ChannelToConsumerMap::iterator IterType;
        std::pair<IterType, IterType> bounds = m_channelToConsumerMap.equal_range(channelName);

        for ( IterType iter = bounds.first; iter != bounds.second; ++iter )
        {
            TA_ASSERT( NULL != iter->second.m_consumer, "The consumer was null before attaching a subscriber");

            // Subscribers for different source locations are attached to different SECs, even
            // though the subscription would otherwise be compatible.  This is probably because
            // a FederationHandler federates with a single source location, rather than some
            // limitation with Corba filters or the StructuredEventConsumer.
            // TODO LPP: Perhaps the SEC can be shared by multiple FederationHandlers.
            if ( supplierLocationKey == iter->second.m_supplierLocationKey &&
                 iter->second.m_consumer->attachSubscriberIfCompatible( handler, subFilter ) )
            {
                m_subscriptionToConsumerMap.insert(SubscriptionToConsumerMap::value_type(subFilter, iter->second));

                return;
            }
        }

        // No compatible StructuredEventConsumer was found for the subscription.  Create a new one.
        // We need to consume events from the local event channel, but federate with a (possibly)
        // remote channel.
        // Servant is activated in the constructor

        StructuredEventConsumer* consumer = new StructuredEventConsumer( new DefaultAdminProvider );
        FederationHandler* fedHandler = new FederationHandler( *consumer,
                                                               channelName,
                                                               gGetLocalLocationKey(),
                                                               supplierLocationKey );

        ChannelLocatorConnectionMgr::getInstance().attach( fedHandler );

        // Store the new consumer
        ConsumerInfo ci(consumer, fedHandler, supplierLocationKey);
        m_channelToConsumerMap.insert( ChannelToConsumerMap::value_type( channelName, ci ) );

        TA_VERIFY(consumer->attachSubscriberIfCompatible( handler, subFilter ), "Subscriber should be compatible with new StructuredEventConsumer");
        m_subscriptionToConsumerMap.insert(SubscriptionToConsumerMap::value_type(subFilter, ci));
    }


    SubscriptionManager::ConsumerInfo::ConsumerInfo( StructuredEventConsumer* consumer,
                                                    FederationHandler* fedHandler,
                                                    unsigned long supplierLocationKey)
        : m_consumer(consumer),
          m_fedHandler(fedHandler),
          m_supplierLocationKey(supplierLocationKey)
    {
        TA_Assert(m_consumer != NULL);
        TA_Assert(m_fedHandler != NULL);
    }
 
}; // TA_Base_Core

