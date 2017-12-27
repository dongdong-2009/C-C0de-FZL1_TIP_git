/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/ConfigLivePropertyData.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include "core/data_access_interface/data_recording_dai/src/ConfigLivePropertyData.h"

namespace TA_Base_Core
{

    ConfigLivePropertyData::ConfigLivePropertyData()
    {
        // Nothing
    }


    ConfigLivePropertyData::ConfigLivePropertyData( unsigned long typeKey, const std::string& name )
    : m_helper( typeKey, name )
    {
        // Nothing else
    }


    ConfigLivePropertyData::~ConfigLivePropertyData()
    {
        // Nothing
    }


    unsigned long ConfigLivePropertyData::getTypeKey()
    {
        return m_helper.getTypeKey();
    }


    std::string ConfigLivePropertyData::getCategory()
    {
        return m_helper.getCategory();
    }


    std::string ConfigLivePropertyData::getName()
    {
        return m_helper.getName();
    }


    std::string ConfigLivePropertyData::getIdentifier()
    {
        return m_helper.getIdentifier();
    }


    std::string ConfigLivePropertyData::getTable()
    {
        return m_helper.getTable();
    }


    std::string ConfigLivePropertyData::getColumn()
    {
        return m_helper.getColumn();
    }
    

    std::string ConfigLivePropertyData::getColumnType()
    {
        return m_helper.getColumnType();
    }


    double ConfigLivePropertyData::getEngineeringMax()
    {
        return m_helper.getEngineeringMax();
    }


    double ConfigLivePropertyData::getEngineeringMin()
    {
        return m_helper.getEngineeringMin();
    }


    std::string ConfigLivePropertyData::getStatus()
    {
        return m_helper.getStatus();
    }


    unsigned long ConfigLivePropertyData::getIntervalBetweenWrites()
    {
        return m_helper.getIntervalBetweenWrites();
    }


    unsigned long ConfigLivePropertyData::getMinSummaryInterval()
    {
        return m_helper.getMinSummaryInterval();
    }

    
    void ConfigLivePropertyData::setTypeKey( unsigned long typeKey )
    {
        m_helper.setTypeKey( typeKey );
    }


    void ConfigLivePropertyData::setCategory( const std::string& category )
    {
        m_helper.setCategory( category );
    }

    
    void ConfigLivePropertyData::setName( const std::string& name )
    {
        m_helper.setName( name );
    }
    
     
    void ConfigLivePropertyData::setIdentifier( const std::string& id )
    {
        m_helper.setIdentifier( id );
    }
    
     
    void ConfigLivePropertyData::setTable( const std::string& tableName )
    {
        m_helper.setTable( tableName );
    }
    

    void ConfigLivePropertyData::setColumn( const std::string& columnName )
    {
        m_helper.setColumn( columnName );
    }
    
     
    void ConfigLivePropertyData::setColumnType( const std::string& columnType )
    {
        m_helper.setColumnType( columnType );
    }
    
     
    void ConfigLivePropertyData::setEngineeringMax( double engMax )
    {
        m_helper.setEngineeringMax( engMax );
    }
    
     
    void ConfigLivePropertyData::setEngineeringMin( double engMin )
    {
        m_helper.setEngineeringMin( engMin );
    }
    

    void ConfigLivePropertyData::setStatus( const std::string& status )
    {
        m_helper.setStatus( status );
    }
    

    void ConfigLivePropertyData::setIntervalBetweenWrites( unsigned long interval )
    {
        m_helper.setIntervalBetweenWrites( interval );
    }
    

    void ConfigLivePropertyData::setMinSummaryInterval( unsigned long interval )
    {
        m_helper.setMinSummaryInterval( interval );
    }

    
    void ConfigLivePropertyData::invalidate()
    {
        m_helper.invalidate();
    }


    void ConfigLivePropertyData::applyChanges()
    {
        m_helper.applyChanges();
    }


    void ConfigLivePropertyData::remove()
    {
        m_helper.remove();
    }

}

