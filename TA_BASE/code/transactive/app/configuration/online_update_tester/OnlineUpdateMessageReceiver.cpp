/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/online_update_tester/OnlineUpdateMessageReceiver.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "OnlineUpdateMessageReceiver.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/MessageTypeAccessFactory.h"
#include "core/data_access_interface/src/IMessageTypeData.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/data_access_interface/src/IGui.h"


#include <iostream>

OnlineUpdateMessageReceiver::OnlineUpdateMessageReceiver()
{
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToConfigUpdateMessage(TA_Base_Core::ConfigUpdate::Context, this);
}


void OnlineUpdateMessageReceiver::receiveSpecialisedMessage(const TA_Base_Core::ConfigUpdateMessageCorbaDef& message)
{
    std::string messageName("");
    try
    {
        TA_Base_Core::IMessageTypeData* messageObj = TA_Base_Core::MessageTypeAccessFactory::getInstance().getMessageTypeByKey( std::string(message.messageTypeKey), false );
        messageName = messageObj->getName();
        delete messageObj;
    }
    catch ( ... )
    {
        messageName = "Unknown";
    }

    
    std::string itemName("");
    
    if ( (messageName == "ConfigEntityOwner") || (messageName == "ConfigEntity") )
    {
        try
        {
            TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( message.assocEntityKey );
            itemName = entity->getName();
            delete entity;
        }
        catch ( ... )
        {
            itemName = "Unknown";
        }
    }
    else if (messageName == "ConfigApplication")
    {
        try
        {
            TA_Base_Core::IGui* gui = TA_Base_Core::GuiAccessFactory::getInstance().getGui( message.assocEntityKey );
            itemName = gui->getName();
            delete gui;
        }
        catch ( ... )
        {
            itemName = "Unknown";
        }
    }
    //TODO: add more conversion here if necessary.


    std::string modificationTypeName("");
    switch(message.modificationType)
    {
        case(TA_Base_Core::Update):
            modificationTypeName = "Update";
            break;

        case(TA_Base_Core::Create):
            modificationTypeName = "Create";
            break;

        case(TA_Base_Core::Delete):
            modificationTypeName = "Delete";
            break;

        default:
            modificationTypeName = "Unknown";
            break;
    }

    std::string changes("");
    int size = message.changes.length();
    for (int i=0; i < size; ++i)
    {
        changes += message.changes[i];
        changes += "\n\t                   ";
    }

    std::cout << "Online update received:";
    std::cout << "\n\tMessageType:       " << messageName;
    std::cout << "\n\tItem Name:         " << itemName;
    std::cout << "\n\tModification Type: " << modificationTypeName;
    std::cout << "\n\tChanges:           " << changes;
    std::cout << "\n\n";
}
