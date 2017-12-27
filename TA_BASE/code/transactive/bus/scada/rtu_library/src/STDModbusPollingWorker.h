/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/bus/scada/rtu_library/src/STDModbusPollingWorker.h $
  * @author:	Liu Yong
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The STDModbusPollingWorker object is responsible for polling input data with the standard modbus protocol.
  *
  */

#ifndef STDModbusPollingWorker_H
#define STDModbusPollingWorker_H

#include <string>
#include <vector>

#include "bus/modbus_comms/src/DataBlock.h"
#include "core/sockets/src/SocketOptions.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/rtu_library/src/TimerWorker.h"
#include "bus/scada/rtu_library/src/CommandQueue.h"
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
    class IModbus;
    class ModbusException;
    class StandardRTU;	
    
	class STDModbusPollingWorker : public TA_Base_Bus::TimerWorker
	{
	// operations
	public:
	    STDModbusPollingWorker( StandardRTU & rtu,
						  unsigned long startInputAddress,
                          unsigned long endInputAddress,
						  const std::string & rtuIpaddress,
  						  const std::string & rtuServicePortNumber,
                          int slaveID,
						  int commsErrorRetries,
                          int maxModbusLength,
                          int interval,
  						  int pollTimeout,
  						  bool standardModbusLengthLimit,
						  bool overlapModbusRequest);        
                          
        virtual ~STDModbusPollingWorker();


		void postRequest(ModbusCommandPtr& cmd);

		//
		// operations to monitor RTU status
		//

		bool getIsCommsOK() const;

		/**
		 * updateCommsErrorRetries
		 *
		 * Updated the comms errors retries of this worker thread with the new value
		 *
		 * @param retries	The new comms error retries
		 *
		 */
		virtual void updateCommsErrorRetries ( int retries );

        /**
		  * setInServiceState
		  *
		  * Set the worker thread in or out of service.  A worker thread that is out of service
		  * would close down the ModbusTcpConnection and not perform any task
		  *
		  * @param inService	Flag to indicate if the worker thread is in service or not
		  *
		  */
		virtual void setInServiceState ( bool inService );
		
	// operations
	protected:
	    virtual void process();

	private:
		void sendModbusCommand(ModbusCommandPtr& cmd);
        void readHoldingRegisters(DataBlock< WORD >& addressBlock);
        bool writeMultipleRegisters(DataBlock< WORD >& addressBlock);
        bool writeMultipleCoils(DataBlock< bool >& addressBlock);

    	
		/**
		 * processCommsErrorCounter
		 *
		 * Check the comms error counter and perform appropriated actions
		 *
		 */
		void processCommsErrorCounter();
		
		/**
		  * processPollingData
		  *
		  * Process the input data stored in the specified address block
		  *
		  * @param addressBlock	The address block to be processed by the polling worker thread
		  *
		  */
		void processPollingData( const TA_Base_Bus::DataBlock< WORD > & addressBlock );

		/**
		  * readStandardModbusDevice
		  *
		  * Read holding registers of a standard Modbus device
		  *
		  */
		//void readStandardModbusDevice();

        void CheckModbusException( const TA_Base_Bus::ModbusException & me );

        // disabled constructors
		STDModbusPollingWorker();
		STDModbusPollingWorker( const STDModbusPollingWorker & obj );
		STDModbusPollingWorker & operator= ( const STDModbusPollingWorker & rhs );

	// attributes
		StandardRTU& 					m_rtu;
		TA_Base_Bus::DataBlock< WORD > 	m_addressBlock;
        std::string 					m_rtuIpaddress;
        std::string 					m_rtuServicePortNumber;
       
        int 							m_slaveID;
        int 							m_commsErrorRetries;
		bool 							m_tcpNoDelay;
		int  							m_pollTimeout;   
		int 							m_interval;		
        bool 							m_standardModbusLengthLimit;
        int  							m_maxModbusLength;
		bool 							m_isFullPolling;
        TA_Base_Bus::EQualityStatus 	m_qualityStatus;
		bool 							m_overlapModbusRequest;

		TA_Base_Bus::ModbusCommandQueue		m_fastPollingQueue;
		TA_Base_Bus::ModbusCommandQueue		m_slowPollingQueue; 
		TA_Base_Bus::ModbusCommandQueue		m_writeQueue; 
        
        TA_Base_Bus::IModbus *  m_currentModbusConnection;

        volatile int 			m_commsErrorCounter;
        volatile bool 			m_commsError;	

        std::string m_readFunctionCode;
        TA_Base_Core::ReEntrantThreadLockable	m_lock;

	};
}

#endif
