#if !defined(MessagePublicationManager_10E8C802_1C9D_4ebb_83B1_9399C70F46CA__INCLUDED_)
#define MessagePublicationManager_10E8C802_1C9D_4ebb_83B1_9399C70F46CA__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/MessagePublicationManager.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * <description>
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/types/MessageTypes.h"
#include "core/message/src/PublicationManager.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class CommsMessageSender;
	class AuditMessageSender;
	class ConfigUpdateMessageSender;
	class StateUpdateMessageSender;
	class BindingRequestMessageSender;
    class StructuredEventSupplier;
};


namespace TA_Base_Core
{
    // Creates a message sender of type SenderType (eg. CommsMessageSender) which will publish
    // messages on the specified channel.  The caller must delete the message sender.
    template <class SenderType>
	SenderType* gCreateMessageSender( const std::string& channel )
	{
		StructuredEventSupplier* supplier = NULL;
		SenderType* sender = NULL;
		try
		{
			supplier = gGetStructuredEventSupplierForChannel( channel );
			sender =  new SenderType( supplier );
			return sender;
		}
		catch( ... )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Failed to create message sender" );

			delete sender;

			throw;
		}
	}

    // TODO LPP: Almost OBSOLETE (kept for backward compatibility)
    // gCreateMessageSender<SenderType>(channel) should probably be used instead.  However, need to consider
    // whether it is useful knowing the message context (since this might allow information about available
    // message types to be sent to consumers).

    // TODO LPP: Remove the getXMessageSender(const MessageType&) overloads. 

	class MessagePublicationManager
	{
	public:

		/**
		* ~MessagePublicationManager()
		*
		* Destructor
		*/
		virtual ~MessagePublicationManager();

		/**
		* getInstance()
		*
		* Singleton access to the MessagePublicationManager
		*
		*/
		static MessagePublicationManager& getInstance();

        // Each of the following methods obtains a new instance of an XMessageSender.
        // The caller is responsible for deleting the sender.
        //
		// @param 	messageContext/Type		specifies the channel the message will be sent on.
		// @return	a new CommsMessageSender

	    // IMPORTANT NOTE: The MessageContext/MessageType argument in each of the following methods 
        // is ONLY used to specify which channel the message is to be sent on.  The types of messages
        // that can be sent by the sender is NOT restricted.  For example, a CommsMessageSender
        // created for some specific type of Comms message will be able to send ANY type of
        // Comms message.  The sender can even specify a MessageType intended for non-Comms messages
        // (eg. Audit messages) - this would also cause the Comms message to be sent on a different 
        // channel.

		CommsMessageSender* getCommsMessageSender( const MessageContext& messageContext )const ;
		CommsMessageSender* getCommsMessageSender( const MessageType& messageType )const ;

		AuditMessageSender* getAuditMessageSender( const MessageContext& messageContext )const ;
		AuditMessageSender* getAuditMessageSender( const MessageType& messageType )const ;

		ConfigUpdateMessageSender* getConfigUpdateMessageSender( const MessageContext& messageContext )const ;
		ConfigUpdateMessageSender* getConfigUpdateMessageSender( const MessageType& messageType )const ;

		StateUpdateMessageSender* getStateUpdateMessageSender( const MessageContext& messageContext )const ;
		StateUpdateMessageSender* getStateUpdateMessageSender( const MessageType& messageType )const ;

		BindingRequestMessageSender* getBindingRequestMessageSender( const MessageContext& messageContext )const ;
		BindingRequestMessageSender* getBindingRequestMessageSender( const MessageType& messageType ) const ;

        bool getEnableSubChangeUpdates(); // limin++ CL-21308
        void setEnableSubChangeUpdates( bool enable = true ); // limin++ CL-21308
	private:

		MessagePublicationManager(){};
		MessagePublicationManager(const MessagePublicationManager &);
		MessagePublicationManager & operator = (const MessagePublicationManager &);
		
		/**
		* the one and only classInstance
		*/
		static MessagePublicationManager* m_theClassInstance;

		/**
		* protect singleton creation
		*/
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
        static bool m_enableSubChangeUpdates; // limin++ CL-21308
	};


}; // TA_Base_Core

#endif // !defined(MessagePublicationManager_10E8C802_1C9D_4ebb_83B1_9399C70F46CA__INCLUDED_)
