#if !defined(BindingRequestMessageSender_3D0CB9F7_A905_4af4_A92F_3B8384807DD4__INCLUDED_)
#define BindingRequestMessageSender_3D0CB9F7_A905_4af4_A92F_3B8384807DD4__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/BindingRequestMessageSender.h $
  * @author:  Jade Welton
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

#include <string>
#include "omniORB4/CORBA.h"

namespace TA_Base_Core
{
    class StructuredEventSupplier;
    class MessageType;

    // BindingRequestMessage header:

    // domain_name = message context name
    // type_name = <agentName>_<messageType>
    // variable_header:
    // - (CosNotification::Priority, priority), where priority is provided by the
    //   MessageTypeTableAccessor for the given message type.
    // filterable_data:
    // - (AGENTNAME_FILTER_NAME, agentName)

    class BindingRequestMessageSender
    {
    public:

        /**
        * constructor
        *
        * @param    eventSupplier    StructuredEventSupplier used to send events
        */
        BindingRequestMessageSender( StructuredEventSupplier* eventSupplier );

        /**
        * destructor
        */
        virtual ~BindingRequestMessageSender();
        
        /**
        * sendBindingRequest
        *
        * This method is used to send a Binding Request.
        *
        * @param    messageType       The actual type of the Message Being Sent
        * @param    agentName         The actual name of the agent to retrieve
        * @param    originator           The origin of the request    
        */
        void sendBindingRequest( const MessageType& messageType,
                                 const std::string& agentName,
                                 CORBA::Object_ptr originator );
        
        /**
        * sendRebindNameMessage
        *
        * This method is used to send a Rebinding update
        *
        * @param    messageType       The actual type of the Message Being Sent
        * @param    agentName          The context that the name resides in
        * @param    data              The rebind data to send
        */
        void sendRebindNameMessage( const MessageType& messageType,
                                    const std::string& agentName,
                                    const CORBA::Any& data );

    private:
        // NOT CLONEABLE
        BindingRequestMessageSender( const BindingRequestMessageSender& theBindingRequestMessageSender);

        StructuredEventSupplier* m_supplier;

    }; // BindingRequestMessageSender


}; // TA_Base_Core

#endif // !defined(BindingRequestMessageSender_3D0CB9F7_A905_4af4_A92F_3B8384807DD4__INCLUDED_)
