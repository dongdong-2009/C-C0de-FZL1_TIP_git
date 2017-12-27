/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class creates objects that are used to read and change rows in the
  * ScheduledJob database table.
  *
  */


#ifndef JOB_DATA_ACCESS_FACTORY_H
#define JOB_DATA_ACCESS_FACTORY_H


#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/data_access_interface/scheduling_agent/src/IConfigJobData.h"


#include <vector>


namespace TA_Base_Core
{

class JobDataAccessFactory
{

public:


    typedef std::vector< IConfigJobData* > IConfigJobDataVector;


    /**
     *  Gets the single instance of this class.
     */
    static JobDataAccessFactory& getInstance();


    /**
     *  Gets a row from the scheduledJob table.
     *
     *  @param id  the unique ID of the row.
     *
     *  @exception JobDoesNotExistException
     *  Thrown if the ID does no exist in the database
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  Thrown if the job's schedule doesn't exist in the database or there
     *  is a problem with the format of the data.
     *
     *  @exception BadScheduleException
     *  Thrown if the format of the schedule is invalid
     *  (eg it represents the 31st of February)
     *
     */
    IConfigJobData* getJob(const std::string& id);


    /**
     *  Gets all rows from the Job table.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if the database contains invalid data.
     *
     *  @exception BadScheduleException
     *  Thrown if the format of the schedule is invalid
     *  (eg it represents the 31st of February)
     *
     *  @return a vector of IConfigJobData pointers that must be deleted by
     *          the caller.
     */
    IConfigJobDataVector getAllJobs();


    /** 
      * Gets all jobs of the specified type.
      *
      * @param jobType  the type of job. This must not be blank
      *
      * @return a vector of IConfigJobData pointers that must be deleted by
      *         the caller.
      *
      *  @exception BadScheduleException
      *  Thrown if the format of a schedule is invalid
      *  (eg it represents the 31st of February)
      *
      */
    JobDataAccessFactory::IConfigJobDataVector
        getAllJobsOfType(const std::string& jobType);

    
    /**
     *  Creates a new empty job. 
     *
     *  The caller must call (TODO create method) to save job.
     *  The caller must delete the object returned.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem 
     *  accessing the database.
     *
     */
    IConfigJobData* createJob();



    /**
     *  Destructor.
     */
    virtual ~JobDataAccessFactory();


private:

    JobDataAccessFactory();
    JobDataAccessFactory(const JobDataAccessFactory&);
    const JobDataAccessFactory& operator=(const JobDataAccessFactory&);


    /** The single instance of this class */
    static JobDataAccessFactory* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
    
};


};// TA_Base_Core


#endif // JOB_DATA_ACCESS_FACTORY_H
