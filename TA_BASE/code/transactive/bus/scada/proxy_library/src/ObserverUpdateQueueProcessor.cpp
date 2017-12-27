/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class managers the list of observers to be notified when a data point
  * proxy has updated information
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ProxyObserverNotifier.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    ObserverUpdateQueueProcessor::ObserverUpdateQueueProcessor(int numberOfWorkers)
    :
    m_updateProcessorPool( NULL ),
    m_logStats( false ),
    m_isProcessing(false)
    {
        m_updateProcessorPool =  
			new TA_Base_Core::QueueProcessorPool<ObserverUpdate, 
				TA_Base_Core::QueueProcessorWorker<ObserverUpdate> >(numberOfWorkers, *this, false);

        // Enable logging of usageCount and observer notification
        if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "LogStatistics" ) )
        {
            m_logStats = true;
        }
    }


    ObserverUpdateQueueProcessor::~ObserverUpdateQueueProcessor()
    {
        delete m_updateProcessorPool;
        m_updateProcessorPool = NULL;
    }

	void ObserverUpdateQueueProcessor::startProcess()
	{
		if(false == m_isProcessing)
		{
			m_updateProcessorPool->startProcessing();
			m_isProcessing = true;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start Processing");
		}
	}

	void ObserverUpdateQueueProcessor::stopProcess()
	{
		if(true == m_isProcessing)
		{
			m_updateProcessorPool->stopProcessing();
			m_isProcessing = false;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Stop Processing");
		}
	}

    void ObserverUpdateQueueProcessor::queueNotification(IScadaProxyManager* manager, unsigned long entityKey, boost::shared_ptr<ProxyDynamicSyncUpdate> updateValue)
    {
        boost::shared_ptr<ObserverUpdate> update (new ObserverUpdate);
        update->manager = manager;
        update->updateValue = updateValue;
		unsigned long itemId = entityKey / 89 * 83 + (entityKey % 97);	// random the key.
        m_updateProcessorPool->queueItem(itemId, update);
    }
    
    void ObserverUpdateQueueProcessor::queueProcessorPoolCallback( boost::shared_ptr<ObserverUpdate> newEvent )
    {
        try
        {
            newEvent.get()->manager->processUpdateEvent(newEvent.get()->updateValue);
        }
        catch (...) 
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught when call notifier's notifyObservers");
        }
    }
}
