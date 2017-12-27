/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PScadaAgent/src/PScadaAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	PScadaAgent implementes the GenericAgent interface.  PScadaAgent monitors and controls the
  * the entities associated with its process, such as PSCADA Controller and the DataPoint entities.
  *
  */

#include "app/scada/PScadaAgent/src/PScadaAgent.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/PMSAgentEntityData.h"
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
	const unsigned long PROXY_DP_MONITORING_PERIOD_SECS = 20;

	PScadaAgent::PScadaAgent (int argc, char* argv[])
	:
	m_pscadaAgentPrimaryKey (0),
    m_proxyDpMonitoringPeriodSecs (PROXY_DP_MONITORING_PERIOD_SECS),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_bulkInterfaceStarted (false),
    m_isRTUSwitchingDisabled (false),
    m_analogueDpUpdateInterval (0),
    m_stationName (""),
    m_agentName (""),
	m_agentAssetName (""),
    m_genericAgent (0),
	m_auditMessageSender (0),
    m_dpBulkPolledInterface (0),
    m_dpAccessInterface(0),
    m_dataPointFactory (0),
    m_scadaProxyFactory (0),
    m_threadPoolSingletonManager (0),
	m_dnBulkPolledInterface(NULL),
	m_dnAccessInterface(NULL),
	m_summaryProcessor(NULL)
	{
		FUNCTION_ENTRY("PScadaAgent::PScadaAgent");
		m_datapoints.clear();
		m_dataNodes.clear();

		// create handle to DataPointFactory
		if (0 == m_dataPointFactory)
		{
			m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
		}

		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

		TA_ASSERT ((0 != m_scadaProxyFactory), "SCADA Proxy Factory not created");

		// create handle to Generic Agent
		m_genericAgent = new TA_Base_Bus::GenericAgent (argc, argv, this);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Created genericAgent");

		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get AgentName = %s",m_agentName.c_str());

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentName, m_genericAgent->getEntityMap());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "m_scadaProxyFactory->registerLocalEntities");

		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "m_scadaProxyFactory->setProxiesToMonitorMode");

        // force datapoints to load config data which will be used used by createPScadaWorkers
        std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
        for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
        {
            // make the data point operated in Monitor mode
            it->second->setToMonitorMode();
        }
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Data Point set to Monitor Mode");
       
        // get handle to ThreadPoolSingletonManager
		m_threadPoolSingletonManager = TA_Base_Core::ThreadPoolSingletonManager::getInstance();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "get ThreadPoolSingletonManager:%p",m_threadPoolSingletonManager);

		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "get m_summaryProcessor:%p",m_summaryProcessor);

		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "m_summaryProcessor->setEntityMap");

        createPScadaWorkers();
		FUNCTION_EXIT("PScadaAgent::PScadaAgent");
	}


	PScadaAgent::~PScadaAgent()
	{
		FUNCTION_ENTRY("PScadaAgent::~PScadaAgent");

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

		// clean up handle to SummaryProcessor
		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop SummaryProcessor");
		}

		// remove handle to ScadaProxyFactory
		if ( NULL != m_scadaProxyFactory )
		{
			TA_Base_Bus::ScadaProxyFactory::removeInstance();
			m_scadaProxyFactory = 0;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to call ScadaProxyFactory::removeInstance");
		}

		// clean up handle to ThreadPoolSingletonManager
		if (0 != m_threadPoolSingletonManager)
		{
			TA_Base_Core::ThreadPoolSingletonManager::removeInstance (m_threadPoolSingletonManager);
			m_threadPoolSingletonManager  = 0;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to call ThreadPoolSingletonManager::removeInstance");
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to clean AlarmHelperManager");

		TA_Base_Core::SingletonTimerUtil::removeInstance();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish remove SingletonTimerUtil" );
		
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
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to call GenericAgent::deactivateAndDeleteServant");
		
		FUNCTION_EXIT("PScadaAgent::~PScadaAgent");
	}


	void PScadaAgent::loadAgentParamValues()
	{
		FUNCTION_ENTRY("loadAgentParamValues");
		// get configuration data for this PScadaAgent
		TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

		// dynamic cast the entity data to PMSAgentEntityData
		TA_Base_Core::PMSAgentEntityDataPtr pscadaAgentEntityData;
		pscadaAgentEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

		// get the primary key of this PScadaAgent
		m_pscadaAgentPrimaryKey = pscadaAgentEntityData->getKey();

		// get the agent asset name
		m_agentAssetName = pscadaAgentEntityData->getAssetName();

		// get indication if the PScadaUnit switching functionality is disabled
        //m_isRTUSwitchingDisabled = pscadaAgentEntityData->getIsRTUSwitchingDisabled();
        //m_analogueDpUpdateInterval = pscadaAgentEntityData->getAnalogueDataPointUpdateInterval();

		// get the configured station name
		m_stationName = pscadaAgentEntityData->getStationName();

		// get the configured health monitoring period for the proxy data points
		// use the defaulted value if not configured in database
		m_proxyDpMonitoringPeriodSecs = pscadaAgentEntityData->getProxyDpHealthMonitoringPeriodSecs();
		if (0 == m_proxyDpMonitoringPeriodSecs)
		{
			m_proxyDpMonitoringPeriodSecs = PROXY_DP_MONITORING_PERIOD_SECS;
		}
		FUNCTION_EXIT("loadAgentParamValues");
	}


	void PScadaAgent::startProcessors()
	{
		FUNCTION_ENTRY("startProcessors");
		FUNCTION_EXIT("startProcessors");
	}


	void PScadaAgent::stopProcessors()
	{
		FUNCTION_ENTRY("stopProcessors");

		m_pscadaManager.stopProcess();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop PScadaManager");

		if ( NULL != m_summaryProcessor )
		{
			m_summaryProcessor->stopProcessor();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop SummaryProcessor");
		}

		if ( NULL != m_scadaProxyFactory )
		{
			m_scadaProxyFactory->stopProcessing();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop ScadaProxyFactory");
		}

		TA_Base_Bus::AlarmAckStateProcessor::getInstance().stopProcess();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish stop AlarmAckStateProcessor");

		TA_Base_Core::SingletonTimerUtil::getInstance().invalidate();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish invalidate SingletonTimerUtil" );
		
		FUNCTION_EXIT("stopProcessors");
	}


	void PScadaAgent::createPScadaWorkers()
	{
		FUNCTION_ENTRY("createPScadaWorkers");
		// if does not have the agent's pkey
		if (0 == m_pscadaAgentPrimaryKey)
		{
			loadAgentParamValues();
            
            m_pscadaManager.setAgentAssetName(m_agentAssetName);
            //m_pscadaManager.setIsRTUSwitchingDisabled(m_isRTUSwitchingDisabled);
            m_pscadaManager.setStationName(m_stationName);
            //m_pscadaManager.setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
        }

        // create PScadaUnit workers
        //m_pscadaManager.createPScadaWorkers();

        // add datapoint to pscada manager except virtual datapoint
        for (std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
        {
            if (false == itrDp->second->isVirtual())
            {
                m_pscadaManager.addDataPoint(itrDp->second);
            }
        }
		FUNCTION_EXIT("createPScadaWorkers");
    }

	TA_Base_Bus::IEntity* PScadaAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//
		FUNCTION_ENTRY("createEntity");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start to create Entity %s (%ul)",
			EntityData->getName().c_str(), EntityData->getKey()  );

		std::string entityDataType = EntityData->getType();

		// if the specified entity is of the RTU type
		if (TA_Base_Core::RTUEntityData::getStaticType() == entityDataType)
		{
			// create and return the PScadaUnit entity
			return m_pscadaManager.createPScadaUnitEntity(EntityData);
		}
		// if the specified entity is of the type DataPoint
		else if (TA_Base_Core::DataPointEntityData::getStaticType() == entityDataType)
		{
			// create and return the DataPoint entity
			return createDataPointEntity(EntityData);
		}
		else if( TA_Base_Core::DataNodeEntityData::getStaticType() == entityDataType )
		{
			TA_Base_Bus::DataNode* dn = NULL;
			try
			{
				TA_Base_Bus::IEntity * theEntity = 0;
				dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence);
                
                m_dataNodes.push_back( dn );
				theEntity = dn;
				return theEntity;
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    EntityData->getName().c_str(), EntityData->getKey() );
				
				// To ensure the generic agent is constructed even if one DataNode creation fails
			}
		}
		FUNCTION_EXIT("createEntity");

		return NULL;
	}

	bool PScadaAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
											  const TA_Base_Core::SharedIEntityDataList& entityDataList,
											  TA_Base_Bus::IEntityList& createdEntities)
	{
		FUNCTION_ENTRY("createAllEntities");
		bool createdDnSpecificUtils = false;

        for ( TA_Base_Core::SharedIEntityDataList::const_iterator it = entityDataList.begin();
        it != entityDataList.end(); ++it )
        {
            std::string entityDataType = (*it)->getType();
            if ((entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType()) ||
                (entityDataType == TA_Base_Core::DataPointEntityData::getStaticType()) )
            {
                m_scadaProxyFactory->registerEntityData(*it, entityDataType);
            }
        }
		for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
		iter != entityDataList.end(); ++iter )
		{
			// only datanodes need these things, so if there are no datanodes, why create them?
			if ( (false == createdDnSpecificUtils) &&
				((*iter)->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) )
			{
				// create an instance of DataNodeAccessFactory if not done already
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Start of create DataNode Hierarchy Map" );
            
				TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);
            
				// initialise ScadaPersistence
				m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntityData->getKey()) );
            
				createdDnSpecificUtils = true;
			}
        
			//Get the IEntity point
			TA_Base_Bus::IEntity * pEntity = createEntity(*iter);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Create IEntity %d -%s(%p) ",(*iter)->getKey(),(*iter)->getName().c_str(),pEntity );

			if (pEntity != NULL)
			{
				pEntity->start();
			}

			createdEntities.push_back( 
				TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), pEntity) );

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Finished Create entity %s ",(*iter)->getName().c_str() );
		}
    
		FUNCTION_EXIT("createAllEntities");
		// return true to indicate not to call createEntity
		return true;
	}


	void PScadaAgent::entityCreationComplete()
	{
		// loading complete, free some memory
		FUNCTION_ENTRY("entityCreationComplete");

		TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();

		FUNCTION_EXIT("entityCreationComplete");
	}

	void PScadaAgent::agentTerminate()
	{
		FUNCTION_ENTRY("agentTerminate");

        // make sure the processors are all terminated
		stopProcessors();

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

		// unsubscribe this agent StateUpdate message
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		FUNCTION_EXIT("agentTerminate");

    }


	void PScadaAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PScadaAgent is being set to monitor...");

        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;

		m_summaryProcessor->setOperatorMode(m_operationMode);
		        
        // set PScada manager to monitor mode
        m_pscadaManager.setToMonitor();
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PScadaManager is set to monitor.");

        // if bulk interface is set up
		if (0 != m_dpBulkPolledInterface)
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
			m_bulkInterfaceStarted = false;
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointBulkPolledInterface is set to monitor.");

        if ( NULL != m_dnBulkPolledInterface )
		{
			m_dnBulkPolledInterface->setToMonitor();
		}
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeBulkPolledInterface is set to monitor.");
        // if bulk interface is set up
		if (0 != m_dpAccessInterface)
		{
			// stop receiving remote requests
			m_dpAccessInterface->setToMonitor();
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointAccessInterface is set to monitor.");

        if (NULL != m_dnAccessInterface)
        {
			m_dnAccessInterface->setToMonitor();
		}
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeAccessInterface is set to monitor.");
        // set all datanodes to monitor mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to monitor.");



        // set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to monitor.");
        
		// for each of the data point associated with this agent
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator it;
		for (it = m_datapoints.begin(); it != m_datapoints.end(); ++it)
		{
			// make the data point operated in Monitor mode
			it->second->setToMonitorMode();
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to monitor.");
        
        // register this agent for Monitor messages
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		registerForMonitorMessages();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PScadaAgent for Station %s has been started up in MONITOR mode",
            m_stationName.c_str());
		FUNCTION_EXIT("agentSetMonitor");
	}

    void PScadaAgent::agentSetControl()
    {
		FUNCTION_ENTRY("agentSetControl");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PScadaAgent is being set to control...");

        if (m_operationMode == TA_Base_Core::Control)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
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
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datapoints are set to control.");
        
        // set the proxies to control mode
        m_scadaProxyFactory->setProxiesToControlMode();
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ScadaProxyFactory is set to control.");
        
        // if bulk interface is set up
        if (0 != m_dpBulkPolledInterface)
        {
            // start receiving remote requests
            m_dpBulkPolledInterface->setToControl();
            m_bulkInterfaceStarted = true;
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointBulkPolledInterface is set to control.");
        
        // if bulk interface is set up
        if (0 != m_dpAccessInterface)
        {
            // start receiving remote requests
            m_dpAccessInterface->setToControl();
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataPointAccessInterface is set to control.");
        
        // set all datanodes to control mode
        for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
        {
            (*itr)->setOperationMode( true );
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "All datanode are set to control.");
        
        if ( NULL != m_dnBulkPolledInterface )
        {
            // start receiving remote requests
            m_dnBulkPolledInterface->setToControl();
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeBulkPolledInterface is set to control.");
        
        if (NULL != m_dnAccessInterface)
        {
            m_dnAccessInterface->setToControl();
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DataNodeAccessInterface is set to control.");
        
        // register this agent for Control messages
        //TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        registerForControlMessages();


        m_pscadaManager.setToControl();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PScadaManager is set to control.");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PScadaAgent for Station %s has been started up in CONTROL mode",
            m_stationName.c_str());
		FUNCTION_EXIT("agentSetControl");
    }
    

    void PScadaAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("processOnlineUpdate");
		unsigned long updateEventPKey = updateEvent.getKey();

		// if the update event is for PScadaAgent
		if (m_pscadaAgentPrimaryKey == updateEventPKey)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for PScadaAgent...");

			// update PScadaAgent configuration
			updatePScadaAgentConfiguration(updateEvent);
			return;
		}

		// if the update event is for PScadaUnit
		if (m_pscadaManager.updatePScadaUnitConfiguration(updateEvent))
		{
            // already processed, no need go further
			return;
		}

		// else update data point configuration
		updateDataPointConfiguration (updateEvent);
		FUNCTION_EXIT("processOnlineUpdate");
	}


	void PScadaAgent::startPScadaAgent()
	{
		FUNCTION_ENTRY("startPScadaAgent");
		//
		// solution for CASE 1193
		//
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

		// start the processors to process data point events
		startProcessors();


		// start the Generic Agent loop
		if (0 != m_genericAgent)
		{
			m_genericAgent->run();
		}
		FUNCTION_EXIT("startPScadaAgent");
	}


	void PScadaAgent::submitAuditMessage (const TA_Base_Core::DescriptionParameters & description,
										const TA_Base_Core::MessageType & messageType)
	{
		FUNCTION_ENTRY("submitAuditMessage");
		try
		{
			if (0 == m_auditMessageSender)
			{
				m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::PMSAgentAudit::Context);
			}

			m_auditMessageSender->sendAuditMessage(messageType,				// message type
												   m_pscadaAgentPrimaryKey,		// entity key
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
		FUNCTION_EXIT("submitAuditMessage");
	}


	void PScadaAgent::registerForControlMessages()
	{
		FUNCTION_ENTRY("registerForControlMessages");
		// nothing to do at this stage
		FUNCTION_EXIT("registerForControlMessages");
	}


	void PScadaAgent::registerForMonitorMessages()
	{
		FUNCTION_ENTRY("registerForMonitorMessages");
		FUNCTION_EXIT("registerForMonitorMessages");
	}



	void PScadaAgent::updatePScadaAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		FUNCTION_ENTRY("updatePScadaAgentConfiguration");
		std::string msg ("");

		switch (updateEvent.getModifications())
		{
			// only if the PScadaAgent has been updated in the database
			case TA_Base_Core::Update:
			{
				// get the list of changed parameter from the config update
				const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

				if ((0 != changes) && (false == changes->empty()))
				{
					// get configuration data for this PScada Agent
					TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();

					// dynamic cast the entity data to PMSAgentEntityData
					TA_Base_Core::PMSAgentEntityDataPtr PScadaAgentEntityData;
					PScadaAgentEntityData = 
						boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

					// invalidate existing data so that new data is loaded from database
					PScadaAgentEntityData->invalidate();

					for (unsigned int i = 0; i < changes->size(); i++)
					{

						// Proxy DataPoint Health Monitoring Period
						if (TA_Base_Core::PMSAgentEntityData::PROXY_DP_HEALTH_MONITORING_PERIOD_SECS == (*changes)[i])
						{
							// proxy health monitoring is no longer available in 
							// the latest ScadaProxyFactory --> therefore this will do nothing
						}

                        // Analogue DataPoint Update Interval // TD16169
                        else if ( TA_Base_Core::PMSAgentEntityData::ANALOGUE_DP_UPDATE_INTERVAL == (*changes)[i] )
                        {
                            m_analogueDpUpdateInterval = PScadaAgentEntityData->getAnalogueDataPointUpdateInterval();
                            //m_pscadaManager.setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
                        }
                        else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration update changes is not for PScadaAgentEntityData");
						}
					}
				}

				break;
			}

			// don't care in other case
			default:
			{
				// log audit message?
				break;
			}
		}
		FUNCTION_EXIT("updatePScadaAgentConfiguration");
	}


    void PScadaAgent::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		FUNCTION_ENTRY("updateDataPointConfiguration");
		unsigned long updateEventPKey = updateEvent.getKey();

		// find the DataPoint object whose entity key match with the
		// key associated with the received message
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator dpIter = m_datapoints.find (updateEventPKey);

		// if found one
		if (dpIter != m_datapoints.end())
		{
			TA_Base_Bus::DataPoint * dp = dpIter->second;

			// get the type of updating event
			switch (updateEvent.getModifications())
			{
            case TA_Base_Core::Update:
                // pass the config update to the data point for updating
                dp->update(updateEvent);
                break;
            
            case TA_Base_Core::Delete:
                m_pscadaManager.removeDataPoint(dp);
                break;
                
            case TA_Base_Core::Create:
                m_pscadaManager.addDataPoint(dp);
                break;
            
            default:
                // do nothing, exit switch statement
                break;
			}


			//
			// check if the data point's update priority has been changed online
			//

			// get the list of changed parameter from the config update
			const std::vector< std::string > * configChanges = 0;
			configChanges = &updateEvent.getChangedParams();

			// for each of the changed parameters
			for (unsigned int i = 0; i < configChanges->size(); i++)
			{
				std::string paramName = (*configChanges)[i];

				// only interested in this parameter
				if (paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_PRIORITY)
				{
					//
					// update the PScadaUnit's with the data point whose update priority
					// has been changed
					//
					m_pscadaManager.removeDataPoint (dp);
					m_pscadaManager.addDataPoint (dp);
				}
			}

			dp = 0;
		}
		FUNCTION_EXIT("updateDataPointConfiguration");
	}

	void PScadaAgent::notifyGroupOffline(const std::string& group)
	{
		// nothing to do at this stage
		FUNCTION_ENTRY("notifyGroupOffline");
		FUNCTION_EXIT("notifyGroupOffline");
	}


	void PScadaAgent::notifyGroupOnline(const std::string& group)
	{
		// nothing to do at this stage
		FUNCTION_ENTRY("notifyGroupOnline");
		FUNCTION_EXIT("notifyGroupOnline");
	}


	TA_Base_Bus::IEntity* PScadaAgent::createDataPointEntity (TA_Base_Core::IEntityDataPtr EntityData)
	{
		FUNCTION_ENTRY("createDataPointEntity");
		// cast the EntityData to DataPointEntityData
		TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
			boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData> (EntityData);

        TA_ASSERT(NULL != dataPointEntityData.get(), "dataPointEntityData is NULL in PScadaAgent::createDataPointEntity");

        // create a data point based on the specified entity data
		TA_Base_Bus::DataPoint * dp = m_dataPointFactory->createDataPoint(dataPointEntityData);

        // insert the newly created data point to internal list for further
        // referencing.
        if (NULL != dp)
        {
            m_datapoints[ dp->getEntityKey() ] = dp;
            //dp->setToMonitorMode(); // force datapoint to load config data
        }
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create entity %s(%u) failed", EntityData->getName().c_str(),EntityData->getKey() );

		}
		FUNCTION_EXIT("createDataPointEntity");

		return dp;
	}


	TA_Base_Bus::DataPoint * PScadaAgent::getDataPoint (unsigned long entityKey)
	{
		FUNCTION_ENTRY("getDataPoint");
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
		FUNCTION_EXIT("getDataPoint");

		return theDataPoint;
	}


    void PScadaAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		FUNCTION_ENTRY("getAllDataPoints");
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListtLock );

        dataPointList.clear();

        dataPointList = m_datapoints;
		FUNCTION_EXIT("getAllDataPoints");
	}

    TA_Base_Bus::DataNode * PScadaAgent::getDataNode( unsigned long entityKey )
	{
		FUNCTION_ENTRY("getDataNode");
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
		FUNCTION_EXIT("getDataNode");
		
		return dn;
	}

	void PScadaAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
		FUNCTION_ENTRY("getAllDataNodes");
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
		FUNCTION_EXIT("getAllDataNodes");
    }


	TA_Base_Bus::DataPointFactory* PScadaAgent::getDataPointFactory()
	{
		FUNCTION_ENTRY("getDataPointFactory");
		TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
	       
		return m_dataPointFactory;
		FUNCTION_EXIT("getDataPointFactory");
	}
		
    void PScadaAgent::registerForStateUpdates()
    {
		FUNCTION_ENTRY("registerForStateUpdates");
		FUNCTION_EXIT("registerForStateUpdates");
   }

	
    void PScadaAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		FUNCTION_EXIT("receiveSpecialisedMessage");
    }

}
