/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AlarmGeneratorJobInstance.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * Implements the IJobInstance interface for AlarmGeneratorJobs.
  *
  */
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/SchedulingAlarms_MessageTypes.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"
#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJobInstance.h"


namespace TA_Base_App
{


void AlarmGeneratorJobInstance::executeJob()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Executing alarm generator job instance at minute %d since the epoch that is a child of job with ID %s.",
        getMinuteStarted(), getJobId().c_str());

    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // [Description]
    TA_Base_Core::NameValuePair descriptionPair("Description", m_job->getAlarmDescription());

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&descriptionPair);


    // Submit the alarm
    SchedulingAlarmSender::getInstance().submitAlarmWithSeverity(TA_Base_Core::SchedulingAlarms::ScheduledAlarm,
        params, m_job->getAlarmSeverityKey());
}


AlarmGeneratorJobInstance::~AlarmGeneratorJobInstance()
{
}


AlarmGeneratorJobInstance::AlarmGeneratorJobInstance( AlarmGeneratorJob* job, const unsigned long minuteStarted) :
AbstractJobInstance(*job, minuteStarted),
m_job(job)
{
}


}; // TA_Base_App

