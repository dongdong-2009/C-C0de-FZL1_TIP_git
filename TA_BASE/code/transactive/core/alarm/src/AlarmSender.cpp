/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmSender.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmSender.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/types/AlarmAgentAlarm_MessageTypes.h"
#include "core/message/src/TA_CosUtility.h"

// For LOCAL_LOCATION and ANY_LOCATION
#include "core/message/src/SubscriptionManager.h"

namespace TA_Base_Core
{

	AlarmSender::AlarmSender( TA_Base_Core::StructuredEventSupplier* supplier) :
		m_supplier(supplier)
	{
        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
	}

	AlarmSender::~AlarmSender()
	{

	}


	void AlarmSender::sendAlarm( const AlarmDetailCorbaDef& alarm )
	{
        TA_Assert(alarm.locationKey != LOCAL_LOCATION && alarm.locationKey != ANY_LOCATION);

		//
		// Collate Variable Header fields, there is none
		//	
		TA_Base_Core::FilterData varHeader;

		char entityKeyChar[16] = {0};
		sprintf( entityKeyChar, "%ld", alarm.assocEntityKey );
		
		char locationChar[16] = {0};
		sprintf( locationChar, "%ld", alarm.locationKey );

		// Store the message type key
		//TD17994,jianghp
		// Wenguang TD18266
		std::stringstream messageTypeKey;
        messageTypeKey << alarm.messageTypeKey;

		//
		// Collate Filterable Body fields
		//
		NameValuePair entityFilter( ASSOC_ENTITYKEY_FILTER_NAME, entityKeyChar );
		NameValuePair regionFilter( REGION_FILTER_NAME, locationChar );
		NameValuePair messageTypeFilter ( MESSAGETYPE_FILTER_NAME, messageTypeKey.str() );

		FilterData filters;
		filters.push_back( &entityFilter );
		filters.push_back( &regionFilter );
		filters.push_back( &messageTypeFilter );

		//
		// Create Structured Event, inject message into Any field
		//
		CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
		event->remainder_of_body <<= alarm;

		gPopulateStructuredEventHeader( 
            *event,
			alarm.messageContext.in(),
			gGetSegmentedEntityKey( alarm.assocEntityKey ) + TYPE_NAME_SEPARATOR + messageTypeKey.str(),
			&varHeader,
			&filters );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
	}

	
	void AlarmSender::sendAlarmUpdate( const AlarmUpdateCorbaDef& alarmUpdate )
	{
		//
		// Collate Variable Header fields, there is none
		//	
		TA_Base_Core::FilterData varHeader;

		char entityKeyChar[16] = {0};
		sprintf( entityKeyChar, "%ld", alarmUpdate.assocEntityKey );
		
		char locationChar[16] = {0};
		sprintf( locationChar, "%ld", alarmUpdate.locationKey );

		// Store the message type key
		//TD17994,jianghp
		std::stringstream alarmTypeKey;
		alarmTypeKey << alarmUpdate.messageTypeKey;

		//
		// Collate Filterable Body fields
		//
		NameValuePair entityFilter( ASSOC_ENTITYKEY_FILTER_NAME, entityKeyChar );
		NameValuePair regionFilter( REGION_FILTER_NAME, locationChar );
		NameValuePair messageTypeFilter ( MESSAGETYPE_FILTER_NAME, alarmTypeKey.str() );

		FilterData filters;
		filters.push_back( &entityFilter );
		filters.push_back( &regionFilter );
		filters.push_back( &messageTypeFilter );

		//
		// Create Structured Event, inject message into Any field
		//
		CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
		event->remainder_of_body <<= alarmUpdate;

		gPopulateStructuredEventHeader( 
            *event,
			alarmUpdate.messageContext.in(),
			gGetSegmentedEntityKey(alarmUpdate.assocEntityKey) + TYPE_NAME_SEPARATOR + alarmTypeKey.str(), 
			&varHeader,
			&filters );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
	}

	void AlarmSender::sendAlarmsLoadedNotification(unsigned long p_location, unsigned long p_entityKey)
	{
		//
		// Collate Variable Header fields, there is none
		//	
		TA_Base_Core::FilterData varHeader;

		char entityKeyChar[16] = {0};
		sprintf( entityKeyChar, "%ld", p_entityKey );
		
		char locationChar[16] = {0};
		sprintf( locationChar, "%ld", p_location );

        std::string messageTypeKey = TA_Base_Core::AlarmAgentAlarm::AlarmsLoaded.getTypeKey();

		//
		// Collate Filterable Body fields
		//
		NameValuePair entityFilter( ASSOC_ENTITYKEY_FILTER_NAME, entityKeyChar );
		NameValuePair regionFilter( REGION_FILTER_NAME, locationChar );
		NameValuePair messageTypeFilter ( MESSAGETYPE_FILTER_NAME, messageTypeKey );

		FilterData filters;
		filters.push_back( &entityFilter );
		filters.push_back( &regionFilter );
		filters.push_back( &messageTypeFilter );

		//
		// Create Structured Event, inject message into Any field
		//
        AlarmsLoadedNotification notification = true;

		CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
		event->remainder_of_body <<= CORBA::Any::from_boolean(notification);

		gPopulateStructuredEventHeader( 
            *event,
			TA_Base_Core::AlarmAgentAlarm::AlarmsLoaded.getContextObj().getContextName(),
			messageTypeKey,
			&varHeader,
			&filters );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
	}

}; // namespace TA_Base_Core

