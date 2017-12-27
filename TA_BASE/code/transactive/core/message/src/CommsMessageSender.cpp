/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/CommsMessageSender.cpp $
  * @author:  Jade Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Comms messages are sent between processes within Transactive
  * They are not logged and do not require a description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <ctime>

#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/TA_CosUtility.h"

// For LOCAL_LOCATION and ANY_LOCATION
#include "core/message/src/SubscriptionManager.h"

namespace TA_Base_Core
{
    CommsMessageSender::CommsMessageSender( StructuredEventSupplier* eventSupplier )
        : m_supplier(eventSupplier)
    {
        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
    }


    CommsMessageSender::~CommsMessageSender()
    {
    }


    void CommsMessageSender::sendCommsMessage( const MessageType& messageType,
                                               unsigned long entityKey,
                                               const CORBA::Any& data,
                                               unsigned long subsystemKey,
                                               unsigned long locationKey,
                                               const FilterData* filterableData )
    {
        // TODO LPP: Perhaps LOCAL_LOCATION should be translated to gGetLocalLocationKey().
        TA_Assert(locationKey != LOCAL_LOCATION && locationKey != ANY_LOCATION);

        // Collate Variable Header fields
        FilterData varHeader;

        short priority = MessageTypeTableAccessor::getInstance().getPriority( messageType );
        NameValuePair priorityField( CosNotification::Priority, toString(priority) );
        varHeader.push_back( &priorityField );

        // Collate Filterable Body fields
        NameValuePair regionFilter( REGION_FILTER_NAME, toString(locationKey) );
        NameValuePair messageTypeFilter( MESSAGETYPE_FILTER_NAME, messageType.getTypeKey() );

        FilterData filters;
        filters.push_back( &regionFilter );
        filters.push_back( &messageTypeFilter );

        if ( filterableData != NULL )
        {
            for ( FilterData::const_iterator it = filterableData->begin(); it != filterableData->end(); ++it )
            {
                filters.push_back( *it );
            }
        }

        // Create CommsMessageCorbaDef object
        CommsMessageCorbaDef* commsMessage = new CommsMessageCorbaDef;

        commsMessage->createTime = time( NULL );
        commsMessage->messageTypeKey = messageType.getTypeKey().c_str();
        commsMessage->assocEntityKey = entityKey;

        // Inject the data
        commsMessage->messageState = data;

        // Create Structured Event, inject Any data
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= commsMessage;

        gPopulateStructuredEventHeader( *event,
                                        messageType.getContextObj().getContextName(),
                                        gGetSegmentedEntityKey(entityKey) + TYPE_NAME_SEPARATOR + messageType.getTypeKey(),
                                        &varHeader,
                                        &filters );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
    }


    void CommsMessageSender::sendBroadcastCommsMessage( const MessageType& messageType,
                                                        unsigned long entityKey,
                                                        const CORBA::Any& data,
                                                        const FilterData* filterableData )
    {
        // Collate Variable Header fields
        FilterData varHeader;

        short priority = MessageTypeTableAccessor::getInstance().getPriority( messageType );
        NameValuePair priorityField( CosNotification::Priority, toString(priority) );
        varHeader.push_back( &priorityField );

        // Collate Filterable Body fields
        NameValuePair messageTypeFilter( MESSAGETYPE_FILTER_NAME, messageType.getTypeKey() );

        FilterData filters;
        filters.push_back( &messageTypeFilter );

        if ( filterableData != NULL )
        {
            for ( FilterData::const_iterator it = filterableData->begin(); it != filterableData->end(); ++it )
            {
                filters.push_back( *it );
            }
        }

        // Create CommsMessageCorbaDef object
        CommsMessageCorbaDef* commsMessage = new CommsMessageCorbaDef;

        commsMessage->createTime = time( NULL );
        commsMessage->messageTypeKey = messageType.getTypeKey().c_str();
        commsMessage->assocEntityKey = entityKey;
        commsMessage->messageState = data;

        // Create Structured Event, inject Any data
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= commsMessage;
        gPopulateStructuredEventHeader( *event,
                                        messageType.getContextObj().getContextName(),
                                        messageType.getTypeKey(),
                                        &varHeader,
                                        &filters );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
    }


    void CommsMessageSender::sendBroadcastCommsMessage( const MessageType& messageType,
                                                        const CORBA::Any& data,
                                                        const FilterData* filterableData )
    {
        sendBroadcastCommsMessage(messageType, 0, data, filterableData);
    }

}; // TA_Base_Core

