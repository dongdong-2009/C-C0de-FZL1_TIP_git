/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/Schedule.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/11/01 15:29:57 $
  * Last modified by:  $Author: huangjian $
  *
  * This class provides some useful methods to be performed on a Schedule.
  * It is not used to write information about a schedule to the database.
  * This is done using the object returned from Job::getSchedule()
  *
  */

// Disable warnings from boost
#if defined(_MSC_VER)
  #pragma warning(disable:4244) // conversion - possible loss of data
#endif // _MSC_VER

#include "boost/date_time/posix_time/posix_time.hpp"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.h"
#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"
#include "app/scheduling/scheduling_agent/src/Schedule.h"

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace TA_Base_App
{


Schedule::Schedule(TA_Base_Core::IConfigScheduleData& schedule) : m_scheduleData(schedule)
{
}

Schedule::~Schedule()
{
}


TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef Schedule::toCorbaObject() const
{
    TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef corbaSchedule;
    
    corbaSchedule.type        = static_cast<TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType>(m_scheduleData.getScheduleType());
    corbaSchedule.minute      = m_scheduleData.getMinute();
    corbaSchedule.hour        = m_scheduleData.getHour();
    corbaSchedule.dayOfMonth  = m_scheduleData.getDayOfMonth();
    corbaSchedule.month       = static_cast<TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear>(m_scheduleData.getMonthOfYear());
    corbaSchedule.year        = m_scheduleData.getYear();
    std::list<unsigned short> days = m_scheduleData.getDaysOfWeek();
    if ( days.size() )
    {
        corbaSchedule.daysOfWeek.length(days.size());
        std::list<unsigned short>::iterator it = days.begin();
        unsigned int i = 0;
        while ( it != days.end() )
        {
            corbaSchedule.daysOfWeek[i++] = static_cast<TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek>(*it++);
        }
    }
    corbaSchedule.endDay      = m_scheduleData.getEndDay();
    corbaSchedule.endMonth    = static_cast<TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear>(m_scheduleData.getEndMonth());
    corbaSchedule.endYear     = m_scheduleData.getEndYear();

    return corbaSchedule;
}



unsigned long Schedule::getNextScheduledMinute() 
{
    return Schedule::getNextScheduledMinute( toCorbaObject() );
}

TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef Schedule::getNextScheduledTime()
{
    return Schedule::getNextScheduledTime( toCorbaObject() );
}

TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef Schedule::getNextScheduledTime(
    const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule)
{

    unsigned short minute = schedule.minute;
    unsigned short hour   = schedule.hour;
    unsigned short day = 0;
    unsigned short month = 0;
    unsigned short year = 0;
    unsigned long nextMinute = 0;
    SchedulingTimer now;
    date today(day_clock::local_day());
    TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef nextTime;
    nextTime.minute = minute;
    nextTime.hour = hour;
    nextTime.dayOfMonth = 0;
    nextTime.month = TA_Base_Bus::SchedulingAgentCorbaDef::January;
    nextTime.year = 0;
    
    switch( schedule.type )
    {
        //
        // Daily
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::Daily:
        {
            ptime nextTime(today, time_duration(hour,minute,0));

            day_iterator nextDay( nextTime.date() );
            while ( now.isPast( ptime(*nextDay, time_duration(hour,minute,0))) )
            {
                ++nextDay;
            }
            year  = nextDay->year();
            month = nextDay->month();
            day   = nextDay->day();
        }
        break;
        //
        // Monthly
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::Monthly:
        {
            unsigned short dayOfMonth = ( schedule.dayOfMonth == 0 ) ? gregorian_calendar::end_of_month_day( today.year(), today.month() ) : schedule.dayOfMonth;

            month = today.month();
            while ( dayOfMonth > gregorian_calendar::end_of_month_day( today.year(), month ))
            {
                if ( month >= boost::date_time::Dec )
                {
                    break;
                }
                else
                {
                    ++month;
                }
            }

            ptime nextTime( date( today.year(), month, dayOfMonth ), time_duration( hour, minute, 0 ));

            month_iterator nextMonth( nextTime.date() );
            while ( now.isPast( ptime(*nextMonth, time_duration(hour,minute,0))) )
            {
                ++nextMonth;
            }
            year  = nextMonth->year();
            month = nextMonth->month();
            day   = nextMonth->day();
        }
        break;
        //
        // Once off
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff:
        {
            day   = schedule.dayOfMonth;
            month = getMonthOfYear( schedule.month );
            year  = schedule.year;

            // if this time is past exit stage left
            if ( now.isPast(ptime( date( year, month, day ),  time_duration(hour,minute,0))) )
            {
                return nextTime;
            }
        }
        break;
        // 
        // Yearly
        // 
        case TA_Base_Bus::SchedulingAgentCorbaDef::Yearly:
        {
            unsigned short monthOfYear = getMonthOfYear( schedule.month );
            unsigned short dayOfMonth = schedule.dayOfMonth;
            if ( dayOfMonth == 0 )
            {
                dayOfMonth = gregorian_calendar::end_of_month_day( today.year(), monthOfYear );
            }

            ptime nextTime( date( today.year(), monthOfYear, dayOfMonth ),
                             time_duration( hour, minute, 0) );

            // have to use 12 month as adding a year doesn't seem to be able to handle 29 Feb
            month_iterator nextMonth( nextTime.date() );

            while ( now.isPast( ptime(*nextMonth, time_duration(hour,minute,0))) )
            {
                ++nextMonth; // doesn't like += either :(
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
                ++nextMonth;
            }

            year  = nextMonth->year();
            month = nextMonth->month();
            day   = nextMonth->day();
        }
        break;
        //
        // Weekly
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::Weekly:
        {
            ptime nextTime(today, time_duration(hour,minute,0));
            std::string daysOfWeek = getDayOfWeekSequenceAsString(schedule.daysOfWeek);

            day_iterator nextDay( nextTime.date() );

            // start by getting a day in the future
            while ( now.isPast( ptime( *nextDay, time_duration(hour,minute,0))) )
            {
                    ++nextDay;
            }

            // now make sure it matches a day in the list of valid days
            std::string thisDay(nextDay->day_of_week().as_long_string());
            while ( daysOfWeek.find( thisDay ) == std::string::npos )
            {
                ++nextDay;
                thisDay = nextDay->day_of_week().as_long_string();
            }
            year  = nextDay->year();
            month = nextDay->month();
            day   = nextDay->day();
        }
        break;
        default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognised Schedule Type");
			return nextTime;
    }

    ptime nextScheduledTime( date( year, month, day ),  time_duration(hour,minute,0));

    //
    // If this job's run time is past the end date, return 0.
    //
    bool hasEndDate = (schedule.endDay != 0);
    if(hasEndDate)
    {
        unsigned short endDay   = schedule.endDay;
        unsigned short endMonth = getMonthOfYear( schedule.endMonth );
        unsigned short endYear  = schedule.endYear;
        ptime  endTime( date( endYear, endMonth, endDay), time_duration(hour,minute,0));
        if ( nextScheduledTime > endTime )
        {
            return nextTime;
        }
    }
    nextTime.dayOfMonth = day;
    nextTime.month = getMonthOfYear(month);
    nextTime.year = year;
    return nextTime;
}

unsigned long Schedule::getNextScheduledMinute(
    const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule)
{

    SchedulingTimer now;

    TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef nextTime = getNextScheduledTime( schedule );

    // if we do not have a vaild date, there can be no next time
    if ( nextTime.year == 0 )
    {
        return 0;
    }


    ptime nextScheduledTime( date( nextTime.year, 
                                   getMonthOfYear( nextTime.month ), 
                                   nextTime.dayOfMonth ),
                             time_duration(
                                   nextTime.hour, 
                                   nextTime.minute, 0));

    return now.getNextTimeInMinutes(nextScheduledTime);
}

std::string Schedule::toString()
{
    std::stringstream scheduleStream;


    scheduleStream
        << "id: " << getID() << std::endl
        << "type: " << toString( getType() ) << std::endl
        << "time: " << getHour() << ":" << getMinute() << std::endl
        << "dayOfMonth: " << getDayOfMonth() << std::endl
        << "monthOfYear: " << getMonthOfYear() << std::endl
        << "daysOfWeek: ";

    TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence daysOfWeek = getDaysOfWeek();
    for(unsigned int i = 0; i < daysOfWeek.length(); i ++)
    {
        if(i != 0)
        {
            scheduleStream << ", ";
        }
        
        scheduleStream << getDayOfWeekAsString(daysOfWeek[i]);
    }

    scheduleStream << std::ends;

    return scheduleStream.str();
}

std::string Schedule::toString( const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule )
{
    std::stringstream scheduleStream;

    
    scheduleStream
        << "type: " << toString( schedule.type ) << std::endl
        << "time: " << schedule.hour << ":" << schedule.minute << std::endl
        << "dayOfMonth: " << schedule.dayOfMonth << std::endl
        << "monthOfYear: " << getMonthAsString( schedule.month ) << std::endl
        << "daysOfWeek: " << getDayOfWeekSequenceAsString(schedule.daysOfWeek ) << std::endl
        << "Year: " << schedule.year << std::endl
        << "End Date: " << schedule.endDay
        << " " << getMonthAsString( schedule.endMonth )
        << " " << schedule.endYear << std::endl
        << std::ends;

    return scheduleStream.str();
}

#ifdef _MSC_VER
	#pragma warning(push)          // save currrent warning state
	#pragma warning(disable: 4715) // not all control paths return a value 
                                   // they ASSERT instead!
#endif // _MSC_VER

std::string Schedule::toString( const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType& type )
{
    switch ( type )
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::Daily:
            return std::string("Daily");
        case TA_Base_Bus::SchedulingAgentCorbaDef::Monthly:
            return std::string("Monthly");
        case TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff:
            return std::string("OnceOff");
        case TA_Base_Bus::SchedulingAgentCorbaDef::Yearly:
            return std::string("Yearly");
        case TA_Base_Bus::SchedulingAgentCorbaDef::Weekly:
            return std::string("Weekly");
        default:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognised Schedule Type");
			return "";
    }
}

unsigned short Schedule::getMonthOfYear(const TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear month)
{
    switch(month)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::January:
            return boost::date_time::Jan;
        case TA_Base_Bus::SchedulingAgentCorbaDef::February:
            return boost::date_time::Feb;
        case TA_Base_Bus::SchedulingAgentCorbaDef::March:
            return boost::date_time::Mar;
        case TA_Base_Bus::SchedulingAgentCorbaDef::April:
            return boost::date_time::Apr;
        case TA_Base_Bus::SchedulingAgentCorbaDef::May:
            return boost::date_time::May;
        case TA_Base_Bus::SchedulingAgentCorbaDef::June:
            return boost::date_time::Jun;
        case TA_Base_Bus::SchedulingAgentCorbaDef::July:
            return boost::date_time::Jul;
        case TA_Base_Bus::SchedulingAgentCorbaDef::August:
            return boost::date_time::Aug;
        case TA_Base_Bus::SchedulingAgentCorbaDef::September:
            return boost::date_time::Sep;
        case TA_Base_Bus::SchedulingAgentCorbaDef::October:
            return boost::date_time::Oct;
        case TA_Base_Bus::SchedulingAgentCorbaDef::November:
            return boost::date_time::Nov;
        case TA_Base_Bus::SchedulingAgentCorbaDef::December:
            return boost::date_time::Dec;
        default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Corba month");
            return boost::date_time::Jan;
    }
}

std::string Schedule::getMonthAsString(const TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear month)
{
    switch(month)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::January:
            return std::string( "January" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::February:
            return std::string( "February" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::March:
            return std::string( "March" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::April:
            return std::string( "April" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::May:
            return std::string( "May" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::June:
            return std::string( "June" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::July:
            return std::string( "July" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::August:
            return std::string( "August" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::September:
            return std::string( "September" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::October:
            return std::string( "October" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::November:
            return std::string( "November" );
        case TA_Base_Bus::SchedulingAgentCorbaDef::December:
            return std::string( "December" );
        default:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Corba month");
			return "";
    }
}

TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear Schedule::getMonthOfYear(const unsigned short month)
{
    switch(month)
    {
        case boost::date_time::Jan:
            return TA_Base_Bus::SchedulingAgentCorbaDef::January;
        case boost::date_time::Feb:
            return TA_Base_Bus::SchedulingAgentCorbaDef::February;
        case boost::date_time::Mar:
            return TA_Base_Bus::SchedulingAgentCorbaDef::March;
        case boost::date_time::Apr:
            return TA_Base_Bus::SchedulingAgentCorbaDef::April;
        case boost::date_time::May:
            return TA_Base_Bus::SchedulingAgentCorbaDef::May;
        case boost::date_time::Jun:
            return TA_Base_Bus::SchedulingAgentCorbaDef::June;
        case boost::date_time::Jul:
            return TA_Base_Bus::SchedulingAgentCorbaDef::July;
        case boost::date_time::Aug:
            return TA_Base_Bus::SchedulingAgentCorbaDef::August;
        case boost::date_time::Sep:
            return TA_Base_Bus::SchedulingAgentCorbaDef::September;
        case boost::date_time::Oct:
            return TA_Base_Bus::SchedulingAgentCorbaDef::October;
        case boost::date_time::Nov:
            return TA_Base_Bus::SchedulingAgentCorbaDef::November;
        case boost::date_time::Dec:
            return TA_Base_Bus::SchedulingAgentCorbaDef::December;
        default:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Boost month");
            return TA_Base_Bus::SchedulingAgentCorbaDef::January;
    }
}

std::string Schedule::getDayOfWeekAsString(TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek day)
{
    switch(day)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::Monday:
            return "Monday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Tuesday:
            return "Tuesday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Wednesday:
            return "Wednesday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Thursday:
            return "Thursday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Friday:
            return "Friday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Saturday:
            return "Saturday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Sunday:
            return "Sunday";
        default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognised day of week");
			return "";
    }
}

#ifdef _MSC_VER
	#pragma warning(pop) // restore warning state 
#endif // _MSC_VER


std::string Schedule::getDayOfWeekSequenceAsString(
    const TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence& daysOfWeek)
{
    if(daysOfWeek.length() == 0)
    {
        return "";
    }

    std::string daysOfWeekString = getDayOfWeekAsString(daysOfWeek[0]);

    for(unsigned int i = 1; i < daysOfWeek.length(); i ++)
    {
        daysOfWeekString += ", ";
        daysOfWeekString += getDayOfWeekAsString(daysOfWeek[i]);
    }


    return daysOfWeekString;
}


void Schedule::checkIfFieldsAreValid(
    const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& schedule)
{
    checkIfFieldsAreValid(
        schedule.type,
        schedule.minute,
        schedule.hour,
        schedule.dayOfMonth,
        schedule.month,
        schedule.year,
        schedule.daysOfWeek,
        schedule.endDay,
        schedule.endMonth,
        schedule.endYear);
}


void Schedule::checkIfFieldsAreValid(
    const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType type,
    const unsigned short minute,
    const unsigned short hour,
    const unsigned short corbaDayOfMonth,
    const TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear corbaMonthOfYear,
    const unsigned short year,
    const TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence& daysOfWeek,
    const unsigned short endDay,
    const TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear corbaEndMonth,
    const unsigned short endYear)
{

    unsigned short monthOfYear = getMonthOfYear( corbaMonthOfYear );
    unsigned short endMonth = getMonthOfYear( corbaEndMonth );
    unsigned short dayOfMonth = corbaDayOfMonth;
    // basic range checks
    if ( checkFieldInRange(minute, 0, 59, "Minute") == false )
    {
        throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
            TA_Base_Bus::SchedulingAgentCorbaDef::InvalidMinute, "Invalid minute");
    }
    if ( checkFieldInRange(hour, 0, 23, "Hour") == false )
    {
        throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
            TA_Base_Bus::SchedulingAgentCorbaDef::InvalidHour, "Invalid hour");
    }
        

    //
    // If this is a recurring schedule that has an end date
    //
    if(type != TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff)
    {
        bool hasEndDate = (endDay != 0);
        if(hasEndDate)
        {
            try
            {
                date(endYear, endMonth, endDay);
            }
            catch(boost::gregorian::bad_day_of_month)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidDayOfMonth,
                    "The end date is not a valid date");
            }
            catch(boost::gregorian::bad_month)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidMonth,
                    "The end date is not a valid date");
            }
            catch(boost::gregorian::bad_year)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidYear,
                    "The end date is not a valid date");
            }
        }
    }
    //
    // If this is not a recurring schedule, make sure the end date is not
    // specified.
    //
    else
    {
        if(endDay != 0)
        {
            throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::EndDateNotRequired,
                    "An end date was specified for a non-recurring job");
        }
    }

    switch(type)
    {
        //
        // Once off
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff:
            try
            {
                date(year, monthOfYear, dayOfMonth);
            }
            catch(boost::gregorian::bad_day_of_month)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidDayOfMonth,
                    "The date is not a valid date");
            }
            catch(boost::gregorian::bad_month)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidMonth,
                    "The date is not a valid date");
            }
            catch(boost::gregorian::bad_year)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidYear,
                    "The date is not a valid date");
            }
            break;

        //
        // Yearly
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::Yearly:
            // if this date is a valid day this year it will be vaild every year
            // unless it's 29, Feburary
            try
            {
                if ( dayOfMonth == 0 )
                { 
                    dayOfMonth = gregorian_calendar::end_of_month_day( year, monthOfYear );
                }
                date(year, monthOfYear, dayOfMonth);
            }
            catch(boost::gregorian::bad_day_of_month)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidDayOfMonth,
                    "The date is not a valid date");
            }
            catch(boost::gregorian::bad_month)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidMonth,
                    "The date is not a valid date");
            }
            catch(boost::gregorian::bad_year)
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidYear,
                    "The date is not a valid date");
            }
            break;
        //
        // Monthly
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::Monthly:
            if ( checkFieldInRange(dayOfMonth, 0, 31, "Day of Month") == false )
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::InvalidDate,
                    "The day specified is not a valid recurring day");
            }
            if ( dayOfMonth == 0 )
            { 
                dayOfMonth = gregorian_calendar::end_of_month_day( year, monthOfYear );
            }

            break;

        //
        // Weekly
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::Weekly:
            if ( daysOfWeek.length() == 0 ) 
            {
                throw TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException(
                    TA_Base_Bus::SchedulingAgentCorbaDef::MissingDayOfWeek,
                    "There must be at least one day of the week specified.");
            }
            break;

        //
        // Daily
        //
        case TA_Base_Bus::SchedulingAgentCorbaDef::Daily:
            // hour and minute are checked above
            break;

        default: 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognised Schedule Type");
    }// switch(type)

}


bool Schedule::checkFieldInRange(
    unsigned short field,
    unsigned short lowerBound,
    unsigned short upperBound,
    const std::string& fieldName)
{
    if ( lowerBound > field || field > upperBound )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s is out of range: %d.  Must be between %d - %d", 
            fieldName.c_str(), field, lowerBound, upperBound);
        return false;
    }
    return true;
}


std::string Schedule::getID()
{
    return m_scheduleData.getID();
}

TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType Schedule::getType()
{
    return static_cast<TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType>(m_scheduleData.getScheduleType());
}

unsigned short Schedule::getMinute()
{
    return m_scheduleData.getMinute();
}

unsigned short Schedule::getHour()
{
    return m_scheduleData.getHour();
}

unsigned short Schedule::getDayOfMonth()
{
    return m_scheduleData.getDayOfMonth();
}

TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear Schedule::getMonthOfYear()
{
    return static_cast<TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear>(m_scheduleData.getMonthOfYear());
}

unsigned short Schedule::getYear()
{
    return m_scheduleData.getYear();
}

TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence Schedule::getDaysOfWeek()
{
    TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence daysOfWeek;
    std::list<unsigned short> days = m_scheduleData.getDaysOfWeek();
    daysOfWeek.length(days.size());
    unsigned int i =0;
    for ( std::list<unsigned short>::iterator it = days.begin();
          it != days.end(); i++, it++ )
    {
        daysOfWeek[i] = static_cast<TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek>(*it);
    }
    return daysOfWeek;
}


unsigned short Schedule::getEndDay()
{
    return m_scheduleData.getEndDay();
}

TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear Schedule::getEndMonth()
{
    return static_cast<TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear>(m_scheduleData.getEndMonth());
}

unsigned short Schedule::getEndYear()
{
    return m_scheduleData.getEndYear();
}

bool Schedule::hasEndDate()
{
    return m_scheduleData.hasEndDate();
}


}; // TA_Base_App
