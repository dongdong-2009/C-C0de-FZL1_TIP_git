/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobFactory.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class is used to create instances of IJob.
  *
  */


#ifndef JOB_FACTORY_H
#define JOB_FACTORY_H


#include <vector>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/scheduling/scheduling_agent/src/IJob.h"
#include "app/scheduling/scheduling_agent/src/IJobFactory.h"


namespace TA_Base_App
{

class JobFactory
{

public:


	/**
	 *  Gets the single instance of this class.
	 */
	static JobFactory& getInstance();


    /**
     *  Initialises the job factories for each job type
     *  eg PlanRunnerJobFactory.
     *  This is called when the Scheduling Agent is first created.
     */
    virtual void initialise( TA_Base_Core::ISchedulingAgentEntityData& entityData );



    /**
     *  Creates a new IJob from the JobCorbaDef and returns a pointer to it.
     *  The calling class is responsible for deleting the pointer.
     *
     *  @exception UnrecognisedJobTypeException
     *  Thrown if the JobCorbaDef cannot be converted to a
     *  subclass of IJob.
     */
    virtual IJob* createJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);


    /**
     *  Retrieves all jobs from persistent storage and creates new IJob
     *  objects from them.
     *  A vector of pointers to these objects is returned.
     *  The calling class is responsible for deleting the pointers.
     *
     *  @exception BadScheduleException
     *  Thrown if the format of the schedule is invalid
     *  (eg it represents the 31st of February)
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     */
    virtual IJob::IJobPtrVector createJobsFromStore();


    /**
     *  Retrieves the job with the given ID from persistent storage and
     *  creates a new IJob object for it.
     *  The calling class is responsible for deleting the pointer returned.
     *
     *  @return a pointer to the IJob object
     *
     *  @exception BadScheduleException
     *  Thrown if the format of the schedule is invalid
     *  (eg it represents the 31st of February)
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception JobDoesNotExistException
     *  Thrown if a job with the given ID does not exist.
     *
     */
    virtual IJob* getJobFromStore(const std::string& id);


    /**
     *  Destructor.
     */
    virtual ~JobFactory();

private:
	JobFactory();
    JobFactory(const JobFactory&);
    const JobFactory& operator=(const JobFactory&);

    typedef std::vector<IJobFactory*> IJobFactoryPtrVector;

    /** The list of IJobFactory subclasses this class uses to create jobs */
    IJobFactoryPtrVector m_jobFactories;

    /** The single instance of this class */
    static JobFactory* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
};


};// TA_Base_App


#endif // JOB_FACTORY_H
