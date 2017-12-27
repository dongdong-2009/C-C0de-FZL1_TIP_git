/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingTimer.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is used by the Scheduling classes for various timing functions.
  *
  */


#ifndef SCHEDULING_TIMER_H
#define SCHEDULING_TIMER_H

#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"

namespace TA_Base_App
{

class SchedulingTimer
{

public:
    
    

    /**
     *  Indicates if the current time is after the time specified.
     *
     *  @param time   The date/time to compre with now
     */
    bool SchedulingTimer::isPast(boost::posix_time::ptime time);


    /**
     *  Gets the number of minutes since the epoch at which the given
     *  time will occur. If the time has passed today, gets the next
     *  one.
     *
     *  @param minute       the minute of the hour (0 - 59).
     *  @param hour         the hour of the day (0 - 23).
     */
    unsigned long getNextTimeInMinutes(boost::posix_time::ptime nextTime);


 
    /**
     *  Constructor.
     */
    SchedulingTimer();

    /**
     *  Destructor.
     */
    virtual ~SchedulingTimer();

    /**
     *  Gets the number of minutes since the epoch at which the given
     *  occurs.
     *
     *  @param minute            the minute of the hour (0 - 59).
     *  @param hour              the hour of the day (0 - 23).
     *  @param dayOfMonth        the day of the month (1 - 31).
     *  @param monthsSinceEpoch  the number of months since the epoch.
     */
    unsigned long getTimeInMinutesSinceEpoch();

    /**
     *  Given the number of minutes from the epoch,
     *  this will function will return a nicely formated string
     *  that mere mortals can understand.
     *
     *  @param minutesSinceEpoch the number of minutes since the epoch
     *
     */
    static std::string SchedulingTimer::getFormatedTime( unsigned long minutesSinceEpoch );

private:


    SchedulingTimer(const SchedulingTimer&);
    const SchedulingTimer& operator=(const SchedulingTimer&);

    boost::posix_time::ptime* m_now;

};


};// TA_Base_App


#endif // SCHEDULING_TIMER_H
