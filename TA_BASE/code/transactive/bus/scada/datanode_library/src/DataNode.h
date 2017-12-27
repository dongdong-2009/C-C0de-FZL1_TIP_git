/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datanode_library/src/DataNode.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  *	DataNode is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * DataNode also realises the DataNodeCorbaDef interface,
  * exposing the DataNode's configuration to remote 
  * clients.
  *
  *
  * Modified: Daniel Hoey : 11/11/02 : Added isVirtual() method
  */

#ifndef DATANODE_H
#define DATANODE_H

#include <string>
#include <vector>

#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datanode_library/src/DataNodeTypes.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_Core
{
	class DataNodeEntityAccess;
	class CommsMessageSender;
}

namespace TA_Base_Bus
{
	class ScadaProxyFactory;
	class IDataNodeObserverNotification;

	enum E_DN_CALC_TYPE_FROM_PARENT
	{
		E_DN_CALC_MIN_FP = 0,
		E_DN_CALC_COMBINE_MMS_I_FP = 0,
		E_DN_CALC_MAX_FP
	};

	enum E_DN_CALC_TYPE_FROM_CHILD
	{
		E_DN_CALC_MIN_FC = 0,
		E_DN_CALC_QUALITY_FC = 0,
		E_DN_CALC_ALARM_FC,
		E_DN_CALC_ACKSTATE_FC,
		E_DN_CALC_ALARM_I_FC,
		E_DN_CALC_MMS_I_FC,
		E_DN_CALC_SCAN_I_FC,
		E_DN_CALC_CTRL_I_FC,
		E_DN_CALC_LAST_OP_INFO_FC,
		E_DN_CALC_CRTL_PENDING_FC,
		E_DN_CALC_OVERRIDE_FC,
		E_DN_CALC_MAX_FC
	};

    class DataNode : public IEntityUpdateEventProcessor,
					 public TA_Base_Bus::IEntity
	{

	public:


		/**
  		 * DataNode::DataNode()
  		 *
  		 * DataNode, initialising all basic configuration items.
		 *
		 * @param	DataNodeEntityAccess	Reference to db entity access
		 *
  		 */
		DataNode( TA_Base_Core::DataNodeEntityDataPtr dataNodeEntityAccess, ScadaPersistencePtr persistence,unsigned long scadaRootkey = ULONG_MAX );

		virtual ~DataNode();	

		// ========================================================================
		// IEntity methods - see IEntity.h 
		//
        bool isValid();
		void start();
		void stop();
		void update(const TA_Base_Core::ConfigUpdateDetails& updateEvent);
		void remove();
		void setToControlMode();
		void setToMonitorMode();

		// ========================================================================	
		// Implementation of IEntityUpdateEventProcessor
		void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType);

		//
		// Implementation to support DataNode Agent BulkPolledInterface
		//
		virtual bool registerForUpdateDetails( TA_Base_Bus::IDataNodeObserverNotification * updateDetailsObserver );
		virtual bool deregisterForUpdateDetails( TA_Base_Bus::IDataNodeObserverNotification * updateDetailsObserver );
		virtual void notifyRegisteredObservers( boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails );
		
		//
		// access methods of DataNode
		//
	    /**
	      * getEntityKey
	      * 
	      * return the entitykey
	      * 
	      * @return unsigned long 
	      * 
	      */
		unsigned long getEntityKey() const;

	    /**
	      * getName
	      * 
	      * return the name of the datanode
	      * 
	      * @return const std::string& 
	      * 
	      * @exception <exceptions> Optional
	      */
        const std::string& getName() const;

	    /**
	      * getCompleteStaticConfiguration
	      * 
	      * return the complete static configuration
	      * 
	      * @return TA_Base_Bus::DataNodeStaticConfiguration& 
	      * 
	      */
        boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> getCompleteStaticConfigurations();

	    /**
	      * getCompleteDynamicStates
	      * 
	      * return the complete dynamic states
	      * 
	      * @return TA_Base_Bus::DnCompleteDynamicSyncState& 
	      * 
	      */
        boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> getCompleteDynamicStates();

	    /**
	      * getInhibitMMS
	      * 
	      * get the Inhibit MMS state
	      * 
	      * @return virtual bool 
	      * 
	      */
		virtual bool getInhibitMMS();
        
		/**
	      * getCombinedInhibitMMS
	      * 
	      * get the combined Inhibit MMS state
	      * 
	      * @return BooleanTag
	      * 
	      */
		virtual BooleanTag getCombinedInhibitMMS();
	    /**
	      * getDisplayState
	      * 
	      * <description>
	      * 
	      * @return virtual boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> getDisplayState();

        /**
	      * setOperationMode
	      * 
	      * set the operation mode of the datanode. This must be called by the datanode agent
	      * 
	      * @return virtual void 
	      * @param : bool inControlMode
	      * 
	      */
        virtual void setOperationMode( bool inControlMode );

	    /**
	      * changeDataNodeTag
	      * 
	      * change the datanode tag
	      * 
	      * @return virtual void 
	      * @param : bool inControlMode
	      * 
	      */
        void changeDataNodeTag( const std::string& sessionId, 
                                TA_Base_Bus::ETagState tagState,
                                TA_Base_Bus::EOutputInhibitState tagOutputInhibitState,
                                TA_Base_Bus::EInputInhibitState tagInputInhibitState,
                                const std::string& p_ptwId, const std::string& ptwComment);


	    /**
	      * setNotes
	      * 
	      * set the datanode notes
	      * 
	      * @return void 
	      * @param : const std::string& sessionId
	      * @param : const std::string& notes
	      * 
	      */
        void setNotes(const std::string& sessionId, const std::string& notes);

	    /**
	      * setInhibitMMS
	      * 
	      * set the inhibit MMS state
	      * 
	      * @return virtual void 
	      * @param :  const char * sessionId
	      * @param : bool inhibitMms
	      * 
	      */
        virtual void setInhibitMMS( const std::string& sessionId, bool inhibitMms );

		//
		// operations specific to DataNode
		//
		void updateActivation( void );

        //
        // Summary Queue Processor
        //
        void processUpdateActivation();

		void processRecalculateRequestFromParent();

		void processRecalculateRequestFromChildren();
        //******************************************
        // Synchronization method for the datanode
        //******************************************
	    /**
	      * updateSynState
	      * 
	      * used by the monitor mode datapoint to synchronize the internal dynamic state with
          * the control mode datapoint
	      * 
	      * @return 
	      * @param : TA_Base_Bus::DnSyncUpdate toupdate
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void updateSynState(const TA_Base_Bus::DnSyncUpdate& toupdate);
		void synTagState( TagState inTagState, const std::string& sessionId);
		void synNotesUpdate(const std::string& notes);
		void synInhibitMMSUpdate(bool inputInhibitMMSState, const std::string & sessionId);
		void syncInhibitDataPoints(TA_Base_Bus::EOutputInhibitState tagOutputInhibitState, 
							   TA_Base_Bus::EInputInhibitState tagInputInhibitState,
							   const std::string& sessionIdStr);

		TA_Base_Core::DataNodeEntityData::ESynthesisType getSynthesisTypeEnum();

		unsigned long getParentKey();

		bool changeOperationMode( TA_Base_Core::EOperationMode operationMode ); 

		unsigned long getScadaRootKey();
    protected:

        /**
	      * readStaticConfiguration
	      * 
	      * read the configuration from the database
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
		void readStaticConfiguration( );

	    /**
	      * updateConfiguration
	      * 
	      * update the configuration 
	      * 
	      * @return void 
	      * 
	      */
		void updateStaticConfiguration( );

	    /**
	      * readDynamicState
	      * 
	      * read dynamic state of the datanode
	      * 
	      * @return void 
	      * 
	      */
		void readDynamicState();

	    /**
	      * clearSummaries
	      * 
	      * clear the summaries
	      * 
	      * @return void 
	      * 
	      */
        void clearSummaries();

		/**
	      * initCombinedState()
	      * 
	      * init those combined state
	      * 
	      * @return void 
	      * 
	      */
		void initCombinedState();


	    /**
	      * destroyProxies
	      * 
	      * destroy child datapoint and datanode proxies
	      * 
	      * @return void 
	      * 
	      */
		void destroyProxies();
		
		// ========================================================================	
		// recalculate those summary information
		void recalculateScanInhibitSummary( void );
		void recalculateAlarmInhibitSummary( void );
		void recalculateMmsAlarmInhibitSummary( void );
		void recalculateControlInhibitSummary( void );
		void recalculateAlarmSummary( void );
		void recalculateAlarmAckSummary( void );
		void recalculateOverrideSummary( void );
		void recalculateQualitySummary( void );
		void recalculateControlPendingSummary( void );
		void recalculateLastOperatorInfo( void );

		// recalculate those combined state
		void recalculateCombinedInhibitMMS();

	    /**
	      * isActionPermitted
	      * 
	      * check the rights
	      * 
	      * @return bool 
	      * @param :  const std::string & sessionID
	      * @param : unsigned long actionKey
	      * 
	      */
		bool isActionPermitted( const std::string & sessionID, unsigned long actionKey );
	
		/**
		 * DataNode::submitAuditEvent()
		 *
		 * Generates a log-only event.
		 *
		 */
		void submitAuditEvent( const std::string & sessionID,
							   const TA_Base_Core::DescriptionParameters & description,
							   const TA_Base_Core::MessageType & messageType );

		/**
  		 * DataNode::submitUpdateEvent()
  		 *
  		 * Submits a new item to the EventQueue, requesting the 
		 * EventProcessor to generate an event describing
		 * the DataNode's current state.
		 *
  		 */
		void submitUpdateEvent(ScadaEntityUpdateType type);

		/**
  		 * DataNode::submitSummaryUpdateEvent()
  		 *
  		 * Submits a new item to the EventQueue, requesting the 
		 * EventProcessor to generate an event describing
		 * the DataNode's current state.
		 *
  		 */
		void submitSummaryUpdateEvent(ScadaEntityUpdateType updateType);

		/**
  		 * DataNode::submitCombinedUpdateEvent()
  		 *
  		 * Submits update to its observers,if its combined state change  
		 *
  		 */
		void submitCombinedUpdateEvent(ScadaEntityUpdateType updateType);

		// ========================================================================
		// Inhibit related operation
		//
		void inhibitDataPoints(TA_Base_Bus::EOutputInhibitState tagOutputInhibitState, 
							   TA_Base_Bus::EInputInhibitState tagInputInhibitState,
							   const std::string& sessionIdStr);
		void setOutputInhibitStatus(TA_Base_Bus::DataPointAgentInterface& dataPointNamedObject,
                                    unsigned long entitykey,
									  TA_Base_Bus::EOutputInhibitState currentInhibit,
									  TA_Base_Bus::EOutputInhibitState newInhibit,
									  std::string sessionId);
		void setInputInhibitStatus(TA_Base_Bus::DataPointAgentInterface& dataPointNamedObject,
                                    unsigned long entitykey,
									 TA_Base_Bus::EInputInhibitState currentInhibit,
									 TA_Base_Bus::EInputInhibitState newInhibit, 
									 std::string sessionId);
		unsigned short calculateFlexibleTimeout(); 


		// ========================================================================
		// Operation mode related methods  
		//
		TA_Base_Core::EOperationMode getOperationMode();
		void checkOperationMode();
		void initCalcFlags();
		
	protected:

		// Configuration
        TA_Base_Bus::DataNodeStaticConfiguration	m_staticConfig;
		
		//Dynamic information
		TA_Base_Bus::DnCompleteDynamicSyncState	    m_dynamicState;

		// Child proxies
		TA_Base_Bus::DataNodeChildProxies			m_childProxies;

		// Flexible Timeout
		//unsigned short						        m_flexibleTimeout; 

		// Internal stuff...
		TA_Base_Core::EOperationMode		        m_operationMode;

        TA_Base_Core::DataNodeEntityDataPtr         m_dataNodeEntityAccess;

		TA_Base_Bus::ScadaProxyFactory&			    m_scadaProxyFactory;

		SummaryProcessor*					        m_summaryQ;

		TA_Base_Core::AuditMessageSender*		    m_auditSender;

		std::vector < TA_Base_Bus::IDataNodeObserverNotification * > m_listOfUpdateDetailsObservers;
		TA_Base_Core::ReEntrantThreadLockable		m_updateDetailsObserverlock;
		TA_Base_Core::ReEntrantThreadLockable		m_childproxieslock;
		TA_Base_Core::ReEntrantThreadLockable		m_scadaRootKeyLock;
		TA_Base_Bus::DataNodeProxySmartPtr*			m_parentProxy;
		unsigned long								m_parentKey;
		bool										m_childProxiesCreated;

		typedef void (DataNode::*RecalculateFunc) (void);
		bool										m_calcFlagsFromParent[E_DN_CALC_MAX_FP];
		static const RecalculateFunc				s_reCalculateFuncsFP[E_DN_CALC_MAX_FP];
		TA_Base_Core::ReEntrantThreadLockable		m_calcFlagsFromParentLock;

		bool										m_calcFlagsFromChild[E_DN_CALC_MAX_FC];
		static const RecalculateFunc				s_reCalculateFuncsFC[E_DN_CALC_MAX_FC];
		TA_Base_Core::ReEntrantThreadLockable		m_calcFlagsFromChildLock;
		
		static unsigned long						m_scadaRootKey;
		static unsigned long						m_parentEntityKey;
		
	private:

		bool isActivated();
        bool areChildProxiesCreated();

	    void saveLastOperatorInfo ( const std::string & sessionID );

        ScadaPersistencePtr m_persistence;

		//
		// Declare the copy constructor as private 
		// to prevent pass-by-value of this monolithic object
		//
		DataNode( const DataNode & );
	};
}
#endif
