/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/Trend.h $
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

#if !defined(Trend_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Trend_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/trends/src/ITrend.h"


namespace TA_Base_Core
{
    class TrendHelper;

    class Trend : public ITrend
    {

    public:

        /**
         * Constructor
         *
         * Construct an Trend class based around a name.
         *
         * @param name The name of the Trend.
         */
        Trend( const std::string& name );

		/**
         * Destructor
         */
        virtual ~Trend();

        /**
         * getDescription
         *
         * Get the description.
         *
         * return std::string - description string.
         */
        virtual std::string getDescription();

        /**
         * getGraphTypeAsString
         *
         * Get the graph type as a string.
         *
         * @return std::string - graph type as a string (LINE, BAR, COMPARION, XY)
         */
        virtual std::string getGraphTypeAsString();

        /**
         * getRangeDaysComponent
         *
         * Get the range day component.
         *
         * @return int - number of days in the range.
         */
        //virtual int getRangeDaysComponent();
	    
        /**
         * getRangeSecondsComponent
         *
         * Get the range time component.
         *
         * @return long - number of seconds in the time component of the range.
         */
        //virtual long getRangeSecondsComponent();
	    
        virtual long getRangeSeconds();

        /**
         * getSampleTypeInSeconds
         *
         * Get the sample type.
         *
         * @return long - sample type in seconds.
         */
        virtual long getSampleTypeInSeconds();
	    
        /**
         * getStartDateTime
         *
         * Get the start/date time
         *
         * @return time_t - start/date time as a time_t
         */
        virtual time_t getStartDateTime();
	    
        /**
         * isLive
         *
         * Get the live mode.
         *
         * @return bool - True: is live, False: not live
         */
        virtual bool isLive();
	    
        /**
         * getOffsetInSeconds
         *
         * Get the offset time.
         *
         * @return long - offset in seconds
         */
        virtual long getOffsetInSeconds();

        /**
         * getItems
         *
         * Get the items set for trending.
         *
         * @return std::map<std::string, ItemInfo> - item name, item index and item colour.
         */
        virtual std::map<std::string, ItemInfo> getItems();

        /**
         * getExtraInfo
         *
         * Get the extra information.
         *
         * @return std::string - extra info string.
         */
        virtual std::string getExtraInfo();

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
        virtual void decomposeTimeComponent(long rangeTimeInSeconds, long& hours, long& minutes);

        /**
         * getSamplePeriodAsString
         *
         * Get the name of the sample period
         *
         * @return std::string - name of sample period, eg. 1 MIN, 1 DAY.
         */
        virtual std::string getSamplePeriodAsString();

        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();
		

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
        // Assignment operator not used so it is made private
		Trend& operator=(const Trend&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor of the TrendHelper
        // has only been written for ConfigTrend objects and will not copy the Trend helper
        // for a read-only object (ie this one).
        Trend( const Trend& theTrend);            

		TrendHelper* m_trendHelper;
    };
} // closes TA_Base_Core

#endif // !defined(Trend_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
