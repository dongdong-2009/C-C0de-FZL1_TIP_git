/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/PlanRunnerJobFactory.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #7 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * Implements the IJobFactory interface to provide a factory that creates
  * plan runner jobs.
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/PlanRunnerJob.h"
#include "app/scheduling/scheduling_agent/src/PlanRunnerJobFactory.h"


namespace TA_Base_App
{


PlanAgentMap PlanRunnerJobFactory::m_planAgentMap;
TA_Base_Bus::PlanAgentLibrary* PlanRunnerJobFactory::m_defaultPlanAgent = NULL;

void PlanRunnerJobFactory::initialise(TA_Base_Core::ISchedulingAgentEntityData& entityData )
{
    unsigned long planAgentLocationKey  = entityData.getPlanAgentLocationKey();

    m_defaultPlanAgent = new TA_Base_Bus::PlanAgentLibrary(planAgentLocationKey);
}



IJob* PlanRunnerJobFactory::createJob( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job )
{
    FUNCTION_ENTRY("createJob");

	// only proceed if of the right type
    std::string jobtype(job.type);

    if( jobtype != PlanRunnerJob::JOB_TYPE )
	{
        return NULL;
	}
    
    // Create a new PlanRunnerJob. 
    PlanRunnerJob* iJob = new PlanRunnerJob(job);

    // and save to db if no problems
    iJob->saveJob();

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Plan Runner job created with id %s, locationID %d, profileID %d, isInhibited %d, description '%s', schedule '%s', plan %s.",
        iJob->getID().c_str(), iJob->getLocationID(), iJob->getProfileID(), iJob->isInhibited(), iJob->getDescription().c_str(),
        iJob->getSchedule().toString().c_str(), iJob->getPlanPath().c_str());

    return iJob;
}

IJob::IJobPtrVector PlanRunnerJobFactory::createJobsFromStore()
{
    FUNCTION_ENTRY("createJobsFromStore");
    //
    // Get the database objects for all jobs that are plans
    //
    TA_Base_Core::JobDataAccessFactory::IConfigJobDataVector jobDataObjects =
        TA_Base_Core::JobDataAccessFactory::getInstance().getAllJobsOfType(PlanRunnerJob::JOB_TYPE);

    //
    // Create a vector that will hold the IJob pointers
    //
    IJob::IJobPtrVector jobs;
    jobs.reserve(jobDataObjects.size());

    for(TA_Base_Core::JobDataAccessFactory::IConfigJobDataVector::iterator it = jobDataObjects.begin(); it != jobDataObjects.end(); it ++)
    {
        // Get the ID of the Plan Runner Job in the database and create a PlanRunnerJob object from it.
        jobs.push_back( new PlanRunnerJob((*it)->getID()) );

        delete *it;
    }

    return jobs;
}


IJob* PlanRunnerJobFactory::getJobFromStore(const std::string& id)
{
    FUNCTION_ENTRY("getJobFromStore");

    // Get the database object for the job
    TA_Base_Core::IConfigJobData* jobData = NULL;
    try
    {
        jobData = TA_Base_Core::JobDataAccessFactory::getInstance().getJob(id);
    }
    // If the job does not exist in the database, let the other JobFactories, try to retrieve it from whatever store they use.
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException&)
    {
    }

    IJob* job = NULL;

    // If the job exists, and it's of the correct type
    if(jobData != NULL && jobData->getJobType() == PlanRunnerJob::JOB_TYPE)
    {
        // Create a PlanRunnerJob object using the ID of the job.
        try
        {
            job = new PlanRunnerJob( jobData->getID().c_str() );
            TA_ASSERT(job != NULL, "job should never be NULL");
        }
        catch(...)
        {
            delete jobData;
			jobData = NULL;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception");
            throw;
        }
    }

    delete jobData;

    return job;
}


TA_Base_Bus::PlanAgentLibrary* PlanRunnerJobFactory::getPlanAgent(unsigned long locationKey)
{
    TA_Base_Bus::PlanAgentLibrary* pPlanAgent = NULL;
	// Location key will be 0 for non-location-specific profiles, eg. Super
	if(locationKey == 0)
	{
		pPlanAgent = m_defaultPlanAgent;
	}
	else
	{
		pPlanAgent = &(m_planAgentMap.getPlanAgent(locationKey));
	}

    if ( NULL == pPlanAgent )
    {
        std::stringstream stmError;
        stmError << "planAgent not exist for location " << locationKey;
        TA_THROW(TA_Base_Core::TransactiveException( stmError.str().c_str() ));
    }

    return pPlanAgent;
}

PlanRunnerJobFactory::~PlanRunnerJobFactory()
{
}


PlanRunnerJobFactory::PlanRunnerJobFactory()
{
}


};// TA_Base_App

