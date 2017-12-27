/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingAgent.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/11/01 15:29:57 $
  * Last modified by:  $Author: huangjian $
  *
  * Creates the Scheduling Agent and starts it.
  *
  */



#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DataConversion.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ISchedulingAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/SchedulingAgentEntityData.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/SchedulingStateUpdate_MessageTypes.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "app/scheduling/scheduling_agent/src/SchedulingMessageSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingSessionManager.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAccessManager.h"
#include "app/scheduling/scheduling_agent/src/JobQueue.h"
#include "app/scheduling/scheduling_agent/src/JobFactory.h"
#include "app/scheduling/scheduling_agent/src/SchedulingController.h"
#include "app/scheduling/scheduling_agent/src/SchedulingMonitor.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceManager.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAgent.h"

    
namespace TA_Base_App
{

const char* SchedulingAgent::RPARAM_WORKSTATIONID = "WorkstationId";


SchedulingAgent::SchedulingAgent(int argc, char* argv[]) : m_genericAgent(NULL)
{
    m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this, NULL, AGENT_USER_TYPE_NORMAL);

    try
    {
        initialiseSingletons();
    }
    catch(const TA_Base_Core::RightsException& re)
    {
        TA_THROW(TA_Base_Core::GenericAgentException(re.what()));
    }
    catch(const TA_Base_Core::AuthenticationAgentException& ae)
    {
        TA_THROW(TA_Base_Core::GenericAgentException(ae.what()));
    }
    catch(const TA_Base_Core::AuthenticationSecurityException& ase)
    {
        TA_THROW(TA_Base_Core::GenericAgentException(ase.what()));
    }
    catch(const TA_Base_Core::DatabaseException& dbe)
    {
        TA_THROW(TA_Base_Core::GenericAgentException(dbe.what()));
    }
    catch(const TA_Base_Core::DataException& de)
    {
        TA_THROW(TA_Base_Core::GenericAgentException(de.what()));
    }
    catch(const CORBA::Exception& ex)
    {
        std::string message = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        TA_THROW(TA_Base_Core::GenericAgentException(message.c_str()));
    }
    catch(const std::exception& ex)
    {
        TA_THROW(TA_Base_Core::GenericAgentException(ex.what()));
    }
    catch(...)
    {
        TA_THROW(TA_Base_Core::GenericAgentException("Unknown exception caught"));
    }
}



void SchedulingAgent::initialiseSingletons()
{
    //
    // Get the entity's data from the database and from run params
    //
    TA_Base_Core::SchedulingAgentEntityDataPtr entityData =
		boost::dynamic_pointer_cast<TA_Base_Core::SchedulingAgentEntityData>(
            m_genericAgent->getAgentEntityConfiguration());
    
    TA_ASSERT(entityData.get() != NULL, "Entity Data should never be NULL");

    //
    // Database parameters
    //
    unsigned long entityKey    = entityData->getKey();
    unsigned long typeKey      = entityData->getTypeKey();
    unsigned long subsystemKey = entityData->getSubsystem();
    unsigned long locationKey  = entityData->getLocation();
    std::string entityName     = entityData->getName();
    unsigned long operatorID   = entityData->getOperatorID();
    std::string password       = entityData->getPassword();
	std::string assetName      = entityData->getAssetName();
	unsigned long parentEntityKey	= entityData->getParent();
    

    //
    // Run params
    //
    unsigned long workstationID = 0;
    if( ! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_WORKSTATIONID) )
    {
        TA_THROW( TA_Base_Core::GenericAgentException( "WorkstationID RunParam must be set") );
    }

    std::string workstationIDAsString = TA_Base_Core::RunParams::getInstance().get(RPARAM_WORKSTATIONID);

    workstationID = TA_Base_Core::DataConversion::toUnsignedInteger( workstationIDAsString );

    //
    // Check that all the fields are valid
    //
    std::string errorMessage = "";
    if(entityKey == 0)
    {
        errorMessage = "Entity key is zero";
    }
    else if(typeKey == 0)
    {
        errorMessage = "Type key is zero";
    }
    else if(entityName == "")
    {
        errorMessage = "Entity Name is blank";
    }

    if(errorMessage != "")
    {
        TA_THROW(TA_Base_Core::GenericAgentException(errorMessage.c_str()));
    }



    //
    // Initialise the singletons
    //
    LOG_GENERIC(
        SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Initializing singletons. entityKey: '%d', subsystemKey: '%d', "
        "locationKey: '%d', workstationKey: '%d', typeKey: '%d', "
        "entityName: '%s', operatorID: '%d', assetName: '%s' ",
        entityKey, subsystemKey, locationKey, workstationID, typeKey,
        entityName.c_str(), operatorID, assetName.c_str() );

    JobFactory::getInstance().initialise(*entityData);

    JobQueue::getInstance().setSchedulingAgentOperatorID(operatorID);

    SchedulingMessageSender::getInstance().initialise(
        entityKey, parentEntityKey, subsystemKey, locationKey);

    SchedulingAlarmSender::getInstance().initialise(
        entityKey, typeKey, entityName, locationKey, subsystemKey, assetName);

    SchedulingSessionManager::getInstance().initialise(
        operatorID, workstationID, password);

    SchedulingAccessManager::getInstance().initialise(entityKey);

    SchedulingMonitor::getInstance().initialise(
        entityKey, subsystemKey, locationKey);

}

SchedulingAgent::~SchedulingAgent()
{
    if( m_genericAgent != NULL )
    {
        m_genericAgent->deactivateAndDeleteServant();
        m_genericAgent = NULL;
    }
}



void SchedulingAgent::agentTerminate()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Terminating the Scheduling Agent");

    SchedulingController::getInstance().disable();

    //
    // Unsubscribe to messages
    //
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    //
    // Terminate any threads that are running
    //
    JobRunner::getInstance().terminateAndWait();
    JobQueue::getInstance().terminateAndWait();
}


TA_Base_Bus::IEntity* SchedulingAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
{
    return NULL;
}


void SchedulingAgent::agentSetMonitor()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting Agent to monitor mode");

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    // If this object is in control mode,
    // deactivate the controller,
    // activate the monitor
    // put the JobQueue in monitor mode.
    SchedulingController::getInstance().disable();
    SchedulingMonitor::getInstance().activate();
    JobQueue::getInstance().setMonitorMode();

    registerForMonitorMessages();
}

void SchedulingAgent::agentSetControl()
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting Agent to control mode");

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    // If this object is in monitor mode,
    // deactivate the monitor,
    // activate the controller and
    // put the JobQueue in control mode.
    SchedulingMonitor::getInstance().deactivate();
    SchedulingController::getInstance().enable();
    JobQueue::getInstance().setControlMode();

    registerForControlMessages();
}


void SchedulingAgent::registerForControlMessages()
{
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateBatchRequest, this, getAgentEntityName());
}


void SchedulingAgent::registerForMonitorMessages()
{
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateBatchUpdate, this, getAgentEntityName());
}


void SchedulingAgent::registerForStateUpdates()
{
    //
    // If the Agent is in control mode, subscribe to BatchUpdateRequests.
    //
    if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
    {
        registerForControlMessages();
    }
    //
    // If the Agent is in monitor mode, subscribe to BatchUpdates.
    //
    else
    {
        registerForMonitorMessages();
    }
}


void SchedulingAgent::receiveSpecialisedMessage(
    const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
    //
    // BatchUpdate received
    //
    if( std::string(message.messageTypeKey) ==
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateBatchUpdate.getTypeKey() )
    {
        receiveBatchUpdate(message);
    }

    //
    // BatchUpdateRequest received
    //
    else if( std::string(message.messageTypeKey) ==
             TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateBatchRequest.getTypeKey() )
    {
        receiveBatchUpdateRequest(message);
    }
}


void SchedulingAgent::receiveBatchUpdate(
    const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
    TA_ASSERT(
        std::string(message.messageTypeKey) ==
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateBatchUpdate.getTypeKey(),
        "receiveBatchUpdate() called with message that isn't a batch update!" );
    

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Batch Update received");

	TA_Base_Bus::SchedulingAgentCorbaDef::BatchUpdateCorbaDef* batchUpdate = NULL;

    if( (message.stateUpdateInfo >>= batchUpdate) != 0 )
    {
        SchedulingMonitor::getInstance().batchUpdateReceivedFromController(
            (*batchUpdate).jobs);
        
        //
        // Initialisation has completed so the initialiseAgentMonitorMode
        // method can now return.
        //
        LOG_GENERIC(
            SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Setting m_batchUpdateReceived to true.");

        m_batchUpdateReceived = true;
    }
}


void SchedulingAgent::receiveBatchUpdateRequest(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
    TA_ASSERT(
        std::string(message.messageTypeKey) ==
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateBatchRequest.getTypeKey(),
        "receiveBatchUpdateRequest() called with message that isn't a "
        "batch update request!" );
    

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Batch Update Request received");

    //
    // When a batch update request is received, send a batch update
    //
    sendBatchUpdateMessage();
}


void SchedulingAgent::sendBatchUpdateMessage()
{
    //
    // Lock the thread so that it waits for initialisation to complete
    // before responding to the batch update request.
    //
    TA_Base_Core::ThreadGuard lock(m_sendBatchUpdateLock);

    //
    // Create a batch update containing all the jobs that are currently
    // scheduled
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::BatchUpdateCorbaDef batchUpdate;

    batchUpdate.jobs =
        JobInstanceManager::getInstance().getAllJobs();
    batchUpdate.jobInstances =
        JobInstanceManager::getInstance().getJobInstances();

    CORBA::Any stateUpdateData;
    stateUpdateData <<= batchUpdate;

    //
    // Send the batch update
    //
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending batch update");

    TA_Base_Core::StateUpdateMessageSender* messageSender =
        TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
        TA_Base_Core::SchedulingStateUpdate::Context);

    messageSender->sendStateUpdateMessage(
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateBatchUpdate,
        0,
        getAgentEntityName().c_str(),
        stateUpdateData);

    delete messageSender;
}



void SchedulingAgent::processOnlineUpdate(
    const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    //
    // Get the entity's data from the database
    //
    TA_Base_Core::SchedulingAgentEntityDataPtr entityData =
		boost::dynamic_pointer_cast<TA_Base_Core::SchedulingAgentEntityData>(
            m_genericAgent->getAgentEntityConfiguration());
    
    TA_ASSERT(entityData.get() != NULL, "Entity Data should never be NULL");

    //
    // Make sure the data is fresh (ie not cached)
    //
    entityData->invalidate();

    unsigned long operatorID = entityData->getOperatorID();
    std::string password     = entityData->getPassword();

    LOG_GENERIC(
        SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Online update received. Updating operatorID ('%d') and password",
        operatorID);


    //
    // Update the operatorID and password
    //
    SchedulingSessionManager::getInstance().setOperatorID(operatorID);
    JobQueue::getInstance().setSchedulingAgentOperatorID(operatorID);
    SchedulingSessionManager::getInstance().setPassword(password);
}


void SchedulingAgent::notifyGroupOffline(const std::string& group)
{
    //
    // The Scheduling Agent is not affected by this method.
    //
}

void SchedulingAgent::notifyGroupOnline(const std::string& group)
{
    //
    // The Scheduling Agent is not affected by this method.
    //
}


void SchedulingAgent::run()
{
    FUNCTION_ENTRY("run");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Starting Scheduling Agent");


    //
    // This will be set to a value if there is an exception
    //
    std::string message = "";

    try
    {
        //
        // If the Agent starts in control mode
        //
        if(m_genericAgent->getOperationMode() == TA_Base_Core::Control)
        {
            initialiseAgentControlMode();
        }
        //
        // If the Agent starts in monitor mode
        //
        else if(m_genericAgent->getOperationMode() == TA_Base_Core::Monitor)
        {
            initialiseAgentMonitorMode();
        }
        else
        {
            message = "Operation mode not recognised: ";
            message += TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE);

            //TA_ASSERT(false, message.c_str());
        }
    }
    catch(const std::exception& ex)
    {
        message = "Error: '" + std::string(ex.what()) + "'";
    }
    catch(const CORBA::Exception& ex)
    {
        message = "Communications Error: '"
                + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) + "'";
    }

    //
    // If message is not blank, there was an exception, so raise an alarm.
    //
    if(message != "")
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, message.c_str());
        
        SchedulingAlarmSender::getInstance().initialisationFailed(message);
    }
    else
    {
        //
        // Start the Generic Agent
        //
        m_genericAgent->run();
    }
}


void SchedulingAgent::initialiseAgentControlMode()
{
    TA_ASSERT(m_genericAgent->getOperationMode() == TA_Base_Core::Control,
              "initialiseAgentControlMode() must be called on agent in "
              "control mode");

    //
    // Lock the thread so that when a batch update request is received,
    // this method will complete before a batch update is sent in reply.
    //
    TA_Base_Core::ThreadGuard lock(m_sendBatchUpdateLock);

    LOG_GENERIC(
        SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Initialising Agent in control mode");


    //
    // Start the JobRunner
    //
    JobRunner::getInstance().start();

    //
    // Tell the JobQueue to read its jobs from the database,
    // put it in control mode, and start it.
    //
    JobQueue::getInstance().readJobsFromDatabase();
    JobQueue::getInstance().setControlMode();
    JobQueue::getInstance().start();


    //
    // Activate the Controller so that it will be able to receive
    // callbacks.
    //
    SchedulingController::getInstance().enable();
}


void SchedulingAgent::initialiseAgentMonitorMode()
{
    TA_ASSERT(m_genericAgent->getOperationMode() == TA_Base_Core::Monitor,
              "initialiseAgentMonitorMode() must be called on agent in "
              "monitor mode");

    LOG_GENERIC(
        SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Initialising Agent in monitor mode");

    //
    // Send a message requesting a state update and then wait for the
    // controlling agent to respond.
    // When it does respond, the receiveSpecialisedMessage method will be
    // called.
    //
    CORBA::Any nothing;

    //
    // Send the batch update request
    //
    LOG_GENERIC(
        SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending batch update request");

    m_batchUpdateReceived = false;

    TA_Base_Core::StateUpdateMessageSender* messageSender =
        TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
        TA_Base_Core::SchedulingStateUpdate::Context);

    messageSender->sendStateUpdateMessage(
            TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateBatchRequest,
            0,
            getAgentEntityName().c_str(),
            nothing);

    delete messageSender;


    //
    // Start the JobRunner
    //
    JobRunner::getInstance().start();


    //
    // Start the job queue.
    // This doesn't actually do anything because the job queue is disabled.
    // It just gives it time to initialise while waiting for the state update
    // to come back from the controlling agent.
    //
    JobQueue::getInstance().setMonitorMode();
    JobQueue::getInstance().start();


    //
    // Sleep briefly untill the batch update comes back from the controlling agent.
    // Log a message every second so we know what we are doing.
    //
    LOG_GENERIC(
        SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "Waiting to receive batch update from controlling agent.");

    unsigned int milliSecondsWaited = 0;
    while( ! m_batchUpdateReceived )
    {
        TA_Base_Core::Thread::sleep(100);

        milliSecondsWaited += 100;

        if(milliSecondsWaited % 1000 == 0)
        {
            LOG_GENERIC(
                SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Waited %d milli-seconds.", milliSecondsWaited);
        }
    }


    //
    // Activate the monitor now that the batch update has been received.
    //
    SchedulingMonitor::getInstance().activate();


    LOG_GENERIC(
        SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
        "Batch update received. Exiting initialiseMonitorMode method.");
}

}; // TA_Base_App
