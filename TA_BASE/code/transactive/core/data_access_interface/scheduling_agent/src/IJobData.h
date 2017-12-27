/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/IJobData.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface represents a row of data in the ScheduledJob table.
  *
  */


#ifndef I_JOB_DATA
#define I_JOB_DATA


#include <string>
#include <map>

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug
#endif // _MSC_VER

namespace TA_Base_Core
{

class IJobData
{

public:

    /** A map of name/value pairs */
    typedef std::map< std::string, std::string > ParameterMap;


    /**
     *  Mark the data contained by this object as invalid. The next call to
     *  get...() following a call to invalidate() will cause all the data to
     *  be reloaded from the database.
     */
    virtual void invalidate() = 0;

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
     */
    virtual void reloadDataFromDB(bool forceReload = false) = 0;


    /**
     *  Gets the ID of the Job.
     *
     *  If the ID has not yet been set a new one will be created.
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
    virtual std::string getID() = 0;

	/**
     *  Gets the Order_Id of the Job.
     *
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
    virtual unsigned long getOrderId() = 0;

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
    virtual std::string getJobType() = 0;


    /**
     *  Gets the description of the job.
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
    virtual std::string getDescription() = 0;


    /**
     *  Gets the inhibition status of the job.
     *
     *  @return true if the job is inhibited, false otherwise.
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
    virtual bool getInhibitionStatus() = 0;


    /**
     *  Gets the ID of the location of the job.
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
    virtual unsigned long getLocationID() = 0;


    /**
     *  Gets the ID of the profile of the job.
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
    virtual unsigned long getProfileID() = 0;


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
    virtual std::string getParameter(const std::string& parameterName) = 0;


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
    virtual ParameterMap getParameters() = 0;

    /**
     *  The destructor.
     */
    virtual ~IJobData() {};

};


};// TA_Base_Core


#endif // I_JOB_DATA
