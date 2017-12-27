/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/configuration_updates/src/ConfigMessageSubscriber.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class listens for and sends config messages 
  */


#if !defined(ConfigMessageSubscriber_h)
#define ConfigMessageSubscriber_h

#include "core/configuration_updates/src/MessageObjectTypeMap.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class OnlineUpdateListener;
    class MessageObjectTypeMap;
    class ConfigUpdateDetails;
    class ConfigUpdateMessageSender;

    class ConfigMessageSubscriber 
        : public SpecialisedMessageSubscriber<ConfigUpdateMessageCorbaDef>
    {
    public:
        ConfigMessageSubscriber()
            :m_sender(NULL)
        {
        }


        virtual ~ConfigMessageSubscriber();


        /**
          * receiveSpecialisedMessage
          *
          * Overriden to receive specialised message
          * 
          * @param ConfigUpdateMessageCorbaDef& - The narrowed Message
          */
        virtual void receiveSpecialisedMessage(const ConfigUpdateMessageCorbaDef& message);


        /**
          * subscribe
          *
          * Start listening for config update messages
          */
        void subscribe();


        /**
          * unsubscribe
          *
          * Stop listening for config update messages
          */
        void unsubscribe();


        /**
          * isSubscribing
          * 
          * Returns true if currently subscribing for update messages,
          * false otherwise.  Used for testing purposes.
          *
          * @return true if currently subscribing for update messages,
          * false otherwise.
          */
        bool isSubscribing() const;


        /**
          * regenerateConfigUpdateMessage
          * 
          * This method has to be called after the callback on IOnlineUpdatable
          * of ENTITY_OWNER object type to regenerate a new message for the 
          * entity users.
          * 
          * @param message the ConfigUpdateDetail object that holds the
          *                modification type.
          */
        void generateEntityUpdateMessage(ConfigUpdateDetails& update);


    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        ConfigMessageSubscriber( const ConfigMessageSubscriber& subscriber){ };
        ConfigMessageSubscriber& operator=(const ConfigMessageSubscriber &) { };


        //////////////////////////////
        //
        // Attributes
        //
        //////////////////////////////

        // for resending config message
        ConfigUpdateMessageSender* m_sender;

	    // protect m_sender creation
	    mutable TA_Base_Core::ReEntrantThreadLockable m_senderLock;

        // the map that maps update messages to object types
        const MessageObjectTypeMap m_messageObjectTypeMap;
    };
}

#endif // !defined(ConfigMessageSubscriber_h)
