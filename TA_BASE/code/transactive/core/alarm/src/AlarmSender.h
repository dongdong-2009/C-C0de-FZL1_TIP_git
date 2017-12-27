#if !defined(AlarmSender_7F0D6190_5504_4193_A55D_3B02FDB10AB3__INCLUDED_)
#define AlarmSender_7F0D6190_5504_4193_A55D_3B02FDB10AB3__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmSender.h $
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

namespace TA_Base_Core
{
	class MessageContext;

	struct AlarmDetailCorbaDef;
	struct AlarmUpdateCorbaDef;
    class StructuredEventSupplier;

    // AlarmMessage header (for sendAlarm and sendAlarmUpdate):

    // domain_name = message context name
    // type_name = <segmentedEntityKey>_<messageType>
    // variable_header:
    // - not used
    // filterable_data:
    // - (ASSOC_ENTITYKEY_FILTER_NAME, entityKey)
    // - (REGION_FILTER_NAME, location key)
    // - (MESSAGETYPE_FILTER_NAME, messageType)

    // AlarmMessage header for sendAlarmsLoadedNotification is the same as above except
    // type_name = message type

    // NOTE that the special locations LOCAL_LOCATION and ANY_LOCATION cannot be used.

	class AlarmSender
	{
	public:
		/**
		* constructor
		*
		* Create a new AlarmSender with a pointer to a StructuredEventSupplier
		*
		* @param	msgContext			The context being used when sending events
		* @param    supplier    Pointer to the structured event supplier the AlarmSender will use to send Alarms
		*/
		explicit AlarmSender(StructuredEventSupplier* supplier);

		/**
		* destructor
		*/
		virtual ~AlarmSender();

		/**
		* sendAlarm
		*
		* Send an alarm on the Alarm channel. The AlarmAgent is the
		* only application that will need to use this library. This
		* function will generate the message type from the provided
		* alarm
		*
		* @param    alarm    The alarm to send out on the alarm channel
		*/
		void sendAlarm( const AlarmDetailCorbaDef& alarm );

		/**
		* sendAlarmUpdate
		*
		* Send an alarm update on the Alarm channel. The AlarmAgent is the
		* only application that will need to use this library. This
		* function will generate the message type from the provided
		* alarm
		*
		* @param    alarmUpdate    The alarmUpdate to send out on the alarm channel
		*/
		void sendAlarmUpdate( const AlarmUpdateCorbaDef& alarmUpdate );

        /**
        * sendAlarmsLoadedNotification
        *
        * Broadcast a notification that the Alarm Agent has finished (re)loading alarms
        * from the database. This notification is really just for the Alarm Stores to
        * receive so that they can refresh their local caches.
        *
        * @param The location key for the Alarm Agent that sends the message
        * @param The entity key of the Alarm Agent that sends the message
        */
        void sendAlarmsLoadedNotification(unsigned long p_location, unsigned long p_entityKey);

	private:

		/**
		* disable default constructor
		*/
		AlarmSender();

		/**
		* disable copy constructor
		*/
		AlarmSender( const AlarmSender& theAlarmSender);
		AlarmSender& operator = ( const AlarmSender& theAlarmSender);

        StructuredEventSupplier* m_supplier;
	};


}; // namespace TA_Base_Core

#endif // !defined(AlarmSender_7F0D6190_5504_4193_A55D_3B02FDB10AB3__INCLUDED_)
