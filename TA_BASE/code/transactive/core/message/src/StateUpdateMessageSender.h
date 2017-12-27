#if !defined(StateUpdateMessageSender_D9D2CCC8_4C85_47f7_9028_A2F035FCC20E__INCLUDED_)
#define StateUpdateMessageSender_D9D2CCC8_4C85_47f7_9028_A2F035FCC20E__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/StateUpdateMessageSender.h $
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

#include <vector>
#include <string>
#include "omniORB4/CORBA.h"
#include "core/message/src/NameValuePair.h"

namespace TA_Base_Core
{
    class StructuredEventSupplier;
    class MessageType;

    // StateUpdateMessage header:

    // domain_name = message context name
    // type_name = <agentName>_<messageType>
    // variable_header:
    // - (CosNotification::Priority, priority), where priority is provided by the
    //   MessageTypeTableAccessor for the given message type.
    // filterable_data:
    // - (AGENTNAME_FILTER_NAME, agentName)
    // - client supplied (name, value) pairs

    class StateUpdateMessageSender
    {
    public:
        /**
        * constructor
        *
        * @param    context          context used to send messages
        * @param    eventSupplier    pointer to the StructuredEventSupplier
        *                            used to send events
        */
        StateUpdateMessageSender( StructuredEventSupplier* eventSupplier);

        /**
        * destructor
        */
        virtual ~StateUpdateMessageSender();

        /**
        * sendStateUpdateMessage
        *
        * Used to send state update information to a Monitor or Control agent instance.
        * 
        * @param    messageType      The specific type of the message being sent
        * @param    entityKey        The entity will receive this update.
        * @param    agentName        The specific name of the agent that will receive the update
        * @param    updateInfo       The update information that will update the monitor agent.
        * @param    filterableData   Any additional data a subscriber may filter upon
        *
        */
        void sendStateUpdateMessage( const MessageType& messageType,
                                     unsigned long entityKey,
                                     const std::string& agentName,
                                     const CORBA::Any& updateInfo,
                                     const FilterData* filterableData = NULL );

    private:
        // NOT CLONEABLE
        StateUpdateMessageSender( const StateUpdateMessageSender& theStateUpdateMessageSender);

        StructuredEventSupplier* m_supplier;

    };


}; // TA_Base_Core

#endif // !defined(StateUpdateMessageSender_D9D2CCC8_4C85_47f7_9028_A2F035FCC20E__INCLUDED_)
