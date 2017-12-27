#if !defined(ItaAuditing_D39D85E5_A49B_4e10_98BE_760FC823F98D__INCLUDED_)
#define ItaAuditing_D39D85E5_A49B_4e10_98BE_760FC823F98D__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/public_interfaces/ItaAuditing.h $
  * @author:  Sean Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Interface to allow the outsourcer to access TransActive audit message
  * functionality.
  */

#include <string>
#include <sys/timeb.h>

#include "core/types/public_types/TypeDefs.h"
#include "core/types/public_types/Constants.h"

namespace TA_Base_Core
{
    class MessageType;       // Forward declaration. Found in core/message/types/MessageTypes.h

    class ItaAuditing
    {

    public:
	    /**
         * submitAuditMessage
         *
	     * Submits a basic TransActive audit message.
         *
	     * @param messageType    Static message type instance as defined by the headers
	     *                       under "core\message\types"
	     * @param descriptionParameters    Collection of of string name-value pairs.  Each
	     *                                 audit message description parameter should be described
         *                                 by a name-value pair.
         *                                 Example - for the IasInvalidPacketData message type, the
         *                                 configured description is defined as "Invalid packet received
         *                                 from [socket].  [detail]".  The client code should call this
         *                                 method with a map of name-value pairs containing
         *                                 ("socket", "iassvr-1:8888"), ("detail", "Message too short").
	     * 
         * @return string The unique id generated for the new audit message
	     */
        virtual std::string submitAuditMessage(const MessageType& messageType,
                                               const NameValuePairs& descriptionParameters) =0;

        
        /**
         * submitAdvancedAuditMessage
         *
	     * Submits a TransActive audit message to the database.
         *
	     * @param messageType    Static message type instance as defined by the headers
	     *                       under "core\message\types"
	     * @param entityKey    The entity to submit the audit message for.  The
	     *                     application entity can be obtained through ItaRuntime.
	     * @param descriptionParameters    Collection of of string name-value pairs.  Each
	     *                                 audit message description parameter should be described
         *                                 by a name-value pair.
         *                                 Example - for the IasInvalidPacketData message type, the
         *                                 configured description is defined as "Invalid packet received
         *                                 from [socket].  [detail]".  The client code should call this
         *                                 method with a map of name-value pairs containing
         *                                 ("socket", "iassvr-1:8888"), ("detail", "Message too short").
	     * @param details    Additional details field.
	     * @param sessionID    The operator session ID, if the action has been triggered
	     *                     by an operator.
	     * @param assetName    The asset name to indicate the associated datapoint.
	     * @param timeStamp    A specific time for the event
	     * 
         * @return string The unique id generated for the new audit message
	     */
	    virtual std::string submitAdvancedAuditMessage(const MessageType& messageType,
                                                       unsigned long entityKey,
                                                       const NameValuePairs& descriptionParameters,
                                                       const std::string& details = "",
                                                       const std::string& sessionID = "",
                                                       const std::string& assetName = "",
                                                       const timeb& timeStamp = defaultTime) =0;


        virtual ~ItaAuditing() { };
    };
}
#endif // !defined(ItaAuditing_D39D85E5_A49B_4e10_98BE_760FC823F98D__INCLUDED_)
