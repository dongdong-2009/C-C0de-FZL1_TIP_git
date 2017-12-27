/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/FederatableEventChannel.cpp $
  * @author:  Lachlan Partington
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/12/17 15:24:44 $
  * Last modified by:  $Author: shurui.liu $
  * 
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "app/notification_service/src/FederatableEventChannel.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"


namespace TA_Base_App
{
    const CosNotifyChannelAdmin::SupplierAdmin_ptr FederatedAdminProvider::getSupplierAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const
    {
        FUNCTION_ENTRY( "getSupplierAdmin" );

        TA_Assert(!CORBA::is_nil(channel));

        CosNotifyChannelAdmin::SupplierAdmin_var admin;

        try
        {
            TA_Base_Core::IFederatableEventChannel_var fedChannel = TA_Base_Core::IFederatableEventChannel::_narrow( channel );
            admin = fedChannel->federated_supplier_admin();
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "FederatedAdminProvider::getSupplierAdmin(): " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
        }

        FUNCTION_EXIT;
        return admin._retn();
    }


    const CosNotifyChannelAdmin::ConsumerAdmin_ptr FederatedAdminProvider::getConsumerAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const
    {
        FUNCTION_ENTRY( "getConsumerAdmin" );

        TA_Assert(!CORBA::is_nil(channel));

        CosNotifyChannelAdmin::ConsumerAdmin_var admin;

        try
        {
            TA_Base_Core::IFederatableEventChannel_var fedChannel = TA_Base_Core::IFederatableEventChannel::_narrow(channel);
            admin = fedChannel->federated_consumer_admin();
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "FederatedAdminProvider::getConsumerAdmin(): " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
        }

        FUNCTION_EXIT;
        return admin._retn();
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EventChannelForwarder::EventChannelForwarder()
    {
        FUNCTION_ENTRY( "EventChannelForwarder" );

        m_channelConsumer.setEventHandler(this);
        m_channelSupplier.setSubChangeHandler(this);

        FUNCTION_EXIT;
    }


    EventChannelForwarder::~EventChannelForwarder()
    {
        FUNCTION_ENTRY( "~EventChannelForwarder" );

        TA_Assert(!m_channelConsumer.hasEventChannel());
        TA_Assert(!m_channelSupplier.hasEventChannel());

        FUNCTION_EXIT;
    }


    void EventChannelForwarder::setEventChannels( CosNotifyChannelAdmin::ConsumerAdmin_ptr consumerAdmin,
                                                  CosNotifyChannelAdmin::SupplierAdmin_ptr supplierAdmin )
    {
        FUNCTION_ENTRY( "setEventChannels" );

        TA_VERIFY(m_channelConsumer.setEventChannel(consumerAdmin, false), "Failed to set ConsumerAdmin");
        m_constraintSet.setFilter(m_channelConsumer.getFilter());

        TA_VERIFY(m_channelSupplier.setEventChannel(supplierAdmin, false), "Failed to set SupplierAdmin");

        FUNCTION_EXIT;
    }


    bool EventChannelForwarder::onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events)
    {
        FUNCTION_ENTRY( "onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>&)" );

        // Events received from the consumer-side are forwarded to the supplier-side
        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub, 
            "EventChannelForwarder::onEventsReceived(): %s forwarding %d events", TA_Base_Core::gPtrToStr(this).c_str(), events.size() );

        FUNCTION_EXIT;
        return m_channelSupplier.publishEvents(events);
    }


    bool EventChannelForwarder::onEventsReceived(const CosNotification::EventBatch& eventBatch)
    {
        FUNCTION_ENTRY( "onEventsReceived(const CosNotification::EventBatch&)" );

        // Events received from the consumer-side are forwarded to the supplier-side
        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub, 
            "EventChannelForwarder::onEventsReceived(): %s forwarding %d events", TA_Base_Core::gPtrToStr(this).c_str(), eventBatch.length() );

        FUNCTION_EXIT;
        return m_channelSupplier.publishEvents(eventBatch);
    }


    void EventChannelForwarder::onSubscriptionChange( const CosNotification::EventTypeSeq& added,
                                                      const CosNotification::EventTypeSeq& removed )
    {
        FUNCTION_ENTRY( "onSubscriptionChange" );

        TA_Assert(added.length() > 0 || removed.length() > 0);

        // We allow exceptions from RefCountedConstraintSet to propagate, as we expect the
        // consumer-side event channel assembly (which is local to this process) to always
        // be available.  Failure should be fatal.

        unsigned int i = 0;
        for ( i = 0; i < added.length(); ++i )
        {
            CosNotifyFilter::ConstraintExp constraint;
            TA_Base_Core::gPopulateConstraintWithEventType(added[i], constraint);
            unsigned int exprId = m_eventTypeSet.addEventType(added[i]);

            // clientId doesn't change since we are the only client
            m_constraintSet.addExpression(0, exprId, constraint);
        }

        for ( i = 0; i < removed.length(); ++i )
        {
            unsigned int exprId = m_eventTypeSet.removeEventType(removed[i]);
            m_constraintSet.removeExpression(0, exprId);
        }

        FUNCTION_EXIT;
    }


    void EventChannelForwarder::onSubscriptionReset()
    {
        FUNCTION_ENTRY( "onSubscriptionReset" );

        m_constraintSet.removeAllExpressions();
        m_eventTypeSet.removeAllEventTypes();

        FUNCTION_EXIT;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FederatableEventChannel_impl::FederatableEventChannel_impl(CosNotifyChannelAdmin::EventChannelFactory_ptr factory)
    {
        FUNCTION_ENTRY( "FederatableEventChannel_impl" );

        TA_Assert(!CORBA::is_nil(factory));

        // Use default QoS and Admin Properties
        CosNotification::QoSProperties qosProperties;
        CosNotification::AdminProperties adminProperties;

        CosNotifyChannelAdmin::ChannelID channelId;

        // Set up the supplier channel (ie. used by suppliers at the local location).
        // We use the default admin objects for local suppliers and consumers.
        m_supplierChannel = factory->create_channel( qosProperties, adminProperties, channelId );
        TA_Assert(!CORBA::is_nil(m_supplierChannel));

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
             "FederatableEventChannel_impl::FederatableEventChannel_impl(): Created supplier channel with "
             "ChannelID %lu, default_consumer_admin AdminID = %d, default_supplier_admin AdminID = %d",
             channelId, m_supplierChannel->default_consumer_admin()->MyID(), m_supplierChannel->default_supplier_admin()->MyID() );

        // Set up the consumer channel (ie. used by consumers at the local location)
        m_consumerChannel = factory->create_channel( qosProperties, adminProperties, channelId );
        TA_Assert(!CORBA::is_nil(m_consumerChannel));

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
             "FederatableEventChannel_impl::FederatableEventChannel_impl(): Created consumer channel with "
             "ChannelID %lu, default_consumer_admin AdminID = %d, default_supplier_admin AdminID = %d",
             channelId, m_supplierChannel->default_consumer_admin()->MyID(), m_supplierChannel->default_supplier_admin()->MyID() );

        m_forwarder.setEventChannels(m_supplierChannel->default_consumer_admin(), m_consumerChannel->default_supplier_admin());

        m_eventChannelFactory = CosNotifyChannelAdmin::EventChannelFactory::_duplicate(factory);

        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
             "FederatableEventChannel_impl::FederatableEventChannel_impl(): Tied consumer & supplier channels together with EventChannelForwarder %s",
             TA_Base_Core::gPtrToStr(&m_forwarder).c_str());

        FUNCTION_EXIT;
    }


    CosNotifyChannelAdmin::SupplierAdmin_ptr FederatableEventChannel_impl::federated_supplier_admin()
    {
        FUNCTION_ENTRY( "federated_supplier_admin" );

        TA_Assert(!CORBA::is_nil(m_consumerChannel));

        FUNCTION_EXIT;
        return m_consumerChannel->default_supplier_admin();
    }


    CosNotifyChannelAdmin::ConsumerAdmin_ptr FederatableEventChannel_impl::federated_consumer_admin()
    {
        FUNCTION_ENTRY( "federated_consumer_admin" );

        TA_Assert(!CORBA::is_nil(m_supplierChannel));

        FUNCTION_EXIT;
        return m_supplierChannel->default_consumer_admin();
    }


    // Methods from AttNotification::Interactive Interface
    char* FederatableEventChannel_impl::do_command( const char* pCmd,
                                                    CORBA::Boolean& success,
                                                    CORBA::Boolean& target_changed,
                                                    AttN_Interactive_outarg next_target  WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "do_command" );

        std::string cmd(pCmd);
        std::string result;

        target_changed = false;

        if (cmd == "help")
        {
            result = " go [supplier | consumer]";
        }
        else if (cmd == "go supplier")
        {
            result = " -> supplier channel";
            target_changed = true;
            next_target = AttNotification::Interactive::_narrow(m_supplierChannel);
        }
        else if (cmd == "go consumer")
        {
            result = " -> consumer channel";
            target_changed = true;
            next_target = AttNotification::Interactive::_narrow(m_consumerChannel);
        }
        else if ( cmd == "up" )
        {
            result = " -> EventChannelFactory";
            target_changed = true;
            next_target = AttNotification::Interactive::_narrow(m_eventChannelFactory);
        }
        else
        {
            result = " Invalid command";
        }

        FUNCTION_EXIT;
        return CORBA::string_dup(result.c_str());
    }


    AttN::NameSeq* FederatableEventChannel_impl::child_names( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "child_names" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    AttN::NameSeq* FederatableEventChannel_impl::my_name( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "my_name" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    AttN::IactSeq* FederatableEventChannel_impl::children(CORBA::Boolean only_cleanup_candidates  WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "children" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    CORBA::Boolean FederatableEventChannel_impl::safe_cleanup( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "safe_cleanup" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return false;
    }


    // Methods from CosEvCA::EventChannel Interface
    CosEvCA::ConsumerAdmin_ptr FederatableEventChannel_impl::for_consumers( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "for_consumers" );
        FUNCTION_EXIT;
        return m_consumerChannel->for_consumers();
    }


    CosEvCA::SupplierAdmin_ptr FederatableEventChannel_impl::for_suppliers( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "for_suppliers" );
        FUNCTION_EXIT;
        return m_supplierChannel->for_suppliers();
    }


    void FederatableEventChannel_impl::destroy( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "destroy" );

        m_supplierChannel->destroy();
        m_consumerChannel->destroy();

        FUNCTION_EXIT;
    }


    // Methods from CosNA::EventChannel Interface
    CosNA::EventChannelFactory_ptr FederatableEventChannel_impl::MyFactory( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "MyFactory" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    CosNA::ConsumerAdmin_ptr FederatableEventChannel_impl::default_consumer_admin( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "default_consumer_admin" );

        // limin++ CL-21308
        if ( CORBA::is_nil( m_defaultConsumerAdmin ) )
        {
            TA_THREADGUARD( m_defaultConsumerAdminLock );

            if ( CORBA::is_nil( m_defaultConsumerAdmin ) )
            {
                m_defaultConsumerAdmin = m_consumerChannel->default_consumer_admin();
            }
        }

        FUNCTION_EXIT;
        return CosNA::ConsumerAdmin::_duplicate( m_defaultConsumerAdmin );
        // ++limin CL-21308
        // return m_consumerChannel->default_consumer_admin();
    }


    CosNA::SupplierAdmin_ptr FederatableEventChannel_impl::default_supplier_admin( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "default_supplier_admin" );

        // limin++ CL-21308
        if ( CORBA::is_nil( m_defaultSupplierAdmin ) )
        {
            TA_THREADGUARD( m_defaultSupplierAdminLock );

            if ( CORBA::is_nil( m_defaultSupplierAdmin ) )
            {
                m_defaultSupplierAdmin = m_supplierChannel->default_supplier_admin();
            }
        }

        FUNCTION_EXIT;
        return CosNA::SupplierAdmin::_duplicate( m_defaultSupplierAdmin );
        // ++limin CL-21308
        // return m_supplierChannel->default_supplier_admin();
    }


    CosNF::FilterFactory_ptr FederatableEventChannel_impl::default_filter_factory( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "default_filter_factory" );

        // TODO LPP: What should we return here?
        FUNCTION_EXIT;
        return m_consumerChannel->default_filter_factory();
    }


    CosNA::ConsumerAdmin_ptr FederatableEventChannel_impl::new_for_consumers( CosNA::InterFilterGroupOperator op,
                                                                              CosNA::AdminID& id WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "new_for_consumers" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    CosNA::SupplierAdmin_ptr FederatableEventChannel_impl::new_for_suppliers( CosNA::InterFilterGroupOperator op,
                                                                              CosNA::AdminID& id WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "new_for_suppliers" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    CosNA::ConsumerAdmin_ptr FederatableEventChannel_impl::get_consumeradmin(CosNA::AdminID id  WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "get_consumeradmin" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    CosNA::SupplierAdmin_ptr FederatableEventChannel_impl::get_supplieradmin(CosNA::AdminID id  WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "get_supplieradmin" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    CosNA::AdminIDSeq* FederatableEventChannel_impl::get_all_consumeradmins( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "get_all_consumeradmins" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    CosNA::AdminIDSeq* FederatableEventChannel_impl::get_all_supplieradmins( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "get_all_supplieradmins" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    // Methods from CosN::AdminPropertiesAdmin Interface
    CosN::AdminProperties* FederatableEventChannel_impl::get_admin( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "get_admin" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    void FederatableEventChannel_impl::set_admin(const CosN::AdminProperties& admin WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "set_admin" );

        TA_Assert(0);

        FUNCTION_EXIT;
    }


    // Methods from CosN::QoSAdmin Interface
    CosN::QoSProperties* FederatableEventChannel_impl::get_qos( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "get_qos" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    void FederatableEventChannel_impl::set_qos(const CosN::QoSProperties& qos WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "set_qos" );

        TA_Assert(0);

        FUNCTION_EXIT;
    }


    void FederatableEventChannel_impl::validate_qos( const CosN::QoSProperties& r_qos,
                                                     CosN_NamedPropertyRangeSeq_outarg a_qos
                                                     WRAPPED_DECLARG )
    {
        FUNCTION_ENTRY( "validate_qos" );

        TA_Assert(0);

        FUNCTION_EXIT;
    }


    // Additional methods from AttNotification::EventChannel
    CosN::EventTypeSeq* FederatableEventChannel_impl::obtain_offered_types( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "obtain_offered_types" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    CosN::EventTypeSeq* FederatableEventChannel_impl::obtain_subscription_types( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "obtain_subscription_types" );

        TA_Assert(0);

        FUNCTION_EXIT;
        return NULL;
    }


    AttN::ChannelStats  FederatableEventChannel_impl::obtain_stats( WRAPPED_DECLARG_VOID )
    {
        FUNCTION_ENTRY( "obtain_stats" );

        //TA_Assert(0);
        FUNCTION_EXIT;
        return AttN::ChannelStats();
    }


};


