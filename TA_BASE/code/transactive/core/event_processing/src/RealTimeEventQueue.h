/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event_processing/src/RealTimeEventQueue.h $
 * @author:  Lu Xiangmei
 * @version: 
 *
 * Last modification: $DateTime: 
 * Last modified by:  $Author: 
 * 
 * RealTimeEventQueue is a thread for sending event to real-time agent.
 *
 */

#if !defined(_EVENT_QUEUE_FOR_EA_H_)
#define _EVENT_QUEUE_FOR_EA_H_
#endif

#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/event/src/EventItem.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/naming/src/NamedObject.h"
#include "core/event_processing/src/ISessionHelper.h"
#include "core/message/src/AuditMessageSender.h"

#include <sstream>
#include <deque>

namespace TA_Base_Core
{
	class RealTimeEventQueue : public Thread
	{
	public:

		/**
		 * constructor
		 */
		RealTimeEventQueue();

		/** 
		 * desctructor
		 */
		virtual ~RealTimeEventQueue();

		/** 
		 * terminate function for thread
		 */
		virtual void terminate() ;

		virtual void run();
		
		/** 
		 * push
		 *
		 * push the event to cache(m_eventReceiveQueue)
		 */
		void push(EventItemInfo * event);

		void setSessionHelper(ISessionHelper * sessionHelper);
		

	private:
		/** 
		 * moveABatchToPushQueue
		 *
		 * move events from m_eventReceiveQueue to m_eventPushQueue, the moving size is the Batch size(if the m_eventReceiveQueue has more than batch size)
		 * or the whole size of m_eventReceiveQueue(if m_eventReceiveQueue size is not bigger than batch size)
		 */
		void moveABatchToPushQueue();

		/** 
		 * prepareEventListFromPushQueue
		 *
		 * create EventSequence (the squence of EventItem) from m_eventPushQueue
		 */
		T_EventItemInfoList prepareEventListFromPushQueue();

		/** 
		 * prepareDataForPushQueue
		 * 
		 * retrieve some data(e.g. Me_message_type data ) 
		 * for all the events in m_eventPushQueue
		 */
		void prepareDataForPushQueue();

		void prepareDataForAnEvent(EventItemInfo * theEvent);

		/** 
		 * cleanPushQueue
		 *
		 * release m_eventPushQueue
		 */
		void cleanPushQueue();

		/** 
		 * cleanReceiveQueue
		 *
		 * release m_eventReceiveQueue
		 */
		void cleanReceiveQueue();
		void RealTimeEventQueue::getMessageTypeInfoByKey(unsigned long eventTypeKey, unsigned long & viewLevel, std::string & eventTypeName, unsigned long & actionId);
		void RealTimeEventQueue::processSubsystemKeyNPhisicalSubsystemKey(unsigned long & subsystemKey,long & physical_subsytem_key);
		void RealTimeEventQueue::modifyValue(std::string & value, std::string eventTypeName);
		TA_Base_Core::ReEntrantThreadLockable m_receiveQueueLock;
		TA_Base_Core::ReEntrantThreadLockable m_pushQueueLock;

		TA_Base_Core::Semaphore m_queueSemaphore;
		bool m_terminated;
		std::deque<EventItemInfo*>  m_eventReceiveQueue;
		std::deque<EventItemInfo*>  m_eventPushQueue;


		ISessionHelper* m_sessionHelper;

	};
};