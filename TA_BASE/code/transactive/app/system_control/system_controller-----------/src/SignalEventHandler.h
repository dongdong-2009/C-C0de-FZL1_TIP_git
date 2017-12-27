/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/SignalEventHandler.h $
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

#ifndef SIGNALEVENTHANDLER_H
#define SIGNALEVENTHANDLER_H

#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifndef WIN32
	#include <signal.h>
#endif

namespace TA_Base_App
{
	class SystemController;	// Forward declaration

    class SignalEventHandler : public TA_Base_Core::Thread
    {

    public:

        /**
          * Constructor
		  *
		  * @param systemController  A pointer to the parent SystemController object.
          */
        SignalEventHandler(SystemController* systemController);

        /**
          * Destructor
          */
        ~SignalEventHandler();

        /** 
          * handleSignal
          *
          * Called by the system when a signal has been
          * received.
          *
          * @param sigNum				  The signal number identifying the type of 
		  *								  signal.
          * @param pid (UNIX only)        The Process ID this signal corresponds to.
          * @param errorCode (UNIX only)  An error code returned with the signal.
          *
          */
#ifdef WIN32
		static void handleSignal(int sigNum);
#else
        static void handleSignal(int sigNum, int pid, int errorCode);
#endif // #ifdef WIN32

		/**
		  * run
		  *
		  * The starting point of the thread. Blocks until a signal comes
		  * in, and then it handles it.
		  */
		void run();

		/**
		  * terminate
		  *
		  * Terminates the thread.
		  */
		void terminate();

		/**
		  * dummyHandler
		  *
		  * This method should never get called. It's needed because Solaris
		  * requires that you add a handler for SIGCHLD's before adding SIGCHLD
		  * to the signal mask.
		  */
		static void dummyHandler(int sigNum)
		{
			//TA_ASSERT(false, "Signal handling has not been set up correctly.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "An unexpected signal received, sigNum: %d", sigNum);
		}


    private:
	    SignalEventHandler( const SignalEventHandler& SignalEventHandler);
	    SignalEventHandler& operator=(const SignalEventHandler&);

        /** 
          * convertCodeToString
          *
          * Converts a signal code returned by the waitid system call
		  * into a string for logging purposes.
          *
          * @param code  The signal code (the reason for the signal).
          *
          * @return  A string explaining the signal code.
          */
		static std::string convertCodeToString(int code);

		/**
		  * A pointer to the parent SystemController object.
		  */
		static SystemController* s_systemController;

#ifndef WIN32

		/**
		  * The set of signals to catch.
		  */
		sigset_t m_signalSet;
#endif

		/**
		  * True if this thread is running.
		  */
        bool volatile m_isRunning;//RenQiang++

		/**
		  * The process id of the system controller
		  */
		static unsigned long s_pid;


    };
} // namespace TA_Base_App

#endif // #ifndef SIGNALEVENTHANDLER_H
