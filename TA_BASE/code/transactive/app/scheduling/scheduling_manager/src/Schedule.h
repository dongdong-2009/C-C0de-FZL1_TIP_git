/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/Schedule.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * A class encapsulating all data related to a schedule.
  */

#if !defined(Schedule_323F1FF3_C4FD_415d_B816_F91F606C42F5__INCLUDED_)
#define Schedule_323F1FF3_C4FD_415d_B816_F91F606C42F5__INCLUDED_

#include "app/scheduling/scheduling_manager/src/stdafx.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h"
#include "bus/scheduling/schedulingagentcorbadef/IDL/src/SchedulingAgentCorbaDef.h"

namespace TA_Base_App
{
    class Schedule
    {
    public:
	    Schedule();

        /** 
          * Schedule
          *
          * Create a schedule from the corba object
          *
          * @param def The schedule corba object
          */
        Schedule(const TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef& def);

	    virtual ~Schedule();

        /** 
          * operator==
          *
          * Returns true if the schedules are equal
          *
          * @param schedule The schedule to compare with
          *
          * @return true if jobs are equal
          */
        bool operator==(const Schedule& schedule) const;


        const Schedule& operator=(const Schedule& rhs);



        //////////////////////////////////////////////////////////////////////////////////
        //
        //
        // Accessor methods
        //
        //
        //////////////////////////////////////////////////////////////////////////////////

 
        /**
         * Gets the type of the Schedule. 
         */
        TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType getType() const
        {
            return m_type;
        }


        /**
         * Gets the day of the month of the Schedule. 
         */
        unsigned short getDayOfMonth() const
        {
            return m_dayOfMonth;
        }


        /**
         * Gets the month of the year of the Schedule. 
         */
        unsigned short getMonthOfYear() const
        {
            return m_month;
        }


        /**
         * Gets the year of the Schedule. 
         */
        unsigned short getYear() const
        {
            return m_year;
        }


        /**
         * Gets the days of the week of the Schedule. 
         */
        unsigned short getDaysOfWeek() const
        {
            return m_daysOfWeek;
        }


        /**
         * Gets the hour of the Schedule. (Note this is a 24 hour number, not 12) 
         */
        unsigned short getHour() const
        {
            return m_hour;
        }


        /**
         * Gets the minute of the Schedule. 
         */
        unsigned short getMinute() const
        {
            return m_minute;
        }


        /**
         * Gets the day on which the Schedule should run for the last time, or 0 if there
         * is no end date. This is only applicable for recurring schedules. 
         */
        unsigned short getEndDay() const
        {
            return m_endDay;
        }


        /**
         * Gets the month in which the Schedule should run for the last time. If there is
         * no end date this field is undefined. This is only applicable for recurring
         * schedules. 
         */
        unsigned short getEndMonth() const
        {
            return m_endMonth;
        }


        /**
         * Gets the year in which the Schedule should run for the last time, or 0 if there
         * is no end date. This is only applicable for recurring schedules. 
         */
        unsigned short getEndYear() const
        {
            return m_endYear;
        }


        /**
         * Indicates if this schedule has an end date, or if it recurs indefinitely. Once
         * off schedules do not have an end date. 
         */
        bool hasEndDate() const
        {
            return (m_type != TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff) && 
                (m_endDay != TA_Base_App::SchedulingManager::LAST_DAY_OF_MONTH);
        }


        /** 
          * isLastDay
          *
          * Returns true if it is on the last day of month
          *
          *
          * @return true if it is on the last day of month
          */
        bool isOnLastDay() const
        {
            return m_dayOfMonth == TA_Base_App::SchedulingManager::LAST_DAY_OF_MONTH;
        }

        /**
         * Converts the schedule to a TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef
         */
        TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef toCorbaObject() const;


        /** 
          * getEndDate
          *
          * If the end date is specified, return the end date
          *
          *
          * @return end date if it's specified, returns NULL if it's not.
          */
        COleDateTime* getEndDate() const;


        //////////////////////////////////////////////////////////////////////////////////
        //
        //
        // Mutator methods
        //
        //
        //////////////////////////////////////////////////////////////////////////////////

        /**
         * Sets the once-off schedule.
         */
        void setOnceOff(const COleDateTime& theTime, const COleDateTime& theDate);


        /**
         * Set the daily schedule.
         */
        void setDaily(const COleDateTime& theTime);


        /**
         * Sets the weekly schedule.
         */
        void setWeekly(const COleDateTime& theTime, const unsigned short theDay);


        /**
         * Sets the monthly schedule.
         */
        void setMonthly(const COleDateTime& theTime, const unsigned short theDay);


        /**
         * Sets the yearly schedule.
         */
        void setYearly(const COleDateTime& theTime, const unsigned int theMonth, const unsigned int theDay);


        /** 
          * setEndDate
          *
          * Set the end date for the schedule.  If this is NULL, then there is no end date.
          *
          * @param endDate end date for schedule.
          */
        void setEndDate(const COleDateTime& endDate);


        /** 
          * setEndDate
          *
          * Calculate the end date based on the number of repeats and schedule type
          *
          * @param numRepeats The number of repeats
          *
          * @return The calculated end date
          *
          */
        void setEndDate(const int numRepeats);

    private:

        /** 
          * setTime
          *
          * Sets the time the job is scheduled to run.
          *
          * @param theTime the time for the schedule
          */
        void setTime(const COleDateTime& theTime, bool startDate = true);


        /** 
          * getValidDate
          *
          * Returns a valid date based on the parameters.  It'll try to get a date
          * using the parameters directly, if that fails, it will try to get a valid date
          * on day 31.  If that fails, then it'll try day 30, then day 29, then day 28.
          *
          * @param year
          * @param month
          * @param day
          * @param hour
          * @param minute
          *
          * @return 
          *
          */
        COleDateTime getValidDate(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute) const;


        // Schedule type
        TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType m_type;

        // The time that the schedule runs
        unsigned short m_minute;
        unsigned short m_hour;

        // The date that the schedule starts running
        unsigned short m_dayOfMonth;
        unsigned short m_month;
        unsigned short m_year;

        // The date that the schedule stops running
        unsigned short m_endDay;
        unsigned short m_endMonth;
        unsigned short m_endYear;

        // The days that the schedule will run on if the type is weekly
        unsigned short m_daysOfWeek;

        // flag to indicate none of the days of week are selected
        static const int NO_WEEKDAYS;

        // The offset that we need to take into account when converting the date from
        // COleDateTime's months of year to the MonthOfYear of the agent
        static const int MONTH_OFFSET;

    };
};
#endif // !defined(Schedule_323F1FF3_C4FD_415d_B816_F91F606C42F5__INCLUDED_)
