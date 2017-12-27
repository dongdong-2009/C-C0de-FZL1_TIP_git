/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/ProcessInfo.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class contains information about a single (managed or
  * unmanaged) process that is local to this System Controller. 
  * It allows processes to be started and stopped, and contains
  * information on the time it was started, number of restarts,
  * etc.
  *
  */

#if !defined(ProcessInfo_4F8EC387_F2C1_4cee_8A18_6BC112914E97__INCLUDED_)
#define ProcessInfo_4F8EC387_F2C1_4cee_8A18_6BC112914E97__INCLUDED_

#include "core/threads/src/Thread.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/types/SystemAlarms_MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include <string>

namespace TA_Base_Core
{
    class IProcess; // Forward declaration
}

namespace TA_Base_App
{
    class ProcessInfo : public TA_Base_Core::Thread
    {

    public:

        /** 
          * Constructor
          *
          * Constructs a ProcessInfo object.
          *
          * @param entityName   The name of the entity.
          * @param processData  A pointer to an IProcess object containing
          *                     data about the process.
          * @param pollPeriod   The time, in milliseconds, between polls
          *                     to the process.
          * @param binDir       The directory that the address is relative from
          */
        ProcessInfo(const std::string& entityName, TA_Base_Core::IProcess* processData,
                    int pollPeriod, std::string binDir);

        /**
          * Destructor
          */
        virtual ~ProcessInfo();

        /**
          * startProcess
          *
          * Starts this process.
          */
        virtual void startProcess();

        /** 
          * stopProcess
          *
          * Stops this process.
          *
          * @param reason An enum explaining the reason for stopping this process.       
          */
        virtual void stopProcess(TA_Base_Core::EManagedProcessTerminateCode reason);

        /**
          * handleProcessFailure
          *
          * Called when it's discovered that this process has failed.
          * Raises an alarm and updates the process status so that the process
          * will be restarted.
          *
          * @param reason An enum explaining the reason the process died.
          *               
          */
        virtual void handleProcessFailure(TA_Base_Core::EManagedProcessTerminateCode reason);

		/**
          * getProcessId
          *
          * Returns the Process ID of this process.
          *
          * @return The PID.
          */
        TA_Base_Core::ProcessId getProcessId()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            return m_processId;
        }

        /**
          * getProcessStatus
          *
          * Returns this process's status.
          *
          * @return EProcessStatus   The process's status.
          */
        TA_Base_Core::EProcessStatus getProcessStatus()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            return m_processStatus;
        }

        /**
         * setProcessStatus
         * 
         * This will set the current process status. This method should be
         * called to ensure thread safety
         *
         * @param EPRocessStatus The process's new state
         */
        void setProcessStatus(TA_Base_Core::EProcessStatus processStatus);

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
          * run
          *
          * The starting point for the thread. 
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
          * getEntityName
          *
          * Returns the entity name of this process.
          *
          * @return The process's entity name.
          */
        virtual std::string getEntityName() const
        {
            return m_entityName;
        }

        /**
          * getEntityDescription
          *
          * Returns the entity description for this process.
          *
          * @return The process's entity description.
          */
        virtual std::string getEntityDescription() const
        {
            return m_entityDescription;
        }

        /**
          * getNumRestarts
          *
          * Returns the number of times this process has been
          * restarted.
          *
          * @return The number of restarts.
          */
        virtual unsigned int getNumRestarts()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            return m_numRestarts;
        }

        /**
          * getTerminateStatus
          *
          * Retrieves the terminate status for the last time the
          * process was terminated.
          *
          * @return The terminate code.
          */
        virtual TA_Base_Core::EManagedProcessTerminateCode getTerminateStatus()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            return m_terminateStatus;
        }

        /** 
          * raiseAlarm
          *
          * Raises an alarm.
          *
          * @param messageType  The type of alarm to raise.
          * @param descParam    The description parameters associated with the
          *                     alarm.
		  *
          * @return The ID of the alarm just raised.
          */
        virtual std::string raiseAlarm(const TA_Base_Core::MessageType& messageType,
                               const TA_Base_Core::DescriptionParameters& descParam);

        /**
          * raiseFailureAlarm
          *
          * Raises an alarm indicating that the process has failed, if a 
		  * ProcessFailure alarm is not already open for this process.
          *
          * @param reason An enum explaining the reason the process died.
          */
        virtual void raiseFailureAlarm(TA_Base_Core::EManagedProcessTerminateCode reason);

        /**
          * closeFailureAlarm
          *
          * Closes the ProcessFailure alarm, if one is open.
          */
        virtual void closeFailureAlarm();

        /** 
          * signalReceived
          *
          * Indicates to this object a signal was received for the process 
          * it is managing.
          *
          */
        virtual void signalReceived()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            m_wasSignalled = true;
        }

        /** 
          * hasBeenSignalled
          *
          * Returns true if a signal was received for the process.
          *
          * @return True if a signal was received, and false otherwise.
          */
        virtual bool hasBeenSignalled()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            return m_wasSignalled;
        }

        /**
          * killProcess
          *
          * Sends a SIGKILL to the process. Used to shutdown a process when
          * it refuses to die.
          */
        virtual void killProcess();

		/** 
		  * getShutdownTime
		  *
		  * Returns the total shutdown time required for this process.
		  */
		virtual int getShutdownTime() const
		{
			return m_killTimeout;
		}

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
          * resetProcess
          *
          * Resets the process's member variables when the process
		  * is restarted.
          */
		virtual void resetProcess();

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
          * Stops the process. Firstly sends a SIGTERM, and then if that
          * didn't work it sends a SIGKILL.
          */
        virtual void performStop();

		/**
          * setProcessId
          *
          * Sets the Process ID of this process.
		  *
		  * @param pid  The Process ID
          */
        void setProcessId(TA_Base_Core::ProcessId pid)
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            m_processId = pid;
        }

        /**
          * setTerminateStatus
          *
          * Sets the terminate status (the reason why the process was last
          * terminated).
		  *
		  * @param terminateStatus  The terminate status.
          */
        virtual void setTerminateStatus(TA_Base_Core::EManagedProcessTerminateCode terminateStatus)
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            m_terminateStatus = terminateStatus;
        }

		/**
		  * updateStartTime
		  *
		  * Updates the process's start time to the current time.
		  */
		void updateStartTime();

        /**
          * getAddress
          *
          * Returns the address of this process.
          */
        virtual std::string getAddress() const
        {
            return m_address;
        }

        /**
          * getUserName
          *
          * Returns the username used to run this process.
          */
        virtual std::string getUserName() const
        {
            return m_userName;
        }

		/**
          * getUserName
          *
          * Returns the username used to run this process.
          */
        virtual std::string getPassWord() const
        {
            return m_passWord;
        }

        /**
          * getKillTimeout
          *
          * Returns the kill timeout of this process.
          */
        virtual int getKillTimeout() const
        {
            return m_killTimeout;
        }

        /**
          * getStartLag
          *
          * Returns the start lag of this process.
          */
        virtual int getStartLag() const
        {
            return m_startLag;
        }

        /**
          * getTimeStarted
          *
          * Returns the time this process was started.
          */
        virtual TA_Base_Core::DateTime_ul getTimeStarted()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            return m_timeStarted;
        }

        /**
          * getPollPeriod
          *
          * Returns the poll period in milliseconds.
          */
        virtual int getPollPeriod() const
        {
            return m_pollPeriod;
        }

        /** 
          * incNumRestarts
          *
          * Increments the number of restarts count for this
          * process.
          *
          */
        virtual void incNumRestarts()
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            ++m_numRestarts;
        }

		
	protected:

		/**
	   	  * True if the thread is running, and false otherwise.
		  */
		bool volatile m_isRunning;
			
		/**
          * The start lag for this process in milliseconds.
          */
        int m_startLag;
			
	private:
	
	    ProcessInfo( const ProcessInfo& theProcessInfo);
   		ProcessInfo& operator=(const ProcessInfo&);

        /**
          * The process's PID.
          */
	    TA_Base_Core::ProcessId m_processId;

        /**
          * The time at which the process was started.
          */
  	    TA_Base_Core::DateTime_ul m_timeStarted;

        /**
          * The reason why the process was last terminated.
          */
        TA_Base_Core::EManagedProcessTerminateCode m_terminateStatus;

        /**
          * True if a SIGCHLD has been received for this process.
          */
        bool m_wasSignalled;

        TA_Base_Core::NonReEntrantThreadLockable m_lock;

        /**
          * The entity name of this process.
          */
	    std::string m_entityName;

        /**
          * The entity description for this process.
          */
	    std::string m_entityDescription;

        /**
          * The "address" (starting path) of this process.
          */
   	    std::string m_address;

        /**
          * The username to use to run this process.
          */
   	    std::string m_userName;

		/**
          * The password to use to run this process.
          */
   	    std::string m_passWord;

        /**
          * The ID of the ProcessFailure alarm. Used to close the
          * alarm when the process is restarted.
          */
        std::string m_failureAlarmId;

        /**
          * The time, in milliseconds, between polls to this process.
          */
        int m_pollPeriod;

        /**
          * The timeout to wait for, in milliseconds, after sending a
          * SIGTERM before sending a SIGKILL.
          */
        int m_killTimeout;

        /**
          * The number of times this process has been restarted.
          */
   	    unsigned int m_numRestarts;

        /**
          * The status of this process.
          */
        TA_Base_Core::EProcessStatus m_processStatus;    
    };

} // namespace TA_Base_App

#endif // !defined(ProcessInfo_4F8EC387_F2C1_4cee_8A18_6BC112914E97__INCLUDED_)
