/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaTisZoneMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaTisZoneMap is a concrete implementation of IConfigPaTisZoneMap, which is in turn an implementation
  * of IPaTisZoneMap. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaTisZoneMaps.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaTisZoneMap.h"
#include "core/data_access_interface/pa/src/PaTisZoneMapHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaTisZoneMap::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string ID                 = "ID";
    
    static const std::string TISZONEENTITY      = "Tis Zone Entity";

    
    ConfigPaTisZoneMap::ConfigPaTisZoneMap()
     : m_PaTisZoneMapHelper( new PaTisZoneMapHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaTisZoneMap::ConfigPaTisZoneMap(unsigned long row, TA_Base_Core::IData& data)
     : m_PaTisZoneMapHelper(new PaTisZoneMapHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaTisZoneMap::ConfigPaTisZoneMap(const unsigned long key)
     : m_PaTisZoneMapHelper( new PaTisZoneMapHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaTisZoneMap::ConfigPaTisZoneMap( const ConfigPaTisZoneMap& theConfigPaTisZoneMap)
     : m_PaTisZoneMapHelper( new PaTisZoneMapHelper( *(theConfigPaTisZoneMap.m_PaTisZoneMapHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaTisZoneMap::~ConfigPaTisZoneMap()
    {
        try
        {
            delete m_PaTisZoneMapHelper;
            m_PaTisZoneMapHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaTisZoneMap::isNew()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");

        return m_PaTisZoneMapHelper->isNew();
    }


    void ConfigPaTisZoneMap::setPaZoneKey(const unsigned long id)
    {
        FUNCTION_ENTRY("setId");
        
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
    
        updatePaTisZoneMapChanges(ID, 
                                    isNew() ? 0 : m_PaTisZoneMapHelper->getPaZoneKey(), 
                                    id);

        m_PaTisZoneMapHelper->setPaZoneKey(id);

        FUNCTION_EXIT;
    }


    void ConfigPaTisZoneMap::setTisZoneEntity(const std::string& strTisZoneEntity)
    {
        FUNCTION_ENTRY("setTisMessageTag");
        
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
    
        updatePaTisZoneMapChanges(TISZONEENTITY, 
                                     isNew() ? "" : m_PaTisZoneMapHelper->getTisZoneEntity(),
                                     strTisZoneEntity);

        m_PaTisZoneMapHelper->setTisZoneEntity(strTisZoneEntity);

        FUNCTION_EXIT;
    }


    unsigned long ConfigPaTisZoneMap::getKey()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
        return m_PaTisZoneMapHelper->getKey();
    }


    unsigned long ConfigPaTisZoneMap::getPaZoneKey()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
        return m_PaTisZoneMapHelper->getPaZoneKey();
    }


    std::string ConfigPaTisZoneMap::getTisZoneEntity()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
        return m_PaTisZoneMapHelper->getTisZoneEntity();
    }


    time_t ConfigPaTisZoneMap::getDateCreated()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
		return m_PaTisZoneMapHelper->getDateCreated();
    }


    time_t ConfigPaTisZoneMap::getDateModified()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");
		return m_PaTisZoneMapHelper->getDateModified();
    }
 
	
    void ConfigPaTisZoneMap::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTisZoneMap::invalidate");
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");

        m_PaTisZoneMapHelper->invalidate();
        m_PaTisZoneMapChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTisZoneMap::invalidate");
    }


    void ConfigPaTisZoneMap::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTisZoneMap::deleteThisObject");
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The PaTisZoneMapHelper pointer is null.");

        // Delete the ConfigPaTisZoneMap from the database
        m_PaTisZoneMapHelper->deletePaTisZoneMap(cascade);
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaTisZoneMapHelper pointer m_PaTisZoneMapHelper");

        delete m_PaTisZoneMapHelper;
        m_PaTisZoneMapHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTisZoneMap::deleteThisObject");
    }


    void ConfigPaTisZoneMap::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTisZoneMap::applyChanges");
        TA_ASSERT(m_PaTisZoneMapHelper != NULL, "applyChanges() has been called after ConfigPaTisZoneMap was deleted");

        m_PaTisZoneMapHelper->writePaTisZoneMapData();

        m_PaTisZoneMapChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTisZoneMap::applyChanges");
    }

    void ConfigPaTisZoneMap::updatePaTisZoneMapChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaTisZoneMapChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_PaTisZoneMapChanges.find(name);

        if (matching != m_PaTisZoneMapChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_PaTisZoneMapChanges.erase(matching);
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
            m_PaTisZoneMapChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaTisZoneMap::updatePaTisZoneMapChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaTisZoneMapChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaTisZoneMapChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaTisZoneMap::setName(const std::string& name)
    {
        //TA_ASSERT(false ,"setName() should not be called.");
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setName() should not be called.");
    }

    std::string ConfigPaTisZoneMap::getName()
    {
        TA_ASSERT(m_PaTisZoneMapHelper != NULL,"The m_PaTisZoneMapHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message " << m_PaTisZoneMapHelper->getKey();

        return name.str();
    }
    
} // closes TA_Base_Core

