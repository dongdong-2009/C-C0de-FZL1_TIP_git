/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/IJob.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This interface represents an executable job.
  *
  */



#ifndef IJOB_H
#define IJOB_H

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/Schedule.h"
#include "app/scheduling/scheduling_agent/src/IJobInstance.h"
#include "app/scheduling/scheduling_agent/src/JobRunner.h"

#include <string>
#include <vector>

namespace TA_Base_App
{



class IJob
{


public:

    typedef std::vector< IJob* > IJobPtrVector;

    /**
     *  Destructor.
     */
    virtual ~IJob() {};

    
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
     *
     *  @exception JobInstanceCreationException
     *             if for any reason a job instance cannot be created.
     */
    virtual IJobInstance* createJobInstance(unsigned long minuteStarted) = 0;


    /**
     *  Returns a JobCorbaDef containing the information in this object.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    virtual TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef toCorbaObject() = 0;
    

    /**
     *  Gets the state of an instance of this IJob.
     *  Note that this method must get the state from an external source
     *  at the time this method is called, rather than periodically updating
     *  an internally-stored state.
     *  Otherwise failover will not work properly.
     *
     *  @param minuteStarted  [in]  the minute at which the instance started.
     *  @param errorMessage   [out] If there was an error that prevented the job
     *                              from completing, the error message is
     *                              stored into this string.
     *
     *  @exception JobInstanceNotFoundException
     *  Thrown if a job instance that started at the specified minute could
     *  not be found.
     *
     *  @exception JobInstanceStateRetrievalException
     *  Thrown if the state of the job instance cannot be retrieved.
     */
    virtual IJobInstance::JobInstanceState getJobInstanceState(unsigned long minuteStarted, std::string& errorMessage) const = 0;


    /**
     *  This method reloads the data from the database if it is invalid.
     *
     *  @param forceReload  if true, the data is reloaded from the database
     *                      even if it hasn't been invalidated.
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
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    virtual void reloadDataFromDB(bool forceReload = false) = 0;


    /**
     *  Marks the job as having been changed in the database. This will cause
     *  the next call to a getXXX method to reload the state from the database.
     */
    virtual void invalidate() = 0;



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
     *  Updates this job's state in the database.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if there is a problem with the format of the data.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    virtual void updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job) = 0;


    /**
     *  Gets the inhibition status of this job.
     *
     *  @return  true if the job is being inhibited,
     *           false otherwise.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if there is a problem with the format of the data.
     */
	virtual bool isInhibited() const = 0;

    /**
     *  Gets the Schedule associated with this job.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if there is a problem with the format of the data.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    virtual Schedule& getSchedule() = 0;

    
    /**
     *  Gets the description of this job.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if there is a problem with the format of the data.
     */
    virtual std::string getDescription() const = 0;


    /**
     *  Returns an ID unique to all IJob objects.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if there is a problem with the format of the data.
     */
    virtual std::string getID() const = 0;



    /**
     *  Gets the type of job that this is, eg PlanRunnerJob
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if there is a problem with the format of the data.
     */
    virtual std::string getJobType() const = 0;


    /**
     *  Gets the ID of the location associated with the profile
     *  used by this job, or 0 is there isn't one.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if there is a problem with the format of the data.
     */
	virtual unsigned long getLocationID() const = 0;


    /**
     *  Gets the ID of the profile used by this job.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be converted to
     *  the required format (e.g. LOCATION_ID should be an unsigned long), or
     *  if there is a problem with the format of the data.
     */
	virtual unsigned long getProfileID() const = 0;
};



}; // TA_Base_App

#endif // IJOB_H
