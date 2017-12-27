/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This singleton class implements the IHeartbeatReceiverCorbaDef
  * interface to allow the System Controller to receive a heartbeat
  * from other System Controllers. When a heartbeat is received, it
  * is passed to the PeersMonitor and GroupMonitor classes to store
  * who the heartbeat came from. Additionally, this class sends a
  * heartbeat to all peers, as well as the System Controllers that 
  * are observing the group it is in (if it is in fact in any group).
  *
  */

#if !defined(SystemControllerMonitor_A488D48B_B412_48e9_8926_FCFDD12DCDBE__INCLUDED_)
#define SystemControllerMonitor_A488D48B_B412_48e9_8926_FCFDD12DCDBE__INCLUDED_

#include "core/process_management/IDL/src/IHeartbeatReceiverCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"

#include "PeersMonitor.h"
#include "GroupMonitor.h"
#include "PeerSCInfo.h"

#include <string>
#include <vector>
#include <ctime>


namespace TA_Base_Core
{
    class ServerEntityData; // Forward declaration
}


namespace TA_Base_App
{
    class SystemControllerMonitor : public virtual POA_TA_Base_Core::IHeartbeatReceiverCorbaDef,
                                    public virtual TA_Base_Core::Thread,
                                    public virtual TA_Base_Core::ServantBase
    {

    public:

        /**
          * getInstance
          *
          * @return The one-and-only instance of this class.
          */
        static SystemControllerMonitor& getInstance();

        /**
          * Destructor
          */
        virtual ~SystemControllerMonitor();

        /** 
          * receiveHeartbeat
          *
          * Called by other System Controllers to send a heartbeat to us. 
          * The heartbeat is passed to the PeersMonitor and GroupMonitor
          * classes which determine where the heartbeat came from a peer
          * or group member.
          *
          * @param host The hostname of the System Controller that sent
          *             the heartbeat.
          */
	    virtual void receiveHeartbeat(const char* host);

        /** 
          * run
          *
          * The starting point for the thread. Periodically sends 
          * heartbeats to all peers and System Controllers monitoring 
          * the group we are in (if any).
          *
          */
   		virtual void run();

        /** 
          * terminate
          *
          * Terminates the thread.
          *
          */
        virtual void terminate();

        /** 
          * startHeartBeating
          *
          * Called to start the sending of heartbeats to our peer 
          * System Controllers, and the System Controllers monitoring the 
          * group we are in (if in fact we are in a group).
          *
          */
        void startHeartBeating();

        /**
          * stopHeartBeating
          *
          * Stops the heartbeats being sent to other System Controllers.
          * 
          */
        void stopHeartBeating();

        /**
          * cleanUp
          *
          * Cleans up the SystemControllerMonitor.
          */
        void cleanUp();

		/**
          * setGroupStarting
          *
          * set Group Starting.
          */
		void setGroupStarting( const char* hostName );
    private:

         // Made private as this is a singleton
        SystemControllerMonitor();
	    SystemControllerMonitor( const SystemControllerMonitor& SystemControllerMonitor);
	    SystemControllerMonitor& operator=(const SystemControllerMonitor& scm);

        /**
          * checkForFailedPeers
          *
          * Checks for failed peers by calling getFailedPeers() in the
          * PeersMonitor class and comparing the vector returned to the
          * vector of previously failed peers. It also checks for previously
          * failed peers that have now recovered.
          *
          */
        void checkForFailedPeers();

        /**
          * checkForFailedGroups
          *
          * Checks for failed groups by calling hasGroupFailed() in the
          * GroupMonitor class on each group. It also checks if any previously
          * failed groups have now come back online.
          *
          */
        void checkForFailedGroups();

		/**
          * checkForGroupProcesses
          *
          * Checks the state of the groups processes and starts 
		  * the heartbeat if they are all running.
          * 
          */
        void checkForGroupProcesses();

        /** 
          * raisePeerFailedAlarm
          *
          * Raises an alarm due to a peer failing.
          *
          * @param hostname  The hostname of the peer that has failed.
          */
        void raisePeerFailedAlarm(const std::string& hostname);

        /** 
          * raiseGroupFailedAlarm
          *
          * Raises an alarm due to a group failing.
          *
          * @param groupName  The name of the group that has failed.
          */
        void raiseGroupFailedAlarm(const std::string& groupName);

        /** 
          * closePeerFailedAlarm
          *
          * Closes an alarm due to a peer recovering.
          *
          * @param hostname  The name of the peer that is now online.
          */
        void closePeerFailedAlarm(const std::string& hostname);

        /** 
          * closeGroupFailedAlarm
          *
          * Closes an alarm due to a group recovering.
          *
          * @param groupName  The name of the group that is now online.
          */
        void closeGroupFailedAlarm(const std::string& groupName);

        /** 
          * The one and only instance.
          */
        static SystemControllerMonitor* s_instance;

        /**
          * The PeersMonitor object that will ensure all heartbeats are
          * received from peers.
          */
        PeersMonitor* m_peersMonitor;

        /**
          * A vector of GroupMonitor objects, each of which ensure that
          * a group is still online.
          */
	    std::vector<GroupMonitor*> m_groupMonitors;

        /**
          * This holds the names of the hosts that are monitoring this group
          */
        std::vector<std::string> m_monitorsOfMyGroup;

        /**
          * A vector of PeerSCInfo objects that this class will send a
          * heartbeat to every time frame.  This is for Peer objects
		  * and heartbeats will be started asap.
          */
	    std::vector<PeerSCInfo*> m_peerSCsToPulsate;

		/**
          * A vector of PeerSCInfo objects that this class will send a
          * heartbeat to every time frame.  This is for Group objects
		  * and heartbeats will be started once all of the processes are
		  * either in RunningControl or RunningMonitor mode.
          */
	    std::vector<PeerSCInfo*> m_groupSCsToPulsate;

        /**
          * A map of hostnames to entity data. Contains the entity data for
          * each peer of this System Controller.
          */
        std::map<std::string, TA_Base_Core::ServerEntityData*> m_peerEntityData;

        /**
          * The hostname that this System Controller is running on.
          */
        std::string m_myHostname;

        /**
          * The interval, in milliseconds, between heartbeats.
          */
        unsigned long m_heartbeatSendPeriod;
        unsigned long m_heartbeatSendNumber;

        /**
          * True if the thread is running, and false otherwise.
          */
        bool volatile m_isRunning;

        /**
          * The timestamps are expected to be greater than
          * this time.
          */
        // time_t m_expectedHeartbeatTime;

        /**
          * Wait for next poll
          */
        TA_Base_Core::Condition m_timedWaitForNextPoll;

        /**
          * The peers that are currently marked as failed.
          */
        std::vector<std::string> m_failedPeers;

        /**
          * A map of hostnames to alarm IDs. If the peer has
          * not failed then the alarm ID is "".
          */
        std::map<std::string, std::string> m_failedPeerAlarmIds;

        /**
          * Indicates whether each group has been marked as failed
          * or not.
          */
        std::vector<bool> m_hasGroupFailed;

        /**
          * A map of group names to alarm IDs. If the group has
          * not failed then the alarm ID is "".
          */
        std::map<std::string, std::string> m_failedGroupAlarmIds;

		/**
		  * A flag to indicate that all group processes have started.
		  */
		bool m_groupProcessesAllStarted;

		static TA_Base_Core::NonReEntrantThreadLockable m_lock;

        static const unsigned long DEFAULT_POLL_CHECK_INTERVALMS;
    };

} // namespace TA_Base_App

#endif // !defined(SystemControllerMonitor_A488D48B_B412_48e9_8926_FCFDD12DCDBE__INCLUDED_)
