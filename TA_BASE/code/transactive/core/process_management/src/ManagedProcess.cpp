/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/ManagedProcess.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/03/17 15:02:39 $
  * Last modified by:  $Author: limin.zhu $
  *
  * <description>
  *
  */

#include "core/process_management/src/ManagedProcess.h"
#include "core/process_management/src/IManagedProcessUser.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/process_management/src/ProcessIdResolver.h"
#include "core/process_management/src/PollListener.h"

#include "core/utilities/src/StdPubUtility.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/ManagedProcessException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"
#include "core/exceptions/src/UtilitiesException.h"
#include "core/message/src/MessageSubscriptionManager.h"

#include <sstream>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::CorbaUtil;
using TA_Base_Core::ManagedProcessException;
using TA_Base_Core::UtilitiesException;

namespace TA_Base_Core
{
    //
    // Class constructor
    //
    ManagedProcess::ManagedProcess(IManagedProcessUser* const user, PollListener* pollListener)
        : m_ProcessManager(IProcessManagerCorbaDef::_nil()),
          m_terminationFromManager(false),
          m_monitoringThread(NULL),
          m_managedProcessUser(user),
          m_pollListener(pollListener)
    {
        bool hasRegistered = false;

        try
        {
            if (pollListener == NULL)
            {
                // Use the default.
                m_pollListener = new PollListener();
            }

            activateServant();
            locateProcessManager();
            registerWithProcessManager();
            hasRegistered = true;
            retrieveSystemParameters();
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Perform necessary clean up before re-throwing the exception");

            if (hasRegistered)
            {
                notifyTerminating(TA_Base_Core::InitError, "Exception raised while initialising ManagedProcess");
            }

            deactivateServant();
            throw;
        }
    }

    //
    // Destructor
    //
    ManagedProcess::~ManagedProcess()
    {
        if (m_monitoringThread != NULL)
        {
            m_monitoringThread->terminateAndWait();
            delete m_monitoringThread;
            m_monitoringThread = 0;
        }

        delete m_pollListener;
        m_pollListener = 0;
    }

    //
    // startMonitoringThread
    //
    void ManagedProcess::startMonitoringThread(unsigned int p_pollingPeriodInSeconds)
    {
        if (RunParams::getInstance().isSet("NoProcessManager"))
        {
            return;
        }

        m_monitoringThread = new MonitoringThread(p_pollingPeriodInSeconds);
        m_monitoringThread->registerForMessage(this, m_ProcessManager);
    }

    //
    // terminate
    //
    void ManagedProcess::terminate()
    {
        m_terminationFromManager = true;
        TA_Base_Core::RunParams::getInstance().set(RPARAM_PROCESSSTATUS, RPARAM_TERMINATING);
    }

    //
    // setParam
    //
#if defined( SOLARIS )
    #define _stricmp TA_Base_Core::CStdPubUtility::stricmp
#endif
    void ManagedProcess::setParam(const TA_Base_Core::RunParam& param)
    {
        if (_stricmp("corba", param.name.in()) == 0)
        {
            TA_Base_Core::CorbaUtil::getInstance().setCorbaParam(param.value.in());
        }
        else
        {
            TA_Base_Core::RunParams::getInstance().set(param.name.in(), param.value.in());
        }
    }

    //
    // getParams
    //
    TA_Base_Core::RunParamSeq* ManagedProcess::getParams()
    {
        TA_Base_Core::RunParams::ParamVector vec;
        TA_Base_Core::RunParams::getInstance().getAll(vec);

        TA_Base_Core::RunParamSeq* seq = new TA_Base_Core::RunParamSeq();
        seq->length(vec.size());

        for (unsigned int i = 0; i < vec.size(); i++)
        {
            (*seq)[i].name = vec[i].name.c_str();
            (*seq)[i].value = vec[i].value.c_str();
        }

        return seq;
    }

    //
    // poll
    //
    void ManagedProcess::poll()
    {
        m_pollListener->poll();
    }

    //
    // registerManagedApplication
    //
    void ManagedProcess::registerManagedApplication(IManagedApplicationCorbaDef_ptr app)
    {
        if (RunParams::getInstance().isSet("NoProcessManager"))
        {
            return;
        }

        if (CORBA::is_nil(m_ProcessManager))
        {
            TA_THROW(ManagedProcessException("Failed to register the Managed Application"));
        }

        try
        {
            m_ProcessManager->registerManagedApplication(app, m_managedProcessID);
        }
        catch (...)
        {
            TA_THROW(ManagedProcessException("Failed to register the Managed Application"));
        }
    }

    //
    // notifyTerminating
    //
    void ManagedProcess::notifyTerminating(EManagedProcessTerminateCode code, const char* msg)
    {
        if (RunParams::getInstance().isSet("NoProcessManager"))
        {
            return;
        }

        try
        {
            if (!CORBA::is_nil(m_ProcessManager))
            {
                m_ProcessManager->terminating(m_managedProcessID, code);
            }
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "Exception raised when notifying Process Manager of termination");
        }
    }

    //
    // changeState
    //
    void ManagedProcess::changeState(EProcessStatus processStatus)
    {
        try
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Changing agent state to %d", processStatus);

            if (!CORBA::is_nil(m_ProcessManager))
            {
                m_ProcessManager->statusChanged(processStatus, m_managedProcessID);
            }

            TA_Base_Core::RunParams::getInstance().set(RPARAM_PROCESSSTATUS, getProcessStatusString(processStatus).c_str());
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Exception raised when notifying Process Manager of state change");
        }
    }

    //
    // servicesRestarted
    //
    void ManagedProcess::servicesRestarted()
    {
    }

    //
    // onControllerFailure
    //
    void ManagedProcess::onControllerFailure()
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugFatal, "Process Manager not responding, shutting down");

        if (m_managedProcessUser != NULL)
        {
            m_managedProcessUser->onTerminate();
        }
    }

    //
    // haveBeenToldToTerminate
    //
    bool ManagedProcess::haveBeenToldToTerminate()
    {
        return m_terminationFromManager;
    }

    //
    // cleanup
    //
    void ManagedProcess::cleanup()
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Tell any registered managed process users that we are terminating");

        if (m_managedProcessUser != NULL)
        {
            m_managedProcessUser->onTerminate();
        }
    }

    //
    // locateProcessManager
    //
    void ManagedProcess::locateProcessManager()
    {
        if (RunParams::getInstance().isSet("NoProcessManager"))
        {
            return;
        }

        try
        {
            unsigned long port = strtoul(RunParams::getInstance().get(RPARAM_MGRPORT).c_str(), NULL, 0);

            if (port == 0)
            {
                const char* portstr = getenv(RPARAM_MGRPORT);

                // If the port was already assigned with an environment variable, use it
                if (portstr != NULL)
                {
                    port = strtoul(portstr, NULL, 0);
                }
            }

            if (port == 0)
            {
                // Port was not already assigned, use our default
                port = DEFAULT_MGR_PORT;
            }

            std::ostringstream ior;
            ior << "corbaloc::localhost:" << port << "/ProcessManager";
            RunParams::getInstance().set("ProcessManagerIor", ior.str().c_str());
            CORBA::Object_var obj;
            obj = CorbaUtil::getInstance().stringToObject(ior.str());

            if (!CORBA::is_nil(obj))
            {
                m_ProcessManager = IProcessManagerCorbaDef::_narrow(obj);
            }

            if (CORBA::is_nil(m_ProcessManager))
            {
                TA_THROW(ManagedProcessException("Failed to narrow to the Process Manager"));
            }
        }
        catch (const ManagedProcessException&)
        {
            throw; // Ensure we are only throwing ManagedProcessExceptions
        }
        catch (...)
        {
            // We are not expecting to catch anything other than ManagedProcessExceptions
            // If any unexpected exceptions are raised (eg CORBA::Exception), we re-throw
            // them as ManagedProcessExceptions.
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                "Caught an unknown exception, throwing a ManagedProcessException in its place");
            TA_THROW(ManagedProcessException("Could not locate Process Manager"));
        }
    }

    //
    // registerWithProcessManager
    //
    void ManagedProcess::registerWithProcessManager()
    {
        if (RunParams::getInstance().isSet("NoProcessManager"))
        {
            return;
        }

        try
        {
            std::string app     = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPTYPE);
            std::string entity  = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);

            // Get our process id
            m_managedProcessID = ProcessIdResolver::getProcessId();

            // Register the process manager
            m_ProcessManager->registerManagedProcess(_this(),
                    m_managedProcessID,
                    strtoul(app.c_str(), NULL, 0),
                    entity.c_str());

            std::ostringstream managedProcessIDAsString;
            managedProcessIDAsString << m_managedProcessID;

            // Set the managed process id run param
            RunParams::getInstance().set(RPARAM_MGDPROCID, managedProcessIDAsString.str().c_str());
        }
        catch (const ManagedProcessException&)
        {
            throw; // Ensure we are only throwing ManagedProcessExceptions
        }
        catch (const TA_Base_Core::ProcessRunningException&)
        {
            TA_THROW(TA_Base_Core::ProcessAlreadyRunningException());
        }
        catch (...)
        {
            // We are not expecting to catch anything other than ManagedProcessExceptions
            // If any unexpected exceptions are raised (eg CORBA::Exception), we re-throw
            // them as ManagedProcessExceptions.
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                "Caught an unknown exception, throwing a ManagedProcessException in its place");
            TA_THROW(ManagedProcessException("Failed to register with the process manager"));
        }
    }

    //
    // retrieveSystemParameters
    //
    void ManagedProcess::retrieveSystemParameters()
    {
        if (RunParams::getInstance().isSet("NoProcessManager"))
        {
            return;
        }

        try
        {
            // Get system parameters from Process Manager
            TA_Base_Core::RunParamSeq_var p = m_ProcessManager->getParams(m_managedProcessID, "");

            for (unsigned int i = 0; i < p->length(); i++)
            {
                if ((p[i].name.in())[0] == '\0')
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received nameless parameter from process manager");
                }
                else
                {
                    // For each parameter - if not already set, set it
                    if (!RunParams::getInstance().isSet(p[i].name.in()))
                    {
                        RunParams::getInstance().set(p[i].name.in(), p[i].value.in());
                    }
                    else
                    {
                        if (RunParams::getInstance().get(p[i].name.in()).empty())
                        {
                            RunParams::getInstance().set(p[i].name.in(), p[i].value.in());
                        }
                    }
                }
            }
        }
        catch (...)
        {
            // We are not expecting any exceptions to be thrown
            // If any unexpected exceptions are raised (eg CORBA::Exception), we re-throw
            // them as ManagedProcessExceptions.
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                "Caught an unknown exception, throwing a ManagedProcessException in its place");
            TA_THROW(ManagedProcessException("Failed to retrieve system parameters from the process manager"));
        }
    }

    const std::string& ManagedProcess::getProcessStatusString(EProcessStatus status)
    {
        static std::map<EProcessStatus, std::string> map;

        if (map.empty())
        {
            map[Unstarted]        = "Unstarted";
            map[Startup]          = "Startup";
            map[GoingToControl]   = "GoingToControl";
            map[GoingToMonitor]   = "GoingToMonitor";
            map[RunningControl]   = "RunningControl";
            map[RunningMonitor]   = "RunningMonitor";
            map[Terminating]      = "Terminating";
            map[NotRunning]       = "NotRunning";
            map[Stopped]          = "Stopped";
            map[RunningStandby]   = "RunningStandby";
            map[GoingToStandby]   = "GoingToStandby";
        }

        return map[status];
    }
}
