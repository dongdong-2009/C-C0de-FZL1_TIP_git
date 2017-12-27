/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/Trend.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Trend is an implementation of ITrend. It holds the data specific to an Trend entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug information
#endif // _MSC_VER

#include "core/data_access_interface/trends/src/Trend.h"
#include "core/data_access_interface/trends/src/TrendHelper.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
   
    Trend::Trend( const std::string& name )
    {
    }


	Trend::~Trend()
    {
    }


    std::string Trend::getDescription()
    {
        TA_ASSERT(false, "getDescription() not implemented");
        return "";
    }


    std::string Trend::getGraphTypeAsString()
    {
        TA_ASSERT(false, "getGraphTypeAsString() not implemented");
        return "";
    }


    long Trend::getRangeSeconds()
    {
        TA_ASSERT(false, "getRangeSeconds() not implemented");
        return 0;
    }

	
    long Trend::getSampleTypeInSeconds()
    {
        TA_ASSERT(false, "getSampleTypeInSeconds() not implemented");
        return 0;
    }

	
    time_t Trend::getStartDateTime()
    {
        TA_ASSERT(false, "getStartDateTime() not implemented");
        return 0;
    }

	
    bool Trend::isLive()
    {
        TA_ASSERT(false, "isLive() not implemented");
        return true;
    }

	
    long Trend::getOffsetInSeconds()
    {
        TA_ASSERT(false, "getOffsetInSeconds() not implemented");
        return 0;
    }


    std::map<std::string, ItemInfo> Trend::getItems()
    {
        TA_ASSERT(false, "getItems() not implemented");
        std::map<std::string, ItemInfo> temp;
        return temp;
    }


    std::string Trend::getExtraInfo()
    {
        TA_ASSERT(false, "getExtraInfo() not implemented");
        return "";
    }


    unsigned long Trend::getKey()
    {
        TA_ASSERT(false, "getKey() not implemented");
        return 0;
    }


    std::string Trend::getName()
    {
        TA_ASSERT(false, "getName() not implemented");
        return "";
    }

	
    void Trend::invalidate()
    {
        TA_ASSERT(false, "invalidate() not implemented");
    }


    void Trend::decomposeTimeComponent(long rangeTimeInSeconds, long& hours, long& minutes)
    {
        TA_ASSERT(false, "decomposeTimeComponent() not implemented");
    }

    std::string Trend::getSamplePeriodAsString()
    {
        TA_ASSERT(false, "getSamplePeriodAsString() not implemented");
        return "";
    }
 
} // closes TA_Base_Core

