/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/ConfigJobData.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Implements the IConfigJobData interface to allow manipulation
  * of a row of data in the Job table.
  *
  */


#ifndef CONFIG_JOB_DATA_H
#define CONFIG_JOB_DATA_H



#include "core/data_access_interface/scheduling_agent/src/IConfigJobData.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"

#include <vector>
#include <string>
#include <map>

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug
#endif // _MSC_VER

namespace TA_Base_Core
{

class ConfigJobData: public IConfigJobData
{

public:


    /** 
     *  Gets all jobs from the database.
     *
     *  @return a vector of IConfigJobData pointers that must be deleted by
     *          the caller.
     *
     *  @exception BadScheduleException
     *  Thrown if the format of the schedule is invalid
     *  (eg it represents the 31st of February)
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if there is a problem with the format
     *  of the data.
     *
     */
    static JobDataAccessFactory::IConfigJobDataVector
        ConfigJobData::getAllJobs();

    /** 
      * Gets all jobs of the specified type.
      *
      * @param jobType  the type of job. This must not be blank
      *
      * @return a vector of IConfigJobData pointers that must be deleted by
      *         the caller.
      *
      *  @exception BadScheduleException
      *  Thrown if the format of the schedule is invalid
      *  (eg it represents the 31st of February)
      *
      *  @exception DatabaseException
      *  A DatabaseException will be thrown if there is a problem retrieving
      *  values from the database.
      *
      *  @exception DataException
      *  A DataException will be thrown if there is a problem with the format
      *  of the data.
      *
      */
    static JobDataAccessFactory::IConfigJobDataVector
        getAllJobsOfType(const std::string& jobType);




    /**
     *  Saves this job to the database.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     */
     void saveJob();


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
     void deleteJob();


    /**
     *  This will write all changes made to the database.
     *
     *  @exception DatabaseException
     *  Thrown if there is a database error.
     *
     *  @exception DataException
     *  Thrown if the a parameter name cannot be found, or if there is more
     *  than one value for a parmeter.
     *
     */
    void applyChanges();


    /**
     *  Indicates that the data in the database has changed and it should be
     *  reloaded if required.
     */
    void invalidate();


    /**
     *  Gets the ID of the Job.
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
	std::string getID();

	/**
     *  Gets the Order_Id of the Job.
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
	unsigned long getOrderId();


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
    std::string getJobType();


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
    std::string getDescription();


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
    bool getInhibitionStatus();


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
    unsigned long getLocationID();


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
    unsigned long getProfileID();


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
    std::string getParameter(const std::string& parameterName);


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
    ParameterMap getParameters();


    /**
     *  Sets the id of this job
     */
    void setId(const std::string& id);

	/**
     *  Sets the id of this job
     */
    void setOrderId(unsigned long orderId);

    
    /**
     *  Sets the type of job that this is, eg PlanRunnerJob
     */
    void setJobType(const std::string& jobType);

    
    /**
     *  Sets the description of the job.
     */
    void setDescription(const std::string& description);


    /**
     *  Sets the inhibition status of the job.
     */
    void setInhibitionStatus(bool isInhibited);


    /**
     *  Sets the ID of the location of the job.
     */
    void setLocationID(unsigned long locationID);


    /**
     *  Sets the ID of the profile of the job.
     */
    void setProfileID(unsigned long profileID);


    /**
     *  Sets the given parameter. If the parameter does not exist,
     *  it's created.
     *
     *  If the name or value contains PARAMETER_SEPARATOR the program asserts.
     */
    void setParameter(
        const std::string& parameterName,
        const std::string& parameterValue);


    /**
     *  Gets a pointer to the schedule associated with this job.
     *  This object is responsible for managing the pointer's memory.
     */
    IConfigScheduleData* getSchedule();


    /**
     *  Deletes the given parameter. If the parameter does not exist,
     *  it silently returns
     *
     *  If the name contains PARAMETER_SEPARATOR the program asserts.
     */
    void deleteParameter( const std::string& parameterName );


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
     *  @exception JobDoesNotExistException
     *  Thrown if no entry is found in the database for the ID stored in this
     *  object.
     * 
     */
    void reloadDataFromDB(bool forceReload = false);



    /**
     *  The destructor.
     */
    virtual ~ConfigJobData();


protected:


    //
    // JobDataAccessFactory::getJob and JobDataAccessFactory::createJob
    // need to be able to call this class' constructors.
    //
    friend
        IConfigJobData* JobDataAccessFactory::getJob(const std::string& id);

    friend
        IConfigJobData* JobDataAccessFactory::createJob();
    
    /**
     *  Creates a new ConfigJobData object and retrieves the information
     *  about it from the database.
     *
     *  @exception JobDoesNotExistException
     *  Thrown if a job with the given ID does not exist.
     *
     *  @exception BadScheduleException
     *  Thrown if the format of the schedule is invalid
     *  (eg it represents the 31st of February)
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if there is a problem with the format
     *  of the data.
     */
    ConfigJobData::ConfigJobData(const std::string& id);


    /**
     *  Creates a new ConfigJobData object. A new job id is created.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     */
    ConfigJobData::ConfigJobData();


    /**
     *  Constructor that takes an IData object and a row number
     *  as a parameter, and creates an object from that row.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if any of the fields in the database
     *  are badly formatted.
     */
    ConfigJobData::ConfigJobData(
        TA_Base_Core::IData* data, unsigned int rowNum);



private:

    
    /** 
      * Gets all jobs of a particular type or all jobs of any type.
      *
      * @param jobType  the job type. If this is blank, all jobs are returned.
      *
      * @return a vector of pointers to IConfigJobData objects that the caller
      *         must delete.
      */
    static JobDataAccessFactory::IConfigJobDataVector
        ConfigJobData::getAllJobsOfAnyType(const std::string& jobType);

    /**
     *  Gets a new job Id from the database
     */
	std::string getNewID();

	/**
     *  Gets a new order Id from the database
     */
	unsigned long getNewOrderId();

    /**
     *  Creates the Job in the database itself.
     */
    void createJobInDB();


	/**
	 *  Gets the data from the given row of the given data object.
     *  Note that this method will only work if the data object holds
     *  all the columns in the table.
	 *
     *  @param data    the IData object from which to retrieve the data.
     *  @param rowNum  the row number to access.
     *
	 *  @exception DataException    thrown if a field is badly formatted.
	 */
    void getDataFromRow(TA_Base_Core::IData* data, unsigned int rowNum);


    /**
     *  Gets the m_parameters field as a string that can be put in the
     *  database.
     */
    std::string getParametersAsString();


    /**
     *  Converts the given string to a ParameterMap.
     */
    IJobData::ParameterMap getParametersAsMap(
        const std::string& parametersString);


    /**
     * getLocalDatabaseName
     *
     * returns the name of the local database
     */
    std::string getLocalDatabaseName();

    // The name of the local database to send updates to
    std::string m_localDatabase;





    /** Indicates whether the data needs to be reloaded from the database. */
    bool m_dataNeedsToBeReadFromDB;

    /**
     *  Indicates if the data has been changed in memory but not in
     *  the database, and therefore needs to be written to the DB.
     */
    bool m_dataNeedsToBeWrittenToDB;



	std::string	  m_id;
	unsigned long m_orderId;
    std::string   m_jobType;
    std::string   m_description;
    bool          m_isInhibited;
    unsigned long m_locationID;
    unsigned long m_profileID;


    /**
     *  A group of parameters may be associated with this object in the
     *  database. This map stores their names and values.
     */
    IJobData::ParameterMap m_parameters;

    IConfigScheduleData* m_schedule;


    /** Indicates whether this object has been deleted from the DB */
    bool m_isDeleted;


    /** The table and column names */
    static const std::string SCHEDULED_JOB_TABLE_NAME;
    static const std::string SCHEDULED_JOB_SEQUENCE;

    static const std::string ID_COLUMN;
	static const std::string ORDER_ID_COLUMN;
    static const std::string JOB_TYPE_COLUMN;
    static const std::string DESCRIPTION_COLUMN;
    static const std::string IS_INHIBITED_COLUMN;
    static const std::string LOCATION_ID_COLUMN;
    static const std::string PROFILE_ID_COLUMN;
    static const std::string PARAMETERS_COLUMN;

    typedef std::vector< std::string > ColumnNamesVector;

    
    /** The strings that indicate that isInhibited is true or false */
    static const std::string IS_INHIBITED_TRUE;
    static const std::string IS_INHIBITED_FALSE;

    /** The string used to separate parameters */
    static const std::string PARAMETER_SEPARATOR;

    /** private copy constructor cause nobody's 'possed to call it */
    ConfigJobData( const ConfigJobData& );
    ConfigJobData operator=( ConfigJobData& );
};


};// TA_Base_Core


#endif // CONFIG_JOB_DATA_H
