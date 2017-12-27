// HeartBeatWork.cpp: implementation of the HeartBeatWork class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>
#include "bus/scada/rtu_library/src/HeartBeatWorker.h"
#include "bus/scada/rtu_library/src/RTU.h"
#include "bus/scada/rtu_library/src/StationSystemOutputWriter.h"

#include "bus/scada/common_library/src/BlockingSingletonMap.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/sockets/src/SocketOptions.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"

using TA_Base_Core::DebugUtil;

namespace
{
    bool isSystemSessionID( const std::string& sessionID )//limin++, SystemSession should not access db
    {
        bool ret = ( TA_Base_Core::SessionAccessFactory::getSuperSessionId() == sessionID ||
                     TA_Base_Bus::DPT_INTERNAL_SESSION_ID == sessionID ||
                     TA_Base_Bus::NotLoaded == sessionID );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[sessionID=%s][return=%d]", sessionID.c_str(), ret );
        return ret;
    }
}


namespace TA_Base_Bus
{


	HeartBeatWorker*              HeartBeatWorker::m_singleton = NULL;
    unsigned int                  HeartBeatWorker::m_refCounter = 0;
    TA_Base_Core::ReEntrantThreadLockable HeartBeatWorker::m_singletonLock;

	HeartBeatWorker* HeartBeatWorker::getInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        if ( m_singleton == NULL )
        {
            m_singleton = new HeartBeatWorker();
        }

        // increment reference counter
        m_refCounter++;

        return m_singleton;
    }   

    void HeartBeatWorker::removeInstance()
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


	HeartBeatWorker::HeartBeatWorker() 
        : 
        m_keepRunning(true),
        m_isInControlMode(false)
        // m_operationMode( TA_Base_Core::NotApplicable ) //limin, fail over issue
	{
		start();
	}

	HeartBeatWorker::~HeartBeatWorker()
	{
		m_rtus.clear();
	}

	void HeartBeatWorker::run()
	{
		TA_Base_Bus::DataPointWriteRequest * newHeartBeat = NULL;
		
		m_keepRunning = true; 
		
		while ( m_keepRunning )
		{
			newHeartBeat = NULL;
			
			try
			{
				newHeartBeat = TA_Base_Bus::DataPointWriteMap::getInstance()->blockForItem();
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "get new HeartBeart request from queue");
                if ( NULL != newHeartBeat )
                {
                    // if ( TA_Base_Core::Control == m_operationMode ) //limin, failover issue
                    if (true == m_isInControlMode)
                    {
                        processEvent( newHeartBeat );
                    }
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
			
			if (NULL != newHeartBeat)
			{
				delete newHeartBeat;
			}

			TA_Base_Core::Thread::sleep( 10 );
		}
	};


	void HeartBeatWorker::terminate()
	{
		m_keepRunning = false;
        TA_Base_Bus::DataPointWriteMap::getInstance()->unBlockQueue();
	};


	void HeartBeatWorker::processEvent( TA_Base_Bus::DataPointWriteRequest * newHeartBeat )
	{
        FUNCTION_ENTRY("processEvent");
		
        bool requestProcessed = false;
		
		for ( std::vector<RTU *>::iterator itr = m_rtus.begin(); itr != m_rtus.end() && requestProcessed == false; ++itr )
		{
			if((* itr)->processWriteRequest( *newHeartBeat))
			{
				requestProcessed = true;
			}
		}
		
        if ( false == requestProcessed )
        {
            TA_Base_Bus::DataPoint * dp = newHeartBeat->getDataPoint();            
			
			if ( dp )
			{
				dp->updateControlState( TA_Base_Bus::comms_failed, isSystemSessionID( newHeartBeat->getSessionID() ) );//limin++, SystemSession should not access db
			}			
        }
        
        FUNCTION_EXIT;		
	}


	void HeartBeatWorker::addRTU(RTU& rtu)
	{
		m_rtus.push_back(&rtu);
	}


    void HeartBeatWorker::setToControl()
    {
        // m_operationMode = TA_Base_Core::Control;
        m_isInControlMode = true;
    }

    
    void HeartBeatWorker::setToMonitor()
    {
        // m_operationMode = TA_Base_Core::Monitor;
        m_isInControlMode = false;
    }
}
