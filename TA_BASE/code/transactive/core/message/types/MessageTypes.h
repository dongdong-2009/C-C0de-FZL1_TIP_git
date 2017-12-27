#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/types/MessageTypes.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file is included by everything that sends messages.
  * It is really just a way of defining the 
  * MessageContext and MessageType.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <vector>
#include <string>

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    // BACKWARDS COMPATIBILITY
    // EMessageCategory is kept so that MessageContext ctors don't need to be changed.
    enum EMessageCategory
    {  
        OBSOLETE_Category = 0,		// Use in place of a specific category for new MessageContext ctor calls.
        CommsMessage = 0,
        StateUpdateMessage,
        AuditMessage,
        ConfigUpdateMessage,
        BindingRequestMessage,
        AlarmMessage
    };

    // A message context is a grouping of message types, for the convenience of clients
    // that need to subscribe to particular sets of message types (ie. rather than having
    // to subscribe to multiple types, a single context can be subscribed to).

    // Normally we would expect clients to specify logical groupings if that was helpful
    // for them, but in this design the MessageType knows which context it is associated
    // with, and registers it's own name with that particular MessageContext.  One 
    // limitation is that a message type can only be associated with one context.

    // Each context is associated with a single corba event channel (convention dictates 
    // that messages are to be sent on a particular channel, although it would be possible to 
    // send any event on any channel).  The name of the event channel is a property of
    // the MessageContext.

    // TODO LPP: A better name might be MessageTypeGroup, since a context seems to represent
    // a grouping of message types.

    class MessageContext
    {
        public:
            explicit MessageContext( const char* channel, const char* context, EMessageCategory notUsed )
            :
            m_contextName(context),
            m_channelName(channel)
			{
				TA_ASSERT(!getChannelName().empty(), "Initialising MessageContext with blank channel");
				TA_ASSERT(!getContextName().empty(), "Initialising MessageContext with blank context");
			}

			~MessageContext(){};
           
            /**
            * getContext
            *
            * Get the name of the context
            *
            * @return   std::string   the channel
            */
            std::string getContextName() const
            {
                return m_contextName;
            }

            /**
            * getChannel
            *
            * Get the channel this MessageContext uses
            *
            * @return   std::string   the channel
            */
            std::string getChannelName() const
            {
                return m_channelName;
            }

			/**
            * getTypes
            *
            * Return the set of message type names that have been associated with this context
            * (by calling addType).
            *
            * @return std::vector<std::string> all the types
            */
            const std::vector<std::string>& getTypes() const
			{
				return m_messageTypeNames;
			}

			/**
			* addTypeKey
			*
			* This is used by each MessageType to add it's name to the set of type names in the 
            * context.  addType() can be called with the same type name multiple times and the
            * name will only be added once.
			*
			* @param	typeKey	the type to add to the list
			*/
			void addTypeKey(const std::string& typeKey);
            
			/**
			* clearAllTypes
			*
			* removes the list of typeKeys
			*/
			void clearAllTypes()
			{
				m_messageTypeNames.clear();
			}
            
        private:
		    /**
	    	* disable the default copy constructor
	    	*/
			MessageContext();
	    	MessageContext( const MessageContext& theMessageContext);
			MessageContext& operator = ( const MessageContext& theMessageContext);

            std::string m_contextName;
            std::string m_channelName;

			std::vector<std::string> m_messageTypeNames;
    };

    
    // The MessageType is a type code that can be used to distinguish between messages
    // having the same Corba type.  For example, all types of CommsMessage have the same
    // Corba type, CommsMessageCorbaDef, but need to be distinguished somehow so that
    // the messageState member (a Corba Any) can be interpreted correctly.

    // By convention, a given MessageType typeKey is only used for a single type of Corba
    // message.  This allows receivers to use the MessageType to determine which Corba 
    // type the message body (which is a Corba Any) should be cast to.  Note that the
    // framework does NOT restrict which MessageTypes can be used with a particular 
    // type of Corba message.  So senders must be careful to specify the appropriate
    // MessageType, otherwise receivers that cast based on MessageType may fail. 

    class MessageType
    {
        public:

            // 'context' is the group that this message type is to be associated with, and must
            // remain valid for the lifetime of the MessageType object.
            // 'typeKey' is the name of the message type.  By convention a numeric value is used (eg. "40035"). 
            explicit  MessageType(MessageContext& context, const char* typeKey)
            :
            m_context(context),    // remember m_context is a reference
            m_typeKey(typeKey)
			{
				TA_ASSERT(!getTypeKey().empty(), "Type key parameter is empty");

				// Add the type to the set of types associated with the specified context.
				 context.addTypeKey( getTypeKey() );
			}
            
			~MessageType() {}

            // TODO LPP: Rename to getContext() once MessageContext::getContext() has been renamed.

            /**
            * getContextObj
            *
            * Get the context that this message type has been associated with.
            *
            * @return   std::string   the context
            */
            const MessageContext& getContextObj() const
            {
                return m_context;
            }

            /**
            * getTypeKey
            *
            * Get the unique type key of this MessageType 
            *
            * @return   std::string   the type name
            */
            std::string getTypeKey() const
            {
                return m_typeKey;
            }

        private:
		    /**
	    	* disable the default copy constructor
	    	*/
			MessageType();
	    	MessageType( const MessageType& theMessageType);
			MessageType& operator = ( const MessageType& theMessageType);

            const MessageContext& m_context;            
            std::string m_typeKey;

    };
    

}; // TA_Base_Core

#endif // !defined(AbstractMessageSender_323E7A23_1CF3_4e34_A2AE_B9382DC8390B__INCLUDED_)
