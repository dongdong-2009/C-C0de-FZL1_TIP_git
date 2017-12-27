#if !defined(ConfigUpdateMessageSender_C4034523_DC83_42cd_B92E_877620CAB98F__INCLUDED_)
#define ConfigUpdateMessageSender_C4034523_DC83_42cd_B92E_877620CAB98F__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/ConfigUpdateMessageSender.h $
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

#include <vector>
#include "core/message/src/NameValuePair.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"

namespace TA_Base_Core
{
    class StructuredEventSupplier;
    class MessageType;

    // ConfigUpdateMessage header:

    // domain_name = message context name
    // type_name = message type
    // variable_header:
    // - (CosNotification::Priority, priority), where priority is provided by the
    //   MessageTypeTableAccessor for the given message type.
    // filterable_data:
    // - client supplied (name, value) pairs

    class ConfigUpdateMessageSender
    {
    public:
        /**
        * constructor
        *
        * @param    eventSupplier    StructuredEventSupplier used to send events
        */
        ConfigUpdateMessageSender( StructuredEventSupplier* eventSupplier );

        /**
        * destructor
        */
        virtual ~ConfigUpdateMessageSender();

        /**
        * sendConfigUpdateMessage
        *
        * Send a configUpdate message to any interested subscribers
        *
        * @param    messageType         The specific type of config update message being sent
        * @param    itemKey             The key of the item that has been updated.
        * @param    modificationType    One of EModificationType
        * @param    changes             The sequence of parameters/information that has changes
        * @param    filterableData      Any other information a subscriber may want to filter on
        */
        void sendConfigUpdateMessage( const MessageType& messageType,
                                      unsigned long itemKey,
                                      EModificationType modificationType,
                                      const std::vector<std::string>& changes,
                                      const FilterData* filterableData = NULL );

    private:
        // NOT CLONEABLE
        ConfigUpdateMessageSender( const ConfigUpdateMessageSender& theConfigUpdateMessageSender);

        StructuredEventSupplier* m_supplier;

    };


}; // TA_Base_Core

#endif // !defined(ConfigUpdateMessageSender_C4034523_DC83_42cd_B92E_877620CAB98F__INCLUDED_)
