/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AbstractJob.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This abstract class represents an executable job.
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.h"
#include "core/data_access_interface/scheduling_agent/src/IJobData.h"
#include "core/exceptions/src/DataException.h"
#include "app/scheduling/scheduling_agent/src/AbstractJob.h"


namespace TA_Base_App
{

TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef AbstractJob::toCorbaObject()
{
    FUNCTION_ENTRY("toCorbaObject");

    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef corbaJob;

    corbaJob.identifier = getID().c_str();
    corbaJob.profileIdentifier = getProfileID();
    corbaJob.locationIdentifier = getLocationID();
    corbaJob.isInhibited = isInhibited();
    corbaJob.description = getDescription().c_str();
    corbaJob.type = getJobType().c_str();
    corbaJob.schedule = getSchedule().toCorbaObject();
    corbaJob.runTime = Schedule::getNextScheduledTime( corbaJob.schedule );

    // copy the job data
    TA_Base_Core::IJobData::ParameterMap data = getParameters();

    corbaJob.data.length(data.size());

    unsigned long i = 0;
    for (TA_Base_Core::IJobData::ParameterMap::const_iterator p = data.begin(); p != data.end(); ++p)
    {
        corbaJob.data[i].name = p->first.c_str();
        corbaJob.data[i].value = p->second.c_str();
        ++i;
    }

    return corbaJob;
}

Schedule& AbstractJob::getSchedule()
{
    return *m_schedule;
}

std::string AbstractJob::getJobType() const
{
    return m_jobData->getJobType();
}

std::string AbstractJob::getDescription() const
{
    return m_jobData->getDescription();
}

bool AbstractJob::isInhibited() const
{
    return m_jobData->getInhibitionStatus();
}

std::string AbstractJob::getID() const
{
    return m_jobData->getID();
}

unsigned long AbstractJob::getLocationID() const
{
    return m_jobData->getLocationID();
}

unsigned long AbstractJob::getProfileID() const
{
    return m_jobData->getProfileID();
}

std::string AbstractJob::getParameter(const std::string& parameterName) const
{
    return m_jobData->getParameter(parameterName);
}

TA_Base_Core::IJobData::ParameterMap AbstractJob::getParameters()
{
    return m_jobData->getParameters();
}

void AbstractJob::setParameter(const std::string& name, const std::string& value)
{
    m_jobData->setParameter(name, value);
    m_jobData->applyChanges();
}

void AbstractJob::deleteParameter(const std::string& name)
{
    m_jobData->deleteParameter(name);
    m_jobData->applyChanges();
}

void AbstractJob::reloadDataFromDB(bool forceReload/* = false*/)
{
    m_jobData->reloadDataFromDB( forceReload );
}

void AbstractJob::invalidate()
{
    m_jobData->invalidate();
}

void AbstractJob::saveJob()
{
    // save the job first as the schedule requires it in the DB
    m_jobData->saveJob();
    m_jobData->getSchedule()->saveSchedule();
}

void AbstractJob::deleteJob()
{
    m_jobData->deleteJob();
}

void AbstractJob::updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    FUNCTION_ENTRY("updateJob");

    updateJobDetails(job);

    m_jobData->applyChanges();
}

void AbstractJob::updateJobDetails(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    FUNCTION_ENTRY("updateJobDetails");
    m_jobData->setDescription( job.description.in() );
    m_jobData->setJobType( job.type.in() );
    m_jobData->setLocationID( job.locationIdentifier );
    m_jobData->setProfileID( job.profileIdentifier );
    m_jobData->setInhibitionStatus ( job.isInhibited );

    if ( job.data.length() != 0 )
    {
        unsigned int i = job.data.length();
        while (i--)
        {
            m_jobData->setParameter( job.data[i].name.in(), job.data[i].value.in() );
        }
    }

    TA_Base_Core::IConfigScheduleData* scheduleData = m_jobData->getSchedule();
    TA_ASSERT(scheduleData != NULL, "Schedule should never be NULL");

    if ( job.schedule.daysOfWeek.length() != 0 )
    {
        std::list<unsigned short> days;
        unsigned int i = job.schedule.daysOfWeek.length();
        while (i--)
        {
            days.push_back( job.schedule.daysOfWeek[i] );
        }
        scheduleData->setDaysOfWeek( days );
    }
    scheduleData->setDayOfMonth( job.schedule.dayOfMonth );
    scheduleData->setEndDay( job.schedule.endDay );
    scheduleData->setEndMonth( job.schedule.endMonth );
    scheduleData->setEndYear( job.schedule.endYear );
    scheduleData->setHour( job.schedule.hour );
    scheduleData->setMinute( job.schedule.minute );
    scheduleData->setMonthOfYear( job.schedule.month );
    scheduleData->setScheduleType( job.schedule.type );
    scheduleData->setYear( job.schedule.year );
}

AbstractJob::AbstractJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    FUNCTION_ENTRY("AbstractJob(job)");
    //
    // Create a new job, filling the details
    //
    m_jobData = TA_Base_Core::JobDataAccessFactory::getInstance().createJob();

    updateJobDetails(job);

    TA_Base_Core::IConfigScheduleData* scheduleData = m_jobData->getSchedule();

    if ( NULL != scheduleData )
    {
        m_schedule = new Schedule( *scheduleData );
    }
}

AbstractJob::AbstractJob(const std::string& jobID)
{
    FUNCTION_ENTRY("AbstractJob(jobID)");
    m_jobData = TA_Base_Core::JobDataAccessFactory::getInstance().getJob(jobID);
    TA_ASSERT(m_jobData != NULL, "Job data should never be NULL");

    // scheduleData is managed by m_jobData
    TA_Base_Core::IConfigScheduleData* scheduleData = m_jobData->getSchedule();
    TA_ASSERT(scheduleData != NULL, "Schedule should never be NULL");

    m_schedule = new Schedule(*scheduleData);
    TA_ASSERT(m_schedule != NULL, "m_schedule should never be NULL");
}

AbstractJob::~AbstractJob()
{
    FUNCTION_ENTRY("~AbstractJob");
    // The schedule should be deleted first as it contains a reference
    // to m_jobData->getSchedule(), which should be cleaned up first
    delete m_schedule;
    m_schedule = NULL;
    
    delete m_jobData;
    m_jobData = NULL;
}

}; // TA_Base_App

