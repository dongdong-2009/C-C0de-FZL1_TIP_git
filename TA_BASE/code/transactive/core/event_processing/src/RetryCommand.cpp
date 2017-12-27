/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/event_processing/src/RetryCommand.cpp $
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

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/event_processing/src/CommandScheduler.h"
#include "core/event_processing/src/IEventProcessor.h"
#include "core/event_processing/src/RetryCommand.h"
#include "core/utilities/src/DebugUtil.h"

namespace
{
    const unsigned char RETRY_PRIORITY(0);
};

namespace TA_Base_Core
{
    RetryCommand::RetryCommand( Event& event, IEventProcessor& processor, unsigned char retryCount, unsigned long delay ) :
        AbstractCommand( delay ), m_event(event), m_processor(processor),
        m_retryCount(retryCount)
    {
    }


    RetryCommand::~RetryCommand()
    {
    }


    void RetryCommand::execute( CommandScheduler& scheduler )
    {
        FUNCTION_ENTRY( "execute" );

        // Execute the command and retry again if it fails.
        if ( !m_processor.processEvent( m_event ) && 0<m_retryCount )
        {
            AbstractCommand* command = new RetryCommand( m_event, m_processor, m_retryCount-1, getDelay() );
            scheduler.push( command );
        }

        FUNCTION_EXIT;
    }
};
