/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/AlarmSupplier.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "AlarmSupplier.h"
#include "Common.h"
#include "app/notification_service/test_framework/IDL/src/framework.h"
#include "core/uuid/src/TAuuid.h"
#include "core/message/types/MessageTypes.h"
#include "core/alarm/src/AlarmPublicationManager.h"
#include "core/alarm/src/AlarmSender.h"
#include "core/utilities/src/TA_String.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"


AlarmSupplierHandler::AlarmSupplierHandler(unsigned long simulatorID, SimpleDb& db, SenderType senderType) :
    m_simulatorID(simulatorID),
    m_db(db),
    m_senderType(senderType)
{
    // Retrieve our configuration settings
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmSupplierHandler::AlarmSupplierHandler(): Retrieving settings");
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


void AlarmSupplierHandler::simulationStep(unsigned long stepID)
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "AlarmSupplierHandler::simulationStep(): step %d", stepID);

    // Send the message
    TA_Base_Core::MessageContext context( "Alarm", m_contextName.c_str(), TA_Base_Core::AlarmMessage );
    TA_Base_Core::MessageType type( context, m_typeName.c_str() );

    TA_Base_Core::AlarmSender* sender = 
        TA_Base_Core::AlarmPublicationManager::getInstance().getAlarmSender( context );

    if (m_senderType == ALARM_DETAIL)
    {
        // Create the payload.  We don't bother populating all fields.
        // Unfortunately we can't add extra data to the message header.
        TA_Base_Core::AlarmDetailCorbaDef alarm;

        alarm.messageContext = CORBA::string_dup( m_contextName.c_str() );
	    alarm.messageTypeKey = CORBA::string_dup( m_typeName.c_str() );
	    alarm.assocEntityKey = m_entityKey;
	    alarm.locationKey = m_locationKey;
	    alarm.messageSequenceNumber = stepID;

        storeMessageToBeSent(
            m_db,
            m_simulatorID,
            stepID,
            m_contextName,
            m_typeName,
            m_entityKey,
            m_locationKey,
            "" );

        sender->sendAlarm(alarm); 
    }
    else if (m_senderType == ALARM_UPDATE)
    {
        TA_Base_Core::AlarmUpdateCorbaDef update;

		update.messageContext = CORBA::string_dup( m_contextName.c_str() );
		update.messageTypeKey = CORBA::string_dup( m_typeName.c_str() );
		update.assocEntityKey = m_entityKey;
		update.locationKey = m_locationKey;

        storeMessageToBeSent(
            m_db,
            m_simulatorID,
            stepID,
            m_contextName,
            m_typeName,
            m_entityKey,
            m_locationKey,
            "" );

        sender->sendAlarmUpdate(update); 
    }
    else if (m_senderType == ALARM_LOADED)
    {
        storeMessageToBeSent(
            m_db,
            m_simulatorID,
            stepID,
            "AlarmLoadedNotification",
            "0",
            m_entityKey,
            m_locationKey,
            "" );

        sender->sendAlarmsLoadedNotification(m_locationKey, m_entityKey);
    }
    else
    {
        TA_Assert(0);
    }

    delete sender;
}
