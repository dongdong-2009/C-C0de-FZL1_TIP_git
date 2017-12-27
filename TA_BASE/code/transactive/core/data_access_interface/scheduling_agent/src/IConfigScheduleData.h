/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/IConfigScheduleData.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface extends the IScheduleData interface to allow a row of
  * data in the Schedule table to be written as well as read.
  *
  */


#ifndef I_CONFIG_SCHEDULE_DATA_H
#define I_CONFIG_SCHEDULE_DATA_H


#include "core/data_access_interface/scheduling_agent/src/IScheduleData.h"

#include <list>
namespace TA_Base_Core
{

class IConfigScheduleData: public IScheduleData
{

public:


    /**
     *  Saves this Schedule to the database.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem storing
     *  this Schedule in the database.
     *
     */
    virtual void saveSchedule() = 0;


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
    virtual void deleteSchedule() = 0;


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
    virtual void applyChanges() = 0;


    /**
     *  Sets the Schedule Id.
     */
    virtual void setScheduleID( const std::string& id) = 0;


    /**
     *  Sets the type of the Schedule.
     */
    virtual void setScheduleType( unsigned short schedultType) = 0;


    /**
     *  Sets the minute of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setMinute( unsigned short minute) = 0;


    /**
     *  Sets the hour of the Schedule. (Note this is a 24 hour number, not 12)
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setHour( unsigned short hour ) = 0;


    /**
     *  Sets the day of the month of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setDayOfMonth( unsigned short dayOfMonth) = 0;


    /**
     *  Sets the month of the year of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setMonthOfYear( unsigned short monthOfYear) = 0;

    
    /**
     *  Sets the year of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setYear( unsigned short year) = 0;


    /**
     *  Sets the days of the week of the Schedule.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setDaysOfWeek( const std::list<unsigned short>& daysOfWeek ) = 0;


    /**
     *  Sets the day on which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setEndDay( unsigned short endDay) = 0;

    /**
     *  Sets the month in which the Schedule should run for the last time.
     *  If there is no end date this field is undefined.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setEndMonth( unsigned short endMonth) = 0;

    /**
     *  Sets the year in which the Schedule should run for the last time,
     *  or 0 if there is no end date.
     *  This is only applicable for recurring schedules.
     *
     *  @exception BadScheduleException
     *             if any of the fields are out of range.
     */
    virtual void setEndYear( unsigned short endYear) = 0;

    /**
     *  The destructor.
     */
    virtual ~IConfigScheduleData() {};

};


};// TA_Base_Core


#endif // I_CONFIG_SCHEDULE_DATA_H
