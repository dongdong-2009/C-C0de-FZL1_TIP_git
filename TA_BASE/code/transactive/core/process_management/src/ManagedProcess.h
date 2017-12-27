/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/ManagedProcess.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0017-ManagedProcess_Usage_Guidelines.doc 
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last mofified by:  $Author: CM $
  *
  * Managed Process provides the point of contact between the
  * Process Manager and any process it is managing.
  */

#if !defined(MANAGEDPROCESS)
#define MANAGEDPROCESS

#include "core/process_management/IDL/src/IManagedProcessCorbaDef.h"
#include "core/process_management/src/MonitoringThread.h"
#include "core/corba/src/ServantBase.h"


namespace TA_Base_Core
{
    // Foward declarations
    class IManagedProcessException; 
    class UtilitiesException;
    class IManagedApplicationCorbaDef;
    class IManagedProcessUser;
    class PollListener;

    class ManagedProcess : public virtual POA_TA_Base_Core::IManagedProcessCorbaDef,
                               public virtual TA_Base_Core::ServantBase,
                               public virtual IMonitoringThreadUser
    {
    public:

        /**
          * Constructor
          * 
          * Registers with the Process Manager and fetches the applications
          * runtime parameters
          *
          * Note - In the event of ManagedProcess failing an initialisation step
          *        after it has registered with the appropriate process manager,
          *        it will make an internal call to notifyTerminating before
          *        raising an exception. So there is no need for you to call
          *        notifyTerminating if you fail to construct the ManagedProcess
          *
          * @param A handle to the managed process user that Managed Process will
          *        call back to if it loses communication with the Process Monitor
          * @param A reference to the object responsible for responding to polls,
          *        or NULL if the default PollListener is required.
          *
          * Note - If a PollListener object is provided, do NOT delete it - it will
          *        be deleted for you.
          *
          * @exception ManagedProcessException thrown if initialisation fails
          * @exception ProcessAlreadyRunningException thrown if the process was
          *            already running.
          */
        ManagedProcess(IManagedProcessUser* const user, PollListener* pollListener = NULL);

        /**
          * Virtual Destructor
          *
          * This should not be used. deactiveateAndDeleteServant() should be called
          * to destroy the managed process.
          */
        virtual ~ManagedProcess();

        /**
          * startMonitoringThread
          *
          * This method initialises and starts the MonitoringThread.
          * The reason this has been left out of the constructor is that once
          * the MonitoringThread is up and running, it may make call backs 
          * to the Managed Process User it was constructed with. This could be a
          * problem if Managed ProcessUser has not finished its own initialisation.
          *
          * @param An optional parameter to set the period (in seconds) at which
          *        the managed process polls the process monitor (defaults to 10)
          */
        void startMonitoringThread(unsigned int p_pollingPeriodInSeconds = 10);

        /**
          * terminate
          *
          * This method is called by the Process Manager if it wants
          * to shut us down. It is also called internally if the MonitoringThread
          * reports Process Manager Failure
          *
          * All this does is set the m_terminationFromManager flag. The Monitoring
          * Thread is responsible for actually calling cleanup. The reason that this
          * is now done this way is that the ManagedProcess could not deactivate
          * while the call to terminate (which is of course a CORBA call) had not
          * finished.
          */
        void terminate();

        /**
          * setParam
          *
          * This method can be called to set a runtime parameter name, value pair.
          *
          * @param     The parameter we want to set
          */
        void setParam(const TA_Base_Core::RunParam& param);

        /**
          * getParams
          *
          * This method can be called to get the applications current run time parameters
          *
          * @return The run param sequence
          */
        TA_Base_Core::RunParamSeq* getParams();

        /**
          * poll
          *
          * This method is called by the Process Manager to verify that the
          * process is still alive and operating correctly. If a problem is detected
          * within the process, then an exception is thrown. Otherwise, it simply
          * returns.
          *
          * @exception PollException  Thrown if a problem is detected by the process.
          */            
        void poll();

        /**
          * notifyTerminating
          *
          * This method is used to notify the Process Manager when we are 
          * terminating
          *
          * @param The status code with which we are exiting
          * @param A termination message
          */
        void notifyTerminating(EManagedProcessTerminateCode code, const char* msg);


        /**
         * changeState
         *
         * This is called when the agent moves into a new state
         *
         * @param The state the agent is now running in
         */
        void changeState(EProcessStatus processStatus);

        
        /**
          * registerManagedApplication
          *
          * This method allows classes that implement the IManagedApplication
          * interface (eg GenericAgent, GenericGUI) to register directly with
          * the Process Manager. This enables the Process Manager to utilise
          * their respective exposed interfaces.
          *
          * @param Handle to the object that wants to register
          */
        void registerManagedApplication(IManagedApplicationCorbaDef_ptr app);

        /**
          * onControllerFailure
          *
          * This is a callback method that is called by the MonitoringThread
          * if it is unable to contact the Process Manager
          */
        void onControllerFailure();

        /**
          * haveBeenToldToTerminate
          *
          * This is called by the Monitoring Thread to determine whether or
          * not the process monitor has told us to terminate
          *
          * @return TRUE if we have been told to terminate
          */
        bool haveBeenToldToTerminate();

        /**
          * cleanup
          *
          * This is called by the Monitoring Thread to tell us to clean up
          * after it detects that we have been told to terminate
          */
        void cleanup();

        /**
          * servicesRestarted
          *
          * This is called by the Process Monitor to tell us to tell the
          * notification service to do some re-resolving
          */
        void servicesRestarted();

    private:

        /**
          * locateProcessManager
          *
          * This method retrieves a handle to the Process Manager from
              * TA_Base_Core::CorbaUtil and sets the m_ProcessManager
          * member variable.
          *
          * @exception ManagedProcessException raised if the Process Manager
          *            cannot be found.
          */
        void locateProcessManager();

        /**
          * registerWithProcessManager
          *
          * This method registers with the Process Manager and sets the 
          * m_managedProcessID variable.
          *
          * @exception ProcessAlreadyRunningException raised if the process was
          *            already running
          * @exception ManagedProcessException raised if registration fails.
          */
        void registerWithProcessManager();

        /**
          * retrieveSystemParameters
          *
          * This method retrieves system parameters from the Process Manager.
          * If any parameters have already been set via the command line, they
          * wont be updated (command line has priority).
          * 
          * @exception ManagedProcessException raised if the Process Manager
          *            cant be contacted.
          */
        void retrieveSystemParameters();

        const std::string& getProcessStatusString(EProcessStatus status);


        // Disable the copy constructor and assignment operator
        ManagedProcess( const ManagedProcess& theManagedProcess);
        ManagedProcess& operator=(const ManagedProcess &);

        // Handle to the Process Manager
        IProcessManagerCorbaDef_ptr m_ProcessManager;

        // Managed process ID
        ProcessId m_managedProcessID;

        // Set to TRUE if we have received a request to terminate
        bool m_terminationFromManager;

        // The thread that monitors the state of the Process Manager
        MonitoringThread* m_monitoringThread;

        // Handle to the registered managed process user
        IManagedProcessUser* m_managedProcessUser;

        // Reference to object that will respond to polls
        PollListener* m_pollListener;
    };
}

#endif // !defined(MANAGEDPROCESS)
