/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class sends both audit and configuration update messages. This should be used by all DLLs
  * to ensure that the sending is consistent and can be changed easily.
  *
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/config_plugin_helper/src/StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "core\message\src\MessagePublicationManager.h"
#include "core\message\src\ConnectionChecker.h"
#include "core/message/src/PublicationManager.h"
#include "core\message\src\AuditMessageSender.h"
#include "core\message\src\ConfigUpdateMessageSender.h"
#include "core\message\types\ConfigUpdate_MessageTypes.h"
#include "core\message\types\Configuration_MessageTypes.h"
#include "core\message\src\TransactiveMessageSubscription.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{

    // This is the column size of the DETAILS field in the EVENT table
    const int MessageSender::EVENT_DETAILS_FIELD_SIZE = 1024;

    MessageSender::MessageSender() : m_auditMessageSender(NULL),
                                     m_configMessageSender(NULL),
                                     m_configurationEditorEntityKey(0)
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }


    MessageSender::~MessageSender()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            bool needToCleanUpSuppliers = false;

            if (m_auditMessageSender != NULL)
            {
                delete m_auditMessageSender;
                m_auditMessageSender = NULL;
                needToCleanUpSuppliers = true;
            }

            if (m_configMessageSender != NULL)
            {
                delete m_configMessageSender;
                m_configMessageSender = NULL;
                needToCleanUpSuppliers = true;
            }

            if ( needToCleanUpSuppliers )
            {
                TA_Base_Core::gDestroySubscriptionManager();
                TA_Base_Core::gTerminateAndDeleteStructuredEventSuppliers();
    			TA_Base_Core::ChannelLocatorConnectionMgr::removeInstance();
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void MessageSender::sendMessagesForTheChange(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendMessagesForTheChange");

        setUpMessageSenders();

        if (updateDetails.isNewItem && updateDetails.isDelete)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This is a new item being deleted so it does not need any messages to be sent");
            FUNCTION_EXIT;
            return;
        }

        if( (m_auditMessageSender == NULL) || (m_configMessageSender == NULL) )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Audit or update messages cannot be sent as we are not connected to the system");
            FUNCTION_EXIT;
            return;
        }

        sendAuditMessages( updateDetails );

        sendOnlineUpdateMessage( updateDetails );

        FUNCTION_EXIT;
    }


    void MessageSender::sendAuditMessagesForTheChange(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendAuditMessagesForTheChange");

        setUpMessageSenders();

        if (updateDetails.isNewItem && updateDetails.isDelete)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This is a new item being deleted so it does not need any messages to be sent");
            FUNCTION_EXIT;
            return;
        }

        if( (m_auditMessageSender == NULL) || (m_configMessageSender == NULL) )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Audit or update messages cannot be sent as we are not connected to the system");
            FUNCTION_EXIT;
            return;
        }

        sendAuditMessages( updateDetails );

        FUNCTION_EXIT;
    }



    void MessageSender::sendOnlineUpdateMessagesForTheChange(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendOnlineUpdateMessagesForTheChange");

        setUpMessageSenders();

        if (updateDetails.isNewItem && updateDetails.isDelete)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This is a new item being deleted so it does not need any messages to be sent");
            FUNCTION_EXIT;
            return;
        }

        if( (m_auditMessageSender == NULL) || (m_configMessageSender == NULL) )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Audit or update messages cannot be sent as we are not connected to the system");
            FUNCTION_EXIT;
            return;
        }

        sendOnlineUpdateMessage( updateDetails );

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////
    // Private Helper Methods
    /////////////////////////////////////////////

    void MessageSender::setUpMessageSenders()
    {
        FUNCTION_ENTRY("setUpMessageSender");

        using TA_Base_Core::MessagePublicationManager;

        // If we do not have a sessionId then we won't be able to set up message senders since we are
        // not connected to the system. The user is working offline.

        if( (m_auditMessageSender == NULL) && (!RunParams::getInstance().get(RPARAM_SESSIONID).empty()) )
        {
            // We haven't set up the audit message sender yet and we need to
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Setting up the audit message sender for the first time");

            m_auditMessageSender = MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::Configuration::Context );

            TA_ASSERT(m_auditMessageSender != NULL, "The audit message sender received is NULL");
        }

        if( (m_configMessageSender == NULL) && (!RunParams::getInstance().get(RPARAM_SESSIONID).empty()) )
        {
            // We haven't set up the audit message sender yet and we need to
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Setting up the config update message sender for the first time");

            m_configMessageSender = MessagePublicationManager::getInstance().getConfigUpdateMessageSender( TA_Base_Core::ConfigUpdate::Context );

            TA_ASSERT(m_configMessageSender != NULL, "The config update message sender received is NULL");
        }

        if (m_configurationEditorEntityKey == 0)
        {
            std::istringstream pkey;
            pkey.str( RunParams::getInstance().get(RPARAM_CONFIGEDITOR_PKEY.c_str()) );
            pkey >> m_configurationEditorEntityKey;
        }

        FUNCTION_EXIT;
    }


    void MessageSender::sendAuditMessages(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendAuditMessages");

        TA_ASSERT(m_auditMessageSender != NULL, "The audit message sender cannot be NULL");

        // Set up the description
        TA_Base_Core::DescriptionParameters description;
        TA_Base_Core::NameValuePair component("ConfigurationItemType", RunParams::getInstance().get(RPARAM_COMPONENT.c_str()));
        TA_Base_Core::NameValuePair item("Item", updateDetails.nameOfChangedItem);
        TA_Base_Core::NameValuePair attribute("Attribute", ""); // If the position of this changes in the vector then
                                                            // the code below much change. At the moment it is
                                                            // assumed to be in position 2 in the vector.

        description.push_back( &component );
        description.push_back( &item );
        description.push_back( &attribute );

        std::string details("");
        if (updateDetails.isNewItem)
        {
            // This is a new item so we need to send an ItemAdded audit message.
            m_auditMessageSender->sendAuditMessage(TA_Base_Core::Configuration::ConfigurationItemAdded,
                                                   m_configurationEditorEntityKey,
                                                   description,
                                                   details,
                                                   TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
                                                   "","","");
        }

        if (updateDetails.isDelete)
        {
            // This is an existing item being deleted
            m_auditMessageSender->sendAuditMessage(TA_Base_Core::Configuration::ConfigurationItemDeleted,
                                                   m_configurationEditorEntityKey,
                                                   description,
                                                   details,
                                                   TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
                                                   "","","");
        }

        if (!updateDetails.isNewItem && !updateDetails.isDelete)
        {
            // Since this is an update we need to record all the changes that were made. So lets
            // step through the vector of changes and generate messages for each change.

            for(TA_Base_Core::ItemChanges::const_iterator iter = updateDetails.changes.begin();
                iter != updateDetails.changes.end();
                ++iter)
            {
                TA_Base_Core::NameValuePair attribute("Attribute", iter->first);
                // Attribute is the 3rd item added to the list
                description[2] = &attribute;

                details = iter->first;
                details += " changed from '";
                details += iter->second.oldValue;
                details += "' to '";
                details += iter->second.newValue;
                details += "'";

				// TD14029 azenith++
				// create a new string to hold the parsed string
				std::string escapedString = details;
				std::string::size_type pos = escapedString.find_first_of("'");
				while (std::string::npos != pos)
				{
					char charToInsert = escapedString.at(pos);
					escapedString.insert(pos,&charToInsert, 1);
					pos = escapedString.find_first_of("'", pos + 2);
				}
				details = escapedString;
				// TD14029 azenith++

                
                // If the details field is too long to fit in the database we will just have to truncate it
                if (details.length() > EVENT_DETAILS_FIELD_SIZE)
                {
                    details = details.substr(0,EVENT_DETAILS_FIELD_SIZE);
                }

                m_auditMessageSender->sendAuditMessage(TA_Base_Core::Configuration::ConfigurationItemUpdated,
                                                       m_configurationEditorEntityKey,
                                                       description,
                                                       details,
                                                       TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
                                                       "","","");
            }

        }

        FUNCTION_EXIT;
    }


    void MessageSender::sendOnlineUpdateMessage(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendAuditMessage");

        TA_ASSERT(m_configMessageSender != NULL, "The config online update message sender cannot be NULL");

        TA_Base_Core::EModificationType configUpdateType;

        if (updateDetails.isNewItem)
        {
            // This is a new item so we need to send config update Create message
            configUpdateType = TA_Base_Core::Create;
        }


        if (updateDetails.isDelete)
        {
            // This is an existing item being deleted
            configUpdateType = TA_Base_Core::Delete;
        }

        if (!updateDetails.isNewItem && !updateDetails.isDelete)
        {
            // This is an update of an existing item so we just need to send an update for it
            configUpdateType = TA_Base_Core::Update;
        }


        // Now we can send the online update message

        // First we iterate through and build up a list of changes. We are only interested in
        // which attributes have changed, not the values they have changed from and to.
        std::vector< std::string > changes;

        for(TA_Base_Core::ItemChanges::const_iterator iter = updateDetails.changes.begin();
            iter != updateDetails.changes.end();
            ++iter)
        {
            changes.push_back( iter->first );
        }

        // Now determine what sort of update we have
        switch (updateDetails.configUpdateMessage)
        {
            case(ENTITY):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigEntityOwner,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(OPERATOR):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigOperator,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(LOCATION):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigLocation,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(SCADA_DISPLAY):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigScadaDisplay,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(PROFILE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigProfile,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(PROFILE_GROUP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigProfileGroup,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(SECURITY):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigSecurity,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(APPLICATION):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigApplication,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(ACTION_GROUP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigActionGroup,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(SYSTEM_CONTROLLER_GROUP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigSystemControllerGroup,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(SYSTEM_CONTROLLER):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigSystemController,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(ALARM_PLAN_ASSOCIATION):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAlarmPlanAssociation,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(ALARMTYPE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAlarmType,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(ALARM_SEVERITY):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAlarmSeverity,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(MESSAGETYPE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigMessageType,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(SUBSYSTEM):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigSubsystem,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;

            case(ALARM_RULE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAlarmRule,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(PA_ZONE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPaZone,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(PA_ZONE_GROUP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPaZoneGroup,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(PA_DVA_MESSAGE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPaDvaMessage,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(PA_TRAIN_DVA_MESSAGE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPaTrainDvaMessage,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(PA_DVA_VERSION):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPaDvaVersion,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(PA_ATS_TRIGGERED_BROADCAST):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPaAtsTriggeredBroadcast,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
            case(PA_FAS_TRIGGERED_BROADCAST):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPaFasTriggeredBroadcast,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(PA_STATION):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPaStation,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(ATS_BRANCH_TRACK):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAtsBranchTrack,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(ATS_FIXED_BLOCK_MAP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAtsFixedBlockMap,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(ATS_MSS_ZONE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAtsMssZone,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(ATS_PLATFORM):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAtsPlatform,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(ATS_POWER_ENTITY_MAP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAtsPowerEntityMap,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(ATS_PSD_ENTITY_MAP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAtsPsdEntityMap,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(ATS_TWP_ENTITY_MAP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAtsTwpEntityMap,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(MMS_SCHEDULE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigMmsSchedule,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(VI_STATION):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigVideoStation,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(VI_TRIGGERING_EVENTS):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigVideoTriggeringEvent,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(SOUND_FILE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigSoundFile,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(RADIO_ITSI_GROUP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigITSIGroup,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(EVENT_PLAN_ASSOCIATION):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigEventPlanAssociation,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(STIS_PID_GROUP):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigPidGroupUpdate,
                                                               updateDetails.keyOfChangedItem,
                                                               configUpdateType,
                                                               changes);
                break;
            case(TIS_RATIS_TAG):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigTisRatisTag,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(TIS_RATIS_DESTINATION):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigTisRatisDestination,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
            case(EVENT_COLOUR):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigEventColour,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
				break;
			//TD15203 
			case (INCIDENT_RIGHTS):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigIncidentRights,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;

			//TD15203 
			case (OPC_TAG_ALIAS):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigOPCTagAlias,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
			//TD12344 - marc_bugfix_start
			//Support following entities to prevent execution crash
			case (ECS_MASTER_MODE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigEcMasterMode,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
			//TD12344 - marc_bugfix_end
			//TD17827 - marc_bugfix_start
			//Support following entities to prevent execution crash
			case (ECS_STATION_MODE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigEcStationMode,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
			case (ECS_STATION_COLOUR):
			//For now, this object type will not send any update messages
			//Add code here if this is supported in the future
				break;
			//TD17827 - marc_bugfix_end
			case (ECS_ZONE):
                m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigEcZone,
                                                                updateDetails.keyOfChangedItem,
                                                                configUpdateType,
                                                                changes);
                break;
			case(NOT_APPLICABLE):
				//added by Jinhua 030408
				//for now, this type will not send any update messages
				//add code here if this is supported in the future
				//do nothing
				break;
            default:
                TA_ASSERT(false, "Unknown ConfigUpdateType. Was a new one added recently?");
        }

        FUNCTION_EXIT;
    }

}
