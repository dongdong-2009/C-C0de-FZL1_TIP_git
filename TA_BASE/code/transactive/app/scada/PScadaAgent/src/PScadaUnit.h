/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PScadaAgent/src/PScadaUnit.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The PScadaUnit object manages a set of DataPoints that have been configured for it.
  *
  */

#ifndef PSCADAUNIT_H
#define PSCADAUNIT_H


#include <string>
#include <vector>


#include "bus/scada/104_library/src/Iec104Api.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/scada/PScadaAgent/src/PScadaMsgTimer.h"
#include "app/scada/PScadaAgent/src/PScadaEventHandler.h"

namespace TA_Base_Bus
{
	class DataPoint;
	class DataPointWriteRequest;
}

namespace TA_Base_Core
{
    class AuditMessageSender;
    class AlarmHelper;
    class MessageType;
}

namespace TA_IRS_App
{
	class PScadaManager;
	class PScadaEventHandler;

	struct PScadaLinkStatus
	{
		int			nGroupID;
		std::string strIPAddress;
		bool		bLinkState;
		bool		bMasterConnection;
	};

    class PScadaUnit : public TA_Base_Bus::IEntity
	{
	public:

		PScadaUnit ( PScadaManager& pscadaManager, TA_Base_Core::RTUEntityDataPtr pscadaEntityData );

		virtual ~PScadaUnit();

	// operations	
	public:
		static void logRecord(int level,std::string &str);

		virtual void updateCommsAlarm( bool raiseAlarm, const std::string & servicePortNumber, const std::string & additionalComment );
		virtual void submitAuditMessage( const TA_Base_Core::DescriptionParameters & description,
                                         const TA_Base_Core::MessageType & messageType );

        void createPScadaWorkers();

        //
		// operations required by IEntity
		//

		/**
		  * isValid
		  *
		  * This is a pure vitual method that must be implemented by 
		  * all enitity classes. It will return true if the entity is 
		  * valid. Otherwise it will return false.
		  *
		  * @return TRUE if the entity is valid, FALSE otherwise 
		  */
		virtual bool isValid();

		/**
		  * start
		  *
		  * This is a pure vitual method that must be implemented by 
		  * all enitity classes. It will start any object specific tasks,
		  * such as creating its own thread.
		  */
		virtual void start();

		/**
		  * stop
		  *
		  * This is a pure vitual method that must be implemented by all
		  * enitity classes. It will stop any object specific tasks, such 
		  * as stopping its own thread. 
		  */
		virtual void stop();

		/**
		  * update
		  *
		  * This method is called by the agent controlling the entity to ask 
		  * the entity to update its configuration. Called after an update 
		  * configuration event is received.
		  */
		virtual void update(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		/**
		  * remove
		  *
		  * When a configuration update event is received to delete an entity 
		  * from an agent, the agent can call this method to tell the entity 
		  * to commence any clean up opperations.
		  * 
		  * The agent then still has to remove the object from its vector of entities.
		  */
		virtual void remove();

		/**
		  * setToControlMode
		  *
		  * This method can be called by the agent responsible for the entity in
		  * the event of its mode of operation changing to control. This enables the
		  * entity to perform tasks such as connecting to any field devices it is
		  * responsible for communicating with.
		  */
		virtual void setToControlMode();

		/**
		  * setToMonitorMode
		  *
		  * This method can be called by the agent responsible for the entity in
		  * the event of its mode of operation changing to monitor. This enables the
		  * entity to perform tasks such as disconnecting from any field devices it was
		  * communicating with.
		  */
		virtual void setToMonitorMode();

		//
		// specific to PScadaUnit
		//

		/**
		  * getName
		  *
		  * Get the name of the PScadaUnit entity
		  *
		  */
		const std::string & getName() const;

		/**
		  * getAddress
		  *
		  * Get the address of the PScadaUnit entity
		  *
		  */
		const std::string & getAddress() const;

		/**
		  * getPKey
		  *
		  * Get the pkey of the PScadaUnit entity
		  *
		  */
		unsigned long getPKey() const;

		/**
		  * getTypeKey
		  *
		  * Get the type key of the PScadaUnit entity
		  *
		  */
		unsigned long getTypeKey() const;

		/**
		  * getLocationKey
		  *
		  * Get the location key of the PScadaUnit entity
		  *
		  */
		unsigned long getLocationKey() const;

		/**
		  * getSubsystemKey
		  *
		  * Get the sub system key of the PScadaUnit entity
		  *
		  */
		unsigned long getSubsystemKey() const;

        int getSlaveID() const;

		/**
		  * isInService
		  *
		  * Get the indication if the PScadaUnit entity is in service or not.
		  * Returns true if the PScadaUnit entity is in service.
		  *
		  */


		/*
		* get the poll period for pscada data;
		*/
		int getPollAllDataTimeInSec();

		
		int getPollKwhDataTimeInSec();

		int getDataLostDurableTimeInsec();

		int getUpdateLostPeriodInSec();

		void setAllDataTransfered(bool isFinished);

		bool isAllDataTransfered();

		void setKwhDataTransfered(bool isFinished);

		bool isKwhDataTransfered();
	
        bool isInService() const;

		int getMaxCommsRetries() const;

		/**
		  * addDataPoint
		  *
		  * Add the specified DataPoint object to the list of data point
		  * controlled and monitored by the PScadaUnit entity
		  *
		  * @param theDataPoint	the DataPoint object that is monitored and controlled
		  *						by the PScadaUnit entity
		  */
		bool addDataPoint( TA_Base_Bus::DataPoint * dp );

		void removeDataPoint( TA_Base_Bus::DataPoint * dp );

        void updatePScadaUnitConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent);
        
        void updateInServiceParameter( bool newState );
		void updateScanTimeMSecs( int scanTimeMSecs );
		void updateCommsErrorRetries( int retries );
		//void updateCommandTimeout( int commandTimeout );  //wenching
		bool getIsInControlMode();
		//void updateDataPointWithBadQuality( bool toBeUpdatedWithBadQuality );
		//TD10586
		bool processWriteRequest( TA_Base_Bus::DataPointWriteRequest & newWrite );

		bool processPollKwhData();

        //Using for IEC 104 Protocol
		void addLink( TA_Base_Core::RTUEntityDataPtr pscadaEntityData );

		void setPScadaLinkConnectStatus(int nGroupID, std::string strIPAddress,bool bLinkState);
		void setPScadaLinkModeStatus(int nGroupID, std::string strIPAddress, bool bMasterLink);
		bool isMasterPScadaLink(int nGroupID, std::string& strIPAddress);
		bool isLinkConnected();
		void resetLink();
	// operations

	private:

		//void startProcessing();
		//void stopProcessing();
		void initializePScadaUnit();
		void addPScadaLink(int nGroupID, std::string strIPAddress);
        
        // disabled contructors
		PScadaUnit();
		PScadaUnit( const PScadaUnit & obj );
		void operator= ( const PScadaUnit & rhs );

	// attributes
	private:
		
        // cache PScadaUnit entitydata
        TA_Base_Core::RTUEntityDataPtr m_pscadaEntityData;
        
        std::string m_name;
		std::string m_address;
		unsigned long m_primaryKey;
		unsigned long m_typeKey;
		unsigned long m_locationKey;
		unsigned long m_subsystemKey;
		bool m_inService;
        bool m_isInControlMode;
		bool m_agentInControlMode;
		bool m_pollingStarted;
        bool m_commsErrorAlarmGenerated;
        bool m_tcpNoDelay;
		TA_Base_Bus::EQualityStatus m_qualityStatus;

		int m_slaveID;
		int m_commsErrorRetries;
        int m_pollTimeout;   //wenching
		int m_commandTimeout;   //wenching

		int m_pollAllDataTimeInSec;//jinmin++
		int m_pollKwhDataTimeInSec;//jinmin++
		// This time for connection lost
		int m_dataLostDurableTimeInsec;
		// This time for connection is ok, but no update from 104 Device
		int m_updateLostPeriodInSec;
		volatile bool m_isAllDataTransfered;//jinmin++
		volatile bool m_isKwhDataTransfered;//jinmin++

        int m_analogueDpUpdateInterval; // TD16169
        
        PScadaManager& m_pscadaManager;
		IEC104_API* m_104ProtocolInterface;
		bool		m_is104Started;
		int			m_pscadaCommonAddr;

		PScadaEventHandler* m_pscadaEventHandler;
		PScadaMsgTimer	   m_pscadaMsgTimer;

		static bool 	   m_stopProcessing;

        TA_Base_Core::AuditMessageSender *      m_auditMessageSender;

		typedef std::map < unsigned long, TA_Base_Bus::DataPoint * > PScadaDataPointMap;
		PScadaDataPointMap						m_OutputDataPoints;
		
        TA_Base_Core::ReEntrantThreadLockable   m_alarmGenereatedLock;

		std::vector<PScadaLinkStatus>			m_vLinkStatus;
	};
}
#endif
