/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/WorkItemQueueProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is a queue processor which processes work items.
  * It provides a means of having a queue which you can put anything in
  * and the items you insert will do their own work sequentially
  *
  */

#ifndef SUMMARYPROCESSOR_H
#define SUMMARYPROCESSOR_H

#include "core/threads/src/QueueProcessor.h"
#include "core/threads/src/IWorkItem.h"

namespace TA_Base_Core
{
	class WorkItemQueueProcessor : public QueueProcessor<IWorkItem>
	{

    public:
        WorkItemQueueProcessor (unsigned long maxQueueSize = MAX_QUEUEPROCESSOR_QUEUE_SIZE);

        virtual ~WorkItemQueueProcessor();

	protected:

		/**
		 * processEvent()
		 *
         * Executes the next work item
		 */
		virtual void processEvent( boost::shared_ptr<IWorkItem> newEvent );
		
	};
}

#endif
