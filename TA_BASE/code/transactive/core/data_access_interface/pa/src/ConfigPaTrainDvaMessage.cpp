/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/pa/src/ConfigPaTrainDvaMessage.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPaTrainDvaMessage is a concrete implementation of IConfigPaTrainDvaMessage, 
  * which is in turn an implementation
  * of IPaTrainDvaMessage. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaTrainDvaMessages.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaTrainDvaMessage.h"
#include "core/data_access_interface/pa/src/PaTrainDvaMessageHelper.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaTrainDvaMessage::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string KEY                = "Key";
    static const std::string LABEL              = "Label";
    
    
    ConfigPaTrainDvaMessage::ConfigPaTrainDvaMessage()
     : m_paTrainDvaMessageHelper( new PaTrainDvaMessageHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaTrainDvaMessage::ConfigPaTrainDvaMessage(unsigned long row, TA_Base_Core::IData& data)
     : m_paTrainDvaMessageHelper(new PaTrainDvaMessageHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaTrainDvaMessage::ConfigPaTrainDvaMessage(const unsigned long key)
     : m_paTrainDvaMessageHelper( new PaTrainDvaMessageHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaTrainDvaMessage::ConfigPaTrainDvaMessage( const ConfigPaTrainDvaMessage& theConfigPaTrainDvaMessage)
     : m_paTrainDvaMessageHelper( new PaTrainDvaMessageHelper( *(theConfigPaTrainDvaMessage.m_paTrainDvaMessageHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaTrainDvaMessage::~ConfigPaTrainDvaMessage()
    {
        try
        {
            delete m_paTrainDvaMessageHelper;
            m_paTrainDvaMessageHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaTrainDvaMessage::isNew()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");

        return m_paTrainDvaMessageHelper->isNew();
    }


    void ConfigPaTrainDvaMessage::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel");
        
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");
    
        updatePaTrainDvaMessageChanges(LABEL, 
                                       isNew() ? "" : m_paTrainDvaMessageHelper->getLabel(), 
                                       label);

        m_paTrainDvaMessageHelper->setLabel(label);

        FUNCTION_EXIT;
    }


    unsigned long ConfigPaTrainDvaMessage::getKey()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");
        return m_paTrainDvaMessageHelper->getKey();
    }


    std::string ConfigPaTrainDvaMessage::getLabel()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");
        return m_paTrainDvaMessageHelper->getLabel();
    }    
    

    time_t ConfigPaTrainDvaMessage::getDateCreated()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");
		return m_paTrainDvaMessageHelper->getDateCreated();
    }


    time_t ConfigPaTrainDvaMessage::getDateModified()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");
		return m_paTrainDvaMessageHelper->getDateModified();
    }
 
	
    void ConfigPaTrainDvaMessage::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTrainDvaMessage::invalidate");
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");

        m_paTrainDvaMessageHelper->invalidate();
        m_paTrainDvaMessageChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTrainDvaMessage::invalidate");
    }


    void ConfigPaTrainDvaMessage::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTrainDvaMessage::deleteThisObject");
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");

        // Delete the ConfigPaTrainDvaMessage from the database
        m_paTrainDvaMessageHelper->deletePaTrainDvaMessage(cascade);
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaTrainDvaMessageHelper pointer m_paTrainDvaMessageHelper");

        delete m_paTrainDvaMessageHelper;
        m_paTrainDvaMessageHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTrainDvaMessage::deleteThisObject");
    }


    void ConfigPaTrainDvaMessage::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaTrainDvaMessage::applyChanges");
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL, "applyChanges() has been called after ConfigPaTrainDvaMessage was deleted");

        m_paTrainDvaMessageHelper->writePaTrainDvaMessageData();

        m_paTrainDvaMessageChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaTrainDvaMessage::applyChanges");
    }


    void ConfigPaTrainDvaMessage::updatePaTrainDvaMessageChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaTrainDvaMessageChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paTrainDvaMessageChanges.find(name);

        if (matching != m_paTrainDvaMessageChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paTrainDvaMessageChanges.erase(matching);
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
            m_paTrainDvaMessageChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaTrainDvaMessage::updatePaTrainDvaMessageChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaTrainDvaMessageChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaTrainDvaMessageChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaTrainDvaMessage::setName(const std::string& name)
    {
        //TA_ASSERT(false ,"setName() should not be called.");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setName() should not be called.");
    }

    std::string ConfigPaTrainDvaMessage::getName()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The m_paTrainDvaMessageHelper pointer is null.");

        std::ostringstream name;
		name << "PA Train DVA Message " << m_paTrainDvaMessageHelper->getKey();

        return name.str();
    }      

    void ConfigPaTrainDvaMessage::setKeyForNewMessage(unsigned long key)
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The m_paTrainDvaMessageHelper pointer is null.");

        // Allow setting of invalid keys (just means must be changed before applying changes)
        /*
        if (TA_Core::DatabaseKey::isInvalidKey(key))
        {
            TA_THROW(TA_Core::DataException("Key is null", 
                TA_Core::DataException::NO_VALUE, "Primary Key"));
        }
        */

        PaTrainDvaMessageHelper::validateKeyUniqueness(key);

        m_paTrainDvaMessageHelper->setKeyToUse(key);
    }
        
    void ConfigPaTrainDvaMessage::setKey(unsigned long key)
    {
        updatePaTrainDvaMessageChanges(KEY, 
                                       m_paTrainDvaMessageHelper->getModifiedKey(), 
                                       key);

        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The m_paTrainDvaMessageHelper pointer is null.");        
        m_paTrainDvaMessageHelper->setKeyToUse(key);
    }

    unsigned long ConfigPaTrainDvaMessage::getModifiedKey()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The m_paTrainDvaMessageHelper pointer is null.");        
        return m_paTrainDvaMessageHelper->getModifiedKey();
    }

} // closes TA_Base_Core

