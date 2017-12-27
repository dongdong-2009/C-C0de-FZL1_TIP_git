/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUOutputWorker.cpp $
  * @author  Raymond Pau
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2017/05/25 10:19:08 $
  * Last modified by : $Author: yong.liu $
  *
  *
  * The RTUOutputWorker thread is responsible
  * for sending DataPointWriteRequest to the actual hardware
  *
  */
#include <sstream>

#include "bus/scada/rtu_library/src/RTUOutputWorker.h"
#include "bus/scada/rtu_library/src/RTU.h"
#include "bus/scada/rtu_library/src/StationSystemOutputWriter.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"

#include "core/sockets/src/SocketOptions.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	RTUOutputWorker*              RTUOutputWorker::m_singleton = NULL;
    unsigned int                    RTUOutputWorker::m_refCounter = 0;
    TA_Base_Core::ReEntrantThreadLockable RTUOutputWorker::m_singletonLock;

	RTUOutputWorker* RTUOutputWorker::getInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        if ( m_singleton == NULL )
        {
            m_singleton = new RTUOutputWorker();
        }

        // increment reference counter
        m_refCounter++;

        return m_singleton;
    }   

    void RTUOutputWorker::removeInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        TA_ASSERT(m_refCounter > 0, "m_refCounter <= 0");

        // decrement reference counter
        --m_refCounter;

        // if not referenced anywhere
        if ( 0 == m_refCounter )
        {
            if ( m_singleton != NULL )
            {
                delete m_singleton;
                m_singleton = NULL;
            }
        }
    }

	RTUOutputWorker::RTUOutputWorker()
    :
    // m_inService( false ),
    m_isInControlMode(false),
	m_writeQ( NULL )
    {
        FUNCTION_ENTRY("Constructor");

		m_rtus.clear();

		// get instance to the write queue
		m_writeQ = TA_Base_Bus::DataPointWriteQueue::getInstance();
        start();
		
        FUNCTION_EXIT;
	}


	RTUOutputWorker::~RTUOutputWorker()
	{
        FUNCTION_ENTRY("Destructor");

		clearWriteQueue();

		// remove instance of the write queue
		if ( NULL != m_writeQ )
		{
			TA_Base_Bus::DataPointWriteQueue::removeInstance( m_writeQ );
		}
		
		m_rtus.clear();

        FUNCTION_EXIT;
	}
    

    void RTUOutputWorker::addRTU(RTU& rtu)
    {
		m_rtus.push_back(&rtu);
	}

    void RTUOutputWorker::setToControl()
    {
        m_isInControlMode = true;
    }

    
    void RTUOutputWorker::setToMonitor()
    {
        m_isInControlMode = false;
    }

	void RTUOutputWorker::processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest )
	{
        FUNCTION_ENTRY("processEvent");

        //Only process it in control mode.
        if (false == m_isInControlMode)
        {
            FUNCTION_EXIT;
            return;
        }

        bool requestProcessed = false;

		for ( std::vector<RTU *>::iterator itr = m_rtus.begin(); 
              itr != m_rtus.end() && requestProcessed == false; 
              ++itr )
		{
			requestProcessed = (* itr)->processWriteRequest( *newDataPointWriteRequest);
		}

        if ( false == requestProcessed )
        {
            // update control status of the data point to indicate that
            // the output command has failed
            
            TA_Base_Bus::DataPoint * dp = newDataPointWriteRequest->getDataPoint();            
            TA_ASSERT( 0 != dp, "RTUOutputWorker::processEvent: newDataPointWriteRequest contains a NULL datapoint." );

            dp->updateControlState( TA_Base_Bus::comms_failed );

            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "No output worker found to process write request for datapoint %s value: %s",
                newDataPointWriteRequest->getDataPoint()->getDataPointName().c_str(),
                newDataPointWriteRequest->getValue().getValueAsString().c_str() );
        }

        FUNCTION_EXIT;
    }

	void RTUOutputWorker::clearWriteQueue()
	{
        // if the queue is initialised
        if ( NULL != m_writeQ )
        {
            // only proceed if there are output commands waiting for processing
            // for each of the requests in the queue
            while ( m_writeQ->getSize() > 0 )
            {
                // remove it from the queue
                TA_Base_Bus::DataPointWriteRequest * writeReq = m_writeQ->blockForItem();
                
                if ( NULL != writeReq )
                {
                    TA_Base_Bus::DataPoint * point = writeReq->getDataPoint();
                    
                    // update the data point control status to failure
                    point->updateControlState( TA_Base_Bus::comms_failed ); //wenching++ (TD13999)
                    
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                        "Unable to establish RTU comms...Have removed output command from data point %s",
                        point->getDataPointName().c_str());
                }
                
                delete writeReq;
                writeReq = NULL;
            }
        }
    }
}
