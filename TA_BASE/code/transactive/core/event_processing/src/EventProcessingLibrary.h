/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/EventProcessingLibrary.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * The EventProcessingLibrary implements the Composite pattern to allow a single
  * method call to result in the same method being called on all of the registered
  * processor objects. It also implements the Singleton pattern to ensure that only
  * one event processing library exists in the system.
  */

#if !defined(_EVENT_PROCESSING_LIBRARY_H_)
#define _EVENT_PROCESSING_LIBRARY_H_

#include <vector>

#include "ace/Singleton.h"
#include "ace/Timer_Heap.h"

#include "core/event_processing/src/IEventProcessor.h"
#include "core/event_processing/src/CommandScheduler.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"

namespace TA_Base_Core
{
    class EventProcessingLibraryImpl : public IEventProcessor
    {
    public:
        // ACE Singleton is a friend as it needs to call constructor.
        friend class ACE_Singleton<EventProcessingLibraryImpl,ACE_Recursive_Thread_Mutex>;

        /**
         * addProcessor
         *
         * Registers the provided processor with the event processing library. Note that
         * the event processing library takes ownership of the processor once it is
         * registered!
         *
         * @param processor The event processor to register.
         */
        virtual void addProcessor( IEventProcessor* processor );

		void cleanUp();

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
        virtual bool processEvent( Event& event );


    private:
        // Hidden methods.
        EventProcessingLibraryImpl();
        EventProcessingLibraryImpl( const EventProcessingLibraryImpl& );
        EventProcessingLibraryImpl& operator=( const EventProcessingLibraryImpl& );
        virtual ~EventProcessingLibraryImpl();


        // The list that will be used to store the event processors.
        std::vector<IEventProcessor*> m_processorList;

        // The lock used to guard against processor list corruption.
        WriteReadThreadLockable m_processorListLock;

        // The queue used to schedule processing events.
        std::auto_ptr<CommandScheduler> m_commandScheduler;
    };

    // Define the singleton version of the class.
    typedef ACE_Singleton<EventProcessingLibraryImpl,ACE_Recursive_Thread_Mutex> EventProcessingLibrary;
};

#endif // !defined(_EVENT_PROCESSING_LIBRARY_H_)

