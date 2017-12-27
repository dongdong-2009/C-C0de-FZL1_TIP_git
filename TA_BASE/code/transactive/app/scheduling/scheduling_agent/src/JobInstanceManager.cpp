/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobInstanceManager.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #7 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class keeps track of the instances of jobs that are created by
  * the JobRunner. It is notified by the JobQueue when a job is created
  * or removed, and handles the logic of deleting the pointer to the job.
  *
  */

#include "app/scheduling/scheduling_agent/src/JobInstanceManager.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_App
{

JobInstanceManager* JobInstanceManager::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable JobInstanceManager::m_singletonLock;

JobInstanceManager::JobInstanceManager() :
m_runningJobsMultiMap(),  // Initialise the data structures
m_jobPointersMap()        // to be empty.
{
    JobRunner::getInstance().addJobExecutionListener(this);
}

JobInstanceManager::~JobInstanceManager()
{
}



JobInstanceManager& JobInstanceManager::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new JobInstanceManager();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }
    
    return *m_instance;
}


void JobInstanceManager::startingJob( const std::string& jobID, unsigned long minuteStarted )
{
    FUNCTION_ENTRY("startingJob");

    addJobInstance(jobID, minuteStarted);

    FUNCTION_EXIT;
}

void JobInstanceManager::addJobInstance( const std::string& strJobID, unsigned long minuteStarted )
{
    FUNCTION_ENTRY("addJobInstance");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Job instance added with ID '%s' and minute started '%d'", strJobID.c_str(), minuteStarted);

    {
        TA_Base_Core::ThreadGuard lock( m_lckJobs );

        // Check that the job exists
        if( m_jobPointersMap.find( strJobID ) == m_jobPointersMap.end() )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "Job with ID '%s' and minute started '%d' not found in job pointers multimap. Ignoring it.",
                strJobID.c_str(), minuteStarted);

            return;
        }
    }

    {
        TA_Base_Core::ThreadGuard lock( m_lckRunningJobs );

        // Add the job instance to the running jobs map.
        RunningJobsMultiMap::value_type entry(strJobID, minuteStarted);

        m_runningJobsMultiMap.insert(entry);
    }

    FUNCTION_EXIT;
}


void JobInstanceManager::jobCompleted( const std::string& jobID, unsigned long minuteStarted )
{
    FUNCTION_ENTRY("removeJobInstance");

    removeJobInstance(jobID, minuteStarted);

    FUNCTION_EXIT;
}


void JobInstanceManager::removeJobInstance( const std::string& strJobID, unsigned long minuteStarted )
{
    FUNCTION_ENTRY("removeJobInstance");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Job instance removed with ID '%s' and minute started '%d'", strJobID.c_str(), minuteStarted);

    TA_Base_Core::ThreadGuard lock( m_lckRunningJobs );

    std::pair<RunningJobsMultiMap::iterator, RunningJobsMultiMap::iterator> pirRange = m_runningJobsMultiMap.equal_range( strJobID );

    if ( pirRange.first == m_runningJobsMultiMap.end() )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
            "Removed job with ID '%s' and minute started '%d' not found in running jobs multimap",
            strJobID.c_str(), minuteStarted);

        FUNCTION_EXIT;
        return;
    }

    // Search through the jobs in the map until one is found whose ID and minute started match the parameter.
    for(RunningJobsMultiMap::iterator it = pirRange.first; it != pirRange.second; ++it)
    {
        if ( it->second == minuteStarted )
        {
            m_runningJobsMultiMap.erase( it );
            break;
        }
    }

    FUNCTION_EXIT;
}

//TD16352++
bool JobInstanceManager::updateEditLock( const std::string& strjobID, std::string strSessionId, bool bLock )
{
	FUNCTION_ENTRY("updateEditLock");

    // Lock the object.
    TA_Base_Core::ThreadGuard lock( m_lckJobs );

	JobPointersMap::iterator jobIt = m_jobPointersMap.find(strjobID);

	if( jobIt == m_jobPointersMap.end() )
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
            "[TD16352]Call to lock for edit but no job exists with jobid %s", strjobID.c_str());
		FUNCTION_EXIT;
		return false;
    }

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "[TD16352] jobid %s locked by %s", strjobID.c_str(),strSessionId.c_str());

	jobIt->second.isLockedForEdit = bLock;
	if(bLock == false)
	{
		jobIt->second.strSessionId = "";
	}
	else
	{
		jobIt->second.strSessionId = strSessionId;
	}
	
	FUNCTION_EXIT;
	return true;
}

bool JobInstanceManager::isJobLocked(const std::string& strjobID, std::string strSessionId)
{
	FUNCTION_ENTRY("isJobLocked");

	bool bReturn=false;

    // Lock the object.
    TA_Base_Core::ThreadGuard lock( m_lckJobs );

	JobPointersMap::iterator jobIt = m_jobPointersMap.find(strjobID);

	if( jobIt == m_jobPointersMap.end() )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
            "[TD16352]Job id %s not found in m_jobPointersMap", strjobID.c_str());

        FUNCTION_EXIT;
        return bReturn;
    }

	if(( jobIt->second.strSessionId != strSessionId) && jobIt->second.isLockedForEdit && !jobIt->second.hasBeenRemoved )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "[TD16352] jobid %s locked by %s", strjobID.c_str(),strSessionId.c_str());
		bReturn = true;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[TD16352] jobid %s not locked", strjobID.c_str());

	FUNCTION_EXIT;
	return bReturn;
}
//++TD16352

bool JobInstanceManager::isJobRunning(const std::string& strJobID)
{
    FUNCTION_ENTRY("isJobRunning");

    TA_Base_Core::ThreadGuard lock( m_lckRunningJobs );

    if ( m_runningJobsMultiMap.find( strJobID ) != m_runningJobsMultiMap.end() )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Job [%s] is Running", strJobID.c_str());

        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


void JobInstanceManager::removeAllJobInstances()
{
    FUNCTION_ENTRY("removeAllJobInstances");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removing all job instances");

    TA_Base_Core::ThreadGuard lock( m_lckRunningJobs );

    m_runningJobsMultiMap.clear();

    FUNCTION_EXIT;
}


void JobInstanceManager::jobCreated(IJob* job)
{
    FUNCTION_ENTRY("jobCreated");

    if (NULL == job)
    {
        return;
    }

    // Lock the object.

    std::string strJobID = job->getID().c_str();

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Job created with ID '%s'", strJobID.c_str());

    JobInfo jobInfo;
    jobInfo.job = job;
    jobInfo.hasBeenRemoved = false;
	jobInfo.isLockedForEdit = false;

    {
        TA_Base_Core::ThreadGuard lock(m_lckJobs);
        m_jobPointersMap.insert(std::make_pair( strJobID, jobInfo));
    }

    FUNCTION_EXIT;
}


void JobInstanceManager::jobRemoved(const std::string& strJobID)
{
    FUNCTION_ENTRY("jobRemoved");

    bool bJobRunning = isJobRunning( strJobID );

    if ( !bJobRunning )
    {
        JobRunner::getInstance().removeJob( strJobID );
    }

    // Lock the object.
    {
        TA_Base_Core::ThreadGuard lock(m_lckJobs);

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Job removed with ID '%s'", strJobID.c_str());

        JobPointersMap::iterator it = m_jobPointersMap.find( strJobID );

        // Check that the job exists in the job pointers map.
        if( it == m_jobPointersMap.end() )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "Job with ID '%s' is not found in job pointers map. This may occur if the job has already completed running", strJobID.c_str() );

            return;
        }

        // If there are no instances of the job running, erase it.
        if( !bJobRunning )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Deleting pointer to job with ID %s", strJobID.c_str());

            if ( NULL != it->second.job )
            {
                delete it->second.job;
                it->second.job = NULL;
            }

            m_jobPointersMap.erase(it);
        }
        // If there are still instances of the job running, mark it as removed.
        else
        {
            it->second.hasBeenRemoved = true;
        }
    }

    FUNCTION_EXIT;
}

void JobInstanceManager::jobDeleted( const std::string& jobID )
{
    FUNCTION_ENTRY("jobDeleted");

    deleteRemovedJob( jobID );

    FUNCTION_EXIT;
}


IJob* JobInstanceManager::getJob(const std::string& strjobID)
{
    FUNCTION_ENTRY("getJob");

    TA_Base_Core::ThreadGuard lock(m_lckJobs);

	JobPointersMap::const_iterator it = m_jobPointersMap.find(strjobID);

    if( it != m_jobPointersMap.end() )
    {
        return it->second.job;
    }

	FUNCTION_EXIT;
    return NULL;
}



TA_Base_Bus::SchedulingAgentCorbaDef::BatchUpdateCorbaDef JobInstanceManager::getBatchUpdate()
{
    FUNCTION_ENTRY("getBatchUpdate");

    TA_Base_Bus::SchedulingAgentCorbaDef::BatchUpdateCorbaDef batchUpdate;

    batchUpdate.jobs = getAllJobs();
    batchUpdate.jobInstances = getJobInstances();

    FUNCTION_EXIT;
    return batchUpdate;
}


TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence JobInstanceManager::getAllJobs()
{
    FUNCTION_ENTRY("getAllJobs");

    TA_Base_Core::ThreadGuard lock(m_lckJobs);

    // Go through the map and convert all the IJob pointers into
    // ScheduledJobCorbaDefs, and put them into a sequence.
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Got %d jobs in total", m_jobPointersMap.size());

    TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence jobSequence;

    unsigned int i = 0;
    for(JobPointersMap::const_iterator it = m_jobPointersMap.begin(); it != m_jobPointersMap.end(); ++it)
    {
        if ( NULL == it->second.job )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Job is NULL");
            continue;
        }

#if defined(HIDE_ALARM_GENERATOR_JOB)
        // if we're supposed to be hiding the alarm generator jobs, then don't add it to the
        // list it it is one
        if (it->second.job->getJobType() == AlarmGeneratorJob::JOB_TYPE)
        {
            continue;
        }
#endif
        jobSequence.length(i+1);
        jobSequence[i] = it->second.job->toCorbaObject();

        ++i;
    }

    FUNCTION_EXIT;
    return jobSequence;
}


TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceSequence JobInstanceManager::getJobInstances()
{
    FUNCTION_ENTRY("getJobInstances");

    TA_Base_Core::ThreadGuard lock(m_lckRunningJobs);

    // Create the sequence and copy in the job ID and minute started
    TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceSequence instances;
    instances.length( m_runningJobsMultiMap.size() );

    unsigned int i = 0;
    for(RunningJobsMultiMap::const_iterator it = m_runningJobsMultiMap.begin(); it != m_runningJobsMultiMap.end(); ++it)
    {
		instances[i].jobIdentifier = it->first.c_str();
        instances[i].minuteStarted = it->second;

        ++i;
    }

    FUNCTION_EXIT;
    return instances;
}

void JobInstanceManager::deleteRemovedJob( const std::string& jobID )
{
    FUNCTION_ENTRY("deleteRemovedJob");

    // If there are no other instances of the job running, and if the job has been marked as removed, delete it.
    if( isJobRunning( jobID ))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Job [%s] is still running while deleting", jobID.c_str());

        FUNCTION_EXIT;
        return;
    }

    // Lock for jobs
    {
        TA_Base_Core::ThreadGuard lock(m_lckJobs);

        JobPointersMap::iterator jobIt = m_jobPointersMap.find(jobID);

        if( jobIt == m_jobPointersMap.end() )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Removing job [%s] but no job exist.", jobID.c_str() );

            FUNCTION_EXIT;
            return;
        }

        if( jobIt->second.hasBeenRemoved )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Deleting pointer to job with ID '%s'", jobID.c_str());

            if ( NULL != jobIt->second.job )
            {
                delete jobIt->second.job;
                jobIt->second.job = NULL;
            }

            m_jobPointersMap.erase(jobIt);
        }
    }

    FUNCTION_EXIT;
}

};// TA_Base_App

