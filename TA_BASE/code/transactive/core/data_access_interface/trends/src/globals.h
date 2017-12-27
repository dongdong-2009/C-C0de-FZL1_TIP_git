#if !defined(TREND_DAI_GLOBALS_H)
#define TREND_DAI_GLOBALS_H

/**
  * globals.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/globals.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Global variables for Trend DAI.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace 
{

    //
    // Time Constants
    //
    // WP 4619M50121008 TD 9435
    // Use integral values rather than doubles
    const unsigned int HOURS_IN_DAY = 24;
    const unsigned int MINUTES_IN_DAY = HOURS_IN_DAY * 60;
    const unsigned int SECONDS_IN_DAY = MINUTES_IN_DAY * 60;
    const unsigned int SECONDS_IN_MINUTE = 60;
    const unsigned int SECONDS_IN_HOUR = SECONDS_IN_MINUTE * 60;

    //
    // Graph Types
    //
    const char* LINE = "LINE";
    const char* BAR = "BAR";
    const char* COMPARISON = "COMPARISON";
    const char* XY = "XY";

    const int NUM_GRAPH_TYPES = 4;
    const std::string GRAPH_TYPES[] =
    {
        LINE,
        BAR,
        COMPARISON,
        XY
    };

    //
    // Miscellaneous
    //
    const char* EMPTY_STRING = "<Empty>";
    const char* LIVE_FIELD_UPDATES_NAME = "0";

}

#endif // TREND_DAI_GLOBALS_H



