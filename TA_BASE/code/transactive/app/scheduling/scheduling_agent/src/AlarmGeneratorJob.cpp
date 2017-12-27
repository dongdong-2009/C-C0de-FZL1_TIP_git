/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AlarmGeneratorJob.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class implements the IJob interface by extending the Job class
  * to create a Job that can be used to generate alarms
  *
  */



#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceNotFoundException.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceStateRetrievalException.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobInstance.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobFactory.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h"


namespace TA_Base_App
{


const std::string AlarmGeneratorJob::JOB_TYPE = "AlarmGeneratorJob";
const std::string AlarmGeneratorJob::ALARM_SEVERITY_KEY_PARAMETER_NAME = "AlarmSeverityKey";
const std::string AlarmGeneratorJob::ALARM_DESCRIPTION_PARAMETER_NAME  = "AlarmDescription";


IJobInstance* AlarmGeneratorJob::createJobInstance(
    unsigned long minuteStarted)
{
    FUNCTION_ENTRY("createJobInstance");

    FUNCTION_EXIT;
    return new AlarmGeneratorJobInstance(this, minuteStarted);
}

IJobInstance::JobInstanceState AlarmGeneratorJob::getJobInstanceState( unsigned long minuteStarted, std::string& errorMessage ) const
{
    FUNCTION_ENTRY("getJobInstanceState");

    //
    // This method is called after a failover to determine the state of
    // a job that should have been run.
    // It is assumed that an alarm takes such an insignificant time to be
    // raised that it will always have been raised when the job was started.
    //
    FUNCTION_EXIT;
    return IJobInstance::Completed;
}


TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef AlarmGeneratorJob::toCorbaObject()
{
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef job = AbstractJob::toCorbaObject();

    return job;
}


unsigned long AlarmGeneratorJob::getAlarmSeverityKey()
{
    std::string alarmSeverityKeyParameter = getParameter(ALARM_SEVERITY_KEY_PARAMETER_NAME);
    
    return TA_Base_Core::DataConversion::toUnsignedInteger(alarmSeverityKeyParameter);
}

std::string AlarmGeneratorJob::getAlarmDescription()
{
    return getParameter(ALARM_DESCRIPTION_PARAMETER_NAME);
}


void AlarmGeneratorJob::updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    AbstractJob::updateJob(job);
}


AlarmGeneratorJob::~AlarmGeneratorJob()
{
}


AlarmGeneratorJob::AlarmGeneratorJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job) : AbstractJob(job)
{
    FUNCTION_ENTRY("AlarmGeneratorJob");

    FUNCTION_EXIT;
}


AlarmGeneratorJob::AlarmGeneratorJob(const std::string& jobID) : AbstractJob(jobID)
{
}


}; // TA_Base_App

