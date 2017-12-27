/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AbstractJob.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This abstract class provides default implementations of many of the
  * methods in the IJob interface. It may be extended by classes implementing
  * the IJob interface.
  *
  */



#ifndef ABSTRACT_JOB_H
#define ABSTRACT_JOB_H

#include "app/scheduling/scheduling_agent/src/IJob.h"
#include "core/data_access_interface/scheduling_agent/src/IConfigJobData.h"

#include <string>
#include <vector>

namespace TA_Base_App
{



class AbstractJob : public IJob
{


public:

    
    /**
     *  Returns a JobCorbaDef containing the information in this object.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    virtual TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef toCorbaObject();
    


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
    virtual void reloadDataFromDB(bool forceReload = false);


    /**
     *  Marks the job as having been changed in the database. This will cause
     *  the next call to a getXXX method to reload the state from the database.
     */
    virtual void invalidate();


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
    virtual void deleteJob();


    /**
     *  Saves this job to the database.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  storing data.
     */
    virtual void saveJob();


    /**
     *  Updates this job's state in the database.
     *
     *  @exception  UnrecognisedJobTypeException if the job.data field cannot
     *              be converted to a specific job type (eg PlanRunnerCorbaDef).
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
    virtual void updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);

    /**
     *  Updates this job's data members.
     *
     *  @exception  UnrecognisedJobTypeException if the job.data field cannot
     *              be converted to a specific job type (eg PlanRunnerCorbaDef).
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    virtual void updateJobDetails(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);


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
	virtual bool isInhibited() const;


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
    virtual Schedule& getSchedule();

    
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
    virtual std::string getDescription() const;


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
    virtual std::string getID() const;



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
    virtual std::string getJobType() const;


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
	virtual unsigned long getLocationID() const;


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
	virtual unsigned long getProfileID() const;


    /**
     *  Sets the given parameter. If the parameter does not exist,
     *  it's created.
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
    void setParameter( const std::string& parameterName, const std::string& parameterValue);


    /**
     *  Deletes the given parameter. If the parameter does not exist,
     *  it silently returns.
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
    void deleteParameter(const std::string& parameterName);


    /**
     *  Gets the given parameter.
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
     *  It will also be thrown if the parameter does not exist in this row's
     *  list of parameters.
     */
    std::string getParameter(const std::string& parameterName) const;

    
    /**
     *  Gets all parameters held by this class.
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
    TA_Base_Core::IJobData::ParameterMap getParameters();


    /**
     *  Destructor.
     */
	virtual ~AbstractJob();
	

	/**
     *  Implementers of the IJob interface should call the AbstractJob
     *  constructor from the sub-class.
     *
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem creating
     *  this Job in the database.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
	 */
    AbstractJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);

    
    /**
     *  Creates the object from the database.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if the ID doesn't exist in the
     *  database.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    AbstractJob(const std::string& jobID);

private:

    AbstractJob();
    AbstractJob(const AbstractJob&);
    const AbstractJob& operator=(const AbstractJob&);

    /** The IJobConfigData object this class uses to store its fields */
    TA_Base_Core::IConfigJobData* m_jobData;

    /** The schedule associated with this job */
    Schedule* m_schedule;
};



}; // TA_Base_App

#endif // ABSTRACT_JOB_H
