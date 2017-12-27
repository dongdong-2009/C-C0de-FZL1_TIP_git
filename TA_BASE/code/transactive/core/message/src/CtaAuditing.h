#if !defined(CtaAuditing_C67F9791_C7C6_44dd_8E2E_42653BCF08FB__INCLUDED_)
#define CtaAuditing_C67F9791_C7C6_44dd_8E2E_42653BCF08FB__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/CtaAuditing.h $
  * @author:  K. Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This wrappers the audit message senders and implements the interface available to outsourcers.
  * One of these objects must be constructed for each message context or message type that needs
  * to be sent.
  */

#include <memory>

#include "core/message/public_interfaces/ItaAuditing.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class AuditMessageSender;
    class MessageContext;

    class CtaAuditing : public virtual ItaAuditing
    {
		public:
        /** 
         * Constructor
         *
         * @param   entityKey           the key of the agent or application creating this object
		 * @param 	messageContext		the message context that specifies the messages to be sent.
         */
        CtaAuditing(unsigned long entityKey, const MessageContext& messageContext);


        /** 
         * Constructor
         *
         * @param   entityKey           the key of the agent or application creating this object
		 * @param 	messageType		the message type that specifies the messages to be sent.
         */
        CtaAuditing(unsigned long entityKey, const MessageType& messageType);


        /**
         * Destructor
         */
	    virtual ~CtaAuditing();


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
        virtual std::string submitAuditMessage(const MessageType& messageType, const NameValuePairs& descriptionParameters);

        
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
                                                       const timeb& timeStamp = defaultTime);

    private:
        /**
         * Disable copy Constructor.
         */
        CtaAuditing(const CtaAuditing& rhs);


        /**
         * Disable assignment operator
         */
        const CtaAuditing& operator=(const CtaAuditing& rhs);


        // The object that will actually raise the audit message
		boost::shared_ptr<AuditMessageSender> m_messageSender;


        // The primary key of the application/agent's entity
        unsigned long m_appEntityKey;
    };
}
#endif // !defined(CtaAuditing_C67F9791_C7C6_44dd_8E2E_42653BCF08FB__INCLUDED_)
