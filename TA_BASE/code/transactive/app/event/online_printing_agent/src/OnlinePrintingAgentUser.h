#if !defined(ONLINEPRINTINGAGENTUSER_H_INCLUDED)
#define ONLINEPRINTINGAGENTUSER_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/OnlinePrintingAgentUser.h $
 * @author:   Bart Golab
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class implements the IGenericAgentUser interface. It is the "main" agent class.
 *
 */

namespace TA_Base_App
{
    class OnlinePrintingAgentUser : public TA_Base_Bus::IGenericAgentUser
    {
    // Operations
    public:
        OnlinePrintingAgentUser(int argCount, char* args[]);
        virtual ~OnlinePrintingAgentUser();

        /**
         * agentTerminate
         *
         * This method is called by GenericAgent when a terminate instruction is received.
         */
        virtual void agentTerminate();
		//TD18706,jianghp
		virtual void checkOperationMode(){};
        /**
         * CreateEntity
         *
         * This method is used to create IEntity-based objects for this agent, from the
         * passed in IEntityData object. 
         * NB: This stub implementation will always return NULL.
         */
        virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr EntityData);

        /**
         * agentSetMonitor
         *
         * This method is called when the agent is placed into monitor mode. It is 
         * responsible for making any necessary state changes in the agent to place it in
         * monitor mode.
         */
        virtual void agentSetMonitor();

        /**
         * agentSetControl
         *
         * This method is called when the agent is placed into control mode. It is 
         * responsible for making any necessary state changes in the agent to place it in
         * control mode.
         */
        virtual void agentSetControl();

		/** 
		  * notifyGroupOffline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOffline from the Process Monitor.
		  */
        virtual void notifyGroupOffline(const std::string& group);

		/** 
		  * notifyGroupOnline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOnline from the Process Monitor.
		  */
        virtual void notifyGroupOnline(const std::string& group);

        /**
         * registerForStateUpdates
         *
         * This method is called upon initialisation. It is to be used to register the 
         * agent for any necessary state updates.
         */
        virtual void registerForStateUpdates();

        /**
         * receiveSpecialisedMessage
         *
         * This method is called when the agent receives a state update message (as 
         * registered for in the registerForStateUpdates method)
         */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);
        
        /**
         * processOnlineUpdate
         *
         * This method is called if Generic Agent receives a config update message that
         * it determines will be of interest to the agent (that is, it pertains to the
         * agent, and not to any libraries). This method is responsible for analyzing the
         * update message, and applying the updates where necessary.
         */
        virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
         * run
         *
         * This method is called by the agent mainline, and is used start any necessary
         * threads before calling GenericAgent::run which will block until the process
         * is terminated.
         */
        virtual void run();
		std::string getEntityName();

    private:
        // Disable the copy constructor and assignment operator unless needed.
        OnlinePrintingAgentUser(const OnlinePrintingAgentUser&);
        OnlinePrintingAgentUser& operator=(const OnlinePrintingAgentUser&);

        void unsubscribeToMessages();//limin, failover

    // Attributes
    private:
        TA_Base_Bus::GenericAgent* m_genericAgent;
        PrintJobSpooler* m_printJobSpooler;
        PrintingService* m_printingService;
        PrintableEventGenerator* m_printableEventGenerator;
		//[[add by wuzhongyi for FailOver
		std::string m_agentEnityName;
		//]]
    };
}

#endif // !defined(ONLINEPRINTINGAGENTUSER_H_INCLUDED)
