/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datanode_library/src/SummaryTriggerTimer.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  *	The SummaryTriggerTimer thread is responsible 
  * for generating events and submitting
  * them.
  *
  *
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "bus/scada/datanode_library/src/SummaryTriggerTimer.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/datanode_library/src/DataNode.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ScadaProxyException.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	SummaryTriggerTimer::SummaryTriggerTimer(SummaryProcessor& summaryProcessor)
	:
    m_summaryProcessor(summaryProcessor),
    m_entityMap(NULL),
    m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance())
    {
        unsigned long summaryInterval = 0;
        
        // check if polling period is set as a runtime parameter
        if ( true == TA_Base_Core::RunParams::getInstance().isSet("SummaryInterval"))
        {
            // get the polling period from command line
            summaryInterval = atoi(TA_Base_Core::RunParams::getInstance().get ("SummaryInterval").c_str());
            
        }
        
        // if polling period is not defined
        if (summaryInterval == 0)
        {
            // default to 10s
            summaryInterval = 10000;
        }

        m_timerUtility.startPeriodicTimeOutClock(this, summaryInterval, false);
    }

	SummaryTriggerTimer::~SummaryTriggerTimer()
	{
        m_timerUtility.stopPeriodicTimeOutClock(this);
    }

    void SummaryTriggerTimer::setEntityMap(const EntityMap* entityMap)
    {
        m_entityMap = entityMap;
    }
    
    void SummaryTriggerTimer::timerExpired(long timerId, void* userData)
    {
        //didn't initialize yet
        if (m_entityMap == NULL)
        {
            return;
        }
        
        for (EntityMap::const_iterator itDataNode = m_entityMap->begin(); itDataNode != m_entityMap->end(); ++ itDataNode)
        {
            DataNode* dataNode = NULL;
            
            try
            {
                dataNode = dynamic_cast<DataNode*>(itDataNode->second);
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "dynamic_cast got an unknown exception");
            }
            
            TA_ASSERT(dataNode != NULL, "m_entityMap contains invalid pointer; Failed to cast into DataNode*");

            //if there are some entity updates, insert ProxyUpdate item
            std::map<ScadaEntityUpdateType, unsigned long> updates = dataNode->getProxyUpdateList();
            for (std::map<ScadaEntityUpdateType, unsigned long>::iterator itUpdate = updates.begin(); itUpdate != updates.end(); ++ itUpdate)
            { 
                //for each update type, insert one item
                boost::shared_ptr<SummaryRequest> request (new SummaryRequest(dataNode->getEntityKey(), itUpdate->first));
                m_summaryProcessor.queueItemUnique(request);

                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "Queue a summary request for proxy update of %u, %d",
                    dataNode->getEntityKey(), itUpdate->first);
            }

            //if there are some alarm ack updates, insert AlarmAckUpdate item
            if (dataNode->isAlarmAckStateChanged() == true)
            {
                TA_Base_Bus::AlarmState dpAlarmState;
                boost::shared_ptr<SummaryRequest> request (new SummaryRequest(dataNode->getEntityKey(), SummaryRequest::AlarmAckUpdate));
                m_summaryProcessor.queueItemUnique(request);
                
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "Queue a summary request for alarm ack state of %u",
                    dataNode->getEntityKey());
            }
        }
        
    }
    
}
