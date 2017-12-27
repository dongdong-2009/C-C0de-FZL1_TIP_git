/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingController.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #7 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  * This class encapsulates the logic for the Scheduling Agent in control
  * mode.
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/CorbaException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/SchedulingMessageSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAccessManager.h"
#include "app/scheduling/scheduling_agent/src/JobQueue.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceManager.h"
#include "app/scheduling/scheduling_agent/src/JobFactory.h"
#include "app/scheduling/scheduling_agent/src/JobRunner.h"
#include "app/scheduling/scheduling_agent/src/PlanRunnerJob.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h"

#include "app/scheduling/scheduling_agent/src/SchedulingController.h"

namespace TA_Base_App
{

SchedulingController* SchedulingController::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable SchedulingController::m_singletonLock;


SchedulingController& SchedulingController::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new SchedulingController();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }
    
    return *m_instance;
}


SchedulingController::SchedulingController()
{
    activateServantWithName(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
}


SchedulingController::~SchedulingController()
{
    deactivateAndDeleteServant();
}



void SchedulingController::enable()
{
    FUNCTION_ENTRY("enable");

    m_bEnabled = true;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Enabling Scheduling Controller");

    FUNCTION_EXIT;
}


void SchedulingController::disable()
{
    FUNCTION_ENTRY("disable");

    m_bEnabled = false;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Disabling Scheduling Controller");

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////
//                       SchedulingAgentCorbaDef                       //
/////////////////////////////////////////////////////////////////////////

TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq* SchedulingController::getSupportedJobTypes(const char* /*sessionIdentifier*/)
{
    FUNCTION_ENTRY("getSupportedJobTypes");

    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    checkOperationMode();

    // for now we only support 1 job type
    TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq_var ret = new TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq();

    // TD 8692: Inserted Alarm Raising back into Scheduler so alarms can be raised at predefined intervals for archiving.
    ret->length(1);
    ret[0] = PlanRunnerJob::JOB_TYPE.c_str();
#if !defined(HIDE_ALARM_GENERATOR_JOB)
    ret->length(2);
    ret[1] = AlarmGeneratorJob::JOB_TYPE.c_str();
#endif

    return ret._retn();
}


char* SchedulingController::scheduleJob( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& scheduledJob, const char* sessionIdentifier )
{
    FUNCTION_ENTRY("scheduleJob");

    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    checkOperationMode();

    // Check if the sessionID is valid for this action.

    SchedulingAccessManager::getInstance().checkIfActionIsPermitted(sessionIdentifier, TA_Base_Bus::aca_SCHEDULE_MODIFY);

    std::string jobID = "";

    try
    {
        // Schedule the job.
        jobID = JobQueue::getInstance().scheduleJob(scheduledJob).c_str();
    }
    // Convert TA_Base_Core::DataExceptions and TA_Base_Core::DatabaseExceptions to the corba equivalent
    catch(const TA_Base_Core::DataException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }
    catch(const TA_Base_Core::DatabaseException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }

    // Inform the SchedulingMessageSender
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef newJob(scheduledJob);
    newJob.identifier = jobID.c_str();

    SchedulingMessageSender::getInstance().jobAdded(newJob, sessionIdentifier);

    return CORBA::string_dup(jobID.c_str());
}

void SchedulingController::removeJob(const char* jobID, const char* sessionID)
{
    FUNCTION_ENTRY("removeJob");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removing job with id '%s'.", jobID);

	//TD16352++
	if(JobInstanceManager::getInstance().isJobLocked(jobID, sessionID))
	{
        throw TA_Base_Bus::SchedulingAgentCorbaDef::EditLockException();
	}
	//TD16352++

    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    checkOperationMode();

    // Check if the sessionID is valid for this action.
    SchedulingAccessManager::getInstance().checkIfActionIsPermitted(sessionID, TA_Base_Bus::aca_SCHEDULE_MODIFY);

	if (JobInstanceManager::getInstance().isJobRunning(jobID))
	{
		std::stringstream ss;
		ss << "can not remove this job[" << jobID << "], because it is running now";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", ss.str().c_str() );
        throw TA_Base_Bus::SchedulingAgentCorbaDef::EditLockException(ss.str().c_str());
	}

    try
    {
        // Remove the job with that ID from the JobQueue.
        JobQueue::getInstance().removeJob(jobID, sessionID);
    }
    // Convert TA_Base_Core::DataExceptions and TA_Base_Core::DatabaseExceptions to the corba equivalent
    catch(const TA_Base_Core::DataException& ex)
    {
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }
    catch(const TA_Base_Core::DatabaseException& ex)
    {
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }

}


void SchedulingController::updateJob( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job,
    const char* sessionID, CORBA::Boolean changeEnabled, CORBA::Boolean  checkIfInPast/*=true*/)
{
    FUNCTION_ENTRY("updateJob");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Updating job with id '%s'.", job.identifier.in());

	//TD16352++
	if(JobInstanceManager::getInstance().isJobLocked( job.identifier.in(), sessionID ))
	{
		throw TA_Base_Bus::SchedulingAgentCorbaDef::EditLockException();
	}
	//TD16352++

    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    checkOperationMode();

    // Check if the sessionID is valid for this action.
    SchedulingAccessManager::getInstance().checkIfActionIsPermitted(sessionID, TA_Base_Bus::aca_SCHEDULE_MODIFY);

    try
    {
        // Update the job in the job queue
        JobQueue::getInstance().updateJob(job, checkIfInPast);
    }
    // Convert TA_Base_Core::DataExceptions and TA_Base_Core::DatabaseExceptions to the corba equivalent
    catch(const TA_Base_Core::DataException& ex)
    {
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }
    catch(const TA_Base_Core::DatabaseException& ex)
    {
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }

    // Inform the SchedulingMessageSender.
    SchedulingMessageSender::getInstance().jobUpdated(job, sessionID, changeEnabled);

}

//TD16352++
void SchedulingController::updateEditLock( const char* jobID, const char* sessionID, CORBA::Boolean  bLock)
{
	FUNCTION_ENTRY("getJob");

	if(!JobInstanceManager::getInstance().updateEditLock(jobID, sessionID, bLock))
	{
		throw TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException();
	}

	FUNCTION_EXIT;
}

bool SchedulingController::isJobLockedForEdit( const char* jobID, const char* sessionID )
{
	return JobInstanceManager::getInstance().isJobLocked(jobID, sessionID);
}
//++TD16352


TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef* SchedulingController::getJob(const char* jobID, const char* sessionID)
{
    FUNCTION_ENTRY("getJob");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get job called for job with id '%s'", jobID);

    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    checkOperationMode();

    IJob* job = NULL;

    try
    {
        // Get the job
        job = JobInstanceManager::getInstance().getJob(jobID);
    }
    // Convert TA_Base_Core::DataExceptions and TA_Base_Core::DatabaseExceptions to the corba equivalent
    catch(const TA_Base_Core::DataException& ex)
    {
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }
    catch(const TA_Base_Core::DatabaseException& ex)
    {
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }

    // If the job is NULL, the job does not exist in the job queue.
    if(job == NULL)
    {
        throw TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException();
    }

    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef_var varJob = new TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef(job->toCorbaObject());
    delete job;
    job = NULL;

    return varJob._retn();
}


TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence* SchedulingController::getAllJobs(
    const char* sessionID)
{
    FUNCTION_ENTRY("getAllJobs");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get scheduled jobs called");

    // Check that the Agent is in control mode, and throw an
    // OperationModeException if it isn't
    checkOperationMode();

    try
    {
        // Get all the jobs that have been scheduled and return them.
        return new TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence( JobInstanceManager::getInstance().getAllJobs() );
    }
    //
    // Convert TA_Base_Core::DataExceptions and TA_Base_Core::DatabaseExceptions to the corba equivalent
    //
    catch(const TA_Base_Core::DataException& ex)
    {
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }
    catch(const TA_Base_Core::DatabaseException& ex)
    {
        TA_THROW_CORBA(TA_Base_Core::DatabaseErrorException(ex.what()));
    }
}


void SchedulingController::checkOperationMode()
{
    if( !m_bEnabled )
    {
        TA_THROW_CORBA(TA_Base_Core::OperationModeException());
    }
}


}; // TA_Base_App
