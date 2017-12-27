/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_agent/src/ScadaAgent.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #3 $
  *
  *
  * Last modification: $DateTime: 2014/07/16 10:52:23 $
  * Last mofified by:  $Author: weikun.lin $
  *
  * SCADA Agent provides a base implementation for functionality that is required
  * by all Agents holding data node and data point within TransActive.
  */

#if !defined(SCADAAGENT)
#define SCADAAGENT

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <map>
#include "ScadaAgentUser.h"

#include "core/threads/src/LFThreadPoolManager.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/timers/src/ITimeoutCallback.h"


namespace TA_Base_Core
{
	// Foward Declaration    
	class ManagedProcess; 
    class ConfigUpdateDetails; 
    class MessageType; 
    class PollListener; 
	class IThreadPoolExecuteItem;
	class AuditMessageSender;
    class MessageType;
	class ThreadPoolSingletonManager;
}

namespace TA_Base_Bus
{
    // Forward Declaration
	class DataPointFactory;
	class LocalUpdateEventProcessor;
	class GenericAgent;
	class AgentStateUpdateQueueSingletonManager;
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNode;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;
 //   class DataUpdateSender;
	//class DataPointUpdateProcessor;
	//class DataNodeUpdateProcessor;
 //   class DataUpdateServant;

	typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;
	class ScadaAgent;
	typedef void (ScadaAgent::*FuncInit)(TA_Base_Core::IEntityDataPtr agentEntityData);

	class AgentInitThread : public TA_Base_Core::Thread
	{
	public:
		AgentInitThread(ScadaAgent& agent, FuncInit func, TA_Base_Core::IEntityDataPtr agentEntityData);
		~AgentInitThread();
		bool waitForCompelete();
		void run();
		void terminate();
	private:
		TA_Base_Core::Semaphore			m_semphore;
		FuncInit						m_func;
		TA_Base_Core::IEntityDataPtr	m_agentEntityData;
		ScadaAgent&						m_agent;
		bool							m_executeSuccess;
	};

	class CreateEntityWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		CreateEntityWorkItem(ScadaAgent& agent, 
			TA_Base_Core::IEntityDataPtr entityData, 
			TA_Base_Core::WorkItemExeSync* workSync);
		void executeWorkItem();
	private:
		ScadaAgent&						m_parent;
		TA_Base_Core::IEntityDataPtr	m_entityData;
		TA_Base_Core::WorkItemExeSync*  m_sync;
	};

	class InitDataPointWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		InitDataPointWorkItem(TA_Base_Bus::DataPoint* dp, TA_Base_Core::WorkItemExeSync* workSync);
		void executeWorkItem();
	private:
		TA_Base_Bus::DataPoint*			m_dp;
		TA_Base_Core::WorkItemExeSync*  m_sync;
	};

	class DataNodeActivateWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		//DataNodeActivateWorkItem(TA_Base_Bus::DataNode* dn, TA_Base_Core::WorkItemExeSync* workSync);
		DataNodeActivateWorkItem(std::vector<TA_Base_Bus::DataNode * > datanodes, TA_Base_Core::WorkItemExeSync* workSync);
		void executeWorkItem();
	private:
		//TA_Base_Bus::DataNode*			m_dn;
		std::vector<TA_Base_Bus::DataNode * > m_datanodes;
		TA_Base_Core::WorkItemExeSync*  m_sync;
	};

	class ChangeDataNodeOpertionModeWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		ChangeDataNodeOpertionModeWorkItem(TA_Base_Bus::DataNode* dn, 
			TA_Base_Core::EOperationMode operationMode,
			TA_Base_Core::WorkItemExeSync* workSync);
		void executeWorkItem();
	private:
		TA_Base_Bus::DataNode*			m_dn;
		TA_Base_Core::EOperationMode 	m_operationMode;
		TA_Base_Core::WorkItemExeSync*  m_sync;
	};

	class ChangeDataPointOpertionModeWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		ChangeDataPointOpertionModeWorkItem(TA_Base_Bus::DataPoint* dp, 
			TA_Base_Core::EOperationMode operationMode,
			TA_Base_Core::WorkItemExeSync* workSync);
		void executeWorkItem();
	private:
		TA_Base_Bus::DataPoint*			m_dp;
		TA_Base_Core::EOperationMode 	m_operationMode;
		TA_Base_Core::WorkItemExeSync*  m_sync;
	};

    class ScadaAgent :	public virtual TA_Base_Bus::IGenericAgentUser,
						public virtual TA_Base_Bus::IDataPointAgent,
						public virtual TA_Base_Bus::IDataNodeAgent
    {
    public:
        /**
          * Constructor
		  */
		  ScadaAgent(int argc, char* argv[], IScadaAgentUser* registeredScadaUser);
      
        /**
          * Virtual destructor
          *
          * Delete should never be called directly. The deactivateAndDeleteServant
          * method inherited from ServantBase should be used instead.
          */
        virtual ~ScadaAgent();

		/** 
		  * notifyGroupOffline
		  *
		  * Used by the Process Monitor to inform an agent that
		  * a group is now offline.
		  */
        void notifyGroupOffline( const std::string& group);

		/** 
		  * notifyGroupOnline
		  *
		  * Used by the Process Monitor to inform an agent that
		  * a group is now online.
		  */
        void notifyGroupOnline( const std::string& group);
		virtual void agentTerminate();
        virtual bool createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                        const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                        TA_Base_Bus::IEntityList& createdEntities );
        virtual void entityCreationComplete();
		virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr EntityData);
		virtual void agentSetMonitor();
		virtual void agentSetControl();
		virtual void agentSetStandby();
		virtual void registerForStateUpdates();
		virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);
		virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);
		void registerForControlMessages();
		void registerForMonitorMessages();
		void submitAuditMessage(const TA_Base_Core::MessageType&);
		//
		// operations required by IDataNodeAgent & IDataPointAgent interface
		//
		virtual TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey );
        virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );
		virtual TA_Base_Bus::DataNode * getDataNode( unsigned long entityKey );
	    virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList );

		virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();
		virtual void beforeGetDescendantsOfAgent(TA_Base_Core::IEntityDataPtr agentEntity);

		void initilizeCache(TA_Base_Core::IEntityDataPtr agentEntity);
		
		void initializeDPAfterProxyReady();
		void changeDataPointOperationMode(TA_Base_Core::EOperationMode operationMode);
		void changeDataNodeOperationMode(TA_Base_Core::EOperationMode operationMode);
		void startScadaAgent();
		TA_Base_Core::IEntityDataPtr getAgentEntityConfiguration();
		virtual void checkOperationMode();
		//Functions for new failover mechanism
  //      TA_Base_Bus::IDataUpdateCorbaDef_ptr getDataUpdateServant();
		//void registerMonitorAgent(const std::string& uuid, TA_Base_Bus::IDataUpdateCorbaDef_ptr monitorAgentStateUpdateRef);
		//TA_Base_Bus::IDataNodeUpdateCorbaDef_ptr getDataNodeUpdateServant();
		//void registerMonitorAgent(const std::string& uuid, TA_Base_Bus::IDataNodeUpdateCorbaDef_ptr monitorAgentStateUpdateRef);

	// operations
	private:
		TA_Base_Bus::IEntity* createDataPointEntity ( TA_Base_Core::IEntityDataPtr EntityData );
		void setScadaInterfaceOperationMode(TA_Base_Core::EOperationMode mode);

	//attributes
	private:

        // Disable the copy constructor and assignment operator
        ScadaAgent( const ScadaAgent& theGenericAgent);
        ScadaAgent& operator=(const ScadaAgent &);

		unsigned long 					m_scadaAgentPrimaryKey;
        TA_Base_Core::EOperationMode	m_operationMode;
		bool 							m_bulkInterfaceStarted;
		std::string 					m_agentName;

		std::map< unsigned long, TA_Base_Bus::DataPoint * > m_datapoints;

		TA_Base_Core::ReEntrantThreadLockable		m_dataPointListtLock;
		TA_Base_Bus::GenericAgent *					m_genericAgent;
		TA_Base_Core::AuditMessageSender *			m_auditMessageSender;
        TA_Base_Bus::DataPointAgentCorbaDef_Impl *	m_dpBulkPolledInterface;
        TA_Base_Bus::DataPointAccessCorbaDef_Impl * m_dpAccessInterface;
        TA_Base_Bus::DataPointFactory *				m_dataPointFactory;
		TA_Base_Bus::ScadaProxyFactory *			m_scadaProxyFactory;
		
		TA_Base_Bus::DataNodeAgentCorbaDef_Impl *	m_dnBulkPolledInterface;
		TA_Base_Bus::DataNodeAccessCorbaDef_Impl *	m_dnAccessInterface;
		TA_Base_Bus::SummaryProcessor*				m_summaryProcessor;
		TA_Base_Bus::ScadaPersistencePtr			m_persistence;
		TA_Base_Core::ReEntrantThreadLockable		m_dataNodeListLock;
		DataNodeList								m_dataNodes;
		AgentInitThread*							m_agentInitThread;
        IScadaAgentUser* 							m_regScadaUser;

		//To fasten failover process w.r.t. datapoints updation.
        //DataNodeUpdateProcessor*  m_dnUpdateProcessor;
        //DataPointUpdateProcessor*  m_dpUpdateProcessor;

		//To fasten failover process w.r.t. datanodes updation. 
		//May be need a lock for below members so that 2 updateProcessors do not use at the same time.

        //DataUpdateSender*     m_UpdateSender;
        //DataUpdateServant*    m_UpdateServant;

		//various for ScadaAgent other businesses
		bool										m_hasOtherBusiness;		
    };
}

#endif // !defined(SCADAAGENT)
