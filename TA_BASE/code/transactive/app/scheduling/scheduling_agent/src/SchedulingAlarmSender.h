/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h $
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


#ifndef SCHEDULING_ALARM_SENDER_H
#define SCHEDULING_ALARM_SENDER_H

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


#include <string>

namespace TA_Base_App
{



class SchedulingAlarmSender
{

public:


	/**
	 *  Gets the single instance of this class.
     *  Note that the first time this method is called, initialise must be
     *  called.
	 */
	static SchedulingAlarmSender& getInstance();


    /**
     *  Initialises the object.
     *
     *  This method is used instead of a constructor.
     *  It must be called the first time getInstance is invoked, or
     *  subsequent invocations will cause an assertion.
     */
    void SchedulingAlarmSender::initialise(
        unsigned long entityKey,
        unsigned long entityTypeKey,
        const std::string& entityName,
        unsigned long locationKey,
        unsigned long subsystemKey,
		std::string assetName);


    /**
     *  submitAlarm
     *
     *  Use this to submit an alarm that is unique in the system
     *
     *  @param alarmType     The type of alarm as defined in the specific
     *                       Message Type header file.
     *  @param description   The parameters to be inserted into the
     *                       description as defined in the AlarmType table.
     *  @param sourceTime	 The time the alarm was first detected.
     *                       0 (default) means filled in by AlarmHelper using
     *                       time( NULL )
     */
    void submitAlarm(
        const TA_Base_Core::MessageType& alarmType,
        const TA_Base_Core::DescriptionParameters& description,
        timeb sourceTime = TA_Base_Core::AlarmConstants::defaultTime);


    /**
     *  submitAlarmWithSeverity
     *
     *  Use this to submit an alarm that is unique in the system, with a
     *  particular severity.
     *
     *  @param alarmType         The type of alarm as defined in the specific
     *                           Message Type header file.
     *  @param description       The parameters to be inserted into the
     *                           description as defined in the AlarmType table.
     *  @param alarmSeverityKey  The severity to associate with this Alarm
     *  @param sourceTime	     The time the alarm was first detected.
     *                           0 (default) means filled in by AlarmHelper
     *                           using time( NULL )
     */
    void submitAlarmWithSeverity(
        const TA_Base_Core::MessageType& alarmType,
        const TA_Base_Core::DescriptionParameters& description,
        unsigned long alarmSeverityKey,
        timeb sourceTime = TA_Base_Core::AlarmConstants::defaultTime);


    /**
     *  submitNonUniqueAlarm
     *
     *  Use this to submit an alarm that is not unique in the system
     *
     *  @param alarmType     The type of alarm as defined in the specific
     *                       Message Type header file.
     *  @param description   The parameters to be inserted into the
     *                       description as defined in the AlarmType table.
     *  @param sourceTime	 The time the alarm was first detected.
     *                       0 (default) means filled in by AlarmHelper using
     *                       time( NULL )
     */
    void submitNonUniqueAlarm(
        const TA_Base_Core::MessageType& alarmType,
        const TA_Base_Core::DescriptionParameters& description,
        timeb sourceTime = TA_Base_Core::AlarmConstants::defaultTime);


    /**
     *  Raises a JobExecutionFailureAlarm with the given parameters in its
     *  description:
     *  Job [JobId] with description '[Description]' and instance ID [InstanceId]
     *  has failed with error '[ErrorMessage]'
     *
     *  @param jobID           the ID of the job.
     *  @param minuteStarted   the minute at which the job was started,
     *                         relative to the epoch.
     *  @param jobDescription  the description of the job.
     *  @param errorMessage    the error message.
     */
    void jobExecutionFailed(
        const std::string& jobID,
        unsigned long minuteStarted,
        const std::string& jobDescription,
        const std::string& errorMessage);


    /**
     *  Raises a SchedulingAgentInitialisationFailureAlarm with the
     *  given reason for failure.
     *
     *  Scheduling Agent has failed to initialise for the following reason:
     *  [Reason]
     *
     *  @param reason  the error message.
     */
    void SchedulingAlarmSender::initialisationFailed(
        const std::string& reason);


    /**
     *  Raises a SchedulingAgentUnexpectedTerminationAlarm with the
     *  given reason for failure.
     *
     *  Scheduling Agent has terminated unexpectedly for the following reason:
     *  [Reason]
     *
     *  @param reason  the error message.
     */
    void SchedulingAlarmSender::unexpectedTermination(
        const std::string& reason);


    /**
     *  Raises a SchedulingInvalidDataConfigurationAlarm with the
     *  given reason for failure.
     *
     *  The database configuration for the Scheduling Agent is invalid in the
     *  following way: [Reason]
     *
     *  @param reason  the error message.
     */
    void SchedulingAlarmSender::invalidDataConfiguration(
        const std::string& reason);


    /**
     *  Raises a SchedulingMonitorAlarm with the given parameters in its
     *  description.
     *
     *  The scheduling monitor was unable to [Action] for the following
     *  reason: [Reason]
     *
     *  @param action  the action the monitor was trying to perform
     *                 (eg "schedule a job").
     *  @param reason  the error message.
     */
    void schedulingMonitorError(
        const std::string& action,
        const std::string& reason);


    /**
     *  Raises a SchedulingControllerAlarm with the given parameters in its
     *  description.
     *
     *  The scheduling controller was unable to [Action] for the following
     *  reason: [Reason]
     *
     *  @param action  the action the agent was trying to perform
     *                 (eg "schedule a job").
     *  @param reason  the error message.
     */
    void schedulingControllerError(
        const std::string& action,
        const std::string& reason);


    
    /**
     *  Raises a SchedulingDatabaseFailureAlarm with the given reason for
     *  failure.
     *
     *  The scheduling agent is unable to communicate with the database for
     *  the following reason: [Reason]
     *
     *  @param reason  the error message.
     */
    void databaseFailure(const std::string& reason);


    /**
     *  Destructor.
     */
    ~SchedulingAlarmSender();



private:

	SchedulingAlarmSender();
    SchedulingAlarmSender(const SchedulingAlarmSender&);
    const SchedulingAlarmSender& operator=(const SchedulingAlarmSender&);


	//TD18093,jianghp
    /*/ Alarm Helper
    TA_Base_Core::AlarmHelper& m_alarmHelper;

    // Non-unique Alarm Helper
    TA_Base_Core::NonUniqueAlarmHelper& m_nonUniqueAlarmHelper;
	//TD18093,jianghp*/

    // The Scheduling Agent's entity key
    unsigned long m_entityKey;

    // The Scheduling Agent's entity type key
    unsigned long m_entityTypeKey;

    // The Scheduling Agent's entity name
    std::string m_entityName;
    
    // The Scheduling Agent's location key
    unsigned long m_locationKey;

    // The Scheduling Agent's subsystem key
    unsigned long m_subsystemKey;

	// The agent's asset name
	std::string m_assetName;



    // The single instance of this class
    static SchedulingAlarmSender* m_instance;

    // The lock used to stop multiple access to the instance of this class
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
};



}; // TA_Base_App


#endif // SCHEDULING_ALARM_SENDER_H
