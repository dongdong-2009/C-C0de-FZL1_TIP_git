/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/ConfigScheduleData.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Implements the IConfigScheduleData interface to allow manipulation
  * of a row of schedule data. Note that schedule data is stored in the
  * Job table, but is accessed using this separate class for convenience.
  *
  */


#ifndef CONFIG_SCHEDULE_DATA_H
#define CONFIG_SCHEDULE_DATA_H



#include "core/data_access_interface/scheduling_agent/src/IConfigScheduleData.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.h"

#include <vector>
#include <list>



namespace TA_Base_Core
{

class ConfigScheduleData: public IConfigScheduleData
{

public:

    /** 
     *  Gets all schedules from the database.
     */
    static ScheduleDataAccessFactory::IConfigScheduleDataVector
        ConfigScheduleData::getAllSchedules();


    /**
     *  Saves this Schedule to the database.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem deleting
     *  this Schedule from the database.
     *
     */
    void saveSchedule();


    /**
     *  Removes this Schedule from the database.
     *
     *  After calling this method, the schedule should be deleted from the
     *  database but the object's get methods should still be available until
     *  the object itself is deleted.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem deleting
     *  this Schedule from the database.
     *
     */
    void deleteSchedule();


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
    void applyChanges();


    /**
     *  Indicates that the data in the database has changed and it should be
     *  reloaded if required.
     */
    void invalidate();


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
     *             if any of the fields are out of range.
     */
    void reloadDataFromDB(bool forceReload = false);


    /**
     *  Gets the ID of the Schedule.
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
     *             if any of the fields are out of range.
     */
	std::string getID();


    /**
     *  Gets the type of the Schedule.
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
     *             if any of the fields are out of range.
     */
    unsigned short getScheduleType();


    /**
     *  Gets the minute of the Schedule.
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
     *             if any of the fields are out of range.
     */
    unsigned short getMinute();


    /**
     *  Gets the hour of the Schedule. (Note this is a 24 hour number, not 12)
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
     *             if any of the fields are out of range.
     */
    unsigned short getHour();


    /**
     *  Gets the day of the month of the Schedule.
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
     *             if any of the fields are out of range.
     */
    unsigned short getDayOfMonth();


    /**
     *  Gets the month of the year of the Schedule.
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
     *             if any of the fields are out of range.
     */
    unsigned short getMonthOfYear();

    /**
     *  Gets the year of the Schedule.
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
     *             if any of the fields are out of range.
     */
    unsigned short getYear();


    /**
     *  Gets the days of the week of the Schedule.
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
     *             if any of the fields are out of range.
     */
    std::list<unsigned short> getDaysOfWeek();


    /**
     *  Gets the day on which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
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
     *             if any of the fields are out of range.
     */
    virtual unsigned short getEndDay();

    /**
     *  Gets the month in which the Schedule should run for the last time.
     *  If there is no end date this field is undefined.
     *  This is only applicable for recurring schedules.
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
     *             if any of the fields are out of range.
     */
    virtual unsigned short getEndMonth();

    /**
     *  Gets the year in which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
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
     *             if any of the fields are out of range.
     */
    virtual unsigned short getEndYear();

    /**
     *  Indicates if this schedule has an end date, or if it recurs
     *  indefinitely. Once off schedules do not have an end date.
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
     *             if any of the fields are out of range.
     */
    virtual bool hasEndDate();

    /**
     *  Sets the Schedule id.
     */
    void setScheduleID( const std::string& id);


    /**
     *  Sets the type of the Schedule.
     */
    void setScheduleType( unsigned short scheduleType);


    /**
     *  Sets the minute of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setMinute( unsigned short minute);


    /**
     *  Sets the hour of the Schedule. (Note this is a 24 hour number, not 12)
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setHour( unsigned short hour );


    /**
     *  Sets the day of the month of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setDayOfMonth( unsigned short dayOfMonth);


    /**
     *  Sets the month of the year of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setMonthOfYear( unsigned short monthOfYear);

    
    /**
     *  Sets the year of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setYear( unsigned short year);


    /**
     *  Sets the days of the week of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setDaysOfWeek( const std::list<unsigned short>& daysOfWeek );


    /**
     *  Sets the day on which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setEndDay( unsigned short endDay);

    /**
     *  Sets the month in which the Schedule should run for the last time.
     *  If there is no end date this field is undefined.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setEndMonth( unsigned short endMonth);

    /**
     *  Sets the year in which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void setEndYear( unsigned short endYear);


    /**
     *  The destructor.
     */
    virtual ~ConfigScheduleData();


protected:


    //
    // ScheduleDataAccessFactory::getSchedule,
    // ScheduleDataAccessFactory::getAllSchedules and
    // ScheduleDataAccessFactory::createSchedule need to be able to call this
    // class' constructors.
    //
    friend
        IConfigScheduleData* ScheduleDataAccessFactory::getSchedule(
		const std::string& id);
    
    friend
        ScheduleDataAccessFactory::IConfigScheduleDataVector
            ScheduleDataAccessFactory::getAllSchedules();

    friend
        IConfigScheduleData* ScheduleDataAccessFactory::createSchedule(
		const std::string& jobID);

    friend
        IConfigScheduleData* ScheduleDataAccessFactory::createSchedule();

    /**
     *  Creates an empty ConfigScheduleData object.
     */
    ConfigScheduleData::ConfigScheduleData( );


    /**
     *  Creates a new ConfigScheduleData object and retrieves the information
     *  about it from the database.
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
     *             if any of the fields are out of range.
     */
    ConfigScheduleData::ConfigScheduleData( const std::string& id);



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
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    ConfigScheduleData::ConfigScheduleData(
        TA_Base_Core::IData* data, unsigned int rowNum);


    /**
     *  Takes a DayOfWeekSequence and converts it to a string.
     */
    std::string getDaysOfWeekAsString( const std::list<unsigned short>& daysOfWeek );


    /**
     *  Takes a string and converts it to a DayOfWeekSequence.
     *
     *  @exception BadScheduleException
     *             if no days of week are specified.
     */
    std::list<unsigned short> getDaysOfWeekFromString(const std::string& days);


    /**
     *  Creates the schedule in the database itself.
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    void ConfigScheduleData::createScheduleInDB();


	/**
	 *  Gets the data from the given row of the given data object.
     *  Note that this method will only work if the data object holds
     *  all the columns in the table.
	 *
     *  @param data    the IData object from which to retrieve the data.
     *  @param rowNum  the row number to access.
     *
	 *  @exception DataException
     *             thrown if a field is badly formatted.
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
	 */
    void getDataFromRow(TA_Base_Core::IData* data, unsigned int rowNum);


private:


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



	std::string	m_id;
    unsigned short m_type;
    unsigned short m_minute;
    unsigned short m_hour;
    unsigned short m_dayOfMonth;
    unsigned short m_monthOfYear;
    std::list<unsigned short> m_daysOfWeek;
    unsigned short m_year;
    unsigned short m_endDay;
    unsigned short m_endMonth;
    unsigned short m_endYear;


    /** Indicates if this schedule has been deleted from the DB */
    bool m_isDeleted;


    /** The separator used in days of week sequences */
    static const std::string DAYS_OF_WEEK_SEPARATOR;

    /** The table and column names */
    static const std::string SCHEDULING_TABLE_NAME;

    static const std::string ID_COLUMN;
    static const std::string SCHEDULE_TYPE_COLUMN;
    static const std::string MINUTE_COLUMN;
    static const std::string HOUR_COLUMN;
    static const std::string DAY_OF_MONTH_COLUMN;
    static const std::string MONTH_OF_YEAR_COLUMN;
    static const std::string YEAR_COLUMN;
    static const std::string DAYS_OF_WEEK_COLUMN;
    static const std::string END_DAY_COLUMN;
    static const std::string END_MONTH_COLUMN;
    static const std::string END_YEAR_COLUMN;


    typedef std::vector< std::string > ColumnNamesVector;
};


};// TA_Base_Core


#endif // CONFIG_SCHEDULE_DATA_H
