#ifndef ACTIVEPLANMESSAGEQUEUE_H_INCLUDED
#define ACTIVEPLANMESSAGEQUEUE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageQueue.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/20 12:26:46 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */

#include "ace/Message_Queue.h"
#include "ace/RW_Mutex.h"
#include "ace/Synch.h"
#include "boost/shared_ptr.hpp"

class ACE_Message_Block;
class ACE_Time_Value;

namespace TA_Base_App
{
	//Class declaration
	class ActivePlanMessageQueue : public ACE_Message_Queue<ACE_MT_SYNCH>
	{
	public:
		ActivePlanMessageQueue();
		~ActivePlanMessageQueue();

        virtual int dequeue( ACE_Message_Block *&first_item, ACE_Time_Value *timeout  = 0 );
        virtual int enqueue( ACE_Message_Block *new_item, ACE_Time_Value *timeout = 0 );

        void abandon();
        void reuse();

	private:
		// Make sure these don't get called
		ActivePlanMessageQueue &operator= (const ActivePlanMessageQueue &);
		ActivePlanMessageQueue (const ActivePlanMessageQueue &);

    private:
        bool m_bAbandoned;
		mutable ACE_RW_Mutex m_mtxDeletionLock;
	}; // ActivePlanMessageQueue

    typedef boost::shared_ptr<ActivePlanMessageQueue> ActivePlanMessageQueuePtr;
}; // TA_Base_App

#endif // ACTIVEPLANMESSAGEQUEUE_H_INCLUDED
