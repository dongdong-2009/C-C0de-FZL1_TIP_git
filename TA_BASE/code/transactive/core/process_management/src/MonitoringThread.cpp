/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/MonitoringThread.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "core/process_management/src/MonitoringThread.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    //
    // Constructor
    //
    MonitoringThread::MonitoringThread(unsigned int p_timeoutInSeconds)
    : m_pollPeriodInSeconds(p_timeoutInSeconds), m_terminated(false)
    {
        m_pUser = 0;
            m_pProcessMan = TA_Base_Core::IProcessManagerCorbaDef::_nil();
    }


    //
    // Destructor
    //
    MonitoringThread::~MonitoringThread()
    {
    }


    //
    // registerForMessage
    //
    void MonitoringThread::registerForMessage(IMonitoringThreadUser* const pUser,
                      TA_Base_Core::IProcessManagerCorbaDef_ptr pProcessMan)
    {
        if (NULL != pUser && !CORBA::is_nil(pProcessMan))
        {
            m_pUser = pUser;
            m_pProcessMan = pProcessMan;
        }
        start();
    }


    //
    // pollPrimary
    //
    void MonitoringThread::pollPrimary()
    {
        if (CORBA::is_nil(m_pProcessMan))
        {
            // We dont have a system controller, tell the MonitoringThreadUser
            m_pUser->onControllerFailure();
            return;
        }

        const int NUM_ATTEMPTS = 2;         // The number of times we should attempt polling
        const int POLL_INTERVAL_MS = 5000;  // Poll interval
        
        for ( size_t i = 0; i < NUM_ATTEMPTS; ++i )
        {
            try
            {     
                m_pProcessMan->poll();
                return;
            }
            catch( ... )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught exception when polling system manager, attempt retrying %d", i + 1 );
            }

            TA_Base_Core::Thread::sleep( POLL_INTERVAL_MS );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to poll system manager after %d retries", NUM_ATTEMPTS );

        // Notify the ManagedProcess that the system controller has failed
        m_pUser->onControllerFailure();
    }


    //
    // run
    //
    void MonitoringThread::run()
    {
        const unsigned int SLEEP_TIME = 1000; // 1 second

        // Keep track of the current count, the first poll should occur imediately
        unsigned int count(m_pollPeriodInSeconds);

        while(true)
        {
	        try
	        {
                // Increment the counter
                count++;

                // Check if the monitoring thread user has been told to terminate
                if( m_pUser->haveBeenToldToTerminate() )
                {
                    try
                    {
                        m_pUser->cleanup();
                    }
                    catch( ... )
                    {
                        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                             "Caught an unknown exception from ManagedProcess::cleanup()");
                    }

                    terminate();
                }

                // Termination has been requested, must wait until all
                // process have been killed before breaking
                if (m_terminated)
                {        
                    break;
                }

                // Check to see if the counter has surpassed the pollPeriod
                if (count >= m_pollPeriodInSeconds)
                {
                    count = 0;
                    pollPrimary();
                }
        
                // Sleep the thread for a second
                this->sleep(SLEEP_TIME);
	        }
	        catch(...)
	        {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught an unknown exception");
                // Sleep the thread for a second
                this->sleep(SLEEP_TIME);
	        }
        }
    }


    //
    // terminate
    //
    void MonitoringThread::terminate()
    {
        m_terminated = true;
    }
};
