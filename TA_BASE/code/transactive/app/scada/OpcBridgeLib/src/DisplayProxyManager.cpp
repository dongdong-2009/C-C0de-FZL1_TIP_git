/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// DisplayProxyManager.cpp: implementation of the DisplayProxyManager class.
//
//////////////////////////////////////////////////////////////////////
#include "ace/OS.h"
#include "app/scada/OpcBridgeLib/src/DisplayProxyManager.h"
#include "app/scada/OpcBridgeLib/src/DataPointAgentProxy.h"
#include "app/scada/OpcBridgeLib/src/DisplayDataPointProxy.h"
#include "app/scada/OpcBridgeLib/src/DataNodeAgentProxy.h"
#include "app/scada/OpcBridgeLib/src/DisplayDataNodeProxy.h"
#include "app/scada/OpcBridgeLib/src/AbstractDataObjectAttributes.h"
#include "app/scada/OpcBridgeLib/src/TagThread.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/RunParams.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{
    DisplayProxyManager* DisplayProxyManager::m_instance = NULL;
    TA_Base_Core::ReEntrantThreadLockable    DisplayProxyManager::m_singletonLock;
    
    DisplayProxyManager* DisplayProxyManager::getInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        if (NULL == m_instance)
        {
            try
            {
                m_instance = new DisplayProxyManager();
            }
            catch (...) 
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception caught during create DisplayProxyManager instance");
            }

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DisplayProxyManager instance created");
        }

        return m_instance;
    }

    void DisplayProxyManager::removeInstance()
    {
        TA_THREADGUARD( m_singletonLock );
        
        if (NULL != m_instance)
        {
            try
            {
                delete m_instance;
                m_instance = NULL;
            }
            catch (...) 
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception caught during delete DisplayProxyManager instance");
            }
        
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DisplayProxyManager instance deleted");
        }
    }
    
    DisplayProxyManager::DisplayProxyManager()
        :m_isSynchronizing(false),
		 m_numWorkers(10),
		 m_tagWokerInited(false)
    {
        m_dpAgentMap.clear();
        m_dnAgentMap.clear();

		std::string tagThreadCount = TA_Base_Core::RunParams::getInstance().get("TagThreadCount");
		if(!tagThreadCount.empty())
		{
			LONG tagThreadCnt = atoi(tagThreadCount.c_str());
			if (tagThreadCnt > 0 && tagThreadCnt <= 30)
			{
				m_numWorkers = (unsigned int)tagThreadCnt;
			}
		}
    }

    DisplayProxyManager::~DisplayProxyManager()
    {
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DisplayProxyManager delete DataPointAgentProxies");

            TA_Base_Core::ThreadGuard lock(m_dpAgentMapLock);
            
			DpAgentMap::iterator itAgent;
            for (itAgent = m_dpAgentMap.begin(); itAgent != m_dpAgentMap.end(); ++itAgent)
            {
                try
                {
                    delete (itAgent->second);
                }
                catch (...) 
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception caught during delete datapoint agent proxies");
                }
            }

            m_dpAgentMap.clear();
        }
        
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DisplayProxyManager delete DataNodeAgentProxies");

            TA_Base_Core::ThreadGuard lock(m_dnAgentMapLock);
           
			DnAgentMap::iterator itAgent;
            for (itAgent = m_dnAgentMap.begin(); itAgent != m_dnAgentMap.end(); ++ itAgent)
            {
                try
                {
                    delete (itAgent->second);
                }
                catch (...) 
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception caught during delete datanode agent proxies");
                }
            }

            m_dnAgentMap.clear();
        }
        
		
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DisplayProxyManager delete TagThreads");

			TA_THREADGUARD(m_tagWokerLock);
			
			for (unsigned int i = 0; i < m_workers.size(); ++i)
			{
				delete m_workers[i];
			}
			m_workers.clear();
		}

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DisplayProxyManager instance deleted");
    }

    void DisplayProxyManager::startSynchronise()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DisplayProxyManager started synchronising");

        m_isSynchronizing = true;

        {
            TA_Base_Core::ThreadGuard lock(m_dpAgentMapLock);

            DpAgentMap::iterator itAgent;
            for (itAgent = m_dpAgentMap.begin(); itAgent != m_dpAgentMap.end(); ++ itAgent)
            {
                itAgent->second->startSynchronise();
            }
        }

        {
            TA_Base_Core::ThreadGuard lock(m_dnAgentMapLock);

            DnAgentMap::iterator itAgent;
            for (itAgent = m_dnAgentMap.begin(); itAgent != m_dnAgentMap.end(); ++ itAgent)
            {
                itAgent->second->startSynchronise();
            }
        }
    }
    
    void DisplayProxyManager::stopSynchronise()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--DisplayProxyManager stopped synchronising");

        m_isSynchronizing = false;
        
        {
            TA_Base_Core::ThreadGuard lock(m_dpAgentMapLock);

            DpAgentMap::iterator itAgent;
            for (itAgent = m_dpAgentMap.begin(); itAgent != m_dpAgentMap.end(); ++ itAgent)
            {
                itAgent->second->stopSynchronise();
            }
        }
        
        {
            TA_Base_Core::ThreadGuard lock(m_dnAgentMapLock);

            DnAgentMap::iterator itAgent;
            for (itAgent = m_dnAgentMap.begin(); itAgent != m_dnAgentMap.end(); ++ itAgent)
            {
                itAgent->second->stopSynchronise();
            }
        }
	}

	void DisplayProxyManager::preSetDnAttributesForAgents(std::map<unsigned long, std::vector<DataNodeAttributes*> >& dnAgents)
	{
		TA_Base_Core::ThreadGuard lock(m_dnAgentMapLock);
		std::map<unsigned long, std::vector<DataNodeAttributes*> >::iterator itAgents;
		for (itAgents = dnAgents.begin(); itAgents != dnAgents.end(); ++itAgents)
		{
			std::vector<DataNodeAttributes*>& dnAttributes = itAgents->second;
			if (dnAttributes.size() > 0)
			{
				unsigned long agentKey = itAgents->first;
				unsigned long locationKey = dnAttributes[0]->getLocationKey();
				DataNodeAgentProxy* pAgent = new DataNodeAgentProxy(agentKey, locationKey, *this);
				pAgent->preSetDnAttribute(dnAttributes);
				m_dnAgentMap[agentKey] = pAgent;
			}
		}
	}

	void DisplayProxyManager::preSetDpAttributesForAgents(std::map<unsigned long, std::vector<DataPointAttributes*> >& dpAgents)
	{
		TA_Base_Core::ThreadGuard lock(m_dpAgentMapLock);
		std::map<unsigned long, std::vector<DataPointAttributes*> >::iterator itAgents;
		for (itAgents = dpAgents.begin(); itAgents != dpAgents.end(); ++itAgents)
		{
			std::vector<DataPointAttributes*>& dpAttributes = itAgents->second;
			if (dpAttributes.size() > 0)
			{
				unsigned long agentKey = itAgents->first;
				unsigned long locationKey = dpAttributes[0]->getLocationKey();
				DataPointAgentProxy* pAgent = new DataPointAgentProxy(agentKey, locationKey, *this);
				pAgent->preSetDpAttribute(dpAttributes);
				m_dpAgentMap[agentKey] = pAgent;
			}
		}
	}

	void DisplayProxyManager::dynamicSetDnAttribute(DataNodeAttributes* dnAttribute)
	{
		DataNodeAgentProxy* dnAgent = NULL;
		unsigned long agentKey = dnAttribute->getAgentKey();
		unsigned long locationKey = dnAttribute->getLocationKey();
		unsigned long entityKey = dnAttribute->getEntityKey();

		TA_Base_Core::ThreadGuard lock(m_dnAgentMapLock);

		DnAgentMap::iterator itDnAgent = m_dnAgentMap.find(agentKey);
		if (itDnAgent != m_dnAgentMap.end())
		{
			dnAgent = itDnAgent->second;
		}
		else
		{
			dnAgent = new DataNodeAgentProxy(agentKey, locationKey, *this);
			m_dnAgentMap.insert(std::make_pair(agentKey, dnAgent));
			if ( m_isSynchronizing ) dnAgent->startSynchronise();
		}

		if (dnAgent != NULL)
		{
			dnAgent->dynamicSetDnAttribute(entityKey, dnAttribute);
		}
	}

	void DisplayProxyManager::dynamicSetDpAttribute(DataPointAttributes* dpAttribute)
	{
		DataPointAgentProxy* dpAgent = NULL;
		unsigned long agentKey = dpAttribute->getAgentKey();
		unsigned long locationKey = dpAttribute->getLocationKey();
		unsigned long entityKey = dpAttribute->getEntityKey();
		TA_Base_Core::ThreadGuard lock(m_dpAgentMapLock);

		DpAgentMap::iterator itDpAgent = m_dpAgentMap.find(agentKey);
		if (itDpAgent != m_dpAgentMap.end())
		{
			dpAgent = itDpAgent->second;
		}
		else
		{
			dpAgent = new DataPointAgentProxy(agentKey, locationKey, *this);
			m_dpAgentMap.insert(std::make_pair(agentKey, dpAgent));
			if ( m_isSynchronizing ) dpAgent->startSynchronise();
		}

		if (dpAgent != NULL)
		{
			dpAgent->dynamicSetDpAttribute(entityKey, dpAttribute);	
		}
	}

	void DisplayProxyManager::queueItemUnique( unsigned long itemId, AbstractDataObjectAttributes * item)
	{
		if ( !m_tagWokerInited )
		{
			initTagThreads();
		}

		if (NULL != item)
		{
			unsigned int worker = itemId % m_numWorkers;
			m_workers[worker]->insertUniqueItem(item);
		}
	}

	unsigned int DisplayProxyManager::getUpdateCnt()
	{
		unsigned int cnt = 0;
		for (unsigned int i = 0; i < m_workers.size(); ++i)
		{
			cnt += m_workers[i]->getUpdateCnt();
		}
		return cnt;
	}

	void DisplayProxyManager::initTagThreads()
	{
		TA_THREADGUARD(m_tagWokerLock);
		
		if (m_tagWokerInited)
		{
			return;
		}

		for (unsigned int i = 0; i < m_numWorkers; ++i)
		{
			m_workers.push_back(new TagThread);
		}

		m_tagWokerInited = true;
	}
}
