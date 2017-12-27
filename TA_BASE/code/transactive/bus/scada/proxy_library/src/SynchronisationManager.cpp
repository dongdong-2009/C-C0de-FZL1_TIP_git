/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/SynchronisationManager.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is responsible for managing the agent interfaces
  * and ensuring that updates are provided to the observers 
  * (DataPointProxyManager and DataNodeProxyManagers)
  *
  */

///////////////////////////////////////////////////////////
//  SynchronisationManager.cpp
//  Implementation of the Class SynchronisationManager
//  Created on:      28-Jun-2004 09:39:02 AM
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning (disable:4284 4786 4146 4018 4250 4290)
#endif // defined (_MSC_VER)

#include <map>

#include "bus/scada/proxy_library/src/SynchronisationManager.h"
#include "bus/scada/proxy_library/src/AgentSynchronisationInterface.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

namespace TA_Base_Bus
{

    const std::string SynchronisationManager::DN_AGENT_NAME_POSTFIX = "DnAgent";	
    const std::string SynchronisationManager::DP_AGENT_NAME_POSTFIX = "DpAgent";	
	/**
	 * Constructor
	 * @param updateCallback
	 * 
	 */
	SynchronisationManager::SynchronisationManager()
    :
	m_localEntityMap( NULL ),
    m_isSynchronising( false )
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}


	SynchronisationManager::~SynchronisationManager()
	{
		FUNCTION_ENTRY("Destructor");
		this->stopSynchronisation();
		this->removeAllAgentInterfaces();
		FUNCTION_EXIT;
	}

	void SynchronisationManager::add(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("add");		
		boost::shared_ptr<AgentSynchronisationInterface> agentInterface = 
			m_agentList.get(getActualAgentName(entityData->getType(), entityData->getAgentName()));

		if (NULL == agentInterface.get())
		{
			agentInterface = addAgentInterface(entityData);
			agentInterface->enableSynchronisation ( m_isSynchronising );
		}

		TA_ASSERT(NULL != agentInterface.get(), "Could not create agent sync interface");

		// Add entity to the agent interface job queue
		agentInterface->add(entityData->getKey());

		FUNCTION_EXIT;
	}

	void SynchronisationManager::remove(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("remove");        
		boost::shared_ptr<AgentSynchronisationInterface> agentInterface = 
			m_agentList.get(getActualAgentName(entityData->getType(), entityData->getAgentName()));

		if (NULL != agentInterface.get())
		{
			agentInterface->remove(entityData->getKey());
		}

		FUNCTION_EXIT;
	}


	boost::shared_ptr<AgentSynchronisationInterface> SynchronisationManager::addAgentInterface (TA_Base_Core::IEntityDataPtr entityData)
	{
		// Create/get(if existed) agent Interface
		boost::shared_ptr<AgentSynchronisationInterface> agentInterface;

		agentInterface = createAgentInterface(entityData);

		agentInterface = m_agentList.insert(getActualAgentName(entityData->getType(), entityData->getAgentName()), agentInterface);

		TA_ASSERT(NULL != agentInterface.get(), "Agent interface not added");

		
		return agentInterface;
	}

	void SynchronisationManager::startSynchronisation()
	{
		FUNCTION_ENTRY("startSynchronisation");
		TA_Base_Core::ThreadGuard guard( m_lock );
		if(m_isSynchronising == false)
		{
			m_isSynchronising = true;

			AgentList::MapVector interfaceVector = m_agentList.getAll();

		// Tell all agent (bulk need to start polling)
			AgentList::MapVector::iterator agentIT;
			for ( agentIT = interfaceVector.begin(); agentIT != interfaceVector.end(); ++agentIT )
			{
				(*agentIT)->enableSynchronisation(true);
			}
		}

		FUNCTION_EXIT;
	}


	void SynchronisationManager::stopSynchronisation()
	{
		FUNCTION_ENTRY("stopSynchronisation");
		TA_Base_Core::ThreadGuard guard( m_lock );
		if ( true == m_isSynchronising )
		{
			m_isSynchronising = false;

			AgentList::MapVector interfaceVector = m_agentList.getAll();

			// Tell all agent (bulk need to stop polling)
			AgentList::MapVector::iterator agentIT;
			for ( agentIT = interfaceVector.begin(); agentIT != interfaceVector.end(); ++agentIT )
			{
				(*agentIT)->enableSynchronisation(false);
			}
		}
	}


	void SynchronisationManager::removeAllAgentInterfaces()
	{
		FUNCTION_ENTRY("removeAllAgentInterfaces");

		/*AgentList::MapVector interfaceVector = m_agentList.getAll();

		// Tell all agent (bulk need to start polling)
		AgentList::MapVector::iterator agentIT;
		for ( agentIT = interfaceVector.begin(); agentIT != interfaceVector.end(); ++agentIT )
		{
			(*agentIT)->terminateAndWait();
		}*/
		this->stopSynchronisation();

		m_agentList.clear();

		FUNCTION_EXIT;
	}

	
    void SynchronisationManager::registerLocalEntities(std::string agentName, const std::map<unsigned long, IEntity*>* entityMap)
    {
        FUNCTION_ENTRY("registerLocalEntities");
        m_localEntityMap = entityMap;
        FUNCTION_EXIT;
    }


    bool SynchronisationManager::isLocal(unsigned long entityKey)
    {
        FUNCTION_ENTRY("isLocal");
        if (m_localEntityMap!=NULL)
        {
            if (m_localEntityMap->find(entityKey)!=m_localEntityMap->end())
            {
                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
        return false;
    }

    TA_Base_Bus::IEntity* SynchronisationManager::getLocalEntity(unsigned long entityKey)
    {
        FUNCTION_ENTRY("getLocalEntity");
        if (m_localEntityMap!=NULL)
        {
            if (m_localEntityMap->find(entityKey)!=m_localEntityMap->end())
            {
                FUNCTION_EXIT;
                return m_localEntityMap->find(entityKey)->second;
            }
        }
        
        FUNCTION_EXIT;
        return NULL;
    }

    std::string SynchronisationManager::getActualAgentName(std::string entityType, std::string entityAgentName)
    {
        FUNCTION_ENTRY("getActualAgentName");
        std::string actualAgentName;
        if ( entityType.compare(TA_Base_Core::DataNodeEntityData::getStaticType()) ==0 )
        {
            actualAgentName = entityAgentName + DN_AGENT_NAME_POSTFIX;
        }
        else if ( entityType.compare(TA_Base_Core::DataPointEntityData::getStaticType()) == 0 ) 
        {
            actualAgentName = entityAgentName + DP_AGENT_NAME_POSTFIX;
        }
        else
        {
		    TA_ASSERT( false, "The entity in the proxy library is not either a datapoint nor a datanode!");
        }
        FUNCTION_EXIT;
        return actualAgentName;
    }

};
