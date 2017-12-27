/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AlarmGeneratorJobFactory.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * Implements the IJobFactory interface to provide a factory that creates
  * alarm generator jobs.
  *
  */



#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobFactory.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h"


namespace TA_Base_App
{


void AlarmGeneratorJobFactory::initialise( TA_Base_Core::ISchedulingAgentEntityData& entityData )
{
    // This job type has no initialization
}


IJob* AlarmGeneratorJobFactory::createJob( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Create Job request for Alarm Generator" );

	if(AlarmGeneratorJob::JOB_TYPE.compare(job.type.in()) != 0)
    {
        return NULL;
    }

    // TODO: verify that the expected parameters are present.

    // Create a new AlarmGeneratorJob. This will also create an entry in
    // the database.
    
    AlarmGeneratorJob* iJob = new AlarmGeneratorJob(job);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "Alarm Generator job created with id %s, locationID %d, profileID %d, isInhibited %d, job description '%s', schedule '%s', severity '%d', alarm description '%s'",
        iJob->getID().c_str(), iJob->getLocationID(), iJob->getProfileID(), iJob->isInhibited(), iJob->getDescription().c_str(), iJob->getSchedule().toString().c_str(),
        iJob->getAlarmSeverityKey(), iJob->getAlarmDescription().c_str());

    return iJob;
}

IJob::IJobPtrVector AlarmGeneratorJobFactory::createJobsFromStore()
{
    // Get the database objects for all jobs that are alarm generators
    TA_Base_Core::JobDataAccessFactory::IConfigJobDataVector jobDataObjects =
        TA_Base_Core::JobDataAccessFactory::getInstance().getAllJobsOfType( AlarmGeneratorJob::JOB_TYPE );

    // Create a vector that will hold the IJob pointers
    IJob::IJobPtrVector jobs;
    jobs.reserve(jobDataObjects.size());

    for( TA_Base_Core::JobDataAccessFactory::IConfigJobDataVector::iterator it = jobDataObjects.begin(); it != jobDataObjects.end(); ++it )
    {
        // Get the ID of the Alarm Generator Job in the database and create
        // an AlarmGeneratorJob object from it.
        jobs.push_back( new AlarmGeneratorJob((*it)->getID()) );

        delete *it;
    }

    return jobs;
}


IJob* AlarmGeneratorJobFactory::getJobFromStore(const std::string& id)
{
    // Get the database object for the job
    TA_Base_Core::IConfigJobData* jobData = NULL;
    try
    {
        jobData = TA_Base_Core::JobDataAccessFactory::getInstance().getJob(id);
    }
    // If the job does not exist in the database, let the other JobFactories
    // try to retrieve it from whatever store they use.
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException&)
    {
    }

    if ( NULL == jobData )
    {
        return NULL;
    }

    const std::string strJobType = jobData->getJobType();
    const std::string strJobId = jobData->getID().c_str();
    delete jobData;
    jobData = NULL;

    IJob* job = NULL;

    // If the job exists, and it's of the correct type
    if( strJobType.compare( AlarmGeneratorJob::JOB_TYPE ) == 0 )
    {
        // Create an AlarmGeneratorJob object using the ID of the job.
        try
        {
            job = new AlarmGeneratorJob( strJobId );
            TA_ASSERT(job != NULL, "job should never be NULL");
        }
		catch(const TA_Base_Core::TransactiveException& ex)
		{
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", ex.what() );
            throw;
		}
        catch(...)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception");
            throw;
        }
    }

    return job;
}


AlarmGeneratorJobFactory::~AlarmGeneratorJobFactory()
{
}


AlarmGeneratorJobFactory::AlarmGeneratorJobFactory()
{
}


}// TA_Base_App

