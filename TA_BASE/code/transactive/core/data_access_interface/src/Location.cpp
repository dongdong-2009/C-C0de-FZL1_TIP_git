/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Location.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Location is an implementation of ILocation. It holds the data specific to an Location entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationHelper.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/StdPubUtility.h"

namespace TA_Base_Core
{
    Location::Location( const unsigned long key )
		: m_locationHelper( new LocationHelper(key) )
    {
    }


	Location::Location( const unsigned long key, const std::string& name, const std::string& description, const unsigned long orderId,
                         const std::string & displayname,const time_t dateCreated, const time_t dateModified,bool displayOnly)
		: m_locationHelper( new LocationHelper(key,name,description,orderId,displayname,dateCreated,dateModified,displayOnly) )
	{
	}

	Location::Location(const unsigned long row, TA_Base_Core::IData& data)
		: m_locationHelper( new LocationHelper(row, data) )
	{
	}

    Location::~Location()
    {
        try
        {
            if (m_locationHelper != NULL)
            {
                delete m_locationHelper;
                m_locationHelper = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    unsigned long Location::getKey()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getKey();
    }


    std::string Location::getName()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getName();
    }
	std::string Location::getDisplayName()
	{
		TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");
		return m_locationHelper->getDisplayName();
	}


	std::string Location::getDescription()
	{
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getDescription();
	}


    unsigned long Location::getOrderId()
	{
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getOrderId();
	}


    void Location::invalidate()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        m_locationHelper->invalidate();
    }


	std::string Location::getDefaultDisplay( unsigned long profileKey, int displayNumber )
	{
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getDefaultDisplay(profileKey, displayNumber);
	}

    bool Location::isProfileAssociated(unsigned long profileId)
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->isProfileAssociated(profileId);
    }
	 bool Location::isDisplayOnly()
	 {
		 TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

		 return m_locationHelper->isDisplayOnly();
	 }


#ifdef PROJECT_4669

    ILocation::ELocationType Location::getLocationType()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        try
        {
            return convertType( m_locationHelper->getTypeName() );
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknown location type name [key=%d][name=%s][type_name=%s]", getKey(), getName().c_str(), getTypeName().c_str() );
        }

        std::string name = getName();

        return LocationAccessFactory::getInstance().getLocationType(name);
    }

#endif // #ifdef PROJECT_4669


    std::string Location::getTypeName()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");
        return m_locationHelper->getTypeName();
    }


    ILocation::ELocationType Location::convertType( const std::string& typeName )
    {
        if ( 0 == TA_Base_Core::CStdPubUtility::stricmp( typeName.c_str(), "OCC" ) )
        {
            return ILocation::OCC;
        }
        else if ( 0 == TA_Base_Core::CStdPubUtility::stricmp( typeName.c_str(), "DEPOT" ) )
        {
            return ILocation::DPT;
        }
        else if ( 0 == TA_Base_Core::CStdPubUtility::stricmp( typeName.c_str(), "STATION" ) )
        {
            return ILocation::STATION;
        }
        else
        {
            throw "Bad type name";
        }
    }


    const std::string& Location::convertType( const ILocation::ELocationType locationType )
    {
        const static std::string OCC("OCC");
        const static std::string DEPOT("DEPOT");
        const static std::string STATION("STATION");
        const static std::string UNKNOWN("");

        switch ( locationType )
        {
        case ILocation::OCC:
            return OCC;

        case ILocation::DPT:
            return DEPOT;

        case  ILocation::STATION:
            return STATION;

        default:
            TA_ASSERT( false, "Bad location type" );
            return UNKNOWN;
        }
    }

} // closes TA_Base_Core
