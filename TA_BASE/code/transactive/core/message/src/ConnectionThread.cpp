/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/ConnectionThread.cpp $
  * @author:  Lachlan Partington
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/ConnectionThread.h"
#include "core/message/src/EventChannelUser.h"
#include "core/message/src/MessageConstants.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"


namespace TA_Base_Core
{
    const int RESOLUTION_FAILURE_DELAY_MS = 2000;
    const int RESOLUTION_SUCCESS_DELAY_MS = 10000;

    // Returns a corbaloc-style object reference to the IChannelLocator instance, of the form
    // "corbaloc::<host>:<port>/ChannelLocator", given the serviceAddr "host:port"
    std::string getChannelLocatorAddr(const std::string& serviceAddr)
    {
        char channelLocatorAddress[256] = {0};

        // Check to see if the port is specified
        if ( serviceAddr.find(":") == serviceAddr.npos )
        {
            // Port is not specified, use default
            sprintf( channelLocatorAddress, "corbaloc::%s:%d/%s", serviceAddr.c_str(),
                     TA_Base_Core::IChannelLocator::DEFAULT_LOCATOR_PORT,
                     TA_Base_Core::IChannelLocator::DEFAULT_LOCATOR_NAME );
        }
        else
        {
            // Port is specified, use provided
            sprintf( channelLocatorAddress, "corbaloc::%s/%s", serviceAddr.c_str(),
                     TA_Base_Core::IChannelLocator::DEFAULT_LOCATOR_NAME );
        }

        return std::string(channelLocatorAddress);
    }


    ChannelLocatorConnection::ChannelLocatorConnection( const std::string& serviceAddr )
        : m_serviceAddr( serviceAddr.c_str() ),
          m_running( false ),
          m_terminated( false ),
          m_pendingInvalidate(false),
          m_attachPending(false),
          m_resolvedLastTime(true)
	{

        TA_ASSERT( serviceAddr.length() > 0, "Blank hostname specified for ChannelLocatorConnection" );

        m_channelLocator = TA_Base_Core::IChannelLocator::_nil();

        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "ChannelLocatorConnection::ChannelLocatorConnection(): ChannelLocatorConnection created to monitor serviceAddr %s",
            m_serviceAddr.c_str() );
	}


    ChannelLocatorConnection::~ChannelLocatorConnection()
    {
        // All ChannelObservers must have deregistered already.
        for ( ChannelNameToObserverDetailMap::iterator iter = m_channelToObserverDetailMap.begin();
              iter != m_channelToObserverDetailMap.end(); ++iter )
        {
            TA_Assert(iter->second != NULL);

            LOG2( SourceInfo, DebugUtil::DebugDebug,
                "ChannelLocatorConnection::~ChannelLocatorConnection(): %s has observer %s still attached",
                gPtrToStr(this).c_str(), gPtrToStr(iter->second->m_observer).c_str() );
        }

        TA_Assert( 0 == m_channelToObserverDetailMap.size() );

        TA_Assert(!m_running);
    }


    void ChannelLocatorConnection::invalidate()
    {
        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
             "ChannelLocatorConnection::invalidate(): %s invalidating connection", gPtrToStr(this).c_str() );

        {
            TA_THREADGUARD( m_resolveLock );
            m_pendingInvalidate = true;
        }

        // Force the run() thread to wake up if it is currently waiting.
        ThreadGuard guard(m_condition.getLinkedThreadLockable());
        m_condition.signal();
    }


    bool ChannelLocatorConnection::isAvailable()
    {
        TA_THREADGUARD( m_resolveLock );
        return !CORBA::is_nil(m_channelLocator);
    }


    void ChannelLocatorConnection::attach( IChannelObserver* obs )
    {
        TA_Assert(obs != NULL);

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnection::attach(): %s queueing observer %s for attach to notifications regarding channel %s",
            gPtrToStr(this).c_str(), gPtrToStr(obs).c_str(), obs->getChannelName().c_str() );

        {
            TA_THREADGUARD( m_observersLock );

            // Queue the observer to be added to the notification set.  Notifications will start
            // the next time channel availability is checked (this avoids certain deadlock scenarios).
            m_obsChangeQueue.push_back(ObserverChangeDetail(obs, true));
            m_attachPending = true;
        }

        // Force the run() thread to wake up if it is currently waiting, so that the observer will
        // be notified relatively soon.
        ThreadGuard guard(m_condition.getLinkedThreadLockable());
        m_condition.signal();
    }


    void ChannelLocatorConnection::detach( IChannelObserver* obs )
    {
        TA_Assert(obs != NULL);

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnection::detach(): %s queueing observer %s for detach from notifications regarding channel %s",
            gPtrToStr(this).c_str(), gPtrToStr(obs).c_str(), obs->getChannelName().c_str() );

        TA_THREADGUARD( m_observersLock );

        // Queue the observer to be removed from the notification set.
        m_obsChangeQueue.push_back(ObserverChangeDetail(obs, false));

        // If we are currently dispatching notifications, we need to prevent a notification
        // being sent to this observer (particularly relevant if the observer will be deleted as
        // soon as detach() returns).  NOTE that we shouldn't dereference the observer after it
        // has been detached (ie. in mergeObserverChanges), and we mustn't remove elements from
        // m_channelToObserverDetailMap here.
        // ALSO, the observer might not have been added to m_channelToObserverDetailMap yet (it
        // may still be in m_obsChangeQueue waiting to be attached).
        ChannelNameToObserverDetailMap::iterator iter = findObserver(obs, obs->getChannelName());
        if ( iter != m_channelToObserverDetailMap.end() )
        {
            TA_Assert(iter->second != NULL);
            TA_Base_Core::ThreadGuard guard( iter->second->m_detachLock );
            iter->second->m_haveDetached = true;
        }
    }


    // STATE TRANSITIONS:
    //
    //          isChannelLocatorReachable() succeeds                                   isChannelLocatorReachable() succeeds
    //              onChannelAvailable() FAILS                                              NO NOTIFICATIONS DISPATCHED
    //                     ---------                                                                ---------
    //                    |         |                                                              |         |
    //                    |         v                                                              |         v
    //             IChannelLocator AVAILABLE   ------------------------------------------>  IChannelLocator AVAILABLE
    //             (m_channelLocator != NULL)     isChannelLocatorReachable() succeeds      (m_channelLocator != NULL)
    //                                                onChannelAvailable() succeeds
    //                          ^      \                                                  /      ^
    //                           \      \ isChannelLocatorReachable() FAILS *            /      /
    //                            \      \onChannelUnavailable() dispatched             /      /
    //                             \      \                                            /      /
    //                              \      \      isChannelLocatorReachable() FAILS * /      / resolveChannelLocator() succeeds
    //                               \      \       onChannelUnavailable() dispatched/      /  onChannelAvailable() succeeds
    //                                \      \                                      /      /
    //                                 \      \                                    /      /
    // resolveChannelLocator() succeeds \      v                                  v      /
    //    onChannelAvailable() FAILS                IChannelLocator UNAVAILABLE
    //                                              (m_channelLocator == NULL)   <------------------ START
    //                                                     |         ^
    //                                                     |         |
    //                                                      ---------                           * or invalidate() has been called
    //                                             resolveChannelLocator() FAILS
    //                                              NO NOTIFICATIONS DISPATCHED
    //

    // TODO LPP: This state transition diagram is not actually sufficient - it allows onChannelUnavailable() to
    // be sent before onChannelAvailable() in some situations.

    void ChannelLocatorConnection::run()
    {
        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnection::run(): %s started", gPtrToStr(this).c_str() );

        if ( false == m_terminated )
        {
            m_running = true;
        }

        try
        {
            while ( m_running )
            {
                // TODO LPP: Do we need to check m_running at various places (eg. before calling
                // resolveChannelLocator)?

                // If the ChannelLocatorConnection has been invalidated, a state change may be required
                bool pendingInvalidate = false;
                {
                    TA_THREADGUARD( m_resolveLock );
                    pendingInvalidate = m_pendingInvalidate;
                    m_pendingInvalidate = false;
                }

                // This thread is the only one that is allowed to modify the channel locator resolution
                // members, so a lock is not necessary when getting the values of these members.
                if ( !CORBA::is_nil(m_channelLocator) )   // AVAILABLE
                {
                    if ( pendingInvalidate || !isChannelLocatorReachable(getChannelLocatorAddr(m_serviceAddr), m_channelLocator, m_agentID) )
                    {
                        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic,
                            "ChannelLocatorConnection::run(): pendingInvalidate == %d", pendingInvalidate );

                        // AVAILABLE -> UNAVAILABLE
                        TA_THREADGUARD( m_resolveLock );
                        m_channelLocator = TA_Base_Core::IChannelLocator::_nil();
                    }
                    else
                    {
                        // AVAILABLE -> AVAILABLE
                        // We need to dispatch onChannelAvailable() to observers that didn't successfully
                        // process the previous notification.
                    }

                    LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "ChannelLocatorConnection::run(): AVAIL 1" );
                    mergeObserverChanges();
                    LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "ChannelLocatorConnection::run(): AVAIL 2" );
                    dispatchNotifications();
                    LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "ChannelLocatorConnection::run(): AVAIL 3" );
                }
                else                                    // UNAVAILABLE
                {
                    IChannelLocator_var channelLocator;
                    ChannelNameToEventChannelMap channelNameToEventChannelMap;
                    std::string agentID;

                    if ( resolveChannelLocator( getChannelLocatorAddr(m_serviceAddr),
                                                channelLocator.out(),
                                                channelNameToEventChannelMap,
                                                agentID ) )
                    {
                        // UNAVAILABLE -> AVAILABLE
                        {
                            TA_THREADGUARD( m_resolveLock );
                            m_channelLocator = channelLocator;
                            m_channelNameToEventChannelMap.swap(channelNameToEventChannelMap);
                            m_agentID = agentID;
                        }

                        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "ChannelLocatorConnection::run(): UNAVAIL -> AVAIL 1" );
                        mergeObserverChanges();
                        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "ChannelLocatorConnection::run(): UNAVAIL -> AVAIL 2" );
                        dispatchNotifications();
                        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "ChannelLocatorConnection::run(): UNAVAIL -> AVAIL 3" );
                    }
                    else
                    {
                        // State is still UNAVAILABLE, so no notification is dispatched.  But we still
                        // merge observer changes so that m_attachPending will be reset if required.
                        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "ChannelLocatorConnection::run(): UNAVAIL 1" );
                        mergeObserverChanges();
                        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic, "ChannelLocatorConnection::run(): UNAVAIL 2" );
                    }
                }

                // TODO LPP: The current approach will wait for RESOLUTION_SUCCESS_DELAY_MS to re-notify
                // any observers that returned false from onChannelAvailable().  Should we only wait
                // for RESOLUTION_FAILURE_DELAY_MS in this case?
                // The same might happen if invalidate() is called, and an available channel becomes unavailable.
                // TODO LPP: A mechanism for terminating isChannelLocatorReachable() and resolveChannelLocator()
                // early would be useful during shutdown.

                // TODO LPP NOW: If there are no observers, we should wait indefinately until signalled
                ThreadGuard guard(m_condition.getLinkedThreadLockable());
                if ( m_running && !m_attachPending && !m_pendingInvalidate )
                {
                    m_condition.timedWait(!CORBA::is_nil(m_channelLocator) ? RESOLUTION_SUCCESS_DELAY_MS : RESOLUTION_FAILURE_DELAY_MS);
                }
            }

            // Our dtor asserts that no observers are still attached, so we need to make sure
            // m_channelToObserverDetailMap is updated correctly.
            mergeObserverChanges();

		LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "ChannelLocatorConnection::run(): %s stopped", gPtrToStr(this).c_str() );
        }
        catch ( const CORBA::Exception& cex )
        {
            LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "ChannelLocatorConnectionThread run Error: address=%s, exception desc=%s",
                gPtrToStr(this).c_str(), CorbaUtil::getInstance().exceptionToString( cex ).c_str() );
        }
        catch(...)
        {
			LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "ChannelLocatorConnection::run(): %s stopped, catch unknown exception", gPtrToStr(this).c_str() );
        }
    }


    void ChannelLocatorConnection::terminate()
    {
        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "want to terminate the ChannelLocatorConnection thread", gPtrToStr(this).c_str() );
        ThreadGuard guard(m_condition.getLinkedThreadLockable());
        m_running = false;
        m_terminated = true;
        m_condition.signal();

        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnection::terminate(): send terminate signal", gPtrToStr(this).c_str() );
    }


    bool ChannelLocatorConnection::isChannelLocatorReachable( const std::string& corbalocUrl,
                                                              IChannelLocator_ptr channelLocator,
                                                              const std::string& prevAgentID ) const
    {
        TA_Assert(!CORBA::is_nil(channelLocator));
        bool isReachable = false;

        // Check to see if the SAME channel locator resolved previously still exists.
        try
        {
            // This is probably done so that failure and restart of the agent can be detected.
            // If agent fails, the subscription state will not exist (eg. proxy suppliers and
            // receivers, filters, etc).  getID returns a UUID.
            CORBA::String_var id = channelLocator->getID();
            if ( 0 == prevAgentID.compare( id ) )
            {
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic,
                    "ChannelLocatorConnection::isChannelLocatorReachable(): %s was able to contact IChannelLocator at %s",
                    gPtrToStr(this).c_str(), corbalocUrl.c_str() );

                isReachable = true;
            }
            else
            {
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "ChannelLocatorConnection::isChannelLocatorReachable(): %s unable to contact IChannelLocator at %s: agentID has changed",
                    gPtrToStr(this).c_str(), corbalocUrl.c_str() );
            }
        }
        catch ( const CORBA::Exception& cex )
        {
            LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "ChannelLocatorConnection::isChannelLocatorReachable(): %s was unable to contact IChannelLocator at %s: %s",
                gPtrToStr(this).c_str(), corbalocUrl.c_str(), CorbaUtil::getInstance().exceptionToString( cex ).c_str() );
        }

        return isReachable;
    }


    bool ChannelLocatorConnection::resolveChannelLocator( const std::string& corbalocUrl,
                                                          IChannelLocator_out channelLocator,                              // out
                                                          ChannelNameToEventChannelMap& channelNameToEventChannelMap,      // out
                                                          std::string& agentID ) const                                     // out
	{
        bool success = false;

        try
        {
            if (m_resolvedLastTime != success)
            {            
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "ChannelLocatorConnection::resolveChannelLocator(): %s attempting to resolve IChannelLocator at %s",
                     gPtrToStr(this).c_str(), corbalocUrl.c_str() );
            }

            CORBA::Object_var obj = CorbaUtil::getInstance().stringToObject( corbalocUrl );

            if ( ! CORBA::is_nil(obj) )
            {
                if (m_resolvedLastTime != success)
                {
                    LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
                        "ChannelLocatorConnection::resolveChannelLocator(): able to convert string to object reference" );
                }

                // Note that _narrow() may attempt to contact the server (ie. if the object is not
                // contactable, this may be where the exception is thrown)
                omniORB::setClientCallTimeout(obj, 2000);
                channelLocator = TA_Base_Core::IChannelLocator::_narrow(obj);

                if ( ! CORBA::is_nil( channelLocator ) )
                {
                    LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
                         "ChannelLocatorConnection::resolveChannelLocator(): able to narrow object reference to IChannelLocator*" );

                    ChannelMappingSeq_var channels = channelLocator->getChannels();

                    LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
                        "ChannelLocatorConnection::resolveChannelLocator(): able to call IChannelLocator::getChannels" );

                    // Copy the channels into the channel map
                    for ( unsigned int i = 0; i < channels->length(); i++ )
                    {
                        ChannelMapping& mapping = channels[i];
                        channelNameToEventChannelMap[mapping.channelName.in()] = CosNotifyChannelAdmin::EventChannel::_duplicate(mapping.channel);
                    }

                    CORBA::String_var id = channelLocator->getID();

                    agentID = id.in();

                    success = true;
                }
            }
        }
        catch ( const CORBA::Exception& cex )
        {
            // This exception will happen repeatedly if a connection can't be made.
            // For example, TRANSIENT_ConnectFailed will occur repeatedly if a notification service
            // with the required serviceAddr doesn't exist.
            if (m_resolvedLastTime != success)
            {            
                LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
                     "ChannelLocatorConnection::resolveChannelLocator(): an exception was thrown while resolving IChannelLocator: %s",
                     CorbaUtil::getInstance().exceptionToString(cex).c_str() );
            }

            channelLocator = IChannelLocator::_nil();
        }

        if ( success )
        {
            LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                 "ChannelLocatorConnection::resolveChannelLocator(): %s resolved IChannelLocator at %s. Hosting channels %s",
                 gPtrToStr(this).c_str(), corbalocUrl.c_str(), getChannelNamesStr(channelNameToEventChannelMap).c_str() );
        }
        else
        {
            if (m_resolvedLastTime != success)
            {
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "ChannelLocatorConnection::resolveChannelLocator(): %s was unable to resolve IChannelLocator at %s",
                    gPtrToStr(this).c_str(), corbalocUrl.c_str() );
            }
        }

        m_resolvedLastTime = success;
        return success;
    }


    void ChannelLocatorConnection::dispatchNotifications()
    {
        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
            "ChannelLocatorConnection::dispatchNotifications(): %s dispatching to up to %d observers",
            gPtrToStr(this).c_str(), m_channelToObserverDetailMap.size() );

        // NOTE: m_resolveLock and m_observersLock don't need to be acquired providing
        // dispatchNotifications() is called by run() - the run() method is the only
        // one that MODIFIES the members these locks protect.

        // This try-catch block is an aid for debugging.  Without it, the application seems
        // to terminate without warning if exceptions are thrown.
        // Exceptions must not propagate out of onChannelAvailable or onChannelUnavailable.
        try
        {
            typedef ChannelNameToObserverDetailMap::iterator IterType;

            for ( IterType iter = m_channelToObserverDetailMap.begin(); iter != m_channelToObserverDetailMap.end(); ++iter )
            {
                TA_Assert(iter->second != NULL);
                TA_Assert(iter->second->m_observer != NULL);

                // TODO LPP: We should use a more efficient algorithm that steps through the channel names.
                ChannelNameToEventChannelMap::iterator channelIter = m_channelNameToEventChannelMap.find(iter->first);

                TA_Base_Core::ThreadGuard guard( iter->second->m_detachLock );

                if ( iter->second->m_haveDetached )
                {
                    LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                        "ChannelLocatorConnection::dispatchNotifications(): %s has received request to detach observer %s so no notification dispatched",
                        gPtrToStr(this).c_str(), gPtrToStr_PossiblyDeletedHACK(iter->second->m_observer).c_str() );
                }
                else if ( channelIter == m_channelNameToEventChannelMap.end() )
                {
                    // The channel the observer is interested in isn't hosted by the IChannelLocator.
                    // We only call onChannelAvailable() for channels that the IChannelLocator actually provides,
                    // so we only need to call onChannelUnavailable() for those channels as well (there may
                    // be observers that are interested in channels that the currently connected IChannelLocator
                    // doesn't actually host - there is no point notifying these that the channel is invalid).
                    LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic,
                        "ChannelLocatorConnection::dispatchNotifications(): %s not hosting channel %s so no notification dispatched to observer %s",
                        gPtrToStr(this).c_str(), iter->first.c_str(), gPtrToStr(iter->second->m_observer).c_str() );
                }
                else if ( CORBA::is_nil(m_channelLocator) )
                {
                    LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                        "ChannelLocatorConnection::dispatchNotifications(): %s dispatching onChannelUnavailable() to observer %s",
                        gPtrToStr(this).c_str(), gPtrToStr(iter->second->m_observer).c_str() );

                    iter->second->m_haveValidated = false;
                    iter->second->m_observer->onChannelUnavailable(m_serviceAddr);
                }
                else if ( !iter->second->m_haveValidated )
                {
                    // The observer has either not been notified before that the channel is AVAILABLE,
                    // or it was previously notified but returned false.
                    TA_Assert(!CORBA::is_nil(channelIter->second));

                    LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                        "ChannelLocatorConnection::dispatchNotifications(): %s dispatching onChannelAvailable() to observer %s",
                        gPtrToStr(this).c_str(), gPtrToStr(iter->second->m_observer).c_str() );

                    iter->second->m_haveValidated =
                        iter->second->m_observer->onChannelAvailable(m_serviceAddr, channelIter->second, m_channelLocator);

                    if ( !iter->second->m_haveValidated )
                    {
                        // The observer will be notified again if the state is AVAILABLE when the next dispatch occurs.
                        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "ChannelLocatorConnection::dispatchNotifications(): onChannelAvailable() returned false." );
                    }
                }
            }
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "ChannelLocatorConnection::dispatchNotifications(): " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
            TA_Assert(0);
        }
        catch (std::exception& e)
        {
            std::string msg( "ChannelLocatorConnection::dispatchNotifications(): " );
            msg.append( e.what() );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
            TA_Assert(0);
        }
        catch (...)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                "ChannelLocatorConnection::dispatchNotifications(): Unknown exception" );
            TA_Assert(0);
        }
    }


    void ChannelLocatorConnection::mergeObserverChanges()
    {
        TA_THREADGUARD( m_observersLock );

        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
            "ChannelLocatorConnection::mergeObserverChanges(): %s merging %d observers",
            gPtrToStr(this).c_str(), m_obsChangeQueue.size() );

        for ( ObsChangeQueue::iterator queueIter = m_obsChangeQueue.begin(); queueIter != m_obsChangeQueue.end(); ++queueIter )
        {
            TA_Assert(queueIter->m_observer != NULL);

            if ( queueIter->m_hasAttached )
            {
                // The observer was attached.  NOTE that it might already have been deleted.

                //LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                //    "ChannelLocatorConnection::mergeObserverChanges(): %s adding observer %s to notification set",
                //    gPtrToStr(this).c_str(), gPtrToStr_PossiblyDeletedHACK(queueIter->m_observer).c_str(), queueIter->m_channelName.c_str() );

                m_channelToObserverDetailMap.insert( ChannelNameToObserverDetailMap::value_type( queueIter->m_channelName,
                                                                                                 new ObserverDetail(queueIter->m_observer) ) );

                // TODO LPP: Should we assert that the observer hasn't already been attached?
                // If it has already been attached, the new ObserverDetail will leak.
            }
            else
            {
                // The observer was detached and needs to be removed from m_channelToObserverDetailMap

                // We mustn't dereference queueIter->m_observer, because the observer may have been deleted already.
                // In particular, gPtrToStr() must not be called (the Solaris implementation doesn't handle invalid
                // pointers properly) and IChannelObserver::getChannel().

                //LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                //    "ChannelLocatorConnection::mergeObserverChanges(): %s removing observer %s from notification set",
                //    gPtrToStr(this).c_str(), gPtrToStr_PossiblyDeletedHACK(queueIter->m_observer).c_str() );

                ChannelNameToObserverDetailMap::iterator iter = findObserver(queueIter->m_observer, queueIter->m_channelName);
                if ( iter != m_channelToObserverDetailMap.end() )
                {
					LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "remove observer from map, channenlName is %s", queueIter->m_channelName.c_str() );
                    TA_Assert(iter->second != NULL);

                    delete iter->second;
                    m_channelToObserverDetailMap.erase(iter);
                }

                // TODO LPP: Should we assert that the observer had been attached?
            }
        }

        m_obsChangeQueue.clear();
        m_attachPending = false;
    }


    ChannelLocatorConnection::ChannelNameToObserverDetailMap::iterator ChannelLocatorConnection::findObserver( IChannelObserver* obs,
                                                                                                               const std::string& channelName )
    {
        // THREAD SAFETY: m_observersLock must be acquired before calling findObserver(), unless
        // called from the run() thread.

        TA_Assert(obs != NULL);

        typedef ChannelNameToObserverDetailMap::iterator IterType;
        std::pair<IterType, IterType> bounds = m_channelToObserverDetailMap.equal_range(channelName);
        for ( IterType iter = bounds.first; iter != bounds.second; ++iter )
        {
            TA_Assert(iter->second != NULL);

            if ( obs == iter->second->m_observer )
            {
                return iter;
            }
        }

        return m_channelToObserverDetailMap.end();
    }


    std::string ChannelLocatorConnection::getChannelNamesStr(ChannelLocatorConnection::ChannelNameToEventChannelMap& map)
    {
        std::vector<std::string> channelNames;
        for ( ChannelNameToEventChannelMap::iterator iter = map.begin(); iter != map.end(); ++iter )
        {
            channelNames.push_back(iter->first);
        }

        return toString(channelNames.begin(), channelNames.end());
    }


    ChannelLocatorConnection::ObserverChangeDetail::ObserverChangeDetail(IChannelObserver* obs, bool hasAttached)
        : m_observer(obs),
          m_hasAttached(hasAttached),
          m_channelName(obs->getChannelName() )
    {
    }
 
}; // TA_Base_Core

