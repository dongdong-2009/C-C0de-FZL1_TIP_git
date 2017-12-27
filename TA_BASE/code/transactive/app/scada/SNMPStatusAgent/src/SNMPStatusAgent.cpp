/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/WXL1_TIP/3002/transactive/app/scada/SNMPStatusAgent/src/SNMPStatusAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2014/08/12 17:53:47 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	SNMPStatusAgent implementes the GenericAgent interface.  SNMPStatusAgent monitors and controls the
  * the entities associated with its process, such as PMS RTU and the DataPoint entities.
  *
  */

#include "app/scada/SNMPStatusAgent/src/SNMPStatusAgent.h"
#include "bus/scada/SNMP_library/src/SNMPProtocolManager.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/business_library/src/BusinessManager.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/SNMPStatusAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/PMSAgentStateUpdate_MessageTypes.h"
#include "core/message/types/PMSAgentAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/timers/src/StopwatchUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
	SNMPStatusAgent::SNMPStatusAgent (int argc, char* argv[])
	:
	m_operationMode (TA_Base_Core::NotApplicable),
	m_dpBulkPolledInterface (NULL),
	m_dpAccessInterface(NULL),
	m_dnBulkPolledInterface(NULL),
	m_dnAccessInterface(NULL),
	m_dataPointFactory (NULL),
	m_scadaProxyFactory (NULL),
	m_summaryProcessor(NULL),
	m_genericAgent (NULL),
	m_auditMessageSender (NULL),
	m_threadPoolSingletonManager (NULL),
	m_snmpIOManager(NULL)
	{
		m_snmpIOManager = new SNMPIOManager();
		m_datapoints.clear();
		m_dataNodes.clear();

		if (NULL == m_dataPointFactory)
		{
			m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
		}

		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

		TA_ASSERT ((0 != m_scadaProxyFactory), "SCADA Proxy Factory not created");

		m_genericAgent = new TA_Base_Bus::GenericAgent (argc, argv, this);

		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();

		m_scadaProxyFactory->registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());

		m_scadaProxyFactory->setProxiesToMonitorMode();

        std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
        for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
        {
            it->second->setToMonitorMode();
        }
        
		m_threadPoolSingletonManager = TA_Base_Core::ThreadPoolSingletonManager::getInstance();

        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());

	}


	SNMPStatusAgent::~SNMPStatusAgent()
	{
		if(m_snmpIOManager != NULL)
		{
			m_snmpIOManager->setOperationMode(TA_Base_Core::Monitor);
			delete m_snmpIOManager;
			m_snmpIOManager = NULL;
		}

		m_scadaProxyFactory->setProxiesToMonitorMode();

		// clean up the list containing DataPoints instances created by
		// this Agent, without deleting the DataPoint instances themselves
		// since they are owned by the GenericAgent
		m_datapoints.clear();
        m_dataNodes.clear();

		if (NULL != m_dataPointFactory)
		{
			delete m_dataPointFactory;
			m_dataPointFactory = NULL;
		}

		if (NULL != m_auditMessageSender)
		{
			delete m_auditMessageSender; 
			m_auditMessageSender = NULL;
		}
		
		if ( NULL != m_dpBulkPolledInterface )
		{
            delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dpAccessInterface)
        {
            delete m_dpAccessInterface;
            m_dpAccessInterface = NULL;
		}
		if ( NULL != m_dnBulkPolledInterface )
		{
            delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dnAccessInterface)
        {
            delete m_dnAccessInterface;
            m_dnAccessInterface = NULL;
		}

		if (NULL != m_genericAgent)
		{
			try
			{
				m_genericAgent->deactivateAndDeleteServant();
				m_genericAgent = NULL;
			}
			catch (...)
			{
				// do nothing 
			}
		}

		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
		}

		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_scadaProxyFactory = NULL;

		if (NULL != m_threadPoolSingletonManager)
		{
			TA_Base_Core::ThreadPoolSingletonManager::removeInstance (m_threadPoolSingletonManager);
			m_threadPoolSingletonManager  = NULL;
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
	}

	void SNMPStatusAgent::notifyGroupOffline(const std::string& group)
	{
		// nothing to do at this stage
	}


	void SNMPStatusAgent::notifyGroupOnline(const std::string& group)
	{
		// nothing to do at this stage
	}

	void SNMPStatusAgent::agentTerminate()
	{
		if (NULL != m_dpBulkPolledInterface)
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}

		if (NULL != m_dpAccessInterface)
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = NULL;
		}

		if ( NULL != m_dnBulkPolledInterface )
		{
			delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}

		if (NULL != m_dnAccessInterface)
		{
			delete m_dnAccessInterface;
			m_dnAccessInterface = NULL;
		}

		// unsubscribe this agent StateUpdate message
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		m_scadaProxyFactory->setProxiesToMonitorMode();

	}

	bool SNMPStatusAgent::createAllEntities(	TA_Base_Core::IEntityDataPtr agentEntityData,
										const TA_Base_Core::SharedIEntityDataList& entityDataList,
										TA_Base_Bus::IEntityList& createdEntities )
	{
		bool hasDataNode = false;

		for ( TA_Base_Core::SharedIEntityDataList::const_iterator it = entityDataList.begin(); it != entityDataList.end(); ++it )
		{
			std::string entityDataType = (*it)->getType();
			if ( entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType() )
			{
				hasDataNode = true;
				m_scadaProxyFactory->registerEntityData(*it, entityDataType);
			}
			else if( entityDataType == TA_Base_Core::DataPointEntityData::getStaticType() )
			{
				m_scadaProxyFactory->registerEntityData(*it, entityDataType);
			}
		}

		if (hasDataNode)
		{
			// create an instance of DataNodeAccessFactory if not done already 
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start of create DataNode Hierarchy Map" );

			TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);

			m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntityData->getKey()) );
		}

		for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin(); iter != entityDataList.end(); ++iter )
		{
			createdEntities.push_back( TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), createEntity(*iter)) );
		}

		// return true to indicate not to call createEntity
		return true;
	}

	void SNMPStatusAgent::entityCreationComplete()
	{
		TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();

		for (std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
        {
            if (false == itrDp->second->isVirtual())
            {
				m_snmpIOManager->addDataPoint(itrDp->second);
            }
        }
		m_snmpIOManager->finishedAddDataPoint();

		
	}

	TA_Base_Bus::IEntity* SNMPStatusAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//
		
		TA_Base_Bus::IEntity* theEntity = NULL;
		std::string entityDataType = EntityData->getType();

		if (TA_Base_Core::DataPointEntityData::getStaticType() == entityDataType)
		{
			TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
				boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData> (EntityData);

			TA_ASSERT(NULL != dataPointEntityData.get(), "dataPointEntityData is NULL");

			TA_Base_Bus::DataPoint * dp = NULL;
			
			try
			{
				dp = m_dataPointFactory->createDataPoint(dataPointEntityData);
			}
			catch(...)
			{
				if (dp != NULL)
				{
					delete dp;
					dp = NULL;
				}
			}

			if (dp != NULL)
			{
				m_datapoints[ dp->getEntityKey() ] = dp;	
				theEntity = dp;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataPoint %s:(%u)",
					EntityData->getName().c_str(), EntityData->getKey() );
			}
		}
		else if( TA_Base_Core::DataNodeEntityData::getStaticType() == entityDataType )
		{
			TA_Base_Core::DataNodeEntityDataPtr dnEntityData = 
				boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData);

			TA_ASSERT(NULL != dnEntityData.get(), "dnEntityData is NULL");

			TA_Base_Bus::DataNode* dn = NULL;
			try
			{
				dn = new TA_Base_Bus::DataNode (dnEntityData, m_persistence);
			}
			catch(...)
			{
				if (dn != NULL)
				{
					delete dn;
					dn = NULL;
				}
			}

			if (dn != NULL) 
			{
				m_dataNodes.push_back( dn );
				theEntity = dn;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s:(%u)",
					EntityData->getName().c_str(), EntityData->getKey() );
			}
		}
		else
		{
			theEntity = m_snmpIOManager->createEntity(EntityData);
		}

		return theEntity;
	}

	void SNMPStatusAgent::agentSetMonitor()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SNMPStatusAgent is being set to monitor...");

		if (m_operationMode == TA_Base_Core::Monitor)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
		}
		m_operationMode = TA_Base_Core::Monitor;

		m_snmpIOManager->setOperationMode(m_operationMode);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SNMPIOManager is set to monitor.");

		if (NULL != m_dpBulkPolledInterface)
		{
			m_dpBulkPolledInterface->setToMonitor();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataPointBulkPolledInterface is set to monitor.");

		if ( NULL != m_dnBulkPolledInterface )
		{
			m_dnBulkPolledInterface->setToMonitor();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataNodeBulkPolledInterface is set to monitor.");
		
		if (NULL != m_dpAccessInterface)
		{
			m_dpAccessInterface->setToMonitor();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataPointAccessInterface is set to monitor.");

		if (NULL != m_dnAccessInterface)
		{
			m_dnAccessInterface->setToMonitor();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataNodeAccessInterface is set to monitor.");

		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "All data nodes are set to monitor.");

		m_scadaProxyFactory->setProxiesToMonitorMode();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ScadaProxyFactory is set to monitor.");

		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			it->second->setToMonitorMode();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "All data points are set to monitor.");
	}

	void SNMPStatusAgent::agentSetControl()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SNMPStatusAgent is being set to control...");

		{
			TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

			TA_Base_Core::SNMPStatusAgentEntityDataPtr AgentEntityData;
			AgentEntityData = 
				boost::dynamic_pointer_cast<TA_Base_Core::SNMPStatusAgentEntityData>(entityData);

			int trapPort = AgentEntityData->getSNMPTrapListenPort();
			char buf[256] = {0};
			sprintf(buf, "%d", trapPort);
			std::string port(buf);
			std::map<std::string, std::string> paras;
			paras[TA_Base_Bus::SNMPProtocolManager::SNMP_TRAP_LISTEN_PORT] = port;
			m_snmpIOManager->updateMoreParas(paras);
		}

		if (m_operationMode == TA_Base_Core::Control)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
		}

		m_operationMode = TA_Base_Core::Control;

		m_scadaProxyFactory->setProxiesToControlMode();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ScadaProxyFactory is set to control.");

		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp;
		for (itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
		{
			itrDp->second->setToControlMode();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "All data points are set to control.");

		for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
		{
			(*itr)->setOperationMode( true );
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "All data nodes are set to control.");
		
		m_snmpIOManager->setOperationMode(m_operationMode);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SNMPIOManager is set to control.");

		if (NULL != m_dpBulkPolledInterface)
		{
			m_dpBulkPolledInterface->setToControl();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataPointBulkPolledInterface is set to control.");

		if (NULL != m_dpAccessInterface)
		{
			m_dpAccessInterface->setToControl();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataPointAccessInterface is set to control.");

		if ( NULL != m_dnBulkPolledInterface )
		{
			m_dnBulkPolledInterface->setToControl();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataNodeBulkPolledInterface is set to control.");

		if (NULL != m_dnAccessInterface)
		{
			m_dnAccessInterface->setToControl();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataNodeAccessInterface is set to control.");
	}

	void SNMPStatusAgent::registerForStateUpdates()
	{
	}

	void SNMPStatusAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
	}

	void SNMPStatusAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		// do nothing
	}

    
	void SNMPStatusAgent::startSNMPStatusAgent()
	{
        m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentName, this );
        m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl(m_agentName, this);
        m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_agentName, this );
        m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl(m_agentName, this);
     
		std::string operationMode = TA_Base_Core::RunParams::getInstance().get (RPARAM_OPERATIONMODE);

		if (RPARAM_CONTROL == operationMode)
		{
			agentSetControl();
		}
		else if (RPARAM_MONITOR == operationMode)
		{
			agentSetMonitor();
		}

		if (0 != m_genericAgent)
		{
			m_genericAgent->run();
		}
	}

	TA_Base_Bus::DataPoint * SNMPStatusAgent::getDataPoint (unsigned long entityKey)
	{
		TA_Base_Core::ThreadGuard guard (m_dataPointListtLock);
		TA_Base_Bus::DataPoint * dp = 0;
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itr = m_datapoints.find (entityKey);
		if (itr != m_datapoints.end())
		{
			dp = itr->second;
		}
		return dp;
	}


    void SNMPStatusAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		TA_Base_Core::ThreadGuard guard ( m_dataPointListtLock );
        dataPointList.clear();
        dataPointList = m_datapoints;
	}

    TA_Base_Bus::DataNode * SNMPStatusAgent::getDataNode( unsigned long entityKey )
	{
		TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
		TA_Base_Bus::DataNode * dn = NULL;
        
        const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_genericAgent->getEntityMap();
		std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator itr = dataNodes->find( entityKey );
		
		if ( itr != dataNodes->end() )
		{
			dn = dynamic_cast<TA_Base_Bus::DataNode *>(itr->second);
			
            if ( NULL == dn )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not convert entity %u to a DataNode", entityKey );
			}
        }
		
		return dn;
	}

	void SNMPStatusAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }

	TA_Base_Bus::DataPointFactory* SNMPStatusAgent::getDataPointFactory()
	{
		TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
	       
		return m_dataPointFactory;
	}
}
