/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/CommsConsumer.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "CommsConsumer.h"
#include "Common.h"
#include "app/notification_service/test_framework/IDL/src/framework.h"
#include "core/uuid/src/TAuuid.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/TA_CosNotifyChannel.h"


CommsConsumerHandler::CommsConsumerHandler(unsigned long simulatorID, SimpleDb& db, bool broadcast) :
    m_simulatorID(simulatorID),
    m_db(db),
    m_broadcast(broadcast)
{
    // Retrieve our configuration settings
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CommsConsumerHandler::CommsConsumerHandler(): Retrieving settings");
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

    // Subscribe for the appropriate type of messages
    TA_Base_Core::MessageContext context( "GeneralComms", m_contextName.c_str(), TA_Base_Core::CommsMessage );
    TA_Base_Core::MessageType type( context, m_typeName.c_str() );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "CommsConsumerHandler::CommsConsumerHandler(): subscribing on channel %s",
        context.getChannelName().c_str() );

    if (m_broadcast)
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
            type,
            this,
            NULL );
    }
    else
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            type,
            this,
            m_entityKey,
            DEFAULT_SUBSYSTEM_KEY,
            m_locationKey,
            NULL );
    }
}


CommsConsumerHandler::~CommsConsumerHandler()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "CommsConsumerHandler::~CommsConsumerHandler(): removing all subscriptions" );
              
    m_subscriptions.removeAllSubscriptions();
}


void CommsConsumerHandler::simulationStep(unsigned long stepID)
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "CommsConsumerHandler::simulationStep(): step %d", stepID);

    // Consumers should be configured so that the delay is long enough to allow all
    // sent messages to be received.  Note that nothing is actually done by the
    // simulation step for a consumer.
    
    // TODO LPP: The consumer doesn't quite fit the Simulator approach, since it is passive
    // and waits for the messaging framework to call it.  Is there a more appropriate 
    // model that we can use?

}


void CommsConsumerHandler::receiveSpecialisedMessage(const CosNotification::StructuredEvent& event, const TA_Base_Core::CommsMessageCorbaDef& payload)
{
	//if going down do not do anything. Increase the poststepdelay for consumer to a larger value
	if ( m_bGoingDown)
		return;

	SupplierInfo* info;
    payload.messageState >>= info;

    std::string srcSupplierIDStr;
    std::string srcStepIDStr;
    TA_Base_Core::getValueFromEventHeader(event, "SupplierID", srcSupplierIDStr);
    TA_Base_Core::getValueFromEventHeader(event, "StepID", srcStepIDStr);

    // We store both the expected sequence number for a given supplier (m_seqNumMap) 
    // and the actual sequence number of the message that was received (srcSeqIndex)
    // from that supplier.  This allows a query to be executed later that finds all 
    // the records with mismatching sequence numbers.  Note that the consumer can be
    // receiving messages from multiple suppliers and keeps track of the expected
    // sequence number for each one.
    std::string srcSeqNumStr;
    TA_Base_Core::getValueFromEventHeader(event, TA_Base_Core::SEQ_NUM_FIELDNAME, srcSeqNumStr);
    unsigned long srcSeqNum = atol(srcSeqNumStr.c_str());

    unsigned long srcSupplierID = atol(srcSupplierIDStr.c_str());
    
    // Insert a new expected sequence number entry if we haven't seen this supplier id before
    // (sequence numbers are expected to start at 1).
    std::pair<SimulatorIDToSeqNumMap::iterator, bool> result = 
        m_seqNumMap.insert( SimulatorIDToSeqNumMap::value_type(srcSupplierID, 1) );
    unsigned long expectedSeqNum = result.first->second;

    // We check that the actual sequence number in the message is the same as the
    // expected sequence number for the given supplier.  If they are different,
    // we will log an error.  The expected sequence number is resynchronised with
    // the actual sequence number so that the error doesn't propagate to subsequent
    // iterations.
    if (srcSeqNum != expectedSeqNum)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "CommsConsumerHandler::receiveSpecialisedMessage(): "
            "MISMATCH: Consumer %d received seqNum %d from supplier %d, expecting seqNum %d (stepID %s)", 
            m_simulatorID, srcSeqNum, srcSupplierID, expectedSeqNum, srcStepIDStr.c_str() );
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "CommsConsumerHandler::receiveSpecialisedMessage(): "
            "MATCH:    Consumer %d received seqNum %d from supplier %d (stepID %s)", 
            m_simulatorID, srcSeqNum, srcSupplierID, srcStepIDStr.c_str() );
    }

    // Increment the expected sequence number.  The following has the side effect that the
    // sequence numbers will be resynchronised if they differ.
    m_seqNumMap[ srcSupplierID ] = srcSeqNum + 1;

    std::string srcEntityKey;
    std::string srcLocationKey;

    if (m_broadcast)
    {
        // Broadcast messages don't use the entity key or location key.
        srcEntityKey = "-1";
        srcLocationKey = "-1";
    }
    else
    {
        TA_Base_Core::getValueFromEventHeader(event, "EntityKey", srcEntityKey);
        TA_Base_Core::getValueFromEventHeader(event, "LocationKey", srcLocationKey);
    }

    storeMessageReceived(
        m_db,
        m_simulatorID,
        srcSupplierIDStr,
        srcStepIDStr,
        srcSeqNum,
        expectedSeqNum,
        event.header.fixed_header.event_type.domain_name.in(),
        event.header.fixed_header.event_type.type_name.in(),
        srcEntityKey,
        srcLocationKey,
        info->uuid );
}

