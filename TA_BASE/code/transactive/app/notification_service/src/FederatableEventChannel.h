#ifndef EVENT_CHANNEL_WRAPPER_H
#define EVENT_CHANNEL_WRAPPER_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/FederatableEventChannel.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/12/17 15:24:44 $
  * Last modified by:  $Author: shurui.liu $
  * 
  * 
  * FederatableEventChannel_impl is an alternative implementation of CosNotifyCA::EventChannel.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "COS/AttNotification.hh"

#include "core/corba/IDL/src/IFederatableEventChannel.hh"
#include "core/message/src/TA_CosNotifyChannel.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <string>

// From omniNotify/include/CosNotifyShorthands.h


#define AttN      AttNotification
#define CosEvC    CosEventComm
#define CosEvCA   CosEventChannelAdmin
#define CosN      CosNotification
#define CosNC     CosNotifyComm
#define CosNA     CosNotifyChannelAdmin
#define CosNF     CosNotifyFilter

#define WRAPPED_DECLARG
#define WRAPPED_DECLARG_VOID void

// Useful OUT_arg typedefs
#define WRAPPED_OUTARG_TYPE(T)       T ## _out

typedef WRAPPED_OUTARG_TYPE(AttNotification::Interactive)
  AttN_Interactive_outarg;
typedef WRAPPED_OUTARG_TYPE(CosNotification::NamedPropertyRangeSeq)
  CosN_NamedPropertyRangeSeq_outarg;


namespace TA_Base_App
{
    // Returns the federatable admin objects associated with the event channel (ie. those
    // that should be used for supplying or receiving events from a remote channel - see
    // IFederatableEventChannel.idl).  The EventChannel instance MUST implement IFederatableEventChannel.
    class FederatedAdminProvider : public TA_Base_Core::IAdminProvider
    {
    public:
        // Implementation of IAdminProvider
        virtual const CosNotifyChannelAdmin::SupplierAdmin_ptr getSupplierAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const;
        virtual const CosNotifyChannelAdmin::ConsumerAdmin_ptr getConsumerAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const;
    };


    // EventChannelForwarder is a consumer of events on the supplier channel, and a supplier of
    // events to the consumer channel.  Events are forwarded from the consumer channel to the
    // supplier channel.  subscription_change() notifications are forwarded in the opposite direction
    // by modifying a filter attached to the supplier event channel. The forwarder doesn't forward
    // ALL events, only those that a consumer of the supplier-side event channel has subscribed to.

    // No queue is used, so events are not buffered, and there is no redundancy.  The event channels
    // are not monitored for availability - it is assumed that they are always available since they
    // are in the same process as the EventChannelForwarder (ie. they should be local).

    // An example where this component is useful is when implementing the dual event
    // channel used for federating between locations - refer to IFederatableEventChannel.idl.
    // The EventChannelForwarder is used to push events received on the consumer-side of the
    // dual event channel onto the supplier-side.

    // ----       CNCA::EventChannel ===> CNF::Filter                (local) supplier event channel
    // | |                |  consumerAdmin      |
    // | |                v                     |
    // | --    CosNotifyChannelConsumer   RefCountedConstraintSet / EventTypeSet
    // |                  |
    // |                  | (consumer-side - relative to ECF)
    // |                  v
    // |--       EventChannelForwarder   <===
    // |                  |                 | subscription_change() notifications
    // |                  | (supplier-side) |
    // |                  v                 |
    // | --    CosNotifyChannelSupplier  ====
    // | |                |
    // | |                v  supplierAdmin
    // ----       CNCA::EventChannel                                 (local) consumer event channel

    class EventChannelForwarder : public TA_Base_Core::IEventHandler,
                                  public TA_Base_Core::ISubChangeHandler
    {
    public:
        EventChannelForwarder();
        ~EventChannelForwarder();

        // TODO LPP: It might be worth checking that consumerAdmin and supplierAdmin don't
        // belong to the same channel.

        // Connects the EventChannelForwarder to two event channels - it is a consumer of events
        // from the consumerAdmin channel, and a supplier of events to the supplierAdmin channel.
        // The channels must be set to nil() before the ECF is destroyed.
        void setEventChannels( CosNotifyChannelAdmin::ConsumerAdmin_ptr consumerAdmin,
                               CosNotifyChannelAdmin::SupplierAdmin_ptr supplierAdmin );

        // Implementation of IEventHandler
        virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events);
        virtual bool onEventsReceived(const CosNotification::EventBatch& eventBatch);

        // Implementation of ISubChangeHandler
        virtual void onSubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed);
        virtual void onSubscriptionReset();

    private:
        TA_Base_Core::CosNotifyChannelConsumer m_channelConsumer;
        TA_Base_Core::CosNotifyChannelSupplier m_channelSupplier;

        // We need to forward subscription_change() notifications to suppliers on
        // the consumer side (relative to the ECF) event channel.  We do this by
        // modifying the CosNotifyChannelConsumer's Filter as event types are
        // added and removed.
        TA_Base_Core::RefCountedConstraintSet m_constraintSet;
        TA_Base_Core::EventTypeSet m_eventTypeSet;
    };


    // TODO LPP NOW:  Does this need to inherit from ServantBase and be activated/deactivated?

    class FederatableEventChannel_impl : public virtual POA_TA_Base_Core::IFederatableEventChannel
    {
    public:
        // Can throw CORBA exceptions.
        FederatableEventChannel_impl(CosNotifyChannelAdmin::EventChannelFactory_ptr factory);

        // Methods from IFederatableEventChannel Interface
        CosNotifyChannelAdmin::SupplierAdmin_ptr federated_supplier_admin();
        CosNotifyChannelAdmin::ConsumerAdmin_ptr federated_consumer_admin();

        // Methods from AttNotification::Interactive Interface
        char* do_command( const char* pCmd, CORBA::Boolean& success, CORBA::Boolean& target_changed,
                          AttN_Interactive_outarg next_target  WRAPPED_DECLARG );
        AttN::NameSeq* child_names( WRAPPED_DECLARG_VOID );
        AttN::NameSeq* my_name( WRAPPED_DECLARG_VOID );
        AttN::IactSeq* children(CORBA::Boolean only_cleanup_candidates  WRAPPED_DECLARG );
        CORBA::Boolean safe_cleanup( WRAPPED_DECLARG_VOID );

        // Methods from CosEvCA::EventChannel Interface
        CosEvCA::ConsumerAdmin_ptr for_consumers( WRAPPED_DECLARG_VOID );
        CosEvCA::SupplierAdmin_ptr for_suppliers( WRAPPED_DECLARG_VOID );
        void destroy( WRAPPED_DECLARG_VOID );

        // Methods from CosNA::EventChannel Interface
        CosNA::EventChannelFactory_ptr MyFactory( WRAPPED_DECLARG_VOID );
        CosNA::ConsumerAdmin_ptr default_consumer_admin( WRAPPED_DECLARG_VOID );
        CosNA::SupplierAdmin_ptr default_supplier_admin( WRAPPED_DECLARG_VOID );
        CosNF::FilterFactory_ptr default_filter_factory( WRAPPED_DECLARG_VOID );
        CosNA::ConsumerAdmin_ptr new_for_consumers( CosNA::InterFilterGroupOperator op,
                                                    CosNA::AdminID& id WRAPPED_DECLARG );
        CosNA::SupplierAdmin_ptr new_for_suppliers( CosNA::InterFilterGroupOperator op,
                                                    CosNA::AdminID& id WRAPPED_DECLARG );
        CosNA::ConsumerAdmin_ptr get_consumeradmin(CosNA::AdminID id  WRAPPED_DECLARG );
        CosNA::SupplierAdmin_ptr get_supplieradmin(CosNA::AdminID id  WRAPPED_DECLARG );
        CosNA::AdminIDSeq* get_all_consumeradmins( WRAPPED_DECLARG_VOID );
        CosNA::AdminIDSeq* get_all_supplieradmins( WRAPPED_DECLARG_VOID );

        // Methods from CosN::AdminPropertiesAdmin Interface
        CosN::AdminProperties* get_admin( WRAPPED_DECLARG_VOID );
        void set_admin(const CosN::AdminProperties& admin WRAPPED_DECLARG );

        // Methods from CosN::QoSAdmin Interface
        CosN::QoSProperties* get_qos( WRAPPED_DECLARG_VOID );
        void set_qos(const CosN::QoSProperties& qos WRAPPED_DECLARG );
        void validate_qos( const CosN::QoSProperties& r_qos,
                           CosN_NamedPropertyRangeSeq_outarg a_qos
                           WRAPPED_DECLARG );

        // Additional methods from AttNotification::EventChannel
        CosN::EventTypeSeq* obtain_offered_types( WRAPPED_DECLARG_VOID );
        CosN::EventTypeSeq* obtain_subscription_types( WRAPPED_DECLARG_VOID );
        AttN::ChannelStats  obtain_stats( WRAPPED_DECLARG_VOID );

    private:
        CosNotifyChannelAdmin::EventChannel_var m_supplierChannel;
        CosNotifyChannelAdmin::EventChannel_var m_consumerChannel;
        CosNotifyChannelAdmin::EventChannelFactory_var m_eventChannelFactory;
        
        EventChannelForwarder m_forwarder;

        CosNA::ConsumerAdmin_var m_defaultConsumerAdmin; // limin++ CL-21308
        CosNA::SupplierAdmin_var m_defaultSupplierAdmin; // limin++ CL-21308

        TA_Base_Core::NonReEntrantThreadLockable m_defaultConsumerAdminLock; // limin++ CL-21308
        TA_Base_Core::NonReEntrantThreadLockable m_defaultSupplierAdminLock; // limin++ CL-21308
    };


};

#endif 
