/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_agent/src/IGenericAgentUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #2 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2016/10/17 17:53:57 $
  * Last mofified by:  $Author: limin.zhu $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#if !defined(IGENERICAGENTUSER)
#define IGENERICAGENTUSER

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

    class IGenericAgentUser :   public virtual TA_Base_Core::SpecialisedMessageSubscriber
                                               <TA_Base_Core::StateUpdateMessageCorbaDef>

    {
    public:

        /**
          * Constructor
          */
        IGenericAgentUser(){};

        /**
          * Destructor
          */
        virtual ~IGenericAgentUser(){}

		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode() = 0;

        /**
          * agentTerminate
          *
          * This pure virtual method is called from the onTerminate
          * method in GenericAgent. This allows the agent to do any
          * of its own cleaning up before exiting.
          *
          * If the agent's entities implement CORBA interfaces,
          * this method should take care of deactivating them. It is not
          * possible for GenericAgent to do this because the IEntity 
          * interface is not a ServantBase.
          *
          * If the class implementing this interface holds the GenericAgent
          * as a member variable, DO NOT delete it inside this method.
          * GenericAgent waits for this call to return before shutting down
          * the ORB. Deleting the GenericAgent from within this method will
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
                                        IEntityList& createdEntities)
        {return false;};

        /**
          * createEntity
          *
          * This method takes Entity Configuration data stored in the database and
          * generates an object that implements the IEntity interface. The returned
          * objects are stored and managed within Generic Agent. The Agent can get a 
          * pointer to the objects via the getEntityMap method in GenericAgent.
          *
          * If create entity throws an exception when Generic Agent is creating its
          * vector of entities (in the GenericAgent::doEntityConfiguration method), 
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
        virtual void entityCreationComplete(){};

        /**
          * agentSetMonitor
          *
          * This pure virtual method is called by GenericAgent whenever it receives
          * a request from the Process Monitor to change its operating state to monitor.
          * The derived agent should respond by subscribing/unsubscribing to/from the 
          * appropriate message channels and performing any other internal house keeping.
          */
        virtual void agentSetMonitor() = 0;

        /**
          * agentSetControl
          *
          * This pure virtual method is called by GenericAgent whenever it receives
          * a request from the Process Monitor to change its operating state to control.
          * The derived agent should respond by subscribing/unsubscribing to/from the 
          * appropriate message channels and performing any other internal house keeping.
          */
        virtual void agentSetControl() = 0;

		/**
          * agentSetStandby
          *
          * This pure virtual method is called by GenericAgent whenever it receives
          * a request from the Process Monitor to change its operating state to standby.
          * The derived agent should respond by subscribing/unsubscribing to/from the 
          * appropriate message channels and performing any other internal house keeping.
          */
		virtual void agentSetStandby(){}

		/** 
		  * notifyGroupOffline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOffline from the Process Monitor.
          *
          * @param The group name
		  */
        virtual void notifyGroupOffline( const std::string& group ) = 0;

		/** 
		  * notifyGroupOnline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
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

		std::string getAgentEntityName()
		{
			if ( m_agentEnityName == "" )
			{
				m_agentEnityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			}
			if ( m_agentEnityName == "")
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "entity name is empty");
				return "ErrorEntity";
			}
			
			return m_agentEnityName;
		}

		virtual void beforeGetDescendantsOfAgent(TA_Base_Core::IEntityDataPtr agentEntity) {};

        virtual void handleDoubleAgentsControlRestored() {};

    private:
        // Disable copy constructor and assignment operator
        IGenericAgentUser( const IGenericAgentUser& theGenericAgentUser);
        IGenericAgentUser& operator=(const IGenericAgentUser &);
	
	protected:
		//xinsong ++
		std::string m_agentEnityName;

    };
}

#endif // !defined(IGENERICAGENTUSER)
