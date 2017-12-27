/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Represents a generic time span parameter (e.g. as used by TIS and PA steps for message timeSpan).
  * The supported time resolutions are: days, hours, minutes, seconds.
  *
  **/

#include "StdAfx.h"

#if defined(_MSC_VER)
    #pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "TimeSpanStepParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TimeSpanStepParameter

TimeSpanStepParameter::TimeSpanStepParameter(PlanStep& parent, std::string name, unsigned long number, ETimeResolution resolution, bool isHidden) :
    NumericStepParameter(parent, name, number, isHidden),
    m_resolution(resolution)
{
    FUNCTION_ENTRY( "TimeSpanStepParameter" );
    FUNCTION_EXIT;
}


TimeSpanStepParameter::TimeSpanStepParameter(PlanStep& parent, const TimeSpanStepParameter& parameter) :
    NumericStepParameter(parent, parameter),
    m_resolution(parameter.m_resolution)
{
    FUNCTION_ENTRY( "TimeSpanStepParameter" );
    FUNCTION_EXIT;
}


TimeSpanStepParameter::TimeSpanStepParameter(const TimeSpanStepParameter& parameter) :
    NumericStepParameter(parameter),
    m_resolution(parameter.m_resolution)
{
    FUNCTION_ENTRY( "TimeSpanStepParameter" );
    FUNCTION_EXIT;
}


TimeSpanStepParameter::~TimeSpanStepParameter()
{
    FUNCTION_ENTRY( "~TimeSpanStepParameter" );
    FUNCTION_EXIT;
}


void TimeSpanStepParameter::addToParameterList(CPlanStepParameterListCtrl& listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    std::ostringstream timeSpanStr;

    if (m_resolution != TR_UNDEFINED)
    {
        // Convert to text based on the resolution.
        // 
        // The general format is: "D days H hours M minutes S seconds"
        // 
        // If the value of any given unit is 0 then that unit is omitted.

        int timeSpanInUnits = static_cast<int>(getNumericValue());
        int days(m_resolution == TR_DAYS? timeSpanInUnits : 0);
        int hours(m_resolution == TR_HOURS? timeSpanInUnits : 0);
        int minutes(m_resolution == TR_MINUTES? timeSpanInUnits : 0);
        int seconds(m_resolution == TR_SECONDS? timeSpanInUnits : 0);

        COleDateTimeSpan timeSpan(days, hours, minutes, seconds);

        days = timeSpan.GetDays();
        hours = timeSpan.GetHours();
        minutes = timeSpan.GetMinutes();
        seconds = timeSpan.GetSeconds();

        timeSpanStr.str("");

        if (days > 0 || timeSpanInUnits == 0 && m_resolution == TR_DAYS)
        {
            timeSpanStr << days << " " << (days == 1? "day" : "days") << " ";
        }

        if (hours > 0 || timeSpanInUnits == 0 && m_resolution == TR_HOURS)
        {
            timeSpanStr << hours << " " << (hours == 1? "hour" : "hours") << " ";
        }

        if (minutes > 0 || timeSpanInUnits == 0 && m_resolution == TR_MINUTES)
        {
            timeSpanStr << minutes << " " << (minutes == 1? "minute" : "minutes") << " ";
        }

        if (seconds > 0 || timeSpanInUnits == 0 && m_resolution == TR_SECONDS)
        {
            timeSpanStr << seconds << " " << (seconds == 1? "second" : "seconds");
        }
    }

    listCtrl.AddParameter(getName().c_str(), (isHiddenValue()? "" : timeSpanStr.str().c_str()), this);

    FUNCTION_EXIT;
}


