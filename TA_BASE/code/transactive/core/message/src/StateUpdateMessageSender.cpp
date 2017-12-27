/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/StateUpdateMessageSender.cpp $
  * @author:  Jade Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Used for sending StateUpdateMessages
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <ctime>

#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/TA_CosUtility.h"

namespace TA_Base_Core
{

    StateUpdateMessageSender::StateUpdateMessageSender( StructuredEventSupplier* eventSupplier )
        : m_supplier(eventSupplier)
	{
        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
	}


    StateUpdateMessageSender::~StateUpdateMessageSender()
    {
    }


    void StateUpdateMessageSender::sendStateUpdateMessage( const MessageType& messageType,
                                                           unsigned long entityKey,
                                                           const std::string& agentName,
                                                           const CORBA::Any& updateInfo,
                                                           const FilterData* filterableData )
    {

        TA_ASSERT( !agentName.empty(), "Agent name was empty" );

        // Collate Variable Header fields
        FilterData varHeader;

        short priority = MessageTypeTableAccessor::getInstance().getPriority( messageType );
        NameValuePair priorityField( CosNotification::Priority, toString(priority) );
        varHeader.push_back( &priorityField );


          // Collate Filterable Body fields
          NameValuePair agentFilter( AGENTNAME_FILTER_NAME, agentName );

          FilterData filters;
          filters.push_back( &agentFilter );

          if ( filterableData != NULL )
          {
              for ( FilterData::const_iterator it = filterableData->begin(); it != filterableData->end(); ++it )
              {
                  filters.push_back( *it );
              }
          }

        // Create StateUpdateMessage
        StateUpdateMessageCorbaDef * updateMessage = new StateUpdateMessageCorbaDef;

        updateMessage->createTime = time( NULL );
        updateMessage->messageTypeKey = messageType.getTypeKey().c_str();
        updateMessage->assocEntityKey = entityKey;
        updateMessage->stateUpdateInfo = updateInfo;

        // Create Structured Event, inject Any data
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= updateMessage;

        gPopulateStructuredEventHeader( *event,
                                        messageType.getContextObj().getContextName(),
                                        agentName + TYPE_NAME_SEPARATOR + messageType.getTypeKey(),
                                        &varHeader,
                                        &filters );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
    }


}; // TA_Base_Core


