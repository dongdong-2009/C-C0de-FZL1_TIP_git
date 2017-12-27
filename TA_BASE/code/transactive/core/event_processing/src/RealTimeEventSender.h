/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_RealTimeEvent/TA_BASE/transactive/core/event_processing/src/RealTimeEventSender.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * RealTimeEventSender is for sending event to real-time agent. It has a 
  * RealTimeEventQueue* which will actually do the task.
  */

#if !defined(_REAL_TIME_EVENT_SENDER_H_)
#define _REAL_TIME_EVENT_SENDER_H_

//#pragma once
#include "ace/Singleton.h"

#include "core/event_processing/src/RealTimeEventQueue.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/event_processing/src/ISessionHelper.h"


namespace TA_Base_Core
{
	class RealTimeEventSender
	{
	public:

		// ACE Singleton is a friend as it needs to call constructor.
        friend class ACE_Singleton<RealTimeEventSender,ACE_Recursive_Thread_Mutex>;

		/** 
		 * getInstance
		 * 
		 * get the singleton instance
		 */
		//static RealTimeEventSender& getInstance();

		/** 
		 * releaseInstance
		 * 
		 * release the singleton instance
		 */ 
		//static void releaseInstance();

		/** 
		 * processEvent
		 *
		 * process the event from AuditMessageSender
		 */
        void processEvent(EventItemInfo *event);

		void cleanUp();

		void setSessionHelper(ISessionHelper * sessionHelper);
		
	private:
		/** 
		 * constructor
		 *
		 */
		RealTimeEventSender(void);
		/** 
		 * destructor
		 *
		 */
		virtual ~RealTimeEventSender(void);
		RealTimeEventSender( const RealTimeEventSender& );
        RealTimeEventSender& operator=( const RealTimeEventSender& );
		//static RealTimeEventSender* m_instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		RealTimeEventQueue* m_worker;
		TA_Base_Core::NonReEntrantThreadLockable m_workerLock;
	};

	// Define the singleton version of the class.
    typedef ACE_Singleton<RealTimeEventSender,ACE_Recursive_Thread_Mutex> RealTimeEventSenderInstance;
};

#endif // !defined(_REAL_TIME_EVENT_SENDER_H_)
