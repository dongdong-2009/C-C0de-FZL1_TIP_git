/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTU.h $
  * @author:	Liu Yong
  * @version:	$Revision: #3 $
  *
  * Last modification:	$DateTime: 2017/05/25 10:19:08 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTU object manages a set of DataPoints that have been configured for it.
  *
  */

#ifndef RTU_H
#define RTU_H


#include <string>
#include <vector>

#include "bus/generic_agent/src/IEntity.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/rtu_library/src/StationSystemOutputWriter.h"
#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/message/src/NameValuePair.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class AuditMessageSender;
    class AlarmHelper;
    class MessageType;
}

namespace TA_Base_Bus
{
	class DataPoint;
	class DataPointWriteRequest;
    class IModbus;
    class BaseRTUManager;
    class RTUCommandWorker;
	class RTUPollingWorker;
    class RTUStatusWorker;
    class ModbusDpProcessor;

	typedef std::vector< TA_Base_Bus::StationSystemOutputWriter* > StationSystemOutputWriters;
 
	enum ModbusConnectionType
    {
        CONNTYPE_POLL,
        CONNTYPE_COMMAND
    };

    enum RTU_STATUS
    {
        RTU_COMMS_DOWN,
        RTU_SWITCHING,
        RTU_STANDBY,
        RTU_PRIMARY
    };

    class RTU : public TA_Base_Bus::IEntity
	{
	public:
		RTU ( BaseRTUManager& rtuManager, TA_Base_Core::RTUEntityDataPtr rtuEntityData );
		virtual ~RTU();
	
		// operations
		void updateCommsAlarm(  bool raiseAlarm, 	
								const std::string & servicePortNumber, 
								const std::string & additionalComment );

		void submitAuditMessage(const TA_Base_Core::DescriptionParameters & description,
								const TA_Base_Core::MessageType & messageType );

		RTU_STATUS getRTUStatus();

		virtual IModbus * createModbusConnection( const std::string & rtuServicePortNumber, 
												int slaveID, 
												ModbusConnectionType connectionType ) = 0;

		virtual void destroyModbusConnection( TA_Base_Bus::IModbus * & modbusConnection );

		//Used for modbus address adjustment.
		virtual unsigned long adjustAddress( unsigned long registerAddress ) = 0;

		virtual void createRTUWorkers() = 0;



		void createOutputWriter(const std::string & name,
								const std::string & outputServiceNumber,
								unsigned long outputStartAddress,
								unsigned long outputEndAddress,
								int slaveID,
								const std::string digitalWriteFunCode);

		/**
		* addDataPoint
		*
		* Add the specified DataPoint object to the list of data point
		* controlled and monitored by the RTU entity
		*
		* @param DataPoint	the DataPoint object that is monitored and controlled
		*						by the RTU entity
		*/
		bool addDataPoint( TA_Base_Bus::DataPoint * dp );

		/**
		* removeDataPoint
		*
		* remove the specified DataPoint object to the list of data point
		* controlled and monitored by the RTU entity
		*
		* @param DataPoint	the DataPoint object that is monitored and controlled
		*						by the RTU entity
		*/		
		void removeDataPoint( TA_Base_Bus::DataPoint * dp );

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
		virtual bool isValid() {return true;};

		/**
		* start
		*
		* This is a pure vitual method that must be implemented by 
		* all enitity classes. It will start any object specific tasks,
		* such as creating its own thread.
		*/
		virtual void start() {};

		/**
		* stop
		*
		* This is a pure vitual method that must be implemented by all
		* enitity classes. It will stop any object specific tasks, such 
		* as stopping its own thread. 
		*/
		virtual void stop() {};

		/**
		* update
		*
		* This method is called by the agent controlling the entity to ask 
		* the entity to update its configuration. Called after an update 
		* configuration event is received.
		*/
		virtual void update(const TA_Base_Core::ConfigUpdateDetails& updateEvent) {};

		/**
		* remove
		*
		* When a configuration update event is received to delete an entity 
		* from an agent, the agent can call this method to tell the entity 
		* to commence any clean up opperations.
		* 
		* The agent then still has to remove the object from its vector of entities.
		*/
		virtual void remove() {};

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
		// specific to RTU
		//

		/**
		* getName
		*
		* Get the name of the RTU entity
		*
		*/
		const std::string & getName() const;

		/**
		* getAddress
		*
		* Get the address of the RTU entity
		*
		*/
		const std::string & getAddress() const;

		/**
		* getPKey
		*
		* Get the pkey of the RTU entity
		*
		*/
		unsigned long getPKey() const;

		/**
		* getTypeKey
		*
		* Get the type key of the RTU entity
		*
		*/
		unsigned long getTypeKey() const;

		/**
		* getLocationKey
		*
		* Get the location key of the RTU entity
		*
		*/
		unsigned long getLocationKey() const;

		/**
		* getSubsystemKey
		*
		* Get the sub system key of the RTU entity
		*
		*/
		unsigned long getSubsystemKey() const;

		/**
		* getSlaveID
		*
		* Get the slave id of the RTU entity
		*
		*/
		int getSlaveID() const;

		/**
		* isInService
		*
		* Get the indication if the RTU entity is in service or not.
		* Returns true if the RTU entity is in service.
		*
		*/
		bool isInService() const;
		
		bool getIsInControlMode();
		int  getMaxCommsRetries() const;


		// update operations;
		void updateRTUConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent);
		virtual void updateInServiceParameter( bool newState );
		virtual void updateScanTimeMSecs( int scanTimeMSecs );
		virtual void updateCommsErrorRetries( int retries );
		virtual void updatePollTimeout( int pollTimeout );
		virtual void updateCommandTimeout( int commandTimeout ); 		

		virtual void updateAnalogueDpUpdateInterval(int analogueDpUpdateInterval);

		// rtustatusworker;
		virtual bool getIsCommsOK() const = 0;

		// rtu process operations.
		virtual void processPollingData(const TA_Base_Bus::DataBlock< WORD > & addressBlock) = 0;
		virtual bool processWriteRequest( TA_Base_Bus::DataPointWriteRequest & newWrite ) = 0;

		virtual bool processSendingData( TA_Base_Bus::DataBlock < WORD > & commandValues, 
										std::string& rtuServicePortNumber,
										int slaveID );

		virtual bool processSendingData( TA_Base_Bus::DataBlock < bool > & commandValues, 
										std::string& rtuServicePortNumber,
										int slaveID );

		unsigned long getPollingStartAddress() { return m_rtuEntityData->getRTUPollingTableStartAddress();};
		unsigned long getPollingEndAddress() { return m_rtuEntityData->getRTUPollingTableEndAddress();};
		int 		  getGroupID() { return m_rtuEntityData->getGroupID(); };


		// operations
	protected:

		bool getAreAddressesValid( unsigned long startAddress, unsigned long endAddress );
		virtual void startProcessing() = 0;
		virtual void stopProcessing()  = 0;
		
		// operations
	private:
		// disabled contructors
		RTU();
		RTU( const RTU & obj );
		void operator= ( const RTU & rhs );

		// attributes
	protected:

		BaseRTUManager& 				m_rtuManager;
		// cache rtu entitydata
		TA_Base_Core::RTUEntityDataPtr	m_rtuEntityData;

		std::string 	m_name;
		std::string 	m_address;
		unsigned long 	m_primaryKey;
		unsigned long 	m_typeKey;
		unsigned long 	m_locationKey;
		unsigned long 	m_subsystemKey;
		bool 			m_inService;
		bool 			m_isInControlMode;
		bool 			m_pollingStarted;
		bool 			m_commsErrorAlarmGenerated;

		bool 			m_tcpNoDelay;
		int 			m_slaveID;
		int 			m_commsErrorRetries;
		int 			m_pollTimeout;   
		int 			m_commandTimeout;

		int 			m_analogueDpUpdateInterval;
		
		TA_Base_Bus::EQualityStatus	m_qualityStatus;
		StationSystemOutputWriters	m_outputWriters;

		RTUStatusWorker* 			m_rtuStatus;
		ModbusDpProcessor* 			m_rtuInternalDpProcessor;

		TA_Base_Core::AuditMessageSender *      m_auditMessageSender;
		TA_Base_Core::ReEntrantThreadLockable   m_alarmGenereatedLock;
	};
}
#endif
