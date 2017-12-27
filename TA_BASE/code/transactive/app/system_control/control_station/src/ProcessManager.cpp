/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This receives callbacks from the ManagedProcesses and will also maintain the running
  * of applications. This implements the ProcessManager corba definition which means this
  * object is what the ManagedProcess's get when they resolve the initial process monitor.
  */
#pragma comment( lib, "winmm.lib" )
#include "app/system_control/control_station/src/StdAfx.h"
#include "app/system_control/control_station/src/EnumProcess.h"
#include "app/system_control/control_station/src/SessionManager.h"
#include "app/system_control/control_station/src/ProcessManager.h"
#include "app/system_control/control_station/src/ProcessWatcher.h"
#include "app/system_control/control_station/src/ApplicationMover.h"
#include "app/system_control/control_station/src/RunningApplication.h"
#include "app/system_control/control_station/src/ServerMonitor.h"
#include "app/system_control/control_station/src/Application.h"
#include "app/system_control/control_station/src/ControlStationRootDialog.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/database_status_monitor/src/SimpleDbStatusChecker.h"
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include <cctype>
#include <algorithm>
#include <stdlib.h>

// using IProcessManager::ApplicationProperties;
typedef IProcessManager::ApplicationProperties ApplicationProperties;
using TA_Base_Core::ApplicationException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::OnlineUpdateListener;

using namespace TA_Base_Core;

const std::string ProcessManager::GLOBAL_JOB_OBJECT_NAME = "Global\\CSGlobalJobObject";
const std::string ProcessManager::SESSION_JOB_OBJECT_NAME = "Global\\CSSessionJobObject";
const std::string USE_DEBUGPIDFILENAMES = "1";

enum EAlignment             // Alignment flags - Used to indicate how the application will be aligned relative to a point, can use a combination
{
    ALIGN_FIT = 0x1,        // Fit into the specified area
    ALIGN_LEFT = 0x2,       // Leftmost position at x coordinate of 'objectDim' or 'boundaryDim'
    ALIGN_VERTICAL = 0x4,   // Vertically centred (i.e. x coordinate = [left+right]/2)'objectDim' or 'boundaryDim'
    ALIGN_RIGHT = 0x8,      // Rightmost position at x coordinate of 'objectDim' or 'boundaryDim'
    ALIGN_TOP = 0x10,       // Topmost position at y coordinate of 'objectDim' or 'boundaryDim'
    ALIGN_HORIZONTAL = 0x20,// Horizontally centred (i.e. y coordinate = [top+bottom]/2) 'objectDim' or 'boundaryDim'
    ALIGN_BOTTOM = 0x40,    // Bottommost position at y coordinate of 'objectDim' or 'boundaryDim'
};

ProcessManager::ProcessManager()
    : m_processWatcherThread(NULL),
      m_initialisedDBStatusMonitor(0),
      m_globalJobObject(NULL),
      m_isStartup(true),
      m_controlDbName(""),
      m_monitoredDbName(""),
      m_centralDbName(""),
      m_sessionJobObject(NULL),
      m_threadPool(TA_Base_Core::ThreadPoolSingletonManager::getInstance())
{
    ThreadGuard guard(m_classLock);

    SECURITY_ATTRIBUTES* lpJobAttributes = NULL;
    m_globalJobObject = CreateJobObject(NULL, GLOBAL_JOB_OBJECT_NAME.c_str());

    OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::APPLICATION, *this);
    loadApplications();

    int pollTime = 5;

    setupDbMonitors(pollTime);

    // Start the database monitors
    for (MoniteredDbs::iterator monDbIt = m_moniteredDatabases.begin();
            monDbIt != m_moniteredDatabases.end(); monDbIt++)
    {
        if (monDbIt->second)
        {
            monDbIt->second->start();
        }
    }

    //wait for database monitor to find a working database
    m_initialisedDBStatusMonitor.wait();
    m_processWatcherThread = new ProcessWatcher(*this);
    m_processWatcherThread->start();

    ServerMonitor::getInstance().setProcessManager(this);
    ServerMonitor::getInstance().start();
}

ProcessManager::~ProcessManager()
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "Destructor");

    try
    {
        ThreadGuard guard(m_classLock);
        OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::APPLICATION, *this);

        if (m_processWatcherThread != NULL)
        {
            m_processWatcherThread->terminateAndWait();
            delete m_processWatcherThread;
            m_processWatcherThread = NULL;
        }

        ServerMonitor::getInstance().terminateAndWait();
        ServerMonitor::getInstance().cleanUp();

        for (RunningApplicationMap::iterator riter = m_runningApplications.begin();
                riter != m_runningApplications.end();
                ++riter)
        {
            riter->second->terminate();
            delete riter->second;
            riter->second = NULL;
        }

        for (ApplicationMap::iterator iter = m_applications.begin();
                iter != m_applications.end();
                ++iter)
        {
            delete iter->second;
            iter->second = NULL;
        }

        m_applications.erase(m_applications.begin(), m_applications.end());
        m_runningApplications.erase(m_runningApplications.begin(), m_runningApplications.end());
    }
    catch (...)
    {
        // Catch all exceptions so we don't get errors filtering out
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of ProcessManager");
    }

    if (m_globalJobObject != NULL)
    {
        TerminateJobObject(m_globalJobObject, 0);
    }

    if (m_sessionJobObject != NULL)
    {
        TerminateJobObject(m_sessionJobObject, 0);
    }

    TA_Base_Core::ThreadPoolSingletonManager::removeInstance(m_threadPool);

    LOG(SourceInfo, DebugUtil::FunctionExit, "Destructor");
}

void ProcessManager::launchApplication(const unsigned long appType,
                                       const std::string& additionalCommands,
                                       const unsigned long posFlag,
                                       const unsigned long alignFlag,
                                       const RECT& objectDim,
                                       const RECT& boundaryDim,
                                       const bool isVisible /*= true*/)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "launchApplication");
    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Launching the application %lu with the command line: %s", appType, additionalCommands.c_str());

    ThreadGuard guard(m_classLock);

    ApplicationMap::iterator appIter = m_applications.find(appType);

    if (appIter == m_applications.end())
    {
        char errorStr[256] = {0};
        sprintf(errorStr, "Application type not found - %lu", appType);
        TA_THROW(ApplicationException(errorStr));
    }

    RunningApplication* alreadyRunningApp = NULL;

    if (!shouldApplicationBeRelaunched(appType, alreadyRunningApp))
    {
        TA_ASSERT(alreadyRunningApp != NULL, "shouldApplicationBeRelaunched returned false but has not populated alreadyRunningApp");

        // Application did not need to be relaunched. Tell the existing one to come to the foreground and if
        // necessary pass it new parameters
        for (int i = 0; i < 2; ++i)
        {
            try
            {
                if (alreadyRunningApp->getGUIEntity().shouldPassParameters())
                {
                    alreadyRunningApp->passNewParameters(additionalCommands);
                }

                if (alreadyRunningApp->getGUIEntity().shouldReposition())
                {
                    //TD:20589
                    //[1/13/2009 zhangziliang]:for td20589

                    unsigned long nAlign = ALIGN_LEFT | ALIGN_TOP;

                    // this moves and gives focus to the application
                    //alreadyRunningApp->move(posFlag, alignFlag, objectDim, boundaryDim);
                    alreadyRunningApp->move(posFlag, nAlign, objectDim, boundaryDim);
                }
                else
                {
                    // don't want to move it, so just give focus
                    alreadyRunningApp->move(0, 0, TA_Base_Bus::TA_GenericGui::DEFAULT_RECT, TA_Base_Bus::TA_GenericGui::DEFAULT_RECT);
                }

                LOG(SourceInfo, DebugUtil::FunctionExit, "launchApplication");
                return;
            }
            catch (ApplicationException&)
            {
                LOG(SourceInfo, DebugUtil::ExceptionCatch, "ApplicationException", "Application could not be given focus.");
            }
            catch (DatabaseException&)
            {
                LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not determine if the application's instances are limited.");
            }
            catch (DataException&)
            {
                LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not determine if the application's instances are limited.");
            }

            TA_Base_Core::Thread::sleep(500);
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "We will relaunch the application %s.", alreadyRunningApp->getApplicationName().c_str());
    }

    RunningApplication* runningApp;

    try
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Application is not already running so we need to launch it");

        // Application is not running or can be launched again so let's start it.
        runningApp = new RunningApplication(appIter->second->getGuiDatabaseEntry(), isVisible);

        if (runningApp == NULL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on RunningApplication returned NULL");
            TA_THROW(ApplicationException("Could not create a new RunningApplication"));
        }

        runningApp->launch(additionalCommands, posFlag, alignFlag, objectDim, boundaryDim);

        if (runningApp->shouldTerminateOnLogout())
        {
            if (m_sessionJobObject == NULL)
            {
                m_sessionJobObject = CreateJobObject(NULL, SESSION_JOB_OBJECT_NAME.c_str());
            }

            AssignProcessToJobObject(m_sessionJobObject, runningApp->getProcessHandle());
        }
        else
        {
            AssignProcessToJobObject(m_globalJobObject, runningApp->getProcessHandle());
        }

        m_runningApplications.insert(RunningApplicationMap::value_type(runningApp->getProcessId(), runningApp));
    }
    catch (DatabaseException&)
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Exception while launching the application. This is being converted into an ApplicationException");
        char errorStr[1000] = {0};

        if (runningApp != NULL)
        {
            sprintf(errorStr, "Application could not be launched. --entity-name=%s AppType=%lu AdditionalCommandLine=%s",
                    runningApp->getGUIEntity().getExecutable().c_str(), appType, additionalCommands.c_str());
            delete runningApp;
        }
        else
        {
            sprintf(errorStr, "Application could not be launched. --entity-name=c:\\Unknown.exe AppType=%lu AdditionalCommandLine=%s", appType, additionalCommands.c_str());
        }

        TA_THROW(ApplicationException(errorStr));
    }
    catch (DataException&)
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Exception while launching the application. This is being converted into an ApplicationException");

        char errorStr[1000] = {0};

        if (runningApp != NULL)
        {
            sprintf(errorStr, "Application could not be launched. --entity-name=%s AppType=%lu AdditionalCommandLine=%s",
                    runningApp->getGUIEntity().getExecutable().c_str(), appType, additionalCommands.c_str());
            delete runningApp;
        }
        else
        {
            sprintf(errorStr, "Application could not be launched. --entity-name=c:\\Unknown.exe AppType=%lu AdditionalCommandLine=%s", appType, additionalCommands.c_str());
        }

        TA_THROW(ApplicationException(errorStr));
    }
    catch (ApplicationException&)
    {
        char errorStr[1000] = {0};

        if (runningApp != NULL)
        {
            sprintf(errorStr, "Application could not be launched. --entity-name=%s AppType=%lu AdditionalCommandLine=%s",
                    runningApp->getGUIEntity().getExecutable().c_str(), appType, additionalCommands.c_str());
            delete runningApp;
        }
        else
        {
            sprintf(errorStr, "Application could not be launched. --entity-name=c:\\Unknown.exe AppType=%lu AdditionalCommandLine=%s", appType, additionalCommands.c_str());
        }

        TA_THROW(ApplicationException(errorStr));
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "launchApplication");
}

bool ProcessManager::shouldApplicationBeRelaunched(const unsigned long appType, RunningApplication*& matchingApp)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "shouldApplicationBeRelaunched");

    // Check if we should be launching a new instance of the application or just giving an existing one focus.
    RunningApplication* matchingApplication = NULL;

    for (RunningApplicationMap::iterator iter = m_runningApplications.begin();
            iter != m_runningApplications.end();
            ++iter
        )
    {
        if (iter->second->isApplicationTypeEqualTo(appType))
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Matching application already running. Lets check to see if we should launch again");
            matchingApplication = iter->second;
            break;
        }
    }

    if ((matchingApplication == NULL) || (!matchingApplication->isRunning()))
    {
        // The application is not already running so it will need to be started
        LOG(SourceInfo, DebugUtil::FunctionExit, "shouldApplicationBeRelaunched");
        return true; //true means the application should be relaunched
    }

    try
    {
        // If the application should only have one instance running then tell it to gain the focus instead of relaunching it
        if (matchingApplication->getGUIEntity().areInstancesLimited())
        {
            matchingApp = dynamic_cast<RunningApplication*>(matchingApplication);
            LOG(SourceInfo, DebugUtil::FunctionExit, "shouldApplicationBeRelaunched");
            return false; //false means the application should NOT be relaunched
        }
    }
    catch (DatabaseException&)
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not determine if the application's instances are limited so relaunching it");
    }
    catch (DataException&)
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not determine if the application's instances are limited so relaunching it");
    }

    matchingApp = NULL;
    LOG(SourceInfo, DebugUtil::FunctionExit, "shouldApplicationBeRelaunched");
    return true; //true means the application should be relaunched
}

void ProcessManager::applicationHasTerminated(unsigned long processId)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "applicationHasTerminated");

    RunningApplication* application = NULL;

    {
        ThreadGuard guard(m_classLock);

        RunningApplicationMap::iterator matchingAppIter = m_runningApplications.find(processId);

        // If we find an entry we want to delete it from the list
        if (matchingAppIter != m_runningApplications.end())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Application, %s, has terminated with process id %lu", matchingAppIter->second->getApplicationFullName().c_str(), processId);
            // Delete the RunningApplication object
            application = matchingAppIter->second;
            m_runningApplications.erase(matchingAppIter);
        }
    }

    if (application != NULL && false == application->shouldTerminateOnLogout())
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Relaunch application %s", application->getApplicationFullName().c_str());
        application->relaunch(*this);
    }

    delete application;
    application = NULL;

    LOG(SourceInfo, DebugUtil::FunctionExit, "applicationHasTerminated");
}

void ProcessManager::serverHasChangedState(bool isContactable)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "serverHasChangedState");

    // Alert the GraphWorX instances
    ControlStationRootDialog::getInstance().PostMessage(WM_SERVER_STATE_CHANGE, isContactable, 0);

    if (isContactable)
    {
        // attempt to get another session
        SessionManager::getInstance().renewSession();
    }

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "MFT has been %s", isContactable ? "reconnected to network" : "disconnected from network");

    ThreadGuard guard(m_classLock);
    // Alert the GUI's
    RunningApplicationMap::iterator iter = m_runningApplications.begin();

    while (iter != m_runningApplications.end())
    {
        iter->second->changeServerState(isContactable);
        ++iter;
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "serverHasChangedState");
}

void ProcessManager::terminating(TA_Base_Core::ProcessId p_processID,
                                 TA_Base_Core::EManagedProcessTerminateCode p_code)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "terminating");

    // No point doing all this work if we arent' going to log so check first.
    if (DebugUtil::DebugInfo <= DebugUtil::getInstance().getLevel())
    {
        std::string status;

        switch (p_code)
        {
        case (TA_Base_Core::RequestedTerminate):
            status = "Requested";
            break;

        case (TA_Base_Core::InitError):
            status = "Init Error";
            break;

        case (TA_Base_Core::CommsError):
            status = "Comms Error";
            break;

        case (TA_Base_Core::UserExit):
            status = "User Exit";
            break;

        case (TA_Base_Core::DependencyFailure):
            status = "Dependency Failure";
            break;

        case (TA_Base_Core::FatalError):
            status = "Fatal Error";
            break;

        case (TA_Base_Core::AccessDenied):
            status = "Access Denied";
            break;

        default:
            status = "Unknown";
            break;
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Managed Process has terminated with id %d and state '%s'", p_processID, status.c_str());
    }

    applicationHasTerminated(p_processID);

    LOG(SourceInfo, DebugUtil::FunctionExit, "terminating");
}

void ProcessManager::registerManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess,
        TA_Base_Core::ProcessId p_processId,
        CORBA::ULong applicationType,
        const char* entity)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "registerManagedProcess");

    ThreadGuard guard(m_classLock);

    RunningApplicationMap::iterator matchingAppIter = m_runningApplications.find(p_processId);

    if (matchingAppIter != m_runningApplications.end())
    {
        // We already have this application in our list of running applications so just store
        // the managed process pointer and we're done.
        TA_Base_Core::IManagedProcessCorbaDef_var managedProcess = TA_Base_Core::IManagedProcessCorbaDef::_duplicate(p_managedProcess);

        matchingAppIter->second->setManagedProcess(managedProcess);
        matchingAppIter->second->setEntityName(entity);
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Managed Process Registered (pid=%lu, appType=%lu, entity=%s)", p_processId, applicationType, entity);
        LOG(SourceInfo, DebugUtil::FunctionExit, "registerManagedProcess");
        return;
    }

    // This is a process that we did not launch so we must add it to our list of running
    // applications.

    // First find the matching IGui*.
    ApplicationMap::iterator appIter = m_applications.find(applicationType);

    if (appIter == m_applications.end())
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Application type not found - %lu", applicationType);
        throw TA_Base_Core::FailedToRegisterException();
    }

    RunningApplication* alreadyRunningApp = NULL;

    if (!shouldApplicationBeRelaunched(applicationType, alreadyRunningApp))
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Application %lu does not need to be relaunched as it is already running", applicationType);
        TA_ASSERT(alreadyRunningApp != NULL, "shouldApplicationBeRelaunched returned false but has not populated alreadyRunningApp");

        // Application did not need to be relaunched. Tell the existing one to come to the foreground and if
        // necessary pass it new parameters
        try
        {
            if (alreadyRunningApp->getGUIEntity().shouldPassParameters())
            {
                if (!CORBA::is_nil(p_managedProcess))
                {
                    try
                    {
                        TA_Base_Core::IManagedProcessCorbaDef_var mp = TA_Base_Core::IManagedProcessCorbaDef::_duplicate(p_managedProcess);
                        TA_Base_Core::RunParamSeq_var params = mp->getParams();
                        alreadyRunningApp->passNewParameters(params);
                    }
                    catch (const CORBA::Exception& ex)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
                            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
                            "Could not retrieve current parameters of the ManagedProcess to pass to the existing ManagedProcess");
                    }
                }
            }

            alreadyRunningApp->changeApplicationFocus(TA_Base_Core::GetFocus);

            // throw CORBA exception so use 'throw' not 'TA_THROW'
            throw TA_Base_Core::ProcessRunningException();
        }
        catch (ApplicationException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "ApplicationException", "Application could not be given focus. We will continue to let this new application run.");
        }
        catch (DatabaseException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not determine if the application's instances are limited so relaunching it");
        }
        catch (DataException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not determine if the application's instances are limited so relaunching it");
        }
    }

    // We can let this application continue to run so add the application as a running application
    RunningApplication* runningApp;

    try
    {
        TA_Base_Core::IManagedProcessCorbaDef_var managedProcess = TA_Base_Core::IManagedProcessCorbaDef::_duplicate(p_managedProcess);
        runningApp = new RunningApplication(appIter->second->getGuiDatabaseEntry(), p_processId, managedProcess);

        if (runningApp == NULL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on RunningApplication returned NULL");
            throw TA_Base_Core::FailedToRegisterException();
        }

        runningApp->setEntityName(entity);
        m_runningApplications.insert(RunningApplicationMap::value_type(p_processId, runningApp));
    }
    catch (DatabaseException&)
    {
        if (runningApp != NULL)
        {
            delete runningApp;
        }

        throw TA_Base_Core::FailedToRegisterException();
    }
    catch (DataException&)
    {
        if (runningApp != NULL)
        {
            delete runningApp;
        }

        throw TA_Base_Core::FailedToRegisterException();
    }
    catch (ApplicationException&)
    {
        if (runningApp != NULL)
        {
            delete runningApp;
        }

        throw TA_Base_Core::FailedToRegisterException();
    }

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Managed Process Registered (pid=%lu, appType=%lu, entity=%s)", p_processId, applicationType, entity);

    LOG(SourceInfo, DebugUtil::FunctionExit, "registerManagedProcess");
}

void ProcessManager::registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp,
        TA_Base_Core::ProcessId p_processId)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "registerManagedApplication");

    ThreadGuard guard(m_classLock);

    RunningApplicationMap::iterator matchingAppIter = m_runningApplications.find(p_processId);

    if (matchingAppIter == m_runningApplications.end())
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Managed application tried to register but the process id is not recognised: %lu", p_processId);
        LOG(SourceInfo, DebugUtil::FunctionExit, "registerManagedApplication");
        return;
    }

    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Managed Application Registered (pid=%lu)", p_processId);

    // If we have it as a running application then we want to store the ManagedApplication pointer
    try
    {
        // First attempt to convert it into a GUI instead of just a generic application.
        if (!CORBA::is_nil(p_managedApp))
        {
            TA_Base_Core::IManagedGUICorbaDef_var gui = TA_Base_Core::IManagedGUICorbaDef::_narrow(p_managedApp);
            matchingAppIter->second->setManagedGui(gui);
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Managed application registering is NULL.");
        }
    }
    catch (const CORBA::Exception& ex)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
            "Managed application registering cannot be narrowed to a GUI.");
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "registerManagedApplication");
}

void ProcessManager::poll()
{
}

TA_Base_Core::RunParamSeq* ProcessManager::getParams(TA_Base_Core::ProcessId p_processId, const char* hostname)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "getParams");
    using TA_Base_Core::RunParams;
    using TA_Base_Core::RunParamSeq;

    // Create an empty sequence
    RunParamSeq* params = new RunParamSeq;

    if (params == NULL)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on RunParamSeq returned NULL");
        return NULL;
    }

    RunParams::ParamVector paramVector;
    RunParams::getInstance().getAll(paramVector);
    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Parameters passed to application with pid %lu:", p_processId);
    int insertPosition = 0;

    using TA_Base_App::TA_ControlStation::DEBUG_LOG_DIR;
    using TA_Base_App::TA_ControlStation::HOST_NAMES;
    using TA_Base_App::TA_ControlStation::PORT_NUMBER;
    using TA_Base_App::TA_ControlStation::STRINGS_EQUIVALENT;

    for (unsigned int i = 0; i < paramVector.size(); ++i)
    {
        if ((STRINGS_EQUIVALENT == paramVector[i].name.compare(RPARAM_ENTITYNAME)) ||
                (STRINGS_EQUIVALENT == paramVector[i].name.compare(RPARAM_DEBUGFILE)) ||
                (STRINGS_EQUIVALENT == paramVector[i].name.compare(RPARAM_MGRPORT)) ||
                (STRINGS_EQUIVALENT == paramVector[i].name.compare(DEBUG_LOG_DIR)) ||
                (STRINGS_EQUIVALENT == paramVector[i].name.compare(HOST_NAMES)) ||
                (STRINGS_EQUIVALENT == paramVector[i].name.compare(PORT_NUMBER)) ||
                (STRINGS_EQUIVALENT == paramVector[i].name.compare(RPARAM_PROGNAME)) ||
                (STRINGS_EQUIVALENT == paramVector[i].name.compare(RPARAM_HELPFILE)) // don't pass the help string of the control station as it's context sensitive
           )
        {
            // Ignore these parameters as they are Control Station specific
            continue;
        }

        if (!paramVector[i].name.empty())
        {
            LOGMORE(SourceInfo, "%s=%s", paramVector[i].name.c_str(), paramVector[i].value.c_str());

            params->length(insertPosition + 1);
            (*params)[insertPosition].name =  CORBA::string_dup(paramVector[i].name.c_str());
            (*params)[insertPosition].value = CORBA::string_dup(paramVector[i].value.c_str());
            ++insertPosition;
        }
    }

    if (p_processId == 0)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Anonymous process fetching RunParams.  Returning the common run params incl debug log dir");

        params->length(insertPosition + 1);
        (*params)[insertPosition].name = CORBA::string_dup(DEBUG_LOG_DIR.c_str());
        (*params)[insertPosition].value = CORBA::string_dup(RunParams::getInstance().get(DEBUG_LOG_DIR.c_str()).c_str());
        ++insertPosition;

        LOG(SourceInfo, DebugUtil::FunctionExit, "getParams");
        return params;
    }

    // If we know about this application then populate the list with some parameters
    RunningApplicationMap::iterator iter = m_runningApplications.find(p_processId);

    if (iter != m_runningApplications.end())
    {
        // Set up the debug file for the application from the DebugPath we have
        std::string debugPath = RunParams::getInstance().get(DEBUG_LOG_DIR.c_str());
        std::string entityName = iter->second->getEntityName();

        if ((!debugPath.empty()) && (!entityName.empty()))
        {
            // set the RPARAM_DEBUGPIDFILENAMES so that it doesn't try to name the files without the PID
            if (!iter->second->areInstancesLimited())
            {
                params->length(insertPosition + 1);
                (*params)[insertPosition].name = CORBA::string_dup(RPARAM_DEBUGPIDFILENAMES);
                (*params)[insertPosition].value = CORBA::string_dup(USE_DEBUGPIDFILENAMES.c_str());
                LOGMORE(SourceInfo, "%s=%s", RPARAM_DEBUGPIDFILENAMES, USE_DEBUGPIDFILENAMES.c_str());
                ++insertPosition;
            }

            // Check if there is a slash at the end of the debug path specified. If there isn't then add one
            if ((debugPath[debugPath.length() - 1] != '/') &&
                    (debugPath[ debugPath.length() - 1] != '\\'))
            {
                debugPath += "\\";
            }

            debugPath += TA_Base_App::TA_ControlStation::LOG_PRE_STRING;
            debugPath += entityName;
            debugPath += TA_Base_App::TA_ControlStation::LOG_POST_STRING;
            params->length(insertPosition + 1);
            (*params)[insertPosition].name = CORBA::string_dup(RPARAM_DEBUGFILE);
            (*params)[insertPosition].value = CORBA::string_dup(debugPath.c_str());
            LOGMORE(SourceInfo, "%s=%s", RPARAM_DEBUGFILE, debugPath.c_str());
            ++insertPosition;
        }

        std::string appName = iter->second->getApplicationFullName();

        if (!appName.empty())
        {
            params->length(insertPosition + 1);
            (*params)[insertPosition].name = CORBA::string_dup(RPARAM_APPNAME);
            (*params)[insertPosition].value = CORBA::string_dup(appName.c_str());
            LOGMORE(SourceInfo, "%s=%s", RPARAM_APPNAME, appName.c_str());
            ++insertPosition;
        }

        // if help file is set and not empty, then use it
        std::string helpFile = iter->second->getHelpFile();

        if (!helpFile.empty())
        {
            params->length(insertPosition + 1);
            (*params)[insertPosition].name = CORBA::string_dup(RPARAM_HELPFILE);
            (*params)[insertPosition].value = CORBA::string_dup(helpFile.c_str());
            LOGMORE(SourceInfo, "%s=%s", RPARAM_HELPFILE, helpFile.c_str());
            ++insertPosition;
        }
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "getParams");
    return params;
}

void ProcessManager::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "processUpdate");

    ThreadGuard guard(m_classLock);

    if (updateEvent.getType() != TA_Base_Core::APPLICATION)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Type of configuration update does not match the update we are interested in. Ignoring");
        LOG(SourceInfo, DebugUtil::FunctionExit, "processUpdate");
        return;
    }

    unsigned long key = updateEvent.getKey();
    ApplicationMap::iterator appIter = m_applications.find(key);

    if ((updateEvent.getModifications() != TA_Base_Core::Create) && (appIter == m_applications.end()))
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Key of configuration update does not match the update we are interested in. Ignoring");
        LOG(SourceInfo, DebugUtil::FunctionExit, "processUpdate");
        return;
    }

    switch (updateEvent.getModifications())
    {
    case TA_Base_Core::Create:
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Received a 'Create' update");
        TA_Base_Core::IGui* gui = TA_Base_Core::GuiAccessFactory::getInstance().getGui(key);
        m_applications.insert(ApplicationMap::value_type(key, new Application(gui)));
        break;
    }

    case TA_Base_Core::Delete:
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Received a 'Delete' update");
        m_applications.erase(appIter);
        break;
    }

    case TA_Base_Core::Update:
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Received a 'Update' update");

        if (updateEvent.getChangedParams().empty())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received a configuration update with an empty list of changes. Ignoring");
        }
        else
        {
            // mark the Gui as dirty so that it'll fetch from database next time it's called
            appIter->second->getGuiDatabaseEntry().invalidate();
        }

        break;
    }

    default:
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Received unknown update");
        break;
    }
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "processUpdate");
}

void ProcessManager::giveApplicationFocus(unsigned long pid)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "giveApplicationFocus");

    ThreadGuard guard(m_classLock);

    RunningApplicationMap::iterator matchingAppIter = m_runningApplications.find(pid);

    if (matchingAppIter == m_runningApplications.end())
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Application cannot be given focus as it cannot be found: %lu", pid);
        LOG(SourceInfo, DebugUtil::FunctionExit, "giveApplicationFocus");
        return;
    }

    try
    {
        matchingAppIter->second->changeApplicationFocus(TA_Base_Core::GetFocus);
    }
    catch (ApplicationException&)
    {
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "ApplicationException", "Application could not be given focus.");
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "giveApplicationFocus");
}

void ProcessManager::terminateAllApplications(TA_Base_App::TA_ControlStation::ETerminateMode terminateMode/*=TA_Base_App::TA_ControlStation::LOGOUT*/)
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "terminateAllApplications");
    bool exitAll = (terminateMode == TA_Base_App::TA_ControlStation::EXIT);

    try
    {
        {
            ThreadGuard guard(m_classLock);

            RunningApplicationMap::iterator iter = m_runningApplications.begin();
            RunningApplicationMap::iterator iterEnd = m_runningApplications.end();

            while (iter != iterEnd)
            {
                if ((iter->second != NULL) && (exitAll || iter->second->shouldTerminateOnLogout()))
                {
                    try
                    {
                        iter->second->terminate();
                    }
                    catch (...)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "General exception",
                                            "An exception was caught while attempting to terminate an application");
                    }
                }

                // Increment the iterator first incase this item is deleted from the map before
                // we can move onto the next item.
                ++iter;
            }
        }

        // Wait to check all applications have terminated.
        if (!m_runningApplications.empty())
        {
            for (int waitCounter = 0; waitCounter < 3; waitCounter++)
            {
                bool needSleep = false;
                {
                    // just guard the class here when it's doing the check before it starts sleeping
                    ThreadGuard guard(m_classLock);

                    if (m_runningApplications.empty())
                    {
                        break;
                    }

                    for (RunningApplicationMap::iterator iter = m_runningApplications.begin(); iter != m_runningApplications.end(); iter++)
                    {
                        // if the element is supposed to terminate on logout but it's
                        // still in the running applications map, then wait
                        if ((iter->second != NULL) && (exitAll || iter->second->shouldTerminateOnLogout()))
                        {
                            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Some applications haven't terminated, waiting 1s for them to terminate");
                            ++waitCounter;
                            needSleep = true;
                            break;
                        }
                    }
                }

                if (needSleep)
                {
                    TA_Base_Core::Thread::sleep(1000);
                }
            }
        }

        // Now we try a nasty method of terminating if the applications
        // that are supposed to be terminated are still running
        if (!m_runningApplications.empty())
        {
            ThreadGuard guard(m_classLock);
            RunningApplicationMap::iterator iter = m_runningApplications.begin();

            while (iter != m_runningApplications.end())
            {
                if ((iter->second != NULL) && (exitAll || iter->second->shouldTerminateOnLogout()))
                {
                    try
                    {
                        iter->second->kill();
                        delete (iter->second);
                        iter->second = NULL;
                        m_runningApplications.erase(iter++);
                    }
                    catch (...)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "General exception",
                                            "An exception was caught while attempting to terminate an application");
                    }
                }
                else
                {
                    ++iter;
                }
            }
        }
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "General exception",
                            "An exception was caught while attempting to terminate applications");
    }

    // The applications won't terminate if they're showing popup messages, the only way of getting rid of
    // them is via the job object
    if (m_sessionJobObject != NULL)
    {
        // terminate all external processes first
        terminateExternalApplications(m_appsToTerminateOnLogout, m_sessionJobObject);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm, "Terminating session job object");
        TerminateJobObject(m_sessionJobObject, 0);
        m_sessionJobObject = NULL;
    }

    if (exitAll && (m_globalJobObject != NULL))
    {
        // terminate any external applications that required to be terminated
        terminateExternalApplications(m_appsToTerminate, m_globalJobObject);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm, "Terminating global job object");
        TerminateJobObject(m_globalJobObject, 0);
        m_globalJobObject = NULL;
    }

    // if there's still some apps running, they must be the ones that don't terminate on logout
    if (!m_runningApplications.empty())
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Applications that are still running:");
        RunningApplicationMap::iterator iter = m_runningApplications.begin();

        while (iter != m_runningApplications.end())
        {
            try
            {
                LOGMORE(SourceInfo, "%s", iter->second->getApplicationName().c_str());
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "General exception",
                                    "An exception was caught while attempting to get application name");
            }

            iter++;
        }
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "terminateAllApplications");
}

std::map<unsigned long, std::string> ProcessManager::getRunningApplications(bool areGettingJustVisible /*=true*/)
{
    ThreadGuard guard(m_classLock);

    std::map<unsigned long, std::string> mapOfRunningApps;

    RunningApplicationMap::iterator iter = m_runningApplications.begin();
    RunningApplicationMap::iterator iterEnd = m_runningApplications.end();

    while (iter != iterEnd)
    {
        if (!areGettingJustVisible || (iter->second->isVisibleToUserAsRunning()))
        {
            try
            {
                mapOfRunningApps.insert(std::map<unsigned long, std::string>::value_type(
                                            iter->first,
                                            iter->second->getApplicationFullName()
                                        )
                                       );
            }
            catch (DatabaseException&)
            {
                LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while retrieving the list of running applications. Insert application as 'Unknown' since we cannot retrieve it's details");
                mapOfRunningApps.insert(std::map<unsigned long, std::string>::value_type(
                                            iter->first,
                                            "Unknown"
                                        )
                                       );
            }
            catch (DataException&)
            {
                LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while retrieving the list of running applications. Insert application as 'Unknown' since we cannot retrieve it's details");
                mapOfRunningApps.insert(std::map<unsigned long, std::string>::value_type(
                                            iter->first,
                                            "Unknown"
                                        )
                                       );
            }
        }

        ++iter;
    }

    return mapOfRunningApps;
}

std::vector<ApplicationProperties> ProcessManager::getApplications()
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "getApplications");

    ThreadGuard guard(m_classLock);

    std::vector<ApplicationProperties> applist;

    ApplicationMap::iterator iter = m_applications.begin();
    ApplicationMap::iterator iterEnd = m_applications.end();

    while (iter != iterEnd)
    {
        try
        {
            if (iter->second->isIconVisible())
            {
                ApplicationProperties app;
                app.key = iter->second->getGuiDatabaseEntry().getKey();
                app.name = iter->second->getFullName();
                app.isAccessible = iter->second->isAccessible();
                applist.push_back(app);
            }
        }
        catch (DatabaseException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while retrieving the list of applications. Ignore this application since we cannot retrieve it's details");
        }
        catch (DataException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while retrieving the list of applications. Ignore this application since we cannot retrieve it's details");
        }

        ++iter;
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "getApplications");
    return applist;
}

void ProcessManager::loadApplications()
{
    LOG(SourceInfo, DebugUtil::FunctionEntry, "loadApplications");

    using TA_Base_Core::GuiAccessFactory;
    using TA_Base_Core::IGui;

    // Retrieve the list of GUIs.
    std::vector<IGui*> guis = GuiAccessFactory::getInstance().getAllGuis();

    std::vector<IGui*>::iterator guiIter;

    for (guiIter = guis.begin(); guiIter < guis.end(); ++guiIter)
    {
        // Application created will delete the IGui when it is destroyed
        m_applications.insert(ApplicationMap::value_type((*guiIter)->getKey(), new Application(*guiIter)));
    }

    LOG(SourceInfo, DebugUtil::FunctionExit, "loadApplications");
}

//
void ProcessManager::setupDbMonitors(int& pollPeriod)
{
    TA_ASSERT(pollPeriod != 0, "Poll period can not 0!");

    if (!  TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE))
    {
        throw TA_Base_Core::FatalConfigurationException("db-connection-file not set");
    }

    std::string filename = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);

    // can throw DbConnectionNoFile
    // no point in catching it cause there is nothing we can do with it
    TA_Base_Core::DbConnectionStrings connectStrings(filename);
    TA_Base_Core::DatabaseConnectionMap dbMap = connectStrings.getConnectionMap();

    TA_Base_Core::DatabaseConnectionMap::iterator it = dbMap.begin();

    while (it != dbMap.end())
    {
        std::vector<TA_Base_Core::DataConnection>::iterator dcIt = it->second.begin();

        while (dcIt != it->second.end())
        {
            // set moniter process pointer to null
            // will fill it in just below
            m_moniteredDatabases.insert(std::make_pair(dcIt->second, (TA_Base_Bus::SimpleDbStatusChecker*)0));
            dcIt++;
        }

        it++;
    }

    // get the runtime parameter of central db and monitored db
    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_MONITOREDDATABASE))
    {
        std::string DbConnectionInfo = TA_Base_Core::RunParams::getInstance().get(RPARAM_MONITOREDDATABASE);
        m_moniteredDatabases.insert(std::make_pair(DbConnectionInfo, (TA_Base_Bus::SimpleDbStatusChecker*)0));

        std::vector<std::string> connectInfo;
        SimpleDb::ParseConnectStr(DbConnectionInfo, connectInfo);

        m_monitoredDbName = connectInfo[0];
    }

    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_CENTRALDATABASE))
    {
        std::string DbConnectionInfo = TA_Base_Core::RunParams::getInstance().get(RPARAM_CENTRALDATABASE);
        m_moniteredDatabases.insert(std::make_pair(DbConnectionInfo, (TA_Base_Bus::SimpleDbStatusChecker*)0));

        std::vector<std::string> connectInfo;
        SimpleDb::ParseConnectStr(DbConnectionInfo, connectInfo);

        m_centralDbName = connectInfo[0];
    }

    // wait at least 5 seconds so db is not overloaded
    int pollTime = pollPeriod < 5 ? 5 : pollPeriod;

    // create db moniter threads (will start them later)
    MoniteredDbs::iterator dbIt = m_moniteredDatabases.begin();

    while (dbIt != m_moniteredDatabases.end())
    {
        dbIt->second = new TA_Base_Bus::SimpleDbStatusChecker(dbIt->first, pollTime, this);

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Add database %s to the monitored database map", dbIt->first.c_str());

        dbIt++;
    }
}
// notifyStatusChanged
//
void ProcessManager::notifyStatusChange(const std::string& DbConnectionInfo, TA_Base_Bus::DatabaseStatusCorbaDef::EState newStatus)
{
    std::string dbStatus;
    std::vector<std::string> connectInfo;
    SimpleDb::ParseConnectStr(DbConnectionInfo, connectInfo);
    std::string dbName(RPARAM_DBPREFIX);

    // dbName += connectInfo[0];
    if (!connectInfo[CON_STR_Hostname].empty())
        dbName += (connectInfo[CON_STR_SchemaName] + "@" + connectInfo[CON_STR_Hostname]);
    else
        dbName += connectInfo[CON_STR_SchemaName];

    bool isOfflineNotification = false;

    switch (newStatus)
    {
    case TA_Base_Bus::DatabaseStatusCorbaDef::statusUnknown:
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Notify applications that monitored db %s status is UNKNOWN", dbName.c_str());
        dbStatus = RPARAM_DBUNKNOWN;
        break;

    case TA_Base_Bus::DatabaseStatusCorbaDef::statusAvailable:
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Notify applications that monitored db %s status is ONLINE", dbName.c_str());
        dbStatus = RPARAM_DBONLINE;
        break;

    case TA_Base_Bus::DatabaseStatusCorbaDef::statusUnAvailable:
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Notify applications that monitored db %s status is OFFLINE", dbName.c_str());
        dbStatus = RPARAM_DBOFFLINE;
        isOfflineNotification = true;

        break;

    default:
        TA_ASSERT(false, "Unknown or new TA_Base_Bus::DatabaseStatusCorbaDef::State");
    }

    std::string rparamDbName(dbName);
    TA_Base_Core::RunParams::getInstance().set(dbName.c_str(), dbStatus.c_str());

    // now let everyone know about the change
    //TD18257, jianghp, to fix the performance of showing manager application
    //here, if use guard, the control station will deadlock after fixing the db failover.
    //ThreadGuard guard(m_classLock);
    RunningApplication* process;
    TA_Base_Core::RunParam runParam;
    runParam.name = dbName.c_str();
    runParam.value = dbStatus.c_str();
    TA_Base_Core::RunParamSeq_var runParamSeq = new TA_Base_Core::RunParamSeq;
    runParamSeq->length(1);
    runParamSeq[0] = runParam;
    RunningApplicationMap runningApplications;

    {
        //ThreadGuard guard(m_classLock);
        runningApplications = m_runningApplications;
    }

    for (RunningApplicationMap::iterator iter = runningApplications.begin(); iter != runningApplications.end(); ++iter)
    {
        process = iter->second;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Runtime param of process %s run param %s = %s is being updated", process->getEntityName().c_str(), dbName.c_str(), dbStatus.c_str());
        typedef void (RunningApplication::*PassNewParameters)(TA_Base_Core::RunParamSeq_var); // disambiguate overload function
        TA_Base_Core::IWorkItemPtr item(new TA_Base_Core::SimpleWorkItem(boost::bind((PassNewParameters)&RunningApplication::passNewParameters, process, runParamSeq)));
        m_threadPool->queueWorkItem(item);
    }

    //monitored db online and another db online
    if (m_isStartup == true && newStatus == TA_Base_Bus::DatabaseStatusCorbaDef::statusAvailable)
    {
        m_isStartup = false;

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Sending signal to continue system controller startup of processes...");
        m_initialisedDBStatusMonitor.post();
    }
}

void ProcessManager::updateSessionId(std::string& sessionId)
{
    FUNCTION_ENTRY("updateSessionId");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Session has been changed to %s !!!", sessionId.c_str());

    RunningApplicationMap runningApplications;

    {
        ThreadGuard guard(m_classLock);
        runningApplications = m_runningApplications;
    }

    for (RunningApplicationMap::iterator iter = runningApplications.begin(); iter != runningApplications.end(); ++iter)
    {
        TA_Base_Core::IWorkItemPtr item(new TA_Base_Core::SimpleWorkItem(boost::bind(&RunningApplication::updateSessionId, iter->second, sessionId)));
        m_threadPool->queueWorkItem(item);
    }

    FUNCTION_EXIT;
}

void ProcessManager::dutyChanged()
{
    FUNCTION_ENTRY("dutyChanged");

    RunningApplicationMap runningApplications;

    {
        ThreadGuard guard(m_classLock);
        runningApplications = m_runningApplications;
    }

    for (RunningApplicationMap::iterator iter = runningApplications.begin(); iter != runningApplications.end(); ++iter)
    {
        TA_Base_Core::IWorkItemPtr item(new TA_Base_Core::SimpleWorkItem(boost::bind(&RunningApplication::dutyChanged, iter->second)));
        m_threadPool->queueWorkItem(item);
    }

    FUNCTION_EXIT;
}

void ProcessManager::terminateExternalApplications()
{
    // terminate all external applications that's supposed to be terminated upon logout and exiting from
    // control station
    terminateExternalApplications(m_appsToTerminateOnLogout, m_sessionJobObject);
    terminateExternalApplications(m_appsToTerminate, m_globalJobObject);
}

void ProcessManager::terminateExternalApplications(const std::vector<std::string>& appsToTerminate, HANDLE jobObject)
{
    FUNCTION_ENTRY("terminateExternalApplications");

    if (appsToTerminate.empty())
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Got no external process to terminate");
        FUNCTION_EXIT;
        return;
    }

    // To kill applications, will need to enumerate the processes running on the machine
    CEnumProcess enumeration;
    CEnumProcess::CProcessEntry entry;

    TerminationResults results;

    // And then search that list for the name of the process we need
    // Step through the process list, see if we can find one we're interested in.
    BOOL isMoreProcesses = enumeration.GetProcessFirst(&entry);

    while (isMoreProcesses)
    {
        std::string filename(entry.lpFilename);
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

        std::vector<std::string>::const_iterator appIter = std::find(appsToTerminate.begin(), appsToTerminate.end(), filename);

        if (appIter != appsToTerminate.end())
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Terminating %s (%d)", filename.c_str(), entry.dwPID);
            // if this process is in our list of applications to terminate, then we need to add it to our job object
            HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.dwPID);
            TerminateProcess(processHandle, 0);

            // just assign it to the job object so that if it is not terminated successfully, the
            // job object will get rid of it eventually
            AssignProcessToJobObject(jobObject, processHandle);
        }

        isMoreProcesses = enumeration.GetProcessNext(&entry);
    }

    FUNCTION_EXIT;
}

std::vector<std::string> ProcessManager::getTerminationList(std::string commaList)
{
    // List of applications to kill when the control station exits
    std::vector<std::string> applicationsToTerminate;

    // Create iterator counters
    unsigned int current = 0;
    unsigned int previous = 0;

    std::transform(commaList.begin(), commaList.end(), commaList.begin(), ::tolower);

    // Add a comma to the end of the string, so the last item will be picked up
    commaList += ",";

    // Search for the first comma
    current = commaList.find_first_of(",", 0);

    // Loop until there are no more commas
    while (current != std::string::npos)
    {
        // If previous is equal to current, then there were no items on the string
        if (previous != current)
        {
            // Otherwise, there was something. Want to retrieve that something, which is the string starting at
            // "previous" (the first character after the last comma found, or 0 if this is the first iteration)
            // and "current" (the most recently found comma) - "previous" long.
            applicationsToTerminate.push_back(commaList.substr(previous, (current - previous)));
        }

        // Move previous to represent the character after the most recently found comma. That is, the first character
        // of the next string to be extracted (if any).
        previous = current + 1;

        // And attempt to find the next comma
        current = commaList.find_first_of(",", previous);
    }

    return applicationsToTerminate;
}

void ProcessManager::setApplicationsToTerminate(const std::string& appName)
{
    m_appsToTerminate = getTerminationList(appName);
}

void ProcessManager::setApplicationsToTerminateOnLogout(const std::string& appName)
{
    m_appsToTerminateOnLogout = getTerminationList(appName);
}

bool ProcessManager::resourceIsPermitted(unsigned long resourseKey)
{
    TA_Base_Bus::ResourceList::iterator iter = permitedResourceList.begin();
    TA_Base_Bus::ResourceList::iterator iterEnd = permitedResourceList.end();

    if (iter == iterEnd)
        return false;
    else
    {
        while (iter != iterEnd)
        {
            if ((*iter) == resourseKey)
                return true;

            iter++;
        }

        return false;
    }
}
