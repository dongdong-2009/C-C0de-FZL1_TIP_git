/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobMonitorThread.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Monitors an instance of a job.
  *
  */


#include "app/scheduling/scheduling_agent/src/JobMonitorThread.h"

namespace TA_Base_Core
{


const unsigned int JobMonitorThread::SLEEP_TIME_MILLIS = 10000;


JobMonitorThread::JobMonitorThread(
    IJob* job,
    unsigned long minuteStarted)
    :
    m_running(false),
    m_job(job),
    m_instanceID(instanceID)
{
}


JobMonitorThread::~JobMonitorThread() {}


SchedulingAgentCorbaDef::JobInstanceID* JobMonitorThread::getInstanceID()
{
    return m_instanceID;
}


SchedulingAgentCorbaDef::JobID* JobMonitorThread::getJobID()
{
    return m_job->getID();
}



void JobMonitorThread::run()
{
    m_running = true;


    //
    // These will be set by the getJobInstanceState method.
    //
    JobRunner::ErrorCode errorCode = JobRunner::NoError;
    std::string errorMessage       = "";

    //
    // Loop until the thread is terminated or the job instance
    // is complete.
    //
    while(m_running)
    {
        if(m_job->getJobInstanceState(
               jobInstanceID,  // in
               errorMesage)    // out
                  ==
           JobInstance::Complete)
        {
            m_running = false;
        }
        else
        {
            Thread::sleep(SLEEP_TIME_MILLIS);
        }
    }

    //
    // Inform the JobRunner that this job instance has completed.
    //
    JobRunner::getInstance().jobCompleted(this, errorMessage);
}


void JobMonitorThread::terminate()
{
    m_running = false;
}



}; // TA_Base_Core

#endif // JOB_THREAD_H
