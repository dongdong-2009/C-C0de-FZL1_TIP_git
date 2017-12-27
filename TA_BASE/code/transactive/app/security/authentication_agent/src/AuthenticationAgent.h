/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/AuthenticationAgent.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IGenericAgentUser interface. It is the "main" agent class.
 */

#if !defined(AFX_AUTHENTICATIONAGENT_H__4205F4F2_F459_4A89_9F2A_DA4B6560198F__INCLUDED_)
#define AFX_AUTHENTICATIONAGENT_H__4205F4F2_F459_4A89_9F2A_DA4B6560198F__INCLUDED_

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "core/data_access_interface/src/ISession.h"

#include "SessionQueue.h"
#include "SessionQueueProcessor.h"

#include "app/security/authentication_agent/src/AuthenticationAgentImpl.h"
#include "app/security/authentication_agent/src/Authentication.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

namespace TA_Base_Bus
{
    class GenericAgent;                 // Forward Declaration
    class IEntity;                      // Forward Declaration
}

namespace TA_Base_Core
{   
    class IEntityData;                  // Forward Declaration
    class ConfigUpdateDetails;          // Forward Declaration
    struct StateUpdateMessageCorbaDef;  // Forward Declaration
}

namespace TA_Base_App
{

    struct TA_Base_Bus::SessionInfoCorbaDef;         // Forward Declaration
    
    class AuthenticationAgent : public TA_Base_Bus::IGenericAgentUser
    {
    public:

         /**
          * Constructor
          *
          * @param The number of command line arguments passed in 
          * @param The command line
          *
          * @exception GenericAgentException thrown if GenericAgent fails to
          *            initialise.
          */
	    AuthenticationAgent(int argc, char* argv[]);

        /**
         * Destructor
         */
	    virtual ~AuthenticationAgent();

		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode();
        /**
         * agentTerminate
         *
         * This method is called by GenericAgent when a terminate instruction is received.
         */
        void agentTerminate();

        /**
         * CreateEntity
         *
         * This method is used to create IEntity-based objects for this agent, from the
         * passed in IEntityData object. 
         * NB: This stub implementation will always return NULL.
         */
        TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

        /**
         * agentSetMonitor
         *
         * This method is called when the agent is placed into monitor mode. It is 
         * responsible for making any necessary state changes in the agent to place it in
         * monitor mode.
         */
        void agentSetMonitor();

        /**
         * agentSetControl
         *
         * This method is called when the agent is placed into control mode. It is 
         * responsible for making any necessary state changes in the agent to place it in
         * control mode.
         */
        void agentSetControl();

        /**
         * registerForStateUpdates
         *
         * This method is called upon initialisation. It is to be used to register the 
         * agent for any necessary state updates.
         */
        void registerForStateUpdates();

        /**
         * receiveSpecialisedMessage
         *
         * This method is called when the agent receives a state update message (as 
         * registered for in the registerForStateUpdates method)
         */
        void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);
        
        /**
         * processOnlineUpdate
         *
         * This method is called if Generic Agent receives a config update message that
         * it determines will be of interest to the agent (that is, it pertains to the
         * agent, and not to any libraries). This method is responsible for analyzing the
         * update message, and applying the updates where necessary.
         */
        void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
         * run
         *
         * This method is called by the agent mainline, and is used start any necessary
         * threads before calling GenericAgent::run which will block until the process
         * is terminated.
         */
        void run();

		/** 
		  * notifyGroupOffline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOffline from the Process Monitor.
          *
          * @param The group name
		  */
        void notifyGroupOffline( const std::string& group );

		/** 
		  * notifyGroupOnline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOnline from the Process Monitor.
          *
          * @param The group name
		  */
        void notifyGroupOnline( const std::string& group );
		std::string getEntityName();

    private:
         // Disable the copy constructor and operator-equals unless needed.
        AuthenticationAgent(const AuthenticationAgent& theAuthenticationAgent);
        AuthenticationAgent& operator=(const AuthenticationAgent&);

        /**
         * processStateUpdate
         *
         * Processes the incoming state update message, updating the monitored queue as necessary
         *
         * @param session The session found on the state update
         */
        void processStateUpdate(TA_Base_Bus::SessionInfoCorbaDef* session);

        /**
         * sendBatchStateUpdate
         *
         * Sends a state update method that contains the entire queue. This is used 
         * when a monitor agent starts up, to retrieve the entire event queue.
         */
        void sendBatchStateUpdate();

        /**
         * setUpAgentEntityConfiguration
         *
         * Retrieves the agent entity configuration and stores it for later retrieval.
         */
        void setUpAgentEntityConfiguration();

        void processSessionTimeStateUpdate(TA_Base_Bus::SessionInfoCorbaDef* session);

		void fillOutSessionRegister(TA_Base_Bus::AuthentcationBatchUpdateCorbaDef* batchUpdate, 
			std::vector<TA_Base_Core::ISession*> &sessions );

		void fillOutDataBaseQueue(TA_Base_Bus::AuthentcationBatchUpdateCorbaDef* batchUpdate, 
		     std::vector<TA_Base_Core::ISession*> &sessions );
		
		void doCleanUp( bool shouldStopThread );
		
		void recieveStateUpdateMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message );
		
		void recieveBatchStateUpdateMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message );
		void recieveSessionTimeStateUpdateMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message );

		void registMonitorMessage();
		void registControlMessage();
        void unsubscribeToMessages();//limin, failover
		
	private:

        /**
         * m_genericAgent
         *
         * A pointer to the Generic Agent instance for this agent.
         */
		TA_Base_Bus::GenericAgent* m_genericAgent;

        /**
         * m_authenticationAgentImpl
         *
         * A pointer to the implementation of the CORBA interface of this agent.
         */
        AuthenticationAgentImpl* m_authenticationAgentImpl;

        /**
         * m_authentication
         *
         * A pointer to the object that the CORBA interface uses to perform all processing.
         */
        Authentication* m_authentication;

		/**
         * m_isInControl
         *
         * A boolean flag that determines if the agent is in control or monitor mode. True if the agent is
         * in control mode.
         */
        bool m_isInControl;

		/**
         * m_sessionQueue
         *
         * Updates the remote session queue.
         */
        QueueStateUpdater* m_queueStateUpdater;

        /**
         * m_sessionQueue
         *
         * Thread-safe queue of session details.
         */
        SessionQueue* m_sessionQueue;

        /**
         * m_sessionQueue
         *
         * Monitors m_sessionQueue and writes sessions to the database.
         */
        SessionQueueProcessor* m_sessionQueueProcessor;

        /**
         * m_updateLock
         *
         * A thread lockable object to guard access to the monitored queue object (esp. for destruction).
         */
        TA_Base_Core::NonReEntrantThreadLockable m_updateLock;

        

        /**
         * m_entityKey
         *
         * The entity key of this agent
         */
        unsigned long m_entityKey;

		TA_Base_Core::StateUpdateMessageSender* m_localStateSender;
		
		TA_Base_Core::Semaphore m_bulkUpdateComplete;
    };
}

#endif // !defined(AFX_AUTHENTICATIONAGENT_H__4205F4F2_F459_4A89_9F2A_DA4B6560198F__INCLUDED_)
