/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/ConfigPaBroadcastShortcut.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaBroadcastShortcut is a concrete implementation of IConfigPaBroadcastShortcut, which is in turn an implementation
  * of IPaDvaMessage. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessages.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/ConfigPaBroadcastShortcut.h"
#include "core/data_access_interface/pa/src/PaBroadcastShortcutHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaBroadcastShortcut::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.

    ConfigPaBroadcastShortcut::ConfigPaBroadcastShortcut()
     : m_pPaBroadcastShortcutHelper( new PaBroadcastShortcutHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaBroadcastShortcut::ConfigPaBroadcastShortcut(unsigned long row, TA_Base_Core::IData& data)
     : m_pPaBroadcastShortcutHelper(new PaBroadcastShortcutHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaBroadcastShortcut::ConfigPaBroadcastShortcut(const unsigned long key)
     : m_pPaBroadcastShortcutHelper( new PaBroadcastShortcutHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaBroadcastShortcut::ConfigPaBroadcastShortcut( const ConfigPaBroadcastShortcut& theConfigPaBroadcastShortcut)
     : m_pPaBroadcastShortcutHelper( new PaBroadcastShortcutHelper( *(theConfigPaBroadcastShortcut.m_pPaBroadcastShortcutHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaBroadcastShortcut::~ConfigPaBroadcastShortcut()
    {
        try
        {
            delete m_pPaBroadcastShortcutHelper;
            m_pPaBroadcastShortcutHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaBroadcastShortcut::isNew()
    {
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");

        return m_pPaBroadcastShortcutHelper->isNew();
    }

    void ConfigPaBroadcastShortcut::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");

        updatePaBroadcastShortcutChanges(LOCATION_KEY, 
                                    isNew() ? 0 : m_pPaBroadcastShortcutHelper->getLocationKey(), 
                                    locationKey);

        m_pPaBroadcastShortcutHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }
    
    void ConfigPaBroadcastShortcut::setShortcutNo( const unsigned long ulShortcutNo )
    {
        FUNCTION_ENTRY("setShortcutNo");
        
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
        
        updatePaBroadcastShortcutChanges(SHORTCUT_NO, 
            isNew() ? 0 : m_pPaBroadcastShortcutHelper->getShortcutNo(), 
            ulShortcutNo);

        m_pPaBroadcastShortcutHelper->setShortcutNo(ulShortcutNo);
        
        FUNCTION_EXIT;
    }
    
    void ConfigPaBroadcastShortcut::setDVAMsgKey( const unsigned long ulDVAMsgKey )
    {
        FUNCTION_ENTRY("setDVAMsgKey");
        
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
        
        updatePaBroadcastShortcutChanges(DVA_MSG_KEY, 
            isNew() ? 0 : m_pPaBroadcastShortcutHelper->getDVAMsgKey(), 
            ulDVAMsgKey);
        
        m_pPaBroadcastShortcutHelper->setDVAMsgKey(ulDVAMsgKey);
        
        FUNCTION_EXIT;
    }

    unsigned long ConfigPaBroadcastShortcut::getKey()
    {
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
        return m_pPaBroadcastShortcutHelper->getKey();
    }

    unsigned long ConfigPaBroadcastShortcut::getLocationKey()
    {
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
        return m_pPaBroadcastShortcutHelper->getLocationKey();
    }
    
    unsigned long ConfigPaBroadcastShortcut::getShortcutNo()
    {
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
        return m_pPaBroadcastShortcutHelper->getShortcutNo();
    }
    
    unsigned long ConfigPaBroadcastShortcut::getDVAMsgKey()
    {
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");
        return m_pPaBroadcastShortcutHelper->getDVAMsgKey();
    }

    void ConfigPaBroadcastShortcut::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaBroadcastShortcut::invalidate");
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");

        m_pPaBroadcastShortcutHelper->invalidate();
        m_paDvaMessageChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaBroadcastShortcut::invalidate");
    }


    void ConfigPaBroadcastShortcut::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaBroadcastShortcut::deleteThisObject");
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The PaBroadcastShortcutHelper pointer is null.");

        // Delete the ConfigPaBroadcastShortcut from the database
        m_pPaBroadcastShortcutHelper->deletePaBroadcastShortcut();
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaBroadcastShortcutHelper pointer m_pPaBroadcastShortcutHelper");

        delete m_pPaBroadcastShortcutHelper;
        m_pPaBroadcastShortcutHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaBroadcastShortcut::deleteThisObject");
    }


    void ConfigPaBroadcastShortcut::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaBroadcastShortcut::applyChanges");
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL, "applyChanges() has been called after ConfigPaBroadcastShortcut was deleted");

        m_pPaBroadcastShortcutHelper->writePaBroadcastShortcut();

        m_paDvaMessageChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaBroadcastShortcut::applyChanges");
    }

    void ConfigPaBroadcastShortcut::updatePaBroadcastShortcutChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaBroadcastShortcutChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paDvaMessageChanges.find(name);

        if (matching != m_paDvaMessageChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paDvaMessageChanges.erase(matching);
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
            m_paDvaMessageChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaBroadcastShortcut::updatePaBroadcastShortcutChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaBroadcastShortcutChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaBroadcastShortcutChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaBroadcastShortcut::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    std::string ConfigPaBroadcastShortcut::getName()
    {
        TA_ASSERT(m_pPaBroadcastShortcutHelper != NULL,"The m_pPaBroadcastShortcutHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message " << m_pPaBroadcastShortcutHelper->getKey();

        return name.str();
    }
    
} // closes TA_IRS_Core

