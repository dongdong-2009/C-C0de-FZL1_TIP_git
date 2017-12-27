/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/IEventProcessor.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * The IEventProcessor interface defines the methods a class will need in order to register
  * themselves with the EventProcessingLibrary.
  */

#if !defined(_I_EVENT_PROCESSOR_H_)
#define _I_EVENT_PROCESSOR_H_

#include "core/event_processing/src/IEvent.h"

namespace TA_Base_Core
{
    class IEventProcessor
    {
    public:
        /**
         * getNumRetries
         *
         * Returns the number of times this processor would like to retry the processing
         * of an event in the situation where it fails.
         *
         * @return unsigned char The number of times to retry.
         */
        virtual unsigned char getNumRetries() =0;


        /**
         * getRetryDelay
         *
         * Returns the amount of number of milliseconds to wait before retrying the processing.
         *
         * @return unsigned long The number of milliseconds wait.
         */
        virtual unsigned long getRetryDelay() =0;


        /** 
          * processEvent
          *
          * This method will perform some processing on the provided event.
          *
          * @param event The event to process.
          *
          * @return bool True if the event processing succeeded. False will result in the processing
          *              being retried.
          */
        virtual bool processEvent( Event& event ) =0;
    };
};

#endif // !defined(_I_EVENT_PROCESSOR_H_)

