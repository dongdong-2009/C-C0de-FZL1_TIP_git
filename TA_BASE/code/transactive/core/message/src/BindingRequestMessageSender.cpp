/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/BindingRequestMessageSender.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * The sender for BindingRequestMessages. This is used by the Transactive	
  * Distributed Naming Service
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <ctime>

#include "core/message/src/BindingRequestMessageSender.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/IDL/src/BindingRequestMessageCorbaDef.h"
#include "core/message/src/TA_CosUtility.h"

#include "core/utilities/src/RunParams.h" //yanrong++: CL-21199, 2010-01-19

namespace TA_Base_Core
{
    BindingRequestMessageSender::BindingRequestMessageSender(StructuredEventSupplier* eventSupplier)
        : m_supplier(eventSupplier)
    {
        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
    }


    BindingRequestMessageSender::~BindingRequestMessageSender()
    {
    }


    void BindingRequestMessageSender::sendBindingRequest( const MessageType& messageType,
                                                          const std::string& agentName,
                                                          CORBA::Object_ptr originator )
    {

        // Collate Variable Header fields
        FilterData varHeader;

        // TODO LPP: Yuck! Use a toString method.
        //short priority = MessageTypeTableAccessor::getInstance().getPriority( messageType );
        //char priorityChar[8];
        //sprintf( priorityChar, "%d", priority );
        std::string priorityString = "0";

        NameValuePair priorityField( CosNotification::Priority, priorityString );
        varHeader.push_back( &priorityField );

        // Create BindingRequestMessageCorbaDef
        BindingRequestMessageCorbaDef* bindingMessage = new BindingRequestMessageCorbaDef;

        bindingMessage->createTime = time( NULL );
        bindingMessage->messageTypeKey = messageType.getTypeKey().c_str();

        // yanrong++: CL-21199, 2010-01-19
        //bindingMessage->nameContext = agentName.c_str();
        if ( TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ENTITYNAME) )
        {
            bindingMessage->nameContext = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
        }
        else
        {
            bindingMessage->nameContext = agentName.c_str();
        }
        // ++yanrong

        bindingMessage->name = agentName.c_str();

        // Do not need to duplicate as it has already been duplicated
        bindingMessage->originator = originator;

        // Create Structured Event, inject Any data
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= bindingMessage;

        gPopulateStructuredEventHeader( *event,
                                        messageType.getContextObj().getContextName(),
                                        agentName + TYPE_NAME_SEPARATOR + messageType.getTypeKey(),
                                        &varHeader,
                                        NULL );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent( *event, time( NULL ) + 2 );
    }


    void BindingRequestMessageSender::sendRebindNameMessage( const MessageType& messageType,
                                                             const std::string& agentName,
                                                             const CORBA::Any& data )
    {
        //
        // Collate Variable Header fields
        //
        FilterData varHeader;

        //short priority = MessageTypeTableAccessor::getInstance().getPriority( messageType );
        //char priorityChar[8];
        //sprintf( priorityChar, "%d", priority );
        std::string priorityString = "0";

        NameValuePair priorityField( CosNotification::Priority, priorityString );
        varHeader.push_back( &priorityField );

        FilterData filterData;
        NameValuePair agentNameField(AGENTNAME_FILTER_NAME, agentName);
        filterData.push_back( &agentNameField);

        // Create Structured Event, inject Any data
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body = data;

        gPopulateStructuredEventHeader( *event,
                                        messageType.getContextObj().getContextName(),
                                        agentName + TYPE_NAME_SEPARATOR + messageType.getTypeKey(),
                                        &varHeader,
                                        &filterData );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
    }


}; // TA_Base_Core

