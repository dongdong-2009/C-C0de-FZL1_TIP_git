/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmAgent.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "app/alarm/alarm_agent/src/AlarmAgent.h"
#include "app/alarm/alarm_agent/src/AlarmAgentImpl.h"
#include "app/alarm/alarm_agent/src/AlarmCache.h"
#include "app/alarm/alarm_agent/src/RuleRepository.h"
#include "app/alarm/alarm_agent/src/LocalRunParamDefinitions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/AlarmStateUpdate_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/AlarmAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/threads/src/Thread.h"
#include "core/threads/src/ThreadMemberFunction.h"

//TD16491++
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "bus/alarm/alarm_common/src/AlarmConstants.h"
//++TD16491

#include "bus/scada/common_library/src/ScadaUtilities.h"

using TA_Base_Bus::GenericAgent;
using TA_Base_Bus::IEntity;
using TA_Base_Core::IEntityData;
using TA_Base_Core::AlarmAgentEntityData;
using TA_Base_Core::ConfigUpdateDetails;
using TA_Base_Core::StateUpdateMessageCorbaDef;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Bus::AuthenticationLibrary;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::AlarmSequence;
using TA_Base_Core::AlarmMessageCorbaDef;
using TA_Base_Core::AlarmDetailCorbaDef;
using TA_Base_Core::AlarmUpdateCorbaDef;
using TA_Base_Core::AlarmAgentQueueItemCorbaDef;
using TA_Base_Core::AlarmAgentQueueItemSequence;
using TA_Base_Core::GenericAgentException;



namespace TA_Base_App
{

	typedef TA_Base_Core::ThreadedMemberFunction< TA_Base_App::AlarmAgent > AgentThreadedMemberFunction;
    //
    // Constructor
    //
    AlarmAgent::AlarmAgent(int argc, char* argv[])
    : m_genericAgent(NULL), 
	m_alarmAgentImpl(NULL)
    {
        FUNCTION_ENTRY("AlarmAgent");
        m_genericAgent = new GenericAgent(argc, argv, this, NULL, AGENT_USER_TYPE_NORMAL);

        TA_ASSERT( RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), 
                   "Location Key was not set as run param" );

        setupRunParams();

        m_alarmAgentImpl = new AlarmAgentImpl();

		// TD 14082 provide an avenue for calling back
//		AlarmCache::getInstance()->setCallbackAgent(this);
        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    AlarmAgent::~AlarmAgent()
    {
        FUNCTION_ENTRY("~AlarmAgent");
        if( m_genericAgent != NULL )
        {
            m_genericAgent->deactivateAndDeleteServant();
            m_genericAgent = 0;
        }
        if( m_alarmAgentImpl != NULL )
        {
            // This *should* already be done, assuming that agentTerminate was called
            m_alarmAgentImpl->deactivateAndDeleteServant();
            m_alarmAgentImpl = 0;
        }


        FUNCTION_EXIT;
    }


    //
    // agentTerminate
    //
    void AlarmAgent::agentTerminate()
    {
		FUNCTION_ENTRY("agentTerminate");

        if( m_alarmAgentImpl != NULL )
        {
            m_alarmAgentImpl->deactivateAndDeleteServant();
            m_alarmAgentImpl = 0;
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

		// Move code here from destructor to avoid crash
		// as corba utilities will be shutdown on delete of the 
		// generic agent.
		TA_Base_Bus::ScadaUtilities::removeInstance();
		TA_Base_App::RuleRepository::releaseInstance();
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "TA_Base_Bus::ScadaUtilities::removeInstance()" );

        FUNCTION_EXIT;
    }


    //
    // createEntity
    //
    IEntity* AlarmAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
    {
        return NULL;
    }


    //
    // agentSetMonitor
    //
    void AlarmAgent::agentSetMonitor()
    {
		FUNCTION_ENTRY("agentSetMonitor");

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Requesting from External to switch agent to monitor mode" );

        //limin++--, failover
        AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
        thrd.add( &AlarmAgent::unsubscribeToMessages );
        thrd.add( &AlarmAgent::registerForMonitorMessages);
        thrd.start();

		// when switch from control to monitor
        //MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        //registerForMonitorMessages();
		// nothing else to take care as all alarms and data should be updated
        //limin++--, failover
		if( true == m_genericAgent->getAgentInfo().FailOverFromStationToOcc )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failover from station to OCC" );
			loadAlarmAgentData();
		}

		FUNCTION_EXIT;
    }


    //
    // limin, failover
    //
    void AlarmAgent::unsubscribeToMessages()
    {
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }


    //
    // agentSetControl
    //
    void AlarmAgent::agentSetControl()
    {
		FUNCTION_ENTRY("agentSetControl");

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Requesting from External to switch agent to control mode" );

        //limin++--, failover
        AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
        thrd.add( &AlarmAgent::unsubscribeToMessages );
        thrd.add( &AlarmAgent::registerForControlMessages);
        thrd.start();

		// when switch from monitor to control
        //MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        //registerForControlMessages();
        //limin++--, failover
		if( true == m_genericAgent->getAgentInfo().FailOverFromStationToOcc )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failover from station to OCC" );
			loadAlarmAgentData();
		}
		else
		{
			//when switched from monitor to control, process MMS Recevied in the cache which was not processed.
			AlarmMMSSubmitter::getInstance()->startProcessingMMS();
		}
		FUNCTION_EXIT;
    }

	void AlarmAgent::agentSetStandby()
	{
		FUNCTION_ENTRY("agentSetControl");

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Requesting from External to switch agent to Standby mode" );

		//limin++--, failover
		AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
		thrd.add( &AlarmAgent::unsubscribeToMessages );
		thrd.start();

		cleanAlarmsAndRules();
		
		FUNCTION_EXIT;
	}

    //
    // notifyGroupOffline
    //
    void AlarmAgent::notifyGroupOffline( const std::string& group )
    {}


    //
    // notifyGroupOnline
    //
    void AlarmAgent::notifyGroupOnline( const std::string& group )
    {}


    //
    // registerForControlMessages
    //
    void AlarmAgent::registerForControlMessages()
    {
		FUNCTION_ENTRY("registerForControlMessages");

        MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::AlarmStateUpdate::AlarmAgentStateRequest,
            this,
            getAgentEntityName());

		FUNCTION_EXIT;
    }


    //
    // registerForMonitorMessages
    //
    void AlarmAgent::registerForMonitorMessages()
    {
		FUNCTION_ENTRY("registerForMonitorMessages");

        MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::AlarmStateUpdate::AlarmStateUpdate,
            this,
            getAgentEntityName());

        MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::AlarmStateUpdate::QueueItemRemoved,
            this,
            getAgentEntityName());

		// TD 14082 remove cache and queue updates via messaging
		// Use direct corba call
        //MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        //   TA_Base_Core::AlarmStateUpdate::AlarmAgentBatchCacheUpdate,
        //    this,
        //    RunParams::getInstance().get(RPARAM_ENTITYNAME));

        //MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        //    TA_Base_Core::AlarmStateUpdate::AlarmAgentBatchQueueUpdate,
        //    this,
        //    RunParams::getInstance().get(RPARAM_ENTITYNAME));

		FUNCTION_EXIT;
    }


    //
    // registerForStateUpdates
    //
    void AlarmAgent::registerForStateUpdates()
    {
		FUNCTION_ENTRY ("registerForStateUpdates");

        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
            //limin++--, failover
            AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
            thrd.add( &AlarmAgent::registerForControlMessages);
            thrd.start();

            //registerForControlMessages();
            //limin++--, failover
        }
        else if ( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
        {
            //limin++--, failover
            AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
            thrd.add( &AlarmAgent::registerForMonitorMessages);
            thrd.start();

            //registerForMonitorMessages();
            //limin++--, failover
        }
		else if ( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_STANDBY) )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Agent is Starting on Standby Mode.");
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
    void AlarmAgent::receiveSpecialisedMessage(const StateUpdateMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");

        if( 0 == std::string(message.messageTypeKey).compare(TA_Base_Core::AlarmStateUpdate::AlarmAgentStateRequest.getTypeKey()) )
        {
            receiveRequestForStateUpdateMsg();

            FUNCTION_EXIT;
            return;
        }

		//Fix to C955PT_098 issue
		//Changed to acquire lock before m_updatesQueue.Empty()
		{
			TA_THREADGUARD(m_updateQueueLock);
			if (!AlarmCache::getInstance()->hasDataLoaded() || m_updatesQueue.empty() == false)
			{
				// TD 14082 the agent is busy acquiring cache and queue updates from control agent
				// or the FIFO queue is not empty.  Queue the updates info for later action


				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "receiveSpecialisedMessage - message received. But agent busy acquiring, push into queue");
				// make a copy of the update message and delete after use
				StateUpdateMessageCorbaDef* updateMessage = new StateUpdateMessageCorbaDef();
				updateMessage->messageTypeKey = message.messageTypeKey;
				updateMessage->stateUpdateInfo = message.stateUpdateInfo;
				updateMessage->createTime = message.createTime;
				updateMessage->assocEntityKey = message.assocEntityKey;

				m_updatesQueue.push(updateMessage);
				
				FUNCTION_EXIT;
				return;
			}
		}
		//else
		//{
			// go ahead and process the message
			processMessageUpdate(message);
		//}

		/* TD 14082 Remove batch cache and queue update from control agent and use direct corba call
        if( 0 == TA_Base_Core::AlarmStateUpdate::AlarmAgentBatchCacheUpdate.getTypeKey().compare(message.messageTypeKey) )
        {
            AlarmSequence* cache;

            if( (message.stateUpdateInfo >>= cache) != 0 )
            {
                receiveBatchCacheStateUpdateMsg(*cache);
            }

            LOG ( SourceInfo, DebugUtil::FunctionExit, "receiveSpecialisedMessage - received batch cache update");

            return;
        }

        if( 0 == TA_Base_Core::AlarmStateUpdate::AlarmAgentBatchQueueUpdate.getTypeKey().compare(message.messageTypeKey) )
        {
            AlarmAgentQueueItemSequence* seq;

            if( (message.stateUpdateInfo >>= seq) != 0 )
            {
                receiveBatchQueueStateUpdateMsg(*seq);
            }

            LOG ( SourceInfo, DebugUtil::FunctionExit, "receiveSpecialisedMessage - received batch queue update");

            return;
        } */

        FUNCTION_EXIT;
    }


    //
    // processOnlineUpdate
    //
    void AlarmAgent::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
    {
    }


    //
    // run
    //
    void AlarmAgent::run()
    {
        FUNCTION_ENTRY("run");
		bool bSkipAlarmLoading = false;
		if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_STANDBY))
		{
			bSkipAlarmLoading = true;
		}

		loadAlarmAgentData(bSkipAlarmLoading);
        m_genericAgent->run();

    }

	void AlarmAgent::cleanAlarmsAndRules()
	{
		FUNCTION_ENTRY("cleanAlarmsAndRules");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Clean all alarms and rules" );

		m_alarmAgentImpl->unloadAlarmsAndRules();

		FUNCTION_EXIT;
	}

	void AlarmAgent::processMessageUpdate(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("processQueueUpdates");

        if( 0 == TA_Base_Core::AlarmStateUpdate::AlarmStateUpdate.getTypeKey().compare(message.messageTypeKey) )
        {
            AlarmDetailCorbaDef* alarm;
            AlarmUpdateCorbaDef* update;

            if( (message.stateUpdateInfo >>= alarm) != 0 )
            {
				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "receiveSpecialisedMessage - received AlarmDetailCorbaDef state update");
				receiveNewAlarmStateUpdateMsg(*alarm);
            }
            else
            {
                if( (message.stateUpdateInfo >>= update) != 0 )
                {
					LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "receiveSpecialisedMessage - received AlarmUpdateCorbaDef state update");
					receiveAlarmUpdateStateUpdateMsg(*update);
                }
            }

        }
        else if( 0 == TA_Base_Core::AlarmStateUpdate::QueueItemRemoved.getTypeKey().compare(message.messageTypeKey) )
        {
            AlarmAgentQueueItemSequence* item;

            if( (message.stateUpdateInfo >>= item) != 0 )
            {
				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "receiveSpecialisedMessage - received AlarmAgentQueueItemSequence item removed update");
				receiveQueueUpdateStateUpdateMsg(*item);
            }
		}
		else
		{
			// something wrong here. an unknown update type rec'd WARNING
			LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "receiveSpecialisedMessage - received Unknown item in the UpdateQueue. Removed update without any action");
		}

		FUNCTION_EXIT;
	}
 
    //
    // receiveNewAlarmStateUpdateMsg
    //
    void AlarmAgent::receiveNewAlarmStateUpdateMsg(const TA_Base_Core::AlarmDetailCorbaDef& p_alarm)
    {
		FUNCTION_ENTRY("receiveNewAlarmStateUpdateMsg");

        try
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "receiveNewAlarmStateUpdateMsg");
			TA_Base_Core::AlarmDetailCorbaDef* alarm =new TA_Base_Core::AlarmDetailCorbaDef(p_alarm);
			std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef> apAlarm= std::auto_ptr<TA_Base_Core::AlarmDetailCorbaDef>(alarm);
            AlarmCache::getInstance()->insert(*alarm, true);
			if(alarm->state == TA_Base_Core::AlarmOpen)
			{
				// to process MMS Rule 
				RuleRepository::getInstance()->processMMSAlarm(*alarm);
			}
            AlarmCache::getInstance()->printCacheSize();
        }
        catch(...)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown",
                 "Received a new alarm via a state update and failed to add it to the cache");
        }

		FUNCTION_EXIT;
    }


    //
    // receiveAlarmUpdateStateUpdateMsg
    //
    void AlarmAgent::receiveAlarmUpdateStateUpdateMsg(const TA_Base_Core::AlarmUpdateCorbaDef& p_update)
    {
        if( p_update.alarmHasBeenAcknowledged )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"receiveAlarmUpdateStateUpdateMsg. Update alarmHasBeenAcknowledged for alarm ID: %s from session: %s", (std::string(p_update.alarmID)).c_str(), (std::string(p_update.session)).c_str() );

            AlarmCache::getInstance()->updateAlarmAcknowledgeStateNonReentrant(std::string(p_update.alarmID), 
                                                                   std::string(p_update.alarmAcknowledgedBy),
                                                                   std::string(p_update.session),
																   !AlarmCache::getInstance()->isSystemOperator(std::string(p_update.session)));
        }
        else if( p_update.alarmHasBeenClosed )
		{
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"receiveAlarmUpdateStateUpdateMsg. Update alarmHasBeenClosed for alarm ID: %s from session: %s", (std::string(p_update.alarmID)).c_str(), (std::string(p_update.session)).c_str() );

			AlarmCache::getInstance()->closeAlarm(std::string(p_update.alarmID), 
				!AlarmCache::getInstance()->isSystemOperator(std::string(p_update.session)), 
													std::string(p_update.session), 
													std::string(p_update.closeValue), 
													p_update.closeTime);
        }
        else if ( p_update.commentsWereAdded )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"receiveAlarmUpdateStateUpdateMsg. Update commentsWereAdded for alarm ID: %s from session: %s", (std::string(p_update.alarmID)).c_str(), (std::string(p_update.session)).c_str() );

				AlarmCache::getInstance()->updateAlarmComment(std::string(p_update.alarmID), 
                                                          std::string(p_update.alarmComments), 
                                                          std::string(p_update.session));
        }
		else if ( p_update.decisionSupportStateHasChanged )
		{
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"receiveAlarmUpdateStateUpdateMsg. Update decisionSupportStateHasChanged for alarm ID: %s from session: %s", (std::string(p_update.alarmID)).c_str(), (std::string(p_update.session)).c_str() );

				AlarmCache::getInstance()->updateDecisionSupportState(std::string(p_update.alarmID), 
                                                          p_update.decisionSupportState, 
                                                          std::string(p_update.session));
		}
		else if ( p_update.mmsStateHasChanged )
		{
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"receiveAlarmUpdateStateUpdateMsg. Update mmsStateHasChanged for alarm ID: %s from session: %s", (std::string(p_update.alarmID)).c_str(), (std::string(p_update.session)).c_str() );

				AlarmCache::getInstance()->updateMMSState(std::string(p_update.alarmID), 
                                                          p_update.mmsState );
                                             
		}
    }


    //
    // receiveQueueUpdateStateUpdateMsg
    //
    void AlarmAgent::receiveQueueUpdateStateUpdateMsg(const TA_Base_Core::AlarmAgentQueueItemSequence& p_update)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "receiveQueueUpdateStateUpdateMsg");

        unsigned int length = p_update.length();

        for( unsigned int i = 0; i < length; i++ )
        {
            AlarmCache::getInstance()->removeQueueItem(p_update[i]);
        }
    }


	/* TD 14082 remove message batch updating from control agent
    //
    // receiveBatchCacheStateUpdateMsg
    //
    void AlarmAgent::receiveBatchCacheStateUpdateMsg(const TA_Base_Core::AlarmSequence& p_alarms)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "receiveBatchCacheStateUpdateMsg");

        AlarmCache::getInstance()->receiveAlarmCacheBatchUpdate(p_alarms);

        AlarmCache::getInstance()->printCacheSize();
    }


    //
    // receiveBatchQueueStateUpdateMsg
    //
    void AlarmAgent::receiveBatchQueueStateUpdateMsg(const TA_Base_Core::AlarmAgentQueueItemSequence& p_queue)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "receiveBatchQueueStateUpdateMsg");

        AlarmCache::getInstance()->receiveAlarmQueueBatchUpdate(p_queue);
    } */


    //
    // receiveRequestForStateUpdateMsg
    //
    void AlarmAgent::receiveRequestForStateUpdateMsg()
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "receiveRequestForStateUpdateMsg");

        AlarmCache::getInstance()->sendBatchStateUpdates();
    }


    //
    // setupRunParams
    //
    void AlarmAgent::setupRunParams()
    {

        try
        {
			TA_Base_Core::AlarmAgentEntityDataPtr entityData = 
				boost::dynamic_pointer_cast<AlarmAgentEntityData>(m_genericAgent->getAgentEntityConfiguration());
            unsigned long key = entityData->getKey();
            unsigned long typeKey = entityData->getTypeKey();
            unsigned long subsystem = entityData->getSubsystem();
            unsigned long numberToTest = entityData->getNumberOfAlarmsToTest();
            unsigned long operatorKey = entityData->getUserId();
            unsigned long workstationId = entityData->getWorkStationId();
            unsigned long profileId = entityData->getProfileId();
			std::string assetName = entityData->getAssetName();
			//TD16491++
			std::string strMmsSubmitterType = entityData->getMmsSubmitterType();
			bool bUseSourceTime = entityData->getUseSourceTime();
			//++TD16491

            char entity[50];            
            char type[50];
            char subsys[50];
            char numToTest[50];
            char opKey[50];
            char workstation[50];
            char profile[50];
            sprintf(entity,"%lu",key);
            sprintf(type,"%lu",typeKey);
            sprintf(subsys,"%lu",subsystem);
            sprintf(numToTest,"%lu",numberToTest);
            sprintf(opKey, "%lu", operatorKey);
            sprintf(workstation, "%lu", workstationId);
            sprintf(profile, "%lu", profileId);

            unsigned long location = atol( (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str() );

            TA_Base_Core::ILocation* locationInfo = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(location);
            std::string locationName = locationInfo->getName();
			delete locationInfo;
			locationInfo = NULL;

            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::OPERATOR_KEY.c_str(), opKey);
            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::PROFILE_ID.c_str(), profile);
            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::WORKSTATION_ID.c_str(), workstation);
            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::PASSWORD.c_str(), entityData->getPassword().c_str());

            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::ENTITY_KEY.c_str(), entity);
            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::ENTITY_TYPE_KEY.c_str(), type);
            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::SUBSYSTEM_KEY.c_str(), subsys);
            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::LOCATION_NAME.c_str(), locationName.c_str());
            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::NUMBER_OF_ALARMS_TO_TEST.c_str(), numToTest);

            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::ASSET_NAME.c_str(), assetName.c_str());

			if(!strMmsSubmitterType.empty())
			{
				//TD16491++
				TA_Base_Core::RunParams::getInstance().set(TA_Base_Core::AlarmRuleAccessFactory::RPARAM_MMS_SUBMITTER_TYPE.c_str(), strMmsSubmitterType.c_str() );
			}
			

			if(bUseSourceTime==true)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting use source time to true" );
				TA_Base_Core::RunParams::getInstance().set(TA_Base_Bus::AlarmConstants::RPARAM_USESOURCETIME.c_str(), "true" );
			}
			
			//++TD16491

            if( entityData->isFederationEnabled() )
            {
                RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::ENABLE_FEDERATION.c_str(), "true");
            }
            else
            {
                RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::ENABLE_FEDERATION.c_str(), "false");
            }

            TA_Base_Core::IOperator* op = TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(operatorKey, false);
            std::string name = op->getName();
			delete op;
			op = NULL;
            RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::OPERATOR_NAME.c_str(), name.c_str());

            //if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
            {
                getSession();
            }
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
            TA_THROW(TA_Base_Core::GenericAgentException("Unknown exception raised while initialising Alarm Agent"));
        }
    }


    //
    // getSession
    //
    void AlarmAgent::getSession()
    {
		FUNCTION_ENTRY("getSession");

        bool resolvedAuthenticationAgent = false;

        const unsigned long getSessionRetryPeriod = 5000; // 5 seconds

        while(!resolvedAuthenticationAgent)
        {
            try
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Requesting session from Authentication Agent" );

                AuthenticationLibrary authLib;

                std::string session = authLib.requestSession(
                        atol(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::OPERATOR_KEY.c_str()).c_str()), 
                        atol(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::PROFILE_ID.c_str()).c_str()),
                        atol(std::string(RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str()), 
                        atol(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::WORKSTATION_ID.c_str()).c_str()),
                        std::string(RunParams::getInstance().get(TA_Base_App::TA_AlarmAgent::PASSWORD.c_str()).c_str()));

                RunParams::getInstance().set(TA_Base_App::TA_AlarmAgent::SESSION.c_str(), session.c_str());

                resolvedAuthenticationAgent = true;
            }
            catch(TA_Base_Core::AuthenticationAgentException& ae)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Failed to request session from Authentication Agent" );

                LOG( SourceInfo, DebugUtil::ExceptionCatch,"AuthenticationAgentException", 
                     ae.what() );

                resolvedAuthenticationAgent = false;

                TA_Base_Core::Thread::sleep(getSessionRetryPeriod);
            }
            catch(TA_Base_Core::AuthenticationSecurityException& ase)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Failed to request session from Authentication Agent" );

                LOG( SourceInfo, DebugUtil::ExceptionCatch,"AuthenticationSecurityException", 
                     ase.what() );

                resolvedAuthenticationAgent = false;

                // TODO: if authentication failed, the agent should terminate

                TA_Base_Core::Thread::sleep(getSessionRetryPeriod);
            }
            catch( ... )
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "An unexpected error occured while requesting session from Authentication Agent" );
                resolvedAuthenticationAgent = false;

                TA_Base_Core::Thread::sleep(getSessionRetryPeriod);
            }
        }

		FUNCTION_EXIT;
    }

	void AlarmAgent::loadAlarmAgentData(bool bSkipAlarmLoading/*=false*/)
	{
		FUNCTION_ENTRY("AlarmAgent::loadAlarmAgentData");

		m_alarmAgentImpl->loadAlarmsAndRules(bSkipAlarmLoading);

		LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "run - AlarmAgent thread starts with updatesQueue of size: %d and hasLoadedData state: %d", m_updatesQueue.size(), AlarmCache::getInstance()->hasDataLoaded() );
		// TD 14082 Clear updates in the queue (if any) after acquiring
		// keep the thread alive if the updatesQueue is not empty
		// keep thread alive if the alarmcache is still acquiring alarms
		{
			TA_THREADGUARD(m_updateQueueLock);

			while (m_updatesQueue.empty() == false || !(AlarmCache::getInstance()->hasDataLoaded()) )
			{
				LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "run - AlarmAgent thread still running with updatesQueue of size: %d", m_updatesQueue.size() );
				// after cache has loaded all data, process the updates
				if ( AlarmCache::getInstance()->hasDataLoaded() && m_updatesQueue.empty() == false)
				{
					// Push the updates one by one in FIFO manner
					LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "run - poping front of updates queue of size: %d", m_updatesQueue.size() );
					// Process the messages queued up so far
					const TA_Base_Core::StateUpdateMessageCorbaDef* queueItem = m_updatesQueue.front();
					if (queueItem!=NULL) 
					{
						processMessageUpdate(*queueItem);
						delete queueItem;
					}
					m_updatesQueue.pop();
				}
				else
				{
					//sleep for a while and process it later
					TA_Base_Core::Thread::sleep(50); // millisec
				}
			}
		}

		FUNCTION_EXIT;
	}


};
