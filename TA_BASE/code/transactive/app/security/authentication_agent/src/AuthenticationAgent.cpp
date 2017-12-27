/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/AuthenticationAgent.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IGenericAgentUser interface. It is the "main" agent class.
 */

#if defined(_MSC_VER)
//#pragma warning(disable:4680)
#endif // defined(_MSC_VER)

#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"
#include "SessionRegister.h"
#include "AuthenticationAgent.h"
#include "SessionQueue.h"
#include "SessionQueueProcessor.h"

#include "bus/security/authentication_library/src/SessionInfo.h"

#include "bus/generic_agent/src/GenericAgent.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/AuthenticationAgentStateUpdate_MessageTypes.h"
#include "core/message/types/AuthenticationAgentStateUpdateBroadcast_MessageTypes.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/AuthenticationAgentEntityData.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/threads/src/ThreadMemberFunction.h"

using TA_Base_Bus::Authentication::BASE_OPERATOR;
using TA_Base_Bus::Authentication::BASE_PROFILE;
using TA_Base_Bus::GenericAgent;

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::ISession;
using TA_Base_Core::SessionAccessFactory;
using TA_Base_Bus::SessionInfo;

const int DEFAULT_FREQUENCY = 30;    // Frequency the session register is checked in seconds
const int DEFAULT_TIMEOUT = 1800;      // Time out period in seconds

namespace TA_Base_App
{

    typedef TA_Base_Core::ThreadedMemberFunction< TA_Base_App::AuthenticationAgent > AgentThreadedMemberFunction;

    AuthenticationAgent::AuthenticationAgent(int argc, char* argv[]) :
        m_genericAgent(NULL),
		m_authenticationAgentImpl(NULL),
		m_authentication(NULL),
		m_isInControl(false),
		m_queueStateUpdater(NULL),
		m_sessionQueue(NULL),
		m_sessionQueueProcessor(NULL),
        m_entityKey(ULONG_MAX),
		m_localStateSender(NULL),
		m_bulkUpdateComplete(0)
	{
        try
        {
            // Construct the various objects controlled by this class, with Generic Agent constructed last
                       
            // Once generic agent is constructed, the UpdateMessageReceiver is running - that is why Monitored Queue
            // has to be constructed first.
            m_genericAgent = new GenericAgent(argc,argv,this,NULL,AGENT_USER_TYPE_NORMAL);

            // Retrieve the agent entity configuration and store. This needs to be called before the Authentication
            // object is created as it uses the agent configuration parameter values.
            setUpAgentEntityConfiguration();

			m_localStateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
				TA_Base_Core::AuthenticationAgentStateUpdate::Context);

            // Set the flag for whether this agent was started in monitor or control mode
            m_isInControl = (m_genericAgent->getOperationMode() == TA_Base_Core::Control);

	        // Get the entity key of this agent
	        m_entityKey = m_genericAgent->getAgentEntityConfiguration()->getKey();

			m_queueStateUpdater     = new QueueStateUpdater(m_entityKey);
			m_sessionQueue          = new SessionQueue(*m_queueStateUpdater, m_isInControl );
			m_sessionQueueProcessor = new SessionQueueProcessor( *m_sessionQueue, *m_queueStateUpdater, m_isInControl );

            // And the Authentication object (responsible for authentication)
            m_authentication = new Authentication(*m_sessionQueue, *m_sessionQueueProcessor, m_isInControl, m_entityKey);
        }
        catch(...)
        {
            // An exception was thrown while creating the objects, so clean them up, then throw the exception on 
            // (so the calling application knows the construciton didn't work)
			doCleanUp( false );
			throw;
        }
    }

	void AuthenticationAgent::doCleanUp( bool shouldStopThread )
	{
		if (m_localStateSender != NULL)
        {
            delete m_localStateSender;
            m_localStateSender = NULL;
        }
        if (m_authentication != NULL)
        {
            delete m_authentication;
            m_authentication = NULL;
        }
		if (m_sessionQueueProcessor != NULL)
        {
			if ( shouldStopThread )
			{
				m_sessionQueueProcessor->terminateAndWait();
			}
            delete m_sessionQueueProcessor;
            m_sessionQueueProcessor = NULL;
        }
        if (m_sessionQueue != NULL)
        {
            delete m_sessionQueue;
            m_sessionQueue = NULL;
        }
        if (m_queueStateUpdater != NULL)
        {
            delete m_queueStateUpdater;
            m_queueStateUpdater = NULL;
        }
        if (m_authenticationAgentImpl != NULL)
        {
            m_authenticationAgentImpl->deactivateAndDeleteServant();
            m_authenticationAgentImpl = NULL;
        }
        if (m_genericAgent != NULL)
        {
            m_genericAgent->deactivateAndDeleteServant();
            m_genericAgent = NULL;
        }        
	}

    AuthenticationAgent::~AuthenticationAgent()
    {
        try
        {
			doCleanUp( true );
        }
        catch (...)
        {
            // This catch is simply to ensure no exceptions escape the destructor.
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown",
                "An exception was caught while destructing Generic Agent.");
        }
    }

    TA_Base_Bus::IEntity* AuthenticationAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
    {
        
        // The agent's entity is not actually used
        return NULL;
    }

    void AuthenticationAgent::agentSetMonitor()
    {
        FUNCTION_ENTRY("agentSetMonitor");

        // Change the flag boolean
        m_isInControl = false;
        
		{
			// Take a thread lock before changing the queue's state
			ThreadGuard guard(m_updateLock);

			// Put the queue into monitor mode
			m_sessionQueue->setMonitor();
			m_sessionQueueProcessor->setMonitor();

            //limin++--, failover
            AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
            thrd.add( &AuthenticationAgent::unsubscribeToMessages );
            thrd.add( &AuthenticationAgent::registMonitorMessage );
            thrd.start();

			// Remove any current message subscriptions
			//MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
			// Re-register
			//registMonitorMessage();
            //limin++--, failover
        
			m_authentication->setMonitor();
		}
		
		CORBA::Any nothing; // An empty any to place in the state update request.

		m_localStateSender->sendStateUpdateMessage( TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentStateUpdateRequest,  
													m_entityKey, 
													RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(), 
													nothing);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "send a StateUpdateRequest to control agent.");

		//m_bulkUpdateComplete.wait();//limin, change operation mode issue

        FUNCTION_EXIT;
    }

    void AuthenticationAgent::agentSetControl()
    {
        FUNCTION_ENTRY("agentSetControl");

        // Change the flag boolean
        m_isInControl = true;

        //limin++--, failover
        AgentThreadedMemberFunction& thrd = AgentThreadedMemberFunction::alloc( *this );
        thrd.add( &AuthenticationAgent::unsubscribeToMessages );
        thrd.add( &AuthenticationAgent::registControlMessage );
        thrd.start();
        // Remove any current message subscriptions
        //MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

        // Re-register
        //registControlMessage();
        //limin++--, failover

        // Take the thread lock before changing the queue's state
        ThreadGuard guard(m_updateLock);
        
        // TD14332 before agent change to control, update all sessionActiveTime to current server time
        // in order for deadSessionChecker not mistakenly remove any active sessions.
        m_authentication->updateAllSessionsActiveTime();

		// LY++ for CL-20784 
		m_authentication->getSessionRegister().updateGhostSessionAliveTime(
			strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSION_TIMEOUT_PERIOD).c_str(), NULL, 10) );

        // Put the queue into control mode
		m_sessionQueue->setControl();
        m_sessionQueueProcessor->setControl();

        m_authentication->setControl();

        LOG (SourceInfo, DebugUtil::FunctionExit, "agentSetControl");
    }

    //limin, failover
    void AuthenticationAgent::unsubscribeToMessages()
    {
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }
    void AuthenticationAgent::registerForStateUpdates()
    {
    }

	void AuthenticationAgent::registControlMessage()
	{
		MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
			TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentStateUpdateRequest,
			this,
			getAgentEntityName());
		LOG0(SourceInfo, DebugUtil::DebugInfo, "register control message");
	}

	void AuthenticationAgent::registMonitorMessage()
	{
		MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
			TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentStateUpdate,
			this,
			getAgentEntityName());
		
		MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
			TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentBatchStateUpdate,
			this,
			getAgentEntityName());
		
		MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
			TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentSessionTimeStateUpdate,
			this,
			getAgentEntityName());
			LOG0(SourceInfo, DebugUtil::DebugInfo, "register monitor message");
	}

    void AuthenticationAgent::receiveSpecialisedMessage(const TA_Base_Core::
        StateUpdateMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");
        if( m_isInControl )
        {   // We're in control mode...so should only get requests for state updates
            if( 0 != std::string(message.messageTypeKey).compare(TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentStateUpdateRequest.getTypeKey()) )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn,
                     "Received an unexpected message ( in Control Mode). Type Key = %s", std::string(message.messageTypeKey).c_str());
                return;
            }
            else
            {
                sendBatchStateUpdate();
            }
        }
        else  //in monitor state
        {
			TA_ASSERT( ! m_isInControl, "authentication agent ran in monitor state now");

            // In monitor mode there are two valid possibilities - a batch state update, or an ordinary state update.
			recieveStateUpdateMessage( message );
			recieveBatchStateUpdateMessage( message );
			recieveSessionTimeStateUpdateMessage( message );
		}
 
        FUNCTION_EXIT;
    }

    void AuthenticationAgent::processOnlineUpdate(const TA_Base_Core::
        ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY( "processOnlineUpdate" );

        FUNCTION_EXIT;
    }

    void AuthenticationAgent::run()
    {
        FUNCTION_ENTRY("run");

		//no need to regist here, because it is already called when created Generic agent
        //registerForStateUpdates();

        // Start the monitoring the queue
        m_sessionQueueProcessor->start();

	    m_authentication->startObjects();

        // Finally, construct the Authentication CORBA object. Once this is constructed, the agent is "open for business".
        m_authenticationAgentImpl = new AuthenticationAgentImpl(*m_authentication, this);

		if( RunParams::getInstance().get(RPARAM_OPERATIONMODE) == RPARAM_CONTROL )
        {
            agentSetControl();
        }

        if( RunParams::getInstance().get(RPARAM_OPERATIONMODE) == RPARAM_MONITOR )
        {
            agentSetMonitor();
        }
      
        m_genericAgent->run();

        FUNCTION_EXIT;
    }

    void AuthenticationAgent::agentTerminate()
    {
        FUNCTION_ENTRY( "agentTerminate");

        // Shutdown the update message receiver so no more queue updates come in.
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

        // Take a thread-lock on the queue. This is to prevent any last incoming message
        // attempting to access a half-dead queue
        ThreadGuard guard(m_updateLock);

        // Shutdown the queue processing
        m_sessionQueueProcessor->terminateAndWait();

        FUNCTION_EXIT;
    }


    void AuthenticationAgent::processStateUpdate(TA_Base_Bus::SessionInfoCorbaDef* session)
    {
        FUNCTION_ENTRY("processStateUpdate");
        // State updates can be of two types - sessionbegin and sessionend. In the context
        // of state updates, a sessionbegin means that it is to be added to the queue, while
        // as sessionend is a session that has been sucessfully written by the control agent, 
        // and can therefore be removed from the queue.

        // So, test what type we have here.
        if (session->updateType == TA_Base_Bus::TA_Security::SessionStart)
        {
            // Add it to the queue - so create an ISession object
            ISession* theSession = SessionAccessFactory::getInstance().createSession(
                CORBA::string_dup(session->SessionId),session->UserId,session->WorkstationId,session->ProfileId,
				session->LocationId, m_entityKey,session->isDisplayOnly);

            // Now place the session on the queue (take a thread guard first).
            ThreadGuard guard(m_updateLock);
            m_sessionQueue->push(theSession);
        }
        else if (session->updateType == TA_Base_Bus::TA_Security::SessionEnd)
        {
            // Remove it from the queue - only need the session ID for this.

            // Need to take a thread guard to prevent mulitple access to the queue
            ThreadGuard guard(m_updateLock);
            m_sessionQueue->removeQueueItem(CORBA::string_dup(session->SessionId));
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "An update message of unknown type was received");
        }
        FUNCTION_EXIT;
    }

    
    void AuthenticationAgent::processSessionTimeStateUpdate(TA_Base_Bus::SessionInfoCorbaDef* session)
    {
        FUNCTION_ENTRY("processSessionTimeStateUpdate");

        TA_Base_Bus::SessionInfo sessionInfo;

        sessionInfo.UserId = session->UserId;

        sessionInfo.SessionId = session->SessionId;
        sessionInfo.WorkstationId = session->WorkstationId;
        sessionInfo.LocationId = session->LocationId;

        sessionInfo.OperatorId.push_back( session->UserId );
        sessionInfo.ProfileId.push_back( session->ProfileId );
		sessionInfo.isDisplayOnly = session->isDisplayOnly;
        // So, test what type we have here.
        if (session->updateType == TA_Base_Bus::TA_Security::SessionStart)
        {
            unsigned long i;
            for ( i=0; i<session->additionalOperators.length(); i++ )
            {
                sessionInfo.OperatorId.push_back( session->additionalOperators[i] );
            }
            for ( i=0; i<session->additionalProfiles.length(); i++ )
            {
                sessionInfo.ProfileId.push_back( session->additionalProfiles[i] );
            }
            m_authentication->updateSessionTime(sessionInfo, false);
        }
        else if (session->updateType == TA_Base_Bus::TA_Security::SessionEnd)
        {
            m_authentication->updateSessionTime(sessionInfo, true);
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "An update message of unknown type was received");
        }

        FUNCTION_EXIT;
    }

    void AuthenticationAgent::fillOutDataBaseQueue(TA_Base_Bus::AuthentcationBatchUpdateCorbaDef* batchUpdate, 
		     std::vector<ISession*> &sessions )
	{
        // Populate the database sequence first. As sequences are primative, need to set their length before using them.
        batchUpdate->DatabaseQueue.length(sessions.size());

        // Now loop through the vector and add items to the sequence for each one.
        for (size_t i = 0;i < sessions.size();i++)
        {
            batchUpdate->DatabaseQueue[i].SessionId = CORBA::string_dup(sessions[i]->getSessionId().c_str());
            batchUpdate->DatabaseQueue[i].UserId = sessions[i]->getOperatorKey();
            batchUpdate->DatabaseQueue[i].ProfileId = sessions[i]->getProfileKey();
            batchUpdate->DatabaseQueue[i].WorkstationId = sessions[i]->getConsoleKey();
            batchUpdate->DatabaseQueue[i].LocationId = sessions[i]->getLocationKey();
			batchUpdate->DatabaseQueue[i].isDisplayOnly = sessions[i]->getDisplayMode();
            batchUpdate->DatabaseQueue[i].additionalOperators.length(0);
            batchUpdate->DatabaseQueue[i].additionalProfiles.length(0);
        }
	}

	void AuthenticationAgent::fillOutSessionRegister(TA_Base_Bus::AuthentcationBatchUpdateCorbaDef* batchUpdate, 
		     std::vector<ISession*> &sessions )
	{
        // Grab the session register
        std::vector<SessionInfo> reg;
		m_authentication->getSessionRegister().getRegister(reg);

        // And bundle it into the other sequence - setting the length first
        batchUpdate->SessionRegister.length(reg.size());

        // Now loop through the vector and add items to the sequence for each one.
        for (size_t i = 0;i < reg.size();i++)
        {
            // Ensure there is at least one profile for this session
            TA_ASSERT(reg[i].ProfileId.size() > 0,"The ProfileId vector was empty");

            batchUpdate->SessionRegister[i].SessionId = CORBA::string_dup(reg[i].SessionId.c_str());
            batchUpdate->SessionRegister[i].UserId = reg[i].OperatorId[BASE_OPERATOR];
            batchUpdate->SessionRegister[i].ProfileId = reg[i].ProfileId[BASE_PROFILE];
            batchUpdate->SessionRegister[i].WorkstationId = reg[i].WorkstationId;
            batchUpdate->SessionRegister[i].LocationId = reg[i].LocationId;
			batchUpdate->SessionRegister[i].isDisplayOnly = reg[i].isDisplayOnly;
            unsigned int j;

            batchUpdate->SessionRegister[i].additionalOperators.length( reg[i].OperatorId.size()-1 );
            for ( j=1; j<reg[i].OperatorId.size(); j++ )
            {
                batchUpdate->SessionRegister[i].additionalOperators[j-1] = reg[i].OperatorId[j];
            }
            batchUpdate->SessionRegister[i].additionalProfiles.length( reg[i].ProfileId.size()-1 );
            for ( j=1; j<reg[i].ProfileId.size(); j++ )
            {
                batchUpdate->SessionRegister[i].additionalProfiles[j-1] = reg[i].ProfileId[j];
            }
        }
	}
    
    void AuthenticationAgent::sendBatchStateUpdate()
    {
        FUNCTION_ENTRY("sendBatchStateUpdate");
        // Create an any into which the sequence can be placed
        CORBA::Any stateUpdate;

        // Grab the queue as it stands
        std::vector<ISession*> sessions = m_sessionQueue->getCurrentQueue();

        // Create the batch update. It contains two sequences.
        TA_Base_Bus::AuthentcationBatchUpdateCorbaDef* batchUpdate = new TA_Base_Bus::AuthentcationBatchUpdateCorbaDef();

		fillOutDataBaseQueue(batchUpdate, sessions);
		fillOutSessionRegister(batchUpdate, sessions);
        
		// Place the sequence into the state update
        stateUpdate <<= *(batchUpdate);

        //TD15069 azenitha++
        if( m_isInControl )
        {
			// Send the message
			m_localStateSender->sendStateUpdateMessage(
						TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentBatchStateUpdate,
						m_genericAgent->getAgentEntityConfiguration()->getKey(),
						getAgentEntityName(),
						stateUpdate);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
                "sending message: AuthenticationAgentBatchStateUpdate. system is in Control Mode");
        }
        else
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
                "sending message error: AuthenticationAgentBatchStateUpdate. system is in Monitor Mode");
        }
        //TD15069 ++azenitha

        // delete the state update message sender, state update, and state update sequence
        delete batchUpdate;
        batchUpdate = NULL;

        FUNCTION_EXIT;
    }


    void AuthenticationAgent::notifyGroupOffline(const std::string& groupName)
    {
		// TD15069 do nothing with group offline, should not end session 
		// since it might be a temparary network breakdown.
    }

    void AuthenticationAgent::notifyGroupOnline(const std::string& s)
    {
    }

    void AuthenticationAgent::setUpAgentEntityConfiguration()
    {
        TA_Base_Core::IEntityDataPtr iData = m_genericAgent->getAgentEntityConfiguration();

        unsigned long frequency = DEFAULT_FREQUENCY;
        unsigned long timeout = DEFAULT_TIMEOUT;

        try
        {
            TA_Base_Core::AuthenticationAgentEntityDataPtr agentData = 
				boost::dynamic_pointer_cast<TA_Base_Core::AuthenticationAgentEntityData>(iData);

            frequency = agentData->getCheckSessionFrequency();
            timeout = agentData->getTimeOutPeriod();
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Error retrieving configuration parameters. Setting to values to default");
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Error retrieving configuration parameters. Setting to values to default");
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Error retrieving configuration parameters. Setting to values to default");
        }

        char buffer[256];
        sprintf(buffer, "%lu", frequency);
        TA_Base_Core::RunParams::getInstance().set(RPARAM_SESSION_CHECKING_FREQUENCY, buffer);

        sprintf(buffer, "%lu", timeout);
        TA_Base_Core::RunParams::getInstance().set(RPARAM_SESSION_TIMEOUT_PERIOD, buffer);
    }

	void AuthenticationAgent::recieveStateUpdateMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message )
	{
        // In monitor mode there are two valid possibilities - a batch state update, or an ordinary state update.
        if ( 0 == std::string(message.messageTypeKey).compare
            ( TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentStateUpdate.getTypeKey() ) )
        {
            // Unfortunately, as the message libraries cause a double-insert on the sending end, need to do a double
            // extraction here. This time, extract to the actual object type.
            TA_Base_Bus::SessionInfoCorbaDef* session;
            if ( ( message.stateUpdateInfo >>= session) != 0 )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Received StateUpdate for session '%s'",
                    session->SessionId.in() );

                // hand off processing of this message
                processStateUpdate(session);
            }
            else 
            {
				// Bad - but can't do anything to prevent it, so just log.
				LOG_GENERIC (SourceInfo, DebugUtil::DebugWarn, "when recieveStateUpdateMessage, Failed attempt to extract batch state update from any.");
            }
        }
	}

	void AuthenticationAgent::recieveSessionTimeStateUpdateMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message )
	{
		if ( 0 == std::string(message.messageTypeKey).compare
			( TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentSessionTimeStateUpdate.getTypeKey() ) )
		{
			
			// Unfortunately, as the message libraries cause a double-insert on the sending end, need to do a double
			// extraction here. This time, extract to the actual object type.
			TA_Base_Bus::SessionInfoCorbaDef* session;
			if ( ( message.stateUpdateInfo >>= session) != 0 )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Received SessionTimeStateUpdate for session '%s'",
					session->SessionId.in() );
				
				// hand off processing of this message
				processSessionTimeStateUpdate(session);
			}
			else 
			{
				// Bad - but can't do anything to prevent it, so just log.
				LOG_GENERIC (SourceInfo, DebugUtil::DebugWarn, "when recieveSessionTimeStateUpdateMessage, Failed attempt to extract batch state update from any.");
			}
		}
	}

	void AuthenticationAgent::recieveBatchStateUpdateMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message )
	{
       if ( 0 == std::string(message.messageTypeKey).compare
            ( TA_Base_Core::AuthenticationAgentStateUpdate::AuthenticationAgentBatchStateUpdate.getTypeKey() ) )
        {
            TA_Base_Bus::AuthentcationBatchUpdateCorbaDef* batchUpdate;
            if ( ( message.stateUpdateInfo >>= batchUpdate) != 0 )
            {                
                unsigned int i; // have to define this here, as Visual Studio does not encapsulate for loop declarations correctly
                for (i = 0;i < batchUpdate->DatabaseQueue.length();i++)
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Processing batch database update for session '%s'.",
                        batchUpdate->DatabaseQueue[i].SessionId.in() );

                    // hand off processing of the DB-queue
                    processStateUpdate( &(batchUpdate->DatabaseQueue[i]) );
                }

                for (i = 0;i < batchUpdate->SessionRegister.length();i++)
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Processing batch register update for session '%s' (U:%d, W:%d, L:%d).",
                        batchUpdate->SessionRegister[i].SessionId.in(), batchUpdate->SessionRegister[i].UserId,
                        batchUpdate->SessionRegister[i].WorkstationId, batchUpdate->SessionRegister[i].LocationId );

                    SessionInfo session;
                    session.SessionId = batchUpdate->SessionRegister[i].SessionId;
                    session.LocationId = batchUpdate->SessionRegister[i].LocationId;
                    session.UserId = batchUpdate->SessionRegister[i].UserId;
                    session.WorkstationId = batchUpdate->SessionRegister[i].WorkstationId;
					session.isDisplayOnly = batchUpdate->SessionRegister[i].isDisplayOnly;	
                    unsigned int j;

                    session.OperatorId.push_back( batchUpdate->SessionRegister[i].UserId );
                    for ( j=0; j<batchUpdate->SessionRegister[i].additionalOperators.length(); j++ )
                    {
                        session.OperatorId.push_back( batchUpdate->SessionRegister[i].additionalOperators[j] );
                    }
                    session.ProfileId.push_back( batchUpdate->SessionRegister[i].ProfileId );
                    for ( j=0; j<batchUpdate->SessionRegister[i].additionalProfiles.length(); j++ )
                    {
                        session.ProfileId.push_back( batchUpdate->SessionRegister[i].additionalProfiles[j] );
                    }

                    m_authentication->registerUpdate(session);
                }

				m_bulkUpdateComplete.post();

            }
            else
            {
                // Bad - but can't do anything to prevent it, so just log.
                LOG_GENERIC (SourceInfo, DebugUtil::DebugWarn, "when recieveBatchStateUpdateMessage, Failed attempt to extract batch state update from any.");
            }
        }
	}

	void AuthenticationAgent::checkOperationMode()
	{
		TA_ASSERT(m_genericAgent != NULL, "m_genericAgent is NULL");
		
		if (m_genericAgent->getOperationMode() == TA_Base_Core::Monitor)
		{
			throw(TA_Base_Core::OperationModeException(RPARAM_MONITOR));
		}
	}

} // end namespace TA_Base_App
