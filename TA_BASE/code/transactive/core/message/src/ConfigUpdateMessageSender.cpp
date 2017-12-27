/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/ConfigUpdateMessageSender.cpp $
  * @author:  Jade Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Used to send ConfigUpdate messages
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <ctime>

#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/src/TA_CosUtility.h"

namespace TA_Base_Core
{
    ConfigUpdateMessageSender::ConfigUpdateMessageSender(StructuredEventSupplier* eventSupplier)
        : m_supplier(eventSupplier)
    {
        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
    }


    ConfigUpdateMessageSender::~ConfigUpdateMessageSender()
    {
    }


    void ConfigUpdateMessageSender::sendConfigUpdateMessage( const MessageType& messageType,
                                                             unsigned long itemKey,
                                                             EModificationType modificationType,
                                                             const std::vector<std::string>& changes,
                                                             const FilterData* filterableData )
    {
        // Collate Variable Header fields
        FilterData varHeader;

        short priority = MessageTypeTableAccessor::getInstance().getPriority( messageType );
        NameValuePair priorityField( CosNotification::Priority, toString(priority) );
        varHeader.push_back( &priorityField );

        // Create ConfigUpdateMessageCorbaDef
        ConfigUpdateMessageCorbaDef* configMessage = new ConfigUpdateMessageCorbaDef;

        configMessage->createTime = time( NULL );
        configMessage->messageTypeKey = messageType.getTypeKey().c_str();
        configMessage->assocEntityKey = itemKey;
        configMessage->modificationType = modificationType;

        configMessage->changes.length( changes.size() );
        std::vector<std::string>::const_iterator changeIter = changes.begin();
        for ( int changeCount = 0; changeIter != changes.end(); ++changeIter, ++changeCount )
        {
            configMessage->changes[changeCount] = changeIter->c_str();
        }

        // Create Structured Event, inject Any data
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= configMessage;
        gPopulateStructuredEventHeader( *event,
                                        messageType.getContextObj().getContextName(),
                                        messageType.getTypeKey(),
                                        &varHeader,
                                        filterableData );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
    }


}; // TA_Base_Core

