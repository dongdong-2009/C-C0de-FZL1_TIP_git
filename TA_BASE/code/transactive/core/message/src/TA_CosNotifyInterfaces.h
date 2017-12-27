#if !defined(TA_CosNotifyInterfaces_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
#define TA_CosNotifyInterfaces_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/TA_CosNotifyInterfaces.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:12:25 $
  * Last modified by:  $Author: limin.zhu $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "COS/CosNotifyChannelAdmin.hh"
#include <vector>

namespace CosNotification
{
    struct StructuredEvent;
    class EventTypeSeq;
};

namespace TA_Base_Core
{
    // Refer to TA_CosNotifyChannel.h for more information about these interfaces.


    // IEventHandler is implemented by components that process batches of events (a batch
    // can contain one or more events).  
    struct IEventHandler
    {
        // Events in the batch must be processed transactionally - ie. the implementor should 
        // either process ALL or NONE of the events in the batch.  false should be returned
        // if a failure occurred and the events were not processed.
        // NOTE that the caller of onEventsReceived() keeps ownership of the events in the 
        // vector (ie. the framework).
        // Exceptions must NOT be thrown.
    	virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events) = 0;
        virtual bool onEventsReceived(const CosNotification::EventBatch& eventBatch) // adapt to the function above by default
        {
            std::vector<const CosNotification::StructuredEvent*> vecBatch( eventBatch.length() );
            for ( size_t i = 0, size = eventBatch.length(); i < size; ++i )
            {
                vecBatch[i] = &eventBatch[i];
            }
            return onEventsReceived(vecBatch);
        }

        virtual ~IEventHandler() {}
    };


    // Subscription change notifications are sent to all suppliers by an event channel when
    // a consumer modifies the filters attached to its proxy supplier.
    struct ISubChangeHandler
    {
        // 'added' and 'removed' will NOT be empty.
        // Exceptions must NOT be thrown.
		virtual void onSubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed) = 0;

        // Indicates that the subscription change deltas previously sent by onSubscriptionChange() 
        // are no longer valid.
        virtual void onSubscriptionReset() = 0;

        virtual ~ISubChangeHandler() {}
    };

}; // TA_Base_Core

#endif // !defined(TA_CosNotifyInterfaces_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
