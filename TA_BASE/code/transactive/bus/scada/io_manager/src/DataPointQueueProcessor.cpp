/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/DataPointQueueProcessor.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "DataPointQueueProcessor.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	DataPointQueueProcessor::DataPointQueueProcessor(int numberOfWorkers)
	{
		if (numberOfWorkers < 5)
		{
			numberOfWorkers = 5;
		}

		m_updateProcessorPool =  
			new TA_Base_Core::QueueProcessorPool<DataPointAndValuePair, 
				TA_Base_Core::QueueProcessorWorker<DataPointAndValuePair> >(numberOfWorkers, *this);
	}

	DataPointQueueProcessor::~DataPointQueueProcessor()
	{
		delete m_updateProcessorPool;
        m_updateProcessorPool = NULL;
	}

	void DataPointQueueProcessor::submitData(DataPoint* dp, DataPointState & dpState)
	{
		boost::shared_ptr<DataPointAndValuePair> update (new DataPointAndValuePair(dp, dpState));
        m_updateProcessorPool->queueItem(dp->getEntityKey(), update);
	}

	void DataPointQueueProcessor::queueProcessorPoolCallback( boost::shared_ptr<DataPointAndValuePair> newEvent )
    {
        try
        {
            newEvent.get()->dataPoint->updateFieldState(newEvent.get()->dpState);
        }
        catch (...) 
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught when call notifier's notifyObservers");
        }
    }
}
