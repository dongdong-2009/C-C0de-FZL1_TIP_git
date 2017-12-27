/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/Schedule.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class provides some useful methods to be performed on a Schedule.
  * It is not used to write information about a schedule to the database.
  * This is done using the object returned from Job::getSchedule()
  *
  */


#ifndef SCHEDULE_H
#define SCHEDULE_H


#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/data_access_interface/scheduling_agent/src/IConfigScheduleData.h"



namespace TA_Base_App
{


class Schedule
{



public:

    /**
     *  Creates a new Schedule from the scheduleDataObject.
     *
     *  @param schedule  the object used to retrieve and set information about
     *                   this schedule in the database.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    Schedule(TA_Base_Core::IConfigScheduleData& schedule);


    /**
     *  Returns a Corba object representing this schedule.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
	TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef toCorbaObject() const;


    /**
     *  Gets the next time at which the given schedule should be executed,
     *
     *  eg The schedule says jobs should be executed daily at 10:00 am.
     *     If it's now 16:20, the next scheduled minute would be at 10:00 am
     *     tomorrow.
     *
     *  If the schedule has expired, ie it's a once off that has been run
     *  already, or it's a recurring job that has passed its end date,
     *  The day, month and year will be 0.
     *
     *  @return a RunTimeCorbaDef with the fields filled in or 0 if
     *          the schedule has expired.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     *
     */
    static TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef getNextScheduledTime(
        const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule);

    /**
     *  Gets the next time at which the given schedule should be executed,
     *
     *  eg The schedule says jobs should be executed daily at 10:00 am.
     *     If it's now 16:20, the next scheduled minute would be at 10:00 am
     *     tomorrow.
     *
     *  If the schedule has expired, ie it's a once off that has been run
     *  already, or it's a recurring job that has passed its end date,
     *  The day, month and year will be 0.
     *
     *  @return a RunTimeCorbaDef with the fields filled in or 0 if
     *          the schedule has expired.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     *
     */
    TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef getNextScheduledTime();


    /**
     *  Gets the next minute at which the given schedule should be executed,
     *  relative to the epoch.
     *
     *  eg The schedule says jobs should be executed daily at 10:00 am.
     *     If it's now 16:20, the next scheduled minute would be at 10:00 am
     *     tomorrow.
     *
     *  If the schedule has expired, ie it's a once off that has been run
     *  already, or it's a recurring job that has passed its end date,
     *  0 is returned.
     *
     *  @return the minute at which the job should next be executed, or 0 if
     *          the schedule has expired.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     *
     */
    static unsigned long getNextScheduledMinute(
        const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule);


    /**
     *  The same as the above method except using this object's fields.
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
     *
     *  @return the minute at which the job should next be executed, or 0 if
     *          the schedule has expired.
     *
     */
    unsigned long getNextScheduledMinute();



    /**
     *  Checks all the fields passed to this method to make sure the schedule
     *  is valid.
     *  eg if the date never occurs the schedule is invalid.
     *
     *  @exception BadScheduleException
     *             if the schedule is invalid.
     */
    static void checkIfFieldsAreValid(
        const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType type,
        const unsigned short minute,
        const unsigned short hour,
        const unsigned short dayOfMonth,
        const TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear monthOfYear,
        const unsigned short year,
        const TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence& daysOfWeek,
        const unsigned short endDay,
        const TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear endMonth,
        const unsigned short endYear);


    /**
     *  Checks the schedule passed to this method to make sure it is valid.
     *  eg if the date never occurs the schedule is invalid.
     *
     *  @exception BadScheduleException
     *             if the schedule is invalid.
     */
    static void checkIfFieldsAreValid(
        const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule);



    /**
     *  Gets a string representation of this object.
     */
    std::string toString();

    /**
     *  Gets a string representation of a corba schedule object.
     */
    static std::string toString( const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule );

    /**
     *  Gets the schedule type as a string
     */
    static std::string toString( const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType& type );

    /**
     *  Gets the given day of the week as a string.
     */
    static std::string getDayOfWeekAsString(
        const TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek day);


    /**
     *  Gets a DayOfWeekSequence as a string.
     */
    static std::string getDayOfWeekSequenceAsString(
        const TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence& daysOfWeek);

    /**
     *  Gets a 'normal' month of year (1 - 12) from the CORBA
     *  enum monthOfYear defination
     */
    static unsigned short Schedule::getMonthOfYear(
        const TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear month);

    /**
     *  Gets a corba enum monthOfYear of year from a 'normal' one
     */
    static TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear Schedule::getMonthOfYear(
        const unsigned short month);

    /**
     *  Gets a string from a corba enum monthOfYear
     */
    static std::string Schedule::getMonthAsString(const TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear month);

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
     */
    virtual std::string getID();


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
     *             if any of the schedule's fields are out of range.
     */
    virtual TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType getType();


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
     *             if any of the schedule's fields are out of range.
     */
    virtual unsigned short getMinute();


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
     *             if any of the schedule's fields are out of range.
     */
    virtual unsigned short getHour();


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
     *             if any of the schedule's fields are out of range.
     */
    virtual unsigned short getDayOfMonth();


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
     *             if any of the schedule's fields are out of range.
     */
    virtual TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear getMonthOfYear();

    
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
     *             if any of the schedule's fields are out of range.
     */
    virtual unsigned short getYear();


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
     *             if any of the schedule's fields are out of range.
     */
    virtual TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence getDaysOfWeek();


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
    virtual TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear getEndMonth();

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
     *  Destructor
     */
    virtual ~Schedule();

protected:


private:

	/**
	 *  Checks the range of the field parameter, and asserts if it's
     *  out of range
	 */
	static bool checkFieldInRange(
		unsigned short field,
		unsigned short lowerBound,
		unsigned short upperBound,
		const std::string& fieldName);


    Schedule();
    Schedule(const Schedule&);
    const Schedule& operator=(const Schedule&);

    TA_Base_Core::IConfigScheduleData& m_scheduleData;
};


}; // TA_Base_App


#endif // SCHEDULE_H
