#ifndef LIVE_PROPERTY_DATA_H
#define LIVE_PROPERTY_DATA_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/LivePropertyData.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include <string>

#include "core/data_access_interface/data_recording_dai/src/ILivePropertyData.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyDataHelper.h"

namespace TA_Base_Core
{
    class LivePropertyData : public ILivePropertyData
    {

    public:

        /**
         * Constructor
         *
         * @param   typeKey     the type key for the LiveProperty
         * @param   name        the name of the LiveProperty
         *
         */
        LivePropertyData( unsigned long typeKey, const std::string& name );


        /**
         * Virtual Destructor
         *
         */
        virtual ~LivePropertyData();

        
        /**
         * getTypeKey
         *
         * @return  the type key for this Live Property
         *
         */
        virtual unsigned long getTypeKey();


        /**
         * getCategory
         *
         * @return  the category for this Live Property
         *
         */
        virtual std::string getCategory();
        
         
        /**
         * getName
         *
         * @return  the name for this Live Property
         *
         */
        virtual std::string getName();
        
         
        /**
         * getIdentifier
         *
         * @return  the identifier for this Live Property
         *
         */
        virtual std::string getIdentifier();
        
         
        /**
         * getTable
         *
         * @return  the table name for this Live Property
         *
         */
        virtual std::string getTable();
        
         
        /**
         * getColumn
         *
         * @return  the column for this Live Property
         *
         */
        virtual std::string getColumn();
        
         
        /**
         * getColumnType
         *
         * @return  the column type for this Live Property
         *
         */
        virtual std::string getColumnType();
        
         
        /**
         * getEngineeringMax
         *
         * @return  the engineering maximum for this Live Property
         *
         */
        virtual double getEngineeringMax();
        
         
        /**
         * getEngineeringMin
         *
         * @return  the engineering minimum for this Live Property
         *
         */
        virtual double getEngineeringMin();
        

        /**
         * getStatus
         *
         * @return  the status for this Live Property
         *
         */
        virtual std::string getStatus();
        

        /**
         * getIntervalBetweenWrites
         *
         * @return  the interval between writes for this Live Property
         *
         */
        virtual unsigned long getIntervalBetweenWrites();
        

        /**
         * getMinSummaryInterval
         *
         * @return  the minimum summary interval for this Live Property
         *
         */
        virtual unsigned long getMinSummaryInterval();
        

        /**
         * invalidate
         *
         * Marks the current data values as being invalid and requiring
         * a refresh from the database
         *
         */
        virtual void invalidate();
        

    private:

        /**
         * Prevent copying
         *
         */
        LivePropertyData( const LivePropertyData& copyMe );
        LivePropertyData& operator=( const LivePropertyData& copyMe );


        LivePropertyDataHelper  m_helper;

    };
}

#endif // LIVE_PROPERTY_DATA_H
