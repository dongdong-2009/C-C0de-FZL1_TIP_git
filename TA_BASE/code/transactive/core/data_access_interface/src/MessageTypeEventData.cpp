/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/MessageTypeEventData.cpp $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by: 
 * 
 * MessageTypeEventData is an implementation of IMessageTypeData. It holds the data specific to a message
 * type entry (for event viewer, i.e., has an related action_id in se_action table)in the database.
 */

#include "core/data_access_interface/src/MessageTypeEventData.h"

#include "core/utilities/src/DebugUtil.h"
//#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{

//     MessageTypeData::MessageTypeData(const std::string& key)
// 		: m_strKey(key)
//     {
//         m_messageHelper = new MessageTypeHelper( key );
//     }
	MessageTypeEventData::MessageTypeEventData()
	{
	}

    MessageTypeEventData::MessageTypeEventData(unsigned long& key,
                        const std::string& name,
                        const std::string& description,
                        int priority,
                        bool toBePrinted,
                        bool toBeLogged,
                        bool toBePersisted,
                        unsigned int viewLevel,
						unsigned long actionId,
						bool isEnableRealTimeEvent,
						time_t dateCreated,
						time_t dateModified)
						:m_key(key),m_name(name),m_description(description), m_priority(priority),
						m_toBePrinted(toBePrinted), m_toBeLogged(toBeLogged),m_toBePersisted(toBePersisted),
						m_viewLevel(viewLevel), m_actionId(actionId), m_isEnableRealTimeEvent(isEnableRealTimeEvent),
						m_dateCreated(dateCreated), m_dateModified(dateModified)
    {
    }

	unsigned long MessageTypeEventData::getActionId()
	{
		return m_actionId;
	}

	bool MessageTypeEventData::getIsEnableRealTimeEvent()
	{
		return m_isEnableRealTimeEvent;
	}

	unsigned long MessageTypeEventData::getKey()
    {
        return m_key;
    }


    std::string MessageTypeEventData::getName()
    {
       return m_name;
    }


    std::string MessageTypeEventData::getDescription()
    {
        return m_description;
    }


    unsigned int MessageTypeEventData::getPriority()
    {
        return m_priority;
    }


    bool MessageTypeEventData::toBePrinted()
    {
        return m_toBePrinted;
    }


    bool MessageTypeEventData::toBeLogged()
    {
       return m_toBeLogged;
    }


    bool MessageTypeEventData::toBePersisted()
    {
        return m_toBePersisted;
    }


    time_t MessageTypeEventData::getDateCreated()
    {
        return m_dateCreated;
    }


    time_t MessageTypeEventData::getDateModified()
    {
        return m_dateModified;
    }


    unsigned int MessageTypeEventData::getViewLevel()
    {
        return m_viewLevel;
    }

} // closes TA_Base_Core


