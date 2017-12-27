/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/DataCache.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Stores the most current data for all Recordable Items, for
 * one Table as defined by the Live Property configuration
 *
 */

#ifdef __WIN32__
#pragma warning (disable : 4786)
#endif

#include <sstream>

#include "bus/history/data_recording/src/DataCache.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    DataCache::DataCache( const std::string& type )
    : m_type( type )
    {
        // Nothing else
    }

    
    DataCache::~DataCache()
    {
        m_items.clear();
    }


    void DataCache::updateCache( unsigned long id, time_t timestamp, const std::vector<Datum>& data, int quality )
    {
        FUNCTION_ENTRY( "updateCache" );

        //
        // Is there any data in the set that is relevant to this cache ?
        //
        std::vector<Datum> relevantData;
        extractRelevantData( data, relevantData );

        if ( relevantData.size() == 0 )
        {
            std::stringstream logMessage;
            logMessage << m_type << ": No relevant data found";
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, logMessage.str().c_str() );
            return;
        }

        //
        // Does the id already exist in the cache ?
        //
        DataMap::iterator cacheIter = m_items.find( id );

        if ( cacheIter == m_items.end() )
        {
            // No - create a new cache entry
            CachedData newData( id, timestamp, relevantData, quality );
            m_items.insert( DataMap::value_type( id, newData ) );
        }
        else
        {
            // Yes - update cached data
            CachedData& currentCache = cacheIter->second; 
            TA_ASSERT( currentCache.m_id == id, "Cache Item Id's do not match" );

            std::vector<Datum>& currentData = currentCache.m_data;
            std::vector<Datum>::const_iterator newDataIter = relevantData.begin();
            for( ; newDataIter != relevantData.end(); ++newDataIter )
            {
                std::vector<Datum>::iterator currentDataIter = currentData.begin();
                bool found = false;
                for( ; currentDataIter != currentData.end(); ++currentDataIter )
                {
                    if ( currentDataIter->getName() == newDataIter->getName() )
                    {
                        //
                        // Already have a value for this property - update if timestamp
                        // is more recent
                        //
                        if ( currentCache.m_timestamp <= timestamp )
                        {
                            *currentDataIter = *newDataIter;
                        }
                        found = true;
                        break;
                    }
                }

                if ( ! found )
                {
                    //
                    // Don't have a value for this property - add it
                    //
                    currentData.push_back( *newDataIter );
                }
            }
            
            if ( currentCache.m_timestamp < timestamp )
            {
                currentCache.m_timestamp = timestamp;
                currentCache.m_quality = quality;
            }
        }

        FUNCTION_EXIT;
    }


    void DataCache::clearCache()
    {
        m_items.clear();
    }


    const std::string& DataCache::getType() const
    {
        return m_type;
    }


    void DataCache::addField( TA_Base_Core::IRecordableDataPtr field )
    {
        FUNCTION_ENTRY( "addField" );

        m_config.push_back( field );
        m_columnMap[ field->getIdentifier() ] = field->getColumn();
    }


    void DataCache::removeField( const std::string& fieldName )
    {
        FUNCTION_ENTRY( "removeProperty" );

        std::vector<TA_Base_Core::IRecordableDataPtr>::iterator dataIter = m_config.begin();
        for( ; dataIter != m_config.end(); ++dataIter )
        {
            TA_Base_Core::IRecordableDataPtr& item = *dataIter;

            if ( item->getName().compare( fieldName ) == 0 )
            {
                //
                // Remove from cached data
                //
                DataMap::iterator cacheIter = m_items.begin();
                for( ; cacheIter != m_items.end(); ++cacheIter )
                {
                    CachedData& data = cacheIter->second;
                    std::vector<Datum>::iterator datumIter = data.m_data.begin();
                    for( ; datumIter != data.m_data.end(); ++datumIter )
                    {
                        if ( (*datumIter).getName().compare( item->getIdentifier() ) == 0 )
                        {
                            data.m_data.erase( datumIter );
                            break;
                        }
                    }
                }
                m_columnMap.erase( item->getIdentifier() );
                m_config.erase( dataIter );
                return;
            }
        }

        std::stringstream errorMessage;
        errorMessage << "Cache " << m_type << " is unable to find field " 
                     << fieldName << " to remove";

        LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, errorMessage.str().c_str() );
    }


    void DataCache::removeItem( unsigned long itemId )
    {
        FUNCTION_ENTRY( "removeProperty" );
        m_items.erase( itemId );
    }


    const std::map<std::string, std::string>& DataCache::getColumnMap() const
    {
        return m_columnMap;
    }


    const DataCache::DataMap& DataCache::getData() const
    {
        return m_items;
    }

    
    unsigned int DataCache::getSize() const
    {
        return m_items.size();
    }


    void DataCache::extractRelevantData( const std::vector<Datum>& data, std::vector<Datum>& relevantData )
    {
        std::vector<Datum>::const_iterator dataIter = data.begin();
        for( ; dataIter != data.end(); ++dataIter )
        {
            // Is this relevant ?
            std::vector<TA_Base_Core::IRecordableDataPtr>::iterator fieldIter = m_config.begin();
            for( ; fieldIter != m_config.end(); ++fieldIter )
            {
                TA_Base_Core::IRecordableDataPtr& field = *fieldIter;

                if ( field->getIdentifier().compare( dataIter->getName() ) == 0 )
                {
                    relevantData.push_back( *dataIter );
                    break;
                }
            }
        }        
    }
}

