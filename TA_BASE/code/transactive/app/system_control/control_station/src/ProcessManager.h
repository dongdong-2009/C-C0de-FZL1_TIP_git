/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ProcessManager.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This receives callbacks from the ManagedProcesses and will also maintain the running
  * of applications. This implements the ProcessManager corba definition which means this
  * object is what the ManagedProcess's get when they resolve the initial process monitor.
  */

#if !defined(ProcessManager_C550A2B9_9026_4f94_9F48_174625E800BF__INCLUDED_)
#define ProcessManager_C550A2B9_9026_4f94_9F48_174625E800BF__INCLUDED_

#include "app/system_control/control_station/src/IProcessManager.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "bus/database_status_monitor/src/IDatabaseStatusObserver.h"
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/Condition.h"
#include <list>

// Forward declarations of classes

class RunningApplication;
class ProcessWatcher;
class Application;

namespace TA_Base_Core
{
    class ConfigUpdateDetails;
    class ThreadPoolSingletonManager;
}

namespace TA_Base_Bus
{
    class SimpleDbStatusChecker;
}

class ProcessManager
    : public virtual POA_TA_Base_Core::IProcessManagerCorbaDef,
      public virtual TA_Base_Core::ServantBase,
      public TA_Base_Core::IOnlineUpdatable,
      public IProcessManager,
      public TA_Base_Bus::IDatabaseStatusObserver
{
public:
    /**
     * Constructor
     *
     * @exception DatabaseException A DatabaseException will be thrown if there is a
     *            problem establishing a connection with the database, or if an
     *            error is encountered while retrieving data.
     * @exception DataException A DataException will be thrown if the data cannot be
     *            converted to the required format (e.g. ACCESSLEVEL should be an
     *            unsigned long), or if the wrong ammount of data is retrieved.
     */
    ProcessManager();

    /**
     * Destructor
     */
    virtual ~ProcessManager();

public:

    /**
      * launchApplication
      *
      * This will launch an application in a specified position. Any additional arguments
      * to provide to the application can be specified.
      *
      * @param appType - The application to launch
      * @param string additionalCommands - This is a string containing any additional commands to
      *                                    be passed to the application. This should take the form
      *                                    of a command line you would pass to the application if
      *                                    you ran it from the command line. The default is no
      *                                    parameters.
      * @param posFlag - How the application is going to be positioned relative to boundary paramter
      * @param alignFlag - How the application is going to be positioned relative to point parameter
      * @param objectDim - The dimension of the object that this application will be launched relative
      *                    to (if POS_RELATIVE or POS_NOOVERLAP is specified).
      * @param boundaryDim - The dimension of the object that this application will be bounded in
      *                      (if POS_BOUNDED is specified)
      * @param bool isVisible - This indicates if the running application should be displayed
      *                         to the user as running. For example a banner should not be
      *                         displayed as a running application that the user can switch to.
      *                         Therefore for this 'false' should be passed in. The default is
      *                         'true' as most applications will want to be seen.
      *
      * @exception ApplicationException - This indicates that the application could not be launched.
      */
    virtual void launchApplication(const unsigned long appType,
                                   const std::string& additionalCommands,
                                   const unsigned long posFlag,
                                   const unsigned long alignFlag,
                                   const RECT& objectDim,
                                   const RECT& boundaryDim,
                                   const bool isVisible = true);

    /**
      * giveApplicationFocus
      *
      * This will tell the specified application to change it's focus so it is the current
      * active window.
      *
      * @param unsigned long pid - This is the process id of the application to gain the focus.
      *
      */
    virtual void giveApplicationFocus(unsigned long pid);

    /**
     * getRunningApplications
     *
     * This will retrieve a list of all running applications and return it. It will return
     * a display name and the process id so that the caller can display the list to the user
     * and then has the processId to identify the application.
     *
     * @param bool - Indicates if we are retrieving only the visible applications (true) or all of
     *               the applications( false). The default is to retrieve only the visible applications.
     *
     * @return map<unsigned long, string> - This contains the name of each running application
     *                                      keyed by process id.
     *
     */
    virtual std::map<unsigned long, std::string> getRunningApplications(bool areGettingJustVisible = true);

    /**
        * setupDbMonitors
        *
        * Fetches and processes the database connection data.
        *
        * @param pollPeriod       Populated with the time, in milliseconds, between polls to
        *                         each process.
        *
        * @exception FatalConfigurationException  Thrown if required run param not set
        * @exception DbConnectionNoFile           Thrown if the database config file
        *                                         can not be opened
        */
    void setupDbMonitors(int& pollPeriod);

    /**
     * getAllApplications
     *
     * This will retrieve a list of all applications available to the user to be launched
     * and returns it. It will return a display name and the application type so that the
     * caller can display the list to the user and then has the application type to identify
     * the application.
     *
     * @return std::vector<IProcessManager::ApplicationProperties>
     *         - This contains the key, name and a boolean indicating whether the current
     *           operator has the rights to run the application.
     */
    virtual std::vector<IProcessManager::ApplicationProperties> getApplications();

    /**
     * getRunningApplicationObjects
     *
     * This will return a reference to the list of running applications.
     *
     * @return RunningApplicationMap& - This returns a reference to the map of running applications
     */
    virtual RunningApplicationMap& getRunningApplicationObjects()
    {
        return m_runningApplications;
    }

    /**
     * getApplicationObjects
     *
     * This will return a reference to the list of running applications.
     *
     * @return ApplicationMap& - This returns a reference to the map of all applications
     */
    virtual ApplicationMap& getApplicationObjects()
    {
        return m_applications;
    }

    /**
     * terminateAllApplications
     *
     * If isLogout is set to true, this will terminate all currently
     * running applications.  Otherwise, this method will only terminate
     * applications that are supposed to be terminated upon logout
     *
     * @param ETerminateMode - By default this is set to LOGOUT to indicate that
     *        only applications that are supposed to be terminated upon
     *        logout are terminated.  If set to EXIT, all applications
     *        will be terminated.
     */
    void terminateAllApplications(TA_Base_App::TA_ControlStation::ETerminateMode terminateMode = TA_Base_App::TA_ControlStation::LOGOUT);

    /**
     * applicationHasTerminated
     *
     * This is called if it is detected that an application has terminated. This method will
     * then delete that instance of RunningApplication and remove it from the map.
     *
     * @param unsigned long processId - The id of the application that has terminated.
     */
    void applicationHasTerminated(unsigned long processId);

    /**
     * serverHasChangedState
     *
     * This is called by the ServerMonitor if it finds that the server has changed state. This will
     * alert GraphWorX and all GUI applications.
     *
     * @param bool - This is true if the server can be contacted. This is false if it cannot
     */
    void serverHasChangedState(bool isContactable);

    /**
      * terminating
      *
      * This is called by a ManagedProcess when it is terminating. This will remove the
      * application from the map of running applications.
      *
      * @param ProcessId - This indicates which application has terminated so it can be accurately
      *                    identified in the map of running applications.
      * @param EManagedProcessTerminateCode - This indicates how the application terminated so
      *                                       we can record why it terminated.
      *
      */
    virtual void terminating(TA_Base_Core::ProcessId p_processID,
                             TA_Base_Core::EManagedProcessTerminateCode p_code);

    /**
      * registerManagedProcess
      *
      * This is called by a ManagedProcess when it wishes to register. The method stored the
      * pointer to the ManagedProcess so it can perform calls on it. It also provides the
      * process id so we can link the managed process to an application that is already running.
      *
      * @param IManagedProcessCorbaDef_ptr - This is the ManagedProcess Corba object.
      * @param ProcessId - This indicates which application is registering so it can be accurately
      *                    identified in the map of running applications (or added if we do not
      *                    yet know about it).
      * @param unsigned long - This indicates the type of application that is registering. If we
      *                        do not yet know about this application we can ensure that we get
      *                        the correct details for it.
      * @param char* entity - This is the entity that the application is using for its configuration
      *                        information. This is used to determine a log file name to be passed
      *                        to the application.
      *
      * @exception IProcessManagerCorbaDef::ProcessAlreadyRunningException
      *            This indicates that there is already a process running of this type and there should not
      *            be another.
      *            IProcessManagerCorbaDef::FailedToRegisterException
      *            This indicates that an error occurred when trying to register the application. This
      *            means the calling application should terminate with an error.
      */
    virtual void registerManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess,
                                        TA_Base_Core::ProcessId p_processId,
                                        CORBA::ULong applicationType,
                                        const char* entity);

    /**
      * registerManagedApplication
      *
      * This is called by a ManagedProcess when it wishes to register as a ManagedApplication.
      * This means that it is a GUI that wants special callbacks. It receives a
      * pointer to the ManagedApplication so it can perform calls on it. It also provides the
      * process id so we can link the managed application to an application that is already running.
      *
      * @param IManagedApplicationCorbaDef_ptr - This is the ManagedApplication Corba object.
      * @param ProcessId - This indicates which application is registering so it can be accurately
      *                    identified in the map of running applications
      *
      */
    virtual void registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp,
                                            TA_Base_Core::ProcessId p_processId);

    /**
      * poll
      *
      * This is called to ensure that the Control Station is still alive. The function actually
      * does nothing.
      */
    virtual void poll();

    /**
      * statusChanged
      *
      * This is for agents only and does nothing in this scope.
      */
    virtual void statusChanged(TA_Base_Core::EProcessStatus status, TA_Base_Core::ProcessId p_processId)
    {
    }

    /**
      * notifyStatusChange
      *
      * called when a database changes state
      */
    virtual void notifyStatusChange(const std::string& DbConnectionInfo, TA_Base_Bus::DatabaseStatusCorbaDef::EState status);

    /**
      * getParams
      *
      * This is called by a ManagedProcess to retrieve the RunParams. This will return all the
      * standard runparams being passed to all applications as well as some application specific
      * ones. (ie DebugFile)
      *
      * @return RunParamSeq - The list of RunParams
      *
      * @param ProcessId - The process id of the application so we can ensure that the application
      *                    requesting the parameters is registered with us and return it
      *                    appropriate parameters.
      * @param char* - The host name. This should always be an empty string.
      */
    virtual TA_Base_Core::RunParamSeq* getParams(TA_Base_Core::ProcessId p_processId, const char* hostname);

    /**
      * processUpdate
      *
      * This tells the application that one of the objects it was interested in has
      * been updated and should be processed.
      *
      * @param    updateEvent - This event contains all the information about the update
      */
    virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    /**
      * updateSessionId
      *
      * Updates the session Id of all running applications
      *
      * @param sessionId - The new session Id
      */
    void updateSessionId(std::string& sessionId);

    /**
     * dutyChanged
     *
     * Notifies all running applications that the duties for the current session have changed.
     */
    void dutyChanged();

    /**
      * setApplicationsToTerminate
      *
      * The name of the application to terminate when the control station terminates
      *
      * @param appName A delimited string containing the names of applications to terminate when
      *         the control station terminates
      */
    void setApplicationsToTerminate(const std::string& appName);

    /**
      * setApplicationsToTerminateOnLogout
      *
      * The name of the application to terminate when the operator logs out
      *
      * @param appName A delimited string containing the names of applications to terminate when
      *         the operator logs out
      */
    void setApplicationsToTerminateOnLogout(const std::string& appName);

    /**
      * terminateExternalApplications
      *
      * Terminate any applications that the control station is supposed to terminate even
      * though it may not have started up these applications.
      *
      */
    void terminateExternalApplications();

private:
    // Hidden as they are not needed
    ProcessManager(const ProcessManager& theProcessManager);
    ProcessManager& operator=(const ProcessManager& pm);

    /**
      * loadApplications
      *
      * This loads all the possible applications that can be launched and run from the database
      * and populates the ApplicationMap.
      *
      * @exception DatabaseException A DatabaseException will be thrown if there is a
      *            problem establishing a connection with the database, or if an
      *            error is encountered while retrieving data.
      * @exception DataException A DataException will be thrown if the data cannot be
      *            converted to the required format (e.g. ACCESSLEVEL should be an
      *            unsigned long), or if the wrong ammount of data is retrieved.
      */
    void loadApplications();

    /**
      * shouldApplicationBeRelaunched
      *
      * This checks if an application is already running. If it is already running and should only have
      * one instance running then this method will populate matchingApp with the already running application
      * and return false.
      *
      * @param unsigned long appType - This indicates which application to check.
      * @param RunningApplication*   - This will be populated with the matching running application if
      *                                one is found.
      *
      * @return bool - This is true if the application should be relaunched. matchingApp will be NULL.
      *              - False indicates the application should not be relaunched and matchingApp will hold
      *                a pointer to the apllication which should be given the focus.
      */
    bool shouldApplicationBeRelaunched(const unsigned long appType, RunningApplication*& matchingApp);

    /**
     * getTerminationList
     *
     * Process the list of applications to be terminated on exit from a comma delimited string to a vector of strings
     *
     * @param commaList The comma delimited string of applications to terminate
     *
     * @return the list of applications to be terminated on exit in lower case
     */
    std::vector<std::string> getTerminationList(std::string commaList);

    /**
      * terminateExternalApplications
      *
      * Terminate any applications that the control station is supposed to terminate even
      * though it may not have started up these applications.
      *
      * @param appsToTerminate A vector of strings containing the names of applications to terminate
      * @param jobObject The job object responsible for force killing the applications if any should
      *        fail to terminate the normal way
      *
      */
    void terminateExternalApplications(const std::vector<std::string>& appsToTerminate, HANDLE jobObject);

    bool resourceIsPermitted(unsigned long resourseKey);

    TA_Base_Core::EOperationMode handleDoubleControlBaseOnCentralDb(const char*, CORBA::Boolean, CORBA::ULong) { return TA_Base_Core::NotApplicable; }

private:

    //
    // This is used to lock the use of the maps below.
    //
    TA_Base_Core::NonReEntrantThreadLockable m_classLock;

    //
    // A map of all possible applications that can be launched keyed by application type.
    //
    ApplicationMap m_applications;

    //
    // A list of all currently running applications
    //
    RunningApplicationMap m_runningApplications;

    //
    // This thread will check the processes to see when they have terminated
    //
    ProcessWatcher* m_processWatcherThread;

    // Stores the name of any external applications to terminate when the control station exits
    std::vector<std::string> m_appsToTerminate;

    // Stores the name of any external applications to terminate when the control station logouts
    std::vector<std::string> m_appsToTerminateOnLogout;

    // The handle to the session job object that will be associated with applications that need
    // to be terminated when the session terminates
    HANDLE m_sessionJobObject;

    // The handle to the global job object that will be associated with applications that don't need
    // to be terminated when the session terminates
    HANDLE m_globalJobObject;

    // The name of the job object that will be associated with the applications that need to be
    // terminated when the session terminates
    static const std::string SESSION_JOB_OBJECT_NAME;

    // The name of the global job object that will be associated with the applications that don't need
    // to be terminated when the session terminates
    static const std::string GLOBAL_JOB_OBJECT_NAME;

    //TD17996
    /**
    * Hold a list of connection strings for every database we have to monitor.
    * Must be unique, thus the map.
    */
    typedef std::map<std::string, TA_Base_Bus::SimpleDbStatusChecker*> MoniteredDbs;
    MoniteredDbs m_moniteredDatabases;

    std::string m_controlDbName;

    std::string m_monitoredDbName;

    std::string m_centralDbName;

    /**
    * Signalled when the ProcessManager has detected an active database
    */
    TA_Base_Core::Semaphore m_initialisedDBStatusMonitor;

    /**
    * true if at startup
    */
    bool volatile m_isStartup;
    TA_Base_Bus::ResourceList permitedResourceList;
    TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;
};

#endif // !defined(ProcessManager_C550A2B9_9026_4f94_9F48_174625E800BF__INCLUDED_)
