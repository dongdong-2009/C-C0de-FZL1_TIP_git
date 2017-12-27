/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/GatewayConsumer.cpp $
  * @author:  Lachlan Partington
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/12/17 15:24:44 $
  * Last modified by:  $Author: shurui.liu $
  * 
  */

// TODO LPP: Rename file to FederationForwarder.cpp

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "app/notification_service/src/FederatableEventChannel.h"
#include "app/notification_service/src/GatewayConsumer.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // FFSubChangeHandler receives onSubscriptionChange() notifications from a CosNotifyChannelSupplier,
    // incrementing and decrementing reference counts on constraints in a shared RefCountedConstraintSet
    // as event types are added and removed.

    // This approach is necessary because the FederationForwarder uses a single RefCountedConstraintSet
    // for all of the redundant channels that the RedundantEventSupplier connects to, and we need to
    // keep track of which event types were ref'ed by a particular channel so that they can be unref'ed
    // when the channel becomes unavailable (ie. when onSubscriptionReset() is called on a particular
    // handler).
    // Note that the SESQueueHandler creates FFSubChangeHandler instances automatically using the
    // FFSubChangeHandlerFactory.
    class FFSubChangeHandler : public TA_Base_Core::ISubChangeHandler
    {
    public:
        FFSubChangeHandler(TA_Base_Core::RefCountedConstraintSet& constraintSet);

        // Implementation of ISubChangeHandler
        virtual void onSubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed);
        virtual void onSubscriptionReset();

    private:
        TA_Base_Core::EventTypeSet m_eventTypeSet;
        TA_Base_Core::RefCountedConstraintSet& m_constraintSet;
    };


    FFSubChangeHandler::FFSubChangeHandler(TA_Base_Core::RefCountedConstraintSet& constraintSet)
        : m_constraintSet(constraintSet)
    {
        FUNCTION_ENTRY( "FFSubChangeHandler" );
        FUNCTION_EXIT;
    }


    void FFSubChangeHandler::onSubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed)
    {
        FUNCTION_ENTRY( "onSubscriptionChange" );

        TA_Assert(added.length() > 0 || removed.length() > 0);

        // We need to forward subscription_change() notifications to suppliers on the supplier-
        // side (local) event channel.  We do this by modifying the CosNotifyChannelConsumer's
        // Filter as subscription_change() events are received by the StructuredEventSupplier.

        // We allow exceptions from RefCountedConstraintSet to propagate, as we expect the
        // consumer-side event channel assembly (which is local to this process) to always
        // be available.  Failure should be fatal.

        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "FFSubChangeHandler::onSubscriptionChange(): %s forwarding subscription_change() to supplier-side EventChannel",
            TA_Base_Core::gPtrToStr(this).c_str() );

        unsigned int i = 0;
        for ( i = 0; i < added.length(); ++i )
        {
            CosNotifyFilter::ConstraintExp constraint;
            TA_Base_Core::gPopulateConstraintWithEventType(added[i], constraint);

            // TODO LPP NOW: HACK to workaround multiple subscription_change notifications for
            // the same event type.  EventTypeSet doesn't allow the same event type to be added
            // or removed multiple times.  We need to determine the reason for this behavior.
            unsigned int exprId = m_eventTypeSet.getEventTypeKey(added[i]);
            if ( TA_Base_Core::NOT_IN_SET == exprId )
            {
                exprId = m_eventTypeSet.addEventType(added[i]);
            }

            m_constraintSet.addExpression( reinterpret_cast<unsigned int>(this), exprId, constraint );
        }

        for ( i = 0; i < removed.length(); ++i )
        {
            unsigned int exprId = m_eventTypeSet.getEventTypeKey(removed[i]);
            if ( exprId != TA_Base_Core::NOT_IN_SET )
            {
                // TODO LPP NOW: HACK to workaround multiple subscription_change notifications for
                // the same event type.  EventTypeSet doesn't allow the same event type to be added
                // or removed multiple times.  We need to determine the reason for this behaviour.
                //exprId = m_eventTypeSet.removeEventType(removed[i]);
            }

            m_constraintSet.removeExpression( reinterpret_cast<unsigned int>(this), exprId );
        }

        FUNCTION_EXIT;
    }


    void FFSubChangeHandler::onSubscriptionReset()
    {
        FUNCTION_ENTRY( "onSubscriptionReset" );

        // We need to unref all of the constraints that have been ref'ed by onSubscriptionChange().
        std::vector<unsigned int> exprIds;
        m_eventTypeSet.getEventTypeKeys(exprIds);

        // We allow exceptions from RefCountedConstraintSet to propagate, as we expect the
        // consumer-side event channel assembly (which is local to this process) to always
        // be available.  Failure should be fatal.

        for ( unsigned int i = 0; i < exprIds.size(); ++i )
        {
            m_constraintSet.removeExpression(reinterpret_cast<unsigned int>(this), exprIds[i]);
        }

        m_eventTypeSet.removeAllEventTypes();

        FUNCTION_EXIT;
    }


    class FFSubChangeHandlerFactory : public TA_Base_Core::ISubChangeHandlerFactory
    {
    public:
        FFSubChangeHandlerFactory(TA_Base_Core::RefCountedConstraintSet& constraintSet);

        // Implementation of ISubChangeHandlerFactory
        /*gives*/ TA_Base_Core::ISubChangeHandler* createInstance();

    private:
        TA_Base_Core::RefCountedConstraintSet& m_constraintSet;
    };


    FFSubChangeHandlerFactory::FFSubChangeHandlerFactory(TA_Base_Core::RefCountedConstraintSet& constraintSet)
        : m_constraintSet(constraintSet)
    {
        FUNCTION_ENTRY( "FFSubChangeHandlerFactory" );
        FUNCTION_EXIT;
    }


    /*gives*/ TA_Base_Core::ISubChangeHandler* FFSubChangeHandlerFactory::createInstance()
    {
        return new FFSubChangeHandler(m_constraintSet);
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FederationForwarder::FederationForwarder ( const std::string& channelName,
                                               unsigned long consumerLocationKey,
                                               TA_Base_Core::IFederatableEventChannel_ptr localChannel )
        : m_refCount(0),
          m_localChannel( TA_Base_Core::IFederatableEventChannel::_duplicate(localChannel) ),
          m_channelName(channelName),
          m_consumerLocationKey(consumerLocationKey),
          m_supplier(NULL),
          m_handler(NULL)
    {
        FUNCTION_ENTRY( "FederationForwarder" );

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "FederationForwarder::FederationForwarder(): %s created with for connection to channelName %s, consumerLocationKey %d",
            TA_Base_Core::gPtrToStr(this).c_str(), m_channelName.c_str(), m_consumerLocationKey );

        TA_Assert(!CORBA::is_nil(m_localChannel));

        m_channelConsumer.setEventHandler( this );

        FUNCTION_EXIT;
    }


    FederationForwarder::~FederationForwarder()
    {
        FUNCTION_ENTRY( "~FederationForwarder" );

        // TODO LPP: Should we use a lock here?

        // TODO LPP: Do we need to call setHandler(NULL) on the CosNotifyChannelConsumer?
        m_channelConsumer.setEventHandler(NULL);

        // TODO LPP: It would probably be better if clients were required to unref() before
        // destroying the FederationForwarder.
        if ( m_refCount > 0 )
        {
            disconnectFederation();
        }

        FUNCTION_EXIT;
    }


    void FederationForwarder::ref()
    {
        FUNCTION_ENTRY( "ref" );

        TA_THREADGUARD( m_refLock );

        if ( 0 == m_refCount )
        {
            connectFederation();
        }

        ++m_refCount;
        TA_Assert(m_refCount > 0)

        FUNCTION_EXIT;
    }


    void FederationForwarder::unref()
    {
        FUNCTION_ENTRY( "unref" );

        TA_THREADGUARD( m_refLock );

        TA_Assert(m_refCount > 0)
        --m_refCount;

        if ( 0 == m_refCount )
        {
            disconnectFederation();
        }

        FUNCTION_EXIT;
    }


    bool FederationForwarder::onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events)
    {
        FUNCTION_ENTRY( "onEventsReceived" );

        TA_THREADGUARD( m_supplierLock );

        // We shouldn't be receiving events if we are not connected.
        // However, it might have been sent just as disconnectFederation() was called.
        if ( m_supplier != NULL )
        {
            LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                "FederationForwarder::onEventsReceived(): %s forwarding %d events to consumer-side EventChannels",
                TA_Base_Core::gPtrToStr(this).c_str(), events.size() );

            // We have to make a copy of the events because StructuredEventSupplier::publishEvent()
            // takes ownership of them.
            std::vector<TA_Base_Core::TimetagedStructuredEvent*> eventsCopy;
            /*
            std::vector<CosNotification::StructuredEvent*> eventsCopy;
            */
            eventsCopy.reserve(events.size());

		    for ( unsigned int i = 0; i < events.size(); ++i )
		    {

				CosNotification::StructuredEvent* newEvent = new CosNotification::StructuredEvent(*events[i]);
                eventsCopy.push_back(new TA_Base_Core::TimetagedStructuredEvent(newEvent, 0));
                /*
                eventsCopy.push_back(new CosNotification::StructuredEvent(*events[i]));
                */
                TA_Base_Core::gLogSpecificMessage(*events[i], "FederationForwarder::onEventsReceived()");
            }

            m_supplier->publishEvents(eventsCopy);
        }
        else
        {
            LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                "FederationForwarder::onEventsReceived(): %s discarding %d events as consumer-side EventChannels have been disconnected.",
                TA_Base_Core::gPtrToStr(this).c_str(), events.size() );
        }

        FUNCTION_EXIT;
        return true;
    }


    void FederationForwarder::connectFederation()
    {
        FUNCTION_ENTRY( "connectFederation" );

        TA_Assert(0 == m_refCount);
        TA_Assert(!m_channelConsumer.hasEventChannel());
        TA_Assert(NULL == m_supplier);
        TA_Assert(NULL == m_handler);

        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "FederationForwarder::connectFederation(): %s connecting as consumer to local EventChannel and as supplier to remote EventChannel",
            TA_Base_Core::gPtrToStr(this).c_str() );

        TA_Base_Core::IAdminProvider* adminProvider = new FederatedAdminProvider;
        CosNotifyChannelAdmin::ConsumerAdmin_var consumerAdmin = adminProvider->getConsumerAdmin(m_localChannel);
        TA_Assert(!CORBA::is_nil(consumerAdmin));

        {
            TA_THREADGUARD( m_consumerLock );
            TA_VERIFY(m_channelConsumer.setEventChannel(consumerAdmin), "Failed to set ConsumerAdmin");

            TA_Assert(!CORBA::is_nil(m_channelConsumer.getFilter()));
            m_constraintSet.setFilter(m_channelConsumer.getFilter());
        }

        {
            TA_THREADGUARD( m_supplierLock );
            TA_Base_Core::ISubChangeHandlerFactory* factory = new FFSubChangeHandlerFactory(m_constraintSet);
            m_handler = new TA_Base_Core::SESQueueHandler(m_channelName, m_consumerLocationKey, /*takes*/ adminProvider, factory);
            m_supplier = new TA_Base_Core::StructuredEventSupplier(/*takes*/ m_handler);

            TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().attach(m_handler);
        }

        FUNCTION_EXIT;
    }


    void FederationForwarder::disconnectFederation()
    {
        FUNCTION_ENTRY( "disconnectFederation" );

        //TA_Assert(0 == m_refCount);
        TA_Assert(m_channelConsumer.hasEventChannel());
        TA_Assert(m_supplier != NULL);
        TA_Assert(m_handler != NULL);

        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "FederationForwarder::disconnectFederation(): %s disconnecting as consumer from local EventChannel and as supplier from remote EventChannel",
            TA_Base_Core::gPtrToStr(this).c_str() );

        // We must disconnect the supplier-side first so that the FFSubChangeHandlers don't
        // try to modify m_constraintSet after the filter has been set to nil().
        {
            TA_THREADGUARD( m_supplierLock );

            TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().detach(m_handler);

            delete m_supplier;
            m_supplier = NULL;

            // m_handler is deleted by m_supplier;
            m_handler = NULL;
        }

        {
            TA_THREADGUARD( m_consumerLock );

            m_constraintSet.setFilter(CosNotifyFilter::Filter::_nil());
            m_channelConsumer.setEventChannel(CosNotifyChannelAdmin::ConsumerAdmin::_nil());
        }

        FUNCTION_EXIT;
    }


}; // TA_Base_Core

