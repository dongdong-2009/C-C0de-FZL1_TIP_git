/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobRunner.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #10 $
  *
  * Last modification: $DateTime: 2013/11/01 15:29:57 $
  * Last modified by:  $Author: huangjian $
  *
  * Used by the JobQueue to execute jobs.
  *
  */

// Disable warnings from boost
#if defined(_MSC_VER)
  #pragma warning(disable:4244) // conversion - possible loss of data
#endif // _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "app/scheduling/scheduling_agent/src/IJobInstance.h"
#include "app/scheduling/scheduling_agent/src/IJobExecutionListener.h"
#include "app/scheduling/scheduling_agent/src/IJob.h"
#include "app/scheduling/scheduling_agent/src/JobThread.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingSessionManager.h"
#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"
#include "app/scheduling/scheduling_agent/src/JobRunner.h"


namespace TA_Base_App
{

JobRunner* JobRunner::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable JobRunner::m_singletonLock;


JobRunner& JobRunner::getInstance()
{
    if ( NULL == m_instance )
    {
        TA_THREADGUARD(m_singletonLock);

        if ( NULL == m_instance )
        {
            m_instance = new JobRunner();
        }
    }
    
    return *m_instance;
}

JobRunner::~JobRunner()
{
}

JobRunner::JobRunner() :
m_listenerList(),
m_lckExeListeners(),
m_mapJobThreads(),
m_running(false)
{
}


void JobRunner::run()
{
    m_running = true;

    int i = 1;
    while(m_running)
    {
        // Every ten seconds, terminate and delete and JobThreads that have completed.
        Thread::sleep(1000);

        if(i >= 10)
        {
            terminateAndDeleteFinishedJobThreads();
            i = 1;
        }
        else
        {
            i ++;
        }
    }

    // Make sure any left over threads are cleaned up before the thread exits.
    terminateAndDeleteFinishedJobThreads();
}


void JobRunner::terminate()
{
    m_running = false;
}


void JobRunner::executeJob(IJob* job)
{
    if (job == NULL)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid input parameter - NULL pointer of IJob" );
		return;
    }

    SchedulingTimer now;
    unsigned long minuteStarted = now.getTimeInMinutesSinceEpoch();

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Executing job with id %s at minute %d", job->getID().c_str(), minuteStarted);

    // Create a thread in which to run the job,
    // inform the listeners that the job is about to start, and start the job.
    JobThread* pNewJob = new JobThread( job->createJobInstance(minuteStarted) );

    pNewJob->markAsDeleted( false );

    fireStartingJobEvent(job->getID(), minuteStarted);

    {
        // Insert into running thread map
        TA_THREADGUARD( m_lckJobThreads );

        JobIdToMinutedJobThreadMap::iterator itMinuteThread = m_mapJobThreads.find( job->getID() );

        if ( itMinuteThread == m_mapJobThreads.end() )
        {
            MinuteStartToJobThreadMap mapNewJobThreads;
            itMinuteThread = m_mapJobThreads.insert( JobIdToMinutedJobThreadMap::value_type(job->getID(), mapNewJobThreads) ).first;
        }

        if ( itMinuteThread != m_mapJobThreads.end() )
        {
            itMinuteThread->second.insert( MinuteStartToJobThreadMap::value_type(minuteStarted, pNewJob) );
        }
    }

    pNewJob->start();
}


void JobRunner::removeJob( const std::string& strJobId )
{
    std::vector<JobThread*> vecToBeDeletedJobs;

    {
        TA_THREADGUARD( m_lckJobThreads );

        JobIdToMinutedJobThreadMap::iterator itMinuteThread = m_mapJobThreads.find( strJobId );

        if ( itMinuteThread == m_mapJobThreads.end() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Job %s is not in execution list", strJobId.c_str() );
            return;
        }

        for ( MinuteStartToJobThreadMap::iterator itThreadLoop = itMinuteThread->second.begin(); itMinuteThread->second.end() != itThreadLoop; ++itThreadLoop )
        {
            vecToBeDeletedJobs.push_back( itThreadLoop->second );
        }

        itMinuteThread->second.clear();

        m_mapJobThreads.erase( itMinuteThread );
    }

    for ( std::vector<JobThread*>::iterator itLoopThread = vecToBeDeletedJobs.begin(); vecToBeDeletedJobs.end() != itLoopThread; ++itLoopThread )
    {
        if ( NULL != *itLoopThread )
        {
			if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == (*itLoopThread)->getCurrentState() )
			{
				(*itLoopThread)->terminateAndWait();
			}

			delete *itLoopThread;
			*itLoopThread = NULL;
        }
    }

    vecToBeDeletedJobs.clear();
}

void JobRunner::jobCompleted( const unsigned long ulStartTime, const std::string& strJobId )
{
    // Inform the listeners that the job has completed and clean up.
    // No matter there is error message or not,
    // Always remove the JobInstance from JobInstanceManager once the job is completed,
    // otherwise user can not be able to delete the job in Scheduling Manager
    fireJobCompletedEvent( strJobId, ulStartTime );

    {
        TA_THREADGUARD( m_lckJobThreads );

        JobIdToMinutedJobThreadMap::const_iterator itMinuteThread = m_mapJobThreads.find( strJobId );

        if ( itMinuteThread == m_mapJobThreads.end() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Job %s is not in execution list", strJobId.c_str() );
            return;
        }

        MinuteStartToJobThreadMap::const_iterator itThread = itMinuteThread->second.find( ulStartTime );
        if ( itThread == itMinuteThread->second.end() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Job %s started at %d is not in execution list", strJobId.c_str(), ulStartTime );
            return;
        }

        itThread->second->markAsDeleted();
    }
}

void JobRunner::terminateAndDeleteFinishedJobThreads()
{
    std::vector<JobThread*> vecToBeDeletedJobs;

    {
        TA_THREADGUARD( m_lckJobThreads );

        for ( JobIdToMinutedJobThreadMap::iterator itMinuteLoop = m_mapJobThreads.begin(); m_mapJobThreads.end() != itMinuteLoop; )
        {
            for ( MinuteStartToJobThreadMap::iterator itThreadLoop = itMinuteLoop->second.begin(); itMinuteLoop->second.end() != itThreadLoop; )
            {
                if ( NULL == itThreadLoop->second )
                {
                    ++itThreadLoop;
                    continue;
                }

                if ( itThreadLoop->second->isMarkDeleted() )
                {
                    vecToBeDeletedJobs.push_back( itThreadLoop->second );

                    itMinuteLoop->second.erase( itThreadLoop++ );
                }
                else
                {
                    ++itThreadLoop;
                }
            }

            if ( itMinuteLoop->second.empty() )
            {
                // Check and remove job
                fireJobDeletedEvent( itMinuteLoop->first );

                m_mapJobThreads.erase( itMinuteLoop++ );
            }
            else
            {
                ++itMinuteLoop;
            }
        }
    }

    for ( std::vector<JobThread*>::iterator itLoopThread = vecToBeDeletedJobs.begin(); vecToBeDeletedJobs.end() != itLoopThread; ++itLoopThread )
    {
        if ( NULL != *itLoopThread )
        {
			if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == (*itLoopThread)->getCurrentState() )
			{
				(*itLoopThread)->terminateAndWait();
			}

			delete *itLoopThread;
			*itLoopThread = NULL;
        }
    }

    vecToBeDeletedJobs.clear();
}



void JobRunner::fireStartingJobEvent( const std::string& jobID, unsigned long minuteStarted )
{
    TA_THREADGUARD(m_lckExeListeners);

    for(JobExecutionListenerList::iterator it = m_listenerList.begin(); it != m_listenerList.end(); it ++)
    {
        if ( NULL == *it )
        {
            continue;
        }

        (*it)->startingJob(jobID, minuteStarted);
    }
}


void JobRunner::fireJobCompletedEvent( const std::string& jobID, unsigned long minuteStarted )
{
    TA_THREADGUARD(m_lckExeListeners);

    for(JobExecutionListenerList::iterator it = m_listenerList.begin(); it != m_listenerList.end(); it ++)
    {
        if ( NULL == *it )
        {
            continue;
        }

        (*it)->jobCompleted(jobID, minuteStarted);
    }
}


void JobRunner::fireJobDeletedEvent( const std::string& jobID )
{
    TA_THREADGUARD(m_lckExeListeners);

    for(JobExecutionListenerList::iterator it = m_listenerList.begin(); it != m_listenerList.end(); it ++)
    {
        if ( NULL == *it )
        {
            continue;
        }

        (*it)->jobDeleted(jobID);
    }
}


void JobRunner::addJobExecutionListener(IJobExecutionListener* listener)
{
    // Lock the method
    TA_THREADGUARD(m_lckExeListeners);

    m_listenerList.push_back(listener);
}


void JobRunner::removeJobExecutionListener(IJobExecutionListener* listener)
{
    // Lock the method
    TA_THREADGUARD(m_lckExeListeners);

    m_listenerList.remove(listener);
}

}; // TA_Base_App

