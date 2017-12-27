/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/EventProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * The EventProcessor thread is responsible
  * for generating events and submitting
  * them.
  *
  */
#ifndef EVENTPROCESSOR_H
#define EVENTPROCESSOR_H

#include <string>

#include "core/threads/src/Thread.h"
#include "bus/scada/common_library/src/BlockingSingletonQueue.h"

template<class ITEM> class EventProcessor : public TA_Base_Core::Thread
{
	static const int MAX_BUNDLE_SIZE;
	static const unsigned int MAX_SEND_WAIT;

public:

	EventProcessor<ITEM> () : m_keepRunning(true)
	{
        m_eventQ = BlockingSingletonQueue<ITEM>::getInstance();
	};


	~EventProcessor<ITEM> () 
	{
        BlockingSingletonQueue<ITEM>::removeInstance( m_eventQ );
	};

	//
	// Thread derived functions to start/stop the
	// DataNodeEventProcessor thread
	//
	virtual void run() 
	{
        ITEM * newEvent;

		//15120. if m_keepRunning is not set to true, the processor can not support restart.
		m_keepRunning = true; 

        while ( m_keepRunning )
		{
            newEvent = NULL;

            try
            {
                // This call blocks until there is an event to process
                newEvent = m_eventQ->blockForItem();
                
                if (NULL != newEvent)
                {
                    processEvent( newEvent );
                    delete newEvent;
                    newEvent = NULL;
                }
            }
            // Catch all exceptions to prevent thread from dying prematurely
            catch( const TA_Base_Core::TransactiveException & te )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", te.what() );
            }
            catch( const std::exception & e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
            }

            // delete newEvent to prevent memory leak
            if (NULL != newEvent)
            {
                delete newEvent;
            }
		}
	};

	virtual void terminate()
	{
		m_keepRunning = false;
		m_eventQ->unBlockQueue();
	};


protected:

	/**
     * DataNodeEventProcessor::processEvent()
     *
     * Adds the new event request to the current bundle
     *
     * @param newEvent	the request for the new event
     *
     */
	virtual void processEvent( ITEM * newEvent ) = 0;

	BlockingSingletonQueue<ITEM>*	m_eventQ;

private:

	bool m_keepRunning;
};

#endif
