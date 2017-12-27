/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/IConfigJobData.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface extends the IJobData interface to allow a row of
  * data in the ScheduledJob table to be written as well as read.
  *
  */


#ifndef I_CONFIG_JOB_DATA_H
#define I_CONFIG_JOB_DATA_H


#include "core/data_access_interface/scheduling_agent/src/IJobData.h"


namespace TA_Base_Core
{
    // Forward declaration
    class IConfigScheduleData;
    

class IConfigJobData: public IJobData
{

public:



    /**
     *  Saves this job to the database.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  storing data.
     */
     virtual void saveJob() = 0;


    /**
     *  Deletes this job from the database.
     *
     *  After calling this method, the job should be deleted from the database
     *  but the object's get methods should still be available until the object
     *  itself is deleted.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     */
     virtual void deleteJob() = 0;


    /**
     *  This will write all changes made to the database.
     *
     *  @exception DatabaseException
     *  Thrown if there is a database error.
     *
     *  @exception DataException
     *  Thrown if the a parameter name cannot be found, or if there is more
     *  than one value for a parameter.
     *
     */
    virtual void applyChanges() = 0;


    /**
     *  Sets the ID of the Job.
     *
     *  @param id The id of this job
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if the wrong amount of data is retrieved.
     */
    virtual void setId(const std::string& id) = 0;


    /**
     *  Sets the type of job that this is, eg PlanRunnerJob
     *
     *  @param jobType The job type eg Plan, Alarm, etc
     *
     */
    virtual void setJobType(const std::string& jobType) = 0;

    /**
     *  Sets the description of the job.
     */
    virtual void setDescription(const std::string& description) = 0;


    /**
     *  Sets the inhibition status of the job.
     */
    virtual void setInhibitionStatus(bool isInhibited) = 0;


    /**
     *  Sets the ID of the location of the job.
     */
    virtual void setLocationID(unsigned long locationID) = 0;


    /**
     *  Sets the ID of the profile of the job.
     */
    virtual void setProfileID(unsigned long profileID) = 0;


    /**
     *  Sets the given parameter. If the parameter does not exist,
     *  it's created.
     */
    virtual void setParameter(
        const std::string& parameterName,
        const std::string& parameterValue) = 0;


    /**
     *  Deletes the given parameter. If the parameter does not exist,
     *  it silently returns.
     */
    virtual void deleteParameter(const std::string& parameterName) = 0;


    /**
     *  Gets a pointer to the schedule associated with this job.
     *  This object is responsible for managing the pointer's memory.
     */
    virtual IConfigScheduleData* getSchedule() = 0;


    /**
     *  The destructor.
     */
    virtual ~IConfigJobData() {};

};


};// TA_Base_Core


#endif // I_CONFIG_JOB_DATA_H
