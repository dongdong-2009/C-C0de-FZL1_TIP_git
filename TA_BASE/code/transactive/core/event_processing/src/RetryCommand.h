/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/RetryCommand.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * The RetryCommand is used a particular processor that failed to successfully process an
  * event in a previous attempt.
  *
  */

#if !defined(_RETRY_COMMAND_H_)
#define _RETRY_COMMAND_H_

#include <vector>

#include "core/event_processing/src/AbstractCommand.h"
#include "core/event_processing/src/IEvent.h"

namespace TA_Base_Core
{
    // Forward declarations.
    class CommandScheduler;
    class IEventProcessor;

    class RetryCommand : public AbstractCommand
    {
    public:
        /**
         * Constructor
         *
         * @param event The event to process.
         * @param processor The processor to use when processing the event.
         * @param retryCount The number of times processing should retry if this attempt fails.
         * @param time The amount of time to wait before executing the retry.
         */
        RetryCommand( Event& event, IEventProcessor& processor, unsigned char retryCount, unsigned long delay );


        /**
         * Destructor
         */
        virtual ~RetryCommand();


        /**
         * execute
         *
         * Performs whatever processing is required of the command object.
         *
         * @param scheduler The scheduler that is executing the command.
         */
        virtual void execute( CommandScheduler& scheduler );


    private:
        // Hidden methods.
        RetryCommand();
        RetryCommand( const RetryCommand& );
        const RetryCommand& operator=( const RetryCommand& );


        // Variables required to perform the processing.
        Event m_event;
        IEventProcessor& m_processor;
        unsigned char m_retryCount;
        time_t m_delay;
    };
};

#endif // !defined(_RETRY_COMMAND_H_)
