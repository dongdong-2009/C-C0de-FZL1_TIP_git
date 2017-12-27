/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageTypeData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/26 14:07:36 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * MessageTypeData is an implementation of IMessageTypeData. It holds the data specific to a message
 * type entry in the database.
 */

#include "core/data_access_interface/src/MessageTypeData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{

    MessageTypeData::MessageTypeData(const std::string& key)
		: m_strKey(key)
    {
        m_messageHelper = new MessageTypeHelper( key );
    }

    MessageTypeData::MessageTypeData(const std::string& key,
                        const std::string& name,
                        const std::string& description,
                        int priority,
                        bool toBePrinted,
                        bool toBeLogged,
                        bool toBePersisted,
						unsigned int viewLevel,
						bool isRealTimeEvent,
						unsigned long actionID)
		: m_messageHelper( new MessageTypeHelper(key,name,description,priority,toBePrinted,toBeLogged,toBePersisted,viewLevel,isRealTimeEvent,actionID) )
    {
    }


	MessageTypeData::MessageTypeData(const unsigned long row, TA_Base_Core::IData& data)
		: m_messageHelper( new MessageTypeHelper(row, data) )
	{
	}

	unsigned long MessageTypeData::getKey()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
		
        return m_messageHelper->getKey();
    }


    std::string MessageTypeData::getName()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->getName();
    }

	std::string MessageTypeData::getDisplayName()
	{
		TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
		return m_messageHelper->getDisplayName();
	}

    std::string MessageTypeData::getDescription()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->getDescription();
    }


    unsigned int MessageTypeData::getPriority()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->getPriority();
    }


    bool MessageTypeData::toBePrinted()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->toBePrinted();
    }


    bool MessageTypeData::toBeLogged()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->toBeLogged();
    }


    bool MessageTypeData::toBePersisted()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->toBePersisted();
    }


    time_t MessageTypeData::getDateCreated()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->getDateCreated();
    }


    time_t MessageTypeData::getDateModified()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->getDateModified();
    }


    unsigned int MessageTypeData::getViewLevel()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        return m_messageHelper->getViewLevel();
    }

	unsigned long MessageTypeData::getActionId()
	{
		TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
		return m_messageHelper->getActionId();
	}

	bool MessageTypeData::getIsEnableRealTimeEvent() 
	{
		TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
		return m_messageHelper->getIsEnableRealTimeEvent();
	}

    void MessageTypeData::invalidate()
    {
        TA_ASSERT(m_messageHelper != NULL,"The MessageHelper pointer is null.");
        m_messageHelper->invalidate();
    }

} // closes TA_Base_Core


