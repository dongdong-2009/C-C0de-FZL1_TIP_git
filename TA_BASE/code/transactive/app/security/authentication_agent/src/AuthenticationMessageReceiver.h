/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/AuthenticationMessageReceiver.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class is the listener for the incoming distributed update messages.
 */

#if !defined(AUTHENTICATIONMESSAGERECEIVER_H)
#define AUTHENTICATIONMESSAGERECEIVER_H

#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include "app/security/authentication_agent/src/SessionRegister.h"

namespace TA_Base_App
{

    struct TA_Base_Bus::SessionInfoCorbaDef;

    class AuthenticationMessageReceiver : public TA_Base_Core::SpecialisedMessageSubscriber
                                        <TA_Base_Core::CommsMessageCorbaDef>
    {
    public:

        /**
         * receiveSpecialisedMessage
         *
         * This method is called when a message is received. It will place a callback
         * to the EventLoggingAgent, that will then deal with the event.
         *
         * @param message The message that was received.
         */
        void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

        /**
         * startReceiver
         *
         * This method is called to start the messageReceiver listening on the channel.
         */
        void startReceiver();

        /**
         * shutdownReceiver
         *
         * The method is called ot shutdown the reciever, and disconnect from the channel.
         */
        void shutdownReceiver();

        /**
         * constructor
         *
         * Constructs a new instance of this class
         *
         * @param reg A reference to the SessionRegister object created by the parent class. This
         * object is the repository for all session information.
         * @param messageName The name by which to register for StateUpdate messages.
         */
        AuthenticationMessageReceiver(SessionRegister& reg);

        virtual ~AuthenticationMessageReceiver();

    private:
        /**
         * processSession
         *
         * Processes the received session and makes the appropriate updates to the SessionRegister
         *
         * @param session The session that was recieved
         */
        void processSession(TA_Base_Bus::SessionInfoCorbaDef* session);

        /**
         * sendBatchStateUpdate
         *
         * Sends a batch state update event when a request is received.
         */
        void sendBatchStateUpdate();

        // The member variable to hold the pointer to the parent class. This pointer
        // should not be deleted, as it is cleaned up when the program finishes.
        SessionRegister& m_sessionRegister;

		std::auto_ptr<TA_Base_Core::CommsMessageSender> m_stateSender;

		bool m_shutdown;
    };
}

#endif // !defined(AUTHENTICATIONMESSAGERECEIVER_H)
