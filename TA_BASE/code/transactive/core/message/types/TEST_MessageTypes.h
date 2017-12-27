#ifndef  TEST_MESSAGE_TYPES
#define TEST_MESSAGE_TYPES
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/types/TEST_MessageTypes.h $
 * @author:  Cameron Rochester
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Message contexts and types for test purposes
 *
 */

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace TEST 
	{
		//
		// Test Contexts
		//
		static MessageContext Context("GeneralComms","TEST", CommsMessage);

		static MessageContext CommsContext( "GeneralComms", "COMMS", CommsMessage );
		static MessageContext StateContext( "StateUpdate", "STATE", StateUpdateMessage );
		static MessageContext AuditContext( "Audit", "AUDIT", AuditMessage );
		static MessageContext ConfigContext( "Config", "CONFIG", ConfigUpdateMessage );
		static MessageContext BindingContext( "NameBind", "BINDING", BindingRequestMessage );

		//
		// Root context for Comms Message types
//			static MessageContext CommsContext("SYSTEM","TEST", CommsMessage);
		
		// Individual Message Types
		//
		static const MessageType TestMessageType1(Context, "9998");
		static const MessageType TestMessageType2(Context, "9999");
		static const MessageType TestMessageType3(Context, "9997");
		static const MessageType TestMessageType4(Context, "9996");
		static const MessageType TestMessageType5(Context, "9995");

		static const MessageType CommsMessageType(CommsContext, "8998");
		static const MessageType StateMessageType(StateContext, "8999");
		static const MessageType AuditMessageType(AuditContext, "8997");
		static const MessageType ConfigMessageType(ConfigContext, "8996");
		static const MessageType BindingMessageType(BindingContext, "8995");
		static const MessageType CommsTestMessageType1(CommsContext, "9900");
		static const MessageType CommsTestMessageType2(CommsContext, "9901");



		// Root context for Audit Message types
//			static MessageContext AuditContext("SYSTEM","TEST", AuditMessage);
		
		// Individual Message Types
		static const MessageType AuditTestMessageType1(AuditContext, "9910");
		static const MessageType AuditTestMessageType2(AuditContext, "9911");
		

		
		// Root context for Config Update Message types
		static MessageContext ConfigUpdateContext("Config","TEST", ConfigUpdateMessage);
		
		// Individual Message Types
		static const MessageType ConfigUpdateTestMessageType1(ConfigUpdateContext, "9920");
		static const MessageType ConfigUpdateTestMessageType2(ConfigUpdateContext, "9921");



		// Root context for StateUpdate Message types
		static MessageContext StateUpdateContext("State","TEST", StateUpdateMessage);
		
		// Individual Message Types
		static const MessageType StateUpdateTestMessageType1(StateUpdateContext, "9930");
		static const MessageType StateUpdateTestMessageType2(StateUpdateContext, "9931");



		// Root context for BindingRequest Message types
		static MessageContext BindingRequestContext("Binding","TEST", BindingRequestMessage);
		
		// Individual Message Types
		static const MessageType BindingRequestTestMessageType1(BindingRequestContext, "9940");
		static const MessageType BindingRequestTestMessageType2(BindingRequestContext, "9941");

	};
};

#endif
