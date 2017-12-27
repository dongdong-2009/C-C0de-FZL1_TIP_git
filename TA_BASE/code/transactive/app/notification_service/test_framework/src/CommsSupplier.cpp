/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/CommsSupplier.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "CommsSupplier.h"
#include "Common.h"
#include "app/notification_service/test_framework/IDL/src/framework.h"
#include "core/uuid/src/TAuuid.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/utilities/src/TA_String.h"


CommsSupplierHandler::CommsSupplierHandler(unsigned long simulatorID, SimpleDb& db, bool broadcast) :
    m_simulatorID(simulatorID),
    m_db(db),
    m_broadcast(broadcast)
{
    // Relevant columns from NST_SIMULATORCONFIG:
    // srcEntityKey             - entity that sent message relates to. Not for Broadcast messages
    // srcLocationKey           - location of the supplier (this is the location that 
    //                            will be sent in the message). Not for Broadcast messages
    // msgContextName           - context that the event will be published with
    // msgTypeName              - type that the event will be published with

    // Retrieve our configuration settings
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CommsSupplierHandler::CommsSupplierHandler(): Retrieving settings");
    const int NUM_COLUMNS = 4;
    std::ostringstream sql;
    sql << "select srcEntityKey, srcLocationKey, msgContextName, msgTypeName ";
    sql << "from NST_SIMULATORCONFIG where simulatorID = " << m_simulatorID;

    SimpleDb::Buffer data;
    m_db.Exec( sql.str().c_str(), NUM_COLUMNS, data );

    m_entityKey = atol(data[0][0].c_str());
    m_locationKey = atol(data[0][1].c_str());
    m_contextName = data[0][2].c_str();
    m_typeName = data[0][3].c_str();
}


void CommsSupplierHandler::simulationStep(unsigned long stepID)
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "CommsSupplierHandler::simulationStep(): step %d", stepID);

    // Create the payload
    SupplierInfo info;
    info.uuid = TA_Base_Core::TA_Uuid::generateUUID().c_str();
    CORBA::Any msgData;
    msgData <<= info;

    // Send the message
    TA_Base_Core::MessageContext context( "GeneralComms", m_contextName.c_str(), TA_Base_Core::CommsMessage );
    TA_Base_Core::MessageType type( context, m_typeName.c_str() );

    TA_Base_Core::CommsMessageSender* sender = 
        TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( context );

    // TODO LPP: Lifetime management of NameValuePairs that are pushed into FilterData
    // is quite annoying - the NameValuePair is not copied but has to live as long as
    // the FilterData.
    TA_Base_Core::FilterData msgHeaderData;
    TA_Base_Core::NameValuePair nvpChannel("Channel", context.getChannelName());
    msgHeaderData.push_back(&nvpChannel);
    TA_Base_Core::NameValuePair nvpSupplierID("SupplierID", TA_Base_Core::gStringFromUnsignedLong(m_simulatorID));
    msgHeaderData.push_back(&nvpSupplierID);
    TA_Base_Core::NameValuePair nvpStepID("StepID", TA_Base_Core::gStringFromUnsignedLong(stepID));
    msgHeaderData.push_back(&nvpStepID);

    if (m_broadcast)
    {
        // Broadcast messages don't use the entity key or location key.
        const int UNUSED_NUMERIC_FIELD = -1;
        storeMessageToBeSent(
            m_db,
            m_simulatorID,
            stepID,
            m_contextName,
            m_typeName,
            UNUSED_NUMERIC_FIELD,
            UNUSED_NUMERIC_FIELD,
            info.uuid );

        sender->sendBroadcastCommsMessage( 
            type,
            msgData,
            &msgHeaderData );
    }
    else
    {
        storeMessageToBeSent(
            m_db,
            m_simulatorID,
            stepID,
            m_contextName,
            m_typeName,
            m_entityKey,
            m_locationKey,
            info.uuid );

        TA_Base_Core::NameValuePair nvpEntityKey("EntityKey", TA_Base_Core::gStringFromUnsignedLong(m_entityKey));
        msgHeaderData.push_back(&nvpEntityKey);
        TA_Base_Core::NameValuePair nvpLocationKey("LocationKey", TA_Base_Core::gStringFromUnsignedLong(m_locationKey));
        msgHeaderData.push_back(&nvpLocationKey);

        sender->sendCommsMessage( 
            type,
            m_entityKey,
            msgData,
            DEFAULT_SUBSYSTEM_KEY,
            m_locationKey,
            &msgHeaderData );
    }

    delete sender;
}
