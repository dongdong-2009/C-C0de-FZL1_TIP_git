/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h $
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
            OPERATOR,
            LOCATION,
            SCADA_DISPLAY,
            PROFILE,
			PROFILE_GROUP,
            SECURITY,
            APPLICATION,
            ACTION_GROUP,
            SYSTEM_CONTROLLER_GROUP,
            SYSTEM_CONTROLLER,
            ALARM_PLAN_ASSOCIATION,
            ALARMTYPE,
            ALARM_SEVERITY,
            MESSAGETYPE,
            SUBSYSTEM,
            ALARM_RULE,
            ECS_ZONE,
            ECS_STATION_MODE,
            ECS_MASTER_MODE,
			ECS_STATION_COLOUR,
            PA_ZONE,
            PA_ZONE_GROUP,
            PA_DVA_MESSAGE,
            PA_TRAIN_DVA_MESSAGE,
            PA_DVA_VERSION,
            PA_ATS_TRIGGERED_BROADCAST,
            PA_FAS_TRIGGERED_BROADCAST,
            PA_STATION,
			ATS_BRANCH_TRACK,
			ATS_FIXED_BLOCK_MAP,
			ATS_MSS_ZONE,
			ATS_PLATFORM,
			ATS_POWER_ENTITY_MAP,
			ATS_PSD_ENTITY_MAP,
			ATS_TWP_ENTITY_MAP,
            MMS_SCHEDULE,
            VI_STATION,
            VI_TRIGGERING_EVENTS,
			TREND,
            SOUND_FILE,
			RADIO_ITSI_GROUP,
            EVENT_PLAN_ASSOCIATION,
            STIS_PID_GROUP,
            TIS_RATIS_TAG,
            TIS_RATIS_DESTINATION,
			EVENT_COLOUR,
			INCIDENT_RIGHTS, //TD15203 
            OPC_TAG_ALIAS,  //Mintao ++ TD15114 
			TR_TPISMESSAGE,
			TR_TRAIN,
			TR_TALKGROUP,
			NOT_APPLICABLE  //added by Jinhua for TrianRadioGroupConfigPlugin 030408
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
         * in the structure passed in. This will set up the message senders and perform everything
         * automatically. Only if more control is required for sending messages should the methods
         * below be used.
         *
         * @param const UpdateDetails& - This is all the details that are required to generate the
         *                               messages.
         */
        void sendMessagesForTheChange(const UpdateDetails& updateDetails);


        /**
         * sendAuditMessagesForTheChange
         *
         * This method will set up and just send audit messages for the specified changes.
         *
         * NOTE: This method should only be used if the config and audit messages that need to be sent
         *       are a little different to the usual. If possible the method sendMessagesForTheChange
         *       should be used instead.
         *
         * @param const UpdateDetails& - This is all the details that are required to generate the
         *                               messages.
         */
        void sendAuditMessagesForTheChange(const UpdateDetails& updateDetails);


        /**
         * sendOnlineUpdateMessagesForTheChange
         *
         * This method will set up and just send online update messages for the specified changes.
         *
         * NOTE: This method should only be used if the config and audit messages that need to be sent
         *       are a little different to the usual. If possible the method sendMessagesForTheChange
         *       should be used instead.
         *
         * @param const UpdateDetails& - This is all the details that are required to generate the
         *                               messages.
         */
        void sendOnlineUpdateMessagesForTheChange(const UpdateDetails& updateDetails);


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
         *
         * NOTE: This method should only be called if the method sendMessagesForChange cannot 
         */
        void setUpMessageSenders();


        /**
         * sendAuditMessages
         *
         * This sets up and sends audit messages for the changes made
         *
         * @param const UpdateDetails& - This is all the details that are required to generate the message
         */
        void sendAuditMessages(const UpdateDetails& updateDetails);


        /**
         * sendOnlineUpdateMessage
         *
         * This sets up and sends an online update message for the changes made
         *
         * @param const UpdateDetails& - This is all the details that are required to generate the message
         */
        void sendOnlineUpdateMessage(const UpdateDetails& updateDetails);


        // This is the column size of the DETAILS field in the EVENT table
        static const int EVENT_DETAILS_FIELD_SIZE;


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
