#ifndef STANDARDRTU_H
#define STANDARDRTU_H


#include <string>

#include "bus/scada/rtu_library/src/RTU.h"
#include "bus/scada/rtu_library/src/StandardRTUManager.h"

namespace TA_Base_Bus
{
	const int MAX_STANDARD_MODBUS_LENGTH_LIMIT = 124;
	class ModbusPollingScheduler;
	class STDModbusPollingWorker;
	
    typedef std::vector< ModbusPollingScheduler* > PollingSchedulers;
    typedef std::vector< ModbusPollingScheduler* >::iterator ItPollingScheduler;
	
    class StandardRTU : public TA_Base_Bus::RTU
	{
	public:
		StandardRTU ( StandardRTUManager& rtuManager, TA_Base_Core::RTUEntityDataPtr rtuEntityData );
		virtual ~StandardRTU();

 		virtual unsigned long adjustAddress( unsigned long registerAddress );

		virtual IModbus* createModbusConnection( const std::string & rtuServicePortNumber, 
												int slaveID, 
												ModbusConnectionType connectionType );
		
		virtual void destroyModbusConnection( TA_Base_Bus::IModbus * & modbusConnection);
	
		virtual void createRTUWorkers();
		
		virtual void processPollingData(const TA_Base_Bus::DataBlock< WORD > & addressBlock);
  		virtual bool processWriteRequest( TA_Base_Bus::DataPointWriteRequest & newWrite );
 
 		virtual bool processSendingData( TA_Base_Bus::DataBlock < WORD > & commandValues, 
										std::string& rtuServicePortNumber,
										int slaveID );

		virtual bool processSendingData( TA_Base_Bus::DataBlock < bool > & commandValues, 
										std::string& rtuServicePortNumber,
										int slaveID );

 		virtual bool getIsCommsOK() const;

		void createPollingScheduler(unsigned long startPollingAddress,
                                    unsigned long endPollingAddress,
									int pollTimeout,
									TA_Base_Bus::ECommandType commandType);

		void setRTUStatus(RTUStatusWorker* rtuStatus) {m_rtuStatus = rtuStatus;};

		bool sendData(TA_Base_Bus::DataBlock < WORD > & commandValues);
									
	// operations
	protected:
		virtual void startProcessing();
		virtual void stopProcessing();

	private:
	
		IModbus* getModbusConnection( const std::string & rtuServicePortNumber, int slaveID, ModbusConnectionType connectionType );

		// disabled contructors
		StandardRTU();
		StandardRTU( const StandardRTU & obj );
		void operator= ( const StandardRTU & rhs );

		
	// attributes
	protected:
		STDModbusPollingWorker*	m_stdModbusPolling;
		PollingSchedulers		m_pollingSchedulers;

		std::string 			m_sendDataPointPortNumber;
		bool					m_onePortOneConnection;
		
		std::map<std::string, TA_Base_Bus::IModbus*> m_connections;
		std::map<TA_Base_Bus::IModbus*, int>         m_connReferences;

		TA_Base_Core::ReEntrantThreadLockable		 m_connectionLock;
		TA_Base_Core::ReEntrantThreadLockable		 m_pollingSchedulersLock;
		
		//Standard MODBUS configuration.
		bool	m_overlapModbusRequest;
		bool	m_standardModbusLengthLimit;
		int		m_maxModbusLength;
		int		m_interval;	
		bool	m_enableWrite;
        std::string    m_digitalWriteFunctionCode;
	};
}
#endif
