#ifndef NOTIFICATION_SERVICE_THREAD_H
#define NOTIFICATION_SERVICE_THREAD_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NotificationServiceThread.h $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/12/17 15:24:44 $
 * Last modified by:  $Author: shurui.liu $
 * 
 * A thread that executes a CORBA Notification Service
 *
 */
#include <sys/types.h>

#include "core/threads/src/Thread.h"

#include <string>
#include <vector>


namespace TA_Base_App
{
    class NotificationServiceThread : public TA_Base_Core::Thread
    {

    public:

        /**
         * Constructor
         *
         * Initialises the argument list to be passed to execv()
         *
         * @param serviceExecutable        the full path name of the NotificationService binary
         * @param serviceArguments        parameters to be passed to the NotificationService at runtime
         *
         */
        NotificationServiceThread( const std::string& serviceExecutable,
                                   const std::vector<std::string>& serviceArguments );

        /**
         * Destructor
         *
         * If the child process is still running, it will be terminated.
         *
         */
        virtual ~NotificationServiceThread();

        /**
         * run()
         *
         * Spawns a child process (which executes the NotificationService) and then monitors
         * for abnormal termination of it. In the event of an abnormal exit, the parent will
         * spawn again and run a new NotificationService.
         *
         */
        virtual void run();

        /**
         * terminate()
         *
         * If child execution is in progress, waits for that to complete. Then if the child
         * process is running, calls killProcess() on it.
         *
         */
        virtual void terminate();


    private:

        /**
         * initialiseServiceArguments()
         *
         * Takes the vector of string and copies it into the array of char*'s (m_serviceArguments)
         *
         */
        void initialiseServiceArguments( const std::vector<std::string>& arguments );


        /**
         * Location of Notification Service executable to run
         */
        std::string m_serviceExecutable;


        /**
         * Arguments to use when executing Notification Service
         */
        char** m_serviceArguments;

        /**
         * Number of arguments being passed to the Notification Service
         */
        int m_argumentsSize;

        /**
         * This indicates whether the object is ready to terminate or not.
         * Used to stop termination occurring during execution of service.
         */
        bool m_terminateEnabled;
    };
}

#endif // NOTIFICATION_SERVICE_THREAD_H
