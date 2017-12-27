/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/Timer.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#include "core/utilities/src/Timer.h"

namespace TA_Base_Core
{
    //
    // Constructor
    //
    Timer::Timer()
    {
        ftime(&m_startTime);
        ftime(&m_endTime);
    }


    //
    // Destructor
    //
    Timer::~Timer()
    {}


    //
    // restart
    //
    void Timer::restart()
    {
        ftime(&m_startTime);
    }


    //
    // elapsedTime
    //
    double Timer::elapsedTime()
    {
        ftime(&m_endTime);

        double endTimeInSeconds = ( ((double)m_endTime.time * 1000.0) + (double)m_endTime.millitm ) / 1000.0;
        double startTimeInSeconds = ( ((double)m_startTime.time * 1000.0) + (double)m_startTime.millitm ) / 1000.0;
       
        return(endTimeInSeconds - startTimeInSeconds);
    }            
};
