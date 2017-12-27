/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobMonitorThread.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Monitors an instance of a job.
  *
  */


#ifndef JOB_MONITOR_THREAD_H
#define JOB_MONITOR_THREAD_H


#include "core/threads/src/Thread.h"

#include "app/scheduling/scheduling_agent/src/IJobInstance.h"
#include "app/scheduling/scheduling_agent/src/IJob.h"


namespace TA_Base_Core
{

// Forward declaration
class IJob;


class JobMonitorThread:  public Thread
{

public:


    /**
     *  Constructor
     */
    JobMonitorThread(IJobInstance* jobInstance);

    /**
     *  Destructor.
     */
	virtual ~JobMonitorThread();


    /**
     *  Gets the minute at which the job instance that this object runs
     *  started.
     */
    unsigned long* getMinuteStarted();
    
    /**
     *  Gets the ID of the job which this object runs an instance of.
     */
    SchedulingAgentCorbaDef::JobID* getJobID();


private:

    /**
     *  Called by the start method.
     */
    void run();

    /**
     *  Called by the terminateAndWait method
     */
    void terminate();



    JobMonitorThread();
    JobMonitorThread(const JobMonitorThread&);
    const JobMonitorThread& operator=(const JobMonitorThread&);


    /** Indicates if the thread is currently running */
    volatile bool m_running;


    /** The job that is being monitored */
    IJob* m_job;

    /** The ID of the instance of the job */
    unsigned long m_instanceID;


    
    /** The time to pause between checking if the job has comepleted. */
    static const unsigned int SLEEP_TIME_MILLIS;

};


}; // TA_Base_Core

#endif // JOB_MONITOR_THREAD_H
