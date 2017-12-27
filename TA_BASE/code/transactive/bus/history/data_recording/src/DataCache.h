#ifndef DATA_CACHE_H
#define DATA_CACHE_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/DataCache.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Stores the most current data for all Recordable Items, for 
 * one type as defined by the Live Property configuration
 *
 */

#include <map>
#include <vector>
#include <string>

#include "bus/history/data_recording/src/CachedData.h"
#include "bus/history/data_recording/src/IDataSink.h"
#include "bus/history/data_recording/src/Datum.h"
#include "core/data_access_interface/data_recording_dai/src/IRecordableData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
    class DataCache
    {
        
    public:

        typedef std::map<unsigned long, CachedData> DataMap;


        /**
         * Constructor
         *
         * @param   type    Live Property type being stored by cache
         *
         */
        DataCache( const std::string& type );

        
        /**
         * Destructor
         *
         */
        ~DataCache();


        /**
         * updateCache
         *
         * Adds a new set of data for one Item to the cache. If
         * data for the Item already exists, then it will be replaced
         * if the new data is more recent.
         *
         * @param   id          unique identifier for the Item
         * @param   timestamp   source time for the data
         * @param   data        the data itself, the Cache must decide what
         *                      (if any) Datums are relevant
         * @param   quality     the quality of the data
         *
         */
        void updateCache( unsigned long id, time_t timestamp, const std::vector<Datum>& data, int quality );


        /**
         * clearCache
         *
         * Removes all data from the cache
         *
         */
        void clearCache();


        /**
         * getName
         *
         * @return the type for the Cache
         *
         */
        const std::string& getType() const;


        /**
         * addField
         *
         * Adds a new LiveProperty to be stored by the Cache for 
         * each Item.
         *
         * @param   property    configuration for the new LiveProperty
         * 
         */
        void addField( TA_Base_Core::IRecordableDataPtr field );

    
        /**
         * removeField
         *
         * Removes a LiveProperty from being stored by the Cache.
         *
         * @param   propertyName    the name of the LiveProperty to remove
         *
         */
        void removeField( const std::string& fieldName );


        /**
         * removeItem
         *
         * Removes an Item from being stored by the Cache.
         *
         * @param   itemId    identifier of Item to remove
         *
         */
        void removeItem( unsigned long itemId );


        /**
         * getColumnMap
         *
         * @return a mapping of LiveProperty Identifiers -> Column names
         *
         */
        const std::map<std::string, std::string>& getColumnMap() const;


        /**
         * getData
         *
         * @return  the cached data, indexed by Item id
         *
         */
        const DataMap& getData() const;


        /**
         * getSize
         *
         * @return the number of cached items 
         *
         */
        unsigned int getSize() const;


    private:


        /**
         * Prevent copying
         *
         */
        DataCache( const DataCache& copyMe );
        DataCache& operator=( const DataCache& copyMe );


        /**
         * extractRelevantData
         *
         * Searches a set of data for LiveProperty identifers 
         * that are relevant to the Cache
         *
         * @param   data            the set of data to search
         * @param   relevantData    relevant data will be copied here
         *
         */
        void extractRelevantData( const std::vector<Datum>& data, std::vector<Datum>& relevantData );


        /**
         * The data in the cache, indexed by the LiveProperty Identifier
         */
        DataMap                                         m_items;

        /**
         * The type that this DataCache represents
         */
        std::string                                     m_type;

        /**
         * LiveProperty configuration for this DataCache
         */
        std::vector<TA_Base_Core::IRecordableDataPtr>   m_config;

        /**
         * Map of Identifier --> Column names
         */
        std::map<std::string, std::string>              m_columnMap;
    };

    typedef boost::shared_ptr<DataCache> DataCachePtr;
}

#endif // DATA_CACHE_H
