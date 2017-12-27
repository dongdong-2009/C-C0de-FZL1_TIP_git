#if !defined(UtilitiesCpp_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_)
#define UtilitiesCpp_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_

/**
  * Utilities.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/Utilities.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Contains useful functions used by other classes.
  *
  */

#include <afx.h>

#include "app/trending/trend_viewer/src/Utilities.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"

#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/TrendViewerException.h"

#include "ace/OS.h"

namespace TA_Base_App
{
	//TD17718++
	int Utilities::m_maxPointsForOtherGraphs = MAX_POINTS;
	int Utilities::m_maxPointsForBarGraphs = MAX_POINTS_BAR;
	//++TD17718

	int Utilities::m_graphRefreshInterval = DEFAULT_SAMPLE_PERIOD;

    //
    // getHourMinuteComponents
    //
    void Utilities::getHourMinuteComponents(time_t rangeTime, int& hours, int& minutes)
    {
        hours = 0;
        minutes = 0;
        
        hours = static_cast<int>(rangeTime / SECONDS_IN_HOUR);

        if(hours <= 0)
        {
            hours = 0;
            minutes = static_cast<int>(rangeTime / SECONDS_IN_MINUTE);
        }
        
        if(hours > 0)
        {
            int mod = rangeTime % static_cast <int> (SECONDS_IN_HOUR);

            if(mod == 0)
            {
                minutes = 0;
            }
            else
            {
                int leftOver = rangeTime - (rangeTime / mod);
                minutes = static_cast<int>(leftOver / SECONDS_IN_MINUTE);
            }
        }
    }


    //
    // getDayTimeComponents
    //
    void Utilities::getDayTimeComponents(time_t rangeSeconds, int& days, time_t& time)
    {
        days = 0;
        time = 0;

        days = static_cast<int>(rangeSeconds / SECONDS_IN_DAY);

        if(days <= 0)
        { 
            days = 0;
            time = rangeSeconds;
        }

        if(days > 0)
        {
            int mod = rangeSeconds % static_cast <int> (SECONDS_IN_HOUR);

            if(mod == 0)
            {
                time = 0;
            }
            else
            {
                time = rangeSeconds - (rangeSeconds / mod);
            }
        }

        TA_ASSERT(!(days == 0 && time == 0), "Invalid time range");
    }


    //
    // getRangeTime
    //
    time_t Utilities::getRangeTime(int hours, int minutes)
    {
        return static_cast<time_t>(minutes * SECONDS_IN_MINUTE + hours * SECONDS_IN_HOUR);
    }


    //
    // getRangeSeconds
    //
    long Utilities::getRangeSeconds(int rangeDays, long rangeTime)
    {
        return static_cast<long>(rangeDays * SECONDS_IN_DAY + rangeTime);
    }

    
    //
    // convertOffsetStringToSeconds
    //
    long Utilities::convertOffsetStringToSeconds(const std::string& offsetString, time_t startTime)
    {
        int pos = offsetString.find(" ");
        std::string numberStr = offsetString.substr(0, pos);
        std::string units = offsetString.substr(pos + 1, offsetString.size() - pos);
        int number = atoi(numberStr.c_str());

        int timeValues[] = 
        {
            60,
            3600,
            86400,
            604800
        };

        for(int i = 0; i < NUM_OFFSET_UNITS - 2; ++i)
        {
            if(OFFSET_UNITS[i] == units)
            {
                return number * timeValues[i];
            }
        }

        if(units == OFFSET_MONTHS)
        {
            CTime start(startTime);
            CTime offsetStart(start.GetYear(), start.GetMonth() - number, start.GetDay(),
                start.GetHour(), start.GetMinute(), start.GetSecond());
            return start.GetTime() - offsetStart.GetTime();
        }

        if(units == OFFSET_YEARS)
        {
            CTime start(startTime);
            CTime offsetStart(start.GetYear() - number, start.GetMonth(), start.GetDay(),
                start.GetHour(), start.GetMinute(), start.GetSecond());
            return start.GetTime() - offsetStart.GetTime();
        }

        return 0;
    }


    //
    // convertHexStringToColorRef
    //
    COLORREF Utilities::convertHexStringToColorRef(const std::string& hexString)
    {
        FUNCTION_ENTRY("convertHexStringToColorRef");

        std::string red = "0x" + hexString.substr(0, 2);
        std::string green = "0x" + hexString.substr(2, 2);
        std::string blue = "0x" + hexString.substr(4, 2);

        unsigned long uRed = strtoul(red.c_str(), NULL, 0);
        unsigned long uGreen = strtoul(green.c_str(), NULL, 0);
        unsigned long uBlue = strtoul(blue.c_str(), NULL, 0);
        
        COLORREF colour = RGB(uRed, uGreen, uBlue);

        FUNCTION_EXIT;
        return colour;
    }


    //
    // createHexStringFromColourRef
    //
    std::string Utilities::createHexStringFromColourRef(const COLORREF& colour)
    {
        FUNCTION_ENTRY("createHexStringFromColourRef");

        int rValue = GetRValue(colour);
        int gValue = GetGValue(colour);
        int bValue = GetBValue(colour);

        // Convert to hexidecimal string
        char buffer[100];
        sprintf(buffer, "%02x%02x%02x", rValue, gValue, bValue);

        FUNCTION_EXIT;
        return std::string(buffer);
    }


    time_t Utilities::getRangeSeconds(const COleDateTime& minTime, const COleDateTime& maxTime)
    {
        time_t minValue = TimeFunction::getInstance().ConvertToTimeT(minTime);
        time_t maxValue = TimeFunction::getInstance().ConvertToTimeT(maxTime);

        return maxValue - minValue;
    }


    std::string Utilities::convertValueWithPrecision(double value, int precision)
    {
        if(value == NO_VALUE)
        {
            return "";
        }

		// TD14799 azenitha++
		FUNCTION_ENTRY("convertValueWithPrecision");
        std::ostringstream str;
		str.setf(std::ios_base::fixed);
		str.precision(precision);
        str << value;
        std::string valueAsString(str.str());
        
		/*int pos = std::string::npos;
        if(precision != 0)
        {
            pos = valueAsString.find(".");
            
            if(pos == std::string::npos)
            {
                // There is no decimal point. Add one.
                valueAsString += ".";
                pos = valueAsString.find(".");
            }
            
            if((pos + precision) >= valueAsString.size())
            {
                // Precision required is greater than the number of values after decimal
                // places. Pad with zeros.
                int endPos = valueAsString.size() - 1;
                int numberOfZerosToAdd = precision - (endPos - pos);

                for(int i = 0; i < numberOfZerosToAdd; ++i)
                {
                    valueAsString += "0";
                }

                pos = std::string::npos;
            }
            else
            {
                // Truncate the value based on precision
                pos += precision - 1;
            }
        }
        else
        {
            // If the precision is 0, we just want to remove anything after the decimal point
            pos = valueAsString.find(".");
        }
        
        if(pos != std::string::npos)
        {
            valueAsString = valueAsString.substr(0, pos);
        }*/
		// TD14799 ++azenitha

        return valueAsString;
    }

	//TD17718
	int Utilities::getMaxPointsToDisplay(bool isBarGraph /* = false */)
	{
		return ((true == isBarGraph) ? m_maxPointsForBarGraphs : m_maxPointsForOtherGraphs);
	}

	//TD17718
	void Utilities::setMaxPointsForOtherGraphs(int newMaxPoints)
	{ 	
		m_maxPointsForOtherGraphs = newMaxPoints;  
	}

	//TD17718
	void Utilities::setMaxPointsForBarGraph(int newMaxPointsForBar)
	{ 
		m_maxPointsForBarGraphs = newMaxPointsForBar; 
	}

	//TD17529
	void  Utilities::setGraphRefreshInterval(int graphRefreshInterval)
	{
		m_graphRefreshInterval = graphRefreshInterval;
	}

	//TD17529
	int Utilities::getGraphRefreshInterval() 
	{
		return m_graphRefreshInterval;
	}
}

#endif UtilitiesCpp_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_
