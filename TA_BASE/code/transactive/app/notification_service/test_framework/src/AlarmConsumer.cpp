/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/AlarmConsumer.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "AlarmConsumer.h"
#include "Common.h"
#include "app/notification_service/test_framework/IDL/src/framework.h"
#include "core/uuid/src/TAuuid.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/TA_CosNotifyChannel.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core/utilities/src/TA_String.h"


AlarmConsumerHandler::AlarmConsumerHandler(unsigned long simulatorID, SimpleDb& db) :
    m_simulatorID(simulatorID),
    m_db(db)
{
    // Retrieve our configuration settings
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmConsumerHandler::AlarmConsumerHandler(): Retrieving settings");
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
    TA_Base_Core::MessageContext context( "Alarm", m_contextName.c_str(), TA_Base_Core::AlarmMessage );
    TA_Base_Core::MessageType type( context, m_typeName.c_str() );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "AlarmConsumerHandler::AlarmConsumerHandler(): subscribing on channel %s",
        context.getChannelName().c_str() );

    if (m_entityKey == 0)
    {
        TA_Base_Core::AlarmSubscriptionManager::getInstance().subscribeToEntityAlarms(
            context, this, m_entityKey, m_locationKey );
    }
    else
    {
        TA_Base_Core::AlarmSubscriptionManager::getInstance().subscribeToAlarmsFromLocation(
            type, this, m_locationKey );
    }
}


AlarmConsumerHandler::~AlarmConsumerHandler()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "AlarmConsumerHandler::~AlarmConsumerHandler(): removing all subscriptions" );
              
    m_subscriptions.removeAllSubscriptions();
}


void AlarmConsumerHandler::simulationStep(unsigned long stepID)
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "AlarmConsumerHandler::simulationStep(): step %d", stepID);
}


void AlarmConsumerHandler::receiveSpecialisedMessage(const CosNotification::StructuredEvent& event, const TA_Base_Core::AlarmMessageCorbaDef& payload)
{
    // TODO LPP NOW: This is virtually the same as CommsConsumerHandler. 
    // Refactor the sequence number checking.

	//if going down do not do anything. Increase the poststepdelay for consumer to a larger value
	if ( m_bGoingDown)
		return;

    std::string srcSupplierIDStr;
    TA_Base_Core::getValueFromEventHeader(event, TA_Base_Core::EVENT_SUPPLIER_FIELDNAME, srcSupplierIDStr);

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
            "AlarmConsumerHandler::receiveSpecialisedMessage(): "
            "MISMATCH: Consumer %d received seqNum %d from supplier %d, expecting seqNum %d", 
            m_simulatorID, srcSeqNum, srcSupplierID, expectedSeqNum );
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "AlarmConsumerHandler::receiveSpecialisedMessage(): "
            "MATCH:    Consumer %d received seqNum %d from supplier %d", 
            m_simulatorID, srcSeqNum, srcSupplierID );
    }

    // Increment the expected sequence number.  The following has the side effect that the
    // sequence numbers will be resynchronised if they differ.
    m_seqNumMap[ srcSupplierID ] = srcSeqNum + 1;

    std::string srcEntityKey;
    std::string srcLocationKey;

	switch ( payload._d() )
	{
	case TA_Base_Core::NewAlarm:
    {
        const TA_Base_Core::AlarmDetailCorbaDef& alarm = payload.alarm();
        srcEntityKey = TA_Base_Core::gStringFromUnsignedLong(alarm.assocEntityKey);
        srcLocationKey = TA_Base_Core::gStringFromUnsignedLong(alarm.locationKey);
        break;
    }
	case TA_Base_Core::UpdateAlarm:
    {
        const TA_Base_Core::AlarmUpdateCorbaDef& update = payload.update();
		srcEntityKey = TA_Base_Core::gStringFromUnsignedLong(update.assocEntityKey);
		srcLocationKey = TA_Base_Core::gStringFromUnsignedLong(update.locationKey);
        break;
    }
    case TA_Base_Core::AlarmsLoadedNotificationMessage:
    {
        const TA_Base_Core::AlarmsLoadedNotification& notification = payload.notification();

        // TODO LPP NOW: Finish extraction of fields

        break;
    }
    default:
        TA_Assert(0);
        break;
    };

    storeMessageReceived(
        m_db,
        m_simulatorID,
        srcSupplierIDStr,
        "", // StepID
        srcSeqNum,
        expectedSeqNum,
        event.header.fixed_header.event_type.domain_name.in(),
        event.header.fixed_header.event_type.type_name.in(),
        srcEntityKey,
        srcLocationKey,
        "" );
}

