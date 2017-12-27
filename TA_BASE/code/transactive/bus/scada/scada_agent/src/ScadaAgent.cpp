/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_agent/src/ScadaAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2014/07/17 12:56:07 $
  * Last modified by:  $Author: weikun.lin $
  *
  */

#include "ScadaAgent.h"

#include "core/threads/src/LFThreadPoolManager.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/business_library/src/BusinessManager.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"

//#include "DataUpdateSender.h"
//#include "DataPointUpdateProcessor.h"
//#include "DataUpdateServant.h"
//#include "DataNodeUpdateProcessor.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PMSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/src/DataPointStateChangeAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointAccessFactory.h"
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
#include "core/threads/src/Thread.h"
#include "core/timers/src/MonotonicTimer.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    //
    // Constructor
    //
    ScadaAgent::ScadaAgent(int argc, char* argv[], IScadaAgentUser* registeredScadaUser):	
	m_scadaAgentPrimaryKey (0),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_bulkInterfaceStarted (false),
    m_agentName (""),
    m_genericAgent (0),
	m_auditMessageSender (0),
    m_dpBulkPolledInterface (0),
    m_dpAccessInterface(0),
    m_dataPointFactory (0),
    m_scadaProxyFactory (0),
	m_dnBulkPolledInterface(NULL),
	m_dnAccessInterface(NULL),
	m_summaryProcessor(NULL),
	m_agentInitThread(NULL),
	m_regScadaUser(registeredScadaUser),
	m_hasOtherBusiness(false)
	{
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::ScadaAgent()" );
		m_datapoints.clear();
		m_dataNodes.clear();
		// TA_Base_Core::RunParams::getInstance().set(PARAM_SCADA_PROXY_CONSTRUCT_WO_START, "---");

		m_regScadaUser->setScadaAgent(this);
		// create handle to DataPointFactory
		// if (0 == m_dataPointFactory)
		// {
		// 	m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
		// }

		// get a handle to the ScadaProxyFactory
		// m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

		// TA_ASSERT ((0 != m_scadaProxyFactory), "SCADA Proxy Factory not created");
		
		//TA_Base_Bus::LFThreadPoolSingleton::getInstanceWithArgs(true);

		
		// create handle to Generic Agent
		m_genericAgent = new TA_Base_Bus::GenericAgent (argc, argv, this, NULL, AGENT_USER_TYPE_NORMAL);

		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());
		TA_Base_Bus::AlarmAckStateProcessor::getInstance().setOperationMode(TA_Base_Core::Monitor);

		initializeDPAfterProxyReady();
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init Begin AlarmAckStateProcessor process" );

		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init Begin AlarmAckStateProcessor process" );
		TA_Base_Bus::AlarmAckStateProcessor::getInstance().initializeDataPointAckStates();
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init End AlarmAckStateProcessor process" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init End AlarmAckStateProcessor process" );

		//m_summaryProcessor->setOperatorMode(TA_Base_Core::Monitor);

		m_dataPointFactory->getMmsScadaUtility().registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());
		//Start AlarmAckStateProcessor

		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init Begin SummaryProcessor SetEntityMap" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init Begin SummaryProcessor SetEntityMap" );
		
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init End SummaryProcessor SetEntityMap" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init End SummaryProcessor SetEntityMap" );
		
		TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
		std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
		workItems.reserve(m_dataNodes.size() + 1);

	    LFThreadPoolSingleton& threadPool =  LFThreadPoolSingleton::getInstance();
		DataNodeList::iterator iterDn = m_dataNodes.begin();
		int i = 0; 
		int j = 1;
		std::vector<TA_Base_Bus::DataNode * > dnLists;
		for (; iterDn != m_dataNodes.end(); ++iterDn)
		{
 			i++;
			if (i < j*350)
			{
				dnLists.push_back(*iterDn);
			}
			else
			{
				j++;
				workItems.push_back(new DataNodeActivateWorkItem(dnLists, pItemSync));
				// threadPool->queueWorkItem(new DataNodeActivateWorkItem(dnLists, pItemSync));
				
				dnLists.clear();
			}
		}

 		threadPool->queueWorkItemList(workItems);
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init Thread pool Begin activate datanode" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init Thread pool Begin activate datanode" );		
		pItemSync->wait();
		/*
		DataNodeList::iterator iterDn = m_dataNodes.begin();
		for (; iterDn != m_dataNodes.end(); ++iterDn)
		{
			//workItems.push_back(new DataNodeActivateWorkItem(*iterDn, pItemSync));
			(*iterDn)->processUpdateActivation();
			
		}
		*/
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init Thread pool End activate datanode" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init Thread pool End activate datanode" );	

		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init In ScadaAgent::startProxyFactory()" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init In ScadaAgent::startProxyFactory()" );		
		m_scadaProxyFactory->setOperationMode(TA_Base_Core::Monitor);
		//Sleep for 10 sec for all of the initialization finished.
		//TA_Base_Core::Thread::sleep(2000);

		// set the proxies to monitor mode to initialize all of the higher datapoint and datanode.
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init ScadaAgent::SummaryProcessor StartHandler" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init ScadaAgent::SummaryProcessor StartHandler" );			
		m_summaryProcessor->startHandler();
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init ScadaAgent::SummaryProcessor StartProcess" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init ScadaAgent::SummaryProcessor StartProcess" );			
		m_summaryProcessor->setOperatorMode(TA_Base_Core::Monitor);
		
		std::string val = TA_Base_Core::RunParams::getInstance().get("OtherBusiness");	
 		if (val == "YES")
 		{
			m_hasOtherBusiness = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent start business module");
		}
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init ScadaAgent::ScadaAgent Created" );
		TA_Base_Core::DebugUtil::getInstance().log ( __FILE__, __LINE__, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo,
			"ScadaAgent_Init ScadaAgent::ScadaAgent Created" );			
	}


	ScadaAgent::~ScadaAgent()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::~ScadaAgent()" );	

		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::removeInstance();
		}
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setOperationMode(TA_Base_Core::Standby);

		// clean up the list containing DataPoints instances created by
		// this Agent, without deleting the DataPoint instances themselves
		// since they are owned by the GenericAgent
		m_datapoints.clear();
        m_dataNodes.clear();

		// clean up handle to DataPointFactory
		if (0 != m_dataPointFactory)
		{
			delete m_dataPointFactory;
			m_dataPointFactory = 0;
		}

		// remove instance of audit message sender
		if (0 != m_auditMessageSender)
		{
			delete m_auditMessageSender; 
			m_auditMessageSender = 0;
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

		// clean up the GenericAgent
		if (0 != m_genericAgent)
		{
			try
			{
				m_genericAgent->deactivateAndDeleteServant();
				m_genericAgent = 0;
			}
			catch (...)
			{
				// do nothing 
			}
		}

		// clean up handle to SummaryProcessor
		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
		}

		// remove handle to ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_scadaProxyFactory = 0;

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

		if (NULL != m_agentInitThread)
		{
			delete m_agentInitThread;
			m_agentInitThread = NULL;
		}
	}

	void ScadaAgent::checkOperationMode()
	{
		bool isInControl= false;
		if ( NULL != m_genericAgent)
		{
			isInControl = m_genericAgent->getOperationMode() == TA_Base_Core::Control ? true : false;
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "m_genericAgent is NULL.");
		}
		
		if (!isInControl)
		{
			throw (TA_Base_Core::OperationModeException(RPARAM_MONITOR));
		}
	}

	TA_Base_Core::IEntityDataPtr ScadaAgent::getAgentEntityConfiguration()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::getAgentEntityConfiguration()" );
		TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
		if ( NULL == entityData )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PMSAgent::loadAgentParamValues()--No data from getAgentEntityConfiguration()");
		}
		return entityData;
	}

	void ScadaAgent::startScadaAgent()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::startScadaAgent()" );
		// create an instance of the class DataPointAgentCorbaDef_Impl
        // create an instance of the class DataPointAgentCorbaDef_Impl
        m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentName, this );
        m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl(m_agentName, this);
        m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_agentName, this );
        m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl(m_agentName, this);
        
		// else do nothing

		// check the runtime parameter OperationMode
		std::string operationMode ("");
		operationMode = TA_Base_Core::RunParams::getInstance().get (RPARAM_OPERATIONMODE);

		// if defined as "Control"
		if (RPARAM_CONTROL == operationMode)
		{
			agentSetControl();
		}
		else if (RPARAM_MONITOR == operationMode)
		{
			agentSetMonitor();
		}
		else if (RPARAM_STANDBY == operationMode)
		{
			agentSetStandby();
		}

		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::getInstance().hostStart();
		}
		
        // Becase in auto failback case, if Agent startup and quick into control status,
        // the datapoints has not been completed sync, if the datapoints not updates,
        // client will show bad quality for these datapoints 
        TA_Base_Core::Thread::sleep(30000); // sleep 30s...
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sleep time finished." );
        
		// start the Generic Agent loop
		if (0 != m_genericAgent)
		{
			m_genericAgent->run();
		}
		
		
	}

	//
    // notifyGroupOffline
    //
    void ScadaAgent::notifyGroupOffline( const std::string& group)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::notifyGroupOffline()" );
		m_regScadaUser->notifyGroupOffline( group );
    }

    //
    // notifyGroupOnline
    //
    void ScadaAgent::notifyGroupOnline( const std::string& group)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::notifyGroupOnline()" );
		m_regScadaUser->notifyGroupOnline( group );
    }

	void ScadaAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		m_regScadaUser->processOnlineUpdate(updateEvent);
	}

	TA_Base_Bus::IEntity* ScadaAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//
		unsigned long entityKey = EntityData->getKey();
		std::string entityName = EntityData->getName();
		TA_Base_Bus::IEntity* createdEntity = NULL;
		TA_Base_Core::MonotonicTimer timeStart;

		std::string entityDataType = EntityData->getType();
		
		// if the specified entity is of the type DataPoint
		if (TA_Base_Core::DataPointEntityData::getStaticType() == entityDataType)
		{
			// create and return the DataPoint entity
			//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In ScadaAgent::createDataPoint:[%lu] %s start",entityKey, entityName.c_str());
			createdEntity = createDataPointEntity(EntityData);
		}
		else if( TA_Base_Core::DataNodeEntityData::getStaticType() == entityDataType )
		{
			//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In ScadaAgent::createDataNode:[%lu] %s start",entityKey, entityName.c_str());
			TA_Base_Bus::DataNode* dn = NULL;
			try
			{
				TA_Base_Bus::IEntity * theEntity = 0;
				dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence,m_dataPointFactory->getScadaRootKey());
				{
					TA_Base_Core::ThreadGuard guard (m_dataNodeListLock);
					m_dataNodes.push_back( dn );
				}
				createdEntity = dn;
				//return theEntity;
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    EntityData->getName().c_str(), EntityData->getKey() );
				
				// To ensure the generic agent is constructed even if one DataNode creation fails
			}
			//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In ScadaAgent::createDataNode:[%lu] %s",entityKey, entityName.c_str());
		}
		else
		{
			//If the entitytype is not Data Node /Data POint,then forward the createEntity function to ScadaUser
			createdEntity = m_regScadaUser->createEntity(EntityData);
		}
		unsigned long nLapsed = timeStart.elapsed();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ScadaAgent_Init createEntity %s [%lu] %s TimeSpan: %lu ms",entityDataType.c_str(),entityKey, entityName.c_str(),nLapsed);

		return createdEntity;
	}

	bool ScadaAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
									  const TA_Base_Core::SharedIEntityDataList& entityDataList,
									  TA_Base_Bus::IEntityList& createdEntities)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::createAllEntities()" );
		// here we will block main thread until cache initialization complete
		if (!m_agentInitThread->waitForCompelete())
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Initialize ScadaAgent cache failed!" );
		}

	    LFThreadPoolSingleton& threadPool =  LFThreadPoolSingleton::getInstance();
		TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
		std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
		TA_Base_Core::SharedIEntityDataList DataNodeEntityLists;
		workItems.reserve(entityDataList.size());
		DataNodeEntityLists.reserve(entityDataList.size());
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator it = entityDataList.begin();
			  it != entityDataList.end(); ++it )
        {
            std::string entityDataType = (*it)->getType();
            if (entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType()) 
			{
				m_scadaProxyFactory->registerEntityData(*it, entityDataType);
				DataNodeEntityLists.push_back(*it);
				//createEntity(*it);
			}
			else if (entityDataType == TA_Base_Core::DataPointEntityData::getStaticType())
			{
				m_scadaProxyFactory->registerEntityData(*it, entityDataType);
				workItems.push_back(new CreateEntityWorkItem(*this, *it, pItemSync));
			}
			else
			{
				//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTU entity found in ScadaAgent::createAllEntities()!" );
				workItems.push_back(new CreateEntityWorkItem(*this, *it, pItemSync));
			}
			
        }
		//Mutltithread create datapoint and other entities except datanode.
		threadPool->queueWorkItemList(workItems);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Thread pool begin create Entities" );

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init Begin create DataNode Hierarchy Map" );
		TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init End create DataNode Hierarchy Map" );

        for ( TA_Base_Core::SharedIEntityDataList::const_iterator itDataNode = DataNodeEntityLists.begin();
			  itDataNode != DataNodeEntityLists.end(); ++itDataNode )
        {
			createEntity(*itDataNode);
		}


		DataNodeList::iterator iterDn = m_dataNodes.begin();
		for (; iterDn != m_dataNodes.end(); ++iterDn)
		{
			createdEntities.push_back(TA_Base_Bus::IEntityList::value_type((*iterDn)->getEntityKey(), *iterDn));
		}
		
		
		pItemSync->wait();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Thread pool end create Entities" );

		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator iterDp = m_datapoints.begin();
		for (; iterDp != m_datapoints.end(); ++iterDp)
		{
			createdEntities.push_back(TA_Base_Bus::IEntityList::value_type(iterDp->first, iterDp->second));
		}
		/*
		workItems.clear();
		workItems.reserve(m_dataNodes.size() + 1);

		DataNodeList::iterator iterDn = m_dataNodes.begin();
		for (; iterDn != m_dataNodes.end(); ++iterDn)
		{
			workItems.push_back(new DataNodeActivateWorkItem(*iterDn, pItemSync));
		}

	    //LFThreadPoolSingleton& threadPool =  LFThreadPoolSingleton::getInstance();
		threadPool->queueWorkItemList(workItems);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Thread pool begin activate datanode" );


        //initializeDPAfterProxyReady();

		pItemSync->wait();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Thread pool end activate datanode" );		
		*/
		m_regScadaUser->createAllEntities(agentEntityData,entityDataList,createdEntities);

		delete pItemSync;
		pItemSync = NULL;
		return true;
	}


	void ScadaAgent::entityCreationComplete()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::entityCreationComplete()" );
		// loading complete, free some memory
		TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
		m_regScadaUser->entityCreationComplete();
	}

	void ScadaAgent::agentTerminate()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In ScadaAgent::agentTerminate()" );

		// if bulk interface is created
		if (0 != m_dpBulkPolledInterface)
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = 0;
			m_bulkInterfaceStarted = false;
		}

        // if bulk interface is created
		if (0 != m_dpAccessInterface)
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = 0;
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
		if ( NULL != m_summaryProcessor )
		{
			m_summaryProcessor->stopProcessor();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish stop SummaryProcessor");
		}

		if (NULL != m_scadaProxyFactory)
		{
			m_scadaProxyFactory->stopProcessing();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish stop ScadaProxyFactory");
		}

		// unsubscribe this agent StateUpdate message
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::getInstance().hostStop();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish stop BusinessManager");
		}
		
		TA_Base_Bus::AlarmAckStateProcessor::getInstance().stopProcess();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish stop AlarmAckStateProcessor");
		
		TA_Base_Core::SingletonTimerUtil::getInstance().invalidate();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish invalidate SingletonTimerUtil" );		

		// set the proxies to monitor mode
		m_regScadaUser->agentTerminate();
    }


	void ScadaAgent::agentSetMonitor()
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaAgent is being set to monitor...");

		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::getInstance().setToMonitor();
		}
		
        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;
        
		m_summaryProcessor->setOperatorMode(m_operationMode);

        setScadaInterfaceOperationMode(m_operationMode);

        //set all datanodes to monitor mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->changeOperationMode( m_operationMode );
		}

		//changeDataNodeOperationMode(m_operationMode);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to monitor.");

		m_scadaProxyFactory->setOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to monitor.");
        
		// for each of the data point associated with this agent
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			// make the data point operated in Monitor mode
			it->second->setToMonitorMode();
		}
        //changeDataPointOperationMode(m_operationMode);
		
		TA_Base_Bus::LFThreadPoolSingleton::getInstance()->start();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to monitor.");
        
        // register this agent for Monitor messages
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		registerForMonitorMessages();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ScadaAgent %s has been started up in MONITOR mode",
            m_agentName.c_str());
		m_regScadaUser->agentSetMonitor();
	}

    void ScadaAgent::agentSetControl()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaAgent is being set to control...");

        if (m_operationMode == TA_Base_Core::Control)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
        }

		if (m_hasOtherBusiness)
		{
			TA_Base_Bus::BusinessManager::getInstance().setToControl();
		}
		
        // set the flag to indicate the agent is now in Control mode
        m_operationMode = TA_Base_Core::Control;
        
		m_summaryProcessor->setOperatorMode(m_operationMode);

        // for each of the data point associated with this agent
        std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp;
        for (itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
        {
            // make the data point operated in Control mode
            itrDp->second->setToControlMode();
        }
        //changeDataPointOperationMode(m_operationMode);
		//TA_Base_Bus::LFThreadPoolSingleton::getInstance()->start();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to control.");
        
		m_scadaProxyFactory->setOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to control.");

        
        // set all datanodes to control mode
        for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
        {
            (*itr)->changeOperationMode( m_operationMode );
        }

		//changeDataNodeOperationMode(m_operationMode);		
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to control.");  
    
        // register this agent for Control messages
        //TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        registerForControlMessages();

		// check the runtime parameter OperationMode
		
		/*int sleepSecs = 1000;
		if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "FailoverWait" ) )
		{
			int waitInMs = atoi ( TA_Base_Core::RunParams::getInstance().get( "FailoverWait" ).c_str() );
			sleepSecs = waitInMs > 0 ? waitInMs : sleepSecs;
		}
        TA_Base_Core::Thread::sleep( sleepSecs);*/

		setScadaInterfaceOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ScadaAgent %s has been started up in CONTROL mode",
            m_agentName.c_str());
		m_regScadaUser->agentSetControl();
	}
    
	void ScadaAgent::agentSetStandby()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaAgent is being set to standby...");

        if (m_operationMode == TA_Base_Core::Standby)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetStandby() called when agent is already in Standby");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Standby;

		m_summaryProcessor->setOperatorMode(m_operationMode);

		setScadaInterfaceOperationMode(m_operationMode);

        // set all datanodes to Standby mode
		//for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		//{
		//	(*itr)->changeOperationMode( m_operationMode );
		//}
		changeDataNodeOperationMode(m_operationMode);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to Standby.");

		m_scadaProxyFactory->setOperationMode(m_operationMode);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to Standby.");
        
		// for each of the data point associated with this agent
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			// make the data point operated in Standby mode
			it->second->setToStandbyMode();
		}
		TA_Base_Bus::LFThreadPoolSingleton::getInstance()->start();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to Standby.");
        
        // register this agent for Standby messages
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ScadaAgent %s has been started up in Standby mode",
            m_agentName.c_str());

		m_regScadaUser->agentSetStandby();
	}

	void ScadaAgent::setScadaInterfaceOperationMode(TA_Base_Core::EOperationMode mode)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "going to set Scada corba Interface to OperationMode[%d]", mode);
		if (0 != m_dpBulkPolledInterface)
		{
			m_dpBulkPolledInterface->setOperationMode(mode);
			m_bulkInterfaceStarted = true;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointBulkPolledInterface operation mode has been set.");
		}

        if ( NULL != m_dnBulkPolledInterface )
		{
			m_dnBulkPolledInterface->setOperationMode(mode);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeBulkPolledInterface operation mode has been set.");
		}

		if (0 != m_dpAccessInterface)
		{
			m_dpAccessInterface->setOperationMode(mode);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointAccessInterface operation mode has been set.");
		}

        if (NULL != m_dnAccessInterface)
        {
			m_dnAccessInterface->setOperationMode(mode);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeAccessInterface operation mode has been set.");
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "finished to set Scada corba Interface to OperationMode[%d]", mode);
	}


  	void ScadaAgent::submitAuditMessage (const TA_Base_Core::MessageType & messageType)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::submitAuditMessage()");
		try
		{
			if (0 == m_auditMessageSender)
			{
				m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::PMSAgentAudit::Context);
			}
        TA_Base_Core::DescriptionParameters description;
        TA_Base_Core::NameValuePair param1("AgentName", RunParams::getInstance().get(RPARAM_ENTITYNAME));
        //TA_Base_Core::NameValuePair param2("HostName", m_hostname);

        description.push_back(&param1);
        //description.push_back(&param2);
        

			m_auditMessageSender->sendAuditMessage(messageType,				// message type
												   m_scadaAgentPrimaryKey,		// entity key
												   description,					// description
												   "",							// details
												   TA_Base_Core::RunParams::getInstance().get (RPARAM_SESSIONID),	// session ID
												   "",							// alarm ID
												   "",							// incidentkey
												   "");						// parent event ID
		}
		catch (...)
		{
			// do nothing
		}
	}  
	void ScadaAgent::registerForStateUpdates()
    {
 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::registerForStateUpdates()");
   }
	
	void ScadaAgent::registerForControlMessages()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::registerForControlMessages()");
		// nothing to do at this stage
	}

	void ScadaAgent::registerForMonitorMessages()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::registerForMonitorMessages()");
	}

	TA_Base_Bus::IEntity* ScadaAgent::createDataPointEntity (TA_Base_Core::IEntityDataPtr EntityData)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::createDataPointEntity()");
		// cast the EntityData to DataPointEntityData
		TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
			boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData> (EntityData);

        TA_ASSERT(NULL != dataPointEntityData.get(), "dataPointEntityData is NULL in ScadaAgent::createDataPointEntity");

        // create a data point based on the specified entity data
		TA_Base_Bus::DataPoint * dp = m_dataPointFactory->createDataPoint(dataPointEntityData);

        // insert the newly created data point to internal list for further
        // referencing.
        if (NULL != dp)
        {
			TA_Base_Core::ThreadGuard guard (m_dataPointListtLock);
            m_datapoints[ dp->getEntityKey() ] = dp;
            //dp->setToMonitorMode(); // force datapoint to load config data
        }
    
		return dp;
	}

	TA_Base_Bus::DataPoint * ScadaAgent::getDataPoint (unsigned long entityKey)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::getDataPoint()");
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard (m_dataPointListtLock);

		TA_Base_Bus::DataPoint * theDataPoint = 0;

		// find if there is a data point whose pkey matches with the specified entity key
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itr = m_datapoints.find (entityKey);

		// if found one
		if (itr != m_datapoints.end())
		{
			// get the reference to the data point
			theDataPoint = itr->second;
		}
		// else do nothing

		return theDataPoint;
	}

    void ScadaAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::getAllDataPoints()");
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListtLock );
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ScadaAgent::getAllDataPoints()= %d", m_datapoints.size());
        dataPointList.clear();

        dataPointList = m_datapoints;
	}

    TA_Base_Bus::DataNode * ScadaAgent::getDataNode( unsigned long entityKey )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::getDataNode()");
		// stop multi threads accessing the list of DataNode objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
		TA_Base_Bus::DataNode * dn = NULL;
        
        const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_genericAgent->getEntityMap();
		std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator itr = dataNodes->find( entityKey );
		
		if ( itr != dataNodes->end() )
		{
			dn = dynamic_cast<TA_Base_Bus::DataNode *>(itr->second);
			
            if ( NULL == dn )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not convert entity %ul to a DataNode", entityKey );
			}
        }
		
		return dn;
	}

	void ScadaAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::getAllDataNodes()");
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }


	TA_Base_Bus::DataPointFactory* ScadaAgent::getDataPointFactory()
	{
		TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::getDataPointFactory()");
	       
		return m_dataPointFactory;
	}
		
	void ScadaAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::receiveSpecialisedMessage()");
    }

	void ScadaAgent::beforeGetDescendantsOfAgent(TA_Base_Core::IEntityDataPtr agentEntity)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::beforeGetDescendantsOfAgent()");

		TA_Base_Core::RunParams::getInstance().set(PARAM_SCADA_PROXY_CONSTRUCT_WO_START, "---");
		
		// create handle to DataPointFactory
		if (0 == m_dataPointFactory)
		{
			m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
		}

		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

		TA_ASSERT ((0 != m_scadaProxyFactory), "SCADA Proxy Factory not created");

		if (m_agentInitThread == NULL)
		{
			m_agentInitThread = new AgentInitThread(*this, &ScadaAgent::initilizeCache, agentEntity);
		}

		m_agentInitThread->start();
	}

	void ScadaAgent::initilizeCache(TA_Base_Core::IEntityDataPtr agentEntity)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "In ScadaAgent::initilizeCache()");
		m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntity->getKey()) );
		if (NULL != m_dataPointFactory)
		{
			unsigned long agentKey = agentEntity->getAgent();
			std::string agentName = agentEntity->getAgentName();
			m_dataPointFactory->initialize(agentKey, agentName);
		}
		// intialize DataPointStateChangeAccessFactory first
		TA_Base_Core::DataPointStateChangeAccessFactory::getInstance();
		TA_Base_Core::DerivedDataPointAccessFactory::getInstance().bulkLoadByAgentKey(agentEntity->getAgent());
	}

	void ScadaAgent::initializeDPAfterProxyReady()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init begin init data point after proxy factory ready");

		TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
		std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
		workItems.reserve(m_datapoints.size() + 1);

		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			workItems.push_back(new InitDataPointWorkItem(it->second, pItemSync));
		}
	    LFThreadPoolSingleton& threadPool =  LFThreadPoolSingleton::getInstance();		
		threadPool->queueWorkItemList(workItems);
		pItemSync->wait();
		/*
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			(it->second)->initAfterProxyFactoryReady();
		}
		*/

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init end init data point after proxy factory ready");
		delete pItemSync;
	}

	void ScadaAgent::changeDataNodeOperationMode(TA_Base_Core::EOperationMode operationMode)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init begin change datanodes operation mode");

		TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
		std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
		workItems.reserve(m_dataNodes.size() + 1);

        // set all datanodes to control mode
        for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
        {
			workItems.push_back(new ChangeDataNodeOpertionModeWorkItem(*itr,operationMode, pItemSync));
        }

	    LFThreadPoolSingleton& threadPool =  LFThreadPoolSingleton::getInstance();		
		threadPool->queueWorkItemList(workItems);
		pItemSync->wait();
		delete pItemSync;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init end change datanodes operation mode");
	}


	void ScadaAgent::changeDataPointOperationMode(TA_Base_Core::EOperationMode operationMode)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init begin change datapoints operation mode");

		TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
		std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
		workItems.reserve(m_datapoints.size() + 1);

        // for each of the data point associated with this agent
        std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp;
        for (itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
        {
            // make the data point operated in Control mode
            //itrDp->second->setToControlMode();
			workItems.push_back(new ChangeDataPointOpertionModeWorkItem(itrDp->second,operationMode, pItemSync));
        }

	    LFThreadPoolSingleton& threadPool =  LFThreadPoolSingleton::getInstance();		
		threadPool->queueWorkItemList(workItems);
		pItemSync->wait();
		delete pItemSync;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init end change datapoints operation mode");
	}
	
	AgentInitThread::AgentInitThread(ScadaAgent& agent, FuncInit func, TA_Base_Core::IEntityDataPtr agentEntityData)
		: m_semphore(0),
		  m_func(func),
		  m_agentEntityData(agentEntityData),
		  m_agent(agent),
		  m_executeSuccess(true)
	{
		
	}

	AgentInitThread::~AgentInitThread()
	{
		terminateAndWait();
	}

	bool AgentInitThread::waitForCompelete()
	{
		m_semphore.wait();
		return m_executeSuccess;
	}

	void AgentInitThread::run()
	{
		if (NULL != m_func)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init begin AgentInitThread");
			
			try
			{
				(m_agent.*m_func)(m_agentEntityData);
			}
			catch(...)
			{
				m_executeSuccess = false;
			}
			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ScadaAgent_Init end AgentInitThread" );
		}

		m_semphore.post();
	}

	void AgentInitThread::terminate()
	{
		m_semphore.post();
	}

	CreateEntityWorkItem::CreateEntityWorkItem(ScadaAgent& agent, 
		TA_Base_Core::IEntityDataPtr entityData,
		TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_parent(agent),
	m_entityData(entityData),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void CreateEntityWorkItem::executeWorkItem()
	{
	
		TA_Base_Core::MonotonicTimer timeStart1,timeStart2;
		TA_Base_Core::MonotonicTimer timeStart;
		m_parent.createEntity(m_entityData);
		unsigned long nLapsed1 = timeStart1.elapsed();
		m_sync->itemEnd();
		unsigned long nLapsed2 = timeStart2.elapsed();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CreateEntityWorkItem TimeSpan: %lu, WithSyncSpan: %lu",nLapsed1,nLapsed2-nLapsed1);
	}

	InitDataPointWorkItem::InitDataPointWorkItem(TA_Base_Bus::DataPoint* dp, TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_dp(dp),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void InitDataPointWorkItem::executeWorkItem()
	{
		TA_Base_Core::MonotonicTimer timeStart1,timeStart2;
		m_dp->initAfterProxyFactoryReady();
		unsigned long nLapsed1 = timeStart1.elapsed();
		m_sync->itemEnd();
		unsigned long nLapsed2 = timeStart2.elapsed();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "InitDataPointWorkItem TimeSpan: %lu, WithSyncSpan: %lu",nLapsed1,nLapsed2-nLapsed1);
	}

	ChangeDataNodeOpertionModeWorkItem::ChangeDataNodeOpertionModeWorkItem(TA_Base_Bus::DataNode* dn, 
		TA_Base_Core::EOperationMode operationMode,
		TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_dn(dn),
	m_operationMode(operationMode),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void ChangeDataNodeOpertionModeWorkItem::executeWorkItem()
	{
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "begin set datanode mode");
		TA_Base_Core::MonotonicTimer timeStart1,timeStart2;
		m_dn->changeOperationMode(m_operationMode);
		unsigned long nLapsed1 = timeStart1.elapsed();
		m_sync->itemEnd();
		unsigned long nLapsed2 = timeStart2.elapsed();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChangeDataNodeOpertionModeWorkItem TimeSpan: %lu, WithSyncSpan: %lu",nLapsed1,nLapsed2-nLapsed1);
	}	

	ChangeDataPointOpertionModeWorkItem::ChangeDataPointOpertionModeWorkItem(TA_Base_Bus::DataPoint* dp, 
		TA_Base_Core::EOperationMode operationMode,
		TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_dp(dp),
	m_operationMode(operationMode),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void ChangeDataPointOpertionModeWorkItem::executeWorkItem()
	{
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "begin set datapoint mode");
		TA_Base_Core::MonotonicTimer timeStart1,timeStart2;

		if (TA_Base_Core::Control == m_operationMode )
		{
			m_dp->setToControlMode();
		}
		else if (TA_Base_Core::Monitor == m_operationMode)
		{
			m_dp->setToMonitorMode();
		}
		unsigned long nLapsed1 = timeStart1.elapsed();
		m_sync->itemEnd();
		unsigned long nLapsed2 = timeStart2.elapsed();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChangeDataPointOpertionModeWorkItem TimeSpan: %lu, WithSyncSpan: %lu",nLapsed1,nLapsed2-nLapsed1);
	}	

	/*
	DataNodeActivateWorkItem::DataNodeActivateWorkItem(TA_Base_Bus::DataNode* dn, TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_dn(dn),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void DataNodeActivateWorkItem::executeWorkItem()
	{
		TA_Base_Core::MonotonicTimer timeStart2;
		TA_Base_Core::MonotonicTimer timeStart1;
		
		m_dn->processUpdateActivation();
		unsigned long nLapsed1 = timeStart1.elapsed();
		m_sync->itemEnd();
		unsigned long nLapsed2 = timeStart2.elapsed();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataNodeActivateWorkItem TimeSpan: %lu, WithSyncSpan: %lu",nLapsed1,nLapsed2-nLapsed1);
	}
	*/
	DataNodeActivateWorkItem::DataNodeActivateWorkItem(std::vector<TA_Base_Bus::DataNode * > datanodes, TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_datanodes(datanodes),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}
	
	void DataNodeActivateWorkItem::executeWorkItem()
	{
		TA_Base_Core::MonotonicTimer timeStart2;
		TA_Base_Core::MonotonicTimer timeStart1;

		std::vector<TA_Base_Bus::DataNode * >::iterator it;
		for (it = m_datanodes.begin(); it != m_datanodes.end(); ++it)
		{
			(*it)->processUpdateActivation();
		}		
		unsigned long nLapsed1 = timeStart1.elapsed();
		m_sync->itemEnd();
		unsigned long nLapsed2 = timeStart2.elapsed();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataNodeActivateWorkItem TimeSpan: %lu, WithSyncSpan: %lu",nLapsed1,nLapsed2-nLapsed1);
	}

	
};

