#if !defined(CommsMessageSender_82E067A3_D2D9_479d_B2AE_137A0A923F62__INCLUDED_)
#define CommsMessageSender_82E067A3_D2D9_479d_B2AE_137A0A923F62__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/CommsMessageSender.h $
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

#include <vector>
#include <string>
#include "omniORB4/CORBA.h"
#include "core/message/src/NameValuePair.h"

namespace TA_Base_Core
{
    class StructuredEventSupplier;
    class MessageType;

    // CommsMessage header:

    // domain_name = message context name
    // type_name = <segmentedEntityKey>_<messageType>
    // variable_header:
    // - (CosNotification::Priority, priority), where priority is provided by the
    //   MessageTypeTableAccessor for the given message type.
    // filterable_data:
    // - (REGION_FILTER_NAME, location key)
    // - (MESSAGETYPE_FILTER_NAME, messageType)
    // - client supplied (name, value) pairs

    class CommsMessageSender
    {
    public:

        /**
        * constructor
        *
        * @param    eventSupplier    StructuredEventSupplier used to send events
        */
        CommsMessageSender( StructuredEventSupplier* eventSupplier );

        /**
        * destructor
        */
        virtual ~CommsMessageSender();

        /**
        * sendCommsMessage
        *
        * Send a comms message. The CORBA::Any is used to store any information.
        * The other parameters are used for extended filtering, please ensure they are
        * filled out or your subscribers may never receive events
        *
        * @param    messageTypeKey    The specific message Type as defined in the database
        * @param    entityKey         The entity this comms message contains data for/about
        * @param    data              Any information to be sent
        * @param    subsystemKey      The subsystem that this message pertains to
        *                             NOTE that subsystemKey IS NOT CURRENTLY USED.
        * @param    locationKey       The region that this message pertains to.  The special
        *                             locations LOCAL_LOCATION and ANY_LOCATION cannot be used.
        * @param    filterableData    Any additional filterable information that you wish to 
        *                             include in the event.
        */
        // TODO LPP: There seems to be a redundency with locationKey - a locationKey has 
        // already been specified for the StructuredEventSupplier, so wouldn't it be
        // reasonable to use that locationKey here?  Does it make sense to send a 
        // message from one location that 'pertains' to another location?
        void sendCommsMessage( const MessageType& messageTypeKey,
                               unsigned long entityKey,
                               const CORBA::Any& data,
                               unsigned long subsystemKey,
                               unsigned long locationKey,
                               const FilterData* filterableData = NULL );
        
        /**
        * sendBroadcastCommsMessage
        *
        * Send a broadcast comms message. The CORBA::Any is used to store any information.
        * The other parameters are used for extended filtering, please ensure they are
        * filled out or your subscribers will never receive events.
        *
        * @param    messageTypeKey    The specific message Type as defined in the database
        * @param    filterableData    Any additional filterable information that you wish to 
        *                             include in the event.
        */
        void sendBroadcastCommsMessage( const MessageType& messageTypeKey,
                                        const CORBA::Any& data,
                                        const FilterData* filterableData = NULL );

        void sendBroadcastCommsMessage( const MessageType& messageTypeKey,
                                        unsigned long entityKey,
                                        const CORBA::Any& data,
                                        const FilterData* filterableData = NULL );

    private:
        // NOT CLONEABLE
        CommsMessageSender( const CommsMessageSender& theCommsMessageSender);

        StructuredEventSupplier* m_supplier;

    };


}; // TA_Base_Core
#endif // !defined(CommsMessageSender_82E067A3_D2D9_479d_B2AE_137A0A923F62__INCLUDED_)
