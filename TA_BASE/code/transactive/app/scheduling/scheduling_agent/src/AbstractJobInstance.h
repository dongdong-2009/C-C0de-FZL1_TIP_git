/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AbstractJobInstance.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This abstract class represents an instance of an executable job.
  * It implements the getJob and getMinuteStarted methods of the
  * IJobInstance interface.
  *
  */



#ifndef ABSTRACT_JOB_INSTANCE_H
#define ABSTRACT_JOB_INSTANCE_H


#include "app/scheduling/scheduling_agent/src/IJobInstance.h"



namespace TA_Base_App
{


// Forward declaration
class IJob;


class AbstractJobInstance : public IJobInstance
{

public:

    /**
    *  Gets the id of this job.
    */
    virtual std::string getJobId() const;

    /**
    *  Gets the description of this job.
    */
    virtual std::string getDescription() const;

    /**
    *  Gets the location id of this job.
    */
    virtual unsigned long getLocationId() const;

    /**
    *  Gets the profile id of this job.
    */
    virtual unsigned long getProfileId() const;

    /**
     *  Gets the job instance state of this job.
     */
    virtual int getJobInstanceState(unsigned long minuteStarted, std::string& errorMessage) const;

    /**
    *  Returns the minute at which the job instance started.
    */
    virtual unsigned long getMinuteStarted() const;

    /**
    *  Destructor.
    */
    virtual ~AbstractJobInstance();


    /**
     *  Executes the job.
     *
     *  This method must execute the job, and return after the job has
     *  finished. It is not implemented by this class.
     *
     *  @exception JobInstanceExecutionException
     *             thrown if there is a problem executing the job
     */
    virtual void execute();

    virtual void executeJob() = 0;

	
protected:

	/**
	 *  Implementers of the IJobInstance interface should call the
     *  AbstractJobInstance constructor from within the body of their
     *  constructor.
     *
	 *  @param job            the parent job.
	 *  @param minuteStarted  the minute at which the instance was started.
	 */
	AbstractJobInstance( const IJob& refJob, const unsigned long minuteStarted );

private:
    AbstractJobInstance();
    AbstractJobInstance(const AbstractJobInstance&);
    const AbstractJobInstance& operator=(const AbstractJobInstance&);

    const IJob& m_refJob;
    unsigned long m_minuteStarted;
};


}; // TA_Base_App

#endif // ABSTRACT_JOB_INSTANCE_H
