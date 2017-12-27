/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/TA_CosNotifyChannel.cpp $
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

#include "core/message/src/TA_CosNotifyChannel.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_String.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/uuid/src/TAuuid.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/ConnectionChecker.h"


namespace TA_Base_Core
{
    CosProxyKeepAlive::CosProxyKeepAlive(unsigned int keepAliveSecs)
        : m_keepAliveSecs(keepAliveSecs),
          m_running(false),
          m_terminated( false )
    {
        TA_Assert(keepAliveSecs != 0);
    }


    CosProxyKeepAlive::~CosProxyKeepAlive()
    {
        TA_Assert(!m_running);
    }


    void CosProxyKeepAlive::setProxy(CosNotifyFilter::FilterAdmin_ptr proxy)
    {
        LOG3(SourceInfo, DebugUtil::DebugMsg,
            "CosProxyKeepAlive::setProxy(): %s: is_nil(m_proxy) = %d --> is_nil(proxy) = %d",
            gPtrToStr(this).c_str(), CORBA::is_nil(m_proxy), CORBA::is_nil(proxy) );

        // Force the run() thread to wake up if it is currently waiting.
        ThreadGuard guard(m_condition.getLinkedThreadLockable());

        {
            TA_THREADGUARD( m_proxyLock );
            m_proxy = CosNotifyFilter::FilterAdmin::_duplicate(proxy);
        }

        m_condition.signal();
    }

    void CosProxyKeepAlive::resetTimer() const
    {
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic,
            "CosProxyKeepAlive::resetTimer(): %s will not call keep alive method", gPtrToStr(this).c_str() );

        // Force the run() thread to wake up if it is currently waiting.
        ThreadGuard guard(m_condition.getLinkedThreadLockable());
        m_condition.signal();
    }


    void CosProxyKeepAlive::run()
    {
        LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
            "CosProxyKeepAlive::run(): %s running. m_keepAliveSecs=%d", gPtrToStr(this).c_str(), m_keepAliveSecs );

        if ( false == m_terminated )
        {
            m_running = true;
        }

        while ( m_running )
        {
            bool timerExpired = false;
            {
                // The lock is released when timedWait() or wait() is called, and reacquired
                // when these methods return.  By acquiring the lock, we make sure we don't
                // miss a call to signal().
                ThreadGuard guard(m_condition.getLinkedThreadLockable());
                if ( m_running )
                {
                    if ( CORBA::is_nil(m_proxy) )
                    {
                        m_condition.wait();
                    }
                    else
                    {
                        // Note that timedWait() returns 1 if successfully signalled or 0 if time expired.
                        const int SECS_TO_MS = 1000;
                        timerExpired = !m_condition.timedWait(m_keepAliveSecs * SECS_TO_MS);
                    }
                }
            }

            // We shouldn't call the keepalive method if m_condition was signaled because
            // resetTimer() was called.
            if ( timerExpired && m_running )
            {
                callProxyMethod();
            }
        }

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
            "CosProxyKeepAlive::run(): %s stopping", gPtrToStr(this).c_str() );
    }


    void CosProxyKeepAlive::terminate()
    {
        // ThreadGuard guard(m_condition.getLinkedThreadLockable());

        // The thread has been terminated
        m_terminated = true;
        m_running = false;
        m_condition.signal();
    }


    void CosProxyKeepAlive::callProxyMethod()
    {
        FUNCTION_ENTRY( "callProxyMethod" );

        bool isGCCollected = false;

        try
        {
            TA_THREADGUARD( m_proxyLock );

            if ( !CORBA::is_nil(m_proxy) )
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic,
                    "CosProxyKeepAlive::callProxyMethod(): %s calling keep alive method, m_keepAliveSecs=%d",
                    gPtrToStr(this).c_str(), m_keepAliveSecs );

                // TODO LPP: A mechanism for terminating get_filter() early would be useful during shutdown,
                // or perhaps calling it as if it was a oneway call.
                CosNotifyFilter::Filter_var filter = m_proxy->get_filter(0);

                TA_ASSERT( false, "No filter with id 0" );
            }
        }
        catch (const CosNotifyFilter::FilterNotFound&)
        {
            // We just have to call a method on the proxy to keep it alive.  We are not interested in
            // the result returned by get_filter(), or whether the filter exists.

            m_keepAliveSecs = DEFAULT_KEEPALIVE_SUCCESS_SECS;

            FUNCTION_EXIT;
            return;
        }
        catch (const CORBA::INV_OBJREF& e)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "CosProxyKeepAlive::callProxyMethod(): %s Exception while calling keep alive method: %s",
                gPtrToStr(this).c_str(), CorbaUtil::getInstance().exceptionToString(e).c_str() );
            
            isGCCollected = true;
        }
        catch (const CORBA::OBJ_ADAPTER& e)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "CosProxyKeepAlive::callProxyMethod(): %s Exception while calling keep alive method: %s",
                gPtrToStr(this).c_str(), CorbaUtil::getInstance().exceptionToString(e).c_str() );

            isGCCollected = true;
        }
        catch (const CORBA::Exception& e)
        {
            // We seem to get an OBJ_ADAPTER_NoServant exception if an object we are trying to dereference
            // has been garbage collected.
            // TODO LPP: What should we do if the object is not available?  We might be
            // trying to keep something alive that doesn't exist.

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "CosProxyKeepAlive::callProxyMethod(): %s Exception while calling keep alive method: %s",
                gPtrToStr(this).c_str(), CorbaUtil::getInstance().exceptionToString(e).c_str() );
        }
		catch(...)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                "CORBA", "CosProxyKeepAlive::callProxyMethod(): Exception while call Proxy  Funcation");
        }

        if ( false == isGCCollected )
        {
            if ( DEFAULT_KEEPALIVE_SUCCESS_SECS == m_keepAliveSecs )
            {
                TA_Assert( DEFAULT_KEEPALIVE_SUCCESS_SECS != DEFAULT_KEEPALIVE_FAIL_SECS );
                m_keepAliveSecs = DEFAULT_KEEPALIVE_FAIL_SECS;
                startTiming();
            }
            
            if ( DEAD_CON_PROXY_INTERVAL * 1000 < timeElapsed() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "CosProxyKeepAlive::callProxyMethod(): %s proxy may be gabbage collected. time elapsed=%dms, DeadConProxyInterval=%ds, m_keepAliveSecs=<success:%d, fail:%d>",
                    gPtrToStr(this).c_str(), timeElapsed(), DEAD_CON_PROXY_INTERVAL, DEFAULT_KEEPALIVE_SUCCESS_SECS, DEFAULT_KEEPALIVE_FAIL_SECS );
                
                isGCCollected = true;
            }
        }

        if ( true == isGCCollected )
        {
            if ( false == m_serviceAddr.empty() )
            {
                // set m_proxy to nil to disable proxy call
                m_proxy = CosNotifyFilter::FilterAdmin::_nil();
            
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "CosProxyKeepAlive::callProxyMethod(): %s invalidate connection %s", gPtrToStr(this).c_str(), m_serviceAddr.c_str() );
            
                TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().invalidateConnection( m_serviceAddr );
            }
        }

        FUNCTION_EXIT;
    }


    void CosProxyKeepAlive::startTiming()
    {
        FUNCTION_ENTRY( "startTiming" );

        ftime( & m_startTime );

        FUNCTION_EXIT;
    }


    unsigned long CosProxyKeepAlive::timeElapsed()
    {
        FUNCTION_ENTRY( "timeElapsed" );

        timeb current;

        ftime( & current );

        unsigned long diff = ( current.time * 1000 + current.millitm ) -
                             ( m_startTime.time * 1000 + m_startTime.millitm );

        FUNCTION_EXIT;
        return diff;
    }


    void CosProxyKeepAlive::setServiceAddr( const std::string& serviceAddr )
    {
        FUNCTION_ENTRY( "setServiceAddr" );

        m_serviceAddr = serviceAddr;

        FUNCTION_EXIT;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool createAndAttachFilter( CosNotifyFilter::FilterFactory_ptr factory,
                                CosNotifyFilter::FilterAdmin_ptr admin,
                                CosNotifyFilter::Filter_out filter,
                                CosNotifyFilter::FilterID& filterID )
    {
        TA_Assert(!CORBA::is_nil(factory));
        TA_Assert(!CORBA::is_nil(admin));

        filter = CosNotifyFilter::Filter::_nil();
        filterID = 0;

        bool success = false;
        try
        {
            LOG0( SourceInfo, DebugUtil::DebugMsg,
                "createAndAttachFilter(): Creating CosNotifyFilter::Filter" );

            // Create a new filter and add it to the filter admin for the proxy consumer
            CosNotifyFilter::Filter_var tmpFilter = factory->create_filter("EXTENDED_TCL");
            filterID = admin->add_filter(tmpFilter);

            // Copy the filter object reference to our outarg.
            filter = CosNotifyFilter::Filter::_duplicate(tmpFilter);

            LOG1(SourceInfo, DebugUtil::DebugMsg,
                "createAndAttachFilter(): Added CosNotifyFilter::Filter with FilterID = %d to FilterAdmin", filterID);

            success = true;
        }
        catch( const CosNotifyFilter::InvalidGrammar& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,
                "CORBA", "createAndAttachFilter(): CosNotifyFilter::InvalidGrammar" );
            filterID = 0;
        }
        catch ( const CORBA::Exception& e )
        {
            std::string msg("createAndAttachFilter(): ");
            msg += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);

            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str());
            filterID = 0;
        }

        return success;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Helper functions that convert CORBA exceptions into normal return values.

    template <class AdminType_ptr>      // eg. CosNotifyChannelAdmin::ConsumerAdmin_ptr
    CosNotifyFilter::FilterFactory_ptr getDefaultFilterFactory(AdminType_ptr admin)
    {
        TA_Assert(!CORBA::is_nil(admin));

        CosNotifyFilter::FilterFactory_var filterFactory;

        try
        {
            // NOTE that we navigate from the Admin object to it's parent event channel,
            // then to the filter factory.  This relation doesn't have to be symmetric (ie. we
            // don't need to be able to navigate from the filter factory back to the Admin.

            // This might fail if the admin or channel objects are not reachable
            CosNotifyChannelAdmin::EventChannel_var myChannel = admin->MyChannel();
            filterFactory = myChannel->default_filter_factory();
        }
        catch (CORBA::Exception& e)
        {
            std::string msg("getDefaultFilterFactory<AdminType_ptr>(): ");
            msg += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str());
        }

        return filterFactory._retn();
    }

    template <class AdminType_ptr>      // eg. CosNotifyChannelAdmin::ConsumerAdmin_ptr
    std::string getAdminIDAsStr(AdminType_ptr admin)
    {
        TA_Assert(!CORBA::is_nil(admin));

        std::string str;
        try
        {
            str = gStringFromUnsignedLong(admin->MyID());
        }
        catch (CORBA::Exception& e)
        {
            std::string msg("getAdminIDAsStr<AdminType_ptr>(): ");
            msg += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str());

            str = "UNKNOWN ID";
        }

        return str;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CosNotifyChannelConsumer::CosNotifyChannelConsumer()
        : m_eventHandler(NULL),
          m_keepAlive(DEFAULT_KEEPALIVE_SUCCESS_SECS)
    {
        LOG1(SourceInfo, DebugUtil::DebugMsg,
            "CosNotifyChannelConsumer::CosNotifyChannelConsumer(): %s activating CORBA servant",
            gPtrToStr(this).c_str() );

        setNilConsumerAdmin();

        activateServant();

        m_myObjectRef = _this();

        m_keepAlive.start();
    }


    CosNotifyChannelConsumer::~CosNotifyChannelConsumer()
    {
        // The ConsumerAdmin must already have been set to _nil by calling
        // setEventChannel(CosNotifyChannelAdmin::ConsumerAdmin::_nil(), cleanDisconnect)
        TA_Assert(CORBA::is_nil(m_consumerAdmin));

         LOG1(SourceInfo, DebugUtil::DebugMsg,
            "CosNotifyChannelConsumer::~CosNotifyChannelConsumer(): %s deactivating CORBA servant",
            gPtrToStr(this).c_str() );

        deactivateServant();

        m_keepAlive.terminateAndWait();
    }


    void CosNotifyChannelConsumer::setEventHandler(IEventHandler* handler)
    {
        TA_THREADGUARD( m_eventHandlerLock );
        m_eventHandler = handler;
    }


    IEventHandler* CosNotifyChannelConsumer::getEventHandler() const
    {
        TA_THREADGUARD( m_eventHandlerLock );

        return m_eventHandler;
    }

    bool CosNotifyChannelConsumer::setEventChannel( const CosNotifyChannelAdmin::ConsumerAdmin_ptr consumerAdmin,
                                                    bool cleanDisconnect)
    {
        // getAdminIDAsStr is a corba call, take note of the corba timeout
        // normally if consumerAdmin passed in is NULL (onChannelUnavailable) which indicate that m_consumerAdmin will probably be invalid.
         /*LOG4(SourceInfo, DebugUtil::DebugDebug,
            "CosNotifyChannelConsumer::setEventChannel(): %s: "
            "m_consumerAdmin AdminID = %s --> consumerAdmin AdminID = %s, cleanDisconnect = %d",
            gPtrToStr(this).c_str(),
            CORBA::is_nil(m_consumerAdmin) ? "NULL ADMIN" : getAdminIDAsStr(m_consumerAdmin).c_str(),
            CORBA::is_nil(consumerAdmin) ? "NULL ADMIN" : getAdminIDAsStr(consumerAdmin).c_str(),
            cleanDisconnect );*/
        LOG4(SourceInfo, DebugUtil::DebugDebug,
            "CosNotifyChannelConsumer::setEventChannel(): %s: "
            "m_consumerAdmin AdminID = %s --> consumerAdmin AdminID = %s, cleanDisconnect = %d",
            gPtrToStr(this).c_str(),
            CORBA::is_nil(m_consumerAdmin) ? "NULL ADMIN" : "NOT NULL ADMIN",
            CORBA::is_nil(consumerAdmin) ? "NULL ADMIN" : getAdminIDAsStr(consumerAdmin).c_str(),
            cleanDisconnect );

        {
            TA_THREADGUARD( m_connectionLock );

            if ( !CORBA::is_nil(m_consumerAdmin) )
            {
                if ( cleanDisconnect )
                {
                    doCleanDisconnect();
                }

                setNilConsumerAdmin();
            }

            if ( !CORBA::is_nil(consumerAdmin) )
            {
                m_consumerAdmin = CosNotifyChannelAdmin::ConsumerAdmin::_duplicate( consumerAdmin );

                CosNotifyFilter::FilterFactory_var filterFactory = getDefaultFilterFactory(m_consumerAdmin);

                if ( !CORBA::is_nil(filterFactory) &&
                     createAndSetProxySupplier() &&
                     createAndAttachFilter(filterFactory, m_proxySupplier, m_filter.out(), m_filterID) )
                {
                    // We are now connected to the event channel.
                }
                else
                {
                    // Creation of the proxy supplier or filter failed, so we need to reset all connection state.
                    LOG0( SourceInfo, DebugUtil::DebugMsg, "CosNotifyChannelConsumer::setEventChannel(): failed" );
                    setNilConsumerAdmin();
                    return false;
                }
            }
        }

        LOG0( SourceInfo, DebugUtil::DebugMsg, "CosNotifyChannelConsumer::setEventChannel(): succeeded" );
        return true;
    }


    bool CosNotifyChannelConsumer::hasEventChannel() const
    {
        TA_THREADGUARD( m_connectionLock );
        return !CORBA::is_nil(m_consumerAdmin);
    }


    CosNotifyFilter::Filter_ptr CosNotifyChannelConsumer::getFilter() const
    {
        TA_THREADGUARD( m_connectionLock );
        return m_filter;
    }


    void CosNotifyChannelConsumer::push_structured_events(const CosNotification::EventBatch& eventBatch)
    {
        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "FunctionEnter: CosNotifyChannelConsumer::push_structured_events: %s received %d events from EventChannel to dispatch to handler %s:",
            gPtrToStr(this).c_str(), eventBatch.length(), gPtrToStr(m_eventHandler).c_str() );

        m_keepAlive.resetTimer();

        if ( TA_Base_Core::DebugUtil::DebugMsgPub <= TA_Base_Core::DebugUtil::getInstance().getLevel() )
        {
            for ( size_t i = 0, size = eventBatch.length(); i < size; ++i )
            {
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                    "CRECEIVE EVT %d: %s", i, gStringFromEventHeader(eventBatch[i]).c_str() );
            }
        }

        TA_THREADGUARD( m_eventHandlerLock );

        if ( m_eventHandler )
        {
            m_eventHandler->onEventsReceived(eventBatch);
        }

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "FunctionExit: CosNotifyChannelConsumer::push_structured_events: %s received %d events from EventChannel to dispatch to handler %s:",
            gPtrToStr(this).c_str(), eventBatch.length(), gPtrToStr(m_eventHandler).c_str() );
    }


    void CosNotifyChannelConsumer::setNilConsumerAdmin()
    {
        m_consumerAdmin = CosNotifyChannelAdmin::ConsumerAdmin::_nil();
        m_proxySupplier = CosNotifyChannelAdmin::SequenceProxyPushSupplier::_nil();

        // The keepAlive member also needs to be reset (because it is
        // associated with the proxy supplier).
        m_keepAlive.setProxy(CosNotifyFilter::FilterAdmin::_nil());

        m_filter = CosNotifyFilter::Filter::_nil();
        m_filterID = 0;
    }


    void CosNotifyChannelConsumer::doCleanDisconnect()
    {
        TA_Assert(!CORBA::is_nil(m_consumerAdmin));
        TA_Assert(!CORBA::is_nil(m_proxySupplier));
        TA_Assert(!CORBA::is_nil(m_filter));

        try
        {
            // EXCEPTION SAFETY: OmniNotify garbage collection will take care of any
            // steps that are not completed due to an exception being thrown here.

            m_proxySupplier->remove_filter(m_filterID);
            m_filter->destroy();

            m_proxySupplier->disconnect_sequence_push_supplier();
        }
        catch ( const CosNotifyFilter::FilterNotFound& )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
                "CosNotifyChannelConsumer::doCleanDisconnect(): CosNotifyFilter::FilterNotFound" );

            // This should be impossible - it indicates that we haven't set up the proxy supplier correctly.
            TA_Assert(0);
        }
        catch ( const CORBA::Exception& e )
        {
            std::string msg("CosNotifyChannelConsumer::doCleanDisconnect(): ");
            msg += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str());
        }
    }


    bool CosNotifyChannelConsumer::createAndSetProxySupplier()
    {
        TA_Assert(!CORBA::is_nil(m_consumerAdmin));
        TA_Assert(CORBA::is_nil(m_proxySupplier));

        bool success = false;
        try
        {
            LOG1( SourceInfo, DebugUtil::DebugMsg,
                "CosNotifyChannelConsumer::createAndSetProxySupplier(): %s attempting to obtain "
                "SequenceProxyPushSupplier from ConsumerAdmin",
                gPtrToStr(this).c_str() );

            // Create the proxy supplier using the consumer admin
            CosNotifyChannelAdmin::ProxyID proxyID;
            CosNotifyChannelAdmin::ProxySupplier_var supplier =
                m_consumerAdmin->obtain_notification_push_supplier(CosNotifyChannelAdmin::SEQUENCE_EVENT, proxyID);

            //different interface, so using narrow
            CosNotifyChannelAdmin::SequenceProxyPushSupplier_var proxySupplier =
                CosNotifyChannelAdmin::SequenceProxyPushSupplier::_narrow(supplier);

            LOG2( SourceInfo, DebugUtil::DebugMsg,
                "CosNotifyChannelConsumer::createAndSetProxySupplier(): %s created SequenceProxyPushSupplier with ProxyID = %d",
                gPtrToStr(this).c_str(), proxyID );

			proxySupplier->connect_sequence_push_consumer( m_myObjectRef.in() );
		    LOG0(SourceInfo, DebugUtil::DebugMsg, 
                "CosNotifyChannelConsumer::createAndSetProxySupplier(): Connected to SequenceProxyPushSupplier");

            m_proxySupplier = proxySupplier;
            m_keepAlive.setProxy(supplier);

            success = true;
        }
        catch( const CosNotifyChannelAdmin::AdminLimitExceeded& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
                "CosNotifyChannelConsumer::createAndSetProxySupplier(): CosNotifyChannelAdmin::AdminLimitExceeded" );
        }
        catch ( const CORBA::Exception& e )
        {
            std::string msg("CosNotifyChannelConsumer::createAndSetProxySupplier(): ");
            msg += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str());
        }

        // We don't reset the connection members (ie. m_proxySupplier) if a failure occurs.  The caller should do that.
        return success;
    }


    void CosNotifyChannelConsumer::setServiceAddr( const std::string& serviceAddr )
    {
        FUNCTION_ENTRY( "setServiceAddr" );

        m_keepAlive.setServiceAddr( serviceAddr.c_str() );

        FUNCTION_EXIT;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CosNotifyChannelSupplier::CosNotifyChannelSupplier()
        : m_subChangeHandler(NULL),
          m_keepAlive(DEFAULT_KEEPALIVE_SUCCESS_SECS)
    {
        LOG1(SourceInfo, DebugUtil::DebugMsg,
            "CosNotifyChannelSupplier::CosNotifyChannelSupplier(): %s activating CORBA servant",
            gPtrToStr(this).c_str() );

        setNilSupplierAdmin();

        activateServant();
        
        m_myObjectRef = _this();

        m_keepAlive.start();
    }


    CosNotifyChannelSupplier::~CosNotifyChannelSupplier()
    {
        // The SupplierAdmin must already have been set to _nil by calling 
        // setEventChannel(CosNotifyChannelAdmin::SupplierAdmin::_nil(), cleanDisconnect)
        TA_Assert(CORBA::is_nil(m_supplierAdmin));

         LOG1(SourceInfo, DebugUtil::DebugMsg,
            "CosNotifyChannelSupplier::~CosNotifyChannelSupplier(): %s deactivating CORBA servant", gPtrToStr(this).c_str() );

        deactivateServant();
        m_keepAlive.terminateAndWait();
    }


    void CosNotifyChannelSupplier::setSubChangeHandler(ISubChangeHandler* handler)
    {
        TA_THREADGUARD( m_subChangeHandlerLock );
        m_subChangeHandler = handler;
    }


    ISubChangeHandler* CosNotifyChannelSupplier::getSubChangeHandler() const
    {
        TA_THREADGUARD( m_subChangeHandlerLock );
        return m_subChangeHandler;
    }


    bool CosNotifyChannelSupplier::setEventChannel(const CosNotifyChannelAdmin::SupplierAdmin_ptr supplierAdmin, bool cleanDisconnect)
    {
        // getAdminIDAsStr is a corba call, take note of the corba timeout
        // normally if supplierAdmin passed in is NULL (onChannelUnavailable) which indicate that m_supplierAdmin will probably be invalid.
         /*LOG4(SourceInfo, DebugUtil::DebugDebug,
            "CosNotifyChannelSupplier::setEventChannel(): %s: "
            "m_supplierAdmin AdminID = %s --> supplierAdmin AdminID = %s, cleanDisconnect = %d",
            gPtrToStr(this).c_str(),
            CORBA::is_nil(m_supplierAdmin) ? "NULL ADMIN" : getAdminIDAsStr(m_supplierAdmin).c_str(),
            CORBA::is_nil(supplierAdmin) ? "NULL ADMIN" : getAdminIDAsStr(supplierAdmin).c_str(),
            cleanDisconnect );*/
        LOG4(SourceInfo, DebugUtil::DebugDebug,
            "CosNotifyChannelSupplier::setEventChannel(): %s: "
            "m_supplierAdmin AdminID = %s --> supplierAdmin AdminID = %s, cleanDisconnect = %d",
            gPtrToStr(this).c_str(),
            CORBA::is_nil(m_supplierAdmin) ? "NULL ADMIN" : "NOT NULL ADMIN",
            CORBA::is_nil(supplierAdmin) ? "NULL ADMIN" : getAdminIDAsStr(supplierAdmin).c_str(),
            cleanDisconnect );

        TA_THREADGUARD( m_connectionLock );

        if ( !CORBA::is_nil(m_supplierAdmin) )
        {
            if ( cleanDisconnect )
            {
                doCleanDisconnect();
            }

            setNilSupplierAdmin();

            TA_THREADGUARD( m_subChangeHandlerLock );

            if ( m_subChangeHandler != NULL )
            {
                m_subChangeHandler->onSubscriptionReset();
            }
        }

        if ( !CORBA::is_nil(supplierAdmin) )
        {
            m_supplierAdmin = CosNotifyChannelAdmin::SupplierAdmin::_duplicate(supplierAdmin);

            CosNotifyFilter::FilterFactory_var filterFactory = getDefaultFilterFactory(m_supplierAdmin);

            bool success = false;
            if ( !CORBA::is_nil(filterFactory) &&
                 createAndSetProxyConsumer() &&
                 createAndAttachFilter(filterFactory, m_proxyConsumer, m_filter.out(), m_filterID) )
            {
                // The lock is acquired before calling enableSubChangeUpdates() so that a subscription_change()
                // that occurs just after obtain_subscription_types() is called will be sent AFTER our
                // notification.

                if ( true == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_ENABLESUBSCRIPTIONCHANGE ) )
                {
                    TA_THREADGUARD( m_subChangeHandlerLock );

                    CosNotification::EventTypeSeq initialTypes;
                    if ( enableSubChangeUpdates(initialTypes) )
                    {
                        try
                        {
                            if ( 0 < initialTypes.length() )
                            {
                                // An initial notification is sent to the subscription change handler with all event types
                                // currently subscribed to by existing consumers of the event channel (the Corba notification
                                // service will only send subsequent delta notifications).
                                notifySubscriptionChange(initialTypes, CosNotification::EventTypeSeq());
                            }

                            success = true;
                        }
                        catch (...)
                        {
                            // DO NOTHING
                        }
                    }
                }
                else
                {
                    success = true;
                }
            }

            if ( !success )
            {
                // Creation of the proxy consumer failed, so we need to reset all connection state.
                LOG0( SourceInfo, DebugUtil::DebugMsg, "CosNotifyChannelSupplier::setEventChannel(): failed" );
                setNilSupplierAdmin();
                return false;
            }
        }

        LOG0( SourceInfo, DebugUtil::DebugMsg, "CosNotifyChannelSupplier::setEventChannel(): succeeded" );
        return true;
    }


    bool CosNotifyChannelSupplier::hasEventChannel() const
    {
        TA_THREADGUARD( m_connectionLock );
        return !CORBA::is_nil(m_supplierAdmin);
    }


    CosNotifyFilter::Filter_ptr CosNotifyChannelSupplier::getFilter() const
    {
        TA_THREADGUARD( m_connectionLock );
        return m_filter;
    }

	bool CosNotifyChannelSupplier::publishEvents(const std::vector<const CosNotification::StructuredEvent*>& events) const
	{
        TA_Assert(events.size() > 0);

        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "CosNotifyChannelSupplier::publishEvents(): %s attempting to push %d events onto EventChannel:",
            gPtrToStr(this).c_str(), events.size() );

        // TODO LPP: If more than one event is dispatched to a proxy consumer at a time,
        // proxy suppliers will dispatch events in the wrong order (on Windows at least).
        // So we are currently restricted to using a batch size of 1.  This seems to be
        // a bug in OmniNotify 2.1.
        //TA_Assert(events.size() == 1);      // WORKAROUND for bug in OmniNotify 2.1//--RenQiang,Enable BatchSize>1

        // Copy the events into a data structure that can be passed to the Corba notification service.
        CosNotification::EventBatch eventBatch(events.size());
        eventBatch.length(events.size());
        for ( unsigned int i = 0; i < events.size(); ++i )
        {
            TA_Assert(events[i] != NULL);

            // Events should not be published with * as the domain_name or type_name, since consumers
            // are not able to subscribe specifically for these events (they will use a wildcard match
            // instead).
            // TODO LPP: Should we also prohibit "%ALL%" when used as the type_name?
            TA_Assert( strcmp( WILDCARD_STR, events[i]->header.fixed_header.event_type.domain_name.in()) != 0 );
            TA_Assert( strcmp( WILDCARD_STR, events[i]->header.fixed_header.event_type.type_name.in()) != 0 );

            // This makes a copy of the event.
			eventBatch[i] = *events[i];
			
		    LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,  
			    "SPUSH   EVT %d: %s", i, gStringFromEventHeader(*events[i]).c_str() );
        }
        
        try
        {
     		ThreadGuard guard( m_connectionLock );

            TA_Assert(!CORBA::is_nil(m_proxyConsumer));
		    m_proxyConsumer->push_structured_events(eventBatch);

            m_keepAlive.resetTimer();

		    return true;
        }
		catch( const CORBA::Exception& cex )
		{
			std::string msg( "CosNotifyChannelSupplier::publishEvents(): " );
			msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
		}
		
		return false;
	}


    bool CosNotifyChannelSupplier::publishEvents(const CosNotification::EventBatch& eventBatch) const
    {
        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "CosNotifyChannelSupplier::publishEvents(): %s attempting to push %d events onto EventChannel:",
            gPtrToStr(this).c_str(), eventBatch.length() );

        if ( TA_Base_Core::DebugUtil::DebugMsgPub <= TA_Base_Core::DebugUtil::getInstance().getLevel() )
        {
            for ( size_t i = 0, size = eventBatch.length(); i < size; ++i )
            {
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,  
                    "SPUSH   EVT %d: %s", i, gStringFromEventHeader(eventBatch[i]).c_str() );
            }
        }

        try
        {
            ThreadGuard guard( m_connectionLock );

            TA_Assert(!CORBA::is_nil(m_proxyConsumer));
            m_proxyConsumer->push_structured_events(eventBatch);

            m_keepAlive.resetTimer();

            return true;
        }
        catch( const CORBA::Exception& cex )
        {
            std::string msg( "CosNotifyChannelSupplier::publishEvents(): " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
        }

        return false;
    }
	
	
    void CosNotifyChannelSupplier::subscription_change(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed)
    {
        m_keepAlive.resetTimer();

        // The event channel calls this method to inform the supplier of changes to the set
        // of subscribers on the channel.  NOTE that the event channel only notifies us of
        // CHANGES to the aggregate list of event types that have been subscribed to (see
        // 2.6.3 Subscription Change in Corba Notification Service specification).
        TA_THREADGUARD( m_subChangeHandlerLock );
        notifySubscriptionChange(added, removed);
    }


    void CosNotifyChannelSupplier::disconnect_sequence_push_supplier()
    {
        LOG1( SourceInfo, DebugUtil::DebugMsg,
                "CosNotifyChannelSupplier::disconnect_sequence_push_supplier(): %s", gPtrToStr(this).c_str() );

        TA_THREADGUARD( m_connectionLock );
        doCleanDisconnect();
	}


    void CosNotifyChannelSupplier::setNilSupplierAdmin()
    {
        m_supplierAdmin = CosNotifyChannelAdmin::SupplierAdmin::_nil();
        m_proxyConsumer = CosNotifyChannelAdmin::SequenceProxyPushConsumer::_nil();

        // The keepAlive member also needs to be reset (because it is associated with the proxy consumer).
        m_keepAlive.setProxy(CosNotifyFilter::FilterAdmin::_nil());

        m_filter = CosNotifyFilter::Filter::_nil();
        m_filterID = 0;
    }


    void CosNotifyChannelSupplier::doCleanDisconnect()
    {
        TA_Assert(!CORBA::is_nil(m_supplierAdmin));
        TA_Assert(!CORBA::is_nil(m_proxyConsumer));
        TA_Assert(!CORBA::is_nil(m_filter));

        try
        {
            // EXCEPTION SAFETY: OmniNotify garbage collection will take care of any
            // steps that are not completed due to an exception being thrown here.

            m_proxyConsumer->remove_filter(m_filterID);
            m_filter->destroy();
            m_proxyConsumer->disconnect_sequence_push_consumer();
        }
        catch ( const CosNotifyFilter::FilterNotFound& )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch,
                "CORBA", "[WARNING] CosNotifyChannelSupplier::doCleanDisconnect(): CosNotifyFilter::FilterNotFound" );

            // This should be impossible - it indicates that we haven't set up the proxy supplier correctly.
            // TA_Assert(0);
        }
        catch( const CORBA::Exception& cex )
        {
            std::string msg( "CosNotifyChannelSupplier::doCleanDisconnect(): " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
        }
    }


    bool CosNotifyChannelSupplier::createAndSetProxyConsumer()
    {
        TA_Assert(!CORBA::is_nil(m_supplierAdmin));
        TA_Assert(CORBA::is_nil(m_proxyConsumer));

        // TODO LPP: What does the AdminLimitExceeded exception relate to?  What other exceptions can be thrown?
        // TODO LPP: Does SupplierAdmin create a new push consumer each time it is called?

        bool success = false;
        try
        {
            LOG1( SourceInfo, DebugUtil::DebugMsg,
                "CosNotifyChannelSupplier::createAndSetProxyConsumer(): %s attempting to obtain "
                "SequenceProxyPushConsumer from SupplierAdmin", gPtrToStr(this).c_str() );

            // Create the proxy consumer using the supplier admin
            CosNotifyChannelAdmin::ProxyID proxyID;
            CosNotifyChannelAdmin::ProxyConsumer_var consumer =
                m_supplierAdmin->obtain_notification_push_consumer(CosNotifyChannelAdmin::SEQUENCE_EVENT, proxyID);

            CosNotifyChannelAdmin::SequenceProxyPushConsumer_var proxyConsumer =
                CosNotifyChannelAdmin::SequenceProxyPushConsumer::_narrow(consumer);

            LOG2( SourceInfo, DebugUtil::DebugMsg,
                "CosNotifyChannelSupplier::createAndSetProxyConsumer(): %s created SequenceProxyPushConsumer with ProxyID %d",
                gPtrToStr(this).c_str(), proxyID );

            // It seems that subscription_change() is sometimes called by OmniNotify just after
            // obtain_subscription_types(ALL_NOW_UPDATES_ON) is called but before it returns.
            // This causes us to send notifications for the same set of event types twice.
            // Perhaps subscription_change() updates are enabled by default, so we explicitly
            // disable them here before we connect to the proxy and then reenable them when we
            // are ready to start receiving subscription_change() notifications.
            proxyConsumer->obtain_subscription_types( CosNotifyChannelAdmin::NONE_NOW_UPDATES_OFF );

            proxyConsumer->connect_sequence_push_supplier( m_myObjectRef.in() );

            LOG0(SourceInfo, DebugUtil::DebugMsg, 
                "CosNotifyChannelSupplier::createAndSetProxyConsumer(): Connected to SequencyProxyPushConsumer");

            m_proxyConsumer = CosNotifyChannelAdmin::SequenceProxyPushConsumer::_duplicate(proxyConsumer);
            m_keepAlive.setProxy(consumer);

            success = true;
        }
        catch( const CosNotifyChannelAdmin::AdminLimitExceeded& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,
                "CORBA", "CosNotifyChannelSupplier::createAndSetProxyConsumer(): CosNotifyChannelAdmin::AdminLimitExceeded" );
        }
        catch( const CosEventChannelAdmin::AlreadyConnected& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,
                "CORBA", "CosNotifyChannelSupplier::createAndSetProxyConsumer(): CosNotifyChannelAdmin::AlreadyConnected" );
        }
        catch( const CORBA::Exception& e )
        {
            std::string msg("CosNotifyChannelSupplier::createAndSetProxyConsumer(): ");
            msg += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str());
        }

        // We don't reset the connection members (ie. m_proxySupplier) if a failure occurs.  The caller should do that.
        return success;
    }


    bool CosNotifyChannelSupplier::enableSubChangeUpdates(CosNotification::EventTypeSeq& initialTypes)
    {
        FUNCTION_ENTRY( "enableSubChangeUpdates" );

        TA_Assert(!CORBA::is_nil(m_proxyConsumer));

        try
        {
            // This enables subscription_change() notifications and also provides us with the
            // current set of subscription types.  subscription_change() can be called on us
            // at any time after this call returns.
            CosNotification::EventTypeSeq_var seq =
                m_proxyConsumer->obtain_subscription_types( CosNotifyChannelAdmin::ALL_NOW_UPDATES_ON );

            initialTypes = seq.in();

            std::stringstream strm;
            strm
                << "CosNotifyChannelSupplier::enableSubChangeUpdates(): " << gPtrToStr(this).c_str()
                << " connected to event channel with event types\n initial=" << gStringFromEventTypes(initialTypes);

            LOGLARGESTRING( SourceInfo, DebugUtil::DebugMsg, strm.str().c_str() );

            FUNCTION_EXIT;
            return true;
        }
        catch( const CORBA::Exception& e )
        {
            std::string msg("CosNotifyChannelSupplier::enableSubChangeUpdates(): ");
            msg += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str());
        }

        FUNCTION_EXIT;
        return false;
    }


    void replaceANYWithStarStar(CosNotification::EventTypeSeq& seq)
    {
        // Determine whether any of the equivalent forms of (*, *) are being used,
        // such as (, %ANY), and replace them with (*, *).
        for (unsigned int i = 0; i < seq.length(); ++i)
        {
            if (gIsStarStar(seq[i]))
            {
                seq[i].domain_name = WILDCARD_STR;
                seq[i].type_name = WILDCARD_STR;
            }
        }
    }

    void CosNotifyChannelSupplier::notifySubscriptionChange( const CosNotification::EventTypeSeq& added,
                                                             const CosNotification::EventTypeSeq& removed )
    {
        // THREAD SAFETY: Caller must acquire m_handlerLock

        // TODO LPP: Is it definately the case that the event channel won't send subscription_change()
        // if both sequences are empty?
        TA_Assert(added.length() != 0 || removed.length() != 0);

         LOG4( SourceInfo, DebugUtil::DebugMsg,
            "CosNotifyChannelSupplier::notifySubscriptionChange(): %s notifying %s\n"
            " added=%s\n"
            " removed=%s",
            gPtrToStr(this).c_str(), gPtrToStr(m_subChangeHandler).c_str(),
            gStringFromEventTypes(added).c_str(), gStringFromEventTypes(removed).c_str() );

        // This try-catch block is an aid for debugging.
        // Exceptions must not propagate out of onSubscriptionChange().
        try
        {
            if ( m_subChangeHandler != NULL )
            {
                // We convert (*, *) equivalent forms, such as (, %ANY), to (*, *) because
                // we are not entirely consistent in our handling of the equivalency
                // (eg. in EventTypeSet, RefCountedConstraintSet, etc).
                // TODO LPP: We need to review our handling of (, %ALL).
                CosNotification::EventTypeSeq addedCopy(added);
                replaceANYWithStarStar(addedCopy);

                CosNotification::EventTypeSeq removedCopy(removed);
                replaceANYWithStarStar(removedCopy);

                m_subChangeHandler->onSubscriptionChange(addedCopy, removedCopy);
            }
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "CosNotifyChannelSupplier::notifySubscriptionChange(): " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
            //TA_Assert(0);
            throw;
        }
        catch (std::exception& e)
        {
            std::string msg( "CosNotifyChannelSupplier::notifySubscriptionChange(): " );
            msg.append( e.what() );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
            //TA_Assert(0);
            throw;
        }
        catch (...)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "CosNotifyChannelSupplier::notifySubscriptionChange(): Unknown exception" );
            //TA_Assert(0);
            throw;
        }
    }


    void CosNotifyChannelSupplier::setServiceAddr( const std::string& serviceAddr )
    {
        FUNCTION_ENTRY( "setServiceAddr" );

        m_keepAlive.setServiceAddr( serviceAddr.c_str() );

        FUNCTION_EXIT;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SubChangeHandlerFan::addHandler(ISubChangeHandler* child)
    {
        TA_Assert(child != NULL);

        std::pair<std::set<ISubChangeHandler*>::iterator, bool> result = m_handlers.insert(child);
        TA_Assert(result.second);   // The child must not have been added already
    }


    void SubChangeHandlerFan::removeHandler(ISubChangeHandler* child)
    {
        TA_Assert(child != NULL);

        std::set<ISubChangeHandler*>::iterator iter = m_handlers.find(child);
        TA_Assert(iter != m_handlers.end());    // The child must have been added already

        m_handlers.erase(iter);
    }


    void SubChangeHandlerFan::onSubscriptionChange( const CosNotification::EventTypeSeq& added,
                                                    const CosNotification::EventTypeSeq& removed )
    {
        for ( std::set<ISubChangeHandler*>::iterator iter = m_handlers.begin(); iter != m_handlers.end(); ++iter )
        {
            TA_Assert(*iter != NULL);
            (*iter)->onSubscriptionChange(added, removed);
        }
    }


    void SubChangeHandlerFan::onSubscriptionReset()
    {
        for ( std::set<ISubChangeHandler*>::iterator iter = m_handlers.begin(); iter != m_handlers.end(); ++iter )
        {
            TA_Assert(*iter != NULL);
            (*iter)->onSubscriptionReset();
        }
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RefCountedConstraintSet::setFilter(CosNotifyFilter::Filter_ptr filter)
    {
        FUNCTION_ENTRY( "setFilter" );

        {
            TA_THREADGUARD( m_countsLock );
            m_counts.clear();
        }

        m_filter = CosNotifyFilter::Filter::_duplicate(filter);
        // set timeout to 3 seconds
        if ( !CORBA::is_nil(m_filter) )
        {
            omniORB::setClientCallTimeout( m_filter, 3000 );
        }

        FUNCTION_EXIT;
    }


    void RefCountedConstraintSet::addExpression( unsigned int clientId,
                                                 unsigned int exprId,
                                                 const CosNotifyFilter::ConstraintExp& constraint )
    {
        FUNCTION_ENTRY( "addExpression" );

        TA_Assert(!CORBA::is_nil(m_filter));

        TA_THREADGUARD( m_countsLock );

        // If a constraint with the same ID has already been set, increment the reference count.
        std::pair<unsigned int, unsigned int> id(clientId, exprId);

        ExpressionIDToCountMap::iterator it = m_counts.find(id);
        if ( it != m_counts.end() )
        {
            // TODO LPP: We could probably assert that the constraint expression is the same
            // as the one previously added to the filter for this exprId.
            ++(it->second.m_count);
        }
        else
        {
            // WORKAROUND for OmniNotify 2.1 %ALL constraint bug
            for ( unsigned int i = 0; i < constraint.event_types.length(); ++i )
            {
                // Determine whether any of the equivalent forms of (*, *) are being used,
                // such as (, %ANY), and disallow them.
                if ( gIsStarStar(constraint.event_types[i]) )
                {
                    TA_Assert( 0 == strcmp(constraint.event_types[i].domain_name.in(), WILDCARD_STR) );
                    TA_Assert( 0 == strcmp(constraint.event_types[i].type_name.in(), WILDCARD_STR) );
                }
            }

            std::string eventTypesStr = gStringFromEventTypes(constraint.event_types);
            LOG5(SourceInfo, DebugUtil::DebugMsg,
                "RefCountedConstraintSet::addExpression(): %s adding constraint (%d, %d) to Filter:\n"
                " event types=%s\n"
                " constraint_expr=%s",
                gPtrToStr(this).c_str(), clientId, exprId, eventTypesStr.c_str(), constraint.constraint_expr.in() );

            // Create the constraint expression.  A single ConstraintExp is added to the
            // filter each time a constraint is added (OR semantics are applied when the
            // filter is evaluated).
            CosNotifyFilter::ConstraintExpSeq constraintSeq;
            constraintSeq.length( 1 );
            constraintSeq[0] = constraint;

            CosNotifyFilter::ConstraintInfoSeq_var infoSeq;
            try
            {
                infoSeq = m_filter->add_constraints(constraintSeq);
                TA_Assert( 1 == infoSeq->length() );
            }
            catch (CosNotifyFilter::InvalidConstraint&)
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
                    "RefCountedConstraintSet::addExpression(): CosNotifyFilter::InvalidConstraint" );

                // TODO LPP: What should we do here?
                TA_Assert(0);
            }

            m_counts.insert( ExpressionIDToCountMap::value_type(id, ConstraintCountDetail(infoSeq[0].constraint_id)) );
        }

        FUNCTION_EXIT;
    }


    void RefCountedConstraintSet::removeExpression( unsigned int clientId, unsigned int exprId )
    {
        FUNCTION_ENTRY( "removeExpression" );

        TA_Assert(!CORBA::is_nil(m_filter));

        TA_THREADGUARD( m_countsLock );

        // Check to see if this ID has actually been added
        std::pair<unsigned int, unsigned int> id(clientId, exprId);

        ExpressionIDToCountMap::iterator it = m_counts.find(id);

        // TODO LPP NOW: HACK to workaround multiple subscription_change notifications for
        // the same event type.  We must allow the same event type to be added
        // or removed multiple times.  We need to determine the reason for this behavior.
        if ( it == m_counts.end() )
        {
            LOG3(SourceInfo, DebugUtil::DebugWarn,
                "RefCountedConstraintSet::removeExpression(): %s doesn't contain constraint (%d, %d)",
                gPtrToStr(this).c_str(), clientId, exprId );

            FUNCTION_EXIT;
            return;
        }

        TA_Assert(it != m_counts.end());
        TA_Assert(it->second.m_count > 0);

        if ( 0 == --(it->second.m_count) )
        {
            LOG2(SourceInfo, DebugUtil::DebugMsg,
                "RefCountedConstraintSet::removeExpression(): %s removing constraint %d from Filter",
                gPtrToStr(this).c_str(), exprId );

            CosNotifyFilter::ConstraintIDSeq delSeq;
            delSeq.length(1);
            delSeq[0] = it->second.m_constraintID;

            try
            {
                // remove the constraint from the filter
                m_filter->modify_constraints( delSeq, CosNotifyFilter::ConstraintInfoSeq() );
            }
            catch (CosNotifyFilter::InvalidConstraint&)
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch,
                    "CORBA", "RefCountedConstraintSet::removeExpression(): CosNotifyFilter::InvalidConstraint" );

                // TODO LPP: What should we do here?
                TA_Assert( false );
            }
            catch ( const CosNotifyFilter::ConstraintNotFound& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch,
                    "CORBA", "RefCountedConstraintSet::removeExpression(): CosNotifyFilter::ConstraintNotFound" );

                // This should be impossible (it indicates that the Filter object is somehow
                // out of sync with our map).
                TA_Assert( false );
            }

            m_counts.erase(it);
        }

        FUNCTION_EXIT;
    }


    void RefCountedConstraintSet::removeAllExpressions()
    {
        FUNCTION_ENTRY( "removeAllExpressions" );

        TA_Assert(!CORBA::is_nil(m_filter));

        m_filter->remove_all_constraints();

        {
            TA_THREADGUARD( m_countsLock );
            m_counts.clear();
        }

        FUNCTION_EXIT;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const char* SEQ_NUM_FIELDNAME = "seq_num";
    const char* EVENT_SUPPLIER_FIELDNAME = "evt_sup";

    SeqNumAppender::SeqNumAppender()
        : m_nextSeqNum(1),
          m_appenderId(TA_Uuid::generateUUID())
    {
    }


    void SeqNumAppender::appendToHeader(CosNotification::StructuredEvent& event)
    {
        // TODO LPP: It might be advisable to make SEQ_NUM_FIELDNAME a list, so that additional
        // sequence numbers can be appended to the same field.  Alternatively perhaps an
        // existing field should be updated rather than appending another one to the tail
        // (although this wouldn't preserve the complete sequence history of a message).

        // We append two additional fields to the variable header of each message,
        // an incrementing sequence number and the identity of the supplier.
        int index = event.header.variable_header.length();
        event.header.variable_header.length(index + 2);

        unsigned long seqNum = 0;
        {
            TA_THREADGUARD( m_lock );
            seqNum = m_nextSeqNum;
            ++m_nextSeqNum;
        }

        event.header.variable_header[index].name = SEQ_NUM_FIELDNAME;
        event.header.variable_header[index].value <<= seqNum;

        ++index;
        event.header.variable_header[index].name = EVENT_SUPPLIER_FIELDNAME;
        event.header.variable_header[index].value <<= m_appenderId.c_str();
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SeqNumFilter::SeqNumFilter(int fieldPosition)
        : m_fieldPosition(fieldPosition),
          m_running(false)
    {
        TA_Assert(m_fieldPosition != 0);
    }


    SeqNumFilter& SeqNumFilter::operator =( const SeqNumFilter& rhs )
    {
        m_fieldPosition = rhs.m_fieldPosition;
        m_nextSeqNumMap = rhs.m_nextSeqNumMap;

        return *this;
    }


    SeqNumFilter::SeqNumFilter( const SeqNumFilter& rhs )
        : m_fieldPosition( rhs.m_fieldPosition ),
          m_nextSeqNumMap( rhs.m_nextSeqNumMap )
    {
    }


    SeqNumFilter::EEventOrderStatus SeqNumFilter::isDuplicateEvent(const CosNotification::StructuredEvent& event )
    {
        // Locate the relevant seqNum (SEQ_NUM_FIELDNAME) and appenderId (EVENT_SUPPLIER_FIELDNAME) fields.

        // TODO LPP: Allow occurrences in other positions to be found.
        TA_Assert( 1 == m_fieldPosition );
        unsigned int index = findFirstInSequence(event.header.variable_header, SEQ_NUM_FIELDNAME);
        unsigned int headerLength = event.header.variable_header.length();

        // TODO LPP: We should probably throw an exception if the event doesn't contain the
        // fields at the specified 'fieldPosition', rather than asserting.
        TA_Assert(headerLength >= 2);
        TA_Assert(index < headerLength - 1);
        TA_Assert( 0 == strcmp(event.header.variable_header[index+1].name, EVENT_SUPPLIER_FIELDNAME) );

        unsigned long evtSeqNum;
        TA_VERIFY(event.header.variable_header[index].value >>= evtSeqNum, "SEQ_NUM_FIELDNAME value must be CORBA::ULong");
        const char* evtAppenderId;
        TA_VERIFY(event.header.variable_header[index+1].value >>= evtAppenderId, "EVENT_SUPPLIER_FIELDNAME value must be CORBA::string");

        // Check whether the event is a duplicate.  It is not a duplicate if the
        // appenderId has not been added already OR it has been added and it's
        // seqNum is greater than the next seqNum for this appenderId.
        EEventOrderStatus duplicate = EVENT_IN_ORDER;
        TA_THREADGUARD( m_lock );

        std::pair<SeqNumMapType::iterator, bool> result = m_nextSeqNumMap.insert(SeqNumMapType::value_type(evtAppenderId, evtSeqNum+1));
        if ( false == result.second )
        {
            // The evtAppenderId has been added already - so map::insert didn't insert.
            // Since the map contains the NEXT expected sequence number, an event with a sequence number
            // EQUAL to this is not a duplicate.
            if ( evtSeqNum >= result.first->second )
            {
                if ( result.first->second < evtSeqNum )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                        "SeqNumFilter::isDuplicateEvent(): OUT OF ORDER event has seqNum %d, expecting %d, sender_entity=%s",
                        evtSeqNum, result.first->second, evtAppenderId );

                    duplicate = EVENT_OUT_OF_ORDER;
                }
                else
                {
                    duplicate = EVENT_IN_ORDER;
                }

                result.first->second = evtSeqNum + 1;
            }
            else
            {
                duplicate = EVENT_DUPLICATE;

                // yanrong++: CL-21120
                // only log SubsystemStateChangeNotification message, its type_name == "40044"
                if ( 0 == strcmp(event.header.fixed_header.event_type.type_name.in(), "40044") )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "[CL-21120] SeqNumFilter::isDuplicateEvent, duplicatedEvent:[evtId: %s, evtNum: %lu, expNum: %lu]",
                        evtAppenderId, evtSeqNum, result.first->second);
                }
                // ++yanrong

                // Note that it is not necessarily an error when a duplicate event is received -
                // for example, an event might be sent to a consumer via multiple redundant paths.
                // The caller must determine the appropriate behaviour.
                LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                    "SeqNumFilter::isDuplicateEvent(): DUPLICATE event has seqNum %d, expecting %d", evtSeqNum, result.first->second );
            }
        }
        else
        {
            // first event
        }

        return duplicate;
    }


    void SeqNumFilter::run()
    {
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, "SeqNumFilter::run(): %s running", gPtrToStr(this).c_str() );

        m_running = true;

        while ( true )
        {
            // TODO LPP NOW: Wait for the garbage collection interval to elapse

            if ( !m_running )
            {
                break;
            }

            // TODO LPP NOW: Erase all old map entries.
        }

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, "SeqNumFilter::run(): %s stopping", gPtrToStr(this).c_str() );
    }


    void SeqNumFilter::terminate()
    {
        m_running = false;

        // TODO LPP NOW: Signal the semaphore
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void gPopulateConstraintWithEventType(const CosNotification::EventType& eventType, CosNotifyFilter::ConstraintExp& constraint)
    {
        constraint.constraint_expr = CORBA::string_dup( "true" );
        constraint.event_types.length(1);
        constraint.event_types[0].domain_name = eventType.domain_name.in();
        constraint.event_types[0].type_name = eventType.type_name.in();
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EventTypeSet::EventTypeSet()
        : m_nextID(1)
    {
    }


    bool EventTypeSet::isMatchingEventType(const CosNotification::EventType& eventType) const
    {
        return ( getEventTypeKey(eventType) != NOT_IN_SET );
    }


    unsigned int EventTypeSet::getEventTypeKey(const CosNotification::EventType& eventType) const
    {
        TA_THREADGUARD( m_lock );

        EventTypeKey type(eventType.domain_name.in(), eventType.type_name.in());

        unsigned int id = NOT_IN_SET;
        EventTypeToIDMap::const_iterator iter = m_eventTypeToIDMap.find(type);
        if ( iter != m_eventTypeToIDMap.end() )
        {
            // We have added this EventType previously, so we need to use the same id
            id = iter->second;
        }

        return id;
    }


    unsigned int EventTypeSet::addEventType(const CosNotification::EventType& eventType)
    {
        TA_THREADGUARD( m_lock );

        EventTypeKey type(eventType.domain_name.in(), eventType.type_name.in());

        std::pair<EventTypeToIDMap::iterator, bool> result =
            m_eventTypeToIDMap.insert(EventTypeToIDMap::value_type(type, m_nextID));
        TA_Assert(result.second);

        return m_nextID++;
    }


    unsigned int EventTypeSet::removeEventType(const CosNotification::EventType& eventType)
    {
        TA_THREADGUARD( m_lock );

        EventTypeKey type(eventType.domain_name.in(), eventType.type_name.in());

        unsigned int id = 0;
        EventTypeToIDMap::iterator iter = m_eventTypeToIDMap.find(type);
        TA_Assert(iter != m_eventTypeToIDMap.end());

        id = iter->second;
        m_eventTypeToIDMap.erase(iter);

        return id;
    }


    void EventTypeSet::removeAllEventTypes()
    {
        TA_THREADGUARD( m_lock );

        m_eventTypeToIDMap.clear();
        m_nextID = 1;
    }


    void EventTypeSet::getEventTypeKeys(std::vector<unsigned int>& vec) const
    {
        TA_THREADGUARD( m_lock );

        for ( EventTypeToIDMap::const_iterator iter = m_eventTypeToIDMap.begin(); iter != m_eventTypeToIDMap.end(); ++iter )
        {
            vec.push_back(iter->second);
        }
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WildcardEventTypeSet::WildcardEventTypeSet()
        : m_subscribedToAll( false )
    {
    }


    bool WildcardEventTypeSet::isMatchingEventType(const CosNotification::EventType& eventType) const
    {
        TA_THREADGUARD( m_lock );

        if ( m_subscribedToAll )
        {
            return true;
        }

        std::string domain_name(eventType.domain_name.in());
        std::string type_name(eventType.type_name.in());

        // See if the domain_name exists first & what type of subscription
        SubscribedDomainsMap::const_iterator domainIter = m_subscribedDomains.find( domain_name );

        // TODO LPP: We don't currently handle (*, x) event types.  For example,
        // if ( *, x ) was added, (a, x) and (b, x) should be matching types.

        // If there is no domain found then there are no subscribers
        if ( domainIter == m_subscribedDomains.end() )
        {
            LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                "WildcardEventTypeSet::isMatchingEventType(): %s has no subscribers for domain %s",
                gPtrToStr(this).c_str(), domain_name.c_str() );
            return false;
        }

        // If we want everything in the domain
        if ( ALL == domainIter->second )
        {
            return true;
        }

        // If ( domainIter->second == TYPE ) we want to check the type subscriptions.
        if ( m_subscribedTypes.isMatchingEventType(eventType) )
        {
            return true;
        }
        else
        {
            LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                "WildcardEventTypeSet::isMatchingEventType(): %s has no subscribers for type (%s, %s)",
                gPtrToStr(this).c_str(), domain_name.c_str(), type_name.c_str() );
            return false;
        }
    }


    void WildcardEventTypeSet::addEventType( const CosNotification::EventType& eventType )
    {
        std::string domain( eventType.domain_name.in() );
        std::string type( eventType.type_name.in() );

        LOG3( SourceInfo, DebugUtil::DebugMsg,
             "WildcardEventTypeSet::addEventType(): %s adding event type (%s, %s)",
             gPtrToStr(this).c_str(), domain.c_str(), type.c_str() );

        // TODO LPP: We don't currently handle (*, x) event types.
        TA_Assert(domain.compare(WILDCARD_STR) != 0 || 0 == type.compare(WILDCARD_STR) );

        TA_THREADGUARD( m_lock );

        if ( gIsStarStar(eventType) )
        {
            m_subscribedToAll = true;
        }
        else if ( 0 == type.compare(WILDCARD_STR) )
        {
            // We have subscribed to everything in a domain - ie. (domain_name, "*")
            // Add the entry to the domain table
            m_subscribedDomains[domain] = ALL;
        }
        else
        {
            // We have subscribed to a specific (domain_name, type_name).  We don't change a
            // dominating domain subscription (ALL) to a type only subscription, but
            // we need to add the domain to the map if it is not already added
            // (std::insert will only insert and modify the map if the domain_name hasn't
            // already been added).
            m_subscribedDomains.insert(SubscribedDomainsMap::value_type(domain, TYPE));
            m_subscribedTypes.addEventType(eventType);
        }
	}


    void WildcardEventTypeSet::removeEventType( const CosNotification::EventType& eventType )
    {
        std::string domain( eventType.domain_name.in() );
        std::string type( eventType.type_name.in() );

        LOG3( SourceInfo, DebugUtil::DebugMsg,
             "WildcardEventTypeSet::removeEventType(): %s removing event type (%s, %s)",
             gPtrToStr(this).c_str(), domain.c_str(), type.c_str() );

        TA_THREADGUARD( m_lock );

        if ( gIsStarStar(eventType) )
        {
            m_subscribedToAll = false;
        }
        else if ( 0 == type.compare(WILDCARD_STR) )
        {
            // We have unsubscribed to everything in a domain - ie. (domain, "*").
            // But there might be other (domain_name, type_name) subscriptions which we don't remove.
            m_subscribedDomains[domain] = TYPE;
        }
        else
        {
            m_subscribedTypes.removeEventType(eventType);
        }
    }


    void WildcardEventTypeSet::removeAllEventTypes()
    {
        LOG1( SourceInfo, DebugUtil::DebugMsg,
             "WildcardEventTypeSet::removeAllEventTypes(): %s removing all event types", gPtrToStr(this).c_str() );

        TA_THREADGUARD( m_lock );

        m_subscribedToAll = false;
        m_subscribedDomains.clear();
        m_subscribedTypes.removeAllEventTypes();
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SubscribedEventFilter::isSubscribedEvent(const CosNotification::StructuredEvent& event) const
    {
        static bool enableSubscriptionChange = TA_Base_Core::RunParams::getInstance().isSet( RPARAM_ENABLESUBSCRIPTIONCHANGE );

        if ( true == enableSubscriptionChange  )
        {
            return m_eventTypeSet.isMatchingEventType(event.header.fixed_header.event_type);
        }
        else
        {
            return true;
        }
    }


    void SubscribedEventFilter::onSubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed)
    {
        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "SubscribedEventFilter::onSubscriptionChange(): %s updating WildcardEventTypeSet", TA_Base_Core::gPtrToStr(this).c_str() );

        // TODO LPP: Does it matter that the WildcardEventTypeSet is not modified atomically?

        for ( unsigned int addCount = 0; addCount < added.length(); ++addCount )
        {
            // TODO LPP NOW: HACK to workaround multiple subscription_change notifications for
            // the same event type.  EventTypeSet doesn't allow the same event type to be added
            // or removed multiple times.  We need to determine the reason for this behaviour.
            if ( !m_eventTypeSet.isMatchingEventType(added[addCount]) )
            {
                m_eventTypeSet.addEventType(added[addCount]);
            }
        }

        for ( unsigned int removeCount = 0; removeCount < removed.length(); ++removeCount )
        {
            // LIMIN TODO
            // TODO LPP NOW: HACK to workaround multiple subscription_change notifications for
            // the same event type.  EventTypeSet doesn't allow the same event type to be added
            // or removed multiple times.  We need to determine the reason for this behavior.
            //m_eventTypeSet.removeEventType( removed[removeCount] );
        }
    }


    void SubscribedEventFilter::onSubscriptionReset()
    {
        m_eventTypeSet.removeAllEventTypes();
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const CosNotifyChannelAdmin::SupplierAdmin_ptr DefaultAdminProvider::getSupplierAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const
    {
        TA_Assert(!CORBA::is_nil(channel));

        CosNotifyChannelAdmin::SupplierAdmin_var admin;

        try
        {
            admin = channel->default_supplier_admin();
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "DefaultAdminProvider::getSupplierAdmin(): " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
        }

        return admin._retn();
    }


    const CosNotifyChannelAdmin::ConsumerAdmin_ptr DefaultAdminProvider::getConsumerAdmin(const CosNotifyChannelAdmin::EventChannel_ptr channel) const
    {
        TA_Assert(!CORBA::is_nil(channel));

        CosNotifyChannelAdmin::ConsumerAdmin_var admin;

        try
        {
            admin = channel->default_consumer_admin();
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "DefaultAdminProvider::getConsumerAdmin(): " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
        }

        return admin._retn();
    }


}; // TA_Base_Core


