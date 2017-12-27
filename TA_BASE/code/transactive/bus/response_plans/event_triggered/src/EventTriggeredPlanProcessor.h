/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/event_triggered/src/EventTriggeredPlanProcessor.h $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the processor that is responsible for triggering plans based off an event.
 */

#if !defined(_EVENT_TRIGGERED_PLAN_PROCESSOR_H_)
#define _EVENT_TRIGGERED_PLAN_PROCESSOR_H_

#include <memory>

#include "bus/response_plans/event_triggered/src/EventPlanAssocCache.h"

#include "core/event_processing/src/IEvent.h"
#include "core/event_processing/src/IEventProcessor.h"

namespace TA_Base_Bus
{
    // Forward declarations.
    class PlanAgentLibrary;

    class EventTriggeredPlanProcessor : TA_Base_Core::IEventProcessor
    {
    public:
        /**
         * registerWithProcessingLibrary
         *
         * Registers this processor with the event processing library. It also ensures
         * that multiple calls will not result in multiple registrations.
         */
        static void registerWithProcessingLibrary();


        /**
         * Destructor
         */
        virtual ~EventTriggeredPlanProcessor();


        /**
         * getNumRetries
         *
         * Returns the number of times this processor would like to retry the processing
         * of an event in the situation where it fails.
         *
         * @return unsigned char The number of times to retry.
         */
        virtual unsigned char getNumRetries();


        /**
         * getRetryDelay
         *
         * Returns the amount of number of milliseconds to wait before retrying the processing.
         *
         * @return unsigned long The number of milliseconds wait.
         */
        virtual unsigned long getRetryDelay();


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
        virtual bool processEvent( TA_Base_Core::Event& event );

    private:
        // Hidden methods.
        EventTriggeredPlanProcessor();
        EventTriggeredPlanProcessor( const EventTriggeredPlanProcessor& );
        const EventTriggeredPlanProcessor& operator=( const EventTriggeredPlanProcessor& );

        // The interface to the plan agent.
        std::auto_ptr<PlanAgentLibrary> m_planAgent;

        // The cache of event-plan associations.
        EventPlanAssocCache m_assocCache;
    };
};

#endif // !defined(_EVENT_TRIGGERED_PLAN_PROCESSOR_H_)
