/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/SystemStatusAgent.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#ifndef SYSTEM_STATUS_AGENT_H
#define SYSTEM_STATUS_AGENT_H

#if defined(WIN32)
#pragma warning(disable:4786)
#endif

#include <vector>
#include <set>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include "core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h"

#include "core/threads/src/Thread.h"

namespace TA_Base_Bus
{
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNode;
	class GenericAgent;
    class NotifyEventProcessor;
    class LocalUpdateEventProcessor;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;
}

namespace TA_Base_Core
{
	class SystemStatusAgentEntityData;
	class AlarmHelper;
	class IEntityData;
    class threadPoolSingletonManager;
}

namespace TA_Base_App
{
	
	class DataPointDriverHelper;


    class SystemStatusAgent : public virtual TA_Base_Bus::IGenericAgentUser,
							  public TA_Base_Bus::IDataPointAgent,
							  public TA_Base_Bus::IDataNodeAgent
    {
		typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;
    public:

        /**
          * The only avaiable CTOR
		  * The operation mode is set to NotApplicable in the CTOR so that
		  * when it is set to either Control or monitor init will take place
          */
        SystemStatusAgent(int argc, char* argv[]);

        /**
          * Destructor
          */
        virtual ~SystemStatusAgent();

       //IGenericAgentUser Methods:
		virtual void checkOperationMode()
		{//TD18706,jianghp
			//do nothing...
		}
        /**
          * agentTerminate
          *
          * This pure virtual method is called from the onTerminate
          * method in GenericAgent. This allows the agent to do any
          * of its own cleaning up before exiting.
          *
          * This method calls deactivateServant() on all the agent's children
		  * and calls DataPointDriverHelper::terminate()
		  *
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
          * If create entity throws an exception when Generic Agent is creating its
          * vector of entities (in the GenericAgent::doEntityConfiguration method), 
          * the Generic Agent construction will fail.
		  *
		  * The children of this agent are all datapoints. 
          *
          * @param Entity data retrieved from the database.
          *
          * @return The constructed and registed entity (datapoint) 
		  * if the config passed in entityData can used to construct a datapoint
		  * that this agent can own (see IDataPointDriver). Will return 0 if the
		  * datapoint can not be constructed.
		  * Assuming the point is created its quality will be set to:
		  * TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED indicating that it has not been initialized
		  * 
		  *
		  * @throw SystemStatusAgentConfigException if:
		  * entityData.getType() != TA_Base_Core::DataPointEntityData::getStaticType()
          */
	    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr EntityData);

        /**
          * agentSetMonitor
          *
          * This pure virtual method is called by GenericAgent whenever it receives
          * a request from the Process Monitor to change its operating state to monitor.
          * The derived agent should respond by subscribing/unsubscribing to/from the 
          * appropriate message channels and performing any other internal house keeping.
		  *
		  * Note that setting the mode of child entities is handled by the gen_agent framework and
		  * should not be done in this method
		  *
		  * stop polling values
		  * set quality to bad for all childpoints
		  */
        virtual void agentSetMonitor();

        /**
          * agentSetControl
          *
          * This pure virtual method is called by GenericAgent whenever it receives
          * a request from the Process Monitor to change its operating state to control.
          * The derived agent should respond by subscribing/unsubscribing to/from the 
          * appropriate message channels and performing any other internal house keeping.
		  *
		  * Note that setting the mode of child entities is handled by the gen_agent framework and
		  * should not be done in this method
		  *
		  * start polling
		  * set quality and values A/R
		  * re-init alarms by sending alarm updates for all points reqarless of state
		  *
          */
        virtual void agentSetControl();

		/** 
		  * notifyGroupOffline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
		  * notifyGroupOffline from the Process Monitor.
		  *
		  * The System Status Agent does not modify its behaviour based on the online/offline
		  * state of the System Controller. Hence, this method does nothing
		  *
		  * @param The group name
		  */
        virtual void notifyGroupOffline( const std::string& group );

		/** 
		  * notifyGroupOnline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
		  * notifyGroupOnline from the Process Monitor.
		  *
		  * The System Status Agent does not modify its behaviour based on the online/offline
		  * state of the System Controller. Hence, this method does nothing
		  *
		  * @param The group name
		  */
        virtual void notifyGroupOnline( const std::string& group );

        /**
          * registerForStateUpdates
          *
          * This pure virtual method forces agents to subscribe for either
          * state updates or state update requests, depending on whether or not they
          * are operating in monitor or control mode.
		  *
		  * Currently this agent does not register for any messages because 
		  * it just don't care about any. This Agent does not need to sync. Hence,
		  * this method don't do jack other than satisfy the IGenericAgentUser interface
		  * 
          */
        virtual void registerForStateUpdates();

        /**
          * receiveSpecialisedMessage
          *
          * This method is called whenever the Agent receives a state update or
          * state update request (depending on their mode of operation)
		  *
		  * Currently I don't register for any messages because I just don't care 
		  * about any. This Agent does not need to sync. Hence this method currently just
		  * logs an error message stating its suprise at this method being called
		  * 
          */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

        /**
          * processOnlineUpdate
          *
          * Generic Agent will handle the receiving of online updates. However, in some
          * cases, it will be necessary to pass the updates on to the registered agent.
          * For example, if a child entity needs to be updated.
		  *
		  * Agent Configuration parameter changes are passed to the DataPointDriverHelper
		  * which in turn passes them to the IDataPointDriver implimentations.
		  *
		  * Online updates of datapoints is not supported. Hence, updates of this
		  * type are ignored by this method. Restart the agent to apply updates.
          *
          * @param The update event
          */
        virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		//End of IGenericAgentUser Methods

		//
		// operations required by IDataPointAgent interface
		//

		virtual TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey );

		/**
		  * run
		  *
		  * This method does the following:
		  * clears alarms on this agent then raises one if any of the children
		  * could not be owned.
		  * Calls SetMonitor or SetControl AR on DataPointDriverHelper
		  * calls GenericAgent::run() which blocks until the System
		  * Controller shuts us down
		  */
		void run();

		/**
		  * getAgentData
		  *
		  * @return SystemStatusAgentEntityData* for this agentUser
		  */
		TA_Base_Core::SystemStatusAgentEntityDataPtr getAgentData();

		//Mintao++
		virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );

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

        virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();

		// Bohong++ for failover
		void failoverSpecific();

	private:

		//data members:

		//The datapoints that this agent owns
		//typedef std::vector< TA_Base_Bus::DataPoint* > DataPoints;
		//typedef DataPoints::iterator IterToDp;
		//DataPoints m_dataPoints;
        std::map< unsigned long, TA_Base_Bus::DataPoint * > m_datapoints;
		
        std::set<std::string> m_failedDpNames;

		//private methods:

		/**
		  * initAlarmsOnThisAgent
		  *
		  * Clear any alarms on this agent (entity versus entity type)
		  * Then raise a single alarm if any datapoints passed to createEntity
		  * can not be registered by the DatapointFactory or owned by an avaiable
		  * driver
		  *
		  * Note that this agent raises alarms on itself (versus a datapoint)
		  * if it can't create or keep-updated any datapoint see 
		  * DataPointDriverHelper::giveDataPointToADriver()
		  */
		void initAlarmsOnThisAgent();

		/**
		  * setUpDataPointLib
		  *
		  * This method instantiates a bunch of objects related to the library
		  * These objects are not accessed dirctly in this code but evidently
		  * we need to bring them to life just the same
		  * 
		  */    
		void setUpDataPointLib();

		/**
		  * cleanUpDataPointLib
		  *
		  * This method cleans up a bunch of objects related to the library
		  * These objects are not accessed dirctly in this code but evidently
		  * we need to bring them to life just the same
		  * 
		  */    
		void cleanUpDataPointLib();

        // Disable copy constructor and assignment operator
        SystemStatusAgent( const SystemStatusAgent& sysStatAgent);
        SystemStatusAgent& operator=(const SystemStatusAgent& anotherSysStatAgent);

		//data members:
		TA_Base_Core::EOperationMode	m_operationMode;

		//The agent and related data:
		unsigned long m_agentKey;

		TA_Base_Bus::GenericAgent* m_genericAgent;
		TA_Base_Core::SystemStatusAgentEntityDataPtr m_agentEntityData;
		DataPointDriverHelper* m_driverHelper;		

		// Deliberately created as a member because it is needed before generic agent is created.
		TA_Base_Bus::DataPointFactory *m_dataPointFactory;
        
		TA_Base_Bus::ScadaProxyFactory* m_scadaProxyFactory;
		TA_Base_Core::ThreadPoolSingletonManager * m_threadPoolSingletonManager;
		TA_Base_Bus::SummaryProcessor* m_summaryProcessor;
		TA_Base_Bus::ScadaPersistencePtr m_persistence;

		TA_Base_Bus::DataPointAgentCorbaDef_Impl* m_dpBulkPolledInterface;
        TA_Base_Bus::DataPointAccessCorbaDef_Impl * m_dpAccessInterface;
		TA_Base_Bus::DataNodeAgentCorbaDef_Impl * m_dnBulkPolledInterface;
		TA_Base_Bus::DataNodeAccessCorbaDef_Impl * m_dnAccessInterface;
        
		TA_Base_Core::ReEntrantThreadLockable m_dataPointListLock;
		TA_Base_Core::ReEntrantThreadLockable m_dataNodeListLock;

		DataNodeList m_dataNodes;

		//Bohong++ for failover only
		bool	isFirstLaunch;

		//TD18093,jianghp
		//TA_Base_Core::AlarmHelper& m_alarmHelper;
    };

	// Bohong++ raise a new thread to reduce the time failover takes
    class failoverLoadingThread : public TA_Base_Core::Thread
    {	
    public:
        failoverLoadingThread(SystemStatusAgent* pSystemStatusAgent): m_agent(pSystemStatusAgent) {};
        ~failoverLoadingThread(){};
		
        virtual void run(){ m_agent->failoverSpecific(); };
        virtual void terminate(){};
		
    private:
        SystemStatusAgent * m_agent;
    };
} // end namespace TA_Base_App



#endif // !defined(SYSTEM_STATUS_AGENT_H)
