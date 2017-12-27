#if !defined(Utilities_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_)
#define Utilities_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_

/**
  * Utilities.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/Utilities.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Contains useful functions used by other classes.
  *
  */

#include <ctime>
#include <string>
#include <map>

#include "app/trending/trend_viewer/src/stdafx.h"

namespace TA_Base_App
{
    //
    // Utilities
    //
    class Utilities
    {

    public:

        /**
		  * getRangeSeconds
		  * 
		  * Get the combined day and time attributes of the time range.
          *
          * @return total range in seconds
		  */
        static long getRangeSeconds(int rangeDays, long rangeTime);

        /**
		  * getHourMinuteComponents
		  * 
		  * Get the hours and minutes components of a given time range
          *
          * @param rangeTime - given time range from which to determine hour and minute components
          * @param hours - in/out paramter - hour component of time range
          * @param minutes - in/out parameter - minute component of time range
          */
        static void getHourMinuteComponents(time_t rangeTime, int& hours, int& minutes);

        /**
		  * getDayTimeComponents
		  * 
		  * Get the days and time components of a give time range
          *
          * @param rangeSeconds - give time range from which to determine days and time components
          * @param days - in/out parameter - day component of time range
          * @param time - in/out parameter - time component of time range
		  */
        static void getDayTimeComponents(time_t rangeSeconds, int& days, time_t& time);

        /**
		  * getRangeTime
		  * 
		  * Get the time range in time_t format given the hour and minute components
          *
          * @param hours - hour component
          * @param minutes - minute component
          *
          * @return time range in time_t format
		  */
        static time_t getRangeTime(int hours, int minutes);

        /**
		  * convertOffsetStringToSeconds
		  * 
		  * Convert the offset string displayed in the properties dialog into a numeric value.
          *
          * @param offsetString - String displayed in the properties dialog
          * @param startTime - Trend template's start time
          *
          * @return numeric value in seconds
		  */
        static long convertOffsetStringToSeconds(const std::string& offsetString, time_t startTime);

        /**
		  * convertHexStringToColorRef
		  * 
		  * Convert the hex string into a COLORREF representation.
          *
          * @param hexString - Hex string to be converted.
          *
          * @return COLORREF representation.
		  */
        static COLORREF convertHexStringToColorRef(const std::string& hexString);

        /**
		  * createHexStringFromColourRef
		  * 
		  * Convert the COLORREF into a hex string.
          *
          * @param colour - COLORREF to be converted
          *
          * @return hex string.
		  */
        static std::string createHexStringFromColourRef(const COLORREF& colour);

        /**
		  * getRangeSeconds
		  * 
		  * Retrieve a range in time_t (seconds) given a start and end time.
          *
          * @param minTime - start time
          * @param maxTime - end time
          *
          * @return range in time_t format
		  */
        static time_t getRangeSeconds(const COleDateTime& minTime, const COleDateTime& maxTime);

        /**
		  * convertValueWithPrecision
		  * 
		  * Given a value, truncate it to the number of decimal places specified by the precision
          * parameter and return the truncated value as a string.
          *
          * @param value - value to truncate
          * @param precision - number of decimal places the value is to be truncated to
          *
          * @return truncated value as a string
		  */
        static std::string convertValueWithPrecision(double value, int precision);

		//TD17718
        /**
		  * setMaxPointsForOtherGraphs
		  * 
		  * sets the maximum points for other graphs as configured in CE          
          *
          * @param newMaxPoints - configured max points in CE
          *
          * @return void
		  */
        static void setMaxPointsForOtherGraphs(int newMaxPoints);		

		//TD17718
        /**
		  * setMaxPointsForBarGraph
		  * 
		  * sets the maximum points for bar graphs as configured in CE          
          *
          * @param newMaxPointsForBar - configured max points for bar graph in CE
          *
          * @return void
		  */
		static void setMaxPointsForBarGraph(int newMaxPointsForBar);
		
		//TD17718
        /**
		  * getMaxPointsToDisplay
		  * 
		  * sets the maximum points for bar graphs as configured in CE          
          *
          * @param isBarGraph - must be set to true if the graph type is a Bar graph 
		  *						since it's the only graph type that has different Max Points
          *
          * @return the configured max points in CE depending on the graph type
		 */
		static int getMaxPointsToDisplay(bool isBarGraph = false);

		//TD17529
        /**
		  * setGraphRefreshInterval
		  * 
		  * sets the graph Refresh Interval (in seconds) as configured in CE          
          *
          * @param graphRefreshInterval - configured graph Refresh Interval (in seconds) in CE
          *
          * @return void
		  */
		static void setGraphRefreshInterval(int graphRefreshInterval);
		
		//TD17529
        /**
		  * getGraphRefreshInterval
		  * 
		  * retrieves the graph Refresh Interval (in seconds) as configured in CE           
          *
          * @return the configured graph Refresh Interval in CE
		 */
		static int getGraphRefreshInterval();

    private:

        Utilities();
        Utilities( const Utilities& theUtilities);
	    Utilities& operator=(const Utilities &);

		//TD17718
		static int m_maxPointsForOtherGraphs;

		static int m_maxPointsForBarGraphs;

		//TD17529
		static int m_graphRefreshInterval;
    };

}

#endif // !defined(Utilities_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_)
