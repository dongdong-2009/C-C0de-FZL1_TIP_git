/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/data_recording_dai/src/LivePropertyData.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * <description>
 */

#include "core/data_access_interface/data_recording_dai/src/LivePropertyData.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    const char* IRecordableData::STATUS_AVAILABLE = "AVAILABLE";

    LivePropertyData::LivePropertyData( unsigned long typeKey, const std::string& name )
    : m_helper( typeKey, name )
    {
        // Nothing else
    }


    LivePropertyData::~LivePropertyData()
    {
        // Nothing
    }

    
    unsigned long LivePropertyData::getTypeKey()
    {
        return m_helper.getTypeKey();
    }


    std::string LivePropertyData::getCategory()
    {
        return m_helper.getCategory();
    }
    
     
    std::string LivePropertyData::getName()
    {
        return m_helper.getName();
    }

    
    std::string LivePropertyData::getIdentifier()
    {
        return m_helper.getIdentifier();
    }

    
     
    std::string LivePropertyData::getTable()
    {
        return m_helper.getTable();
    }

    
     
    std::string LivePropertyData::getColumn()
    {
        return m_helper.getColumn();
    }

    
     
    std::string LivePropertyData::getColumnType()
    {
        return m_helper.getColumnType();
    }

    
     
    double LivePropertyData::getEngineeringMax()
    {
        return m_helper.getEngineeringMax();
    }

    
     
    double LivePropertyData::getEngineeringMin()
    {
        return m_helper.getEngineeringMin();
    }

    

    std::string LivePropertyData::getStatus()
    {
        return m_helper.getStatus();
    }

    

    unsigned long LivePropertyData::getIntervalBetweenWrites()
    {
        return m_helper.getIntervalBetweenWrites();
    }

    

    unsigned long LivePropertyData::getMinSummaryInterval()
    {
        return m_helper.getMinSummaryInterval();
    }

    
    void LivePropertyData::invalidate()
    {
        m_helper.invalidate();
    }
        
}

