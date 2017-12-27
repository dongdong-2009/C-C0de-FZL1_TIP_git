#if !defined(ALARM_RECEIVER_H)
#define ALARM_RECEIVER_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmReceiver.h $
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

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/message/src/AbstractMessageSubscriber.h"
#include "core/message/src/IMessageSubscriber.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class AlarmReceiver : public TA_Base_Core::AbstractMessageSubscriber
	{
	public:
		AlarmReceiver();

		virtual ~AlarmReceiver();
		
		/** 
		  * receiveMessage
		  *
		  * This function will receive the AlarmDetail/AlarmUpdate.
		  * It will work out what type of message it is, and pass on the populated message
		  * to the derived class by calling receiveAlarmMessage
		  *
		  * @param Any message
		  */
		virtual void receiveMessage(const CORBA::Any& message);

		/** 
		  * receiveAlarmMessage
		  *
		  * This function is overridden by the client that wishes to receive AlarmMessages
		  *
		  * @param TA_Base_Core::AlarmMessageCorbaDef& alarmMessage
		  */
		virtual void receiveAlarmMessage( const TA_Base_Core::AlarmMessageCorbaDef& alarmMessage ) = 0;

	private:
		/**
		* Thread guard the received message call back
		*/
		TA_Base_Core::NonReEntrantThreadLockable m_lock;
	};

	
	class AlarmsMessageFilter : public IMessageFilter
	{
	public:
		
		/**
		* constructor
		* 
		* default constructor
		*/
		explicit AlarmsMessageFilter( unsigned long entityKey )
			: m_entityKey( entityKey ) 
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugMsg, 
                "AlarmsMessageFilter::AlarmsMessageFilter(): constructing instance 0x%x entityKey=%d", 
                this, m_entityKey);
        }
		
		/**
		* destructor
		*/
		virtual ~AlarmsMessageFilter(){};

		/**
		* match
		*
		* This will return true if the filter matches the received message
		*
		* @param    event
		*/
		virtual bool match( const CosNotification::StructuredEvent& event ) const;

	private:
		/**
		* disable the default constructor
		*/
		AlarmsMessageFilter();

		/**
		* copy constructor
		* 
		* disable the default copy constructor
		*/
		AlarmsMessageFilter( const AlarmsMessageFilter& );

	private:
		/**
		 * store the entity key for comparison purposes
		 */
		unsigned long m_entityKey;
	};

}; // namespace TA_Base_Core

#endif // !defined(ALARM_RECEIVER_H)
