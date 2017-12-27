/**
* The source code in this file is the property of Ripple Systems and is not for redistribution in
* any form.
*
* Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/StandardRTU.cpp $
* @author:	HoaVu
* @version:	$Revision: #8 $
*
* Last modification:	$DateTime: 2017/06/02 12:08:57 $
* Last modified by:	$Author: yong.liu $
*
* Description:
*
*	The StandardRTU object manages a set of DataPoints that have been configured for it.
*
*/

#include "bus/scada/rtu_library/src/StandardRTU.h"
#include "bus/scada/rtu_library/src/RTUPollingWorker.h"
#include "bus/scada/rtu_library/src/STDModbusPollingWorker.h"
#include "bus/scada/rtu_library/src/ModbusPollingScheduler.h"
#include "bus/scada/rtu_library/src/RTUStatusWorker.h"
#include "bus/scada/rtu_library/src/StationSystemOutputWriter.h"
#include "bus/scada/rtu_library/src/StandardRTUManager.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/utilities/src/RunParams.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "bus/modbus_comms/src/DuplexModbusTcpConnection.h"
#include "bus/scada/rtu_library/src/CommonDpProcessor.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

namespace TA_Base_Bus
{
	StandardRTU::StandardRTU( StandardRTUManager& rtuManager, TA_Base_Core::RTUEntityDataPtr rtuEntityData )
		:
		RTU(rtuManager, rtuEntityData),
		m_stdModbusPolling(NULL),
		m_onePortOneConnection(true),
		m_overlapModbusRequest(rtuManager.getOverlapModbusRequest()),
		m_standardModbusLengthLimit(rtuManager.getStandardModbusLengthLimit()),
		m_maxModbusLength(rtuManager.getMaxModbusLength()),
		m_interval(rtuManager.getIntervalBetweenModbusTransaction()),
		m_enableWrite(rtuManager.getIsWriteEabled()),
		m_digitalWriteFunctionCode(rtuManager.getdigitalWriteFunctionCode())
	{
		std::string strPollingPort = m_rtuEntityData->getRTUPollingTableServiceNumber();
		std::string strCommandPort = m_rtuEntityData->getRTUCommandTableServiceNumber();
		if (strPollingPort.compare(strCommandPort) == 0)
		{
			std::string parameterStr = TA_Base_Core::RunParams::getInstance().get("OnePortOneConnection");
			if(0 != parameterStr.compare("1"))
			{
				m_onePortOneConnection = false;
			}
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
			"Creating StandardRTU[%s], PollingTablePortNumber[%s], CommandTablePortNumber[%s], onePortOneConnection:[%s]",
			getName().c_str(), 
			m_rtuEntityData->getRTUPollingTableServiceNumber().c_str(), 
			m_rtuEntityData->getRTUCommandTableServiceNumber().c_str(), 
			m_onePortOneConnection?"Yes":"No");

	}


	StandardRTU::~StandardRTU()
	{
		ItPollingScheduler itPollingScheduler; 
		for ( itPollingScheduler  = m_pollingSchedulers.begin(); 
			  itPollingScheduler != m_pollingSchedulers.end(); 
			  ++itPollingScheduler)
		{
			delete *itPollingScheduler;
		}

		if (NULL != m_stdModbusPolling)
		{
			delete m_stdModbusPolling;
		}
		
		{
			TA_Base_Core::ThreadGuard guard(m_connectionLock);
			std::map<std::string, TA_Base_Bus::IModbus*>::iterator itConn;
			for (itConn = m_connections.begin(); itConn != m_connections.end(); ++itConn)
			{
				delete itConn->second;
				itConn->second = NULL; 
			}
			m_connections.clear();
			m_connReferences.clear();
		}
	}

	unsigned long StandardRTU::adjustAddress(unsigned long registerAddress)
	{
		// Standard Modbus device is 0-based address
		return registerAddress - 1;
	}	

	void StandardRTU::createPollingScheduler(unsigned long startPollingAddress,
								unsigned long endPollingAddress,
								int pollTimeout,
								TA_Base_Bus::ECommandType commandType)
	{
		ModbusPollingScheduler* pollingScheduler = NULL; 
		pollingScheduler = new ModbusPollingScheduler(startPollingAddress,
												  endPollingAddress,
												  pollTimeout,
												  commandType,
												  m_stdModbusPolling);
												  
		m_pollingSchedulers.push_back(pollingScheduler);
	}
	
	
	void StandardRTU::createRTUWorkers()
	{
		// if polling thread already exist, must be called more than once
		TA_ASSERT(NULL == m_stdModbusPolling, "createRTUWorkers can be only called once");

		std::string strPollingPort = m_rtuEntityData->getRTUPollingTableServiceNumber();
		std::string strOutputPort  = m_rtuEntityData->getRTUCommandTableServiceNumber();
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating polling worker for StandardRTU [%s]...", getName().c_str());
		// create StandardModbus polling worker
		m_stdModbusPolling = new STDModbusPollingWorker(*this,
			m_rtuEntityData->getRTUPollingTableStartAddress(),
			m_rtuEntityData->getRTUPollingTableEndAddress(),
			m_address,
			m_rtuEntityData->getRTUPollingTableServiceNumber(),
			m_slaveID,
			m_commsErrorRetries,
			m_maxModbusLength,
			m_interval,
			m_pollTimeout,
			m_standardModbusLengthLimit,
			m_overlapModbusRequest);
	
		//Only offset greater than 0 can have its own internalDpProcessor.
		if (0 < m_rtuEntityData->getRTUStatusOffsetAddress())
		{
			m_rtuInternalDpProcessor = new CommonDpProcessor(
			m_name + "InternalDpProcessor",
			m_rtuEntityData->getRTUPollingTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
			m_rtuEntityData->getRTUPollingTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
			m_rtuEntityData->getRTUStatusOffsetAddress(),
			false,
			m_analogueDpUpdateInterval);
    	}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating Standard Modbus Polling Worker");
		
		if (true == m_enableWrite)
		{
			StationSystemOutputWriter* writer = NULL;

			writer = new StationSystemOutputWriter(*this, 
			getName(),
			m_rtuEntityData->getRTUCommandTableServiceNumber(),
			m_rtuEntityData->getRTUCommandTableStartAddress(),
			m_rtuEntityData->getRTUCommandTableEndAddress(),
			m_rtuEntityData->getSlaveID(),
			m_digitalWriteFunctionCode,
			m_rtuEntityData->getRTUStatusOffsetAddress());					
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating internal output writer for StandardRTU [%s]", getName().c_str());

			m_outputWriters.push_back(writer);

		}
	}	

	void StandardRTU::processPollingData(const TA_Base_Bus::DataBlock< WORD > & addressBlock)
	{
		// process rtu status
		if (m_stdModbusPolling != NULL)
		{
			if (NULL != m_rtuStatus)
			{
				m_rtuStatus->processRTUDataBlock(addressBlock, m_stdModbusPolling->getIsCommsOK());
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "No Status Worker for StandardRTU [%s]", getName().c_str());
				return;
			}

			if (m_stdModbusPolling->getIsCommsOK())
			{
				m_qualityStatus = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
			}
			else if (TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != m_qualityStatus)
			{
				m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
			}
		}

		if (NULL != m_rtuInternalDpProcessor)
		{
			m_rtuInternalDpProcessor->processDataBlock(addressBlock, m_qualityStatus);
		}

		// notify holder to process polling datablock
		m_rtuManager.processRTUDataBlock(*this, addressBlock);
	}

	bool StandardRTU::processWriteRequest( TA_Base_Bus::DataPointWriteRequest & newWrite )
	{
		RTU* rtu = this;
		if (rtu != m_rtuManager.getPrimaryRTU() )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTU %s is not primary rtu, no need to process writerequest", getName().c_str());
			return false;
		}

		bool isProcessed = false;

		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
			isProcessed == false && itr != m_outputWriters.end();
			itr++)
		{
			// if station system slaveId equil to rtu's slaveId, then should process write request
			// else only process write request when this rtu is primary
			if ((*itr)->getSlaveID() == m_slaveID)
			{
				isProcessed = (*itr)->processWriteRequest(newWrite);
			}
		}

		return isProcessed;
	}	

	bool StandardRTU::processSendingData(TA_Base_Bus::DataBlock < WORD > & commandValues, 
										std::string& rtuServicePortNumber,
										int slaveID )
	{
		if (rtuServicePortNumber.compare(m_rtuEntityData->getRTUPollingTableServiceNumber()) == 0)
		{
			//use the polling modbusconnection to send the data.
			ModbusCommandPtr cmd(new ModbusCommand(&commandValues));
			m_stdModbusPolling->postRequest(cmd);
			return cmd->waitResult();
		}
		else
		{
			//create a new  modbusconnection to send the data.
			return RTU::processSendingData(commandValues,rtuServicePortNumber,slaveID);
		}
	}

	bool StandardRTU::processSendingData(TA_Base_Bus::DataBlock < bool > & commandValues, 
										std::string& rtuServicePortNumber,
										int slaveID )
	{
		if (rtuServicePortNumber.compare(m_rtuEntityData->getRTUPollingTableServiceNumber()) == 0)
		{
			//use the polling modbusconnection to send the data.
			ModbusCommandPtr cmd(new ModbusCommand(&commandValues));
			m_stdModbusPolling->postRequest(cmd);
			return cmd->waitResult();
		}
		else
		{
			//create a new  modbusconnection to send the data.
			return RTU::processSendingData(commandValues,rtuServicePortNumber,slaveID);
		}		
	}

	bool StandardRTU::getIsCommsOK() const
	{
		TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in StandardRTU::getIsCommsOK");
		return m_rtuStatus->getIsCommsOK();
	}
	
	TA_Base_Bus::IModbus * StandardRTU::createModbusConnection( const std::string & rtuServicePortNumber, int slaveID, ModbusConnectionType connectionType )
	{
		if ( m_onePortOneConnection )
		{
			return getModbusConnection(rtuServicePortNumber, slaveID, connectionType);
		}
		else
		{
			TA_Base_Bus::IModbus * modbusConnection = NULL;

			//
			// create and enable TCP Modbus Connection, either STE specific device or
			// standard TCP Modbus device, depending on runtime parameter
			//

			int timeout = 0;
			std::string connType;
			switch(connectionType)
			{
			case CONNTYPE_COMMAND:
				timeout = m_commandTimeout;
				break;

			case CONNTYPE_POLL:
			default:
				timeout = m_pollTimeout;
			}

			modbusConnection = new TA_Base_Bus::ModbusTcpConnection(m_address,
				rtuServicePortNumber,
				timeout,
				m_tcpNoDelay);

			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "StandardRTU::createModbusConnection - Timeout=%d",timeout);

			modbusConnection->setDeviceAddress(slaveID);
			modbusConnection->disable();

			return modbusConnection;
		}
	}

	IModbus* StandardRTU::getModbusConnection( const std::string & rtuServicePortNumber, int slaveID, ModbusConnectionType connectionType )
	{
		TA_Base_Bus::IModbus* modbusConn = NULL;
		TA_Base_Core::ThreadGuard guard(m_connectionLock);
		std::map<std::string, TA_Base_Bus::IModbus*>::iterator itConn = m_connections.find(rtuServicePortNumber);
		if ( itConn != m_connections.end() )
		{
			modbusConn = itConn->second;
			int references = m_connReferences[modbusConn] + 1;
			m_connReferences[modbusConn] = references;

 			int curSlaveID = modbusConn->getDeviceAddress();
			if (slaveID != curSlaveID)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "get exist connection but salve id different, current:%d, new:%d", curSlaveID, slaveID);
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "get exist connection, reference:%d", references);
			}
		}
		else
		{
			StandardRTUManager* standardRTUManager = dynamic_cast<StandardRTUManager* >(&m_rtuManager);
			int timeout = m_commandTimeout < m_pollTimeout ? m_commandTimeout : m_pollTimeout;
			if (standardRTUManager->getOverlapModbusRequest())
			{
	    		modbusConn = new TA_Base_Bus::DuplexModbusTcpConnection(m_address,
	    			rtuServicePortNumber,
	    			timeout,
	    			m_tcpNoDelay);
			}
			else
			{
				modbusConn = new TA_Base_Bus::ModbusTcpConnection(m_address,
					rtuServicePortNumber,
					timeout,
					m_tcpNoDelay);
			}
			modbusConn->setDeviceAddress(slaveID);
			m_connections[rtuServicePortNumber] = modbusConn;
			m_connReferences[modbusConn] = 1;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "RTU::createModbusConnection - Timeout=%d", timeout);
		}

		return modbusConn;
	}

	void StandardRTU::destroyModbusConnection( TA_Base_Bus::IModbus * & modbusConnection )
	{
		if ( !m_onePortOneConnection )
		{
			RTU::destroyModbusConnection(modbusConnection);
		}
		else
		{
			TA_Base_Core::ThreadGuard guard(m_connectionLock);
			int connReference = m_connReferences[modbusConnection] - 1;
			m_connReferences[modbusConnection] = connReference;
			if (connReference <= 0)
			{
				m_connReferences.erase(modbusConnection);
				std::map<std::string, TA_Base_Bus::IModbus*>::iterator itConn = m_connections.begin();
				for (; itConn != m_connections.end(); ++itConn)
				{
					if (itConn->second == modbusConnection)
					{
						m_connections.erase(itConn);
						break;
					}
				}
				RTU::destroyModbusConnection(modbusConnection);
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "connection reference 0, destroy it");
			}
			else
			{
				//don't need delete object, but we need set this pointer to NULL
				modbusConnection = NULL;
			}
		}
	}
	
	void StandardRTU::startProcessing()
	{
		TA_ASSERT(NULL != m_stdModbusPolling, "m_stdModbusPolling is NULL in StandardRTU::startProcessing");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Start the polling thread of RTU %s, IP address %s",
			m_name.c_str(), m_address.c_str());

		// only proceed if the RTU is in service
		if (false == m_inService)
		{
			m_pollingStarted = false;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Cannot start polling for status since RTU %s, IP address %s is not IN SERVICE.",
				m_name.c_str(), m_address.c_str());
			return;
		}

		// enable modbus connection
		m_stdModbusPolling->setInServiceState(true);

		for ( ItPollingScheduler itPollingScheduler  = m_pollingSchedulers.begin(); 
			  itPollingScheduler != m_pollingSchedulers.end(); 
			  ++itPollingScheduler)
		{
			(*itPollingScheduler)->setInServiceState(true);
		}

		m_pollingStarted = true;		
	}


	void StandardRTU::stopProcessing()
	{
		TA_ASSERT(NULL != m_stdModbusPolling, "m_stdModbusPolling is NULL in StandardRTU::stopProcessing");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Terminate the polling thread of RTU %s, IP address %s",
			m_name.c_str(), m_address.c_str());

		for (ItPollingScheduler itPollingScheduler  = m_pollingSchedulers.begin(); 
			  itPollingScheduler != m_pollingSchedulers.end(); 
			  ++itPollingScheduler)
		{
			(*itPollingScheduler)->setInServiceState(false);
		}

		m_stdModbusPolling->setInServiceState(false);

		m_pollingStarted = false;		
    }	

	bool StandardRTU::sendData(TA_Base_Bus::DataBlock < WORD > & commandValues)
	{
		std::string strCommandPort = m_rtuEntityData->getRTUCommandTableServiceNumber();
		return processSendingData(commandValues,strCommandPort,getSlaveID() );
	}

}
