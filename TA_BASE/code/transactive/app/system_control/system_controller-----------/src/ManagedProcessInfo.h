/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/ManagedProcessInfo.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * ManagedProcessInfo maintains information about a
  * managed process (meaning a process that has control
  * and monitor modes, can be polled, etc). It inherits
  * from ProcessInfo and uses the IManagedProcessCorbaDef
  * and IManagedAgentCorbaDef interfaces to communicate
  * with the process.
  *
  */

#if !defined(ManagedProcessInfo_A1327B22_239D_4fbe_BEB3_CB98565075C9__INCLUDED_)
#define ManagedProcessInfo_A1327B22_239D_4fbe_BEB3_CB98565075C9__INCLUDED_

#include "ProcessInfo.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/process_management/IDL/src/IManagedProcessCorbaDef.h"
#include "core/process_management/IDL/src/IManagedApplicationCorbaDef.h"
#include "core/process_management/IDL/src/IManagedAgentCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/utilities/src/RunParams.h"

#include <string>
#include <vector>
#include <set>

namespace TA_Base_Core
{
    class IProcess; // Forward declaration
}

namespace TA_Base_App
{

    class ManagedProcessInfo : public ProcessInfo
    {

    public:

		enum EPeerState			// The states of the peer agents.
		{
			NotInControl,		// No peer agent is running in Control.
			StartingInControl,	// There is a peer agent starting in Control.
			RunningInControl,	// There is a peer agent running in Control.
            StartingInMonitor,	// There is a peer agent starting in Monitor.
            RunningInMonitor,	// There is a peer agent running in Monitor.
            StartingInStandby,	// There is a peer agent starting in Standby.
            RunningInStandby	// There is a peer agent running in Standby.
		};


        /** 
          * Constructor
          *
          * Constructs a new ManagedProcessInfo object
          *
          * @param entityName            The name of the monitored process entity.
          * @param childEntityName       The name of the monitored process's child entity.
          * @param processData           A pointer to an IProcess object containing
          *                              data about the managed process.
          * @param peerHosts             The System Controller(s) running the peer
          *                              for this process.
          * @param pollPeriod            The time, in milliseconds, between polls
          *                              to the process.
          * @param normalOperationMode   The normal operation mode of this process.
          * @param notifyPort            The notification port to use if this
          *                              process is a service process.
          * @param responseTimeout       The time an agent has to respond to a state
          *                              change request
          *
          */
        ManagedProcessInfo(const std::string& entityName, 
                             const std::string& childEntityName,
                             TA_Base_Core::IProcess* processData, 
                             const std::vector<std::string>& peerHosts,
                             int pollPeriod,
                             const TA_Base_Core::EOperationMode& normalOperationMode,
                             int notifyPort,
                             unsigned int responseTimeout,
							 std::string binDir,
							 bool isStandbyProcess,
							 std::string monitorGroupName,
							 int startLag,
							 bool hasStandbyProcess);
        /**
          * Destructor
          */
        virtual ~ManagedProcessInfo();

        /**
          * NotifyGroupOffline
          *
          * Notifies this process that a group has gone offline.
          * 
          * @param group The name of the group that has gone offline.
          */
	    void notifyGroupOffline(const std::string& group);

        /**
          * NotifyGroupOnline
          *
          * Notifies this process that a group has come back online.
          * 
          * @param group The name of the group that is now online.
          */
        void notifyGroupOnline(const std::string& group);

        /** 
          * invalidateNotifyHosts
          *
          * Informs the process that there are notify hosts that have been
          * invalidated.
          *
          * @param invalidNotifyHosts  A list of invalidated notify hosts.
          */
        void invalidateNotifyHosts(const std::vector<std::string>& invalidNotifyHosts);

        /** 
          * getOperationMode
          *
          * Retreives the operation mode of this process.
          *
          * @return The operation mode.
          *
          */
        TA_Base_Core::EOperationMode getOperationMode();

        /** 
          * getNormalOperationMode
          *
          * Returns the process's normal (expected) operation mode.
          *
          * @return The process's normal operation mode.
          *
          */
        TA_Base_Core::EOperationMode getNormalOperationMode() const
        {
            return m_normalOperationMode;
        }

        /** 
          * getInitOperationMode
          *
          * Gets the operation mode that the process will first start in.
          *
          * @return The process's initial operation mode.
          */
        TA_Base_Core::EOperationMode getInitOperationMode();

        /**
          * startProcess
          *
          * Starts this process. This is called when the user chooses to start a process
          * from the System Manager.
          */
        virtual void startProcess();

        /** 
          * stopProcess
          *
          * Stops this process. This is called when the user chooses to stop a process
          * from the System Manager.
          *
          * @param reason An enum explaining the reason for stopping this process.       
          */
        virtual void stopProcess(TA_Base_Core::EManagedProcessTerminateCode reason);

        /**
          * terminateProcess
          *
          * Attempts to terminate the process nicely through the CORBA
          * interface.
          */
	    void terminateProcess();

        /**
         * statusChanged
         *
         * This is called by agents to indicate when they move between
         * states.
         *
         * @param EProcessStatus - The state the agent is now in
         */
        void statusChanged(TA_Base_Core::EProcessStatus status);

        /**
          * setParam
          *
          * Sets a runtime parameter on the process, such as the debug
          * level.
          *
          * @param param  The runparam to set.
          */
        void setParam(TA_Base_Core::RunParam param);

        /**
          * poll
          *
          * Polls the process through the CORBA interface.
		  *
		  * @exception CORBA::SystemException  Thrown if the poll fails.
          */
	    void poll();

        /** 
          * run
          *
          * The starting point for the thread. 
          *
          */
   		virtual void run();

        /** 
          * setManagedProcess
          *
          * Assigns the interface to the managed process so the System Controller
          * can communicate with it directly. Set when the managed process registers
          * with the System Controller.
          *
          * @param managedProcess  The IManagedProcessCorbaDef interface
          */
        void setManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr managedProcess);

        /** 
          * setManagedAgent
          *
          * Assigns the interface to the managed agent so the System Controller can
          * communicate with it directly. The agent registers by passing in a pointer
          * to a ManagedApplicationCorbaDef, which is narrowed to a ManagedAgentCorbaDef.
          *
          * @param managedApplication  The IManagedApplicationCorbaDef interface
          */
	    void setManagedAgent(TA_Base_Core::IManagedApplicationCorbaDef_ptr managedApplication);

        /**
         * setStatusToChangeTo
         *
         * This sets the new status the process should change to
         *
         * @param newStatus The status to switch the process into
         */
        void setStatusToChangeTo(TA_Base_Core::EProcessStatus newStatus);

        /** 
          * getProcessData
          *
          * Returns information about this process.
          *
          * @return A ProcessData struct that contains the information.
          *
          */
        virtual TA_Base_Core::ProcessData* getProcessData();

        /**
          * isAPeer
          *
          * Returns true if the System Controller on the given hostname is 
          * running a peer for this process.
          *
          * @return True if the System Controller is running a peer, and
          *         false otherwise.
          *
          * @param hostname  The hostname of the System Controller.
          *
          */
        virtual bool isAPeer(const std::string& hostname);

        /**
          * shouldAutoFailback
          *
          * Returns true if the managed process should failback automatically,
          * and false otherwise.
          *
          * @return True if the process should automatically failback.
          *
          */
        virtual bool shouldAutoFailback() const
        {
            return m_shouldAutoFailback;
        }

        /** 
          * isServiceProcess
          *
          * Returns true if this process is a service process (ie the notification
          * service agent).
          *
          * @return True if this is a service process, and false otherwise.
          *
          */
        virtual bool isServiceProcess() const
        {
            return m_isServiceProcess;
        }

        /**
          * getParams
          *
          * Returns the runtime parameters associated with this process.
          *
          * @return A vector of runparams.
          */
        virtual TA_Base_Core::RunParams::ParamVector getParams();

        /**
          * getPeerHosts
          *
          * Returns the peers for this process.
          *
          * @return A vector of hostnames.
          */
        virtual std::vector<std::string> getPeerHosts()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            return m_peerHosts;
        }

        /**
          * getDebugLevel
          *
          * Returns the debug level of this process.
          *
          * @return The debug level.
          */
        virtual std::string getDebugLevel()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            return m_debugLevel.c_str();
        }

        /**
          * getNotifyPort
          *
          * Returns the notify port for this process, if this process is
          * a service process.
          *
          * @return The notification port.
          */
        virtual int getNotifyPort() const
        {
            return m_notifyPort;
        }

		/**
		  * getLocationKey
		  *
		  * Returns the location key of this process.
		  *
		  * @return The process's location key.
		  */
		virtual unsigned long getLocationKey() const
		{
			return m_locationKey;
		}

        /**
		  * getLocationName
		  *
		  * Returns the location name of this process.
		  *
		  * @return The process's location name.
		  */
        virtual std::string getLocationName() const
		{
            return m_locationName.c_str();
		}

		/** 
		  * getShutdownTime
		  *
		  * Returns the total shutdown time required for this process.
		  */
		virtual int getShutdownTime() const
		{
			// 
			// return m_terminateTimeout + getKillTimeout() + SIGKILL_TIMEOUT;
			// just return m_terminateTimeout. this is consistent with system manager status.
			// otherwise, system manager already shows it's stopped but agent isnt killed yet.

			return m_terminateTimeout;
		}

        /** 
          * setPeerState
          *
          * Instructs this process of the state of a peer agent.
          *
          * @param hostName  The hostname the peer is running on.
          * @param state	 The state of the peer.
          *
          */
		virtual void setPeerState(std::string hostName, EPeerState state);

        /** 
          * setPeerRunning
          *
          * Instructs this object that a peer for this process is now running.
          *
          * @param peerRunning  The hostname of the peer that is running.
          */
        virtual void setPeerRunning(std::string peerRunning);

        /** 
          * setPeerNotRunning
          *
          * Instructs this object that a peer for this process is no longer 
          * running.
          *
          * @param peerNotRunning  The hostname of the peer that is not running.
          */
        virtual void setPeerNotRunning(std::string peerNotRunning);

        /** 
          * isPeerRunning
          *
          * Returns true if there is a peer for this agent running.
          *
          * @return True if a peer is running, and false otherwise.
          */
        virtual bool isPeerRunning() const
        {
            return (!m_runningPeerHosts.empty());
        }
        virtual bool isStandbyProcess() const
        {
            return m_isStandbyProcess;
        }

        /**
          * processTerminating
          *
          * Called to update the process's status due to the process terminating.
          * This is called by ProcessManager when the process informs the System
          * Controller that it is terminating. It may also be called when the
          * System Controller is shutting down.
          *
          * @param reason An enum explaining the reason why this process is terminating.       
          */
        virtual void processTerminating(TA_Base_Core::EManagedProcessTerminateCode reason);

        /**
          * isRunning
          *
          * There are many states a process can be in and be classed as running. This
          * checks the processes current state to see if it is in a running state.
          *
          * @param bool Returns true if the process is running
          */
        bool isRunning();

		// gongzong++ CL-21328
//         /**
//           * raiseFailoverAlarm
//           *
//           * This will raise an alarm indicating that the process has failed 
// 		  * over.
//           */
//         virtual void raiseFailoverAlarm();
// 
//         /**
//           * closeFailoverAlarm
//           *
//           * Closes the FailoverOccurred alarm, if one is open.
//           */
//         virtual void closeFailoverAlarm();


    protected:

        /**
          * waitForStartLag
          *
          * Waits for the start lag to expire, unless the process is told to
          * terminate mid-way through.
          *
          * @return True if the start lag has finished, and false if the process
          *         is required to terminate before it is complete.
          */
        virtual bool waitForStartLag();

		/**
		  * buildCommandLine
		  *
		  * Creates the process's command line arguments that are used to start
		  * the process.
		  *
		  * @param startPath  Populated with the process's start path.
		  *
		  * @return A vector of strings containing the command line arguments.
		  */
		virtual std::vector<std::string> buildCommandLine(std::string& startPath);

        /**
          * performStart
          *
          * Starts the process.
          */
        virtual void performStart();

        /**
          * performPoll
          *
          * Polls the process.
          */
        virtual void performPoll();

        /**
          * performStop
          *
          * Stops the process. Firstly tries to stop it using the CORBA
          * terminate() call, then it sends a SIGTERM, and then if that
          * didn't work it sends a SIGKILL.
          */
        virtual void performStop(TA_Base_Core::EProcessStatus statusToChangeTo);

        /**
          * performChangeToControl
          *
          * This will tell the process to change into control mode. It
          * ensures that the agent responds and then changes to the required
          * mode within a time limit.
          */
        virtual void performChangeToControl();

        /**
          * performChangeToMonitor
          *
          * This will tell the process to change into monitor mode. It
          * ensures that the agent responds and then changes to the required
          * mode within a time limit.
          */
        virtual void performChangeToMonitor();


    private:
        ManagedProcessInfo( const ManagedProcessInfo& theManagedProcessInfo);
   		ManagedProcessInfo& operator=(const ManagedProcessInfo&);


        /**
         * changeState
         *
         * If the status the process should be changing to does not match the
         * current status of the process then this method is called. It will
         * look at the status and call the correct method to change the status
         * to the requested one.
         *
         * @param currentStatus - The current status of the process
         * @param newStatus - The status the process needs to be changed to be
         *                    running in.
         */
        void changeState(TA_Base_Core::EProcessStatus currentStatus,
                         TA_Base_Core::EProcessStatus newStatus);


        /**
         * getStatusToChangeTo
         *
         * This returns the status the process should be changing to
         */
        TA_Base_Core::EProcessStatus getStatusToChangeTo()
        {
            TA_Base_Core::ThreadGuard guard(m_changeStateLock);
            return m_statusToChangeTo;
        };

        /** 
          * getDate
          *
          * Returns the date as a string. Used to create the logfile name
          * for this process.
          *
          * @return The date in the form 'dd.mm.yy'.
          *
          */
        std::string getDate() const;

		/**
		  * modeChangeComplete
		  *
		  * Indicates to this object that the process has finished changing operation
		  * modes (ie it's now in RunningControl or RunningMonitor).
		  */
		void modeChangeComplete()
		{
            TA_Base_Core::ThreadGuard guard(m_lock);
			m_isModeChangeComplete = true;
		}

		/**
		  * isModeChangeComplete
		  *
		  * Returns true if the agent has finished changing operation modes,
		  * and false otherwise.
		  */
		bool isModeChangeComplete()
		{
			TA_Base_Core::ThreadGuard guard(m_lock);
			return m_isModeChangeComplete;
		}
		void performChangeToStandby();
		void notifyStandbyProcess();

        void resetProcess();
		std::map<std::string, TA_Base_Core::ISystemControllerAdminCorbaDef_var> ManagedProcessInfo::resolveStandbySystemControllerAdmin();
        // The default timeouts in milliseconds
        static const unsigned int DEFAULT_STARTUP_TIMEOUT;
        static const unsigned int DEFAULT_TERMINATE_TIMEOUT;
        static const unsigned int DEFAULT_CHANGE_CONTROL_TIMEOUT;
        static const unsigned int DEFAULT_CHANGE_MONITOR_TIMEOUT;

		// The timeout used to wait for SIGKILLS.
		static const unsigned int SIGKILL_TIMEOUT;

        /**
          * The hostname(s) of the System Controller(s) that are configured to 
          * run a peer for this agent.
          */
        std::vector<std::string> m_peerHosts;

        /**
          * The hostname(s) of the System Controller(s) that are currently
          * running a peer for this agent. This is needed to see if a Change
          * Agent Operation Mode request should be allowed.
          */
        std::set<std::string> m_runningPeerHosts;

		/**
		  * The overall state of the peer(s) of this process.
		  */
		EPeerState m_peerState;

        /**
          * The name of the peer System Controller running the agent in Control
          * mode (if m_peerState is not set to NotControl). This is needed to 
          * correctly update m_peerState when we receive a process update 
		  * from a peer.
          */
        std::string m_peerInControl;        

        /**
          * A pointer to an IManagedProcessCorbaDef interface, used to
          * terminate and poll the process.
          */
	    TA_Base_Core::IManagedProcessCorbaDef_ptr m_managedProcess;

        /**
          * A pointer to an IManagedAgentCorbaDef interface, used to set
          * and get the operation mode, and to notify the process of the
          * status of groups.
          */
	    TA_Base_Core::IManagedAgentCorbaDef_ptr m_managedAgent;

  	    /**
          * The "normal" operation mode of this process (ie the operation
          * mode read from the database).
          */                    
        TA_Base_Core::EOperationMode m_normalOperationMode;

		// gongzong++ CL-21328
//         /**
//           * The ID of the FailoverOccurred alarm. Used to close the
//           * alarm when the process is failbacked (either automatically
// 		  * or manually).
//           */
//         std::string m_failoverAlarmId;

        /**
          * True if this process should failback automatically after a
          * failover, and false otherwise.
          */
	    bool m_shouldAutoFailback;

		/**
		  * The process's scheduling priority (nice value).
		  */
		int m_processPriority;

		/**
		  * The processor that this process is bound to, or NOT_BOUND
		  * if the binding should be left up to the OS.
		  */
		int m_boundProcessor;

        /**
          * The time, in milliseconds, to wait after a terminate() call
          * before assuming it has failed and trying a SIGTERM.
          */
        unsigned int m_terminateTimeout;

        /**
          * The time, in milliseconds, to wait after launching a process
          * before assuming it has failed and killing it.
          */
        unsigned int m_startupTimeout;

        /**
          * The time, in milliseconds, to wait after asking a process to
          * change into control before assuming it has failed and killing it.
          */
        unsigned int m_changeControlTimeout;

        /**
          * The time, in milliseconds, to wait after asking a process to
          * change into monitor before assuming it has failed and killing it.
          */
        unsigned int m_changeMonitorTimeout;

        /**
          * True if this process is a service process, and false otherwise.
          */
        bool m_isServiceProcess;

        /**
          * If this is a service process, then this member variable holds
          * the notify port that the process is started with.
          */
        unsigned short m_notifyPort;

        /**
          * This is the time the agent has to respond with a state change
          */
        unsigned int m_responseTimeout;

        /**
          * The debug level of this process.
          */
        std::string m_debugLevel;

        /**
          * This is the status to change the process into and the thread
          * lock for it
          */
        TA_Base_Core::EProcessStatus m_statusToChangeTo;
        TA_Base_Core::NonReEntrantThreadLockable m_changeStateLock;

        /**
          * The initial operation mode when the app started
          */
        TA_Base_Core::EOperationMode m_initialOperationMode;

        /**
          * The name of the entity that is the child of the MonitoredProcess 
          * entity. This is used to set the agent's debug file name, and is 
          * required for the --entity-name command line parameter.
          */
        std::string m_childEntityName;

		/**
		  * The location key of this process.
		  */
		unsigned long m_locationKey;
        std::string m_locationName;

		/**
		  * True if the process has just finished changing operation modes,
		  * and false otherwise.
		  */
		bool m_isModeChangeComplete;

        TA_Base_Core::NonReEntrantThreadLockable m_lock;

		/**
          * True if this process is a standby process, and false otherwise.
          */
        bool m_isStandbyProcess;
		
		/**
          * the group name this managed Process pertains to
          */
		std::string m_monitorGroupName;
		
		/**
          * indicate if the group has been in groupoffline state
          */
		bool m_isGroupOffline;

		bool m_hasStandbyProcess;

		bool m_isGroupStartup;

public:
		void GroupStartup(bool val) { m_isGroupStartup = val; }
    };

} // namespace TA_Base_App

#endif // !defined(ManagedProcessInfo_A1327B22_239D_4fbe_BEB3_CB98565075C9__INCLUDED_)
