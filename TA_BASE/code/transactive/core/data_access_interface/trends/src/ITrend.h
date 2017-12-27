/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/ITrend.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ITrend is an interface to a Trend object. It allows the Trend object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(ITrend_4F2C11BC_93BE_443d_85E4_362DB7A3AEE5__INCLUDED_)
#define ITrend_4F2C11BC_93BE_443d_85E4_362DB7A3AEE5__INCLUDED_

#include <string>
#include <map>

#include "core/data_access_interface/trends/src/ItemInfoDetails.h"
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class ITrend : public IItem
    {

    public:
	    
        virtual ~ITrend() {}

        /**
         * getDescription
         *
         * Get the description.
         *
         * return std::string - description string.
         */
        virtual std::string getDescription() =0;

        /**
         * getGraphTypeAsString
         *
         * Get the graph type as a string.
         *
         * @return std::string - graph type as a string (LINE, BAR, COMPARION, XY)
         */
        virtual std::string getGraphTypeAsString() =0;

        /**
         * getRangeSeconds
         *
         * Get the range (in seconds) of the time display of the graph
         *
         * @return long - range in seconds
         */
        virtual long getRangeSeconds() =0;
	    
        /**
         * getSampleTypeInSeconds
         *
         * Get the sample type.
         *
         * @return long - sample type in seconds.
         */
        virtual long getSampleTypeInSeconds() =0;
	    
        /**
         * getStartDateTime
         *
         * Get the start/date time
         *
         * @return time_t - start/date time as a time_t
         */
        virtual time_t getStartDateTime() =0;
	    
        /**
         * isLive
         *
         * Get the live mode.
         *
         * @return bool - True: is live, False: not live
         */
        virtual bool isLive() =0;
	    
        /**
         * getOffsetInSeconds
         *
         * Get the offset time.
         *
         * @return long - offset in seconds
         */
        virtual long getOffsetInSeconds() =0;

        /**
         * getOffsetInSeconds
         *
         * Get the offset time.
         *
         * @return long - offset in seconds
         */
        virtual std::map<std::string, ItemInfo> getItems() =0;

        /**
         * getExtraInfo
         *
         * Get the extra information.
         *
         * @return std::string - extra info string.
         */
        virtual std::string getExtraInfo() =0;

        /**
         * decomposeTimeComponent
         *
         * Decompose the time component of the range into hours and minutes.
         *
         * @param rangeTimeInSeconds - time component in seconds
         * @param hours - returned hours
         * @param minutes - return minutes
         *
         */
        virtual void decomposeTimeComponent(long rangeTimeInSeconds, long& hours, long& minutes) =0;

        /**
         * getSamplePeriodAsString
         *
         * Get the name of the sample period
         *
         * @return std::string - name of sample period, eg. 1 MIN, 1 DAY.
         */
        virtual std::string getSamplePeriodAsString() =0;
    };
}

#endif // !defined(ITrend_4F2C11BC_93BE_443d_85E4_362DB7A3AEE5__INCLUDED_)
