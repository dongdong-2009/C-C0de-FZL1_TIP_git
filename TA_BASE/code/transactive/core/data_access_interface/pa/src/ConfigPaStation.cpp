/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaStation.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaStation is a concrete implementation of IConfigPaStation, which is in turn an implementation
  * of IPaStation. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaStations.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaStation.h"
#include "core/data_access_interface/pa/src/PaStationHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaStation::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string ID                 = "Station ID";

    
    ConfigPaStation::ConfigPaStation()
     : m_paStationHelper( new PaStationHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaStation::ConfigPaStation(unsigned long row, TA_Base_Core::IData& data)
     : m_paStationHelper(new PaStationHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaStation::ConfigPaStation(const unsigned long key)
     : m_paStationHelper( new PaStationHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaStation::ConfigPaStation( const ConfigPaStation& theConfigPaStation)
     : m_paStationHelper( new PaStationHelper( *(theConfigPaStation.m_paStationHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaStation::~ConfigPaStation()
    {
        try
        {
            delete m_paStationHelper;
            m_paStationHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaStation::isNew()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");

        return m_paStationHelper->isNew();
    }

    void ConfigPaStation::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        
        updatePaStationChanges(LOCATIONKEY, 
                                  isNew() ? 0 : m_paStationHelper->getLocationKey(), 
                                  locationKey);

        m_paStationHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }
    

    void ConfigPaStation::setKey(const unsigned long id)
    {
        FUNCTION_ENTRY("setKey");
        
        updatePaStationChanges(ID, 
                               m_paStationHelper->getModifiedKey(), 
                               id);

        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");

        m_paStationHelper->setKeyToUse(id);

        FUNCTION_EXIT;
    }


    unsigned long ConfigPaStation::getLocationKey()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        return m_paStationHelper->getLocationKey();
    }

    unsigned long ConfigPaStation::getKey()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        return m_paStationHelper->getKey();
    }

    time_t ConfigPaStation::getDateCreated()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
		return m_paStationHelper->getDateCreated();
    }


    time_t ConfigPaStation::getDateModified()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
		return m_paStationHelper->getDateModified();
    }
 
	
    void ConfigPaStation::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaStation::invalidate");
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");

        m_paStationHelper->invalidate();
        m_paStationChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaStation::invalidate");
    }


    void ConfigPaStation::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaStation::deleteThisObject");
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");

        // Delete the ConfigPaStation from the database
        m_paStationHelper->deletePaStation(cascade);
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaStationHelper pointer m_paStationHelper");

        delete m_paStationHelper;
        m_paStationHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaStation::deleteThisObject");
    }


    void ConfigPaStation::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaStation::applyChanges");
        TA_ASSERT(m_paStationHelper != NULL, "applyChanges() has been called after ConfigPaStation was deleted");

        m_paStationHelper->writePaStationData();

        m_paStationChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaStation::applyChanges");
    }

    void ConfigPaStation::updatePaStationChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaStationChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paStationChanges.find(name);

        if (matching != m_paStationChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paStationChanges.erase(matching);
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
            TA_Base_Core::Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_paStationChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaStation::updatePaStationChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaStationChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaStationChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaStation::setName(const std::string& name)
    {
        //TA_ASSERT(false ,"setName() should not be called.");
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setName() should not be called.");
    }

    std::string ConfigPaStation::getName()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The m_paStationHelper pointer is null.");

        std::ostringstream name;
		name << "PA Station " << m_paStationHelper->getKey();

        return name.str();
    }
    
    unsigned long ConfigPaStation::getModifiedKey()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The m_paTrainDvaMessageHelper pointer is null.");        
        return m_paStationHelper->getModifiedKey();
    }

} // closes TA_Base_Core

