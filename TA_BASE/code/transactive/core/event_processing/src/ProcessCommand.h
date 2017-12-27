/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/ProcessCommand.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * The ProcessCommand is used to invoke all processors on the given event.
  *
  */

#if !defined(_PROCESS_COMMAND_H_)
#define _PROCESS_COMMAND_H_

#include <vector>

#include "core/event_processing/src/AbstractCommand.h"
#include "core/event_processing/src/IEvent.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"

namespace TA_Base_Core
{
    // Forward declarations.
    class CommandScheduler;
    class IEventProcessor;

    class ProcessCommand : public AbstractCommand
    {
    public:
        /**
         * Constructor
         *
         * @param event The event to process.
         * @param processorList The processors to use when processing the event.
         * @param processorListLock The lock to use when accessing the processor list.
         */
        ProcessCommand( Event& event, std::vector<IEventProcessor*>& processorList,
                        WriteReadThreadLockable& processorListLock );


        /**
         * Destructor
         */
        virtual ~ProcessCommand();


        /**
         * execute
         *
         * Performs whatever processing is required of the command object.
         *
         * @param queue The queue that is executing the command.
         */
        virtual void execute( CommandScheduler& queue );


    private:
        // Hidden methods.
        ProcessCommand();
        ProcessCommand( const ProcessCommand& );
        const ProcessCommand& operator=( const ProcessCommand& );


        // Variables required to perform the processing.
        Event m_event;
        std::vector<IEventProcessor*>& m_processorList;
        WriteReadThreadLockable& m_processorListLock;
    };
};

#endif // !defined(_PROCESS_COMMAND_H_)
