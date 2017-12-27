/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/Schedule.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * A class encapsulating all data related to a schedule.
  */

#include "app/scheduling/scheduling_manager/src/Schedule.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Bus::SchedulingAgentCorbaDef;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    const int Schedule::NO_WEEKDAYS = 0;
    const int Schedule::MONTH_OFFSET = 1;

    Schedule::Schedule()
    :
        m_type(SchedulingAgentCorbaDef::OnceOff),
        m_minute(0),
        m_hour(0),
        m_dayOfMonth(0),
        m_month(MONTH_OFFSET),     // Initialise to January
        m_year(0), 
        m_endDay(0),
        m_endMonth(MONTH_OFFSET),  // Initialise to January
        m_endYear(0),
        m_daysOfWeek(NO_WEEKDAYS)
    {
		// if zero the job is not scheduled
    }


    Schedule::Schedule(const SchedulingAgentCorbaDef::ScheduleCorbaDef& def)
    : 
        m_type(def.type),
        m_minute(def.minute),
        m_hour(def.hour),
        m_dayOfMonth(def.dayOfMonth),
        m_month(def.month + MONTH_OFFSET),
        m_year(def.year), 
        m_endDay(def.endDay),
        m_endMonth(def.endMonth + MONTH_OFFSET),
        m_endYear(def.endYear),
        m_daysOfWeek(NO_WEEKDAYS)
    {
        FUNCTION_ENTRY("Constructor");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job details");
        LOGMORE ( SourceInfo, "Type = %d", m_type);
        LOGMORE ( SourceInfo, "Time = %d:%d", m_hour, m_minute);
        LOGMORE ( SourceInfo, "Scheduled Date = %d-%d-%d", m_dayOfMonth, m_month, m_year);
        LOGMORE ( SourceInfo, "End Date = %d-%d-%d", m_endDay, m_endMonth, m_endYear);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Days of week are");
        for (unsigned int i = 0; i < def.daysOfWeek.length(); i++)
        {
            switch (def.daysOfWeek[i])
            {
            case SchedulingAgentCorbaDef::Monday:
                m_daysOfWeek |= TA_Base_App::SchedulingManager::Monday;
                LOGMORE ( SourceInfo, "Monday");
                break;
            case SchedulingAgentCorbaDef::Tuesday:
                m_daysOfWeek |= TA_Base_App::SchedulingManager::Tuesday;
                LOGMORE ( SourceInfo, "Tuesday");
                break;
            case SchedulingAgentCorbaDef::Wednesday:
                m_daysOfWeek |= TA_Base_App::SchedulingManager::Wednesday;
                LOGMORE ( SourceInfo, "Wednesday");
                break;
            case SchedulingAgentCorbaDef::Thursday:
                m_daysOfWeek |= TA_Base_App::SchedulingManager::Thursday;
                LOGMORE ( SourceInfo, "Thursday");
                break;
            case SchedulingAgentCorbaDef::Friday:
                m_daysOfWeek |= TA_Base_App::SchedulingManager::Friday;
                LOGMORE ( SourceInfo, "Friday");
                break;
            case SchedulingAgentCorbaDef::Saturday:
                m_daysOfWeek |= TA_Base_App::SchedulingManager::Saturday;
                LOGMORE ( SourceInfo, "Saturday");
                break;
            case SchedulingAgentCorbaDef::Sunday:
                m_daysOfWeek |= TA_Base_App::SchedulingManager::Sunday;
                LOGMORE ( SourceInfo, "Sunday");
                break;
            default:
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                    "Unable to figure out the day of week.  Ignore");
                // don't know what we got, so just ignore
                break;
            }
        }
        FUNCTION_EXIT;
    }

 
    Schedule::~Schedule()
    {

    }


    bool Schedule::operator==(const Schedule& schedule) const
    {
        return ((m_type == schedule.m_type) &&
                (m_minute == schedule.m_minute) &&
                (m_hour == schedule.m_hour) &&
                (m_dayOfMonth == schedule.m_dayOfMonth) &&
                (m_daysOfWeek == schedule.m_daysOfWeek) &&
                (m_month == schedule.m_month) &&
                (m_year == schedule.m_year) && 
                (m_endDay == schedule.m_endDay) &&
                (m_endMonth == schedule.m_endMonth) &&
                (m_endYear == schedule.m_endYear));
    }


    const Schedule& Schedule::operator=(const Schedule& rhs)
    {
        // gurad against self assignment
        if (this != &rhs)
        {
            m_type = rhs.m_type;
            m_minute = rhs.m_minute;
            m_hour = rhs.m_hour;
            m_dayOfMonth = rhs.m_dayOfMonth;
            m_daysOfWeek = rhs.m_daysOfWeek;
            m_month = rhs.m_month;
            m_year = rhs.m_year;
            m_endDay = rhs.m_endDay;
            m_endMonth = rhs.m_endMonth;
            m_endYear = rhs.m_endYear;
        }
        return *this;
    }


    SchedulingAgentCorbaDef::ScheduleCorbaDef Schedule::toCorbaObject() const
    {
        FUNCTION_ENTRY("toCorbaObject");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Schedule details");
        LOGMORE ( SourceInfo, "Type = %d", m_type);
        LOGMORE ( SourceInfo, "Time = %d:%d", m_hour, m_minute);
        LOGMORE ( SourceInfo, "Scheduled Date = %d-%d-%d", m_dayOfMonth, m_month, m_year);
        LOGMORE ( SourceInfo, "End Date = %d-%d-%d", m_endDay, m_endMonth, m_endYear);

        SchedulingAgentCorbaDef::ScheduleCorbaDef schedule;
        schedule.type = static_cast<SchedulingAgentCorbaDef::ScheduleType>(m_type);
        schedule.minute = m_minute;
        schedule.hour = m_hour;
        schedule.dayOfMonth = m_dayOfMonth;
        // the m_month is storing month 1 - 12 but the MonthOfYear is storing
        // it form 0 - 11 so need to adjust the value
        schedule.month= static_cast<SchedulingAgentCorbaDef::MonthOfYear>(m_month - MONTH_OFFSET); 
        schedule.year = m_year;

        if (m_daysOfWeek == NO_WEEKDAYS)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No days of week");
            schedule.daysOfWeek.length(0);
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Days of week are");
            int num = 0;
    
            if ((m_daysOfWeek & SchedulingManager::Monday) == SchedulingManager::Monday)
            {
                schedule.daysOfWeek.length(num+1);
                schedule.daysOfWeek[num] = SchedulingAgentCorbaDef::Monday;
                LOGMORE ( SourceInfo, "Monday");
                ++num;
            }
            if ((m_daysOfWeek & SchedulingManager::Tuesday) == SchedulingManager::Tuesday)
            {
                schedule.daysOfWeek.length(num+1);
                schedule.daysOfWeek[num] = SchedulingAgentCorbaDef::Tuesday;
                LOGMORE ( SourceInfo, "Tuesday");
                ++num;
            }
            if ((m_daysOfWeek & SchedulingManager::Wednesday) == SchedulingManager::Wednesday)
            {
                schedule.daysOfWeek.length(num+1);
                schedule.daysOfWeek[num] = SchedulingAgentCorbaDef::Wednesday;
                LOGMORE ( SourceInfo, "Wednesday");
                ++num;
            }
            if ((m_daysOfWeek & SchedulingManager::Thursday) == SchedulingManager::Thursday)
            {
                schedule.daysOfWeek.length(num+1);
                schedule.daysOfWeek[num] = SchedulingAgentCorbaDef::Thursday;
                LOGMORE ( SourceInfo, "Thursday");
                ++num;
            }
            if ((m_daysOfWeek & SchedulingManager::Friday) == SchedulingManager::Friday)
            {
                schedule.daysOfWeek.length(num+1);
                schedule.daysOfWeek[num] = SchedulingAgentCorbaDef::Friday;
                LOGMORE ( SourceInfo, "Friday");
                ++num;
            }
            if ((m_daysOfWeek & SchedulingManager::Saturday) == SchedulingManager::Saturday)
            {
                schedule.daysOfWeek.length(num+1);
                schedule.daysOfWeek[num] = SchedulingAgentCorbaDef::Saturday;
                LOGMORE ( SourceInfo, "Saturday");
                ++num;
            }
            if ((m_daysOfWeek & SchedulingManager::Sunday) == SchedulingManager::Sunday)
            {
                schedule.daysOfWeek.length(num+1);
                schedule.daysOfWeek[num] = SchedulingAgentCorbaDef::Sunday;
                LOGMORE ( SourceInfo, "Sunday");
                ++num;
            }
        }

        schedule.endDay = m_endDay;
        schedule.endMonth = static_cast<SchedulingAgentCorbaDef::MonthOfYear>(m_endMonth - MONTH_OFFSET);
        schedule.endYear = m_endYear;
        FUNCTION_EXIT;
        return schedule;
    }


    COleDateTime Schedule::getValidDate(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute) const
    {
        COleDateTime date(year, month, day, hour, minute, 0);
        if (date.GetStatus() == COleDateTime::valid)
        {
            return date;
        }

        // we're going to try to see which day is the last day of month, starting
        // from 31, then 30, then 29, then 28
        for (int i = 31; i >= 28; i--)
        {
            if (date.SetDateTime(year, month, i, hour, minute, 0) == 0)
            {
                return date;
            }
        }

        TA_ASSERT(false, "Can't determine date");
        return date;
    }


    COleDateTime* Schedule::getEndDate() const
    {
        if (hasEndDate())
        {
            return new COleDateTime(m_endYear, m_endMonth, m_endDay, m_hour, m_minute, 0);
        }
        return NULL;
    }

 
    //////////////////////////////////////////////////////////////////////////////////
    //
    //
    // Mutator methods
    //
    //
    //////////////////////////////////////////////////////////////////////////////////
 
    void Schedule::setOnceOff(const COleDateTime& theTime, const COleDateTime& theDate)
    {
        FUNCTION_ENTRY("setOnceOff");

        setTime(theTime, false);

        // we'll set the end date to NULL so that the agent doesn't throw bad schedule exception
        setEndDate(NULL);

        m_type = SchedulingAgentCorbaDef::OnceOff;
        m_dayOfMonth = theDate.GetDay();
        m_month = theDate.GetMonth(); 
        m_year = theDate.GetYear();
        m_daysOfWeek = NO_WEEKDAYS;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job runs once off on %d-%d-%d at %d:%d", 
            m_dayOfMonth, m_month, m_year, m_hour, m_minute);
        FUNCTION_EXIT;
    }

 
 
    void Schedule::setDaily(const COleDateTime& theTime)
    {
        FUNCTION_ENTRY("setDaily");

        setTime(theTime);

        m_type = SchedulingAgentCorbaDef::Daily;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job runs daily at %d:%d", 
            m_dayOfMonth, m_month, m_year, m_hour, m_minute);

        FUNCTION_EXIT;
    }

 
 
    void Schedule::setWeekly(const COleDateTime& theTime, const unsigned short theDay)
    {
        FUNCTION_ENTRY("setWeekly");

        setTime(theTime);

        m_type = SchedulingAgentCorbaDef::Weekly;
        m_daysOfWeek = theDay;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job runs weekly (day flag = %d) at %d:%d", 
            m_daysOfWeek, m_hour, m_minute);
        FUNCTION_EXIT;
    }
 
 
    void Schedule::setMonthly(const COleDateTime& theTime, const unsigned short theDay)
    {
        FUNCTION_ENTRY("setMonthly");

        setTime(theTime);

        m_type = SchedulingAgentCorbaDef::Monthly;
        m_dayOfMonth = theDay;          // set the starting date
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job runs monthly on day %d at %d:%d", 
            m_dayOfMonth, m_hour, m_minute);
        FUNCTION_EXIT;
    }
 
 
    void Schedule::setYearly(const COleDateTime& theTime, const unsigned int theMonth, const unsigned int theDay)
    {
        FUNCTION_ENTRY("setYearly");

        setTime(theTime);

        m_type = SchedulingAgentCorbaDef::Yearly;
        m_dayOfMonth = theDay;
        m_month = theMonth;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job runs yearly on %d-%d at %d:%d", 
            m_dayOfMonth, m_month, m_hour, m_minute);
        FUNCTION_EXIT;
    }


    void Schedule::setEndDate(const COleDateTime& endDate)
    {
        FUNCTION_ENTRY("setEndDate");
        m_endDay = endDate.GetDay();
        m_endMonth = endDate.GetMonth(); 
        m_endYear = endDate.GetYear();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job will end on %d-%d-%d", m_endDay, m_endMonth + MONTH_OFFSET, m_endYear);
        FUNCTION_EXIT;
    }


    void Schedule::setEndDate(const int numRepeats)
    {
        FUNCTION_ENTRY("setEndDate");
        if (numRepeats == 0)
        {
            m_endDay = TA_Base_App::SchedulingManager::RECUR_INDEFINITELY;
            m_endMonth = MONTH_OFFSET;
            m_endYear = 0;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job is to recur indefinitely");
            return;
        }

        COleDateTime endDate = getValidDate(m_year, m_month, m_dayOfMonth, m_hour, m_minute);
        // if the job hasn't started, then it's start date will be the first occurrence
        int val = (endDate > COleDateTime::GetCurrentTime()) ? numRepeats - 1 : numRepeats;
        switch (m_type)
        {
			case SchedulingAgentCorbaDef::Daily:
				if(val > 0)
				{
					endDate += COleDateTimeSpan(val, 0, 0, 0);
				}
				break;
			case SchedulingAgentCorbaDef::Weekly:
				{
					// TD #12040
					// daySpan indicates the number of days between the day of the job creation
					// and the LAST occurrence in the week for this job.
					//    i.e. if today is Monday, and the job is set to occur weekly on
					//         Tuesdays and Thursdays, then daySpan will be 3
					//         (days between Monday and Thursday)
					int daySpan = 0;
					int tempDaySpan = 0;

					// has Monday been ticked?
					if ((m_daysOfWeek & TA_Base_App::SchedulingManager::Monday) != 0)
					{
						// get the number of days between Monday and the job creation day
						// Monday = 2, as returned by COleDateTime::GetDayOfWeek()
						tempDaySpan = 2 - endDate.GetDayOfWeek();
						// if the result is negative, just add 7.
						// i.e. if today is Sunday (1), tempDaySpan = 2 - 1 = 1
						//      this is correct, since there is only 1 day between Sunday and Monday
						// 
						//      if today is Wednesday (4), tempDaySpan = 2 - 4 = -2
						//      add 7 to adjust, tempDaySpan = -2 + 7 = 5.
						//      this is now correct, since there are 5 days between Wednesday and the following Monday
						tempDaySpan += (tempDaySpan < 0) ? 7 : 0;

						// if tempDaySpan is 0, that means the first occurrence could either happen
						// today, or in a week, depending on if the job time has already passed.
						// i.e. if today is Monday, 13:00 and the first occurrence of the job
						//      is set for Monday, 12:00, the job cannot occur today, so tempDaySpan
						//      will be set to 7 for a week's time.
						if(tempDaySpan == 0)
						{
							tempDaySpan = (endDate > COleDateTime::GetCurrentTime()) ? 0 : 7;
						}

						// update daySpan if this is the last occurrence so far
						daySpan = (tempDaySpan > daySpan) ? tempDaySpan : daySpan;
					}
					// repeat for the other days..
					if ((m_daysOfWeek & TA_Base_App::SchedulingManager::Tuesday) != 0)
					{
						// Tuesday = 3, as returned by COleDateTime::GetDayOfWeek()
						tempDaySpan = 3 - endDate.GetDayOfWeek();
						tempDaySpan += (tempDaySpan < 0) ? 7 : 0;

						if(tempDaySpan == 0)
						{
							tempDaySpan = (endDate > COleDateTime::GetCurrentTime()) ? 0 : 7;
						}

						daySpan = (tempDaySpan > daySpan) ? tempDaySpan : daySpan;
					}
					if ((m_daysOfWeek & TA_Base_App::SchedulingManager::Wednesday) != 0)
					{
						// Wednesday = 4, as returned by COleDateTime::GetDayOfWeek()
						tempDaySpan = 4 - endDate.GetDayOfWeek();
						tempDaySpan += (tempDaySpan < 0) ? 7 : 0;

						if(tempDaySpan == 0)
						{
							tempDaySpan = (endDate > COleDateTime::GetCurrentTime()) ? 0 : 7;
						}

						daySpan = (tempDaySpan > daySpan) ? tempDaySpan : daySpan;
					}
					if ((m_daysOfWeek & TA_Base_App::SchedulingManager::Thursday) != 0)
					{
						// Thursday = 5, as returned by COleDateTime::GetDayOfWeek()
						tempDaySpan = 5 - endDate.GetDayOfWeek();
						tempDaySpan += (tempDaySpan < 0) ? 7 : 0;

						if(tempDaySpan == 0)
						{
							tempDaySpan = (endDate > COleDateTime::GetCurrentTime()) ? 0 : 7;
						}

						daySpan = (tempDaySpan > daySpan) ? tempDaySpan : daySpan;
					}
					if ((m_daysOfWeek & TA_Base_App::SchedulingManager::Friday) != 0)
					{
						// Friday = 6, as returned by COleDateTime::GetDayOfWeek()
						tempDaySpan = 6 - endDate.GetDayOfWeek();
						tempDaySpan += (tempDaySpan < 0) ? 7 : 0;

						if(tempDaySpan == 0)
						{
							tempDaySpan = (endDate > COleDateTime::GetCurrentTime()) ? 0 : 7;
						}

						daySpan = (tempDaySpan > daySpan) ? tempDaySpan : daySpan;
					}
					if ((m_daysOfWeek & TA_Base_App::SchedulingManager::Saturday) != 0)
					{
						// Saturday = 7, as returned by COleDateTime::GetDayOfWeek()
						tempDaySpan = 7 - endDate.GetDayOfWeek();
						tempDaySpan += (tempDaySpan < 0) ? 7 : 0;

						if(tempDaySpan == 0)
						{
							tempDaySpan = (endDate > COleDateTime::GetCurrentTime()) ? 0 : 7;
						}

						daySpan = (tempDaySpan > daySpan) ? tempDaySpan : daySpan;
					}
					if ((m_daysOfWeek & TA_Base_App::SchedulingManager::Sunday) != 0)
					{
						// Sunday = 1, as returned by COleDateTime::GetDayOfWeek()
						tempDaySpan = 1 - endDate.GetDayOfWeek();
						tempDaySpan += (tempDaySpan < 0) ? 7 : 0;

						if(tempDaySpan == 0)
						{
							tempDaySpan = (endDate > COleDateTime::GetCurrentTime()) ? 0 : 7;
						}

						daySpan = (tempDaySpan > daySpan) ? tempDaySpan : daySpan;
					}

					if(daySpan == 0)
					{
						daySpan = (endDate > COleDateTime::GetCurrentTime()) ? 0 : 7;
					}

					endDate += COleDateTimeSpan(7 * (numRepeats - 1) + daySpan, 0, 0, 0);
				}
				
				// 7 days in a week
				//endDate += COleDateTimeSpan(7 * val, 0, 0, 0);
				break;
			case SchedulingAgentCorbaDef::Monthly:
				if(val > 0)
				{
					int newMonth = (val + endDate.GetMonth()) % 12;
					int yearElapsed = (val + endDate.GetMonth()) / 12;

					// if it falls on December, we don't count it as 1 year
					if (newMonth == 0)
					{
						newMonth = 12;
						yearElapsed--;
					}

					endDate = getValidDate(endDate.GetYear() + yearElapsed, newMonth, m_dayOfMonth, m_hour, m_minute);
				}
				break;
			case SchedulingAgentCorbaDef::Yearly:
				if(val > 0)
				{
					endDate = getValidDate(endDate.GetYear() + val, m_month, m_dayOfMonth, m_hour, m_minute);
				}
				break;
			case SchedulingAgentCorbaDef::OnceOff:
			default:
				TA_ASSERT(false, "Only have end date when it's daily, weekly, monthly, or yearly");
        }
        setEndDate(endDate);
        FUNCTION_EXIT;
    }


    void Schedule::setTime(const COleDateTime& theTime, bool startToday /*=true*/)
    {
        FUNCTION_ENTRY("setTime");
        m_minute = theTime.GetMinute();
        m_hour = theTime.GetHour();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Job runs at %d:%d", m_hour, m_minute);

        // start date is today's date
        if (startToday)
        {
            COleDateTime today = COleDateTime::GetCurrentTime();
            m_dayOfMonth = today.GetDay();
            m_month = today.GetMonth(); 
            m_year = today.GetYear();
        }
        m_daysOfWeek = NO_WEEKDAYS;
        FUNCTION_EXIT;
    }
};
