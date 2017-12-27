/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/src/MessageSender.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class sends both audit and configuration update messages. This should be used by all DLLs
  * to ensure that the sending is consistent and can be changed easily.
  * 
  */

#if !defined(MessageSender_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define MessageSender_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <vector>
#include <string>

#include "core/message/types/MessageTypes.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class AuditMessageSender;
    class ConfigUpdateMessageSender;
}


namespace TA_Base_App
{
    class MessageSender
    {

    public:

        /**
         * Constructor
         */
        MessageSender();


        /**
         * Destructor
         */
        virtual ~MessageSender();

        enum ConfigUpdateType
        {
            ENTITY,
            OPERATOR
        };

        struct UpdateDetails
        {
            bool isNewItem;                            // This indicates if the item is a new one
            bool isDelete;                             // This indicates if the item was deleted
            unsigned long keyOfChangedItem;            // This is the key of the item that changed
            std::string nameOfChangedItem;             // This is the name of the item that changed
            ConfigUpdateType configUpdateMessage;      // This is which config update we should send (ie Entity, Operator etc)
            TA_Base_Core::ItemChanges changes;              // This is a vector of all changes made.
        };


        /**
         * sendMessagesForTheChange
         *
         * This will send an audit message and an online update message for the details specified
         * in the structure passed in.
         *
         * @param const UpdateDetails& - This is all the details that are required to generate the
         *                               messages.
         */
        void sendMessagesForTheChange(const UpdateDetails& updateDetails);


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        MessageSender( const MessageSender& theMessageSender){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        MessageSender& operator=(const MessageSender& pm);


        /**
         * setUpMessageSender
         *
         * This is called to set up the message senders. If they have already been set up then this
         * will return and not do anything.
         */
        void setUpMessageSenders();


        /**
         * sendAuditMessage
         *
         * This sets up and sends an audit message for the changes made
         *
         * @param const UpdateDetails& - This is all the details that are required to generate the message
         */
        void sendAuditMessage(const UpdateDetails& updateDetails);


        /**
         * sendOnlineUpdateMessage
         *
         * This sets up and sends an online update message for the changes made
         *
         * @param const UpdateDetails& - This is all the details that are required to generate the message
         */
        void sendOnlineUpdateMessage(const UpdateDetails& updateDetails);


        // This is used to send audit messages indicating changes to the configuration data.
        TA_Base_Core::AuditMessageSender* m_auditMessageSender;


        // This is used to send configuration update messages indicating changes to the configuration data.
        TA_Base_Core::ConfigUpdateMessageSender* m_configMessageSender;

        // This is the pkey of the ConfigurationEditor entity passed on the command line so we can log
        // our audit messages under it.
        unsigned long m_configurationEditorEntityKey;
    };
}

#endif // !defined(MessageSender_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
