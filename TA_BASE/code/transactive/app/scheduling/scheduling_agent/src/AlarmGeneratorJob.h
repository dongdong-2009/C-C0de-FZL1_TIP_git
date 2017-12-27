/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h $
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



#ifndef ALARM_GENERATOR_JOB_H
#define ALARM_GENERATOR_JOB_H

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/naming/src/NamedObject.h"


#include "app/scheduling/scheduling_agent/src/AbstractJob.h"


namespace TA_Base_App
{


class AlarmGeneratorJob : public AbstractJob
{


public:

    /**
     *  Creates a new IJobInstance.
     *
     *  Implementers of the IJob interface should create an IJobInstance
     *  object using new, and return a pointer to it.
     *
     *  @param minuteStarted  the minute at which the instance is started,
     *                        relative to the epoch.
     *                        This may be used as a unique identifier for the
     *                        job instance.
     */
    virtual IJobInstance* createJobInstance(
        unsigned long minuteStarted);


    /**
     *  Gets the state of an instance of this IJob.
     *  Note that this method must get the state from an external source
     *  at the time this method is called, rather than periodically updating
     *  an internally-stored state.
     *  Otherwise failover will not work properly.
     *
     *  @param minuteStarted  [in]  the minute at which the instance started.
     *  @param errorMessage   [out] the error message, if there was an error.
     *
     *  @exception JobInstanceNotFoundException
     *  Thrown if a job instance that started at the specified minute could
     *  not be found.
     *
     *  @exception JobInstanceStateRetrievalException
     *  Thrown if the job instance cannot be retrieved.
     *
     */
    virtual IJobInstance::JobInstanceState getJobInstanceState(
        unsigned long minuteStarted,
        std::string& errorMessage) const;


    /**
     *  Returns a JobCorbaDef containing the information in this object.
     */
    virtual TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef toCorbaObject();


    /** 
      * Updates the job in the database.
      *
      * @param job  the JobCorbaDef with the details of the job to be updated.
      *             Note that it must be possible to convert the job.data
      *             field to an alarm generator job.
      *
      * @exception  UnrecognisedJobTypeException if the job.data cannot be
      *             converted to an alarm generator job.
      */
    void updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);



    /**
     *  Destructor.
     */
	virtual ~AlarmGeneratorJob();
	

	/**
     *  Constructs an alarm generator job from the given parameters.
     *
     *  @param job               a JobCorbaDef object containing the fields of
     *                           this job that are common to all jobs.
     *  @param alarmSeverityKey  the key of the alarm severity with which the
     *                           alarm is raised.
     *  @param alarmDescription  the description that will be shown in the
     *                           alarm viewer.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem writing
     *  values to the database.
	 */
    AlarmGeneratorJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);


    /** 
      *  Constructs an alarm generator job from the database.
      *
      *  @param jobID  the ID of the job in the database.
      *  @exception DatabaseException
      *
      *  A DatabaseException will be thrown if there is a problem retrieving
      *  values from the database.
      *
      *  @exception DataException
      *  A DataException will be thrown if the ID doesn't exist in the
      *  database or if any database fields are badly formatted.
      */
    AlarmGeneratorJob(const std::string& jobID);


    /**
     *  Gets the alarm severity key of the alarm that is raised.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if the ID doesn't exist in the
     *  database or if any database fields are badly formatted.
     */
    unsigned long getAlarmSeverityKey();


    /**
     *  Gets the description of the alarm that is raised.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if the ID doesn't exist in the
     *  database or if any database fields are badly formatted.
     */
    std::string getAlarmDescription();


    /**
      * Used in the database to indicate what type of job this is
      * (ie Alarm Generator Job).
      */
    static const std::string JOB_TYPE;


private:

    AlarmGeneratorJob();
    AlarmGeneratorJob(const AlarmGeneratorJob&);
    const AlarmGeneratorJob& operator=(const AlarmGeneratorJob&);
    
    // The names of the parameters used to store the alarm severity key
    // and alarm description in the SH_JOB table's parameters field.
    static const std::string ALARM_SEVERITY_KEY_PARAMETER_NAME;
    static const std::string ALARM_DESCRIPTION_PARAMETER_NAME;
};



}; // TA_Base_App

#endif // ALARM_GENERATOR_JOB_H
