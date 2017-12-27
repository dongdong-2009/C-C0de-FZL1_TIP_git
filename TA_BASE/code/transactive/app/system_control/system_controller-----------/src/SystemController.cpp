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
  * This is the starting point of the System Controller.
  * It starts the ProcessManager and SystemControllerMonitor
  * threads.
  *
  */

#include "SystemController.h"
#include "SystemControllerMonitor.h"
#include "ProcessManager.h"
#include "SystemControllerAdmin.h"
#include "SignalEventHandler.h"
#include "ProcessUtilities.h"
#include "ShutdownExecutor.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/SystemControllerException.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/PublicationManager.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/event_processing/src/RealTimeEventSender.h"
//#include "core/event_processing/src/ISessionHelper.h"

#include <iostream>
#include <sstream>

#if !defined( WIN32 )
    #include <unistd.h>
#else
    #include <process.h>
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    //
    // Constructor
    //
    SystemController::SystemController()
        : m_signalHandler(NULL), m_systemControllerAdmin(NULL), 
		m_port(0), m_shutdownSemaphore(0)
    {
        FUNCTION_ENTRY("Constructor");

        // Print out the System Controller's PID (useful for debugging).
        std::cout << "The System Controller's PID is " << getProcessId() << std::endl;
	    // Set our own nice value to -1	
	    ProcessUtilities::setPriority(getProcessId(), -1);

        if (!checkCommandLine())
        {
            exit(1);
        }

		m_signalHandler = new TA_Base_App::SignalEventHandler(this);

		m_signalHandler->start();

        TA_Base_Core::CorbaUtil::getInstance().initialise(m_port);
        TA_Base_Core::CorbaUtil::getInstance().activate();

        try
        {
            TA_Base_App::ProcessManager::getInstance().start();

            // Wait until the ProcessManager is fully initialised before
            // we start the SystemControllerMonitor (and hence the heartbeats).
            TA_Base_App::ProcessManager::getInstance().waitUntilInitialised();

            TA_Base_App::SystemControllerMonitor::getInstance().start();

            m_systemControllerAdmin = new SystemControllerAdmin(this);


            m_shutdownExecutor = new ShutdownExecutor;

        }
        catch(TA_Base_Core::SystemControllerException& ex)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "Fatal error: %s",
                            ex.what());
            exit(1);
        }

        FUNCTION_EXIT;
    }

    //
    // checkCommandLine
    //
    bool SystemController::checkCommandLine()
    {
        FUNCTION_ENTRY("checkCommandLine");
        bool isOkay = true;
		
        // Check to see if the debug level is specified.
        if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGLEVEL))
        {
            TA_Base_Core::DebugUtil::getInstance().setLevel(
                DebugUtil::getDebugLevelFromString(
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL).c_str()));
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                "Debug level not specified");
            isOkay = false;
        }           

        // Check to see if the debug file is specified.
        if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGFILE))
        {
            TA_Base_Core::DebugUtil::getInstance().setFile(
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE).c_str());
        }

        // Check to see if the max number of debug files is specified.
        if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGMAXFILES))
        {
            TA_Base_Core::DebugUtil::getInstance().setMaxFiles(
                    atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGMAXFILES).c_str()));
        }

        // Check to see if the max debug file size is specified.
        if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGFILEMAXSIZE))
        {
            TA_Base_Core::DebugUtil::getInstance().setMaxSize(
                    atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE).c_str()));
        }
        
        if(!isOkay)
        {
            return false;
        }

        // Pin the port that this process will use. We can set this from the
        // command line, or use the default.
        m_port = atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_MGRPORT).c_str());
        if (m_port == 0)
        {
            m_port = TA_Base_Core::DEFAULT_MGR_PORT;
			std::ostringstream portString;
			portString << m_port;
			TA_Base_Core::RunParams::getInstance().set(RPARAM_MGRPORT, portString.str().c_str());
        }

        if (m_port < 1024 || m_port > 65536)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                "Port number out of range: %i", m_port);
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }

    //
    // shutDown
    //
    void SystemController::shutdown()
    {
        FUNCTION_ENTRY("shutDown");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "System Controller has been asked to shutdown");

        TA_Base_App::SystemControllerMonitor::getInstance().terminateAndWait();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
            "SystemControllerMonitor terminated");
        
        TA_Base_App::SystemControllerMonitor::getInstance().cleanUp();
        TA_Base_App::SystemControllerMonitor::getInstance().deactivateServant();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "SystemControllerMonitor deactivated");

        TA_Base_App::ProcessManager::getInstance().terminateAndWait();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ProcessManager terminated");

        TA_Base_App::ProcessManager::getInstance().cleanUp();
        TA_Base_App::ProcessManager::getInstance().deactivateServant();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ProcessManager deactivated");

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

		//Clean up Real Time Event Sender Quene
		TA_Base_Core::RealTimeEventSenderInstance::instance()->cleanUp();

        TA_Base_Core::gDestroySubscriptionManager();
        TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();
		TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Alarm and message libraries cleaned up");

        TA_Base_Core::CorbaUtil::getInstance().stop();

        FUNCTION_EXIT;
    }

	//
	// waitUntilShutdown
	//
	void SystemController::waitUntilShutdown()
	{
		m_shutdownSemaphore.wait();
	}

	//
	// shutDownRequested
	//
	void SystemController::shutDownRequested()
	{
		m_shutdownSemaphore.post();
	}

    //
    // getProcessId
    //
    unsigned long SystemController::getProcessId()
    {
        #if !defined( WIN32 )
            pid_t pid = getpid();
            return (unsigned long)pid;
        #else
            return (unsigned long)_getpid();
        #endif
    }

    //
    // Destructor
    //
    SystemController::~SystemController()
    {
    }

} // namespace TA_Base_App
