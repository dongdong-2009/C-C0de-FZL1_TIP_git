/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/ConnectionChecker.cpp $
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

#include "core/message/src/ConnectionChecker.h"
#include "core/message/src/ConnectionThread.h"
#include "core/message/src/EventChannelUser.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"


namespace TA_Base_Core
{
    // initialise statics
    TA_Base_Core::NonReEntrantThreadLockable ChannelLocatorConnectionMgr::m_singletonLock;
    ChannelLocatorConnectionMgr* ChannelLocatorConnectionMgr::m_theClassInstance = NULL;
    volatile bool ChannelLocatorConnectionMgr::s_shutdownInProgress = false;

    ChannelLocatorConnectionMgr::ChannelLocatorConnectionMgr()
    {
    }


    void ChannelLocatorConnectionMgr::addConnection( const std::string& serviceAddr, unsigned long locationKey )
    {
        if (s_shutdownInProgress)
        {
            return;
        }

        LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnectionMgr::addConnection(): adding serviceAddr %s, location key %d",
            serviceAddr.c_str(), locationKey );

        TA_Assert(serviceAddr.length() > 0);

        TA_THREADGUARD( m_connectionsLock );

        // Check to see if the connection has already been added
        ConnectionsMap::iterator it = m_connectionsMap.find( serviceAddr );
        if ( it == m_connectionsMap.end() )
        {
            // The connection was not found, so add a new connection
            ChannelLocatorConnection* connection = new ChannelLocatorConnection(serviceAddr);
            m_connectionsMap.insert( ConnectionsMap::value_type(serviceAddr, ConnectionDetail(connection, locationKey)) );

            // TODO LPP: It seems inefficient to start the thread if there are no observers attached.
            // However, avoiding deadlock when starting and stopping in response to attaching and
            // detaching observers seems to be a non-trivial problem.
            connection->start();

            TA_THREADGUARD( m_locationsLock );

            // Attach each existing IChannelObserver to the new ChannelLocatorConnection thread
            typedef LocationToObservers::iterator IterType;
            std::pair<IterType, IterType> bounds = m_locationToObservers.equal_range(locationKey);
            for ( IterType iter = bounds.first; iter != bounds.second; ++iter )
            {
                TA_Assert(iter->second != NULL);
                TA_Assert(iter->second->getLocationKey() == locationKey);
                connection->attach( iter->second );
            }
        }
        else
        {
            TA_Assert(it->second.m_connection != NULL);

            // The connection has already been added.  A different locationKey must NOT be specified.
            // TODO LPP: Perhaps this shouldn't be an assertion - it may be caused by a problem
            // with configuration.
            //Causes windows notification service for KRTC to crash
            LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "ChannelLocatorConnectionMgr::addConnection(): adding serviceAddr %s, location key %d, m_location key %d",
                serviceAddr.c_str(), locationKey,it->second.m_locationKey );
            //TA_Assert(it->second.m_locationKey == locationKey);
        }
    }


    ChannelLocatorConnectionMgr::~ChannelLocatorConnectionMgr()
    {
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnectionMgr::~ChannelLocatorConnectionMgr(): cleaning up %d ChannelLocatorConnections",
            m_connectionsMap.size() );

        TA_THREADGUARD( m_connectionsLock );

        // All ChannelObservers must have unregistered already.
        for ( LocationToObservers::iterator iter = m_locationToObservers.begin(); iter != m_locationToObservers.end(); ++iter )
        {
            LOG1( SourceInfo, DebugUtil::DebugDebug,
                "ChannelLocatorConnectionMgr::~ChannelLocatorConnectionMgr(): observer %s still attached",
                gPtrToStr(iter->second).c_str() );
        }

        TA_Assert(m_locationToObservers.empty());

        // Clean up the connections
        for ( ConnectionsMap::iterator it = m_connectionsMap.begin(); it != m_connectionsMap.end(); ++it )
        {
            TA_Assert(it->second.m_connection != NULL);
            it->second.m_connection->terminate();
        }

        for ( ConnectionsMap::iterator it = m_connectionsMap.begin(); it != m_connectionsMap.end(); ++it )
        {
            TA_Assert(it->second.m_connection != NULL);

            // TODO LPP: Calling terminateAndWait() serially on multiple ChannelLocatorConnections may
            // cause a significant delay during shutdown.
            it->second.m_connection->terminateAndWait();
            delete it->second.m_connection;
            it->second.m_connection = NULL;
        }

        m_connectionsMap.clear();
    }


    void ChannelLocatorConnectionMgr::invalidateConnection( const std::string& serviceAddr )
    {
        TA_THREADGUARD( m_connectionsLock );

        ConnectionsMap::iterator it = m_connectionsMap.find( serviceAddr );
        if ( it != m_connectionsMap.end() )
        {
            TA_Assert(it->second.m_connection != NULL);
            it->second.m_connection->invalidate();
        }
    }


    ChannelLocatorConnectionMgr& ChannelLocatorConnectionMgr::getInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        if ( NULL == m_theClassInstance )
        {
            LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "ChannelLocatorConnectionMgr::getInstance(): creating singleton instance" );

            // Create the one & only object
            m_theClassInstance = new ChannelLocatorConnectionMgr();

            // TODO LPP: It might be clearer if some client had to make an explicit call to add the
            // --notify-hosts connections, rather than magically adding them here.
            gAddNotifyHostConnections(m_theClassInstance);
        }

        return *m_theClassInstance;
    }


    void ChannelLocatorConnectionMgr::removeInstance()
    {
        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnectionMgr::removeInstance(): m_theClassInstance=%s", gPtrToStr(m_theClassInstance).c_str() );

        TA_THREADGUARD( m_singletonLock );

        if ( m_theClassInstance != NULL )
        {
            delete m_theClassInstance;
            m_theClassInstance = 0;
        }
    }


    void ChannelLocatorConnectionMgr::attach( IChannelObserver* obs )
    {
        if (s_shutdownInProgress)
        {
            return;
        }

        TA_ASSERT( NULL != obs, "The IChannelObserver trying to register was null" );

        LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnectionMgr::attach(): attaching IChannelObserver %s to all ChannelLocatorConnections "
            "at locationKey %d for channel %s",
            gPtrToStr(obs).c_str(), obs->getLocationKey(), obs->getChannelName().c_str() );

        // TODO LPP: What happens if an IChannelObserver is attached more than once?  Do we
        // need to assert that this isn't done?

        {
            TA_THREADGUARD( m_connectionsLock );

            // Register the observer with ChannelLocatorConnection threads
            for ( ConnectionsMap::iterator it = m_connectionsMap.begin(); it != m_connectionsMap.end(); ++it )
            {
                TA_Assert(it->second.m_connection != NULL);

                // The observer is only interested in notifications relating to channel
                // availability for a particular location.
                if ( it->second.m_locationKey == obs->getLocationKey() )
                {
                    it->second.m_connection->attach( obs );
                }
            }
        }

        {
            TA_THREADGUARD( m_locationsLock );

            // Add the observer to the location map
            m_locationToObservers.insert(
                std::pair<unsigned long, IChannelObserver*>( obs->getLocationKey(), obs ) );
        }
    }


    void ChannelLocatorConnectionMgr::detach( IChannelObserver* obs )
    {
        TA_ASSERT( NULL != obs, "The IChannelObserver trying to de-register was null" );

        LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ChannelLocatorConnectionMgr::detach(): detaching IChannelObserver %s on channel %s from ChannelLocatorConnection threads",
            gPtrToStr(obs).c_str(), obs->getChannelName().c_str() );

        {
            TA_THREADGUARD( m_connectionsLock );

            // Deregister the observer with ChannelLocatorConnection threads.
            for ( ConnectionsMap::iterator it = m_connectionsMap.begin(); it != m_connectionsMap.end(); ++it )
            {
                // The observer only attached to ChannelLocatorConnection threads for a
                // particular location.
                if ( it->second.m_locationKey == obs->getLocationKey() )
                {
                    it->second.m_connection->detach( obs );
                }
            }
        }

        {
            TA_THREADGUARD( m_locationsLock );

            // Remove the observer from the locations map
            typedef LocationToObservers::iterator IterType;
            std::pair<IterType, IterType> bounds = m_locationToObservers.equal_range(obs->getLocationKey());
            for ( IterType iter = bounds.first; iter != bounds.second; ++iter )
            {
                if ( iter->second == obs )
                {
                    m_locationToObservers.erase(iter);
                    break;
                }
            }
        }
    }


    void gAddNotifyHostConnections(ChannelLocatorConnectionMgr* mgr)
    {
        TA_Assert(mgr != NULL);

        // Parse the NotifyHosts runparam and add each host to the manager.
        std::string notifyHosts = TA_Base_Core::RunParams::getInstance().get( RPARAM_NOTIFYHOSTS );
        std::vector<std::string> tokens;
        gTokeniseCommaSeparatedString(notifyHosts, tokens);

        for (unsigned int i = 0; i < tokens.size(); ++i)
        {
            mgr->addConnection( tokens[i], gGetLocalLocationKey() );
        }
    }

}; // TA_Base_Core

