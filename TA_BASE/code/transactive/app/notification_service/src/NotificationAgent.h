#ifndef NOTIFICATION_AGENT_H
#define NOTIFICATION_AGENT_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NotificationAgent.h $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/12/17 15:24:44 $
 * Last modified by:  $Author: shurui.liu $
 * 
 * Agent wrapper for CORBA Notification Service
 *
 */

#include "COS/CosNotifyChannelAdmin.hh"

#include "app/notification_service/src/FederatableEventChannel.h"
#include "app/notification_service/src/NSALocator.h"

#include "bus/generic_agent/src/IGenericAgentUser.h"

#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <map>
#include <string>
#include <utility>
#include <vector>


namespace TA_Base_Bus
{
    class IEntity;
    class GenericAgent;
}

namespace TA_Base_App
{
    class FederationForwarderMgr;
    class Federator;
    class NotificationServiceThread;
    class NotificationStatisticsThread;

    // The notification agent hosts one or more corba event channels.  Clients
    // that want to publish events or subscribe for events on a particular channel
    // use IChannelLocator::getChannels() to obtain a reference to the channel.

    // Since the reference is a corba object reference, the client doesn't have
    // to be on the same host as the notification agent, although it may not be
    // efficient if it is not.

    // FEDERATION allows consumers that attach to event channels hosted by one notification
    // agent (the 'consumer-side') to receive events that are pushed by a supplier attached
    // to an event channel hosted by a different notification agent (the 'supplier-side').
    // This can be used to minimise network traffic - instead of connecting to a remote
    // notification service, a consumer can attach to a local one and establish a federated
    // connection between the notification services.

    // The steps and classes involved in establishing federation are broadly as follows:
    // 1. The consumer initiates federation by calling IChannelLocator::federateIfRemoteSource().
    // 2. In our implementation of federateIfRemoteSource(), a Federator instance will be
    //    created (or ref'ed).  The Federator will add the supplierServiceAddr to the
    //    ChannelLocatorConnectionMgr for the given (channelName, supplierLocationKey).
    // 3. The Federator will call IChannelLocator::federateWithRemoteIChannelLocator()
    //    on the supplier side NS when the IChannelLocator with the given supplierServiceAddr
    //    becomes available (Federator::onChannelAvailable() will be called back).
    // 4. In our implementation of federateWithRemoteIChannelLocator(), the consumer NS will be
    //    added to the set of connections being monitored, and a FederationForwarderMgr instance
    //    will create (or ref) a FederationForwarder.
    // 5. The FederationForwarder will establish a connection between the supplier side
    //    EventChannel and ALL consumer side EventChannels at the specified location.  NOTE that
    //    subsequent calls to federateWithRemoteIChannelLocator() with the same locationKey
    //    add the consumer's serviceAddr to the set of connections being monitored without
    //    having to create an additional FederationForwarder.

    // NOTE that our implementation of federateWithRemoteIChannelLocator() will actually
    // establish federation with ALL consumer side EventChannels at the specified location
    // (if their serviceAddrs have already been added to the ChannelLocationConnectionMgr),
    // NOT JUST the consumerServiceAddr specified in the call.

    class NotificationAgent : public TA_Base_Bus::IGenericAgentUser,
                              public POA_TA_Base_Core::IChannelLocator,
                              public TA_Base_Core::ServantBase
    {
    public:
        // Initialise CorbaUtil using the port number specified on the command line by
        // RPARAM_NOTIFYPORT, and create the GenericAgent.
        NotificationAgent( int argc, char* argv[] );

        // Terminate the NotificationService thread and deactivate the GenericAgent.
        virtual ~NotificationAgent();

		//TD18706
		virtual void checkOperationMode()
		{
			//do nothing
		}
        // Creates a list of arguments for the NotificationService executable, creates 
		// the NotificationServiceThread and starts it. Then resolves the EventChannelFactory
		// and creates event channels. Blocks on GenericAgent::run().
		// The notification service will load the config file specified by OMNI_NOTIFY_CONFIG_FILE
		// or "./omni_notification.cfg" if none is specified.
		void startService();

        // Implementation of IGenericAgentUser
        // TODO: Does Monitor/Control apply to NotificationAgent? If so, some of these
        // methods need implementations.
        virtual void agentTerminate();
        virtual TA_Base_Bus::IEntity* createEntity( TA_Base_Core::IEntityDataPtr entityData );
        virtual void agentSetMonitor() {}
        virtual void agentSetControl() {}
        virtual void notifyGroupOffline( const std::string& group ) {}
        virtual void notifyGroupOnline( const std::string& group ) {}
        virtual void registerForStateUpdates() {}
        virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message) {}
         virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent) {}

        // Implementation of IChannelLocator
        char* getID();
        TA_Base_Core::ChannelMappingSeq* getChannels();
        void federateIfRemoteSource(CORBA::ULong supplierLocationKey, const char* channelName);
        void unfederateIfRemoteSource(CORBA::ULong supplierLocationKey, const char* channelName);
        void federateWithRemoteIChannelLocator(const char* consumerServiceAddr, CORBA::ULong consumerLocationKey, const char* channelName);
        void unfederateWithRemoteIChannelLocator(CORBA::ULong consumerLocationKey, const char* channelName);

        // Implementation of ISCSInteractive
        char* processCommand(const char* cmd, const char* args, CORBA::Boolean& success, 
             CORBA::Boolean& targetChanged, TA_Base_Core::ISCSInteractive_out nextTarget);

		// Data structure describing the event channels managed by this Notification Service
        // (needs to be public so that NotificationStatisticsThread can access it).
        // Note that the channel name is not used by the CNCA::EventChannel.
		struct ChannelDetails
		{
			TA_Base_Core::IFederatableEventChannel_ptr eventChannel;
			unsigned long entityKey;
        };

        // channelName -> ChannelDetails
        typedef std::map<std::string, ChannelDetails> ChannelNameToChannelDetailsMap;

    private:
        // EVENT CHANNEL MANAGEMENT

        // Attempts to resolve the CosNotifyChannelAdmin::EventChannelFactory.  Regular attempts are
        // made to resolve the factory before timing out and throwing a TransActiveException.
        // The timeout value, RESOLVE_CHANNELFACTORY_TIMEOUT, is defined in NotificationServiceContants.h
        void resolveEventChannelFactory();
        CosNotifyChannelAdmin::EventChannelFactory_var m_eventChannelFactory;

        // Creates event channels corresponding to child entities of this agent and stores
        // the object references in m_channels (createEntity() will already have been called
        // once for each child entity by the agent framework, and the channel names stored
        // in m_channels).  resolveEventChannelFactory() must be called first.
        void createEventChannels();
        ChannelNameToChannelDetailsMap    m_channels;
        TA_Base_Core::NonReEntrantThreadLockable m_channelsLock;  // Guard m_channels


        // SUPPLIER -> CONSUMER FEDERATION
        FederationForwarderMgr* m_fedMgr;


        // CONSUMER -> SUPPLIER FEDERATION
        // channelName           - the channel being federated
        // supplierServiceAddr   - the service address of the remote notification service that is to
        //                         be federated with.
        // supplierLocationKey   - the location key of the remote notification service that is to
        //                         be federated with.
        void addFederator( const std::string& channelName,
                           const std::string& supplierServiceAddr,
                           unsigned long supplierLocationKey );

        void removeFederator( const std::string& channelName,
                              const std::string& supplierServiceAddr,
                              unsigned long supplierLocationKey );

        // yanrong++: CL-21222, 2010-01-27: wait until connections initialized completely
        void waitInitialised();
        // ++yanrong

        // TODO LPP: Why is the supplierLocationKey needed as part of the key?  Wouldn't all
        // instances of the given supplierServiceAddr have the same location key?
        struct FederatorKey
        {
        public:
            FederatorKey( const std::string& channel, unsigned long supplierLocationKey, const std::string& supplierServiceAddr );

            // operator< is required by std::map<FederatorKey, ...>
            // true is returned if this FederatorKey is less than 'other'.
            bool operator<(const FederatorKey& other) const;

        private:
            std::string m_channel;
            unsigned long m_supplierLocationKey;
            std::string m_supplierServiceAddr;
        };

        struct FederatorDetail
        {
            explicit FederatorDetail(Federator* federator) : m_federator(federator), m_refCount(1) {}

            void ref() {++m_refCount; TA_Assert(m_refCount > 0);}
            void unref() {TA_Assert(m_refCount > 0); --m_refCount;}

            Federator* m_federator;
            int m_refCount;
        };

        // (channelName, supplierLocationKey, supplierServiceAddr) -> (Federator*, ref count)
        typedef std::map<FederatorKey, FederatorDetail> Federators;
        Federators m_federators;
        TA_Base_Core::NonReEntrantThreadLockable m_federatorsLock;  // Guard m_federators

        NSALocator* m_nsaLocator;

        // Local notify port that this notification service is listening on.
        // Initialized from the RPARAM_NOTIFYPORT runparam.
        unsigned short m_localNotifyPort;


        TA_Base_Bus::GenericAgent*    m_agent;

        // Set to true when the service has finished initializing
        // yanrong++: CL-21222, 2010-01-27
        //bool m_initialised;
        volatile bool m_initialised;

        // Guard m_fedMgr
        TA_Base_Core::ReEntrantThreadLockable m_fedMgrLock;
        // ++yanrong

        // Thread that executes the omniNotify notification service
        NotificationServiceThread*    m_serviceThread;

        NotificationStatisticsThread*   m_statisticsThread;

        // UUID returned by IChannelLocator::getID()
        std::string m_agentID;
    };


    // Returns the hostname of the local machine
    // TODO LPP: How many versions of this function do we need?  eg. see Hostname.cpp
    // in core.utilities.  Need to determine whether the implementations are
    // the same and cull duplicates.
    std::string gGetHostname();

}
#endif // NOTIFICATION_AGENT_H
