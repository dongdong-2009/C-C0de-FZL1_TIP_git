/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobFactory.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/06/03 11:49:57 $
  * Last modified by:  $Author: huangjian $
  *
  * This class is used to create instances of IJob.
  *
  */


#include "core/utilities/src/TAAssert.h"
#include "app/scheduling/scheduling_agent/src/PlanRunnerJobFactory.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobFactory.h"

#include "app/scheduling/scheduling_agent/src/JobFactory.h"



typedef TA_Base_App::IJob::IJobPtrVector IJobPtrVector;
typedef TA_Base_Bus::SchedulingAgentCorbaDef::UnrecognisedJobTypeException UnrecognisedJobTypeException;


namespace TA_Base_App
{

JobFactory* JobFactory::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable JobFactory::m_singletonLock;


JobFactory& JobFactory::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new JobFactory();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }
    
    return *m_instance;
}


JobFactory::JobFactory()
    :
    m_jobFactories()   // Initialise the vector to be empty
{
    //
    // All factories that this class knows about should be created here.
    //
    m_jobFactories.push_back( new PlanRunnerJobFactory() );

    // TD 8692: Added Alarm generation back in for archiving.
    m_jobFactories.push_back( new AlarmGeneratorJobFactory() );
}

JobFactory::~JobFactory()
{
}


void JobFactory::initialise( TA_Base_Core::ISchedulingAgentEntityData& entityData )
{
    //
    // Go through each IJobFactory this class knows about and initialise it
    //
    for(IJobFactoryPtrVector::const_iterator it = m_jobFactories.begin();
        it != m_jobFactories.end();
        it ++)
    {
        IJobFactory* subclassFactory = *it;

        TA_ASSERT( subclassFactory != NULL,
                   "Subclass factory must not be NULL" );

        subclassFactory->initialise(entityData);
    }
}


IJob* JobFactory::createJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaJob)
{
    FUNCTION_ENTRY("createJob");
    IJob* job = NULL;

    // Go through each IJobFactory this class knows about, and see if it will
    // convert the CORBA job to an IJob.
    for(IJobFactoryPtrVector::const_iterator it = m_jobFactories.begin(); it != m_jobFactories.end() && job == NULL; it ++)
    {
        IJobFactory* subclassFactory = *it;

        if ( NULL == subclassFactory )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Subclass factory must not be NULL" );
            continue;
        }

        job = subclassFactory->createJob(corbaJob);
    }

    // If no factory can create the IJob, throw an exception.
    if( job == NULL )
    {
        std::string message = "The job type was not recognised";

        FUNCTION_EXIT;
        throw UnrecognisedJobTypeException(message.c_str());
    }

    FUNCTION_EXIT;
    return job;
}


IJob::IJobPtrVector JobFactory::createJobsFromStore()
{
    FUNCTION_ENTRY("createJobsFromStore");

    IJobPtrVector allJobs;

    //
    // Go through each IJobFactory this class knows about,
    // and retrieve all IJobs of the type that it creates, from the database.
    //
    for(IJobFactoryPtrVector::const_iterator it = m_jobFactories.begin();
        it != m_jobFactories.end();
        it ++)
    {
        IJobFactory* subclassFactory = *it;

        if ( NULL == subclassFactory )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Subclass factory must not be NULL" );
            continue;
        }

        IJobPtrVector newJobs = subclassFactory->createJobsFromStore();

        allJobs.reserve(allJobs.size() + newJobs.size());
        allJobs.insert( allJobs.end(), newJobs.begin(), newJobs.end() );
    }

    FUNCTION_EXIT;
    return allJobs;
}


IJob* JobFactory::getJobFromStore(const std::string& id)
{
    FUNCTION_ENTRY("getJobFromStore");

    IJob* job = NULL;

    //
    // Go through each IJobFactory this class knows about, and see if it will
    // retrieve the job with the given ID
    //
    for(IJobFactoryPtrVector::const_iterator it = m_jobFactories.begin();
        it != m_jobFactories.end()   &&    job == NULL;
        it ++)
    {
        IJobFactory* subclassFactory = *it;

        if ( NULL == subclassFactory )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Subclass factory must not be NULL" );
            continue;
        }

        job = subclassFactory->getJobFromStore(id);
    }

    // If no factory can create the IJob, throw an exception.
    if( job == NULL )
    {
        std::ostringstream message;
        message << "A job with id " << id << " could not be found";

        FUNCTION_EXIT;
        throw TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException(
            message.str().c_str());
    }

    FUNCTION_EXIT;
    return job;
}



};// TA_Base_App

