/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/IScheduleData.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface represents a row of data in the Schedule table.
  *
  */


#ifndef I_SCHEDULE_DATA_H
#define I_SCHEDULE_DATA_H

#include <list>

namespace TA_Base_Core
{

class IScheduleData
{

public:

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
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void reloadDataFromDB(bool forceReload = false) = 0;


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
    virtual std::string getID() = 0;


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
    virtual unsigned short getScheduleType() = 0;


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
    virtual unsigned short getMinute() = 0;


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
    virtual unsigned short getHour() = 0;


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
    virtual unsigned short getDayOfMonth() = 0;


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
    virtual unsigned short getMonthOfYear() = 0;

    
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
     *  the required format (e.g. YEAR should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual unsigned short getYear() = 0;


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
    virtual std::list<unsigned short> getDaysOfWeek() = 0;


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
    virtual unsigned short getEndDay() = 0;

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
    virtual unsigned short getEndMonth() = 0;

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
    virtual unsigned short getEndYear() = 0;

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
    virtual bool hasEndDate() = 0;


    /**
     *  The destructor.
     */
    virtual ~IScheduleData() {};

};


};// TA_Base_Core


#endif // I_SCHEDULE_DATA_H
