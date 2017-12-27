/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NotificationAgent.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/12/17 15:24:44 $
 * Last modified by:  $Author: shurui.liu $
 * 
 * Agent wrapper for CORBA Notification Service
 *
 */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <ctime>
#include <exception>
#ifdef WIN32
    #include <windows.h>
	#include <winsock2.h>
#else
	#include <unistd.h>
#endif


#include "COS/CosNotification.hh"

#include "app/notification_service/src/NotificationAgent.h"
#include "app/notification_service/src/NotificationServiceConstants.h"
#include "app/notification_service/src/NotificationServiceThread.h"
#include "app/notification_service/src/NotificationStatisticsThread.h"
#include "app/notification_service/src/Federator.h"
#include "app/notification_service/src/Gateway.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/utilities/src/StdPubUtility.h"
#include "core/data_access_interface/entity_access/src/NotificationAgentData.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_String.h"
#include "core/threads/src/Thread.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/CorbaException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/uuid/src/TAuuid.h"
#include "core/message/src/MessagePublicationManager.h"
#include <algorithm>
#include <functional>

// For LOCAL_LOCATION and ANY_LOCATION
#include "core/message/src/SubscriptionManager.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"

#include "omniNotify.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::NotificationAgentData;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DbConnectionStrings;

namespace TA_Base_App
{
    NotificationAgent::FederatorKey::FederatorKey( const std::string& channel,
                                                   unsigned long supplierLocationKey,
                                                   const std::string& supplierServiceAddr )
        : m_channel( channel ),
          m_supplierLocationKey( supplierLocationKey ),
          m_supplierServiceAddr( supplierServiceAddr )
    {
        FUNCTION_ENTRY( "FederatorKey" );
        FUNCTION_EXIT;
    }


    bool NotificationAgent::FederatorKey::operator< (const FederatorKey& other) const
    {
        return ( m_channel < other.m_channel ) ||
               ( m_channel == other.m_channel && m_supplierLocationKey < other.m_supplierLocationKey ) ||
               ( m_channel == other.m_channel && m_supplierLocationKey == other.m_supplierLocationKey && m_supplierServiceAddr < other.m_supplierServiceAddr );
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NotificationAgent::NotificationAgent( int argc, char* argv[] )
        : m_agent( NULL ),
          m_nsaLocator( NULL ),
          m_initialised( false ),
          m_serviceThread( NULL ),
          m_fedMgr( new FederationForwarderMgr ),
          m_statisticsThread(NULL),
          m_eventChannelFactory(CosNotifyChannelAdmin::EventChannelFactory::_nil())
    {
        FUNCTION_ENTRY( "NotificationAgent" );

        // Generate the agent ID for this instance
        m_agentID = TA_Base_Core::TA_Uuid::generateUUID();

        try
        {
            RunParams::getInstance().set( RPARAM_OPERATIONMODE, RPARAM_CONTROL );
            RunParams::getInstance().set( RPARAM_ENABLESUBSCRIPTIONCHANGE, "1" );

            // We have to call CorbaUtil::initialise() before we construct the GenericAgent so that we
            // can specify the pinned port that we want (GenericAgent uses the UtilityInitialiser to
            // initialise CorbaUtil with a default port).
            // TODO LPP: A side effect of this is that debug information won't be written to the
            // --debug-file until the GenericAgent is constructed, because the UtilityInitialiser
            // configures the logging settings.  This is quite messy.
            if ( false == RunParams::getInstance().parseCmdLine(argc, argv) )
            {
                if ( RunParams::getInstance().isSet(RPARAM_VERSION) )
                {
                    // Version information will have been printed to std::cout
                    // No need to proceed any further.

                    FUNCTION_EXIT;
                    exit(0);
                }
            }

            TA_Base_Core::MessagePublicationManager::getInstance().setEnableSubChangeUpdates( true ); // limin++ CL-21308
            // Create the new NSALocator
            m_nsaLocator = new NSALocator();

            if ( RunParams::getInstance().isSet(RPARAM_NOTIFYPORT) )
            {
                m_localNotifyPort = atoi( RunParams::getInstance().get( RPARAM_NOTIFYPORT ).c_str() );
                TA_ASSERT( m_localNotifyPort > 1024 && m_localNotifyPort < 65535, "Port number to pin out of range");
            }
            else
            {
                m_localNotifyPort = TA_Base_Core::IChannelLocator::DEFAULT_LOCATOR_PORT;
            }

            TA_Base_Core::CorbaUtil::getInstance().initialise( m_localNotifyPort );

            // Set Servant key so that IChannelLocator clients can find the Agent
            // TA_Base_Core::CorbaUtil::getInstance().setServantKey( this,  // limin-- CL-21308
            //     TA_Base_Core::IChannelLocator::DEFAULT_LOCATOR_NAME );

            // Create the new generic agent
            m_agent = new TA_Base_Bus::GenericAgent( argc, argv, this, NULL, AGENT_USER_TYPE_NORMAL );

			
            // DebugUtil has been initialised by now, so this message will be output to the log file.
            LOG2( SourceInfo, DebugUtil::DebugInfo,
                "NotificationAgent::NotificationAgent(): listening on port %d, servantKey '%s'",
                m_localNotifyPort, TA_Base_Core::IChannelLocator::DEFAULT_LOCATOR_NAME );
        }
        catch( std::exception& sex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "std::exception", sex.what() );

            // Clean up GenericAgent if necessary
            if ( m_agent != NULL )
            {
                m_agent->deactivateAndDeleteServant();
                m_agent = NULL;
            }
            throw;
        }

        FUNCTION_EXIT;
    }


    NotificationAgent::~NotificationAgent()
    {
        FUNCTION_ENTRY( "~NotificationAgent" );

        // agentTerminate() should already have been called by now.  It tears down all
        // federations with suppliers (ie. Federators) and consumers (ie. FederationForwarderMgr),
        // and terminates the service and statistics threads.
        TA_Assert(NULL == m_fedMgr);
        TA_Assert(NULL == m_serviceThread);
        TA_Assert(NULL == m_statisticsThread);

        try
        {
            // deactivate GenericAgent
            if ( m_agent != NULL )
            {
                m_agent->deactivateAndDeleteServant();
                m_agent = NULL;
            }

            // yanrong++: CL-21222: m_fedMgr should has been deleted before here, no need delete again
            /*if ( m_fedMgr != NULL )
            {
                delete m_fedMgr;
                m_fedMgr = NULL;
            }*/
            //

            delete m_nsaLocator;
            m_nsaLocator = NULL;
        }
        catch( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Trying to deactivate GenericAgent" );
        }

        FUNCTION_EXIT;
    }


    void NotificationAgent::startService()
    {
        FUNCTION_ENTRY( "startService" );

        try
        {
            // Use omniNotify - create the commandline parameters
            // This vector will hold all arguments to be passed to the Notification Service
            std::vector<std::string> notifyArguments;

            // do not register with the naming service
            notifyArguments.push_back( "-n" );

            // load config file
            notifyArguments.push_back( "-c" );

            if ( TA_Base_Core::RunParams::getInstance().isSet( OMNI_NOTIFY_CONFIG_FILE ) )
            {
                // path as specified on the command line
                notifyArguments.push_back( TA_Base_Core::RunParams::getInstance().get( OMNI_NOTIFY_CONFIG_FILE ) );
            }
            else
            {
                // default path to config file
                notifyArguments.push_back( "./omni_notification.cfg" );
            }

            // Create thread to execute Notification Service and run it. Use the agent's
            // address as a unique id for the NotificationServiceThread.
            m_serviceThread = new NotificationServiceThread( "notifd", notifyArguments );

            // Start the thread, this will activate the service
            m_serviceThread->start();

            TA_Base_Core::Thread::sleep(1000);
            resolveEventChannelFactory();
            TA_Base_Core::Thread::sleep( 500 );
            createEventChannels();

            // Let the service sleep for a while to sort itself out
            TA_Base_Core::Thread::sleep( 500 );

			// Start the statistics monitoring
			
			//xinsong++ no need to launch the statistics thread
			//m_statisticsThread = new NotificationStatisticsThread(m_channels);
			//TA_ASSERT(m_statisticsThread!=NULL,"Unable to construct statistics thread.");
			//m_statisticsThread->start();

            TA_Base_Core::CorbaUtil::getInstance().setServantKey( this, TA_Base_Core::IChannelLocator::DEFAULT_LOCATOR_NAME );

            // The agent is now ready to accept IChannelLocator requests
            activateServant();

            m_initialised = true;

            // Finished initialisation, call GenericAgent::run()
            m_agent->run();
        }
        catch (CORBA::Exception& e)
        {
            std::string msg(  TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );
            agentTerminate();
            throw;
        }
        catch (std::exception& e)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "std::exception", e.what() );
            agentTerminate();
            throw;
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Unable to start Notification Service" );
            agentTerminate();
            throw;
        }

        FUNCTION_EXIT;
    }


    void NotificationAgent::agentTerminate()
    {
        FUNCTION_ENTRY( "agentTerminate" );

        // Tear down any existing federations
        {
            // yanrong++: CL-21222, guard m_federators
            TA_THREADGUARD( m_federatorsLock );
            // ++yanrong
            for ( Federators::iterator iter = m_federators.begin(); iter != m_federators.end(); ++iter )
            {
                delete iter->second.m_federator;
                iter->second.m_federator = NULL;
            }
            m_federators.clear();
        }

        // yanrong++: CL-21222, guard m_fedMgr
        {
            TA_THREADGUARD( m_fedMgrLock );
            delete m_fedMgr;
            m_fedMgr = NULL;
        }
        // ++yanrong

        // terminate the statistics thread
        if ( m_statisticsThread != NULL )
        {
            m_statisticsThread->terminateAndWait();
            delete m_statisticsThread;
            m_statisticsThread = NULL;
        }

        // terminate the service thread
        if ( m_serviceThread != NULL )
        {
            m_serviceThread->terminateAndWait();
            delete m_serviceThread;
            m_serviceThread = NULL;
        }

        // Release all of the channel pointers
        {
            // yanrong++: CL-21222: guard m_channels
            TA_THREADGUARD( m_channelsLock );
            // ++yanrong
            for ( ChannelNameToChannelDetailsMap::iterator iter = m_channels.begin(); iter != m_channels.end(); ++iter )
		    {
			    CORBA::release( iter->second.eventChannel );
		    }
		    m_channels.clear();
        }

        deactivateServant();

        FUNCTION_EXIT;
    }


    TA_Base_Bus::IEntity* NotificationAgent::createEntity( TA_Base_Core::IEntityDataPtr entityData )
    {
        FUNCTION_ENTRY( "createEntity" );

        // createEntity() is called once for each child entity.  The child entities represent
        // the event channels that need to be created.

        LOG2( SourceInfo, DebugUtil::DebugDebug,
            "NotificationAgent::createEntity(): child entity pkey=%d, channelName=%s",
            entityData->getKey(), entityData->getAddress().c_str() );

        ChannelDetails  details;
        details.eventChannel = TA_Base_Core::IFederatableEventChannel::_nil();
        details.entityKey    = entityData->getKey();

        // getAddress() should return the name of the event channel.
        m_channels.insert( ChannelNameToChannelDetailsMap::value_type( entityData->getAddress(), details ) );

        FUNCTION_EXIT;
        return NULL;
    }


    void NotificationAgent::resolveEventChannelFactory()
    {
        FUNCTION_ENTRY( "resolveEventChannelFactory" );

        TA_ASSERT( CORBA::is_nil( m_eventChannelFactory ), "EventChannelFactory has already been resolved" );

        // Start time for resolving channel
        time_t startTime = time( NULL );

        while ( ( time( NULL ) - startTime ) < RESOLVE_CHANNELFACTORY_TIMEOUT )
        {
            try
            {
                // Use omniNotify - resolve the event channel factory
                AttNotification::Server_var server = AttNotification::Server::_nil();

                // Initialise the Notification Service
                server = omniNotify::get_server();

                if ( CORBA::is_nil( server ) )
                {
                    LOG0( SourceInfo, DebugUtil::DebugWarn,
                        "NotificationAgent::resolveEventChannelFactory(): unable to resolve EventChannelFactory - retrying." );
                }
                else
                {
                    AttNotification::EventChannelFactory_var channelFactory = server->get_channel_factory();

                    m_eventChannelFactory = CosNotifyChannelAdmin::EventChannelFactory::_narrow( channelFactory );

                    FUNCTION_EXIT;
                    return;
                }
            }
            catch( CORBA::Exception& cex )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch,
                    "CORBA", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ).c_str() );
            }

            // The Notification Service might not have made the EventChannelFactory available yet. Sleep
            // and try again (as long as the timeout has not been met)
            TA_Base_Core::Thread::sleep( 500 );
        }

        // Failed to resolve Event Channel factory
        TA_THROW( TA_Base_Core::TransactiveException( "Failed to resolve Notification Event Channel Factory" ) );

        FUNCTION_EXIT;
    }


    void NotificationAgent::createEventChannels()
    {
        FUNCTION_ENTRY( "createEventChannels" );

        TA_ASSERT( ! CORBA::is_nil( m_eventChannelFactory ), "EventChannelFactory has not been resolved" );

        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "NotificationAgent::createEventChannels() creating %d channels", m_channels.size() );

        for ( ChannelNameToChannelDetailsMap::iterator it = m_channels.begin(); it != m_channels.end(); it++ )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                 "NotificationAgent::createEventChannels(): Creating FederatableEventChannel_impl for channel %s", it->first.c_str() );

            TA_Assert(CORBA::is_nil(it->second.eventChannel));

            // Store the channel reference so that it can be passed onto IChannelLocator clients
            // Can throw CORBA exceptions.
			it->second.eventChannel = (new FederatableEventChannel_impl(m_eventChannelFactory))->_this();
        }

        FUNCTION_EXIT;
    }


    char* NotificationAgent::getID()
    {
        return CORBA::string_dup( m_agentID.c_str() );
    }


    // yanrong++: CL-21222, 2010-01-27: wait until connections initialized completely
    void NotificationAgent::waitInitialised()
    {
        FUNCTION_ENTRY( "waitInitialised" );

        while ( ! m_initialised )
        {
            TA_Base_Core::Thread::sleep( 100 );
        }

        FUNCTION_EXIT;
    }


    // ++yanrong

    TA_Base_Core::ChannelMappingSeq* NotificationAgent::getChannels()
    {
        FUNCTION_ENTRY( "getChannels" );

        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, "NotificationAgent::getChannels() called" );

        // Guard this against MT access
        // TA_THREADGUARD( m_channelsLock );

        // yanrong++: CL-21222
        /*
        while ( ! m_initialised )
        {
            TA_Base_Core::Thread::sleep( 100 );
        }
        */
        waitInitialised();
        // ++yanrong

        TA_THREADGUARD( m_channelsLock );

        // Create the new channel sequence
        TA_Base_Core::ChannelMappingSeq* channels = new TA_Base_Core::ChannelMappingSeq;

        channels->length( m_channels.size() );

        int count = 0;
        for ( ChannelNameToChannelDetailsMap::iterator iter = m_channels.begin(); iter != m_channels.end(); ++iter )
        {
            // Create the new ChannelMapping
            TA_Base_Core::ChannelMapping& channel = (*channels)[count++];
            channel.channelName = iter->first.c_str();
            channel.channel = CosNotifyChannelAdmin::EventChannel::_narrow( iter->second.eventChannel ); // _narrow implicitly duplicate
        }

        FUNCTION_EXIT;
        return channels;
    }


    void NotificationAgent::federateIfRemoteSource(CORBA::ULong supplierLocationKey, const char* channelName)
    {
        FUNCTION_ENTRY( "federateIfRemoteSource" );

        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "NotificationAgent::federateIfRemoteSource(): channelName = %s, supplierLocationKey = %d",
            channelName, supplierLocationKey );

        // If federation is disabled, or the source location is the same as our location, do nothing
        bool federationDisabled = RunParams::getInstance().isSet("FederationDisabled");
        if ( federationDisabled ||
             supplierLocationKey == TA_Base_Core::gGetLocalLocationKey() ||
             supplierLocationKey == TA_Base_Core::LOCAL_LOCATION )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "NotificationAgent::federateIfRemoteSource(): will NOT federate. FederationDisabled = %d", federationDisabled );

            FUNCTION_EXIT;
            return;
        }

        // Get vector of NSAs we need to contact.  Note that we are not interested in federating with
        // broadcast notification services or services at our own location.
        NSALocator::NSADetailVec nsaDetails;
        if ( NULL == m_nsaLocator )
        {
            FUNCTION_EXIT;
            return;
        }

        m_nsaLocator->findNSAsAtLocation(supplierLocationKey, nsaDetails);

        for ( unsigned int i = 0; i < nsaDetails.size(); i++ )
        {
            std::string serviceAddr(TA_Base_Core::gMakeServiceAddr(nsaDetails[i].m_hostname, nsaDetails[i].m_portnum));

            if ( nsaDetails[i].m_isBroadcast || nsaDetails[i].m_locationKey == TA_Base_Core::gGetLocalLocationKey() )
            {
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "NotificationAgent::federateIfRemoteSource(): Not federating with NSA at %s: m_isBroadcast=%d",
                    serviceAddr.c_str(), nsaDetails[i].m_isBroadcast );
            }
            else
            {
                addFederator( channelName, serviceAddr, nsaDetails[i].m_locationKey );
            }
        }

        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "NotificationAgent::federateIfRemoteSource(): exiting" );

        FUNCTION_EXIT;
    }


    void NotificationAgent::unfederateIfRemoteSource(CORBA::ULong supplierLocationKey, const char* channelName)
    {
        FUNCTION_ENTRY( "unfederateIfRemoteSource" );

        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "NotificationAgent::unfederateIfRemoteSource(): channelName = %s, supplierLocationKey = %d",
            channelName, supplierLocationKey );

        // If federation is disabled, or the source location is the same as our location, do nothing
        bool federationDisabled = RunParams::getInstance().isSet("FederationDisabled");
        if ( federationDisabled ||
             supplierLocationKey == TA_Base_Core::gGetLocalLocationKey() ||
             supplierLocationKey == TA_Base_Core::LOCAL_LOCATION )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "NotificationAgent::unfederateIfRemoteSource(): will NOT unfederate. FederationDisabled = %d", federationDisabled );

            FUNCTION_EXIT;
            return;
        }

        // Get vector of NSAs we need to contact.  Note that we are not interested in unfederating with
        // broadcast notification services or services at our own location.
        NSALocator::NSADetailVec nsaDetails;
        if ( NULL == m_nsaLocator )
        {
            FUNCTION_EXIT;
            return;
        }

        m_nsaLocator->findNSAsAtLocation(supplierLocationKey, nsaDetails);

        for ( unsigned int i = 0; i < nsaDetails.size(); i++ )
        {
            std::string serviceAddr(TA_Base_Core::gMakeServiceAddr(nsaDetails[i].m_hostname, nsaDetails[i].m_portnum));

            if ( nsaDetails[i].m_isBroadcast || nsaDetails[i].m_locationKey == TA_Base_Core::gGetLocalLocationKey() )
            {
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "NotificationAgent::unfederateIfRemoteSource(): Not unfederating with NSA at %s: m_isBroadcast=%d",
                    serviceAddr.c_str(), nsaDetails[i].m_isBroadcast );
            }
            else
            {
                removeFederator( channelName, serviceAddr, nsaDetails[i].m_locationKey );
            }
        }

        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "NotificationAgent::unfederateIfRemoteSource(): exiting" );

        FUNCTION_EXIT;
    }


    void NotificationAgent::federateWithRemoteIChannelLocator( const char* consumerServiceAddr,
                                                               CORBA::ULong consumerLocationKey,
                                                               const char* channelName )
    {
        FUNCTION_ENTRY( "federateWithRemoteIChannelLocator" );

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "NotificationAgent::federateWithRemoteIChannelLocator(): consumerServiceAddr=%s, consumerLocationKey=%d, channelName=%s",
            consumerServiceAddr, consumerLocationKey, channelName );

        // yanrong++: CL-21222, wait until channels initialization.
        waitInitialised();
        // ++yanrong

        // Check if this is a valid location key
        if ( consumerLocationKey == TA_Base_Core::LOCAL_LOCATION || consumerLocationKey == TA_Base_Core::ANY_LOCATION )
        {
            std::string errMsg("Invalid location key");
            TA_THROW_CORBA( TA_Base_Core::FederationFailed(CORBA::string_dup(errMsg.c_str())) );
        }

        // Find the object reference for the local channel
        TA_Base_Core::IFederatableEventChannel_ptr supplierChannel = TA_Base_Core::IFederatableEventChannel::_nil();
        {
            {
                // limin++ CL-21308
                TA_THREADGUARD( m_channelsLock );

                ChannelNameToChannelDetailsMap::iterator iter = m_channels.find( channelName );
                if ( iter == m_channels.end() )
                {
                    std::stringstream ss;
                    ss << "Unable to federate with consumer as the channel isn't hosted by this NotificationAgent";
                    TA_THROW_CORBA( TA_Base_Core::FederationFailed(CORBA::string_dup(ss.str().c_str())) );
                }

                supplierChannel = iter->second.eventChannel;
            }

            // yanrong++: CL-21222: put into m_channelsLock and guard m_fedMgr
            // TODO LPP: We add the connection here, but we never remove it.  Is this a problem?
            TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().addConnection( consumerServiceAddr, consumerLocationKey );

            // guard m_fedMgr
            {
                TA_THREADGUARD( m_fedMgrLock );
                //TA_Assert(m_fedMgr != NULL);
                if ( m_fedMgr != NULL )
                {
                    TA_Assert(!CORBA::is_nil(supplierChannel));
                    m_fedMgr->federateWithConsumers(channelName, consumerLocationKey, supplierChannel);
                }
            }
            // ++yanrong
        }

        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "NotificationAgent::federateWithRemoteIChannelLocator(): exiting" );

        FUNCTION_EXIT;
    }


    void NotificationAgent::unfederateWithRemoteIChannelLocator(CORBA::ULong consumerLocationKey, const char* channelName)
    {
        FUNCTION_ENTRY( "unfederateWithRemoteIChannelLocator" );

        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "NotificationAgent::unfederateWithRemoteIChannelLocator(): consumerLocationKey=%d, channelName=%s",
            consumerLocationKey, channelName );

        // Check if this is a valid location key
        if ( consumerLocationKey == TA_Base_Core::LOCAL_LOCATION || consumerLocationKey == TA_Base_Core::ANY_LOCATION )
        {
            std::string errMsg("Invalid location key");
            TA_THROW_CORBA( TA_Base_Core::FederationFailed(CORBA::string_dup(errMsg.c_str())) );
        }

        // yanrong++: CL-21222, guard m_fedMgr
        {
            TA_THREADGUARD( m_fedMgrLock );
            if ( m_fedMgr != NULL )
            {
                m_fedMgr->unfederateWithConsumers(channelName, consumerLocationKey);
            }
        }
        // ++yanrong

        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "NotificationAgent::unfederateWithRemoteIChannelLocator(): exiting" );

        FUNCTION_EXIT;
    }


    void NotificationAgent::addFederator( const std::string& channelName,
                                          const std::string& supplierServiceAddr,
                                          unsigned long supplierLocationKey )
    {
        FUNCTION_ENTRY( "addFederator" );

        TA_Assert(supplierLocationKey != TA_Base_Core::gGetLocalLocationKey());

        std::string ourServiceAddr( TA_Base_Core::gMakeServiceAddr(gGetHostname(), m_localNotifyPort) );

        LOG4( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "NotificationAgent::addFederator(): channelName=%s, ourServiceAddr=%s, supplierServiceAddr=%s, supplierLocationKey=%d",
            channelName.c_str(), ourServiceAddr.c_str(), supplierServiceAddr.c_str(), supplierLocationKey );

        TA_THREADGUARD( m_federatorsLock );

        // Check to see if we already have a federator created for this connection
        FederatorKey key( channelName, supplierLocationKey, supplierServiceAddr );
        Federators::iterator iter = m_federators.find(key);
        if ( iter == m_federators.end() )
        {
            FederatorKey key(channelName, supplierLocationKey, supplierServiceAddr);
            Federator* federator =
                new Federator( ourServiceAddr, TA_Base_Core::gGetLocalLocationKey(), supplierServiceAddr, supplierLocationKey, channelName );

            m_federators.insert( Federators::value_type(key, FederatorDetail(federator)) );
        }
        else
        {
            iter->second.ref();
        }

        FUNCTION_EXIT;
    }


    void NotificationAgent::removeFederator( const std::string& channelName,
                                             const std::string& supplierServiceAddr,
                                             unsigned long supplierLocationKey )
    {
        FUNCTION_ENTRY( "removeFederator" );

        TA_Assert(supplierLocationKey != TA_Base_Core::gGetLocalLocationKey());

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "NotificationAgent::removeFederator(): channelName=%s, supplierServiceAddr=%s, supplierLocationKey=%d",
            channelName.c_str(), supplierServiceAddr.c_str(), supplierLocationKey );

        TA_THREADGUARD( m_federatorsLock );

        // Decrement the refcount for the Federator for this connection, and delete if no longer used.
        FederatorKey key( channelName, supplierLocationKey, supplierServiceAddr );
        Federators::iterator iter = m_federators.find(key);
        if ( iter != m_federators.end() )
        {
            TA_Assert(iter->second.m_federator != NULL);

            iter->second.unref();

            // If the refcount reaches zero, clean up the Federator
            if ( 0 == iter->second.m_refCount )
            {
                delete iter->second.m_federator;
                m_federators.erase(iter);
            }
        }
        // TODO LPP: Should we assert that the FederatorKey was found in our map?

        FUNCTION_EXIT;
    }


    std::string gGetHostname()
    {
        char name[32];
        int length = 32;
        gethostname(name, length);

        return name;
    }

#if defined( SOLARIS )
    #define _stricmp TA_Base_Core::CStdPubUtility::stricmp
#endif

    char * NotificationAgent::processCommand(const char* cmd, const char* args, CORBA::Boolean& success,
        CORBA::Boolean& targetChanged, TA_Base_Core::ISCSInteractive_out nextTargetObj)
    {
        assert(cmd!=NULL && args!=NULL);

        success = false;
        nextTargetObj = TA_Base_Core::ISCSInteractive::_nil();
        targetChanged = false;

        AttNotification::Interactive_var newTargetObj;

        AttNotification::Server_var server = omniNotify::get_server();

        char commandBuffer[255];
        commandBuffer[0] = 0x0;

        if(_stricmp( "addflag", cmd ) == 0 )
        {
            sprintf(commandBuffer, "%c%s", '+', args);
        }
        else if(_stricmp( "removeflag", cmd ) == 0 )
        {
            sprintf(commandBuffer, "%c%s", '-', args);
        }
        else
        {
            return CORBA::string_dup(" wrong command");
        }

       char * result = server->do_command(commandBuffer, success, targetChanged, newTargetObj);

        success = true;

        return CORBA::string_dup(result);
    }
		

}
