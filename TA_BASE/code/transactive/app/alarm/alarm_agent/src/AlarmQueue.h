/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File$
  * @author:   Ross Tucker
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * 
  */
#if !defined(ALARMQUEUE)
#define ALARMQUEUE

#include "app/alarm/alarm_agent/src/TransactionQueue.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include <deque>
#include <vector>

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef; // Forward Declaration

    struct AlarmUpdateCorbaDef; // Forward Declaration

    class StateUpdateMessageSender; // Forward declaration

	// TD 13530 Audit messages to be sent by the alarm queue instead.
	// This is to ensure that the alarm in the alarm table is inserted or updated before event is created
    class AuditMessageSender;       // Forward declaration

}

namespace TA_Base_App
{
    // CL-21508
    class AlarmQueueWrapper;

    class AlarmQueue : public virtual TA_Base_App::TransactionQueue
    {
    public:
        
        /**
          * Constructor
          */
        AlarmQueue();

        /**
          * Destructor
          */
        virtual ~AlarmQueue();

        /**
          * run
          *
          * Overides the pure virtual run method declared in Thread
          */
        void run();

        /**
          * terminate
          *
          * Overides the pure virtual terminate method declared in Thread
          */
        void terminate();

        /**
          * removeQueueItem
          *
          * This method is called when the agent is in Monitor mode to allow
          * queue synchronisation. It results in the queue item being deleted from
          * the queue.
          *
          * @param An AlarmAgentQueueItemUpdate
          */
        void removeQueueItem(const TA_Base_Core::AlarmAgentQueueItemCorbaDef& p_item);

    private:

        AlarmQueue( const AlarmQueue& theAlarmQueue);
        AlarmQueue& operator=(const AlarmQueue&);

        // We test to see whether or not the first 'alarmsToTest' alarms have already
        // been inserted into the database before inserting them. This prevents us from
        // inserting alarms twice after failover occurs.
        unsigned int m_alarmsToTest;

        unsigned int m_queueUpdateBatchSize;

        // State update message sender
        TA_Base_Core::StateUpdateMessageSender* m_stateSender;

        TA_Base_Core::AlarmAgentQueueItemSequence* m_queueItemsProcessed;

		// TD 13530 Audit messages to be sent by the alarm queue instead.
		// This is to ensure that the alarm in the alarm table is inserted or updated before event is created
        // Audit message sender
        TA_Base_Core::AuditMessageSender* m_auditSender;

        /**
          * safeToWrite
          *
          * This method is in place to prevent multiple occurances of the same alarm
          * being written to the database (typically after fail over has occured)
          *
          * @param The alarm we are testing
          *
          * @return TRUE if it is safe to insert the alarm. FALSE otherwise
          */
        bool safeToWrite( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );

        /**
          * handleNewQueueItem
          *
          * This method takes and processes the first item on the queue
          *
          * @param The Queue item
          */
        // CL-21508 begin
        //void handleNewQueueItem( TA_Base_Core::AlarmAgentQueueItemCorbaDef* p_item );
        void handleNewQueueItem( AlarmQueueWrapper* refItem );
        // CL-21508 end

        /**
          * insertNewAlarm
          *
          * This method inserts a new alarm into the database
          *
          * @param A pointer to the new alarm
          */
        void insertNewAlarm( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );

        /**
          * updateAlarm
          *
          * This method updates an alarms details in the database
          *
          * @param A pointer to the alarm update
          */
        void updateAlarm( const TA_Base_Core::AlarmUpdateCorbaDef& p_update );


        /**
          * sendStateUpdates
          *
          * This method takes a queue item. State updates are only sent as a batch
          * whenever the number to send reaches m_alarmsToTest. The queue item is
          * stored in the m_queueItemsProcessed member variable, once the size of 
          * this sequence reaches m_alarmsToTest, the update is sent.
          *
          * @param The queue item
          */
        void sendStateUpdates(TA_Base_Core::AlarmAgentQueueItemCorbaDef* p_item);


		//
		// TD 13530 This is to send the audit message after handling (ie. updating or inserting DB) for an alarm
		//
        // CL-21508 begin
		//void sendUpdateAuditMessage( const TA_Base_Core::AlarmUpdateCorbaDef& p_update, bool isOperatorTriggered = false );
		//void sendInsertAuditMessage( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );
        void sendUpdateAuditMessage( AlarmQueueWrapper& refWrapper, bool isOperatorTriggered = false );
        void sendInsertAuditMessage( AlarmQueueWrapper& refWrapper );
        // CL-21508 end
    };
}

#endif // !defined(ALARMQUEUE)
