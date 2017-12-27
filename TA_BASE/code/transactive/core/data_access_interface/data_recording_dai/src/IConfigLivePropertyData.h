#ifndef ICONFIG_LIVE_PROPERTY_DATA_H
#define ICONFIG_LIVE_PROPERTY_DATA_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/IConfigLivePropertyData.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include "core/data_access_interface/data_recording_dai/src/ILivePropertyData.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class IConfigLivePropertyData : public ILivePropertyData
    {

    public:

        /**
         * Virtual Destructor
         *
         */
        virtual ~IConfigLivePropertyData() {}

        
        /**
         * setTypeKey
         *
         * @param typeKey  the type key for this Live Property
         *
         */
        virtual void setTypeKey( unsigned long typeKey ) = 0;


        /**
         * setCategory
         *
         * @param category  the category for this Live Property
         *
         */
        virtual void setCategory( const std::string& category ) = 0;
        
         
        /**
         * setName
         *
         * @param name  the name for this Live Property
         *
         */
        virtual void setName( const std::string& name ) = 0;
        
         
        /**
         * setIdentifier
         *
         * @param id  the identifier for this Live Property
         *
         */
        virtual void setIdentifier( const std::string& id ) = 0;
        
         
        /**
         * setTable
         *
         * @param tableName  the table name for this Live Property
         *
         */
        virtual void setTable( const std::string& tableName ) = 0;
        
         
        /**
         * setColumn
         *
         * @param columnName  the column for this Live Property
         *
         */
        virtual void setColumn( const std::string& columnName ) = 0;
        
         
        /**
         * setColumnType
         *
         * @param columnType  the column type for this Live Property
         *
         */
        virtual void setColumnType( const std::string& columnType ) = 0;
        
         
        /**
         * setEngineeringMax
         *
         * @param engMax  the engineering maximum for this Live Property
         *
         */
        virtual void setEngineeringMax( double engMax ) = 0;
        
         
        /**
         * setEngineeringMin
         *
         * @param engMin  the engineering minimum for this Live Property
         *
         */
        virtual void setEngineeringMin( double engMin ) = 0;
        

        /**
         * setStatus
         *
         * @param status  the status for this Live Property
         *
         */
        virtual void setStatus( const std::string& status ) = 0;
        

        /**
         * setIntervalBetweenWrites
         *
         * @param interval  the interval between writes for this Live Property
         *
         */
        virtual void setIntervalBetweenWrites( unsigned long interval ) = 0;
        

        /**
         * setMinSummaryInterval
         *
         * @param interval  the minimum summary interval for this Live Property
         *
         */
        virtual void setMinSummaryInterval( unsigned long interval ) = 0;


        /**
         * applyChanges
         *
         * Writes pending changes to the database
         *
         */
        virtual void applyChanges() = 0;


        /**
         * remove
         *
         * Deletes this Live Property from the database
         *
         */
        virtual void remove() = 0;
        
    };

    typedef boost::shared_ptr<IConfigLivePropertyData> IConfigLivePropertyDataPtr;
}

#endif // ICONFIG_LIVE_PROPERTY_DATA_H
