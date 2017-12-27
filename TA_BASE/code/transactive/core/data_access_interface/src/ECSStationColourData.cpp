/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ECSStationColourData.cpp $
 * @author:  Andy Siow
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * ECSStationColourData is an implementation of IECSStationColourData. It holds the data specific to a Alarm
 * Station entry in the database.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/ECSStationColourData.h"
#include "core/data_access_interface/src/ECSStationColourHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    ECSStationColourData::ECSStationColourData( const unsigned long key )
		:
        m_helper( new ECSStationColourHelper(key) )
    {
    }


    ECSStationColourData::ECSStationColourData(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_helper( new ECSStationColourHelper(row, data) )
    {
    }


    ECSStationColourData::~ECSStationColourData()
    {
        try
        {
            delete m_helper;
            m_helper = NULL;
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "Unknown exception caught in destructor");
        }
    }


    unsigned long ECSStationColourData::getKey()
    {
        return m_helper->getKey();
    }


    std::string ECSStationColourData::getName()
    {
        return m_helper->getName();
    }


	unsigned long ECSStationColourData::getStationColourKey(EColourType type)
	{
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

		return m_helper->getStationColourKey(type);
	}

    time_t ECSStationColourData::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getDateCreated();
    }


    time_t ECSStationColourData::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getDateModified();

    }

    void ECSStationColourData::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "ECSStationColourData::invalidate" );
		m_helper->invalidate();
    }



} // closes TA_Base_Core
