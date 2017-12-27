// EventAgent.cpp : Defines the entry point for the console application.
//

#include "app/event/event_agent/src/EventAgent.h"
#include "app/event/event_agent/src/EventAgentImpl.h"
#include "app/event/event_agent/src/LocalRunParamDefinitions.h"
#include "app/event/event_agent/src/EventCache.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EventAgentEntityData.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/event/IDL/src/EventDetailCorbaDef.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ObjectResolutionException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Bus;
using namespace TA_Base_Core;

namespace TA_Base_App
{
	EventAgent::EventAgent( int argc, char* argv[] )
		: m_eventAgentImpl(0)
		, m_genericAgent(0)
	{
		FUNCTION_ENTRY("EventAgent");

		// Create the generic agent
		m_genericAgent = new GenericAgent(argc, argv, this, NULL, AGENT_USER_TYPE_NORMAL);

		TA_ASSERT( RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), 
			"Location Key was not set as run param" );

		// Setup runtime parameters
		setupRunParams();

		m_eventAgentImpl = new EventAgentImpl();

		//Initialize the event cache
		EventCache::getInstance().initialize();

		FUNCTION_EXIT;
	}
	
	EventAgent::~EventAgent()
	{
		FUNCTION_ENTRY("~EventAgent");
		if( m_genericAgent != NULL )
		{
			m_genericAgent->deactivateAndDeleteServant();
			m_genericAgent = 0;
		}
		if( m_eventAgentImpl != NULL )
		{
			// This *should* already be done, assuming that agentTerminate was called
			m_eventAgentImpl->deactivateAndDeleteServant();
			m_eventAgentImpl = 0;
		}

		m_myStateController.terminateAndWait();

		FUNCTION_EXIT;
	}

	void EventAgent::agentTerminate()
	{
		FUNCTION_ENTRY("agentTerminate");

		if( m_eventAgentImpl != NULL )
		{
			m_eventAgentImpl->deactivateAndDeleteServant();
			m_eventAgentImpl = 0;
		}

		MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		if (m_updatesQueue.empty() == false)
		{
			TA_THREADGUARD(m_updateQueueLock);
			// pop all the queue items
			while (m_updatesQueue.empty() == false)
			{
				const TA_Base_Core::StateUpdateMessageCorbaDef* queueItem = m_updatesQueue.front();
				if (queueItem!=NULL) 
					delete queueItem;
				m_updatesQueue.pop();
			}
		}

		FUNCTION_EXIT;
	}

	TA_Base_Bus::IEntity* EventAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("createEntity");

		FUNCTION_EXIT;
		return 0;
	}

	void EventAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");
		m_myStateController.SetAgentState(RPARAM_MONITOR);
		FUNCTION_EXIT;
	}

	void EventAgent::agentSetControl()
	{
		FUNCTION_ENTRY("agentSetControl");
		m_myStateController.SetAgentState(RPARAM_CONTROL);
		FUNCTION_EXIT;
	}

	void EventAgent::agentSetStandby()
	{
		FUNCTION_ENTRY("agentSetStandby");
		m_myStateController.SetAgentState(RPARAM_STANDBY);
		FUNCTION_EXIT;
	}

	void EventAgent::registerForStateUpdates()
	{
		FUNCTION_ENTRY("registerForStateUpdates");

		m_myStateController.SetMonitoredAgent(this);
		m_myStateController.SetAgentState(RunParams::getInstance().get(RPARAM_OPERATIONMODE));
		

		FUNCTION_EXIT;
	}

	void EventAgent::notifyGroupOffline( const std::string& group )
	{
		FUNCTION_ENTRY("notifyGroupOffline");
		//Empty
		FUNCTION_EXIT;
	}

	void EventAgent::notifyGroupOnline( const std::string& group )
	{
		FUNCTION_ENTRY("notifyGroupOnline");
		//Empty
		FUNCTION_EXIT;
	}

	void EventAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		//Todo:
		FUNCTION_EXIT;
	}

	void EventAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("processOnlineUpdate");
		//Empty
		FUNCTION_EXIT;
	}

	void EventAgent::initialize()
	{
		FUNCTION_ENTRY("Initialize");
		m_eventAgentImpl->initialize();
		
		m_myStateController.start();

		FUNCTION_EXIT;	
	}

	void EventAgent::run()
	{
		FUNCTION_ENTRY("run");
		initialize();

		m_genericAgent->run();
		FUNCTION_EXIT;
	}

	void EventAgent::processStateChange(const std::string& oldState, const std::string& newState)
	{
		FUNCTION_ENTRY("processStateChange");
		//Todo: process state change
		m_eventAgentImpl->changeStateProcess(oldState, newState);

		FUNCTION_EXIT;
	}

	//Private methods
	void EventAgent::setupRunParams()
	{
		FUNCTION_ENTRY("setupRunParams");

		try
		{
 			TA_Base_Core::EventAgentEntityDataPtr entityData = 
 				boost::dynamic_pointer_cast<EventAgentEntityData>(m_genericAgent->getAgentEntityConfiguration());

			unsigned long key = entityData->getKey();
			unsigned long typeKey = entityData->getTypeKey();
			unsigned long subsystem = entityData->getSubsystem();
			std::string assetName = entityData->getAssetName();
			unsigned long dbRefreshInterval = entityData->getDbRefreshInterval();
			unsigned long evSubmitInterval = entityData->getEVSubmitInterval();
			unsigned long maxCacheSize = entityData->getMaxCacheSize();
			unsigned long agentRefreshInterval = entityData->getAgentRefreshInterval();
			unsigned long propagateBatchSize = entityData->getPropagationBatchSize();

			char entity[50];            
			char type[50];
			char subsys[50];
			char dbRefresh[50];
			char submitInt[50];
			char maxCache[50];
			char agentRefresh[50];
			char propagateSize[50];

			sprintf(entity,"%lu",key);
			sprintf(type,"%lu",typeKey);
			sprintf(subsys,"%lu",subsystem);
			sprintf(dbRefresh,"%lu",dbRefreshInterval);
			sprintf(submitInt,"%lu",evSubmitInterval);
			sprintf(maxCache,"%lu",maxCacheSize);
			sprintf(agentRefresh,"%lu",agentRefreshInterval);
			sprintf(propagateSize,"%lu",propagateBatchSize);
			
			unsigned long location = atol( (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str() );

			TA_Base_Core::ILocation* locationInfo = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(location);
			std::string locationName = locationInfo->getName();
			delete locationInfo;
			locationInfo = NULL;

			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::ENTITY_KEY.c_str(), entity);
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::ENTITY_TYPE_KEY.c_str(), type);
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::SUBSYSTEM_KEY.c_str(), subsys);
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::LOCATION_NAME.c_str(), locationName.c_str());			
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::ASSET_NAME.c_str(), assetName.c_str());
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::DATABASE_REFRESH_INTERVAL.c_str(), dbRefresh);			
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::EVENT_SUBMIT_INTERVAL.c_str(), submitInt);
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::MAX_CACHE_SIZE.c_str(), maxCache);
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::AGENT_REFRESH_INTERVAL.c_str(), agentRefresh);
			RunParams::getInstance().set(TA_Base_App::TA_EventAgent::PROPAGATE_BATCH_SIZE.c_str(), propagateSize);

		}
		catch(TA_Base_Core::DatabaseException& dbe)
		{
			TA_THROW(TA_Base_Core::GenericAgentException(dbe.what()));
		}
		catch(TA_Base_Core::DataException& de)
		{
			TA_THROW(TA_Base_Core::GenericAgentException(de.what()));
		}
		catch(TA_Base_Core::ObjectResolutionException& oe)
		{
			TA_THROW(TA_Base_Core::GenericAgentException(oe.what()));
		}
		catch( ... )
		{
			TA_THROW(TA_Base_Core::GenericAgentException("Unknown exception raised while initialising Event Agent"));
		}

		FUNCTION_EXIT;
	}
}