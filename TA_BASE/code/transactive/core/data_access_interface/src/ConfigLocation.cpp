/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigLocation.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigLocation is an implementation of IConfigLocation. It holds the data specific to a Location entry
  * in the database, and allows read-write access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/ConfigLocation.h"
#include "core/data_access_interface/src/LocationHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    long ConfigLocation::s_nextAvailableIdentifier = 0;

    const std::string ConfigLocation::NAME = "Name";
    const std::string ConfigLocation::DESCRIPTION = "Description";
	const std::string ConfigLocation::ORDER_ID = "OrderId";
	const std::string ConfigLocation::DISPLAYNAME = "DisplayName";
	const std::string ConfigLocation::DISPLAYONLY="DisplyOnly";

    ConfigLocation::ConfigLocation()
        : m_locationHelper( new LocationHelper() ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigLocation::ConfigLocation( const unsigned long key )
		: m_locationHelper( new LocationHelper(key) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


	ConfigLocation::ConfigLocation( const ConfigLocation& theLocation)
        : m_locationHelper( new LocationHelper(*(theLocation.m_locationHelper)) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}


	ConfigLocation::ConfigLocation( const unsigned long key, const std::string& name, const std::string& description, const unsigned long orderId,
                                    const std::string & displayName ,const time_t dateCreated, const time_t dateModified,bool dispayOnly)
		: m_locationHelper( new LocationHelper(key,name,description,orderId,displayName,dateCreated,dateModified,dispayOnly) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}

	ConfigLocation::ConfigLocation(const unsigned long row, TA_Base_Core::IData& data)
		: m_locationHelper( new LocationHelper(row, data) ),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}


    ConfigLocation::~ConfigLocation()
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


    bool ConfigLocation::isNew()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->isNew();
    }


    unsigned long ConfigLocation::getKey()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getKey();
    }


    std::string ConfigLocation::getName()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getName();
    }


	std::string ConfigLocation::getDescription()
	{
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getDescription();
	}

	std::string ConfigLocation::getDisplayName()
	{
		TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

		return m_locationHelper->getDisplayName();
	}

    unsigned long ConfigLocation::getOrderId()
	{
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getOrderId();
	}


    time_t ConfigLocation::getDateCreated()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getDateCreated();
    }


    time_t ConfigLocation::getDateModified()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getDateModified();
    }
	
	bool ConfigLocation::isDisplayOnly()
	{
		TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

		return m_locationHelper->isDisplayOnly();
	}

    void ConfigLocation::invalidate()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        m_locationHelper->invalidate();
        m_locationChanges.clear();
    }


    void ConfigLocation::applyChanges()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        m_locationHelper->applyChanges();

        m_locationChanges.clear();
    }

    
    void ConfigLocation::deleteThisLocation()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        m_locationHelper->deleteThisLocation();
    }


    void ConfigLocation::setName(const std::string& name)
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        updateLocationChanges(NAME,m_locationHelper->getName(), name);

        m_locationHelper->setName(name);
    }

	void ConfigLocation::setDisplayOnly(bool displayonly)
	{
		TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

		updateLocationChanges(DISPLAYONLY,m_locationHelper->isDisplayOnly()?"TRUE":"FALSE", displayonly?"TRUE":"FALSE");

		m_locationHelper->setDisplayOnly(displayonly);
	}

    void ConfigLocation::setDescription(const std::string& description)
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        updateLocationChanges(DESCRIPTION,m_locationHelper->getDescription(), description);

        m_locationHelper->setDescription(description);
    }


    void ConfigLocation::setOrderId(const unsigned long orderId)
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

		std::stringstream newOrderString;
		newOrderString << orderId;
		std::stringstream oldOrderString;
		oldOrderString << m_locationHelper->getOrderId();

		updateLocationChanges(ORDER_ID, oldOrderString.str(), newOrderString.str());

        m_locationHelper->setOrderId(orderId);
    }

	void ConfigLocation::setDisplayName(const std::string& displayName)
	{
		TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

		updateLocationChanges(DISPLAYNAME,m_locationHelper->getDisplayName(), displayName);

		m_locationHelper->setDisplayName(displayName);
	}


    std::string ConfigLocation::getDefaultDisplay(unsigned long profileKey, int displayNumber)
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getDefaultDisplay(profileKey, displayNumber);
    }


    bool ConfigLocation::isProfileAssociated(unsigned long profileId)
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->isProfileAssociated(profileId);
    }


#ifdef PROJECT_4669

    ILocation::ELocationType ConfigLocation::getLocationType() 
    { 
        TA_ASSERT(false,"Using getLocationType in ConfigLocation. Do not use this method here, it is only for use with Location.");
        return STATION;
    }

#endif // #ifdef PROJECT_4669


    void ConfigLocation::updateLocationChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateLocationChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_locationChanges.find(name);

        if (matching != m_locationChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_locationChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_locationChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


    std::string ConfigLocation::getTypeName()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getTypeName();
    }


} // closes TA_Base_Core
