/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/SignalEventHandler.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is used to catch SIGCHLD signals under
  * Solaris to deal with a process dying. It also catches
  * SIGTERM (Solaris) and SIGINT (Windows & Solaris) to
  * allow the System Controller to die nicely.
  *
  */

#include "SignalEventHandler.h"
#include "ProcessManager.h"
#include "SystemController.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/SystemControllerException.h"

using TA_Base_Core::DebugUtil;

#include <signal.h>

#ifndef WIN32
	#include <pthread.h>
    #include <unistd.h>
	#include <wait.h>		// For waitid
#endif // #ifndef WIN32


namespace TA_Base_App
{
	// Static definitions
	SystemController* SignalEventHandler::s_systemController = NULL;
	unsigned long SignalEventHandler::s_pid = 0;

    //
    // Constructor
    //
    SignalEventHandler::SignalEventHandler(SystemController* systemController)
    {

		s_systemController = systemController;

		s_pid = s_systemController->getProcessId();

#ifdef WIN32
        if (signal(SIGINT, handleSignal) == SIG_ERR)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Could not instantiate SIGINT Handler");
        }
#else
		
		// Create, temporarily, a separate signal handler for SIGCHLD signals.
		// This is required under Solaris.
		// sigset(SIGCHLD, dummyHandler);
		struct sigaction old_action;
		memset(&old_action, 0, sizeof(old_action));
		if (0 != sigaction(SIGCHLD, NULL, &old_action))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get inherited signal action handler of SIGCHLD failed!");
		}
		if (SIG_IGN != old_action.sa_handler)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "set a dummy handler to signal SIGCHLD...");
			sigset(SIGCHLD, dummyHandler);
		}
		
		// Create a signal mask. This will stop all threads but this one from
		// catching signals.
		sigemptyset(&m_signalSet);
		sigaddset(&m_signalSet, SIGCHLD);
		sigaddset(&m_signalSet, SIGTERM);
		sigaddset(&m_signalSet, SIGINT);
		if (pthread_sigmask(SIG_BLOCK, &m_signalSet, NULL) != 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error creating signal mask");
		}
#endif // #ifdef WIN32
    }
	
    //
    // Destructor
    //
    SignalEventHandler::~SignalEventHandler()
    {
    }

#ifdef WIN32

    //
    // handleSignal
    //
	void SignalEventHandler::handleSignal(int sigNum)
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Received SIGINT signal");
        
        // Reinstate the handler
        if (signal(SIGINT, handleSignal) == SIG_ERR)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Could not instantiate SIGINT Handler");
        }

        s_systemController->shutDownRequested();
	}

#else

    //
    // handleSignal
    //
    void SignalEventHandler::handleSignal(int sigNum, int pid, int errorCode)
    {		
        std::string entityName;

        switch(sigNum)
        {
        case SIGCHLD:

			// Prevent defunct processes
           	pid_t temp;
 			int status;
 			temp = waitpid(pid, &status, WNOHANG);

			try
			{
				// Convert the PID to an entity name.
				entityName = ProcessManager::getInstance().getEntityNameOfProcess(pid);

				LOG_GENERIC(SourceInfo, DebugUtil::DebugUtil::DebugError,
					"Received signal for process with pid %lu (%s). Reason: %s",
					pid, entityName.c_str(), convertCodeToString(errorCode).c_str());
			}
			catch(TA_Base_Core::SystemControllerException& ex)
			{
				// The PID is not known. Do nothing but log.
				LOG_GENERIC(SourceInfo, DebugUtil::DebugUtil::DebugWarn,
					"Received signal for process with unknown PID %lu.", pid);
				return;
			}

            // Tell the ProcessManager to inform the process object
            // that a signal was received.
            ProcessManager::getInstance().signalReceived(entityName);

            // Let the ProcessManager deal with it.
            ProcessManager::getInstance().handleProcessDeath(entityName);
            break;

        case SIGTERM:
			// if( pid == (int)s_pid)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
				   "Received SIGTERM signal from pid: %d, code: %d, reason: %s. Shutting down system controller...",
                   pid, errorCode, convertCodeToString(errorCode).c_str() );
				s_systemController->shutDownRequested();
			}
            // limin--
            // COMMENT NOTE: http://www.helsinki.fi/atk/unix/dec_manuals/DOC_40D/APS33DTE/DOCU_006.HTM
            //               The si_pid member contains the process identification (PID) of the sending process. 
            // else
            // {
            // 	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            // 		"Received SIGTERM signal. No need to handle. Please check process owner:group System controller must be root other.");
            // 
            // 	try
            // 	{
            // 		// Convert the PID to an entity name.
            // 		entityName = ProcessManager::getInstance().getEntityNameOfProcess(pid);
            // 
            // 		LOG_GENERIC(SourceInfo, DebugUtil::DebugUtil::DebugWarn,
            // 			"Received signal %lu for process with pid %lu (%s). Reason: %s, errorCode: %lu",
            // 				sigNum, pid, entityName.c_str(), convertCodeToString(errorCode).c_str(), errorCode);
            // 	}
            // 	catch(TA_Base_Core::SystemControllerException& ex)
            // 	{
            // 		// The PID is not known. Do nothing but log.
            // 		LOG_GENERIC(SourceInfo, DebugUtil::DebugUtil::DebugWarn,
            // 			"Received SIGTERM signal for process with unknown PID %lu.", pid);
            // 	}
            // }
			
            break;
        case SIGINT:
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Received SIGINT signal. No need to handle. Please check process owner:group System controller must be root other.");

			try
				{
					// Convert the PID to an entity name.
					entityName = ProcessManager::getInstance().getEntityNameOfProcess(pid);

					LOG_GENERIC(SourceInfo, DebugUtil::DebugUtil::DebugWarn,
						"Received signal %lu for process with pid %lu (%s). Reason: %s, errorCode: %lu",
							sigNum, pid, entityName.c_str(), convertCodeToString(errorCode).c_str(), errorCode);
				}
				catch(TA_Base_Core::SystemControllerException& ex)
				{
					// The PID is not known. Do nothing but log.
					LOG_GENERIC(SourceInfo, DebugUtil::DebugUtil::DebugWarn,
						"Received SIGINT signal for process with unknown PID %lu.", pid);
				}
            break;
        default:
            // Received signal we don't care about. Do nothing.
            break;
        }
    }

#endif // #ifdef WIN32


	//
	// run
	//
	void SignalEventHandler::run()
	{
#ifndef WIN32
		m_isRunning = true;
		siginfo_t info;

		while (m_isRunning)
		{
			// Wait for a signal to come in. This blocks until a signal is received.
			sigwaitinfo(&m_signalSet, &info);

			// Now handle the signal.
		    handleSignal(info.si_signo, info.si_pid, info.si_code);
		} 
#endif
	}

	//
	// terminate
	//
	void SignalEventHandler::terminate()
	{
		m_isRunning = false;
	}

	//
	// convertCodeToString
	//
	std::string SignalEventHandler::convertCodeToString(int code)
	{
#ifdef WIN32
		
		return "Unknown";

#else

		if (code <= 0)
		{
			return "Generated by user process";
		}

		switch (code)
		{
		case CLD_EXITED:
			return "Child has exited";
		case CLD_KILLED:
			return "Child was killed";
		case CLD_DUMPED:
			return "Child terminated abnormally";
		case CLD_TRAPPED:          
			return "Traced child has trapped";
		case CLD_STOPPED:          
			return "Child has stopped";
		case CLD_CONTINUED:
			return "Stopped child had continued";
		default:
			return "Unknown";
		}

#endif	// #ifdef WIN32
	}

} // namespace TA_Base_App
