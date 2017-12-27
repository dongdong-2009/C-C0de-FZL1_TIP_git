/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/IConfigTrend.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigTrend is an interface to a Trend object. It allows the Trend object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it. This particular object is a configuration location and allows modifications to be
  * performed in the database.
  */

#if !defined(IConfigTrend_BD299C0C_E517_4d7d_AE02_5BF43670DA53__INCLUDED_)
#define IConfigTrend_BD299C0C_E517_4d7d_AE02_5BF43670DA53__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/trends/src/ItemInfoDetails.h"
#include "core/data_access_interface/trends/src/ITrend.h"

namespace TA_Base_Core
{

    class IConfigTrend : public ITrend, virtual public IConfigItem 
    {

    public:

        virtual ~IConfigTrend() {}

        /**
         * setGraphType
         *
         * Sets the graph type.
         *
         * @param graphType - Graph type as a string (LINE, BAR, COMPARISON, XY)
         */
        virtual void setGraphType(const std::string& graphType) =0;
	    
        /**
         * setRangeSeconds
         *
         * Sets the range (in seconds) of the time display of the graph
         *
         * @param rangeSeconds - range in seconds
         */
        virtual void setRangeSeconds(long rangeSeconds) =0;
	    
        /**
         * setSampleType
         *
         * Sets the Sample Period
         *
         * @param sampleInSeconds - sample period in seconds
         */
        virtual void setSampleType(long sampleInSeconds) =0;
	    
        /**
         * setStartDateTime
         *
         * Sets the start/date time.
         *
         * @param startDateTime - start/date time as a time_t
         */
        virtual void setStartDateTime(time_t startDateTime) =0;
	    
        /**
         * setLive
         *
         * Sets the live status.
         *
         * @param live - flag representing live mode. True: live, False: not live.
         */
        virtual void setLive(bool live) =0;
	    
        /**
         * setOffset
         *
         * Sets the offset time.
         *
         * @param offsetInSeconds - offset time in seconds
         */
        virtual void setOffset(long offsetInSeconds) =0;
	    
        /**
         * setItems
         *
         * Sets the items for trending.
         *
         * @param items - items for trending. ItemInfo contains the item index and colour.
         */
        virtual void setItems(const std::map<std::string, ItemInfo>& items) =0;

        /**
         * setExtraInfo
         *
         * Sets the extra info.
         *
         * @param info - extra information.
         */
        virtual std::string getName() =0;

        /**
         * setExtraInfo
         *
         * Sets the extra info.
         *
         * @param info - extra information.
         */
        virtual void setExtraInfo(const std::string& info) = 0;

        /**
         * setDescription
         *
         * Sets the description.
         *
         * @param description - description as a string.
         */
        virtual void setDescription(const std::string& description) = 0;

        /**
         * deleteThisObject
         *
         * Removes this profile from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile has not been deleted
         */
        virtual void deleteThisObject() = 0;

        /**
         * getDateCreated
         *
         * Get the date created
         *
         * @return time_t - date created
         */
        virtual time_t getDateCreated() = 0;

        /**
         * getDateModified
         *
         * Get the date modified
         *
         * @return time_t - date modified
         */
        virtual time_t getDateModified() = 0;
    };
}

#endif // !defined(IConfigTrend_BD299C0C_E517_4d7d_AE02_5BF43670DA53__INCLUDED_)
