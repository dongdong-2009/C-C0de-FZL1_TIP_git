///////////////////////////////////////////////////////////
//  DataLogAgent.cpp
//  Implementation of the Class DataLogAgent
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:03
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#include <sstream>
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/DataLogStateUpdate_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataLogAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h" 
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/threads/src/Thread.h"
#include "core/threads/src/ThreadMemberFunction.h"
#include "core/datalog/IDL/src/DataLogMessageCorbaDef.h"
#include "bus/datalog/datalog_library/src/LocalRunParamDefinitions.h"

#include "bus/datalog/datalog_library/src/DataLogAgentImpl.h"
#include "bus/datalog/datalog_library/src/DataLogManager.h"
#include "DataLogAgent.h"

using TA_Base_Bus::GenericAgent;
using TA_Base_Bus::IEntity;
using TA_Base_Core::IEntityData;
 
using TA_Base_Core::ConfigUpdateDetails;
using TA_Base_Core::StateUpdateMessageCorbaDef;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::DataLogSyncCorbaDef;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
 
using TA_Base_Core::GenericAgentException;
using TA_Base_Bus::DataLogAgentImpl;
using TA_Base_Bus::DataLogManager;
namespace TA_Base_App
{
	typedef TA_Base_Core::ThreadedMemberFunction< TA_Base_App::DataLogAgent > AgentThreadedMemberFunction;
	
	DataLogAgent::DataLogAgent(int argc, char* argv[])
		: m_genericAgent(NULL)
    {
        FUNCTION_ENTRY("DataLogAgent");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start to new GenericAgent in DataLogAgent Constructor.");
        m_genericAgent = new GenericAgent(argc, argv, this, NULL, AGENT_USER_TYPE_NORMAL);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New GenericAgent in DataLogAgent Constructor Ends.");

        TA_ASSERT( RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), 
			"Location Key was not set as run param" );
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start to call setupRunParams() in DataLogAgent Constructor.");
        setupRunParams();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "setupRunParams() in DataLogAgent Constructor Ends.");	 	
		 
		

        FUNCTION_EXIT;
    }

	DataLogAgent::~DataLogAgent()
	{
		FUNCTION_ENTRY("~DataLogAgent");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "~DataLogAgent() enter.");

		DataLogManager::removeInstance();
        if( m_genericAgent != NULL )
        {
            m_genericAgent->deactivateAndDeleteServant();
            m_genericAgent = NULL;
        }       
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "~DataLogAgent() exit.");
        FUNCTION_EXIT;
	}

	void DataLogAgent::agentTerminate()
	{
		FUNCTION_ENTRY("agentTerminate");       
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "agentTerminate() enter.");

		DataLogManager::removeInstance();
		
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);				 
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "agentTerminate() exit.");
        FUNCTION_EXIT;
		
	}
   
	//
	// createEntity
	//
	IEntity* DataLogAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		return NULL;
	}
    
	//
	// agentSetMonitor
	//
	void DataLogAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Requesting from External to switch agent to monitor mode" );

		//limin++--, failover
		AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
		thrd.add( &DataLogAgent::unsubscribeToMessages );
		thrd.add( &DataLogAgent::registerForMonitorMessages);
		thrd.start();	 

		FUNCTION_EXIT;
	}


	//
	// limin, failover
	//
	void DataLogAgent::unsubscribeToMessages()
	{
		MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
	}
  
	//
	// agentSetControl
	//
	void DataLogAgent::agentSetControl()
	{
		FUNCTION_ENTRY("agentSetControl");

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Requesting from External to switch agent to control mode" );

		//limin++--, failover
		AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
		thrd.add( &DataLogAgent::unsubscribeToMessages );
		thrd.add( &DataLogAgent::registerForControlMessages);
		thrd.start(); 

		FUNCTION_EXIT;
	}


	//
	// notifyGroupOffline
	//
	void DataLogAgent::notifyGroupOffline( const std::string& group )
	{}


	//
	// notifyGroupOnline
	//
	void DataLogAgent::notifyGroupOnline( const std::string& group )
	{}


	//
	// registerForControlMessages
	//
	void DataLogAgent::registerForControlMessages()
	{
		FUNCTION_ENTRY("registerForControlMessages");

		MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
			TA_Base_Core::DataLogStateUpdate::DataLogAgentStateRequest,
			this,
			getAgentEntityName());

		FUNCTION_EXIT;
	}


	//
	// registerForMonitorMessages
	//
	void DataLogAgent::registerForMonitorMessages()
	{
		FUNCTION_ENTRY("registerForMonitorMessages");

		MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
			TA_Base_Core::DataLogStateUpdate::DataLogStateUpdate,
			this,
			getAgentEntityName());
 

 
		FUNCTION_EXIT;
	}


	//
	// registerForStateUpdates
	//
	void DataLogAgent::registerForStateUpdates()
	{
		FUNCTION_ENTRY ("registerForStateUpdates");

		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
		{
			//limin++--, failover
			AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
			thrd.add( &DataLogAgent::registerForControlMessages);
			thrd.start();

	 
		}
		else if ( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
		{
			//limin++--, failover
			AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
			thrd.add( &DataLogAgent::registerForMonitorMessages);
			thrd.start();
 
		}
		else
		{
			const char * errorMsg = "the control/monitor state is not set"; 
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMsg);
			throw TA_Base_Core::OperationModeException(errorMsg);
		}

		FUNCTION_EXIT;
	}


	//
	// receiveSpecialisedMessage
	//
	void DataLogAgent::receiveSpecialisedMessage(const StateUpdateMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");

		if( 0 == std::string(message.messageTypeKey).compare(TA_Base_Core::DataLogStateUpdate::DataLogAgentStateRequest.getTypeKey()) )
		{
			receiveRequestForStateUpdateMsg();
			FUNCTION_EXIT;
			return;
		}
		 
		{
			// handle message
			processMessageUpdate(message);
		}
 

		FUNCTION_EXIT;
	}


	//
	// processOnlineUpdate
	//
	void DataLogAgent::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
	{
	}


	//
	// run
	//
	void DataLogAgent::run()
	{
		LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "run come in");


		DataLogManager::getInstance()->setupDataLogAgent(RunParams::getInstance().get(RPARAM_ENTITYNAME));

        //begin to monitor data points
		DataLogManager::getInstance()->init();

        
		
		LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "run Init completed");
		 
		m_genericAgent->run();

		LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "run come out");

	}

	void DataLogAgent::processMessageUpdate(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("processQueueUpdates");
		//handle state update message
		if( 0 == TA_Base_Core::DataLogStateUpdate::DataLogStateUpdate.getTypeKey().compare(message.messageTypeKey) )
		{
// 			DataLogSyncCorbaDef* datalogSync;
// 			if( (message.stateUpdateInfo >>= datalogSync) != 0 )
// 			{
// 				DataLogManager::getInstance()->handleStateMesssage(*datalogSync);
// 			}			
		}		 
		FUNCTION_EXIT;
	}
  
	void DataLogAgent::receiveRequestForStateUpdateMsg()
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "receiveRequestForStateUpdateMsg");

		 
	}


	//
	// setupRunParams
	//
	void DataLogAgent::setupRunParams()
	{
		FUNCTION_ENTRY("setupRunParams");

		try
		{
			
			TA_Base_Core::DataLogAgentEntityDataPtr entityData = 
				boost::dynamic_pointer_cast<TA_Base_Core::DataLogAgentEntityData>(m_genericAgent->getAgentEntityConfiguration());

			if( entityData == NULL )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Get DataLogAgentEntityData failed.");
				TA_THROW(TA_Base_Core::GenericAgentException("Get DataLogAgentEntityData failed."));
			}
 			unsigned long key = entityData->getKey();
			int dataMaxTimeout=entityData->getDataMaxTimeout();
            int timeoutDelay = entityData->getTimeoutDelay();
			int maxDelayTimes = entityData->getMaxDelayTimes();
			int batchUpdateSize = entityData->getBatchUpdateSize();

			std::ostringstream entityStream;
			entityStream<<key;

 
 			unsigned long location = atol( (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str() );
 
			TA_Base_Core::ILocation* locationInfo = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(location);
			std::string locationName = locationInfo->getName();
			delete locationInfo;
			locationInfo = NULL; 
 
 			RunParams::getInstance().set(TA_Base_Bus::TA_DataLogAgent::ENTITY_KEY.c_str(), entityStream.str().c_str());
			RunParams::getInstance().set(TA_Base_Bus::TA_DataLogAgent::LOCATION_NAME.c_str(), locationName.c_str());

			DataLogManager::getInstance()->setDataMaxTimeout(dataMaxTimeout);
			DataLogManager::getInstance()->setTimeoutDelay(timeoutDelay);
			DataLogManager::getInstance()->setBatchUpdateSize(batchUpdateSize);
			DataLogManager::getInstance()->setMaxDelayTimes(maxDelayTimes);

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
			TA_THROW(TA_Base_Core::GenericAgentException("Unknown exception raised while initialising DataLogAgent."));
		}

		FUNCTION_EXIT;
	}

	
 
}