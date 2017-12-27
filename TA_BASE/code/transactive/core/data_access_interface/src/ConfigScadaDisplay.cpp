/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigScadaDisplay.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigScadaDisplay is a concrete implementation of IConfigScadaDisplay, which is in turn an implementation
  * of IScadaDisplay. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for displays.
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/ConfigScadaDisplay.h"
#include "core/data_access_interface/src/ScadaDisplayHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    const std::string ConfigScadaDisplay::NAME = "Name";
    const std::string ConfigScadaDisplay::PATH = "Path";
    const std::string ConfigScadaDisplay::LOCATION = "Location";

    // The next unique identifier available
    long ConfigScadaDisplay::s_nextAvailableIdentifier = 0;


    ConfigScadaDisplay::ConfigScadaDisplay()
		: m_displayHelper( new ScadaDisplayHelper() ),
		  m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigScadaDisplay::ConfigScadaDisplay(const unsigned long key)
		: m_displayHelper( new ScadaDisplayHelper(key) ),
		  m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

	ConfigScadaDisplay::ConfigScadaDisplay(const unsigned long row, TA_Base_Core::IData& data)
		: m_displayHelper( new ScadaDisplayHelper(row, data) ),
		  m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigScadaDisplay::ConfigScadaDisplay( const ConfigScadaDisplay& theConfigScadaDisplay)
		: m_displayHelper( new ScadaDisplayHelper( *(theConfigScadaDisplay.m_displayHelper)) ),
		  m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigScadaDisplay::~ConfigScadaDisplay()
    {
    	if (m_displayHelper != NULL)
        {
            delete m_displayHelper;
            m_displayHelper = NULL;
        }
    }


    bool ConfigScadaDisplay::isNew()
    {
        TA_ASSERT(m_displayHelper != NULL, "isNew() has been called after display was deleted");

        return m_displayHelper->isNew();
    }

    
    void ConfigScadaDisplay::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");

        TA_ASSERT(m_displayHelper != NULL, "setName() has been called after display was deleted");
    
        updateChanges(NAME, m_displayHelper->getName(), name);

        m_displayHelper->setName(name);

        FUNCTION_EXIT;
    }


    void ConfigScadaDisplay::setPath(const std::string& path)
    {
        FUNCTION_ENTRY("setPath");

        TA_ASSERT(m_displayHelper != NULL, "setPath() has been called after display was deleted");

        updateChanges(PATH, m_displayHelper->getPath(), path);

        m_displayHelper->setPath(path);

        FUNCTION_EXIT;
    }


    void ConfigScadaDisplay::setLocation(const unsigned long location)
    {
        FUNCTION_ENTRY("setLocation");

        TA_ASSERT(m_displayHelper != NULL, "setLocation() has been called after display was deleted");

        // Retrieve the name of the old location now while we have the key set
        std::string oldLocation = m_displayHelper->getLocationName();

        m_displayHelper->setLocation(location);

        // Now retrieve the name of the new location now that the key has been changed
        std::string newLocation = m_displayHelper->getLocationName();
        
        updateChanges(LOCATION, oldLocation, newLocation);

        FUNCTION_EXIT;
    }

	

    unsigned long ConfigScadaDisplay::getKey()
    {
        TA_ASSERT(m_displayHelper != NULL, "getKey() has been called after display was deleted");

        return m_displayHelper->getKey();
    }


    std::string ConfigScadaDisplay::getName()
    {
        TA_ASSERT(m_displayHelper != NULL, "getName() has been called after display was deleted");

        return m_displayHelper->getName();
    }


    std::string ConfigScadaDisplay::getPath()
    {
        TA_ASSERT(m_displayHelper != NULL, "getPath() has been called after display was deleted");
        return m_displayHelper->getPath();
    }


	unsigned long ConfigScadaDisplay::getLocation()
	{
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
		return m_displayHelper->getLocation();
	}


    std::string ConfigScadaDisplay::getLocationName()
    {
        TA_ASSERT(m_displayHelper != NULL,"The ScadaDisplayHelper pointer is null.");
		return m_displayHelper->getLocationName();
    }


    time_t ConfigScadaDisplay::getDateCreated()
    {
        TA_ASSERT(m_displayHelper != NULL,"The OperatorHelper pointer is null.");
		return m_displayHelper->getDateCreated();
    }


    time_t ConfigScadaDisplay::getDateModified()
    {
        TA_ASSERT(m_displayHelper != NULL,"The OperatorHelper pointer is null.");
		return m_displayHelper->getDateModified();
    }
 
	
    void ConfigScadaDisplay::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(m_displayHelper != NULL, "invalidate() has been called after display was deleted");

        m_displayHelper->invalidate();
        m_displayChanges.clear();

        FUNCTION_EXIT;
    }


    void ConfigScadaDisplay::deleteThisObject()
    {
        FUNCTION_ENTRY("deleteThisObject");

        TA_ASSERT(m_displayHelper != NULL, "deleteThisObject() has been called after display was deleted");

        // Delete the display from the database
        m_displayHelper->deleteDisplay();
        
        delete m_displayHelper;
        m_displayHelper = NULL;

        FUNCTION_EXIT;
    }


    void ConfigScadaDisplay::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_displayHelper != NULL, "applyChanges() has been called after display was deleted");

        m_displayHelper->writeData();

        m_displayChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigScadaDisplay::applyChanges");
    }


    void ConfigScadaDisplay::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_displayChanges.find(name);

        if (matching != m_displayChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_displayChanges.erase(matching);
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
            m_displayChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core

