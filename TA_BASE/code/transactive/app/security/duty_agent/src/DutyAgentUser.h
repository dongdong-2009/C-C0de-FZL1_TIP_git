#ifndef DUTYAGENTUSER_H_INCLUDED
#define DUTYAGENTUSER_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyAgentUser.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IGenericAgentUser interface. It is the "main" agent class.
 */


namespace TA_Base_App
{
	class DutyAgentImpl;

    class DutyAgentUser : public TA_Base_Bus::IGenericAgentUser
    {
    public:
	    DutyAgentUser(int argc, char* argv[]);
	    virtual ~DutyAgentUser();

		//
		// operations required by IGenericAgentUser
		//
		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode()
		{
			//do nothing...
		};
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


		virtual void notifyGroupOffline( const std::string& group );
		virtual void notifyGroupOnline ( const std::string& group );

		void unsubscribeMessages();    //limin, failover
		
		void registerForControlMessages();
		void registerForMonitorMessages();
    private:
        // Disable the copy constructor and operator-equals unless needed.
        DutyAgentUser(const DutyAgentUser& );
        DutyAgentUser& operator=(const DutyAgentUser&);


        /**
         * m_genericAgent
         *
         * A pointer to the Generic Agent instance for this agent.
         */
        TA_Base_Bus::GenericAgent* m_genericAgent;

        /**
         * m_DutyAgentImpl
         *
         * A pointer to the implementation of this agents CORBA interface.
         */
        DutyAgentImpl* m_dutyAgentImpl;

		std::string m_corbaName;

		ACE_Thread_Semaphore m_bulkUpdateComplete;

		std::string m_mode;

    };
}

#endif // !defined(DUTYAGENTUSER_H_INCLUDED)
