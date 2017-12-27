/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/EventProcessingLibrary.cpp $
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

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/event_processing/src/AbstractCommand.h"
#include "core/event_processing/src/EventLoggingProcessor.h"
#include "core/event_processing/src/EventProcessingLibrary.h"
#include "core/event_processing/src/ProcessCommand.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    EventProcessingLibraryImpl::EventProcessingLibraryImpl()
    {
        // Register any default event processors.

        // Log all events.
        addProcessor( new EventLoggingProcessor() );

        // Create the command scheduler.
        m_commandScheduler = std::auto_ptr<CommandScheduler>( new CommandScheduler() );
    }


	void EventProcessingLibraryImpl::cleanUp()
	{
		// TD11793: First destroy the command scheduler to ensure no processors are used
		// after they are destroyed.
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventProcessingLibraryImpl::cleanUp start");
		delete m_commandScheduler.release();

		// Get a write lock.
		ThreadGuard guard( m_processorListLock );

		// Destroy all event processors.
		std::vector<IEventProcessor*>::iterator it;
		for ( it=m_processorList.begin(); it!=m_processorList.end(); it++ )
		{
			delete *it;
		}
		m_processorList.clear();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventProcessingLibraryImpl::cleanUp end");
	}
    EventProcessingLibraryImpl::~EventProcessingLibraryImpl()
    {
//         // TD11793: First destroy the command scheduler to ensure no processors are used
//         // after they are destroyed.
//         delete m_commandScheduler.release();
// 
//         // Get a write lock.
//         ThreadGuard guard( m_processorListLock );
// 
//         // Destroy all event processors.
//         std::vector<IEventProcessor*>::iterator it;
//         for ( it=m_processorList.begin(); it!=m_processorList.end(); it++ )
//         {
//             delete *it;
//         }
//         m_processorList.clear();
		cleanUp();
    }


    void EventProcessingLibraryImpl::addProcessor( IEventProcessor* processor )
    {
        FUNCTION_ENTRY( "addProcessor" );

        TA_ASSERT( NULL != processor, "Cannot register NULL processor." );

        // Get a write lock.
        ThreadGuard guard( m_processorListLock );

        // Now add the processor.
        m_processorList.push_back( processor );

        FUNCTION_EXIT;
    }


    unsigned char EventProcessingLibraryImpl::getNumRetries()
    {
        TA_ASSERT( false, "getNumRetries should not be called on EventProcessingLibrary" );
        return 0;
    }


    unsigned long EventProcessingLibraryImpl::getRetryDelay()
    {
        TA_ASSERT( false, "getRetryDelay should not be called on EventProcessingLibrary" );
        return 0;
    }


    bool EventProcessingLibraryImpl::processEvent( Event& event )
    {
        FUNCTION_ENTRY( "processEvent" );

        // Create the command to execute.
        AbstractCommand* command = new ProcessCommand( event, m_processorList, m_processorListLock );

        // Add it to the queue.
        m_commandScheduler->push( command );

        // Always return true as processing is performed asynchronously.
        FUNCTION_EXIT;
        return true;
    }
}
