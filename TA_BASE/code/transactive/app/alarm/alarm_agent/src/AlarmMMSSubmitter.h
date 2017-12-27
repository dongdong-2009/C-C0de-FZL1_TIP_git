/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution in any form.
*
* Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmMMSSubmitter.h $
* @author:
* @version:  $Revision:  $
*
* Last modification: $DateTime: $
* Last modified by:  $Author:  $
*
*
*/
#if !defined(ALARMMMSSUBMITTER)
#define ALARMMMSSUBMITTER

#include "core/threads/src/Thread.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include <deque>

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef; // Forward Declaration
}

namespace TA_Base_App
{
	class AlarmMMSSubmitter : public virtual TA_Base_Core::Thread
	{
	public:

		/**
		* Constructor
		* @return
		*/
		//AlarmMMSSubmitter();

		 /**
          * getInstance
          *
          * @return Returns the single instance of the AlarmCache
          */
        static AlarmMMSSubmitter* getInstance();

		static void releaseInstance();

		/**
		* Destructor
		* @return
		*/
		virtual ~AlarmMMSSubmitter();

		/**
		* run
		*
		* Overides the pure virtual run method declared in Thread
		* @return void
		*/
		void run();

		/**
		* terminate
		*
		* Overides the pure virtual terminate method declared in Thread
		* @return Void
		*/
		void terminate();

		/**
		* remove
		*
		* This method is called when the agent is in Monitor mode to allow
		* queue synchronisation. It results in the queue item being deleted from
		* the queue.
		*
		* @param An AlarmDetailCorbaDef
		* @return
		*/
		//void remove(TA_Base_Core::AlarmDetailCorbaDef& p_item);

		/**
		* remove
		*
		* This method is called when the agent is in Monitor mode to allow
		* queue synchronisation. It results in the queue item being deleted from
		* the queue.
		*
		* @param Alarm ID
		* @return
		*/
		void AlarmMMSSubmitter::remove(std::string messageId);

		/**
		* removeQueueItem
		*
		* This method is used to remove alarm from its quene.
		*
		* @param An AlarmDetailCorbaDef
		* @return
		*/
		bool removeQueueItem(std::string messageId, std::deque<TA_Base_Core::AlarmDetailCorbaDef*>* quene);

		/**
		* push
		*
		* This method enables clients to push a new IQueueItem onto
		* our queue
		*
		* @param A pointer to the IQueueItem to be pushed
		* @return
		*/
		void push(  TA_Base_Core::AlarmDetailCorbaDef* pAlarm );


		/**
		* push
		*
		* This method enables clients to push a IQueueItem list onto
		* our queue
		*
		* @param A pointer to the IQueueItem to be pushed
		* @return
		*/
		//void push(  std::deque<TA_Base_Core::AlarmDetailCorbaDef*> alarmList );

		/**
		* isInternalQueueEmpty
		*
		* This method returns true if Queue is empty.
		* @return return whether quene is empty or not
		*/
		bool isInternalQueueEmpty();


		/**
		* MoveToInternalQueue
		*
		* This method moves the IQueneItem from the Client Push Quene 
		*  to internal Quene for executing its operation.
		* @return
		*/
		void MoveToInternalQueue();

		/**
		* startProcessingMMS
		*
		* This method wakes the AlarmMMSSubmitter thread 
		* to process the MMS Request in its Quene after Switch over(Monitor to Control mode).
		* @return
		*/
		void startProcessingMMS();

	private:

		AlarmMMSSubmitter();
		AlarmMMSSubmitter( const AlarmMMSSubmitter& theAlarmMMSSubmitter);
		AlarmMMSSubmitter& operator=(const AlarmMMSSubmitter&);


		TA_Base_Core::NonReEntrantThreadLockable m_lock;
		TA_Base_Core::NonReEntrantThreadLockable m_internalLock;
		TA_Base_Core::Semaphore m_queueSemaphore;
		std::deque<TA_Base_Core::AlarmDetailCorbaDef*> m_alarmMMSQuene;
		std::deque<TA_Base_Core::AlarmDetailCorbaDef*> m_alarmMMSInternalQuene;
		bool m_terminated;

		 static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		 static AlarmMMSSubmitter* m_submitterInstance;

		/**
		* popTopInternalQueue
		*
		* This method returns a pointer to the top AlarmDetailCorbaDef of the Queue.
		* @return Top Element of the Internal Quene
		*/
		TA_Base_Core::AlarmDetailCorbaDef* popTopInternalQueue();

		/**
		* popTop
		*
		* Deletes the content of two quene.
		* @return 
		*/
		void clear();

		
		/**
		* pushTopInternalQueue
		*
		* Pushes the given alarm to top(front) in the internal quene
		* @return 
		*/
		void pushTopInternalQueue(TA_Base_Core::AlarmDetailCorbaDef* pAlarm );

		/**
		* pushTopInternalQueue
		*
		* Pushes the given alarm to back in the internal quene
		* @return 
		*/
		void pushBackInternalQueue(TA_Base_Core::AlarmDetailCorbaDef* pAlarm );

		/**
		* findQueueItem
		*
		* Find given alarm already exits or not in Quene
		* @return true if found
		*/
		bool findQueneItem(std::string messageId, std::deque<TA_Base_Core::AlarmDetailCorbaDef*>* quene);

		/**
		* findAlarm
		*
		*Find given alarm already exits or not in both the Quene
		* @return true if found
		*/
		bool findAlarm(std::string alarmID);
	};
}

#endif // !defined(ALARMMMSSUBMITTER)
