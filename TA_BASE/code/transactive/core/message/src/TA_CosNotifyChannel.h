#if !defined(TA_CosNotifyChannel_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
#define TA_CosNotifyChannel_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/TA_CosNotifyChannel.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:12:25 $
  * Last modified by:  $Author: limin.zhu $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include <set>
#include <vector>
#include "COS/CosNotifyChannelAdmin.hh"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/corba/src/ServantBase.h"
#include "core/message/src/TA_CosNotifyInterfaces.h"
#include "core/utilities/src/DebugUtil.h"
#include <sys/timeb.h>


namespace TA_Base_Core
{
    // OMNINOTIFY GARBAGE COLLECTION:
    // OmniNotify objects will be garbage collected UNLESS at least one of the conditions below
    // is satisfied for the given object type (see the excerpt from standard.cfg at the end of
    // this file):
    // Non-default channel (and its default admins):
    // - default admin has connected proxies, or
    // - channel or default admins have been used recently, or
    // - non-default admins exist
    // Non-default admin:
    // - has connected proxies, or
    // - has been used recently
    // Proxy object (ie. consumer or supplier):
    // - has been used recently
    // Filter object:
    // - is attached to a proxy or admin, or
    // - has been used recently

    // It is not clear what having been 'used' actually means, but we will assume that
    // it includes calling any method in the public interface, and in the case of a 
    // proxy object either:
    // - a client pushing an event to it (for a ProxyConsumer)
    // - a client receiving an event from it (for a ProxySupplier)
    // TODO LPP: Are these assumptions correct (particularly those regarding proxy objects)?

    // To enable garbage collection, DeadConProxyInterval in omni_notification.cfg needs to be 
    // set to a non-zero value.  It should be larger than DEFAULT_KEEPALIVE_SECS.

    // We seem to get an OBJ_ADAPTER_NoServant exception if an object we are trying to dereference
    // has been garbage collected.  We can deal with this if we want by catching the exception:
    // catch (const CORBA::OBJ_ADAPTER& e)
    // {
    //     if (e.minor() == omni::OBJ_ADAPTER_NoServant) 
    //     {
    //         ...
    //     }
    // }


    // SPECIFYING AN APPROPRIATE KEEPALIVE INTERVAL:
    // If the client's connection fails, the keep alive messages will not be received by the
    // event channel and the proxy objects will be garbage collected.  When the channel later
    // becomes available again, the proxy objects will be invalid.
    // The connection needs to be monitored periodically and reset if a failure is detected
    // (the ChannelLocatorConnectionMgr can be used for this purpose).  It is important to
    // choose appropriate intervals for the garbage collection interval (DeadConProxyInterval),
    // the keep alive interval (DEFAULT_KEEPALIVE_SECS) and the connection monitoring 
    // interval (eg. RESOLUTION_SUCCESS_DELAY_MS).
    //
    //                   gi (garbage collection interval)
    //     |<--------------------------------------------------------->|
    //              ki (keepalive interval)          ci (max connection check interval)
    //     |<----------------------------------->|<------------------->|
    //
    //     |                                     NETWORK FAILS         |  
    //     |-------------------------------------//////////////////////|  PROXIES INVALID
    //     |                                    | |                  | |
    //     ^ KeepAlive call                     | ^ KeepAlive call   | ^ Garbage Collector runs
    //       SUCCEEDS                           |   FAILS            |
    //                                          |                    |
    //                                          ^ Connection Check   ^ Connection Check
    //                                            SUCCEEDS             FAILS
    // The general requirement is for:
    // ci + ki < gi

    // TODO LPP: What if the network fails every time the KeepAlive is called, but it is
    // available every time the connection check is done?  Our model breaks down.


    // CosProxyKeepAlive periodically calls a method on the proxy object to ensure that it
    // is not garbage collected by omniNotify. 
    // TODO LPP: Each CosProxyKeepAlive has it's own thread, which may be quite resource-intensive.
    // Is it possible to share threads between all objects with the same keepalive interval?

    // NOTE: if "omni_notification.cfg: DeadConProxyInterval" changed, must change these 3 const variables
    const unsigned long DEAD_CON_PROXY_INTERVAL        = 180; // 180
    const unsigned long DEFAULT_KEEPALIVE_SUCCESS_SECS = DEAD_CON_PROXY_INTERVAL / 3;
    const unsigned long DEFAULT_KEEPALIVE_FAIL_SECS    = DEAD_CON_PROXY_INTERVAL / 18;


    // NOTE: This worker thread must be started and stopped explicitly by the client.

    // TODO LPP: CosProxyKeepAlive could be refactored into a much more general purpose
    // PeriodicTimer.  In fact, it is very similar to utility.AlarmClock, except it provides a
    // continuous stream of notifications, whereas AlarmClock is a single shot timer.  It is 
    // also similar to security.KeepSessionAliveThread.  We should merge these concepts. 
    class CosProxyKeepAlive : public Thread
    {
    public:
        // A keepalive message will be sent to the proxy every keepAliveSecs.
        // keepAliveSecs should be less than DeadConProxyInterval in the omniNotify
        // configuration file.
        CosProxyKeepAlive(unsigned int keepAliveSecs);
        ~CosProxyKeepAlive();

        // FilterAdmin is a common base class of ProxyConsumer and ProxySupplier.  We will call 
        // FilterAdmin::get_filter() on the proxy object to keep it alive (which should be fairly 
        // quick to execute).
        // The proxy can be set to _nil().
        void setProxy(CosNotifyFilter::FilterAdmin_ptr proxy);

        void setServiceAddr( const std::string& serviceAddr );
        // Allows the keepalive timer to be restarted so that the keepalive method is not 
        // called.  NOTE that the keepalive method won't be called if resetTimer() is called 
        // more frequently than the keepalive interval (keepAliveSecs).
        void resetTimer() const;

        // Implementation of TA_Base_Core::Thread
		virtual void run();
		virtual void terminate();

    private:

        void callProxyMethod();
        
        void startTiming();
        unsigned long timeElapsed();

    private:

        unsigned int m_keepAliveSecs;

		NonReEntrantThreadLockable m_proxyLock;
        CosNotifyFilter::FilterAdmin_var m_proxy;

        // true if the thread is running.
		volatile bool m_running;
        volatile bool m_terminated;
		mutable Condition m_condition;
        timeb m_startTime;
        std::string m_serviceAddr;
    };
    

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // IDs in the Corba notification service:
    // The Corba Notification Service uses unique IDs to identify objects, rather than pointer
    // values as in C++.  For example, after a filter is added to a ProxySupplier it must be 
    // identified by a FilterID when it is to be subsequently removed.  Presumably this is
    // because object identity based on the address of the local proxy doesn't work for 
    // distributed objects.
    // This makes it necessary to store the ID of any objects of interest, and possibly also
    // a Corba _ptr or _var smart pointer to the actual object as well, so that methods can
    // be called on the object.  For example, CosNotifyFilter::FilterID and CosNotifyFilter::Filter_ptr.
    // Although it is typically possible to obtain a pointer to an object given it's ID,
    // this is unlikely to be as efficient.


    // SERVER-SIDE FILTERS 
    // The filter criteria (ie. the logical expression evaluated by the filter) is 
    // represented by one or more Filter objects that are added to a FilterAdmin 
    // object.  The Omni notification service is apparently much faster when a single 
    // filter with a large number of filter constraints is used than if a large number 
    // of filters, each with simple constraints, is added to a ProxySupplier.  Note 
    // that OR semantics are applied between the ConstraintExp objects, and also
    // between multiple Filter objects.  

    // The constraint is of the form event_type_filter AND constraint_expression where 
    // the (domain_name, type_name) of the event has to match any of the event types
    // in the event_type_filter (ie. the event types are ORed).  An example of a 
    // constraint_expression is "($priority = '3') and ($subsystem != 'power')"
    // $name is searched for in $.header.variable_header (optional header), then 
    // $.header.filterable_data (see "2.4.5 A Short-hand Notation for Filtering a 
    // Generic Event" in the Corba Notification Service specification for more information).

    // The general form of the expression evaluated by an individual filter is therefore:
    // ( (event_type_1a OR event_type_1b OR ...) AND constraint_expr_1 ) OR           - ConstraintExp object 1
    // ( (event_type_2a OR event_type_2b OR ...) AND constraint_expr_2 ) OR           - ConstraintExp object 2
    // ...                                                                            - ...

    //  struct ConstraintExp
    //  {
    //      CosNotification::EventTypeSeq event_types
    //      string constraint_expr;
    //  };

    // Any fields in the header of an event that are not in the constraint expression 
    // will be ignored by the server-side filter.  However, any fields in the 
    // constraint expression that aren't in the header of the event will result in 
    // an error when the particular field constraint is evaluated (short circuit 
    // semantics apply to the logical operators), and the filter will reject the
    // event.  It is possible to ignore missing fields by using a constraint expression 
    // of the form (exist $field and $field == value).  For example, for the event
    // (fld1, 10), (fld3, 5),
    // ($fld1 > 32) or ($fld2 == 5) or ($fld3 > 3) will return false because fld2 is
    // missing in the event header, but
    // ($fld1 > 32) or (exist $fld2 and $fld2 == 5) or ($fld3 > 3) will return true.

    // TODO LPP: Check the validity of the above statement.


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // TODO LPP: Can CosNotifyChannelConsumer and CosNotifyChannelSupplier log a warning if the
    // proxy is not available because it has been garbage collected?  
    // TODO LPP: Is there some way of allowing a client to add the CosFilterSet and CosProxyKeepAlive 
    // behaviour to CosNotifyChannelConsumer and CosNotifyChannelSupplier externally if the client needs 
    // this functionality?  

    // CosNotifyChannelConsumer connects to an event channel as a SequencePushConsumer and
    // forwards batches of events to a handler specified by the client.  
    class CosNotifyChannelConsumer : public virtual POA_CosNotifyComm::SequencePushConsumer,
                                     public ServantBase
    {
    public:
        // The CORBA servant is activated automatically by the ctor.
        CosNotifyChannelConsumer();

        // The ConsumerAdmin must already have been set to _nil by calling setEventChannel()
        // The CORBA servant is deactivated automatically by the dtor.
        ~CosNotifyChannelConsumer();

        // Events that are received from the event channel are forwarded to the handler.
        // Events are discarded if no handler has been set, or if IEventHandler::onEventsReceived()
        // returns a value indicating failure to process some events in the batch.
        // CosNotifyChannelConsumer does NOT /*take*/ IEventHandler, so don't forget to delete it 
        // (this is done so that a class that has-a CosNotifyChannelConsumer can implement IEventHandler).
        void setEventHandler(IEventHandler* handler);
        IEventHandler* getEventHandler() const;

        // We allow the client to specify the ConsumerAdmin that this PushConsumer will be connected to,
        // rather than assuming that it is the default_consumer_admin - this unambiguously specifies 
        // which EventChannel the consumer is connected to. setEventChannel() removes all constraints.

        // If cleanDisconnect is true, setEventChannel() will try to disconnect cleanly 
        // from the current event channel (ie. remote calls will be made).  Specify false if 
        // the channel is known to be inaccessible.
        // The consumerAdmin can be set to CosNotifyChannelAdmin::ConsumerAdmin::_nil() to disconnect
        // the consumer from the event channel.
        // false is returned if the admin was not set successfully.  Will NOT throw exceptions.
		bool setEventChannel(const CosNotifyChannelAdmin::ConsumerAdmin_ptr consumerAdmin, bool cleanDisconnect = true);

        // Returns true if a non-nil event channel has been set.  Note that this doesn't necessarily 
        // mean that the channel is reachable.
        bool hasEventChannel() const;
        
        // We expose a single filter object for clients to add constraint expressions to
        // (it is apparently more efficient to use a single filter with a large number of
        // constraints, than a large number of filters each with a single constraint).
        // nil() will be returned if no event channel has been set.
        // TODO LPP: Is there likely to be a problem with calling setEventChannel() concurrently 
        // while the filter is being manipulated?
        CosNotifyFilter::Filter_ptr getFilter() const;

        // Implementation of CosNotifyComm::SequencePushConsumer
		virtual void push_structured_events(const CosNotification::EventBatch& batch);
		virtual void disconnect_sequence_push_consumer() {}
		virtual void offer_change(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed) {}

        void setServiceAddr( const std::string& serviceAddr );

    private:
        // Protects m_handler
		mutable NonReEntrantThreadLockable m_eventHandlerLock;
        IEventHandler* m_eventHandler;

        // Protects m_consumerAdmin, m_proxySupplier, m_filter, m_filterID.
		mutable NonReEntrantThreadLockable m_connectionLock;

        // Resets all connection state.  Can be used to disconnect from the event channel without
        // making any remote calls.
        void setNilConsumerAdmin();

		CosNotifyChannelAdmin::ConsumerAdmin_var m_consumerAdmin;

        // Cleanly disconnects from the event channel (remote calls will be made).
        // Exceptions will not be thrown.
        void doCleanDisconnect();

        bool createAndSetProxySupplier();
		CosNotifyChannelAdmin::SequenceProxyPushSupplier_var m_proxySupplier;

        // The filter is attached to the ProxySupplier.
		CosNotifyFilter::Filter_var m_filter;
		CosNotifyFilter::FilterID m_filterID;

        CosProxyKeepAlive m_keepAlive;

         CosNotifyComm::SequencePushConsumer_var m_myObjectRef;
    };
    

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // CosNotifyComm::NotifySubscribe::subscription_change() notifications:    
    // The subscription_change() notification seems to be sent by an EventChannel to all
    // suppliers to the channel when a consumer adds an event type (ie. domain_name, type_name)
    // to the CosNotifyFilter that hasn't been added before.  It is likely that the
    // EventChannel maintains a set of event types that have been subscribed to, and maintains
    // a reference count for each event type, so it is able to determine whether 
    // subscription_change() needs to be sent.
    // Once a subscription_change() is sent for a particular event type, it doesn't seem
    // to be sent again.  It is necessary to call obtain_subscription_types() to retrieve
    // all of the event types that consumers have registered interest in.  Note that
    // StructuredEventSupplier calls obtain_subscription_types() just after it has
    // connected to the EventChannel, then passes the event types to the handler
    // by calling onSubscriptionChange().

    // NOTE that the subscription_change() notification that is forwarded to suppliers seems to 
    // convert (*, *) to (, %ALL).  Because our handling of (*, *) equivalent forms is not 
    // entirely correct, we currently convert (, %ALL) to (*, *) when sending onSubscriptionChange()
    // notifications.
    // TODO LPP: We need to review our handling of (, %ALL).

    // subscription_change() FORWARDING:
    // It is necessary to forward subscription_change notifications in the opposite
    // direction to events when federating between event channels.  The event channel 
    // calls subscription_change() on suppliers in response to changes made to 
    // consumer Filter objects.  It is not possible for a client to call subscription_change 
    // directly, so a federation client needs to modify the Filter object to propagate
    // the subscription_change() notification.  


    // CosNotifyChannelSupplier connects to an event channel as a SequencePushSupplier and
    // allows the client to push events onto that channel.  Subscription change notifications
    // are forwarded to the ISubChangeHandler.
    class CosNotifyChannelSupplier : public virtual POA_CosNotifyComm::SequencePushSupplier,
                                     public ServantBase
    {
    public:
        // The CORBA servant is activated automatically by the ctor.
        CosNotifyChannelSupplier();

        // The SupplierAdmin must already have been set to _nil by calling setEventChannel().
        // The CORBA servant is deactivated automatically by the ctor.
        ~CosNotifyChannelSupplier();

        // subscription_change notifications that are received from the event channel are forwarded 
        // to the handler.  The handler can be NULL.  
        // CosNotifyChannelSupplier does NOT /*take*/ ISubChangeHandler, so don't forget to delete it
        // (this is done so that a class that has-a CosNotifyChannelSupplier can implement ISubChangeHandler).
        void setSubChangeHandler(ISubChangeHandler* handler);
        ISubChangeHandler* getSubChangeHandler() const;

        // We allow the client to specify the SupplierAdmin that this PushSupplier will be connected to,
        // rather than assuming that it is the default_supplier_admin - this unambiguously specifies 
        // which EventChannel the consumer is connected to. 

        // If cleanDisconnect is true, setEventChannel() will try to disconnect cleanly 
        // from the current event channel (ie. remote calls will be made).  Specify false if 
        // the channel is known to be inaccessible.
        // The SupplierAdmin can be set to CosNotifyChannelAdmin::SupplierAdmin::_nil() to disconnect
        // the supplier from the event channel.
        // ISubChangeHandler::onSubscriptionReset() and/or ISubChangeHandler::onSubscriptionChange()
        // may be sent.
        // false is returned if the admin was not set successfully.  Will NOT throw exceptions.
		bool setEventChannel(const CosNotifyChannelAdmin::SupplierAdmin_ptr supplierAdmin, bool cleanDisconnect = true);

        void setServiceAddr( const std::string& serviceAddr );
        
        // Returns true if a non-nil event channel has been set.  Note that this doesn't necessarily 
        // mean that the channel is reachable.
        bool hasEventChannel() const;

        // We expose a single filter object for clients to add constraint expressions to
        // (it is apparently more efficient to use a single filter with a large number of
        // constraints, than a large number of filters each with a single constraint).
        // nil() will be returned if no event channel has been set.
        // TODO LPP: Is there likely to be a problem with calling setEventChannel() concurrently 
        // while the filter is being manipulated?
        CosNotifyFilter::Filter_ptr getFilter() const;

		// Pushes the events onto the event channel (without queuing them), returning true if 
        // all events were successfully pushed.  Will NOT throw exceptions.
        // Must not be called unless an event channel has been sent, and 'events' contains at least one event.
		bool publishEvents(const std::vector<const CosNotification::StructuredEvent*>& events) const;
        bool publishEvents(const CosNotification::EventBatch& eventBatch) const;

        // Implementation of POA_CosNotifyComm::SequencePushSupplier
		virtual void subscription_change(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed); 
		virtual void disconnect_sequence_push_supplier();

    private:
        // THREAD SAFETY: If both m_connectionLock and m_handler need to be acquired, they MUST
        // be acquired in this order to avoid deadlock.

        // Protects m_handler
		mutable NonReEntrantThreadLockable m_subChangeHandlerLock;
        ISubChangeHandler* m_subChangeHandler;

        // Protects m_supplierAdmin, m_proxyConsumer, m_filter, m_filterID.
		mutable NonReEntrantThreadLockable m_connectionLock;

        // Resets all connection state.  Can be used to disconnect from the event channel without
        // making any remote calls.
        void setNilSupplierAdmin();

		CosNotifyChannelAdmin::SupplierAdmin_var m_supplierAdmin;

        // Cleanly disconnects from the event channel (remote calls will be made).
        // Exceptions will not be thrown.
        void doCleanDisconnect();

        bool createAndSetProxyConsumer();
		CosNotifyChannelAdmin::SequenceProxyPushConsumer_var m_proxyConsumer;

        // The filter is attached to the ProxyConsumer.
		CosNotifyFilter::Filter_var m_filter;
		CosNotifyFilter::FilterID m_filterID;

        CosProxyKeepAlive m_keepAlive;

        CosNotifyComm::SequencePushSupplier_var m_myObjectRef;

		// Enables automatic sending of subscription_change() updates by the event channel,
        // at the same time populating 'initialTypes' with the current set of event types 
        // that existing consumers of the event channel are subscribed to (the Corba 
        // notification service will only send subsequent delta notifications).  
        // true is returned if the operation was successful.
        bool enableSubChangeUpdates(CosNotification::EventTypeSeq& initialTypes);

        // Helper that sends ISubChangeHandler::onSubscriptionChange() to m_handler, enforcing 
        // the invariants.  Also converts the (, %ALL) event type to (*, *) so that we avoid
        // issues with our inconsistent handling of (*, *) equivalent forms and with OmniNotify
        // 2.1's constraint handling.
	    void notifySubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed);
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////
        
    // Forwards onSubscriptionChange() to each of the child handlers.
    class SubChangeHandlerFan : public ISubChangeHandler
    {
    public:
        // addHandler() must not be called if child has already been added, and
        // removeHandler() must not be called if child has already been removed.
        // The client keeps ownership of the child ISubChangeHandlers.
        void addHandler(ISubChangeHandler* child);
        void removeHandler(ISubChangeHandler* child);

        // Implementation of ISubChangeHandler
		virtual void onSubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed);
        virtual void onSubscriptionReset();

    private:
        std::set<ISubChangeHandler*> m_handlers;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // Helper class that manages the constraint expressions that are added to a Filter.
    // Arbitrary ConstraintExp expressions can be added to the filter.  The expressions are
    // reference counted.  Rather than comparing ConstraintExp objects to determine equality
    // (for reference counting), a client-provided (clientId, exprId) pair is used to identify 
    // expressions.  Constraints sharing the same (clientId, exprId) are considered to have the 
    // SAME contraint expression (NOTE that the ConstraintExp objects are NOT compared).  
    // Clients have to manage the association between ConstraintExp objects and (clientId, exprId)
    // pairs.

    // TODO LPP: It is ugly that we provide a clientId.  This is a hack to allow a single
    // RefCountedConstraintSet to be shared by multiple clients - it provides a separate
    // exprId space for each client.

    // OmniNotify 2.1 %ALL constraint bug:
    // When a consumer subscribes to all events by adding a ConstraintExp containing the
    // event type (*, *) to the Filter object, the event channel will forward all events 
    // to it.  (, %ALL) is supposed to be equivalent to (*, *), but OmniNotify 2.1 doesn't 
    // seem to consider (, %ALL) a match for any event type (perhaps there is a bug in OmniNotify 
    // filtering).  
    // The RefCountedConstraintSet disallows the (, %ALL) event type to highlight this problem.
    // TODO LPP: Determine what the cause of this bug is (ie. is it a problem with filtering).

    class RefCountedConstraintSet
    {
    public:
        // setFilter() must be called before calling addExpression() or removeExpression().
        // setFilter() will clear the map, but doesn't try to remove any constraints.  
        // 'filter' can be nil.
        void setFilter(CosNotifyFilter::Filter_ptr filter);

        // Add the constraint to the Filter if it hasn't been added already, otherwise increment
        // the reference count (and ignore the constraint argument).  (clientId, exprId) is used 
        // to detect common constraints - a constraint is only added to the Filter if it's 
        // (clientId, exprId) hasn't been seen before (the constraints are NOT compared by 
        // addExpression).  CAN throw corba exceptions.
        // NOTE: (, %ALL) must NOT be in the list of event types (part of the workaround
        // for the OmniNotify 2.1 %ALL constraint bug).  
		void addExpression( unsigned int clientId, unsigned int exprId, const CosNotifyFilter::ConstraintExp& constraint );

        // Decrement the reference count for the constraint, and remove the ConstraintExp from the 
        // filter if the reference count falls to zero.
        // CAN throw corba exceptions.  
		void removeExpression( unsigned int clientId, unsigned int exprId );

        // Clears the map and removes the constraints from the filter.
        // CAN throw corba exceptions.  
        void removeAllExpressions();

    private:

        CosNotifyFilter::Filter_var m_filter;

        struct ConstraintCountDetail
        {
            // Initial count is 1 as the constraint is valid when ConstraintCountDetail is 
            // constructed (otherwise we wouldn't have an ID).
            ConstraintCountDetail(CosNotifyFilter::ConstraintID constraintID) :
                m_count(1), m_constraintID(constraintID)
            {
            }

            // The number of times the constraint expression has been added.
            unsigned int m_count;

            // The ID of the constraint, used when removing it from m_filter.
            CosNotifyFilter::ConstraintID m_constraintID;
        };

	    // (clientId, exprId) -> ConstraintCountDetail
	    typedef std::map< std::pair<unsigned int, unsigned int>, ConstraintCountDetail > ExpressionIDToCountMap;
		ExpressionIDToCountMap m_counts;
        TA_Base_Core::NonReEntrantThreadLockable m_countsLock;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // Names of fields added to the variable_header by the framework
    extern const char* SEQ_NUM_FIELDNAME;           // "seq_num" - unsigned long
    extern const char* EVENT_SUPPLIER_FIELDNAME;    // "event_supplier" - char*

    // Appends a pair of fields (SEQ_NUM_FIELDNAME, EVENT_SUPPLIER_FIELDNAME) to the
    // variable header of each event, containing an incrementing sequence number
    // (as unsigned long) and the identity of this appender (as char*).  
    // Note that existing fields with these names will NOT be replaced - new fields will
    // be appended. Each SeqNumAppender has it's own sequence number 'space', so the
    // pair (appenderId, seqNum) should be unique.
    
    // If a process restarts and new SeqNumAppender instances are created, the appenderIds
    // will be different.  It is necessary to realise that:
    // - the seqNum of subsequent events from a process that has restarted will restart from 1,
    //   but won't clash with previous sequence numbers because the (appenderId, seqNum) pair
    //   will still be unique.
    // - there is no relationship between the appenderId of the old (dead) appender and the 
    //   new appender.
    // - no further events will be received that have the appenderId of the dead appender.

    class SeqNumAppender
    {
    public:
        SeqNumAppender();
        
        // Appends (SEQ_NUM_FIELDNAME, EVENT_SUPPLIER_FIELDNAME) to the variable_header.
        // appendToHeader() is threadsafe.
        void appendToHeader(CosNotification::StructuredEvent& event);

    private:
        unsigned long m_nextSeqNum;
		TA_Base_Core::NonReEntrantThreadLockable m_lock;

        // appenderId must be a unique value across all SeqNumAppenders (in all processes)
        // and across time.  A UUID is a satisfactory id - ours is created using 
        // TA_Uuid::generateUUID().
        std::string m_appenderId;

    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // Examines the variable header of each event for a field with the name SEQ_NUM_FIELDNAME 
    // at a specified position (which allows for multiple occurrences of this field).  The
    // field EVENT_SUPPLIER_FIELDNAME is required to immediately follow SEQ_NUM_FIELDNAME.
    
    // Sequence numbers with a given appenderId should always be increasing (but won't necessarily
    // be sequential (eg. 1,4,7) because some events may have been filtered out).  If the sequence
    // number remains the same or decreases, duplicate or out of order events have been received.
    // Various actions may be sensible when this happens.  For example, it may violate a requirement
    // of the system, so an exception might be thrown or the process aborted.  Alternatively, the 
    // duplicate events might be ignored, necessary when events are delivered by redundant pathways.

    // (appenderId, lastSeqNum) pairs will be discarded if the interval APPENDERID_GC_SECS
    // expires with no change occurring to lastSeqNum during that interval.
    
    // NOTE that we only store the LAST seqNum seen for a given appenderId, so it is possible
    // that out-of-order events that haven't been received are discarded - but it is reasonable
    // to require that events will be delivered IN ORDER.

    const int APPENDERID_GC_SECS = 120;

    // NOTE: This worker thread must be started and stopped explicitly by the client.

    class SeqNumFilter : public Thread
    {
    public:

        typedef enum
        {
            EVENT_IN_ORDER = 0,
            EVENT_DUPLICATE = 1,
            EVENT_OUT_OF_ORDER = 2
        } EEventOrderStatus;

        // Events will be discarded if either handler is NULL.
        // fieldPosition - the position of the fields (SEQ_NUM_FIELDNAME, EVENT_SUPPLIER_FIELDNAME)
        // in the variable header.  This is not an absolute position - only occurrences of these 
        // fields are considered.  The position is taken from the first leftmost occurrence if
        // fieldPosition is positive, otherwise it is from the from the rightmost occurrence.
        // fieldPosition must not be 0.
        // eg. if snx denotes the pair of sequence number fields, and other letters denote other
        // unrelated fields, in (sn1, b, sn2, d, sn3), fieldPosition 1 == sn1, 2 == sn2 (not b), -1 == sn3        
        SeqNumFilter(int fieldPosition);

        SeqNumFilter( const SeqNumFilter& rhs );
        SeqNumFilter& operator =( const SeqNumFilter& rhs );
        
        // Returns true if the (appenderId, seqNum) pair has been seen before.
        // If the pair hasn't been seen before, m_nextSeqNumMap will be updated and 
        // false will be returned.  
        EEventOrderStatus isDuplicateEvent(const CosNotification::StructuredEvent& event );

        // TODO LPP: Need to work out how to flush expired map entries (and how to identify 
        // their expiry time).
        // TODO LPP: We could stop the worker thread when all entries have been flushed from
        // the map (the client would still need to stop it manually during shutdown).

        // Implementation of TA_Base_Core::Thread
		virtual void run();
		virtual void terminate();

    private:
        int m_fieldPosition;

        // appender id -> next sequence number
        // (ie. the next sequence number should be greater than or equal to the value in the map.
        typedef std::map<std::string, unsigned int> SeqNumMapType;
        SeqNumMapType m_nextSeqNumMap;

        // true if the thread is running.
		volatile bool m_running;
		//Condition m_condition;

		TA_Base_Core::NonReEntrantThreadLockable m_lock;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // Populates the constraint_expr and event_types members of 'constraint' with 
    // the expression "(eventType) AND true".
    void gPopulateConstraintWithEventType(const CosNotification::EventType& eventType, CosNotifyFilter::ConstraintExp& constraint);


    // Represents a set of EventTypes, allowing clients to add and remove them from the
    // set, and to determine whether a given EventType matches any of those in the set.
    // The match must be exact (it doesn't use the * wildcard).  For example, (*, x) 
    // will match (*, x) but not (a, x).
    // Each distinct event type is associated with an integer key.
    const unsigned int NOT_IN_SET = 0;

    class EventTypeSet
    {
    public:
        EventTypeSet();

        // Returns true if the event type is in the set.
        bool isMatchingEventType(const CosNotification::EventType& eventType) const;

        // Returns the key for the event type if it is in the set, otherwise returns NOT_IN_SET.
        unsigned int getEventTypeKey(const CosNotification::EventType& eventType) const;

        // Adds an event type to the set of event types.  Returns a key that identifies
        // the event type.  The event type must not have already been added.
		unsigned int addEventType( const CosNotification::EventType& eventType );

        // Removes an event type from the set of event types.  Returns the key that was
        // associated with the event type.  The event type must have already been added.
		unsigned int removeEventType( const CosNotification::EventType& eventType );

        // Clears all of the event types in the set. Key numbering will also be invalidated.
        void removeAllEventTypes();

        // Returns all of the keys for the event types that have been added to this set.
        // TODO LPP: This seems to be a hack.  Perhaps exposing iterators would be better.
        void getEventTypeKeys(std::vector<unsigned int>& vec) const;

    private:
		mutable TA_Base_Core::NonReEntrantThreadLockable m_lock;

        // (domain_name, type_name) -> Id
        typedef std::pair<std::string, std::string> EventTypeKey;
		typedef std::map<EventTypeKey, unsigned int> EventTypeToIDMap;
		EventTypeToIDMap m_eventTypeToIDMap;

        unsigned int m_nextID;
    };


    // Represents a set of EventTypes, allowing clients to add and remove them from the
    // set and to determine whether a given EventType matches any of those in the set.
    // The match uses wildcard substitution.  For example, (x, *) will match
    // (x, a) or (x, b), but not (y, a).
    class WildcardEventTypeSet
    {
    public:
        WildcardEventTypeSet();
        
        // Returns true if the event is in the set of events that have been added to the
        // set by addEventType().
        bool isMatchingEventType(const CosNotification::EventType& eventType) const;

        // TODO LPP: We probably need to be more careful about adding and removing 
        // variants of (*, *).  For example, if (*, *) and (, %ALL) are added, 
        // but only one of these is removed, the WildcardEventTypeSet will reset
        // m_subscribedToAll.  Perhaps we need to use ref counting.

        // Adds an event type to the event types in the set.  Variants of (*, *)
        // are treated equivalently.
        // TODO LPP: We don't currently handle (*, x) event types.
		void addEventType( const CosNotification::EventType& eventType );

        // Removes an event type from the event types in the set.  Variants of (*, *)
        // are treated equivalently.
		void removeEventType( const CosNotification::EventType& eventType  );

        // Remove all of the event types from the set.
        void removeAllEventTypes();

    private:
        // Event types in the set are tracked using a three-level data structure.
        // If all event types in all domains are subscribed to, (*, *), m_subscribedToAll will 
        // be true.
        // If all event types in a particular domain are subscribed to, (domain_name, *), 
        // m_subscribedDomains[domain_name] will be ALL.
        // Otherwise, if one or more specific event types are subscribed to, (domain_name, type_name), 
        // m_subscribedDomains[domain_name] will be TYPE and m_subscribedTypes will contain the
        // event types.

        // Note that removing (*, *) doesn't affect any (domain_name, *) or (domain_name, type_name) 
        // entries, and removing (domain_name, *) doesn't affect any (domain_name, type_name) 
        // entries.

        // true if all event types are subscribed to.  This corresponds to the (*, *) subscription.
        // ie. all event types in all domains.
		bool m_subscribedToAll;

		enum DomainSubscriptionType
		{
			TYPE = 0,   // Subscribed to one or more specific types in the domain_name 
			ALL         // Subscribed to all event types in the domain_name
		};

        // domain_name -> DomainSubscriptionType
		typedef std::map<std::string, DomainSubscriptionType> SubscribedDomainsMap;
		SubscribedDomainsMap m_subscribedDomains;

        // Contains all specific event types that are subscribed to.  These don't include
        // (*, *) and (domain_name, *) event types.  Note that a dominating subscription
        // doesn't remove any event types from m_subscribedTypes.
		EventTypeSet m_subscribedTypes;

        // Synchronises access to the above data structure.
		mutable TA_Base_Core::NonReEntrantThreadLockable m_lock;
    };


    // The filter keeps track of which event types consumers of an event channel 
    // are subscribed to, updating this information when onSubscriptionChange() 
    // is received.
    class SubscribedEventFilter : public ISubChangeHandler
    {
    public:
        // Returns true if the event is in the set of events that have been added to the
        // subscription by onSubscriptionChange().
        bool isSubscribedEvent(const CosNotification::StructuredEvent& event) const;

        // Implementation of ISubChangeHandler
		virtual void onSubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed);
        virtual void onSubscriptionReset();

    private:
        WildcardEventTypeSet m_eventTypeSet;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // EVENT CHANNEL TO ADMIN OBJECT MAPPING
    // IAdminProvider provides a mapping from an event channel to the appropriate supplier
    // and consumer admin objects.  This is useful when we have an EventChannel reference,
    // but the default admins are not necessarily appropriate.
    struct IAdminProvider
    {
        // NOTE that 'channel' must not be NULL, but is not necessarily reachable.  NULL should be returned
        // if the admin object couldn't be resolved (eg. because the channel wasn't reachable).  
        // Exceptions must NOT be thrown.
        virtual const CosNotifyChannelAdmin::SupplierAdmin_ptr getSupplierAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const = 0;
        virtual const CosNotifyChannelAdmin::ConsumerAdmin_ptr getConsumerAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const = 0;
    
        virtual ~IAdminProvider() {}
    };

    // Returns the default admin objects associated with the event channel
    // (ie. default_supplier_admin(), default_consumer_admin() ).
    class DefaultAdminProvider : public IAdminProvider
    {
    public:
        // Implementation of IAdminProvider
        virtual const CosNotifyChannelAdmin::SupplierAdmin_ptr getSupplierAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const;
        virtual const CosNotifyChannelAdmin::ConsumerAdmin_ptr getConsumerAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const;
    };

}; // TA_Base_Core


#if 0
// The following description of OmniNotify garbage collection is from the 
// standard.cfg configuration file:

// Object Garbage collection
// -------------------------
//
// N.B.: If omniNotify is compiled with -DNO_OBJ_GC, then no object garbage
// collection is performed and the following parameters are not available.
//
// The channel, admin, proxy, and filter objects that are constructed by notifd
// sometimes become garbage: applications stop using them without properly
// destroying them.  E.g., a client could create an admin and a proxy and then
// crash before it has a chance to use or destroy these objects.  Unlike event
// queue garbage collection, it is hard or impossible for the notifd to know that
// an object is garbage: in some application scenarios, an object that is not
// used for more than 10 minutes should be considered garbage, while for other
// applications there might be valid objects that are only used once every 3
// days.  Each channel has an object garbage collection thread that is controlled
// by the following parameters.
//
// ObjectGCPeriod: period in seconds between object garbage collection steps.
//                 Set to 0 to disable.
//
// DeadChanInterval, DeadAdminInterval, DeadConProxyInterval,
// DeadOtherProxyInteral, DeadFilterInterval: if non-zero, these intervals (in
// seconds) specify how much time must pass since the last 'use' of one of these
// types of objects before the object can be considered garbage.  For proxies,
// DeadConProxyInterval specifies the interval to use for connected proxies, and
// DeadOtherProxyInterval specifies the interval to use for other proxies.  A
// proxy is not connected if (a) it has never been connected or (b) there has
// been a communication failure between the proxy and its connected supplier or
// consumer.
// 
// Rules for object garbage collection:
//
//   1. The default channel is never destroyed by the object garbage collector.
//
//   2. The default admins of the default channel are never destroyed by
//      the object garbage collector.
//
//   3. A proxy is destroyed by the object garbage collector
//      if all of the following hold:
//          A. EITHER it is connected and has not been used for more than DeadConProxyInterval seconds
//             OR it is not connected and has not been used for more than DeadOtherProxyInterval seconds.
// 
//          Note: if DeadConProxyInterval is set to zero, no connected proxies will be garbage collected.
//          If DeadOtherProxyInterval is set to zero, no non-connected proxies will be garbage collected.
//
//   4. A non-default admin is destroyed by the object garbage collector
//      if all of the following hold:
//          A. It has not been used for more than DeadAdminInterval seconds.
//          B. It has no connected proxies.
// 
//          Note: if DeadAdminInterval is set to zero, no admins will be garbage collected.
//          Also note that garbage collection of proxies will enable garbage collection
//          of admins, so turning off proxy GC can disable some admin GC.
//
//   5. A non-default channel, together with its default admins, is destroyed
//      by the object garbage collector if all of the following hold:
//          A. The channel has not been used for more than DeadChanInterval seconds.
//          B. Neither default admin has been used for more than DeadAdminInterval seconds.
//          C. Neither default admin has any connected proxies.
//          D. There are no non-default admins.
//
//          Note: if DeadAdminInterval or DeadChanInterval is set to zero, no
//          channels will be garbage collected.  Also note that garbage
//          collection of admins will enable garbage collection of channels, so
//          turning off admin GC can disable some channel GC.
//
//   6. A filter is destroyed by the object garbage collector
//      if all of the following hold:
//          A. It has not been used for more than DeadFilterInterval seconds.
//          B. It is not attached to any proxy or admin objects.
//
//          Note: if DeadFilterInterval is set to zero, no filters will be
//          garbage collected.  Also note that garbage collection of proxies and
//          admins will enable garbage collection of filters, so turning off
//          proxy or admin GC can disable some filter GC.

// Note: 600 below ==>ten minutes, 0 below ==> disabled

ObjectGCPeriod            600
DeadChanInterval            0
DeadAdminInterval         450
DeadConProxyInterval        0
DeadOtherProxyInterval    600
DeadFilterInterval        300

#endif  // #if 0 

#endif // !defined(TA_CosNotifyChannel_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
