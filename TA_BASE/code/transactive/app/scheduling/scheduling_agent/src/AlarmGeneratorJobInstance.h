/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AlarmGeneratorJobInstance.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * Implements the IJobInstance interface for AlarmGeneratorJobs.
  *
  */



#ifndef ALARM_GENERATOR_JOB_INSTANCE_H
#define ALARM_GENERATOR_JOB_INSTANCE_H

#include "app/scheduling/scheduling_agent/src/AlarmGeneratorJob.h"

#include "app/scheduling/scheduling_agent/src/AbstractJobInstance.h"


namespace TA_Base_App
{

class AlarmGeneratorJobInstance : public AbstractJobInstance
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
	virtual ~AlarmGeneratorJobInstance();


	
protected:

    //
    // AlarmGeneratorJob::createJobInstance needs to be able to call this
    // class' constructor.
    //
    friend IJobInstance* AlarmGeneratorJob::createJobInstance(unsigned long);


    /**
     *  Constructs a new job with the given parent job to be started at the
     *  given minute.
     *
	 *  @param job            the parent job.
	 *  @param minuteStarted  the minute at which the instance will be started.
	 */
	AlarmGeneratorJobInstance(
        AlarmGeneratorJob* job,
        unsigned long minuteStarted);


private:

    AlarmGeneratorJobInstance();
    AlarmGeneratorJobInstance(const AlarmGeneratorJobInstance&);
    const AlarmGeneratorJobInstance& operator=(const AlarmGeneratorJobInstance&);


    /** The parent job of this instance */
    AlarmGeneratorJob* m_job;
};


}; // TA_Base_App

#endif // ALARM_GENERATOR_JOB_INSTANCE_H
