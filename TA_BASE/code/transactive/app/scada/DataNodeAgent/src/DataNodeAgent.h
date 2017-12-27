/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeAgent/src/DataNodeAgent.h $
  * @author:	Darren Sampson
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2012/06/12 13:35:44 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	DataNodeAgent implementes the GenericAgent interface.  DataNodeAgent monitors and controls the 
  * the entities associated with its process, such as DataNode entities.
  *
  */

#ifndef DATANODEAGENT_H
#define DATANODEAGENT_H

#include <list>

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class StateUpdateMessageSender;
	class DataNodeAccessFactory;
};

namespace TA_Base_Bus
{
    class DataNode;
    class DataNodeActivationProcessor;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;
	class GenericAgent;
	class ScadaProxyFactory;
	class SummaryProcessor;
	class SummaryProcessor;
};

namespace TA_Base_App
{
    typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;

	class DataNodeAgent : public TA_Base_Bus::IGenericAgentUser,
						  public TA_Base_Bus::IDataNodeAgent
	{

	public:

		DataNodeAgent ( int argc, char* argv[] );
		virtual ~DataNodeAgent();

		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode()
		{
			//do nothing...
		};
		//
		// operations required by IGenericAgentUser
		//

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
		virtual void agentTerminate();

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
                                        TA_Base_Bus::IEntityList& createdEntities );

        /** 
          * entityCreationComplete
          *
          * [optional]
          *
          * this is called back after 
          * createEntity has been called for all entities.
          * The client can do any clean up of temporary data.
          */
        virtual void entityCreationComplete();

		/**
		  * createEntity
		  *
		  * This method takes Entity Configuration data stored in the database and
		  * generates an object that implements the IEntity interface. The returned
		  * objects are stored and managed within Generic Agent. The Agent can get a 
		  * pointer to the objects via the getEntityMap method in GenericAgent.
		  *
		  * @param Entity data retrieved from the database.
		  *
		  * @return The constructed entity
		  */
		virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

		/**
		  * agentSetMonitor
		  *
		  * This pure virtual method is called by GenericAgent whenever it receives
		  * a request from the Process Monitor to change its operating state to monitor.
		  * The derived agent should respond by subscribing/unsubscribing to/from the 
		  * appropriate message channels and performing any other internal house keeping.
		  */
		virtual void agentSetMonitor();

		/**
		  * agentSetControl
		  *
		  * This pure virtual method is called by GenericAgent whenever it receives
		  * a request from the Process Monitor to change its operating state to control.
		  * The derived agent should respond by subscribing/unsubscribing to/from the 
		  * appropriate message channels and performing any other internal house keeping.
		  */
		virtual void agentSetControl();

		/**
		  * registerForStateUpdates
		  *
		  * This pure virtual method forces agents to subscribe for either
		  * state updates or state update requests, depending on whether or not they
		  * are operating in monitor or control mode.
		  */
		virtual void registerForStateUpdates();

		/**
		  * receiveSpecialisedMessage
		  *
		  * This method is called whenever the Agent receives a state update or
		  * state update request (depending on their mode of operation)
		  */
		virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

		/**
		  * processOnlineUpdate
		  *
		  * Generic Agent will handle the receiving of online updates. However, in some
		  * cases, it will be necessary to pass the updates on to the registered agent.
		  * For example, if a child entity needs to be updated.
		  *
		  * @param The update event
		  */
		virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		virtual void notifyGroupOffline( const std::string& group );

		virtual void notifyGroupOnline( const std::string& group );


		//
		// operations required by IDataPointAgent interface
		//

		virtual TA_Base_Bus::DataNode * getDataNode( unsigned long entityKey );

        //Mintao++
	    /**
	      * getAllDataNodes 
	      * 
	      * <description>
	      * 
	      * @return virtual void 
	      * @param : std::map < unsigned long
	      * @param : TA_Base_Bus::DataNode *>& dataNodeList
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList );

		//
		// opearations specific to DataNodeAgent
		//

		/**
		  * startDataNodeAgent
		  *
		  * Start this DataNodeAgent process
		  *
		  */
		void startDataNodeAgent();

	// attributes
	private:

        void agentSetControlInThread();//limin, for failover
		unsigned long m_DataNodeAgentPrimaryKey;
		bool m_agentSetControl;
		std::string m_stationName;
		std::string m_agentName;

		TA_Base_Bus::GenericAgent * m_genericAgent;
		
		TA_Base_Bus::ScadaProxyFactory* m_scadaProxyFactory;
				
		TA_Base_Bus::SummaryProcessor* m_summaryProcessor;

		TA_Base_Bus::DataNodeAgentCorbaDef_Impl * m_bulkPolledInterface;
		TA_Base_Bus::DataNodeAccessCorbaDef_Impl * m_datanodeAccessInterface;
		TA_Base_Core::ReEntrantThreadLockable m_dataNodeListLock;
        TA_Base_Bus::ScadaPersistencePtr m_persistence;

        DataNodeList m_dataNodes;
	};
}
#endif

