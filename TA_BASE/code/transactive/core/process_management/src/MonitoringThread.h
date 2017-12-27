/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/MonitoringThread.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last mofified by:  $Author: CM $
  *
  * MonitoringThread monitors the state of the System Controller and checks
  * whether or not the MonitoringThreadUser has been terminated.
  * 
  * IMonitoringThreadUser provides an interface that should be
  * implemented by any class that needs to be notified of a
  * System Controller failure (eg ManagedProcess). In the event
  * of the MonitoringThread discovering that they System Controller
  * has failed, it will call onControllerFailure on the 
  * IMonitoringThreadUser.
  */

#ifndef MONITORING_THREAD_H
#define MONITORING_THREAD_H

#include "core/threads/src/Thread.h"
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"

namespace TA_Base_Core
{
    class IMonitoringThreadUser
    {
    public:
        /**
          * onControllerFailure
          *
          * This pure virtual method provides a method for
          * the MonitoringThread to notify any derived
          * MonitoringThreadUsers of a System Controller
          * failure
          */
        virtual void onControllerFailure() = 0;

        /**
          * hasTerminated
          *
          * This pure virtual method allows the Monitoring
          * Thread to check whether or not the Monitoring Thread
          * User has had a request to terminate.
          *
          * @return TRUE if the monitoring thread has been terminated
          */
        virtual bool haveBeenToldToTerminate() = 0;

        /**
          * cleanup
          *
          * If the MonitoringThreadUser has been instructed to terminate,
          * this method is called to tell it to cleanup
          */
        virtual void cleanup() = 0;

        /**
          * Virtual destructor
          */
        virtual ~IMonitoringThreadUser() {}
    };
}

namespace TA_Base_Core
{
        class MonitoringThread : public TA_Base_Core::Thread
    {
    public:
        /**
          * Default Constructor
          *
          * @param Optional parameter to set the polling period (defaults to 10 seconds)
          */
        MonitoringThread(unsigned int p_timeoutInSeconds = 10);

        /**
          * Virtual Destructor
          */
        virtual ~MonitoringThread();

        /**
          * registerForMessage
          *
          * This method allows IMonitoringThreadUsers to register
          * for notification in the event of the System Controller
          * failing.
          *
          * @param Handle to the IMonitoringThreadUser
          * @param Handle to the System Controller
          */
            void registerForMessage(IMonitoringThreadUser* const pUser, TA_Base_Core::IProcessManagerCorbaDef_ptr pProcessMan);

        /**
          * run
          *
          * This method overrides the run method defined in Thread
          */
   	        virtual void run();
   
        /**
          * terminate
          *
          * This method overrides the terminate method defined in Thread
          */
        virtual void terminate();

    private:

        /**
          * pollPrimary
          *
          * This method is responsible for polling the System Controller
          * every m_pollPeriodInSeconds seconds.
          */
        void pollPrimary();

        // Disable the copy constructor and assignment operator
        MonitoringThread( const MonitoringThread& theMonitoringThread);
        MonitoringThread& operator=(const MonitoringThread &);

        // Number of seconds between each poll
        unsigned int m_pollPeriodInSeconds;

        // Set to TRUE if terminate has been called
        bool m_terminated;

        // Handle to the System Controller
            TA_Base_Core::IProcessManagerCorbaDef_ptr m_pProcessMan;

        // Handle to the registered monitoring thread user
        IMonitoringThreadUser* m_pUser;
    };
}

#endif // !defined MONITORING_THREAD_H
