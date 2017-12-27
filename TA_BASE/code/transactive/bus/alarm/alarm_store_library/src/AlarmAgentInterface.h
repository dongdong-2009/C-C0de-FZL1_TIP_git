/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmAgentInterface.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmAgentInterface.h
//  Implementation of the Class AlarmAgentInterface
//  Created on:      24-Jan-2004 04:17:11 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmAgentInterface_405509EE_4F53_49fa_8758_8BAFF2AAEF66__INCLUDED_)
#define AlarmAgentInterface_405509EE_4F53_49fa_8758_8BAFF2AAEF66__INCLUDED_

#include <queue>

#include "IAlarmUpdate.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmReceiver.h"
#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/naming/src/namedobject.h"

namespace TA_Base_Bus
{

	/**
	 * This class provides access to an alarm store at a specific location.  It will
	 * manage the initial loading of alarms from that location and any necessary re-
	 * loading.  It will also manage the subscription to notification of alarm changes
	 * at the location
	 * @version 1.0
	 * @created 24-Jan-2004 04:17:11 PM
	 */
	class AlarmAgentInterface : public virtual TA_Base_Core::AlarmReceiver, 
								public virtual TA_Base_Core::Thread 
	{
		public:
			/**
			 * Constructor
			 * @param callback    Callback used to notify new alarm details
			 * @param alarmAgentEntityName
			 * 
			 */
			AlarmAgentInterface(IAlarmUpdate& callback, std::string entityName, 
                                unsigned long myLocation, unsigned long agentLocation, bool alarmStoreStart);
			virtual ~AlarmAgentInterface();


			/**
			 * getAgentEntityName
			 * @return Returns the entity name for this interface
			 */
			std::string AlarmAgentInterface::getAgentEntityName() {return m_entityName;};


			/**
			 * getAgentLocationKey
			 * @return Returns the key for location of this agent interface
			*/
			unsigned long AlarmAgentInterface::getAgentLocationKey() {return m_agentLocation;};

			/*
			 * AlarmAgentInterface::reloadAlarms
			 *
			 * This method will reload the alarms from the agent.
			 *
			 */
			void reloadAlarms();

			/**
			 * The terminate() method should cause run() to return. eg: CorbaUtil::
			 * GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
			 * to call this method directly - call terminateAndWait() instead
			 */
			virtual void terminate();
		protected:
			/**
			 * Define a run() method to do your work. This should block until terminate() is
			 * called eg: CorbaUtil::GetInstance().Run() or m_dialog->DoModal()  NOTE: do not
			 * call this method directly. It needs to be public so that it can be called from
			 * the thread-spawning function
			 */
			virtual void run();
			/**
			 * receiveSpecialisedMessage  This is overriden by the client to receive their
			 * specialise message
			 * @param	T	The narrowed Message
			 * @param message
			 * 
			 */
			virtual void receiveAlarmMessage( const TA_Base_Core::AlarmMessageCorbaDef& alarmMessage );

			static const unsigned long ALARM_STORE_POLL_RETRY_COUNT;

		private:

			// Disable the copy constructor
			AlarmAgentInterface(const AlarmAgentInterface& theAlarmAgentInterface);
			AlarmAgentInterface& operator = (const AlarmAgentInterface& theAlarmAgentInterface);


			// Check sequence number
			bool isMessageInSequence(unsigned long sequenceNumber);


			std::string							m_entityName;
			IAlarmUpdate&						m_callback;
			volatile bool						m_terminate;
			TA_Base_Core::ReEntrantThreadLockable	m_loadLock;
			unsigned long						m_agentLocation;
            unsigned long                       m_myLocation;
			bool								m_subscribed;
			volatile bool                       m_reload;
			volatile unsigned long              m_notificationSequence;

            // Used to identify if the update can be processed immediately or if they
            // must be queue.
            volatile bool m_queueUpdates;

            // This queue will hold any updates received while the alarms are being loaded.
            std::queue<TA_Base_Core::AlarmMessageCorbaDef> m_updateQueue;

            // This thread lock ensures there is no concurrent access to the update queue.
            TA_Base_Core::ReEntrantThreadLockable m_updateQueueLock;

			TA_Base_Core::NamedObject<TA_Base_Core::AlarmAgentCorbaDef,
				                 TA_Base_Core::AlarmAgentCorbaDef_ptr,
								 TA_Base_Core::AlarmAgentCorbaDef_var>   m_alarmAgent;


			void startSubscription();
			void stopSubscription();
			void loadCurrentAlarms();

            /** 
              * shouldDiscardAlarm
              *
              * 4669 specific behaviour. Checks to see if alarm should be discarded
              * if certain conditions hold true.
              *
              * @param alarm  The alarm to check.
              *
              * @return       True if the alarm should be discarded, false otherwise.
              */
            bool shouldDiscardAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);

			// Wenguang TD18266
			std::string		m_visualAlarmContext;
			unsigned long	m_CctvTriggeringAlarmTypeKey;
			unsigned long	m_CctvTriggeringAlarmAMSTypeKey;
			unsigned long	m_CctvTriggeringAlarmIntercomTypeKey;
			unsigned long	m_CctvTriggeringAlarmESPTypeKey;
			unsigned long	m_CctvTriggeringAlarmPATTypeKey;
			unsigned long	m_CctvTriggeringAlarmBLSTypeKey;
            /**
              * processUpdate
              *
              * Processes the given update.
              *
              * @param update The update to check.
              */
            void processUpdate( const TA_Base_Core::AlarmMessageCorbaDef& alarmMessage );

			//chenlei++
			unsigned long m_observerId;
			void alarmAgentPolling();
			unsigned long m_retryCount;
			//chenlei++
			//luohuirong++ 30-10-2009
			//AlarmMessageQueue* m_alarmMessageQueueProcess;
            //luohuirong++ 30-10-2009
			bool m_AlarmStoreStart; 
	};
};  // namespace TA_Base_App

#endif // !defined(AlarmAgentInterface_405509EE_4F53_49fa_8758_8BAFF2AAEF66__INCLUDED_)
