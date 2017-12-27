/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/NotifyEventRequest.h $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	NotifyEventRequest represents a request to the NotifyEventProcessor
  * to generate and log a message to the eventlog
  *
  *
  */

#ifndef NOTIFYEVENTREQUEST_H
#define NOTIFYEVENTREQUEST_H

#include <string>
#include "core/message/types/MessageTypes.h"


namespace TA_Base_Bus
{

	class NotifyEventRequest
	{
	public:

		/**
		 * NotifyEventRequest::NotifyEventRequest()
		 *
		 * This constructor is for a notification event that is logged but
		 * not sent.
		 *
		 * @param entityKey			The pkey of the entity associated with this request
		 * @param messageType		The message type associated with this event
		 * @param message			The message to log to eventlog
		 *
		 */
		NotifyEventRequest( const unsigned long entityKey,
							const TA_Base_Core::MessageType & messageType,
							const std::string & message,
							const std::string & sessionID )
		: m_entityKey( entityKey ),
		  m_sessionID ( sessionID ),
		  m_message ( message ),
		  m_messageType ( 0 ),
          m_messageContext( 0 )
		{
            // The MessageType class has-a MessageContext by reference.  We make a copy of 
            // the MessageContext at the same time as we copy the message context - note 
            // that the MessageType copy references the new MessageContext copy rather 
            // than the original.
            // TODO LPP: Something isn't right with the MessageType/MessageContext design...
            m_messageContext = new TA_Base_Core::MessageContext(
                messageType.getContextObj().getChannelName().c_str(),
                messageType.getContextObj().getContextName().c_str(),
                TA_Base_Core::OBSOLETE_Category);
			m_messageType = new TA_Base_Core::MessageType ( *m_messageContext, messageType.getTypeKey().c_str() );
		}


		~NotifyEventRequest()
		{
			delete m_messageType;
			m_messageType = 0;

            delete m_messageContext;
            m_messageContext = 0;
		}

		/**
		 * NotifyEventRequest::getEntityKey()
		 *
		 * @return 	the pkey for the entity related to this event
		 *
		 */
		unsigned long getEntityKey() const
		{ 
			return m_entityKey;
		}

		/**
		 * NotifyEventRequest::getMessage()
		 *
		 * @return	the message to be written to the eventlog
		 *
		 */
		const std::string & getMessage() const 
		{ 
			return m_message;
		}

		/**
		 * NotifyEventRequest::getSessionID()
		 *
		 * @return	the message to be written to the eventlog
		 *
		 */
		const std::string & getSessionID() const 
		{ 
			return m_sessionID;
		}

		/**
		 * NotifyEventRequest::getEventType()
		 *
		 * @return	the message type associated with this notify event
		 *
		 */
		const TA_Base_Core::MessageType & getMessageType() const
		{
			return *m_messageType;
		}


	private:

		unsigned long			m_entityKey;
		std::string				m_sessionID;
		std::string 			m_message;
		TA_Base_Core::MessageType* m_messageType;
        TA_Base_Core::MessageContext* m_messageContext;
	};

}

#endif	
