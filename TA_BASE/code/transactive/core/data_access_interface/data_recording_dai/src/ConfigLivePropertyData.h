#ifndef CONFIG_LIVE_PROPERTY_DATA_H
#define CONFIG_LIVE_PROPERTY_DATA_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/ConfigLivePropertyData.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include "core/data_access_interface/data_recording_dai/src/IConfigLivePropertyData.h"
#include "core/data_access_interface/data_recording_dai/src/LivePropertyDataHelper.h"

namespace TA_Base_Core
{
    class ConfigLivePropertyData : public IConfigLivePropertyData
    {

    public:

        /**
         * Constructor
         *
         */
        ConfigLivePropertyData();


        /**
         * Constructor
         *
         */
        ConfigLivePropertyData( unsigned long typeKey, const std::string& name );


        /**
         * Virtual Destructor
         *
         */
        virtual ~ConfigLivePropertyData();


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
         * setTypeKey
         *
         * @param typeKey  the type key for this Live Property
         *
         */
        virtual void setTypeKey( unsigned long typeKey );


        /**
         * setCategory
         *
         * @param category  the category for this Live Property
         *
         */
        virtual void setCategory( const std::string& category );
        
         
        /**
         * setName
         *
         * @param name  the name for this Live Property
         *
         */
        virtual void setName( const std::string& name );
        
         
        /**
         * setIdentifier
         *
         * @param id  the identifier for this Live Property
         *
         */
        virtual void setIdentifier( const std::string& id );
        
         
        /**
         * setTable
         *
         * @param tableName  the table name for this Live Property
         *
         */
        virtual void setTable( const std::string& tableName );
        
         
        /**
         * setColumn
         *
         * @param columnName  the column for this Live Property
         *
         */
        virtual void setColumn( const std::string& columnName );
        
         
        /**
         * setColumnType
         *
         * @param columnType  the column type for this Live Property
         *
         */
        virtual void setColumnType( const std::string& columnType );
        
         
        /**
         * setEngineeringMax
         *
         * @param engMax  the engineering maximum for this Live Property
         *
         */
        virtual void setEngineeringMax( double engMax );
        
         
        /**
         * setEngineeringMin
         *
         * @param engMin  the engineering minimum for this Live Property
         *
         */
        virtual void setEngineeringMin( double engMin );
        

        /**
         * setStatus
         *
         * @param status  the status for this Live Property
         *
         */
        virtual void setStatus( const std::string& status );
        

        /**
         * setIntervalBetweenWrites
         *
         * @param interval  the interval between writes for this Live Property
         *
         */
        virtual void setIntervalBetweenWrites( unsigned long interval );
        

        /**
         * setMinSummaryInterval
         *
         * @param interval  the minimum summary interval for this Live Property
         *
         */
        virtual void setMinSummaryInterval( unsigned long interval );

        
        /**
         * invalidate
         *
         * Marks the current data values as being invalid and requiring
         * a refresh from the database
         *
         */
        virtual void invalidate();


        /**
         * applyChanges
         *
         * Writes pending changes to the database
         *
         */
        virtual void applyChanges();


        /**
         * remove
         *
         * Deletes this Live Property from the database
         *
         */
        virtual void remove();


    private:


        /**
         * Prevent copying
         *
         */
        ConfigLivePropertyData( const ConfigLivePropertyData& copyMe );
        ConfigLivePropertyData& operator=( const ConfigLivePropertyData& copyMe );


        LivePropertyDataHelper  m_helper;  
        
    };
}

#endif // CONFIG_LIVE_PROPERTY_DATA_H
