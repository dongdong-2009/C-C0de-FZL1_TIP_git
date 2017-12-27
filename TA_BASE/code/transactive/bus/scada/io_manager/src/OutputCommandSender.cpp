/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/OutputCommandSender.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include <sstream>
#include "OutputCommandSender.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	OutputCommandSender::OutputCommandSender()
    :
    m_inService( false ),
	m_writeQ( NULL )
    {
		m_writeQ = TA_Base_Bus::DataPointWriteQueue::getInstance();
		start();
	}


	OutputCommandSender::~OutputCommandSender()
	{
		clearWriteQueue();
		if ( NULL != m_writeQ )
		{
			TA_Base_Bus::DataPointWriteQueue::removeInstance( m_writeQ );
		}		
	}

	void OutputCommandSender::setDataPointSenderMap(const std::map<unsigned long, IProtocolIOBase*>& dpToSender)
	{
		m_dpToSenderMap = dpToSender;
	}

	void OutputCommandSender::processEvent( TA_Base_Bus::DataPointWriteRequest * newDataPointWriteRequest )
	{
        FUNCTION_ENTRY("processEvent");

        bool requestProcessed = false;

		TA_Base_Bus::DataPoint * dp = newDataPointWriteRequest->getDataPoint();
		if (0 == dp)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "can not get datapoint pointer from datapoint write request");
			return ;
		}

		unsigned long dpKey = dp->getEntityKey();
		std::map<unsigned long, IProtocolIOBase*>::const_iterator it = m_dpToSenderMap.find(dpKey);
	
		if (m_dpToSenderMap.end() != it)
		{
			requestProcessed = it->second->processWriteRequest(*newDataPointWriteRequest);
		}
		else
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "No output worker process write request for datapoint %s value: %s",
                newDataPointWriteRequest->getDataPoint()->getDataPointName().c_str(),
                newDataPointWriteRequest->getValue().getValueAsString().c_str() );
            
			dp->updateControlState( TA_Base_Bus::comms_failed );
        }

        FUNCTION_EXIT;
    }

	void OutputCommandSender::clearWriteQueue()
	{
        if ( NULL != m_writeQ )
        {
			if (m_writeQ->getSize() > 0)
			{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "there are %d datapoint write request not processed, when stop this processor",
				m_writeQ->getSize());
			}
            while ( m_writeQ->getSize() > 0 )
            {
                // remove it from the queue
                TA_Base_Bus::DataPointWriteRequest * writeReq = m_writeQ->blockForItem();
                
                if ( NULL != writeReq )
                {
                    TA_Base_Bus::DataPoint * point = writeReq->getDataPoint();
                    point->updateControlState( TA_Base_Bus::comms_failed );
                    
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                        "Unable to establish comms...Have removed output command from data point %s",
                        point->getDataPointName().c_str());
                }
                
                delete writeReq;
                writeReq = NULL;
            }
        }
    }
}
