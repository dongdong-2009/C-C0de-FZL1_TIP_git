/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/src/MessageSender.cpp $
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

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/generic_entity/src/StdAfx.h"
#include "app/configuration/config_plugins/src/MessageSender.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "core\message\src\MessagePublicationManager.h"
#include "core\message\src\AuditMessageSender.h"
#include "core\message\src\ConfigUpdateMessageSender.h"
#include "core\message\types\ConfigUpdate_MessageTypes.h"
#include "core\message\types\Configuration_MessageTypes.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{

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
                TA_Base_Core::MessagePublicationManager::getInstance().cleanUpSuppliers();
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

        sendAuditMessage( updateDetails );

        sendOnlineUpdateMessage( updateDetails );


        FUNCTION_EXIT;
    }


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

    
    void MessageSender::sendAuditMessage(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendAuditMessage");

        TA_ASSERT(m_auditMessageSender != NULL, "The audit message sender cannot be NULL");

        // Set up the description
        TA_Base_Core::DescriptionParameters description;
        description.push_back( new TA_Base_Core::NameValuePair("ConfigurationItemType", RunParams::getInstance().get(RPARAM_COMPONENT.c_str())) );
        description.push_back( new TA_Base_Core::NameValuePair("Item", updateDetails.nameOfChangedItem) );

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
            // step through the vector of changes and generate a nice string.

            for(TA_Base_Core::ItemChanges::const_iterator iter = updateDetails.changes.begin();
                iter != updateDetails.changes.end();
                ++iter)
            { 
                details += iter->first;
                details += "('";
                details += iter->second.oldValue;
                details += "' to '";
                details += iter->second.newValue;
                details += "')  ";
            }

            m_auditMessageSender->sendAuditMessage(TA_Base_Core::Configuration::ConfigurationItemUpdated,
							                       m_configurationEditorEntityKey,
							                       description,
                                                   details,
                                                   TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
                                                   "","","");
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
        }

        FUNCTION_EXIT;
    }

}
