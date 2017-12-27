/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobQueue.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class is used to store information about scheduled jobs, and to
  * execute them at the correct time.
  *
  */


#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H


#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "app/scheduling/scheduling_agent/src/JobRunner.h"
#include "app/scheduling/scheduling_agent/src/IJobExecutionListener.h"

#include <string>
#include <map>


namespace TA_Base_App
{

class IJob;

class JobQueue : public TA_Base_Core::Thread, public TA_Base_Core::ReEntrantThreadLockable
{

public:


    // The modes the JobQueue may be in.
    typedef bool Mode;

    const static Mode MONITOR_MODE;
    const static Mode CONTROL_MODE;


	/**
	 *  Gets the single instance of this class.
	 */
	static JobQueue& getInstance();


    /**
     *  Reads all jobs from the database and schedules them at the appropriate
     *  minute. This method should only be called once when the controlling
     *  agent starts up.
     *
     *  @exception BadScheduleException
     *  thrown if the schedule is invalid, eg if the date does not occur
     *
     *  @exception DataException
     *  thrown if there is a problem with the format of the data in
     *  the database
     *
     *  @exception DatabaseException
     *  thrown if there is a problem communicating with the database
     */
    void readJobsFromDatabase();


	/**
	 *  Adds the given job to the queue of scheduled jobs.
     *
     *  @return the ID of the job that was added.
     *
     *  @exception JobInPastException
     *  thrown if the job is a once off and is scheduled in the past.
     *
     *  @exception BadScheduleException
     *  thrown if the schedule is invalid, eg if the date never occurs
     *
     *  @exception DataException
     *  thrown if there is a problem with the format of the data in
     *  the database
     *
     *  @exception DatabaseException
     *  thrown if there is a problem communicating with the database
	 */
    std::string scheduleJob( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job );


	/**
	 *  Updates the job with the ID matching the parameter.
     *
     *  @exception JobDoesNotExistException
     *  thrown if the job is not in the queue of scheduled jobs.
     *
     *  @exception BadScheduleException
     *  thrown if the schedule is invalid, eg if the date never occurs
     *
     *  @exception DataException
     *  thrown if there is a problem with the format of the data in
     *  the database
     *
     *  @exception DatabaseException
     *  thrown if there is a problem communicating with the database
	 */
    void JobQueue::updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job, bool checkIfPastDate = true);


	/**
	 *  Removes the given job from the queue of scheduled jobs.
     *
     *  @exception JobDoesNotExistException
     *  thrown if there is no job with that ID in the queue of scheduled jobs.
     *
     *  @exception DataException
     *  thrown if there is a problem with the format of the data in
     *  the database
     *
     *  @exception DatabaseException
     *  thrown if there is a problem communicating with the database
	 */
    void removeJob(const std::string& jobID, std::string sessionId = "");


	/**
	 *  Puts the JobQueue into Monitor mode.
	 */
	void setMonitorMode();
	

	/**
	 *  Puts the JobQueue into Control mode.
	 */
	void setControlMode();
	

	/**
	 *  Indicates which mode the JobQueue is currently in.
	 */
	Mode getMode() const;

    
	/**
	 *  Called by the start() method.
	 */
	void run();

	/**
	 *  Called by the terminateAndWait() method.
	 */
	void terminate();


    /**
     *  Sets the ID of the operator that the Scheduling Agent runs as.
     */
    void setSchedulingAgentOperatorID(unsigned long operatorID);

    
    /**
     *  Destructor.
     */
    virtual ~JobQueue();



protected:
	JobQueue();
    JobQueue(const JobQueue&);


    //
    // The queue maintained by this class is a multimap containing
    // [minute started => jobID] pairs
    //
    typedef std::multimap<unsigned long, std::string> ScheduledJobsQueue;


    /**
     *  Adds the given job to the queue at the appropriate minute.
     */
    void addJobToQueue(IJob* job);


    /**
     *  Checks that the given schedule's date and end date are not in the
     *  past. Throws an exception if they are.
     *
     *  @exception BadScheduleException
     *  thrown if the schedule is invalid, eg if the date never occurs
     *
     *  @exception JobInPastException
     *  if either the schedule's date or end date is in the past.
     */
    void checkDatesAreNotInPast(
        const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule);

    /**
     *  Sleeps until the next minute starts, or until the thread is
     *  terminated.
     */
    void sleepTillNextMinuteStartsOrThreadTerminated();


    /**
     *  Processes any scheduled jobs that are ready to run.
     */
    void processReadyJobs();


    /**
     *  Processes the next scheduled job that is ready to run.
     *
     *  @param job              the job
     *  @param minuteStarted    the minute at which the job was actually started
     *                          (which may, in a failover situation, be
     *                          different to when it was scheduled to start)
     */
    void JobQueue::processJob(  IJob* job, unsigned long minuteStarted );


    /**
     *  Removes a job whose schedule has expired because it is a once off
     *  or is past its end date.
     *  This also informs the SchedulingMessageSender that the job has beem
     *  removed.
     *
     *  @exception DatabaseException
     *  if there is problem communicating with the database.
     */
    void removeCompletedJob(const std::string& jobToRemoveID);


    /** 
      * removeJobUnlocked
      *
      * Removes a job from the queue and database without locking the object.
      *
      * @param jobToRemoveID The job ID of the job to be removed
      * @param deleteJobFromDb If this is true, the job will be deleted from the database and the appropriate
      *         Comms and Audit messages will be sent to indicate the deletion.  Otherwise, it will only
      *         delete the job from the queue
      * @param sessionId The session ID which will be used to send the audit message
      * @exception DatabaseException
      * if there is problem communicating with the database.
      *
      */
    void removeJobUnlocked(const std::string& jobToRemoveID, bool deleteJobFromDb = true, std::string sessionId = "");


    /**
     *  Finds the job with the given ID in the scheduled jobs queue.
     *
     *  @return an iterator pointing to the entry in the queue,
     *          or m_scheduledJobsQueue.end() if there was no job with that ID
     */
    ScheduledJobsQueue::iterator findScheduledJob( const std::string& jobID );



private:

	/**
	 *  Indicates if the thread is running.
	 */
	volatile bool m_running;


    /**
     *  Indicates in which mode the monitor is in.
     */
    Mode m_mode;



	/**
	 *  The queue of jobs to be run. The queue is a multimap with entries of
     *  the form
	 *
	 *  [minute since epoch] => [JobID]
	 *
	 *  The key is the minute since the epoch, so the map is sorted such that
	 *  the next job to be executed is at the front.
	 */
    ScheduledJobsQueue m_scheduledJobsQueue;


    /**
     *  The ID of operator that the Scheduling Agent runs as.
     */
    unsigned long m_operatorID;

    /**
     *  Gets the nember of jobs that in the queue that are scheduled to run
     *  at the specifed point in time.
     */
    unsigned int JobQueue::getNumJobsAtMinute(const unsigned long runMinute);

    /** The single instance of this class */
    static JobQueue* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;


	/**
	 *  The time that this thread sleeps between checking if it has been
	 *  terminated.
	 */
	static const unsigned long SLEEP_TIME_MILLIS;

    /**
	 *  The maximum number of jobs the client can schedule at any time.
	 */
	static const unsigned int MAX_ALLOWED_JOBS_AT_ONCE;


    /** 
     *  Prevent assignments
     */
    JobQueue operator= (JobQueue& job);
};


};// TA_Base_App


#endif // JOB_QUEUE_H
