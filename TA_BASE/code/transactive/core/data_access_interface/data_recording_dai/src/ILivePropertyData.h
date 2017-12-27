#ifndef ILIVE_PROPERTY_DATA_H
#define ILIVE_PROPERTY_DATA_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/ILivePropertyData.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <string>

#include "IRecordableData.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class ILivePropertyData : public IRecordableData
    {

    public:

        /**
         * Virtual Destructor
         *
         */
        virtual ~ILivePropertyData() {}

        
        /**
         * getTypeKey
         *
         * @return  the type key for this Live Property
         *
         */
        virtual unsigned long getTypeKey() = 0;


        /**
         * getCategory
         *
         * @return  the category for this Live Property
         *
         */
        virtual std::string getCategory() = 0;
        
         
        /**
         * getColumnType
         *
         * @return  the column type for this Live Property
         *
         */
        virtual std::string getColumnType() = 0;
        
         
        /**
         * getEngineeringMax
         *
         * @return  the engineering maximum for this Live Property
         *
         */
        virtual double getEngineeringMax() = 0;
        
         
        /**
         * getEngineeringMin
         *
         * @return  the engineering minimum for this Live Property
         *
         */
        virtual double getEngineeringMin() = 0;
        

        /**
         * getMinSummaryInterval
         *
         * @return  the minimum summary interval for this Live Property
         *
         */
        virtual unsigned long getMinSummaryInterval() = 0;
        

        /**
         * invalidate
         *
         * Marks the current data values as being invalid and requiring
         * a refresh from the database
         *
         */
        virtual void invalidate() = 0;
        
    };

    typedef boost::shared_ptr<ILivePropertyData> ILivePropertyDataPtr;
}

#endif // ILIVE_PROPERTY_DATA_H
