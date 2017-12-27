/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobQueue.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #8 $
  *
  * Last modification: $DateTime: 2013/11/01 15:29:57 $
  * Last modified by:  $Author: huangjian $
  *
  * This class is used to store information about scheduled jobs, and to
  * execute them at the correct time.
  *
  */

// Disable warnings from boost
#if defined(_MSC_VER)
  #pragma warning(disable:4244) // conversion - possible loss of data
#endif // _MSC_VER

#include "ace/OS.h"

#include "app/scheduling/scheduling_agent/src/IJob.h"
#include "app/scheduling/scheduling_agent/src/JobQueue.h"
#include "app/scheduling/scheduling_agent/src/JobFactory.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceManager.h"
#include "app/scheduling/scheduling_agent/src/SchedulingException.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceStateRetrievalException.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingMessageSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_App
{

JobQueue* JobQueue::m_instance = NULL;
TA_Base_Core::ReEntrantThreadLockable JobQueue::m_singletonLock;

const JobQueue::Mode JobQueue::MONITOR_MODE = false;
const JobQueue::Mode JobQueue::CONTROL_MODE = true;

const unsigned int JobQueue::MAX_ALLOWED_JOBS_AT_ONCE = 20;
const unsigned long JobQueue::SLEEP_TIME_MILLIS = 100;



JobQueue::JobQueue() :
m_running(false),
m_mode(MONITOR_MODE),
// Initialize the scheduled jobs queue to be empty
m_scheduledJobsQueue()
{
}


JobQueue::~JobQueue()
{
}


JobQueue& JobQueue::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new JobQueue();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }
    
    return *m_instance;
}


void JobQueue::readJobsFromDatabase()
{
    FUNCTION_ENTRY("readJobsFromDatabase");

    //
    // This should only ever be called when the queue is empty
    //
    TA_ASSERT(m_scheduledJobsQueue.empty(),
        "Jobs should not be read from database when queue is not empty");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Reading jobs from database" );

    // Get stored jobs out of the database and schedule them.
    IJob::IJobPtrVector jobs = JobFactory::getInstance().createJobsFromStore();

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "There are %d jobs in the database.", jobs.size() );

    IJob::IJobPtrVector::iterator it;
    try
    {
        for(it = jobs.begin(); it != jobs.end(); it ++)
        {
            try
            {
                IJob* job = *it;

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "Trying to load job '%s'",(*it)->getID().c_str() );

                //
                // Queue the job
                // If the job is past it's end date this method 
                // will silently fail - this is ok
                //
                addJobToQueue(job);

                //
                // Inform the instance manager that this job has been created.
                // It is possible to have jobs that are
                // not runnable (end date in past) but we still want to
                // track them
                //
                JobInstanceManager::getInstance().jobCreated(job);
            }
            catch(const TA_Base_Bus::SchedulingAgentCorbaDef::JobInPastException&)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                    "JobInPastException caught for job with id '%s'. Ignoring job.", (*it)->getID().c_str() );

                // TODO
                // NOTE The jury is still out on what to do here
                // delete/inhibit/mark complete (requires db changes)
                // for now just ignore it
                delete (*it);
                *it = NULL;
            }
        }// for(IJob::IJobPtrVector::iterator it = jobs.begin(); it != jo ...
    }
    catch(...)
    {
        //
        // Lock the object
        //
        TA_Base_Core::ThreadGuard(*this);

        //
        // addJobToQueue and JobInstanceManager::jobCreated don't throw
        // exceptions, so we can ignore the job that was about to be
        // scheduled when the exception was thrown from checkDatesAreNotInPast
        //
        for(IJob::IJobPtrVector::iterator deleteIt = jobs.begin(); deleteIt != it && deleteIt != jobs.end(); ++deleteIt)
        {
            if (NULL != *deleteIt)
            {
				std::string jobID = (*deleteIt)->getID().c_str();
				ScheduledJobsQueue::iterator jobIt = findScheduledJob(jobID);

				//
				// Remove any jobs that have already been scheduled from
				// the JobQueue and tell the JobInstanceManager to delete them
				//
				if( jobIt != m_scheduledJobsQueue.end() )
				{
					m_scheduledJobsQueue.erase(jobIt);
					JobInstanceManager::getInstance().jobRemoved(jobID);
				}
            }
        }

		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknow Exception Occurred");
        throw;
    }// catch

}


std::string JobQueue::scheduleJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaJob)
{
    FUNCTION_ENTRY("scheduleJob");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Scheduling job with description '%s'\n Schedule:\n%s", 
        corbaJob.description.in(), Schedule::toString( corbaJob.schedule ).c_str() );

    IJob* job = NULL;

    //
    // Check that the scheduled date and end date of the job aren't in the past
    //
    if(m_mode == CONTROL_MODE)
    {
        checkDatesAreNotInPast(corbaJob.schedule);

        //
        // Check to ensure that there are not to many jobs currently running
        //
        unsigned long runTime = Schedule::getNextScheduledMinute(corbaJob.schedule);
        unsigned int unJobsCountAtMinute = getNumJobsAtMinute( runTime );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "[TD16068] runTime: %lu, MAX_ALLOWED_JOBS_AT_ONCE: %lu, getNumJobsAtMinute( runTime ): %lu", 
            runTime, MAX_ALLOWED_JOBS_AT_ONCE, unJobsCountAtMinute );

        if ( unJobsCountAtMinute >= MAX_ALLOWED_JOBS_AT_ONCE )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "[TD16068] getNumJobsAtMinute( runTime ) >= MAX_ALLOWED_JOBS_AT_ONCE");
            throw TA_Base_Bus::SchedulingAgentCorbaDef::TooManyJobsException();
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "[TD16068] getNumJobsAtMinute( runTime ) < MAX_ALLOWED_JOBS_AT_ONCE");
        }

        job = JobFactory::getInstance().createJob(corbaJob);

        TA_ASSERT(job != NULL, "Job must not be NULL");
    }
    //
    // If the queue is in monitor mode, get the job from the database.
    //
    else
    {
        job = JobFactory::getInstance().getJobFromStore(corbaJob.identifier.in());
    }

    std::string strJobID = "";
    //
    // If the job has not already been removed (this will only happen in
    // monitor mode), add it to the queue and inform the JobInstanceManager
    // that it has been created.
    //
    if(job != NULL)
    {
        addJobToQueue(job);

        //
        // Inform the instance manager that this job has been created.
        //
        JobInstanceManager::getInstance().jobCreated(job);

        strJobID = job->getID().c_str();
    }

    return strJobID;
}


void JobQueue::addJobToQueue(IJob* job)
{
    FUNCTION_ENTRY("addJobToQueue");
    TA_ASSERT(job != NULL, "Job must not be NULL");

    //
    // Get the minute at which the job will run for the first time.
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef runTime = job->getSchedule().getNextScheduledTime();

    unsigned long minuteToStartJob = job->getSchedule().getNextScheduledMinute();

    if ( minuteToStartJob != 0 )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Scheduling job with ID %s at %d minutes after the epoch.\n This is at %s", 
            job->getID().c_str(), minuteToStartJob, SchedulingTimer::getFormatedTime( minuteToStartJob ).c_str());

        //
        // Put the job in the scheduling queue at the appropriate place
        //
        TA_Base_Core::ThreadGuard lock(*this);
    
        ScheduledJobsQueue::value_type entry( minuteToStartJob, job->getID() );
        m_scheduledJobsQueue.insert(entry);

        if (CONTROL_MODE == m_mode)
        {
            // tell those who care
            SchedulingMessageSender::getInstance().jobScheduled( job->getID(), runTime );
        }
    }
}



void JobQueue::updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaJob, bool checkIfPastDate/*=true*/)
{
    FUNCTION_ENTRY("updateJob");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "Updating job with ID '%s'.", corbaJob.identifier.in() );

    //
    // Lock this object.
    //
    TA_Base_Core::ThreadGuard lock(*this);

    IJob* job = JobInstanceManager::getInstance().getJob(corbaJob.identifier.in());

    //
    // If the job was found
    //
    if(job != NULL)
    {
        //
        // If the Job Queue is in control mode, update the database.
        //
        if(m_mode == CONTROL_MODE)
        {
            //
            // Check that the new schedule date and end date have not already
            // passed
            //
			if(checkIfPastDate)
			{
	            checkDatesAreNotInPast(corbaJob.schedule);
			}

			//
			// Check to ensure that there are not to many jobs currently running
			//
			unsigned long runTime = Schedule::getNextScheduledMinute(corbaJob.schedule);
            unsigned int unJobsCountAtMinute = getNumJobsAtMinute( runTime );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "[TD16068] runTime: %lu, MAX_ALLOWED_JOBS_AT_ONCE: %lu, getNumJobsAtMinute( runTime ): %lu", 
                runTime, MAX_ALLOWED_JOBS_AT_ONCE, unJobsCountAtMinute );
        
            //TD16069++
            if ( unJobsCountAtMinute > MAX_ALLOWED_JOBS_AT_ONCE )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"[TD16068] getNumJobsAtMinute( runTime ) > MAX_ALLOWED_JOBS_AT_ONCE" );
				throw TA_Base_Bus::SchedulingAgentCorbaDef::TooManyJobsException();
			}
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"[TD16068] getNumJobsAtMinute( runTime ) < MAX_ALLOWED_JOBS_AT_ONCE" );
			}
			//TD16069++

            //
            // Update the database
            //
            job->updateJob(corbaJob);
        }
        else
        {
            bool forceReload = true;
            job->reloadDataFromDB(forceReload);
        }

        //
        // Update the queue
        //
        ScheduledJobsQueue::iterator it = findScheduledJob( job->getID() );
        
        // it is possible for the job to exist in the db but not the queue
        if ( it != m_scheduledJobsQueue.end() )
        {
            m_scheduledJobsQueue.erase(it);
        }
        
        //
        // this function applies it's own lock
        // so we have to release ours
        //
        addJobToQueue( job );
    }
    
    //
    // If there were no jobs in the map that match the ID of the
    // parameter, throw an exception.
    //
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not get job" );
        throw TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException();
    }
}




void JobQueue::removeJob(const std::string& jobToRemoveID, std::string sessionId /*=""*/)
{
    //
    // Lock the object
    //
    TA_Base_Core::ThreadGuard lock(*this);

    removeJobUnlocked(jobToRemoveID, true, sessionId);
}



void JobQueue::removeCompletedJob(const std::string& jobToRemoveID)
{
    //
    // Remove the job
    //
    removeJobUnlocked(jobToRemoveID, false);
}


//
// NOTE: THE OBJECT MUST BE LOCKED BEFORE CALLING THIS METHOD
//
void JobQueue::removeJobUnlocked(const std::string& jobToRemoveID, bool deleteJobFromDb /*= true*/, std::string sessionId /*=""*/)
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "Removing job with ID '%s'.", jobToRemoveID.c_str() );

    //
    // Find the job
    //
    IJob* job = JobInstanceManager::getInstance().getJob(jobToRemoveID);

    //
    // If there are no jobs in the queue whose ID matches the parameter,
    // throw an exception.
    //
    if(job == NULL)
    {
        throw TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException();
    }
    
    //
    // If the Job Queue is in control mode, update the database.
    //
    if (deleteJobFromDb && (m_mode == CONTROL_MODE))
    {
        //
        // Inform the SchedulingMessageSender that the job has been removed before it's 
        // actually removed from the database
        //

        SchedulingMessageSender::getInstance().jobDeleted(jobToRemoveID, sessionId);

        job->deleteJob();
    }

    //
    // Remove the job from the scheduled jobs queue
    //
    ScheduledJobsQueue::iterator it = findScheduledJob(jobToRemoveID);

    // as ended jobs are not deleted, it is possible to have a job in the 
    // JobInstanceManager but in the scheduled jobs queue
    if (it != m_scheduledJobsQueue.end() )
    {
        m_scheduledJobsQueue.erase(it);
    }

    //
    // Inform the JobInstanceManager that the job has been removed.
    //
    JobInstanceManager::getInstance().jobRemoved(jobToRemoveID);
}


void JobQueue::checkDatesAreNotInPast(
    const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule)
{
    FUNCTION_ENTRY("checkDatesAreNotInPast");
    //
    // Make sure the schedule is valid
    //
    Schedule::checkIfFieldsAreValid(schedule);

    if ( Schedule::getNextScheduledMinute(schedule) == 0 )
    {
        std::string message = "The end date specified is in the past";
        throw TA_Base_Bus::SchedulingAgentCorbaDef::JobInPastException();
    }

}



//
// NOTE: THE OBJECT MUST BE LOCKED BEFORE CALLING THIS METHOD
//
JobQueue::ScheduledJobsQueue::iterator JobQueue::findScheduledJob(const std::string& jobID)
{
    //
    // Search through the jobs in the queue until one is found whose ID
    // matches the parameter.
    //
    for(ScheduledJobsQueue::iterator it = m_scheduledJobsQueue.begin();
        it != m_scheduledJobsQueue.end(); it ++)
    {
        if( jobID.compare(it->second) == 0 )
        {
            return it;
        }
    }

    return m_scheduledJobsQueue.end();
}


void JobQueue::setSchedulingAgentOperatorID(unsigned long operatorID)
{
    m_operatorID = operatorID;
}


void JobQueue::setMonitorMode()
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "JobQueue set to monitor mode." );

    //
    // lock the object
    //
    TA_Base_Core::ThreadGuard lock(*this);

    m_mode = MONITOR_MODE;
}


void JobQueue::setControlMode()
{
    FUNCTION_ENTRY("setControlMode");

    TA_ASSERT( m_mode != CONTROL_MODE,
        "JobQueue set to control mode when it was already in control mode" );

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "JobQueue set to control mode." );

    //
    // lock the object
    //
    TA_Base_Core::ThreadGuard lock(*this);

    m_mode = CONTROL_MODE;

    //
    // If the time now is after the 30th second of the current minute,
    // assume all jobs that should have been started have started by now.
    //
    unsigned long secondOfMinute = 5 ; //ptime(second_clock::local_time()).time_of_day().seconds();

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Second of minute < 30 (%d). Synchronising job instances.", secondOfMinute );

    //
    // If the time is before the 30th second of the current minute,
    // some jobs that should have been started by the other controller
    // may not have been started before it crashed.
    //
    // So get all job instances that should have started, and make
    // sure that each has been started.
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceSequence instances =
        JobInstanceManager::getInstance().getJobInstances();

    for(unsigned int i = 0; i < instances.length(); i ++)
    {
        std::string jobID = instances[i].jobIdentifier.in();

        unsigned long minuteStarted = instances[i].minuteStarted;

        IJob* job = JobInstanceManager::getInstance().getJob(jobID);
        
        std::string errorMessage;

        try
        {
            //
            // If the job instance hasn't yet started, start it
            //
            if( job->getJobInstanceState( minuteStarted, errorMessage ) == IJobInstance::NotStarted )
            {
                JobRunner::getInstance().executeJob(job);
            }
            else
            {
                //
                // If the job instance has already started remove it from
                // this object's responsibility and let it run its course.
                // If it has completed we don't need to worry about it either.
                //
                JobInstanceManager::getInstance().removeJobInstance( jobID, minuteStarted );

                //
                // If there was an error executing the job, raise an alarm.
                //
                if( ! errorMessage.empty())
                {
                    SchedulingAlarmSender::getInstance().jobExecutionFailed(
                        jobID, minuteStarted, job->getDescription(), errorMessage);
                }
            }
        }
        catch(const JobInstanceStateRetrievalException& ex)
        {
            std::ostringstream action;
            action << "retrieve the state of job with ID " << jobID << " and instance ID " << minuteStarted;
            
            SchedulingAlarmSender::getInstance().schedulingControllerError( action.str(), ex.what() );
        }
        catch(...)
        {
            std::ostringstream action;
            action << "retrieve the state of job with ID " << jobID << " and instance ID " << minuteStarted;
            
            SchedulingAlarmSender::getInstance().schedulingControllerError( action.str(), "Unknown" );
        }

    }// for(unsigned int i = 0; i < instances.length(); i ++)
}


JobQueue::Mode JobQueue::getMode() const
{
    return m_mode;
}

unsigned int JobQueue::getNumJobsAtMinute(const unsigned long runMinute)
{
    unsigned int jobsAtMinute = 0;

    for(ScheduledJobsQueue::iterator it = m_scheduledJobsQueue.begin();
        it != m_scheduledJobsQueue.end();
        it ++)
    {
        if ( it->first == runMinute )
        {
            ++jobsAtMinute;
        }
    }
    return jobsAtMinute;
}


void JobQueue::run()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "JobQueue starting.");

    // If there is an error, the message will be put into this string
    std::string errorMessage = "";

    m_running = true;

    //
    // Wait till the start of the next minute before beginning the loop
    //
    sleepTillNextMinuteStartsOrThreadTerminated();

    try
    {
        while(m_running)
        {
            processReadyJobs();

            //
            // Wait till the start of the next minute before checking for new
            // scheduled items.
            //
            sleepTillNextMinuteStartsOrThreadTerminated();

        }// while(m_running)
    }
    catch(const TA_Base_Core::DataException& ex)
    {
        errorMessage  = "DataException: ";
        errorMessage += ex.what();
    }
    catch(const TA_Base_Core::DatabaseException& ex)
    {
        errorMessage  = "DatabaseException: ";
        errorMessage += ex.what();
    }
    catch(const std::exception& ex)
    {
        errorMessage  = "Exception: ";
        errorMessage += ex.what();
    }
    catch(const CORBA::Exception& ex)
    {
        errorMessage  = "CORBA Exception: ";
        errorMessage += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
    }
    catch(...)
    {
        errorMessage = "Unknown Exception";
    }

    //
    // If there was an error, raise an alarm
    //
    if( ! errorMessage.empty() )
    {
        SchedulingAlarmSender::getInstance().unexpectedTermination(
            errorMessage);
    }
}



void JobQueue::processReadyJobs()
{
    FUNCTION_ENTRY("processReadyJobs");
    //
    // lock the object
    //
    TA_Base_Core::ThreadGuard lock(*this);

    //
    // If the JobQueue is in monitor mode, remove all job instances
    // that were added to the JobInstanceManager at the start of the
    // last minute.
    //
    if(m_mode == MONITOR_MODE)
    {
        JobInstanceManager::getInstance().removeAllJobInstances();
    }


    //
    // Get the number of minutes since the epoch.
    //
    SchedulingTimer now;
    const unsigned long LAST_ELAPSED_MINUTE = now.getTimeInMinutesSinceEpoch();


    //
    // Log some information about the job queue
    //
    std::ostringstream debugOutput;
    debugOutput << "Minute: " << LAST_ELAPSED_MINUTE 
                << " is: " << SchedulingTimer::getFormatedTime( LAST_ELAPSED_MINUTE )
                << std::endl << "Scheduled jobs queue size: "
                << m_scheduledJobsQueue.size() << std::endl;

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", debugOutput.str().c_str() );

    for( ScheduledJobsQueue::iterator it = m_scheduledJobsQueue.begin(); 
        it != m_scheduledJobsQueue.end(); ++it )
    {
        IJob* job = JobInstanceManager::getInstance().getJob( it->second);

		if( job != NULL)
		{
            debugOutput.str("");

            debugOutput << "Job " << job->getID() << " with description '"
                        << job->getDescription() << "' and schedule "
                        << std::endl << job->getSchedule().toString() << std::endl
                        << "scheduled for " << it->first << " " << "minutes after the epoch." 
                        << " This is at " << SchedulingTimer::getFormatedTime( it->first ) << std::endl;

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", debugOutput.str().c_str() );
		}
    }

    //
    // While there are jobs in the queue and the job at the front
    // is ready to be executed.
    //
    while ( !m_scheduledJobsQueue.empty()  && (*m_scheduledJobsQueue.begin()).first <= LAST_ELAPSED_MINUTE )
    {
        std::string jobID = m_scheduledJobsQueue.begin()->second;

        IJob* job = JobInstanceManager::getInstance().getJob(jobID);

        TA_ASSERT(job != NULL, "JobInstanceManager should always know about scheduled jobs");

        //
        // Process the next ready job
        //
        processJob(job, LAST_ELAPSED_MINUTE);

    }// while

}


void JobQueue::processJob( IJob* job, unsigned long minuteStarted )
{
    FUNCTION_ENTRY("processJob");
    TA_ASSERT(job != NULL, "Job must not be NULL");

    std::string jobID = job->getID().c_str();

    //
    // If any exceptions are caught, the agent will raise an alarm and
    // attempt to continue scheduling jobs.
    //
    try
    {
        //
        // If the JobQueue is in control mode
        //
        if( m_mode == CONTROL_MODE )
        {
            //
            // If the job is not inhibited
            //
            if( ! job->isInhibited() )
            {
                //
                // Execute the job.
                //
                JobRunner::getInstance().executeJob(job);

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Job with ID %s executed at minute %d.", jobID.c_str(), minuteStarted );
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Job with ID %s not being started at minute %d because it is inhibited.", jobID.c_str(), minuteStarted );
            }
        }

        //
        // If the JobQueue is in monitor mode, tell the
        // JobInstanceManager that a job has started.
        // (In control mode, the JobInstanceManager is informed
        //  that the job has started when the job is executed)
        //
        else
        {
            if( ! job->isInhibited() )
            {
                JobInstanceManager::getInstance().addJobInstance( jobID, minuteStarted );
            }
        }



        //
        // Get the job's next scheduled minute
        // Throws BadScheduleException, DatabaseException, DataException
        //
        unsigned long nextMinuteToStartJob = job->getSchedule().getNextScheduledMinute();

        if(nextMinuteToStartJob == 0)
        {
            m_scheduledJobsQueue.erase( m_scheduledJobsQueue.begin() );
            SchedulingMessageSender::getInstance().jobCompletedAllExecutions(jobID);
        }
        else
        {

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Rescheduling Job with ID %s to run at minute %d.",
                jobID.c_str(), nextMinuteToStartJob );
            //
            // If the job is recurring, and not yet past its end date,
            // erase it from its current position and put it back at the
            // next scheduled minute.
            //
            m_scheduledJobsQueue.erase( m_scheduledJobsQueue.begin() );

            ScheduledJobsQueue::value_type entry(nextMinuteToStartJob, jobID);
            m_scheduledJobsQueue.insert(entry);

            // tell those who care
            SchedulingMessageSender::getInstance().jobScheduled(
                jobID, job->getSchedule().getNextScheduledTime());
        }
    }

    //
    // If there is any sort of exception, remove the job from the queue,
    // otherwise the processReadyJobs method will loop continuously.
    //
    catch(const TA_Base_Core::DatabaseException& ex)
    {
        SchedulingAlarmSender::getInstance().databaseFailure(ex.what());
        removeCompletedJob(jobID);
    }
    catch(const TA_Base_Core::DataException& ex)
    {
        SchedulingAlarmSender::getInstance().invalidDataConfiguration(
            ex.what());
        removeCompletedJob(jobID);
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException& ex)
    {
        SchedulingAlarmSender::getInstance().invalidDataConfiguration(
            std::string(ex.what));
        removeCompletedJob(jobID);
    }
    catch(const CORBA::Exception& ex)
    {
        std::string action = "execute a job";
        std::string reason = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        if(m_mode == CONTROL_MODE)
        {
            SchedulingAlarmSender::getInstance().schedulingControllerError(
                action, reason);
        }
        else
        {
            SchedulingAlarmSender::getInstance().schedulingMonitorError(
                action, reason);
        }
        removeCompletedJob(jobID);
    }
    catch(const std::exception& ex)
    {
        std::string action = "execute a job";
        std::string reason = ex.what();
        if(m_mode == CONTROL_MODE)
        {
            SchedulingAlarmSender::getInstance().schedulingControllerError(
                action, reason);
        }
        else
        {
            SchedulingAlarmSender::getInstance().schedulingMonitorError(
                action, reason);
        }
        removeCompletedJob(jobID);
    }
    catch(...)
    {
        std::string action = "execute a job";
        std::string reason = "Unknown";
        if(m_mode == CONTROL_MODE)
        {
            SchedulingAlarmSender::getInstance().schedulingControllerError(
                action, reason);
        }
        else
        {
            SchedulingAlarmSender::getInstance().schedulingMonitorError(
                action, reason);
        }
        removeCompletedJob(jobID);
    }

}

void JobQueue::sleepTillNextMinuteStartsOrThreadTerminated()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "Sleeping till next minute starts.");

    //
    // If we are at the top of a minute when we enter this method
    // we want to wait at least one second
    //
    long minWaitMillis = 0;
    unsigned long secondOfMinute = ACE_OS::gettimeofday().sec() % 60u;
    if ( 0u == secondOfMinute )
    {
        minWaitMillis = 1000;
    }

    //
    // Keep checking the time and sleeping for a while until the 
    // the thread is terminated or the minimun wait time
    // is passed and we are at '00' seconds.
    //
    // This while is carefully constructed to ensure that we wake up
    // within SLEEP_TIME_MILLIS after the top of the minute. (currently 100)
    while( m_running && (( minWaitMillis  > 0 ) || ( 0u != ( ACE_OS::gettimeofday().sec() % 60u ))))
    {
        Thread::sleep(SLEEP_TIME_MILLIS);

        minWaitMillis -= SLEEP_TIME_MILLIS;
    }
}


void JobQueue::terminate()
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Terminating JobQueue" );

    m_running = false;
}


}; // TA_Base_App
