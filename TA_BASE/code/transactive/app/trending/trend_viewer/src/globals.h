#if !defined(HISTORY_VIEWER_GLOBALS_H)
#define HISTORY_VIEWER_GLOBALS_H

/**
  * globals.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/globals.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Global variables for Trend Viewer.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/trends/src/Globals.h"
#include "bus/scada/common_library/src/CommonDefs.h"

namespace
{

//
// Entity Parameters
//
static const char* RPARAM_ENTITY_KEY = "EntityKey";

//
// Data Point Status Values
//
const int COMMS_ERROR_UNKNOWN = -1;
unsigned long STATUS_UNKNOWN = 0;
const int NUM_GOOD_QUALITY_WORDS = 2;
//TD17884 - not necessary as trend viewer will only listen to 
//           value updates with QUALITY_GOOD_NO_SPECIFIC_REASON quality
//unsigned long DP_GOOD_QUALITY_WORDS[NUM_GOOD_QUALITY_WORDS] = 
//{
//    TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON,
//    TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE
//};

//
// Limits
//
const int MAX_DATAPOINT_NAME_LENGTH = 80;
const int TREND_VIEWER_RANGE_YEAR = 10;

const int MAX_POINTS = 1000;
const int MAX_POINTS_BAR = 100;
const int MAX_DATABASE_STORAGE_DAYS = 100;
const int MAX_DECIMAL_PLACES = 2;

const int MAX_OFFSET_YEARS = 5;

const int MAX_ITEMS_DEFAULT = 8;
const int MAX_ITEMS_XY = 2;

const int MAX_PEN_STYLES = 6;

const int LINE_MIN_ITEMS = 0;
const int LINE_MAX_ITEMS = MAX_ITEMS_DEFAULT;
const int BAR_MIN_ITEMS = 0;
const int BAR_MAX_ITEMS = MAX_ITEMS_DEFAULT;
const int COMPARISON_MIN_ITEMS = 0;
const int COMPARISON_MAX_ITEMS = MAX_ITEMS_DEFAULT;
const int XY_MIN_ITEMS = 2;
const int XY_MAX_ITEMS = 2;

// 
// GRAPH DEFS
//
const int LINE_GRAPH = 0;
const int LINE_PEN_WIDTH = 2;
const int LINE_POINT_STYLE = 1;
const int LINE_POINT_HORIZONTAL_SIZE = 3;
const int LINE_POINT_VERTICAL_SIZE = 3;

const int BAR_GRAPH = 1;
const int BAR_RECTANGLE_STYLE = 0;
const int BAR_WIDTH_PERCENT = 5;

const int ORIGINAL_PEN_STYLE = 0;
const int ORIGINAL_BRUSH_STYLE = 0;
const int NUMBER_AVAILABLE_BRUSHES = 8;

const long AXIS_ARROW_TOOL_INDEX = 1;
const long AXIS_ARROW_LENGTH = 16;

const long NO_CUSTOM_AXIS_DISPLAYED = -1;

const char* PERCENTAGE_TITLE = "Percentage";
const char* TIME_TITLE = "Time";


const double COMPARISON_TOP_GRAPH_END_POSITION_PERCENTAGE = 46.0;
const double COMPARISON_BOTTOM_GRAPH_START_POSITION_PERCENTAGE = 52.0;
const double COMPARISON_TOP_GRAPH_HOR_POSITION_PERCENTAGE = 54.0;

//
// AXIS DEFS
//
const double TICKS_5_OR_10_SECONDS = 10.0 / SECONDS_IN_DAY + 1e-05;
const double TICKS_15_OR_30_SECONDS = 30.0 / SECONDS_IN_DAY + 1e-05;
const double TICKS_1_MINUTE = 1.0 / MINUTES_IN_DAY + 1e-05;
const double TICKS_5_OR_10_MINUTES = 10.0 / MINUTES_IN_DAY + 1e-05;
const double TICKS_15_OR_30_MINUTES = 30.0 / MINUTES_IN_DAY + 1e-05;
const double TICKS_1_HOUR = 1.0 / HOURS_IN_DAY + 1e-05;
const double TICKS_6_OR_12_HOUR = 12.0 / HOURS_IN_DAY + 1e-05;

const int MINOR_TICK_COUNT_2 = 2;
const int MINOR_TICK_COUNT_4 = 4;
const int MINOR_TICK_COUNT_3 = 3;
const int MINOR_TICK_COUNT_5 = 5;

const double TIME_AXIS_15_MINUTES = 15.0 / MINUTES_IN_DAY;
const double TIME_AXIS_1_DAY = 1.0;
const double TIME_AXIS_4_DAYS = 4.0; //TD17174
const double TIME_AXIS_7_DAYS = 7.0;
const double TIME_AXIS_1_YEAR = 365.0;

const double PERCENTAGE_AXIS_MIN = 0.0;
const double PERCENTAGE_AXIS_MAX = 100.0;

const int DEFAULT_AXIS_POSITION_PERCENT = 0;
const int DEFAULT_VERTICAL_AXIS_ANGLE = 90;
const int DEFAULT_AXIS_START_POSITION = 0;
const int DEFAULT_AXIS_END_POSITION = 100;

const int AXIS_LABEL_FONT_SIZE = 8; //TD17605 - decrease font size to display longer labels

//
// PRINTER DEFS
//
const int DEFAULT_PRINTER_TOP_MARGIN = 5;
const int DEFAULT_PRINTER_BOTTOM_MARGIN = 5;
const int DEFAULT_PRINTER_LEFT_MARGIN = 5;
const int DEFAULT_PRINTER_RIGHT_MARGIN = 5;
//
// Available sample rates
//
const int NUM_SAMPLE_RATES = 21;
const int SAMPLE_RATES[] = 
{ 
    60,     // 1 min
    120,    // 2 min
    180,    // 3 min
    240,    // 4 min
    300,    // 5 min
    360,    // 6 min
    600,    // 10 min
    720,    // 12 min
    900,    // 15 min
    1200,   // 20 min
    1800,   // 30 min
    2400,   // 40 min
    2700,   // 45 min
    3600,   // 1 hr
    7200,   // 2 hr
    10800,  // 3 hr
    14400,  // 4 hr
    21600,  // 6 hr
    28800,  // 8 hr
    43200,  // 12 hr
    86400   // 1 day
};

//
// Graph Types
//

const char* SHOW_OFFSET_GRAPH_TYPE = "COMPARISON";

//
// Offset Units (for Comparison Graph)
//

const char* OFFSET_MINUTES = "MINUTES";
const char* OFFSET_HOURS = "HOURS";
const char* OFFSET_DAYS = "DAYS";
const char* OFFSET_WEEKS = "WEEKS";
const char* OFFSET_MONTHS = "MONTHS";
const char* OFFSET_YEARS = "YEARS";

const int NUM_OFFSET_UNITS = 6;
const std::string OFFSET_UNITS[] =
{
    OFFSET_MINUTES,
    OFFSET_HOURS,
    OFFSET_DAYS,
    OFFSET_WEEKS,
    OFFSET_MONTHS,
    OFFSET_YEARS
};

//
// Offset Unit Seconds - There are no values for MONTHS and YEARS as the value
// will depend on the current date. These values will have to be manually calculated.
//
long OFFSET_UNIT_VALUES[] = 
{
    60,
    3600,
    86400,
    604800
};

const int NUM_NO_VALUE_UNITS = 2;
const std::string NO_VALUE_UNITS[] =
{
    OFFSET_MONTHS,
    OFFSET_YEARS
};

const int DEFAULT_OFFSET_VALUE = 1;
const char* DEFAULT_OFFSET_UNIT = OFFSET_HOURS;

//
// Time/Date Format Strings
//
//const char* DATE_TIME_FORMAT = "%d/%m/%Y %H:%M:%S"; //TD12474 
const char* TIME_ONLY_WITH_SECONDS = "hh:nn:ss";
const char* TIME_ONLY_WITHOUT_SECONDS = "hh:nn";
const char* DATE_TIME_WITH_SECONDS = "dd-mmm-yy hh:nn:ss"; //TD17174
//const char* DATE_AND_TIME = "dd-mmm-yy hh:nn"; //TD12474 
//const char* DATE_ONLY_WITH_DAYS = "dd-mmm-yy"; //TD12474 
//const char* DATE_ONLY_WITHOUT_DAYS = "mmm-yy"; //TD12474 
//const char* DATE_TIME_FORMAT_WITHOUT_SECONDS = "%d-%b-%y %H:%M"; //TD12474 

//
// Misc
//
const double NO_VALUE = 99999.99999;
const char* DP_BOOLEAN_TYPE = "Boolean";
const char* DP_DERIVED_TYPE = "Derived";
const char* DP_TEXT_TYPE = "Text";
const char* DP_ENUM_TYPE = "Enum";
//const char* DEFAULT_SAMPLE_PERIOD_NAME = "1MIN";
const char* LIVE_UPDATES_NAME = "REAL";
const long DEFAULT_SAMPLE_PERIOD = 60;
const char* DEFAULT_SAMPLE_TYPE = "IAVG";
const int CUSTOM_AXIS_CHART_OFFSET = 8;
const long SCROLL_HORIZONTAL = 1;
const long SCROLL_BOTH = 3;
const int SCROLL_PERCENTAGE = -25;
const long LIVE_FIELD_UPDATES = 0;
const int MAX_OFFSET_DAY_LENGTH = 5;
const int MAX_DISPLAY_NAME_LENGTH = 20;
const char* REG_SUBKEY_STRING = "TeeChart.TChart.7";
const char* COTS_COMPONENT_NAME = "TeeChart AX 7";
const int GRAPH_BOUNDARY_INCREMENT = 300; //TD17883 
}

#endif // HISTORY_VIEWER_GLOBALS_H



