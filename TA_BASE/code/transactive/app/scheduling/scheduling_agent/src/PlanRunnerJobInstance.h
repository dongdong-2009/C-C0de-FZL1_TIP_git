/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/PlanRunnerJobInstance.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * Implements the IJobInstance interface for PlanRunnerJobs.
  *
  */



#ifndef PLAN_RUNNER_JOB_INSTANCE_H
#define PLAN_RUNNER_JOB_INSTANCE_H

#include "app/scheduling/scheduling_agent/src/PlanRunnerJob.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include "app/scheduling/scheduling_agent/src/AbstractJobInstance.h"


using TA_Base_Core::CommsMessageCorbaDef;


namespace TA_Base_App
{

class PlanRunnerJobInstance : public AbstractJobInstance, TA_Base_Core::SpecialisedMessageSubscriber <CommsMessageCorbaDef>
{

public:


    /**
     *  Executes the job.
     *
     *  This method must execute the job, and return after the job has
     *  finished.
     *
     *  @exception JobInstanceExecutionException
     *             thrown if there is a problem executing the job
     */
    virtual void executeJob();

    /**
     *  Destructor.
     */
	virtual ~PlanRunnerJobInstance();


	
protected:

    //
    // PlanRunnerJob::createJobInstance and PlanRunnerJob::getJobInstanceState
    // need to be able to call this class' constructor.
    //
    friend IJobInstance* PlanRunnerJob::createJobInstance(unsigned long);
    friend IJobInstance::JobInstanceState PlanRunnerJob::getJobInstanceState(
        unsigned long minuteStarted, std::string& errorMessage) const;


    /**
     *  Constructs a new job with the given parent job to be started at the
     *  given minute.
     *
	 *  @param job            the parent job.
	 *  @param minuteStarted  the minute at which the instance will be started.
	 */
	PlanRunnerJobInstance( const PlanRunnerJob* job, const unsigned long minuteStarted );

    /**
     *  Saves a <minute started / instance ID> pair to the database.
     *  The instance ID is the number stored by the plan agent for this job.
     *
     *  @exception DatabaseException
     *  If there is a problem communicating with the database.
     *
     *  @exception DataException
     *  If there is a problem with the format of the fields.
     */
    void PlanRunnerJobInstance::saveMinuteStartedInstancePairToDB(
        unsigned long minuteStarted,
        CORBA::ULong instance);

    /**
     *  Removes a <minute started / instance ID> pair from the database.
     *  The instance ID is the number stored by the plan agent for this job.
     *
     *  As this is stroed as a key value pair, only the key (minute started)
     *  is required.
     *
     *  @exception DatabaseException
     *  If there is a problem communicating with the database.
     *
     *  @exception DataException
     *  If there is a problem with the format of the fields.
     */
    void PlanRunnerJobInstance::removeMinuteStartedInstancePairFromDB(
        unsigned long minuteStarted );

    /**
     *  Gets the instance ID corresponding to this job instance from the
     *  database.
     *  The instance ID is the number stored by the plan agent for this job.
     *
     *  @exception DatabaseException
     *  If there is a problem communicating with the database.
     *
     *  @exception DataException
     *  If the data is badly formatted, or the instance ID can't be cast to
     *  a CORBA::ULong.
     */
    CORBA::ULong PlanRunnerJobInstance::getInstanceIDFromDB() const;


private:

    PlanRunnerJobInstance();
    PlanRunnerJobInstance(const PlanRunnerJobInstance&);
    const PlanRunnerJobInstance& operator=(const PlanRunnerJobInstance&);


    /**
     *  Throws a JobInstanceExecutionException with some information
     *  about this object (the job id, minute started and job description)
     *  that starts with exceptionName and includes message at the end.
     */
    void throwJobInstanceExecutionException(
        const std::string& exceptionName,
        const std::string& message);


    /**
     *  Checks if the plan this job instance runs is complete by querying
     *  the Plan Agent through CORBA.
     *
     *  @return true if it's complete, false otherwise.
     */
    bool isPlanCompleteCorbaCall();


    /**
     *  Called when a Comms message is received from CORBA.
     */
    void receiveSpecialisedMessage(
        const CommsMessageCorbaDef& message);


    /** The parent job of this instance */
    PlanRunnerJob* m_job;

    /** The plan instance stored by the Plan Agent */
    CORBA::ULong m_planInstance;

    /** Indicates whether the plan is complete */
    bool m_planComplete;
};


}; // TA_Base_App

#endif // PLAN_RUNNER_JOB_INSTANCE_H
