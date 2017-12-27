/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmReceiver.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * description
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/alarm/src/AlarmReceiver.h"

namespace TA_Base_Core
{
	AlarmReceiver::AlarmReceiver( )
	{

	}

	AlarmReceiver::~AlarmReceiver()
	{

	}
	
	void AlarmReceiver::receiveMessage(const CORBA::Any& message)
	{
		// The local alarm message object
		AlarmMessageCorbaDef alarmMessage;

		// Placeholders for the message type
		AlarmDetailCorbaDef* detail;
		AlarmUpdateCorbaDef* update;
        AlarmsLoadedNotification notification;

		// Try to extract to an update
		if ( ( message >>= detail) != 0)
		{
			// Extraction was successful, create the detail
			alarmMessage.alarm( *detail );

			// Prevent this being called on the same subscriber at the same time
			TA_Base_Core::ThreadGuard guard(m_lock);

			// Pass the message onto the subscriber
			receiveAlarmMessage( alarmMessage );
		}
		else if ( ( message >>= update ) != 0 )
		{
			// Extraction was successful, create the update
			alarmMessage.update( *update );

			// Prevent this being called on the same subscriber at the same time
			TA_Base_Core::ThreadGuard guard(m_lock);

			// Pass the message onto the subscriber
			receiveAlarmMessage( alarmMessage );
		}
        else if( ( message >>= CORBA::Any::to_boolean(notification) ) != 0 )
        {
			// Extraction was successful, create the notification
			alarmMessage.notification( notification );

			// Prevent this being called on the same subscriber at the same time
			TA_Base_Core::ThreadGuard guard(m_lock);

			// Pass the message onto the subscriber
			receiveAlarmMessage( alarmMessage );
        }
		else
		{
			LOG_GENERIC( SourceInfo, 
					TA_Base_Core::DebugUtil::DebugError,
					"Error extracting Alarm in AlarmReceiver." );

			// return quietly
		}
	}

	
	bool AlarmsMessageFilter::match( const CosNotification::StructuredEvent& event ) const
	{
		// Placeholders for the message type
		AlarmDetailCorbaDef* detail;
		AlarmUpdateCorbaDef* update;
		AlarmsLoadedNotification notification;

		// Try to extract to an update
		if ( ( event.remainder_of_body >>= detail) != 0)
		{
			// Check that the entityKey is a match
			if ( detail->assocEntityKey == m_entityKey )
			{
				return true;
			}
		}
		else if ( ( event.remainder_of_body >>= update ) != 0 )
		{
			// Check that the entityKey is a match
			if ( update->assocEntityKey == m_entityKey )
			{
				return true;
			}
		}
		else if( ( event.remainder_of_body >>= CORBA::Any::to_boolean(notification) ) != 0 )
		{
			return true;
		}
		else
		{
			LOG_GENERIC( SourceInfo, 
					TA_Base_Core::DebugUtil::DebugError,
					"Error extracting Alarm in AlarmsMessageFilter." );

			// return quietly
		}

		// If we get here we have a failure
		return false;
	}

}; // namespace TA_Base_Core

