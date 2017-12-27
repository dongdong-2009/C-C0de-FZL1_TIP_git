/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobInstanceManager.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #6 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class keeps track of the instances of jobs that are created by
  * the JobRunner. It is notified by the JobQueue when a job is created
  * or removed, and handles the logic of deleting the pointer to the job.
  *
  */


#ifndef JOB_INSTANCE_MANAGER_H
#define JOB_INSTANCE_MANAGER_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "app/scheduling/scheduling_agent/src/IJob.h"
#include "app/scheduling/scheduling_agent/src/IJobExecutionListener.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"



#include <map>
#include <string>


namespace TA_Base_App
{


class JobInstanceManager : public IJobExecutionListener
{

public:


	/**
	 *  Gets the single instance of this class.
	 */
	static JobInstanceManager& getInstance();

    /**
     *  Called when a job is about to start.
     *
     *  @param jobID          the ID of the job.
     *  @param minuteStarted  the minute at which the job was started, relative to the epoch.
     *
     *  Note: This method should not be called directly.
     *  Call addJobInstance() instead.
     */
    virtual void startingJob( const std::string& jobID, unsigned long minuteStarted );

    /**
     *  Called immediately after a job completes.
     *  If there was an error, the errorMessage parameter is not "".
     *
     *  @param jobID          the ID of the job.
     *  @param minuteStarted  the minute at which the job was started, relative to the epoch.
     *
     *  Note: This method should not be called directly.
     *  Call removeJobInstance() instead.
     */
    virtual void jobCompleted( const std::string& jobID, unsigned long minuteStarted );

    /**
     *  Called immediately after a job instance is deleted.
     *  If there was an error, the errorMessage parameter is not "".
     *
     *  @param jobID          the ID of the job.
     *
     *  Note: This method should not be called directly.
     *  Call deleteRemovedJob() instead.
     */
    virtual void jobDeleted( const std::string& jobID );

    /**
     *  Adds the job instance to the list this object maintains.
     */
    void addJobInstance( const std::string& jobID, unsigned long minuteStarted);

    /**
     *  Removes the job instance from the list this object maintains.
     */
    void removeJobInstance( const std::string& jobID, unsigned long minuteStarted );


    /**
     *  Removes all job instances.
     */
    void removeAllJobInstances();


    /**
     *  Called when a job is created in the JobQueue.
     */
    void jobCreated(IJob* job);


    /**
     *  Called when a job is removed from the JobQueue.
     */
    void jobRemoved(const std::string& jobID);


	//TD16352++
	/**
     *  Called to lock/unlock a job from being manipulated
	 *  @param JobID          the ID of the job.
     *  @param strSessionId   session that locked the job
	 *  @param isLocked       lock the stuff
	 *  
	 *  returns true if job is found
     */
	bool updateEditLock(const std::string& jobID, std::string strSessionId, bool isLocked);

	/**
     *  Called by client to determine whether job is locked
	 *  @param JobID          the ID of the job.
     *  @param strSessionId   session that locked the job
	 *  
	 *  returns true if job is locked
     */
	bool isJobLocked(const std::string& jobID, std::string strSessionId);
	//++TD16352

    /**
     *  check whether job is running.
     */
	bool isJobRunning(const std::string& jobID);

    /**
     *  delete job which has been removed.
     */
	void deleteRemovedJob(const std::string& jobID);

    /**
     *  Gets all job instances that have been added to the JobInstanceManager.
     */
    TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceSequence getJobInstances();

    /**
     *  Gets a BatchUpdate of the state of the jobs in the system.
     */
    TA_Base_Bus::SchedulingAgentCorbaDef::BatchUpdateCorbaDef getBatchUpdate();

    /**
     *  Gets all scheduled jobs.
     */
    TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence getAllJobs();

    /**
     *  Gets a pointer to the job with the given ID.
     *
     *  @return a pointer to the job with the given ID if there is one,
     *          NULL otherwise.
     */
    IJob* getJob(const std::string& jobID);

    /**
     *  Destructor.
     */
    ~JobInstanceManager();

private:
	JobInstanceManager();
    JobInstanceManager(const JobInstanceManager&);
    const JobInstanceManager& operator=(const JobInstanceManager&);

    /**
     *  The multimap of job instances that have been started.
     *  The multimap contains entries of the form
     *
     *  [JobID] => [minuteStarted]
     *
     *  This list is used to check when all instances of a job have been
     *  removed so the IJob pointer is allowed to be deleted.
     */
    typedef std::multimap<std::string, unsigned long> RunningJobsMultiMap;

    TA_Base_Core::NonReEntrantThreadLockable m_lckRunningJobs;
    RunningJobsMultiMap m_runningJobsMultiMap;

    typedef struct JobInfo
    {
        IJob* job;

        //
        // Set to true when the job has been removed from the scheduled jobs
        // queue. This indicates that when all instances of the job have
        // completed, it may be deleted.
        //
        bool hasBeenRemoved;

		//TD16352++
		//Use this in the instance manager to determine lock type
		bool isLockedForEdit;
		//Use this for determining who has locked 
		std::string strSessionId;
		//++TD16352
    };

    // typedef std::map< TA_Base_Bus::SchedulingAgentCorbaDef::JobID, JobInfo > JobPointersMap;
	typedef std::map<std::string, JobInfo> JobPointersMap;

    /**
     *  The jobs that have been created by the JobQueue.
     *  The map contains entries of the form
     *
     *  [JobID] => [ JobInfo{ IJob*, hasBeenRemoved } ]
     *
     *  This map holds pointers to the job objects so that they can be
     *  deleted when all references to the job have been cleaned up.
     *
     *  They are cleaned up when the job has been removed from the queue, and
     *  all instances of the job have finished running.
     */
    TA_Base_Core::NonReEntrantThreadLockable m_lckJobs;
    JobPointersMap m_jobPointersMap;

    /** The single instance of this class */
    static JobInstanceManager* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
};


};// TA_Base_App


#endif // JOB_INSTANCE_MANAGER_H
