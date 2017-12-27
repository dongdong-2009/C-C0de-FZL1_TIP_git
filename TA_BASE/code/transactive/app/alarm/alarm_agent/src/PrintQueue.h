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
#if !defined(PRINTQUEUE)
#define PRINTQUEUE

#include "app/alarm/alarm_agent/src/TransactionQueue.h"
#include <deque>

namespace TA_Base_Core
{
	struct AlarmDetailCorbaDef; // Forward Declaration

    class StateUpdateMessageSender; // Forward declaration
}

namespace TA_Base_App
{
    class AbstractPrinter; // Forward declaration

        class PrintQueue : public virtual TA_Base_App::TransactionQueue
    {
    public:
        
        /**
          * Constructor
          */
        PrintQueue();

        /**
          * Destructor
          */
        virtual ~PrintQueue();

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

        PrintQueue( const PrintQueue& thePrintQueue);
        PrintQueue& operator=(const PrintQueue&);

        AbstractPrinter* m_printer;

        // State update message sender
        TA_Base_Core::StateUpdateMessageSender* m_stateSender;

        /**
          * handleNewQueueItem
          *
          * This method takes and processes the first item on the queue
          *
          * @param The Queue item
          */
        void handleNewQueueItem( AlarmQueueWrapper* pWrapper);

        /**
          * printAlarm
          *
          * This method sends the alarm to our IPrinter object
          *
          * @param A pointer to the alarm
          */
        void printAlarm( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );

    };
}

#endif // !defined(PRINTQUEUE)
