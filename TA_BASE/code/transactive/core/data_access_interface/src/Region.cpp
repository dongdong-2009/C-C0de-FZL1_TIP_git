/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Region.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Region is an implementation of IRegion. It holds the data specific to an Region entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable: 4786)
#endif

#include "core/data_access_interface/src/Region.h"
#include "core/data_access_interface/src/RegionHelper.h"
#include "core/data_access_interface/src/RegionAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    Region::Region( const unsigned long key )
		: m_regionHelper( new RegionHelper(key) )
    {
    }


	Region::Region( const unsigned long key, const std::string& name, const std::string& description,
                        const time_t dateCreated, const time_t dateModified)
		: m_regionHelper( new RegionHelper(key,name,description,dateCreated,dateModified) )
	{
	}


	Region::Region(const unsigned long row, TA_Base_Core::IData& data)
		: m_regionHelper( new RegionHelper(row, data) )
    {
    }


    Region::~Region()
    {
        try
        {
            if (m_regionHelper != NULL)
            {
                delete m_regionHelper;
                m_regionHelper = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    unsigned long Region::getKey()
    {
        TA_ASSERT(m_regionHelper != NULL, "The region helper is NULL");

        return m_regionHelper->getKey();
    }


    std::string Region::getName()
    {
        TA_ASSERT(m_regionHelper != NULL, "The region helper is NULL");

        return m_regionHelper->getName();
    }


	std::string Region::getDescription()
	{
        TA_ASSERT(m_regionHelper != NULL, "The region helper is NULL");

        return m_regionHelper->getDescription();
	}

	std::string Region::getDisplayName()
	{
		TA_ASSERT(m_regionHelper != NULL, "The region helper is NULL");

		return m_regionHelper->getDisplayName();
	}


    void Region::invalidate()
    {
        TA_ASSERT(m_regionHelper != NULL, "The region helper is NULL");

        m_regionHelper->invalidate();
    }

} // closes TA_Base_Core
