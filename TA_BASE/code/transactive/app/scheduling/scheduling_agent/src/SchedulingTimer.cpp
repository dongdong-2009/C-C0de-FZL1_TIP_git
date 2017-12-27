/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingTimer.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/11/01 15:29:57 $
  * Last modified by:  $Author: huangjian $
  *
  * This class is used by the Scheduling classes for various timing functions.
  * The epoch is the date which most operating system timing functions use as
  * absolute 0. It is 00:00 January 1st, 1970 GMT.
  *
  */


// Disable warnings from boost
#if defined(_MSC_VER)
  #pragma warning(disable:4715) // not all control paths return a value
  #pragma warning(disable:4244) // conversion - possible loss of data
#endif // _MSC_VER


#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"


#include <sys/timeb.h>
#include <sys/types.h>


#include "core/utilities/src/TAAssert.h"


using namespace boost::gregorian;
using namespace boost::posix_time;

namespace TA_Base_App
{

// NOTE: epoch can be anything.
// As long as it doesn't change after it goes into production
#define EPOCH_DAY     1
#define EPOCH_MONTH   Jan
#define EPOCH_YEAR    1970
#define EPOCH_HOUR    0
#define EPOCH_MINUTE  0
#define EPOCH_SECOND  0

SchedulingTimer::SchedulingTimer()
{
    m_now = new ptime(second_clock::local_time());
    TA_ASSERT(m_now != NULL, "m_now should never be NULL");
}

SchedulingTimer::~SchedulingTimer() {}



bool SchedulingTimer::isPast(ptime nextTime)
{
    if ( *m_now >= nextTime )
    {
        return true;
    }
    return false;
}

unsigned long SchedulingTimer::getNextTimeInMinutes(ptime nextTime)
{
    if ( *m_now >= nextTime )
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Time can not be in the past");
		return 0u;
    }
    TA_ASSERT( nextTime.date().is_not_a_date() == false, "Time is invalid");

    unsigned long nextTimeInMinutes = 0;
    time_duration td = nextTime - *m_now;

    nextTimeInMinutes += td.hours() * 60 + td.minutes();
    if ( td.seconds() != 0) // correct for fractional minutes
    {
        ++nextTimeInMinutes;
    }

    nextTimeInMinutes += getTimeInMinutesSinceEpoch();
    return nextTimeInMinutes;
}

unsigned long SchedulingTimer::getTimeInMinutesSinceEpoch()
{
    ptime epoch( date( EPOCH_YEAR, EPOCH_MONTH, EPOCH_DAY ),
                 hours( EPOCH_HOUR ) + minutes( EPOCH_MINUTE ) + seconds( EPOCH_SECOND ) );
    time_period tp( epoch, *m_now );
    time_duration duration = tp.length();
    return (duration.total_seconds() / 60);
}

std::string SchedulingTimer::getFormatedTime( const unsigned long minutesSinceEpoch )
{
    unsigned long hoursSinceEpoch = minutesSinceEpoch / 60;
    unsigned long remainingMinutes = minutesSinceEpoch % 60;

    time_duration timeSinceEpoch( hours( hoursSinceEpoch ) + minutes( remainingMinutes ) );

    ptime epoch( date( EPOCH_YEAR, EPOCH_MONTH, EPOCH_DAY ),
                 hours( EPOCH_HOUR ) + minutes( EPOCH_MINUTE ) + seconds( EPOCH_SECOND ) );

    return to_simple_string( epoch + timeSinceEpoch );


}

};// TA_Base_App

