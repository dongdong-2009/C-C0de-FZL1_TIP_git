/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada_agent/src/IScadaAgentUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/02/06 10:52:23 $
  * Last mofified by:  $Author: Dhanshri $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#if !defined(ISCADAAGENTUSER)
#define ISCADAAGENTUSER

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"
#include "bus/generic_agent/src/IEntity.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{
    class ConfigUpdateDetails; // Forward Declaration
}

namespace TA_Base_Bus
{
	class ScadaAgent;
    class IScadaAgentUser
    {
    public:
		virtual void setScadaAgent(ScadaAgent* scadaAgent) = 0;
        /**
          * agentTerminate
          *
          * This pure virtual method is called from the onTerminate
          * method in ScadaAgent. This allows the agent to do any
          * of its own cleaning up before exiting.
          *
          * If the agent's entities implement CORBA interfaces,
          * this method should take care of deactivating them. It is not
          * possible for ScadaAgent to do this because the IEntity 
          * interface is not a ServantBase.
          *
          * If the class implementing this interface holds the ScadaAgent
          * as a member variable, DO NOT delete it inside this method.
          * ScadaAgent waits for this call to return before shutting down
          * the ORB. Deleting the ScadaAgent from within this method will
          * cause obvious problems.
          */
        virtual void agentTerminate() = 0;

        
        /** 
          * createAllEntities
          *
          * [optional]
          *
          * This allows for bulk entity creation.
          * It is called first (before any calls to createEntity) to allow agents to
          * optimise entity creation. If true is returned, the entities are already created
          * if false is returned, then createEntity is called one by one for each entity.
          * Also entityCreationComplete will be called at the end of the creation
          *
          * Note that even if using this method, createEntity should still be implemented
          * as it is used for entity updates
          *
          * @param agentEntityData the agent's entity data
          * @param entityDataList a list of shared pointers to all entities
          * @param createdEntities the entities created to return to generic agent
          *
          * @return true if entity creation was done, false if createEntity should be used
          *
          */
        virtual bool createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                        const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                        IEntityList& createdEntities) = 0;        

        /**
          * createEntity
          *
          * This method takes Entity Configuration data stored in the database and
          * generates an object that implements the IEntity interface. The returned
          * objects are stored and managed within Generic Agent. The Agent can get a 
          * pointer to the objects via the getEntityMap method in ScadaAgent.
          *
          * If create entity throws an exception when Generic Agent is creating its
          * vector of entities (in the ScadaAgent::doEntityConfiguration method), 
          * the Generic Agent construction will fail.
          *
          * @param entityData data retrieved from the database.
          *
          * @return The constructed entity
          */
        virtual IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData) = 0;

        /** 
          * entityCreationComplete
          *
          * [optional]
          *
          * this is called back after 
          * createEntity has been called for all entities.
          * The client can do any clean up of temporary data.
          */
        virtual void entityCreationComplete() = 0;

        /**
          * agentSetMonitor
          *
          * This pure virtual method is called by ScadaAgent whenever it receives
          * a request from the Process Monitor to change its operating state to monitor.
          * The derived agent should respond by subscribing/unsubscribing to/from the 
          * appropriate message channels and performing any other internal house keeping.
          */
        virtual void agentSetMonitor() = 0;

        /**
          * agentSetControl
          *
          * This pure virtual method is called by ScadaAgent whenever it receives
          * a request from the Process Monitor to change its operating state to control.
          * The derived agent should respond by subscribing/unsubscribing to/from the 
          * appropriate message channels and performing any other internal house keeping.
          */
        virtual void agentSetControl() = 0;

		/**
          * agentSetStandby
          *
          * This pure virtual method is called by ScadaAgent whenever it receives
          * a request from the Process Monitor to change its operating state to standby.
          * The derived agent should respond by subscribing/unsubscribing to/from the 
          * appropriate message channels and performing any other internal house keeping.
          */
		virtual void agentSetStandby() = 0;

		/** 
		  * notifyGroupOffline
		  *
		  * This request is passed on by ScadaAgent when it receives a call to 
          * notifyGroupOffline from the Process Monitor.
          *
          * @param The group name
		  */
        virtual void notifyGroupOffline( const std::string& group ) = 0;

		/** 
		  * notifyGroupOnline
		  *
		  * This request is passed on by ScadaAgent when it receives a call to 
          * notifyGroupOnline from the Process Monitor.
          *
          * @param The group name
		  */
        virtual void notifyGroupOnline( const std::string& group ) = 0;

        /**
          * registerForStateUpdates
          *
          * This pure virtual method forces agents to subscribe for either
          * state updates or state update requests, depending on whether or not they
          * are operating in monitor or control mode.
          */
        virtual void registerForStateUpdates() = 0;

        /**
          * receiveSpecialisedMessage
          *
          * This method is called whenever the Agent receives a state update or
          * state update request (depending on their mode of operation)
          */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message) = 0;

        /**
          * processOnlineUpdate
          *
          * Generic Agent will handle the receiving of online updates. However, in some
          * cases, it will be necessary to pass the updates on to the registered agent.
          * For example, if a child entity needs to be updated.
          *
          * @param The update event
          */
        virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent) = 0;

		virtual void beforeGetDescendantsOfAgent(TA_Base_Core::IEntityDataPtr agentEntity) = 0;
    };
}

#endif // !defined(ISCADAAGENTUSER)
