/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigMessageType.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by:  $Author: Ouyang $
 * 
 * ConfigMessageType is a read/write interface to the database for Message Types.
 */

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/ConfigMessageType.h"

namespace TA_Base_Core
{
    const std::string ConfigMessageType::NAME                   = "Name";
	const std::string ConfigMessageType::DISPLAY_NAME			= "DisplayName";
    const std::string ConfigMessageType::DESCRIPTION            = "Description";
    const std::string ConfigMessageType::PRINT_MESSAGE_STATUS   = "Print Status";
    const std::string ConfigMessageType::LOG_MESSAGE_STATUS     = "Log Status";
    const std::string ConfigMessageType::PERSIST_MESSAGE_STATUS = "Persist Status";
    const std::string ConfigMessageType::PRIORITY               = "Priority";
    const std::string ConfigMessageType::VIEW_LEVEL             = "View Level";

    long ConfigMessageType::s_nextAvailableIdentifier = 0;


    ConfigMessageType::ConfigMessageType(const std::string& key)
        :
        m_helper( new MessageTypeHelper(key) ),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


	ConfigMessageType::ConfigMessageType(const unsigned long row, TA_Base_Core::IData& data)
		:
		m_helper( new MessageTypeHelper(row, data) ),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}


	ConfigMessageType::~ConfigMessageType()
    {
        try
        {
            if (m_helper != NULL)
            {
                delete m_helper;
                m_helper = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "Caught in destructor");
        }
    }
   
    unsigned long ConfigMessageType::getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    }

    ItemChanges ConfigMessageType::getAllItemChanges()
    {
        return m_messageTypeChanges;
    }

    bool ConfigMessageType::hasChanged()
    {
        // If the alarm type changes list is empty then nothing has changed
        return !m_messageTypeChanges.empty();
    }
    

    void ConfigMessageType::setDescription(const std::string& description)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        std::string oldDescription = m_helper->getDescription();
        
        m_helper->setDescription(description);

        updateMessageTypeChanges(DESCRIPTION, oldDescription, description);
    }

    void ConfigMessageType::setName(const std::string& name)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        std::string oldName = m_helper->getName();

        m_helper->setName(name);

        updateMessageTypeChanges(NAME, oldName, name);
    }

	
    void ConfigMessageType::setPriority(const unsigned int priority)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        unsigned int oldPriority = getPriority();
        
        m_helper->setPriority(priority);

        updateMessageTypeChanges(
            PRIORITY,
            getUnsignedLongAsString(oldPriority),
            getUnsignedLongAsString(priority)    );
    }

	
    void ConfigMessageType::setPrintMessageStatus(const bool printMessageStatus)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        bool oldPrintStatus = toBePrinted();

        m_helper->setPrintMessageStatus(printMessageStatus);

        updateMessageTypeChanges(
            PRINT_MESSAGE_STATUS,
            getBoolAsString(oldPrintStatus),
            getBoolAsString(printMessageStatus) );
    }

	
    void ConfigMessageType::setLogMessageStatus(const bool logMessageStatus)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        bool oldLogStatus = toBeLogged();

        m_helper->setLogMessageStatus(logMessageStatus);

        updateMessageTypeChanges(
            LOG_MESSAGE_STATUS,
            getBoolAsString(oldLogStatus),
            getBoolAsString(logMessageStatus) );
    }


    void ConfigMessageType::setPersistMessageStatus(const bool persistMessageStatus)
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        bool oldPersistStatus = toBePersisted();

        m_helper->setPersistMessageStatus(persistMessageStatus);

        updateMessageTypeChanges(
            PERSIST_MESSAGE_STATUS,
            getBoolAsString(oldPersistStatus),
            getBoolAsString(persistMessageStatus) );
    }


    void ConfigMessageType::setViewLevel( const unsigned int viewLevel )
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        unsigned int oldViewLevel = getViewLevel();

        m_helper->setViewLevel( viewLevel );

        updateMessageTypeChanges(
            VIEW_LEVEL,
            getUnsignedLongAsString(oldViewLevel),
            getUnsignedLongAsString(viewLevel) );
    }

  
    void ConfigMessageType::deleteThisObject()
    {
        m_helper->deleteMessageType();
        delete m_helper;
        m_helper = NULL;
    }


    void ConfigMessageType::applyChanges()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        m_helper->writeMessageTypeData();
        m_messageTypeChanges.clear();
    }


    unsigned long ConfigMessageType::getKey()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getKey();
    }


    bool ConfigMessageType::isNew()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->isNew();
    }

  
    std::string ConfigMessageType::getName()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getName();
    }

	std::string ConfigMessageType::getDisplayName()
	{
		TA_ASSERT(m_helper != NULL,
			"Attempt to change state when object has been deleted.");
		return m_helper->getDisplayName();
	}

    std::string ConfigMessageType::getDescription()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getDescription();
    }


    unsigned int ConfigMessageType::getPriority()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");

        return m_helper->getPriority();
    }

	bool ConfigMessageType::toBePrinted()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        return m_helper->toBePrinted();
    }


	bool ConfigMessageType::toBeLogged()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        return m_helper->toBeLogged();
    }


    bool ConfigMessageType::toBePersisted()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        return m_helper->toBePersisted();
    }


    time_t ConfigMessageType::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        return m_helper->getDateCreated();
    }


    time_t ConfigMessageType::getDateModified()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        return m_helper->getDateModified();
    }


    unsigned int ConfigMessageType::getViewLevel()
    {
        TA_ASSERT(m_helper != NULL,
                  "Attempt to change state when object has been deleted.");
        
        return m_helper->getViewLevel();
    }

    void ConfigMessageType::invalidate()
    {
        m_helper->invalidate();
        m_messageTypeChanges.clear();
    }


    void ConfigMessageType::updateMessageTypeChanges(
        const std::string& name,
        const std::string& oldValue,
        const std::string& newValue)
    {
        FUNCTION_ENTRY("updateMessageTypeChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_messageTypeChanges.find(name);

        if (matching != m_messageTypeChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well
                // remove this from the list as it is not a valid change
                m_messageTypeChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the
        // old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this
            // item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_messageTypeChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }




    std::string ConfigMessageType::getBoolAsString(bool theBool)
    {
        //                 true    false
        return ( theBool ? "Yes" : "No" );
    }


    bool ConfigMessageType::getBoolFromString(const std::string& theString)
    {
        // "Yes" is true, anmything else is false
        return ( (theString == "Yes") ? true : false );
    }


    std::string ConfigMessageType::getUnsignedLongAsString(
        unsigned long number)
    {
        char buff[256] = {0};

        sprintf(buff,"%ld",number);
        //ltoa(number, buff, 10);

        return std::string(buff);
    }

    unsigned long ConfigMessageType::getUnsignedLongFromString(
        const std::string& theString)
    {
        return atol(theString.c_str());
    }


} //close namespace TA_Base_Core

