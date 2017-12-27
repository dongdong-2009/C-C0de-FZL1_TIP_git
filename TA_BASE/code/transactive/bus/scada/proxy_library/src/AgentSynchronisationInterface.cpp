/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/AgentSynchronisationInterface.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  */
///////////////////////////////////////////////////////////
//  AgentSynchronisationInterface.cpp
//  Implementation of the Class AgentSynchronisationInterface
//  Created on:      28-Jun-2004 03:51:38 PM
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/AgentSynchronisationInterface.h"
#include "bus/scada/proxy_library/src/DataPointProxyManager.h"
#include "bus/scada/proxy_library/src/DataNodeProxyManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	/**
	 * Constructor
	 * @param agentName
	 * 
	 */
	AgentSynchronisationInterface::AgentSynchronisationInterface(std::string agentName, unsigned long locationKey)
    :
    m_agentName(agentName),
    m_terminate(true),
    m_locationKey(locationKey),
    m_synchronisationEnabled(false),
	m_isConnected(true), //TD15939
	m_qualityBadPeriod(0),
	m_isLocal(false)
	{
		m_qualityBadPeriod = SET_BAD_QUALITY_RETRY_PERIOD_SECS;
		unsigned long qualityBadPeriod = atol(TA_Base_Core::RunParams::getInstance().get("ProxyQualityBadPeriod").c_str());
		// range is 10 second to 300 second
		if ( (qualityBadPeriod > SET_BAD_QUALITY_RETRY_PERIOD_SECS) && (qualityBadPeriod <= SET_BAD_QUALITY_RETRY_PERIOD_SECS * 30) )
		{
			m_qualityBadPeriod = qualityBadPeriod;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ProxyQualityBadPeriod is %u", m_qualityBadPeriod);
	}


	AgentSynchronisationInterface::~AgentSynchronisationInterface()
	{
	}

	void AgentSynchronisationInterface::enableSynchronisation(bool enabled)
	{
		if(enabled == m_synchronisationEnabled)
		{
			return;
		}
		//if enable start the thead, or sotp the thread
		if(true == m_terminate && true == enabled)
		{
			m_terminate = false;
			this->start();
		}
		else if(false == m_terminate && false == enabled)
		{
			m_terminate = true;
			this->terminateAndWait();
		}
		m_synchronisationEnabled = enabled;
	}

	void AgentSynchronisationInterface::add(unsigned long entityKey)
	{
        RequestItem item;

        item.requestType = Add;
        item.entityKey   = entityKey;

		//To improve the performance, for local entity,process it directly.
		if (false == m_isLocal)
		{
        	queueRequest(item);
		}
		else
		{
			processRequest(item);
		}
	}

	void AgentSynchronisationInterface::remove(unsigned long entityKey)
	{
        // We could check the queue for any pending adds and remove them
        // This might improve efficiency when queue is blocked and then 
        // agent starts working again


        //if there are alot of adds and removes in the queue, we shouldnt bother processing them
        //so we will clear the queue of all requests related to this point
        removeOldRequests(entityKey);
        RequestItem item;

        item.requestType = Remove;
        item.entityKey   = entityKey;

		//To improve the performance, for local entity,process it directly.
		if (false == m_isLocal)
		{
        	queueRequest(item);
		}
		else
		{
			processRequest(item);
		}

	}


	void AgentSynchronisationInterface::removeOldRequests(unsigned long entityKey)
	{
        TA_THREADGUARD(m_requestListLock);
        std::vector<RequestItem>::iterator it = m_requestList.begin();

        while(it != m_requestList.end())
        {
            if ( it->entityKey == entityKey )
            {
                it = m_requestList.erase(it);
            }
            else
            {
                it++;
            }
        }
	}

    
	AgentSynchronisationInterface::RequestItem AgentSynchronisationInterface::getNextRequest()
	{
	   RequestItem item;

	   TA_THREADGUARD(m_requestListLock);
	   if (m_requestList.size()<1)
	   {
		  item.requestType = EmptyQueue;
		  item.entityKey   = 0;
	   }
	   else
	   {
		  item = m_requestList.at(0);
		  m_requestList.erase(m_requestList.begin());
	   }

	   return item;
	}


	void AgentSynchronisationInterface::terminate()
	{
	   m_terminate = true;
	   m_requestListSemaphore.post();
	}


	void AgentSynchronisationInterface::queueRequest(RequestItem request)
	{
	   // Lock queue
	   TA_THREADGUARD(m_requestListLock);
	   m_requestList.push_back(request);

	   // Signal waiting thread
	   m_requestListSemaphore.post();
	}
}
