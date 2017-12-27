/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaTisMessageMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaTisMessageMap is a concrete implementation of IConfigPaTisMessageMap, which is in turn an implementation
  * of IPaTisMessageMap. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaTisMessageMaps.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaTisMessageMap.h"
#include "core/data_access_interface/pa/src/PaTisMessageMapHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaTisMessageMap::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string ID                 = "ID";
    static const std::string LABEL              = "Label";
    static const std::string TYPE               = "Type";
    
    static const std::string TISMESSAGETAG      = "Tis Message Tag";
    static const std::string TISLIBRARYSECTION  = "Tis Library Section";
    static const std::string TISLIBRARYVER      = "TIS Library Version";

    
    ConfigPaTisMessageMap::ConfigPaTisMessageMap()
     : m_paTisMessageMapHelper( new PaTisMessageMapHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaTisMessageMap::ConfigPaTisMessageMap(unsigned long row, TA_Base_Core::IData& data)
     : m_paTisMessageMapHelper(new PaTisMessageMapHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaTisMessageMap::ConfigPaTisMessageMap(const unsigned long key)
     : m_paTisMessageMapHelper( new PaTisMessageMapHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaTisMessageMap::ConfigPaTisMessageMap( const ConfigPaTisMessageMap& theConfigPaTisMessageMap)
     : m_paTisMessageMapHelper( new PaTisMessageMapHelper( *(theConfigPaTisMessageMap.m_paTisMessageMapHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaTisMessageMap::~ConfigPaTisMessageMap()
    {
        try
        {
            delete m_paTisMessageMapHelper;
            m_paTisMessageMapHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaTisMessageMap::isNew()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");

        return m_paTisMessageMapHelper->isNew();
    }


    void ConfigPaTisMessageMap::setPaDvaMessageKey(const unsigned long id)
    {
        FUNCTION_ENTRY("setId");
        
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
    
        updatePaTisMessageMapChanges(ID, 
                                    isNew() ? 0 : m_paTisMessageMapHelper->getPaDvaMessageKey(), 
                                    id);

        m_paTisMessageMapHelper->setPaDvaMessageKey(id);

        FUNCTION_EXIT;
    }


    void ConfigPaTisMessageMap::setTisMessageTag(const unsigned long tisMessageTag)
    {
        FUNCTION_ENTRY("setTisMessageTag");
        
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
    
        updatePaTisMessageMapChanges(TISMESSAGETAG, 
                                     isNew() ? 0 : m_paTisMessageMapHelper->getTisMessageTag(),
                                     tisMessageTag);

        m_paTisMessageMapHelper->setTisMessageTag(tisMessageTag);

        FUNCTION_EXIT;
    }


    void ConfigPaTisMessageMap::setTisLibrarySection(const unsigned long tisLibrarySection)
    {
        FUNCTION_ENTRY("setTisLibrarySection");
        
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
    
        updatePaTisMessageMapChanges(TISLIBRARYSECTION, 
                                     isNew() ? 0 : m_paTisMessageMapHelper->getTisLibrarySection(),
                                     tisLibrarySection);

        m_paTisMessageMapHelper->setTisLibrarySection(tisLibrarySection);

        FUNCTION_EXIT;
    }


    void ConfigPaTisMessageMap::setTisLibraryVersion(const unsigned long version)
    {
        FUNCTION_ENTRY("setTisLibraryVersion");
        
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
    
        updatePaTisMessageMapChanges(TISLIBRARYVER, 
                                  isNew() ? 0 : m_paTisMessageMapHelper->getTisLibraryVersion(), 
                                  version);

        m_paTisMessageMapHelper->setTisLibraryVersion(version);

        FUNCTION_EXIT;
    }


    unsigned long ConfigPaTisMessageMap::getKey()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getKey();
    }


    unsigned long ConfigPaTisMessageMap::getPaDvaMessageKey()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getPaDvaMessageKey();
    }


    unsigned long ConfigPaTisMessageMap::getTisMessageTag()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getTisMessageTag();
    }


    unsigned long ConfigPaTisMessageMap::getTisLibrarySection()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getTisLibrarySection();
    }


    unsigned long ConfigPaTisMessageMap::getTisLibraryVersion()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
        return m_paTisMessageMapHelper->getTisLibraryVersion();
    }

    time_t ConfigPaTisMessageMap::getDateCreated()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
		return m_paTisMessageMapHelper->getDateCreated();
    }


    time_t ConfigPaTisMessageMap::getDateModified()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");
		return m_paTisMessageMapHelper->getDateModified();
    }
 
	
    void ConfigPaTisMessageMap::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTisMessageMap::invalidate");
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");

        m_paTisMessageMapHelper->invalidate();
        m_paTisMessageMapChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTisMessageMap::invalidate");
    }


    void ConfigPaTisMessageMap::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTisMessageMap::deleteThisObject");
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The PaTisMessageMapHelper pointer is null.");

        // Delete the ConfigPaTisMessageMap from the database
        m_paTisMessageMapHelper->deletePaTisMessageMap(cascade);
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaTisMessageMapHelper pointer m_paTisMessageMapHelper");

        delete m_paTisMessageMapHelper;
        m_paTisMessageMapHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTisMessageMap::deleteThisObject");
    }


    void ConfigPaTisMessageMap::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTisMessageMap::applyChanges");
        TA_ASSERT(m_paTisMessageMapHelper != NULL, "applyChanges() has been called after ConfigPaTisMessageMap was deleted");

        m_paTisMessageMapHelper->writePaTisMessageMapData();

        m_paTisMessageMapChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTisMessageMap::applyChanges");
    }

    void ConfigPaTisMessageMap::updatePaTisMessageMapChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaTisMessageMapChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paTisMessageMapChanges.find(name);

        if (matching != m_paTisMessageMapChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paTisMessageMapChanges.erase(matching);
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
            m_paTisMessageMapChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaTisMessageMap::updatePaTisMessageMapChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaTisMessageMapChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaTisMessageMapChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaTisMessageMap::setName(const std::string& name)
    {
        //TA_ASSERT(false ,"setName() should not be called.");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setName() should not be called.");
    }

    std::string ConfigPaTisMessageMap::getName()
    {
        TA_ASSERT(m_paTisMessageMapHelper != NULL,"The m_paTisMessageMapHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message " << m_paTisMessageMapHelper->getKey();

        return name.str();
    }
    
} // closes TA_Base_Core

