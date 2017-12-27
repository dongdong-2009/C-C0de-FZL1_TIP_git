/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingAlarmSender.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  * This class is used to submit alarms for the Scheduling Agent.
  *
  */

#include "core/alarm/src/AlarmHelperManager.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/SchedulingAlarms_MessageTypes.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"

namespace TA_Base_App
{

SchedulingAlarmSender* SchedulingAlarmSender::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable SchedulingAlarmSender::m_singletonLock;


SchedulingAlarmSender::SchedulingAlarmSender()
    :
    m_entityKey(0),
    m_entityTypeKey(0),
    m_entityName(""),
    m_locationKey(0),
    m_subsystemKey(0),
	m_assetName("")
	/*/TD18093, m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),
    m_nonUniqueAlarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper())*/
{
    FUNCTION_ENTRY("SchedulingAlarmSender");
}


SchedulingAlarmSender::~SchedulingAlarmSender()
{
    FUNCTION_ENTRY("~SchedulingAlarmSender");
	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

}


void SchedulingAlarmSender::initialise(
    unsigned long entityKey,
    unsigned long entityTypeKey,
    const std::string& entityName,
    unsigned long locationKey,
    unsigned long subsystemKey,
	std::string assetName)
{
    FUNCTION_ENTRY("initialise");

    TA_ASSERT(entityKey     != 0,  "Entity key must not be 0");
    TA_ASSERT(entityTypeKey != 0,  "Entity type key must not be 0");
    TA_ASSERT(entityName    != "", "Entity name must not be blank");

    m_entityKey     = entityKey;
    m_entityTypeKey = entityTypeKey;
    m_entityName    = entityName;
    m_locationKey   = locationKey;
    m_subsystemKey  = subsystemKey;
	m_assetName     = assetName;

    try
    {
        // close alarms that were previously opened
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Closing Agent initialisation Failure Alarm");
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::SchedulingAlarms::SchedulingAgentInitialisationFailureAlarm, m_entityKey, m_locationKey);

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Closing Agent Unexpected Termination Alarm");
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::SchedulingAlarms::SchedulingAgentUnexpectedTerminationAlarm, m_entityKey, m_locationKey);

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Closing Database Failure Alarm");
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::SchedulingAlarms::SchedulingDatabaseFailureAlarm, m_entityKey, m_locationKey);
    }
    catch (...) 
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught some exception when attempted to close the failure/unexpected termination/db failure alarm");
    }
}


SchedulingAlarmSender& SchedulingAlarmSender::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    //
    // If this method has been called before and initialise was not called,
    // assert.
    //
    TA_ASSERT(m_instance == NULL || m_instance->m_entityKey != 0,
              "SchedulingAlarmSender::initialise() must be called the "
              "first time getInstance() is invoked");

    if(m_instance == NULL)
    {
        m_instance = new SchedulingAlarmSender();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }

    return *m_instance;
}



void SchedulingAlarmSender::submitAlarm(
    const TA_Base_Core::MessageType& alarmType,
	const TA_Base_Core::DescriptionParameters& description,
	timeb sourceTime/* = 0*/)
{
    TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(
        alarmType, m_entityKey, m_entityTypeKey, description, m_entityName, m_locationKey, m_subsystemKey, m_assetName, sourceTime);
}


void SchedulingAlarmSender::submitAlarmWithSeverity(
    const TA_Base_Core::MessageType& alarmType,
	const TA_Base_Core::DescriptionParameters& description,
    unsigned long alarmSeverityKey,
	timeb sourceTime/* = 0*/)
{
    TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithSeverityAndAsset(
        alarmType, m_entityKey, m_entityTypeKey, description, m_entityName, m_locationKey, m_subsystemKey, alarmSeverityKey, m_assetName, sourceTime);
}


void SchedulingAlarmSender::submitNonUniqueAlarm(
    const TA_Base_Core::MessageType& alarmType,
	const TA_Base_Core::DescriptionParameters& description,
	timeb sourceTime/* = 0*/)
{
    TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
        alarmType, m_entityKey, m_entityTypeKey, description, m_entityName, m_locationKey, m_subsystemKey, m_assetName, sourceTime);
}


void SchedulingAlarmSender::jobExecutionFailed( const std::string& jobID, unsigned long minuteStarted, const std::string& jobDescription, const std::string& errorMessage)
{
    //
    // Get the minute started (instance ID as a string)
    //
    std::stringstream minuteStartedString;
    minuteStartedString << minuteStarted;

    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // Job [JobId] with description '[Description]' and instance ID [InstanceId]
    // has failed with error '[ErrorMessage]'
    TA_Base_Core::NameValuePair jobIDPair("JobId", jobID);
    TA_Base_Core::NameValuePair descriptionPair("Description", jobDescription);
    TA_Base_Core::NameValuePair instanceIDPair("InstanceId", minuteStartedString.str());
    TA_Base_Core::NameValuePair errorMessagePair("ErrorMessage", "");

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&jobIDPair);
    params.push_back(&descriptionPair);
    params.push_back(&instanceIDPair);
    params.push_back(&errorMessagePair);

    // Submit the non-unique alarm
    submitNonUniqueAlarm( TA_Base_Core::SchedulingAlarms::JobExecutionFailureAlarm, params );
}



void SchedulingAlarmSender::initialisationFailed(const std::string& reason)
{
    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // Scheduling Agent has failed to initialise for the following reason:
    // [Reason]
    TA_Base_Core::NameValuePair reasonPair("Reason", reason);

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&reasonPair);

    // Submit the unique alarm
    //submitAlarm( TA_Base_Core::SchedulingAlarms::SchedulingAgentInitialisationFailureAlarm, params );
}


void SchedulingAlarmSender::unexpectedTermination(const std::string& reason)
{
    //
    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // Scheduling Agent has terminated unexpectedly for the following reason:
    // [Reason]
    //
    TA_Base_Core::NameValuePair reasonPair("Reason", reason);

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&reasonPair);

    // Submit the unique alarm
    //submitAlarm( TA_Base_Core::SchedulingAlarms::SchedulingAgentUnexpectedTerminationAlarm, params );
}


void SchedulingAlarmSender::invalidDataConfiguration(const std::string& reason)
{
    //
    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // The database configuration for the Scheduling Agent is invalid in
    // the following way: [Reason]
    //
    TA_Base_Core::NameValuePair reasonPair("Reason", reason);

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&reasonPair);

    // Submit the non-unique alarm
    //submitNonUniqueAlarm( TA_Base_Core::SchedulingAlarms::SchedulingInvalidDataConfigurationAlarm, params );
}


void SchedulingAlarmSender::schedulingMonitorError(const std::string& action, const std::string& reason)
{
    //
    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // The scheduling monitor was unable to [Action] for the following reason:
    // [Reason]
    //
    TA_Base_Core::NameValuePair actionPair("Action", action);
    TA_Base_Core::NameValuePair reasonPair("Reason", reason);

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&actionPair);
    params.push_back(&reasonPair);

    // Submit the non-unique alarm
    //submitNonUniqueAlarm(TA_Base_Core::SchedulingAlarms::SchedulingMonitorAlarm, params);
}


void SchedulingAlarmSender::schedulingControllerError(
    const std::string& action, const std::string& reason)
{
    //
    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // The scheduling controller was unable to [Action] for the following
    // reason: [Reason]
    //
    TA_Base_Core::NameValuePair actionPair("Action", action);
    TA_Base_Core::NameValuePair reasonPair("Reason", reason);

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&actionPair);
    params.push_back(&reasonPair);

    // Submit the non-unique alarm
    submitNonUniqueAlarm(TA_Base_Core::SchedulingAlarms::SchedulingControllerAlarm, params);
}


void SchedulingAlarmSender::databaseFailure(const std::string& reason)
{
    // Create the name/value pairs that will be constructed into
    // the alarm type message:
    // The scheduling agent is unable to communicate with the database for
    // the following reason: [Reason]
    TA_Base_Core::NameValuePair reasonPair("Reason", reason);

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&reasonPair);

    // Submit the unique alarm
    submitAlarm(TA_Base_Core::SchedulingAlarms::SchedulingDatabaseFailureAlarm, params);
}


}; // TA_Base_App

