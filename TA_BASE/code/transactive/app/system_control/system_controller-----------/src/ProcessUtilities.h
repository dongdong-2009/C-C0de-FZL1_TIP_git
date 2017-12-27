/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/ProcessUtilities.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class contains only static functions that perform 
  * a number of process-specific utilities such as starting 
  * and killing processes.
  *
  */

#ifndef PROCESSUTILITIES_H
#define PROCESSUTILITIES_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <string>
#include <vector>

namespace TA_Base_App
{

    class ProcessUtilities
    {

    public:

		/**
		  * Constructor
		  */
		ProcessUtilities();

        /**
          * launchProcess()
          *
          * This method is used to launch a process.
          *
          * @param startPath  The absolute path to the process
          * @param argList    The list of arguments to be started (the first
          *                   arg is the processName).
		  * @param userName   The username to use to start the process.
          *
          * @return  The pid of the started process.
          */
        static TA_Base_Core::ProcessId launchProcess(const std::string& startPath,
											const std::vector<std::string>& argList,
											const std::string& userName,
											const std::string& passWord);
        /**
          * terminateProcess()
          *
          * terminateProcess is used to send a SIGTERM notification to the process
          * 
          * @param pid   The pid of the process to terminate 
          *
          * @return bool True if the process has been terminated (either because
          *              of the SIGTERM or because the pid didn't exist), and false
          *              otherwise.
          */
        static bool terminateProcess(int pid);
 
        /**
          * killProcess()
          *
          * Kill process is used to send a SIGKILL notification to the process
          *  
          * @param pid    The pid of the process to kill
          *
          */
        static void killProcess(int pid);

        /** 
          * getHostname
          *
          * Returns the name of the host we are running on.
          *
          * @return The hostname.
          *
          */
        static std::string getHostname();

        /**
          * parseProcessAddress
          *
          * This method is used to return the startPath and command line arguments for
          * a process.
          * 
          * @param startPath       Populated with the absolute location of the process.
          * @param argList         Populated with the list of arguments, where the
          *                        first argument is the process name.
          * @param processAddress  The string containing the necessary info.
          */
        static void parseProcessAddress(std::string& startPath,
										std::vector<std::string>& argList,
										const std::string& processAddress);

        /** 
          * setPriority
          *
          * Sets the process's priority. UNIX allows a nice value to be set
		  * in the range of -20 (highest priority) to 20 (lowest priority).
          *
          * @param pid       The process's ID.
          * @param priority  The process's new priority.
          *
          */
		static void setPriority(int pid, int priority);

        /** 
          * bindToProcessor
          *
          * Binds a process (and all of its threads) to a particular 
		  * processor.
          *
          * @param pid        The process's ID.
          * @param processor  The processor's number.
          *
          */
		static void bindToProcessor(int pid, int processor);
/*
		static int getProcessIdbyName(std::string pname);

*/
#ifdef WIN32

        /**
          * checkProcess()
          *
          * This method will check to see if the process is running, if not 
          * it will return false.
          *
          * @param int pid   The handle of the process to check
          *
          * @return bool     True if the process is running
          */
        static bool checkProcess(int pid);

#endif //#ifdef WIN32


    private:

        /**
          * A lock for system calls to getpwnam.
          */
        static TA_Base_Core::NonReEntrantThreadLockable m_getpwnamLock;

    };

} // namespace TA_Base_App

#endif // PROCESSUTILITIES_H
