/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/SystemStatusAgent.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * SystemStatusAgent class implimentation. See header file for class descrption
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4503)
#endif

#include <sstream>

#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/types/SystemStatusAgentAlarms_MessageTypes.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"

#include "bus/generic_agent/src/IEntity.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"

#include "app/system_control/system_status_agent/src/SystemStatusAgent.h"
#include "app/system_control/system_status_agent/src/DataPointDriverHelper.h"
#include "app/system_control/system_status_agent/src/SystemStatusAgentConfigException.h"


using TA_Base_Core::UtilityInitialiser;

namespace TA_Base_App
{
	SystemStatusAgent::SystemStatusAgent(int argc, char* argv[])
    :
    m_operationMode(TA_Base_Core::NotApplicable),
    m_genericAgent(NULL),
    m_agentKey(0),
    //m_agentEntityData(NULL),
    m_driverHelper(NULL),
    m_dpBulkPolledInterface(NULL),
    m_dpAccessInterface(NULL),
	m_dnBulkPolledInterface(NULL),
	m_dnAccessInterface(NULL),
	m_scadaProxyFactory(NULL),
    m_threadPoolSingletonManager(NULL),
	m_summaryProcessor(NULL),
    m_dataPointFactory(NULL),
	isFirstLaunch(true)
	//TD18093,jianghp
	//m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper())
	{
		FUNCTION_ENTRY("SystemStatusAgent");
        m_datapoints.clear();
        m_dataNodes.clear();

		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
        m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);

		//This object must be instantiated before the GenericAgent object
		//because we use it to determine a given datapoint (config) can be supprted
		//by one of the avaiable drivers
		m_driverHelper = new DataPointDriverHelper(*this);

		// The GenericAgent CTOR calls createEntity for each of your entities
		// createEntity call will use DataPoint Factory to register a datapoint
		//object. If 
		m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);


		// get configuration data for this Agent
		TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
		TA_ASSERT(0 != entityData.get(), "entityData pointer is null");
		m_agentKey = entityData->getKey();

		// dynamic cast the entity data to SystemStatusAgentEntityData
		//m_agentEntityData = 0;
		m_agentEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::SystemStatusAgentEntityData>(entityData);

		TA_ASSERT(0 != m_agentEntityData.get(), 
			"dynamic_cast < TA_Base_Core::SystemStatusAgentEntityData* > ( entityData ) failed");

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentEntityData->getAgentName() , m_genericAgent->getEntityMap());
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

        // get handle to ThreadPoolSingletonManager
		m_threadPoolSingletonManager = TA_Base_Core::ThreadPoolSingletonManager::getInstance();
		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());

		//Do not set the m_operationMode member here! It will be set in run().
		FUNCTION_EXIT;
	}


	TA_Base_Core::SystemStatusAgentEntityDataPtr SystemStatusAgent::getAgentData()
	{
		FUNCTION_ENTRY("getAgentData");

		TA_ASSERT(0 != m_agentEntityData.get(), "m_agentEntityData is null");

		FUNCTION_EXIT;
		return m_agentEntityData;
	}


	void SystemStatusAgent::cleanUpDataPointLib()
	{
		FUNCTION_ENTRY("cleanUpDataPointLib");

		// if bulk interface is created
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

        FUNCTION_EXIT;
	}


	SystemStatusAgent::~SystemStatusAgent()
	{
		FUNCTION_ENTRY("~SystemStatusAgent");

		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

		// clean up the list containing DataPoints instances created by
		// this Agent, without deleting the DataPoint instances themselves
		// since they are owned by the GenericAgent
		m_datapoints.clear();
        m_dataNodes.clear();
		cleanUpDataPointLib();

		if (NULL != m_driverHelper)
		{
			delete m_driverHelper;
            m_driverHelper = NULL;
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
		
		// clean up handle to SummaryProcessor
		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
		}
		
		// remove handle to ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_scadaProxyFactory = NULL;
		// clean up handle to ThreadPoolSingletonManager
		if (0 != m_threadPoolSingletonManager)
		{
			TA_Base_Core::ThreadPoolSingletonManager::removeInstance (m_threadPoolSingletonManager);
			m_threadPoolSingletonManager  = 0;
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
        FUNCTION_EXIT;
	}


    void SystemStatusAgent::agentTerminate()
	{
		FUNCTION_ENTRY("agentTerminate");

		// if bulk interface is created
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

		// else do nothing

		m_driverHelper->onTerminate();

		FUNCTION_EXIT;
	}


    bool SystemStatusAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                               const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                               TA_Base_Bus::IEntityList& createdEntities)
    {
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

            createdEntities.push_back( 
                TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), createEntity(*iter)) );
        }

        // return true to indicate not to call createEntity
        return true;
    }


    void SystemStatusAgent::entityCreationComplete()
    {
        // loading complete, free some memory
        TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
    }


	TA_Base_Bus::IEntity* SystemStatusAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		FUNCTION_ENTRY("createEntity");

		TA_Base_Bus::IEntity * theEntity = 0;
		
		std::string entityDataType = EntityData->getType();

		// if the specified entity is not of the type DataPoint...
		if ( TA_Base_Core::DataPointEntityData::getStaticType() == entityDataType )
		{
			// cast the EntityData to DataPointEntityData
            TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
                boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(EntityData);
            TA_ASSERT(dataPointEntityData.get() != 0, "Unable to cast when clearly it should work");

			//Returns a registered datapoint CORBA object
			TA_Base_Bus::DataPoint* dp = m_dataPointFactory->createDataPoint ( dataPointEntityData );

			//If we could not create a datapoint with this configuration...
			if (0 == dp)
			{
				//add this to the set of failed keys and return 0
				m_failedDpNames.insert(EntityData->getName());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"createDataPoint() on pkey %u failed", EntityData->getKey());
			}

            // this agent can host virtual DP, which require no driver
            if (false == dp->isVirtual() )
            {
                //see if one of the avaialable IDataPointDriver implimentations
                //will own this point:
                try
                {
                    /**
                      * giveDataPointToADriver
                      * @postcondition Assuming the method does not throw, datapoint you pass 
                      * will be kept updated by some IDataPointDriver implimentation. If this
                      * method does throw the datapoint will be unchanged
                      */
                    m_driverHelper->giveDataPointToADriver(dp);
                }
                catch(...) //if the call failed no driver own the point so...
                {
                    //add this to the set of failed keys and return 0
                    m_failedDpNames.insert(EntityData->getName());
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "giveDataPointToADriver() on pkey %u failed", EntityData->getKey());
                }
            }

			//add the datapoint to the list of points you own
            m_datapoints[ dp->getEntityKey() ] = dp;

			// verify that the quality of the point to TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED
			std::ostringstream msg;
			msg << "Quality for datapoint (pkey " << EntityData->getKey() << ") is set to " << dp->getStatus();
			std::string str = msg.str();

			TA_ASSERT(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED == dp->getStatus() ||
				TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE  == dp->getStatus(), str.c_str()); 

			theEntity = dp;
		}
		else if(entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType())
		{
			TA_Base_Bus::DataNode* dn = NULL;

			try
			{
				dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence);

                m_dataNodes.push_back( dn );
				theEntity = dn;
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    EntityData->getName().c_str(), EntityData->getKey() );
				
				// To ensure the generic agent is constructed even if one DataNode creation fails
			}
		}
		else
		{
			//add this to the set of failed names and return 0
			m_failedDpNames.insert(EntityData->getName());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Entity %s [%u] is not a supported type",
                EntityData->getName().c_str(), EntityData->getKey());
		}

		FUNCTION_EXIT;
        return theEntity;
	}

	void SystemStatusAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

		dataPointList.clear();
		
        dataPointList = m_datapoints;
		
	}


    void SystemStatusAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");
		
		//If you are already in monitor mode...
		if (m_operationMode == TA_Base_Core::Monitor)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");

			FUNCTION_EXIT;

			return;
		}

		// Bohong++
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp;
		for (itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
		{
			// make the data point operated in Monitor mode
			itrDp->second->setToMonitorMode();
		}

		m_driverHelper->setMonitor();

		m_operationMode = TA_Base_Core::Monitor;

		// if bulk interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
		}

        if (NULL != m_dpAccessInterface)
        {
            m_dpAccessInterface->setToMonitor();
        }

		// if bulk interface is set up
		if ( NULL != m_dnBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dnBulkPolledInterface->setToMonitor();
		}
		
        if (NULL != m_dnAccessInterface)
        {
            m_dnAccessInterface->setToMonitor();
        }

		// set all datanodes to monitor mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();


		//currently does nothing
		registerForStateUpdates();

		FUNCTION_EXIT;
	}


    void SystemStatusAgent::agentSetControl()
	{
		FUNCTION_ENTRY("agentSetControl");
	
		//If you are already in control mode...
		if (m_operationMode == TA_Base_Core::Control)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");

			FUNCTION_EXIT;
			return;
		}

        // for each of the data point associated with this agent
		// Bohong++ we need to raise a new thread to do following works when failover
		// to reduce the time failover takes
		if(true == isFirstLaunch)
		{
			std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp;
			for (itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
			{
				// make the data point operated in Control mode
				itrDp->second->setToControlMode();
			}
		}
		else
		{
			failoverLoadingThread* failoverThread = new failoverLoadingThread(this);
			failoverThread->start();			
		}

		m_driverHelper->setControl();

		m_operationMode = TA_Base_Core::Control;

		// set the proxies to control mode
		m_scadaProxyFactory->setProxiesToControlMode();


		// if bulk interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// start receiving remote requests
			m_dpBulkPolledInterface->setToControl();
		}

        if (NULL != m_dpAccessInterface)
        {
            m_dpAccessInterface->setToControl();
        }

		// set all datanodes to control mode
		for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
		{
			(*itr)->setOperationMode( true );
        }

		if ( NULL != m_dnBulkPolledInterface )
		{
			// start receiving remote requests
			m_dnBulkPolledInterface->setToControl();
		}
		
        if (NULL != m_dnAccessInterface)
        {
            m_dnAccessInterface->setToControl();
        }

		// else do nothing

		//currently does nothing
		registerForStateUpdates();

		FUNCTION_EXIT;
	}


    void SystemStatusAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("processOnlineUpdate");

		//make sure your config data is up to date
		//m_agentEntityData->invalidate();
		
		//If this is an update on the system status agent...
		if ( m_agentKey == updateEvent.getKey() )
		{
			//Just pass the update along to the DataPointDriverHelper
			std::vector<std::string> changedParams = updateEvent.getChangedParams();
			m_driverHelper->processOnlineAgentUpdate(changedParams);
		}
		else //it's either a datapoint update or bogus data
		{
			// Must be for the child entities (datapoints)
			// Will be an Update, Create, or Delete modification.
			try
			{
//				TA_Base_Bus::DataPoint* theDataPoint = 0;
//				unsigned long updateEventPKey = updateEvent.getKey();
//
//				const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap = m_genericAgent->getEntityMap();
//				std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator it = entityMap->find(updateEventPKey);
//
//				if (it != entityMap->end())
//				{
//					theDataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>((*it).second);
//					TA_ASSERT(0 != theDataPoint, "dynamic_cast to DataPoint failed");
//
//					// get the type of updating event
//					switch ( updateEvent.getModifications() )
//					{
//                    case TA_Base_Core::Update:
//                    case TA_Base_Core::Delete:
//                    case TA_Base_Core::Create:
//                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
//                            "Online update of datapoints are not supported. You will need to restart the agent for these changes to take effect.");
//                        break;
//                        
//                    default:
//                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
//                            "updateEvent.getModifications() returned unrecognised update type");
//                    }
//				}
//				else
//				{
//					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
//						"processOnlineUpdate() called for update on entity I do not own. Key = %u", updateEvent.getKey());
//				}
			} // end try block  
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "exception thrown while trying to process onLine update of datapoint");

				throw;
			}
		}

		FUNCTION_EXIT;
	}


	void SystemStatusAgent::run()
	{
		FUNCTION_ENTRY("run()");

		TA_ASSERT(0 != m_genericAgent, "m_genericAgent pointer is null");


		/**
		  * initAlarmsOnThisAgent
		  *
		  * Clear any alarms on this agent (entity versus entity type)
		  * Then raise a single alarm if a datapoint passed in createEntity
		  * can not be registered by the DatapointFactory or owned by an avaiable
		  * driver
		  *
		  * Note that this agent raises alarms on itself (versus a datapoint)
		  * if it can't create or keep-updated any datapoint see 
		  * DataPointDriverHelper::giveDataPointToADriver()
		  */
		initAlarmsOnThisAgent();
		
		//Do not set the m_operationMode member here! It will be set in following call
		//to SetMonitor or SetControl
		//Note it is initialized to TA_Base_Core::NotApplicable in CTOR to drive init
		TA_Base_Core::EOperationMode operationMode = m_genericAgent->getOperationMode();

		switch(operationMode)
		{
			case TA_Base_Core::Control :
			{
				agentSetControl();
				break;
			}
			case TA_Base_Core::Monitor :
			{
				agentSetMonitor();
				break;
			}
			default:
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"SystemStatusAgent operation mode was not set to monitor or control on init. Defaulting to monitor mode");

				agentSetMonitor();
			}
		} //end switch

		// create an instance of the class DataPointAgentCorbaDef_Impl
		m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentEntityData->getName(), this );
        m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl(m_agentEntityData->getName(), this);
		m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_agentEntityData->getName(), this );
        m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl(m_agentEntityData->getName(), this);


		if ( operationMode == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dpBulkPolledInterface->setToControl();
			m_dpAccessInterface->setToControl();
			m_dnBulkPolledInterface->setToControl();
			m_dnAccessInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
			m_dpAccessInterface->setToMonitor();
			m_dnBulkPolledInterface->setToMonitor();
			m_dnAccessInterface->setToMonitor();
		}
        

		//At this point in the code your are saying that you have applied the mode
		//and are good to go

		//Bohong++
		isFirstLaunch = false;

		m_genericAgent->run();

		FUNCTION_EXIT;
	}

    void SystemStatusAgent::notifyGroupOffline( const std::string& group )
	{
		FUNCTION_ENTRY("notifyGroupOffline");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"This agent takes no action on based on state of the System Controller");
		FUNCTION_EXIT;
	}


    void SystemStatusAgent::notifyGroupOnline( const std::string& group )
	{
		FUNCTION_ENTRY("notifyGroupOnline");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"This agent takes no action on based on state of the System Controller.");
		FUNCTION_EXIT;
	}

    void SystemStatusAgent::registerForStateUpdates()
	{
		FUNCTION_ENTRY("registerForStateUpdates");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"Given that I don't register for any messages this call doesn't do a bloody thing.");
 		FUNCTION_EXIT;
	}

    void SystemStatusAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");
    
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
		"Given that I don't register for any messages I find it deeply disturbing that receiveSpecialisedMessage() has been called.");

		FUNCTION_EXIT;
	}

	void SystemStatusAgent::initAlarmsOnThisAgent()
	{
		FUNCTION_ENTRY("clearAlarmsOnThisAgent");

		unsigned long agentKey = m_agentEntityData->getKey();
		unsigned long location = m_agentEntityData->getLocation();

		// Get an alarm helper
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::SystemStatusAgentAlarms::DataPointNotCreated, agentKey, location);
		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::SystemStatusAgentAlarms::NoLanStatusPoint, agentKey, location);

		//TODO:
		//If you add any new alarm types raised on the agent (vs. children) add a new close here

		//If any datapoints could not be created or owned in createEntity call...
		if (m_failedDpNames.size() > 0)
		{
			//the string should be the first name in the list, with an additional ...
			//if there is more than one name
			std::ostringstream nameString;
            std::set<std::string>::iterator iter = m_failedDpNames.begin();
			nameString << *iter;
			if (1 < m_failedDpNames.size())
			{
				nameString << "...";
			}			
				

			// Set up the description parameters
			TA_Base_Core::DescriptionParameters dp;
			dp.push_back(new TA_Base_Core::NameValuePair("DataPointName", nameString.str().c_str() ) );

			// Submit the alarm
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset (TA_Base_Core::SystemStatusAgentAlarms::DataPointNotCreated, 
				m_agentEntityData->getKey(),
				m_agentEntityData->getTypeKey(),
				dp,
				m_agentEntityData->getName(),
				m_agentEntityData->getLocation(),
				m_agentEntityData->getSubsystem(),
				m_agentEntityData->getStringParamValue(TA_Base_Core::SystemStatusAgentEntityData::ASSET_NAME_PARAM),
				TA_Base_Core::AlarmConstants::defaultTime );
		}
		
		FUNCTION_EXIT;
	}


	TA_Base_Bus::DataPoint * SystemStatusAgent::getDataPoint ( unsigned long entityKey )
	{
    	FUNCTION_ENTRY("getDataPoint");
		TA_Base_Bus::DataPoint* theDataPoint = NULL;

		try
		{
			// Find datapoint
			const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap = m_genericAgent->getEntityMap();
			std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator it = entityMap->find(entityKey);

			if (it != entityMap->end())
			{		
				theDataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>((*it).second);
			}
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "getDataPoint() - Exception thrown while retrieving datapoint");
		}

		FUNCTION_EXIT;
		return theDataPoint;
	}

	TA_Base_Bus::DataNode * SystemStatusAgent::getDataNode( unsigned long entityKey )
	{
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

	void SystemStatusAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }
    
    
    TA_Base_Bus::DataPointFactory* SystemStatusAgent::getDataPointFactory()
    {
        TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
        
        return m_dataPointFactory;
    }

	// Bohong++
	void SystemStatusAgent::failoverSpecific()
	{
		FUNCTION_ENTRY("failoverSpecific");

		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp;
		for (itrDp = m_datapoints.begin(); itrDp != m_datapoints.end(); ++itrDp)
		{
			// make the data point operated in Control mode
			itrDp->second->setToControlMode();
		}

		FUNCTION_EXIT;
	}
}



