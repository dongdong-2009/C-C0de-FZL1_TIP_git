/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/bus/scada/rtu_library/src/STDModbusPollingWorker.cpp $
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

#include <sys/timeb.h>

#include "bus/scada/rtu_library/src/STDModbusPollingWorker.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "bus/modbus_comms/src/DuplexModbusTcpConnection.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/IModbus.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/rtu_library/src/StandardRTU.h"

using TA_Base_Core::DebugUtil;

//TD15792, remove AvalancheHeaderDataPointTable processing codes
namespace TA_Base_Bus
{
	const int MAX_NUMBER_ERROR_RETRIES = 5;
    
	STDModbusPollingWorker::STDModbusPollingWorker( StandardRTU& rtu,
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
										bool overlapModbusRequest)
	:
    TA_Base_Bus::TimerWorker(interval),
	m_rtu( rtu ),
	m_addressBlock( startInputAddress, endInputAddress ),
	m_rtuIpaddress( rtuIpaddress ),
    m_rtuServicePortNumber( rtuServicePortNumber ),
    m_slaveID( slaveID ),
	m_commsErrorRetries( commsErrorRetries ),
	m_tcpNoDelay( true),
	m_pollTimeout( pollTimeout ),
	m_interval(interval),
    m_standardModbusLengthLimit( standardModbusLengthLimit ),
    m_maxModbusLength(maxModbusLength),
	m_overlapModbusRequest(overlapModbusRequest),
    m_currentModbusConnection( NULL ),
	m_isFullPolling(true),
	m_commsErrorCounter(0),
	m_commsError(false),
    m_readFunctionCode("04")
	{
		if ( 0 >= m_commsErrorRetries )
		{
			m_commsErrorRetries = MAX_NUMBER_ERROR_RETRIES;
		}
        
        // some cases use ReadHoldingRegister method read data with command "03",
        // most cases use ReadInputRegister method read data with command "04"
        if (TA_Base_Core::RunParams::getInstance().isSet("ReadHoldingRegister"))
        {
            m_readFunctionCode = "03";
        }
		
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, 
			"STDModbusPollingWorker startIOAddress = %d, endIOAddress = %d, standardModbusLengthLimit = %d, overlapModbusRequest = %d, commsErrorRetries = %d, maxModbusLength = %d, pollTimeout = %d, readFunctionCode = %s", 
			startInputAddress, endInputAddress, standardModbusLengthLimit, overlapModbusRequest, 
			commsErrorRetries, maxModbusLength, pollTimeout, m_readFunctionCode.c_str());
	}


	STDModbusPollingWorker::~STDModbusPollingWorker()
	{
        //TD13398, should stop this thread first, then stop the timers.
		// Ensure that STDModbusPollingWorker thread is stopped
        setInServiceState( false );
     
	}
	
	void STDModbusPollingWorker::updateCommsErrorRetries ( int retries )
	{
		// save the new retries
        m_commsErrorRetries = retries;
	}

	void STDModbusPollingWorker::process()
	{
		//boost::shared_ptr<Command> cmd;
		ModbusCommandPtr cmd;

		while ( m_writeQueue.isNotEmpty() || 
				m_slowPollingQueue.isNotEmpty() ||
				m_fastPollingQueue.isNotEmpty())
		{
			if (m_writeQueue.isNotEmpty())									
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Getting writeQueue Command.");
				cmd = m_writeQueue.getNextCommand();				
			}
			else if (m_slowPollingQueue.isNotEmpty())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Getting slowPollingQueue Command.");
				cmd = m_slowPollingQueue.getNextCommand();	
			}
			else if (m_fastPollingQueue.isNotEmpty())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Getting fastPollingQueue Command.");
				cmd = m_fastPollingQueue.getNextCommand();	
			}
			
			if (cmd.get() != NULL)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Execute sendModbusCommand.");
				sendModbusCommand(cmd);
			}

		}
	}

	void STDModbusPollingWorker::setInServiceState( bool inService )
	{
		TimerWorker::setInServiceState(inService);
		
		if (NULL != m_currentModbusConnection && false == inService)
		{
			m_rtu.destroyModbusConnection( m_currentModbusConnection );
			m_currentModbusConnection = NULL;
		}
	}
	
    void STDModbusPollingWorker::sendModbusCommand(ModbusCommandPtr& cmd)
    {
       DataBlock< WORD >* blockRegister;
       DataBlock< bool >* blockCoil;

	   unsigned long startAddress;
	   unsigned long endAddress;
	   unsigned long pollingLen;
	   bool 		 bWriteResult;
	   
       switch (cmd->getCommandType())
       {
			case FAST_POLLING: 
			case SLOW_POLLING:
				{
				    DataBlock< WORD > block;
					if (m_isFullPolling == true)
					{
						startAddress = m_addressBlock.start();
						endAddress = m_addressBlock.end();
						LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Update all the datapoint during initial startup.");
					}
					else
					{
						startAddress = cmd->getStartAddress();
						endAddress	= cmd->getEndAddress();
					}
					pollingLen = endAddress - startAddress + 1;

					//If using the standard Modbus, the max MODBUS LENGTH should be less than 255
					//For the pollingLen is exceed the maxModbusLength, need to poll some times.
	                if (m_standardModbusLengthLimit && m_maxModbusLength < pollingLen)
	                {
	                    int loopTimes = pollingLen / m_maxModbusLength;
	                    int remaining = pollingLen % m_maxModbusLength;
			            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Commandtype %s,Exceed Modbus Length Limit,Start %d,End %d, pollingLen %d, maxModbusLen %d, loopTimes %d, remaining %d",
				                    cmd->getCommandTypeString().c_str(),startAddress, endAddress, pollingLen,m_maxModbusLength,loopTimes,remaining);

	                    for (int i = 0; i <= loopTimes; i++)
	                    {
			                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "SetDataBlock Start = %d with Length = %d",
	                        startAddress + i * m_maxModbusLength,
	                        (i == loopTimes) ? ( pollingLen % m_maxModbusLength) : m_maxModbusLength);
	                        
	            			block.setStartAndLength ( startAddress + i * m_maxModbusLength, 
	                                                 (i == loopTimes) ? (pollingLen % m_maxModbusLength) : m_maxModbusLength) ;
	                        
	     		            readHoldingRegisters(block);
							// transfer the data in local mini block (0 based address) to the
							// global data block (1 based address)
							if (m_commsErrorCounter == 0)
							{
								for( int address=block.start(); address<=block.end(); address++ )
								{
									if ( ( address >= block.start() ) && ( address <= block.end() ) )
									{
										m_addressBlock.set( address, block[address] );
									}
								}
														
								Thread::sleep(m_interval);
								LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Finished readHoldingRegisters");
	                        }
							else
							{
								break;
							}
	                    }
												
						// check the error counter and perform appropriated action
						bool preCommsError = m_commsError;
						LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Start processCommsErrorCounter");
						processCommsErrorCounter();
			
						//TD15792, process data block only when connection is ok or is confirmed failed firstly
						//that is, need not process data during retrying and known comms error.
						if ((0 == m_commsErrorCounter && false == m_commsError) || (true == m_commsError && false == preCommsError))
						{
							try
							{
								LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Start processPollingData");
								processPollingData( m_addressBlock);
								LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "End processPollingData");

								if ((false == m_isFullPolling ) && (true == m_commsError) )
								{
									m_isFullPolling = true;
								}
							}
							catch (...) 
							{
								LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RTU %s, Processing data error.", m_rtu.getName().c_str());
							}
						}
	                }
	                else
	                {
						block.setStartAndLength ( startAddress, pollingLen) ;
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Start %d,End %d, pollingLen %d",
					                 startAddress, endAddress, pollingLen);
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Commandtype %d", cmd->getCommandType());
	        		    readHoldingRegisters(block);
						// transfer the data in local mini block (0 based address) to the
						// global data block (1 based address)
						if (m_commsErrorCounter == 0)
						{
							for( int address=block.start(); address<=block.end(); address++ )
							{
								if ( ( address >= block.start() ) && ( address <= block.end() ) )
								{
									m_addressBlock.set( address, block[address] );
								}
							}
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Finished readHoldingRegisters");
						
							Thread::sleep(m_interval);
						}
						// check the error counter and perform appropriated action
						bool preCommsError = m_commsError;
						LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Start processCommsErrorCounter");
						processCommsErrorCounter();
				
						//TD15792, process data block only when connection is ok or is confirmed failed firstly
						//that is, need not process data during retrying and known comms error.
						if ((0 == m_commsErrorCounter && false == m_commsError) || (true == m_commsError && false == preCommsError))
						{
							try
							{
								LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Start processPollingData");
								processPollingData( m_addressBlock );
								LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "End processPollingData");
								
								if ((false == m_isFullPolling ) && (true == m_commsError) )
								{
									m_isFullPolling = true;
								}								
							}
							catch (...) 
							{
								LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RTU %s, Processing data error.", m_rtu.getName().c_str());
							}
						}
	                }
	            }
                break;
				
            case WRITE_REQUEST_REGISTER:  
				blockRegister = cmd->getDataBlockRegister() ;
 				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Start writeMultipleRegisters Commandtype: %s,DataBlock StartAddress:%d,Length:%d",
					cmd->getCommandTypeString().c_str(),blockRegister->start(),blockRegister->length());
                bWriteResult = writeMultipleRegisters(*blockRegister);
                cmd->postResult(bWriteResult);
				break;

			case WRITE_REQUEST_COILS:  
				blockCoil = cmd->getDataBlockCoils();
 				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Start writeMultipleRegisters Commandtype: %s,DataBlock StartAddress:%d,Length:%d",
					cmd->getCommandTypeString().c_str(),blockCoil->start(),blockCoil->length());
                bWriteResult = writeMultipleCoils(*blockCoil);
                cmd->postResult(bWriteResult);
				break;
            default:
                break;
        }
	
        Thread::sleep(m_interval);
       
    }
	
	void STDModbusPollingWorker::processCommsErrorCounter()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "m_commsErrorCounter = %d", m_commsErrorCounter);
		// stop multi threads processing the comms error counter
		TA_THREADGUARD( m_lock );

		// if comms error counter or the socket timeout counter exceeds the max retries
		if ( m_commsErrorCounter > m_commsErrorRetries )
		{
			// reset the comms error counters
			m_commsErrorCounter = 0;

			// set the comms error flag and raise the alarm
			m_commsError = true;
            m_rtu.updateCommsAlarm( true, m_rtuServicePortNumber, "" );

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RTU %s, Service Port Number %s: Comms error counter exceeds maximum retries...",
                m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str() );
		}

		// if there are valid polling data
		if ( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == m_qualityStatus )
		{
			// reset the comms error flag and close alarm
			m_commsError = false;
			m_rtu.updateCommsAlarm( false, m_rtuServicePortNumber, "" );
		}
	}
	
    bool STDModbusPollingWorker::getIsCommsOK() const
    {
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "m_commsError[%s] in the STDModbusPollingWorker ", m_commsError?"False":"True" );
        return (false == m_commsError);
    }

	void STDModbusPollingWorker::processPollingData( const TA_Base_Bus::DataBlock< WORD > & addressBlock)
	{
        // pass address block to rtu
        m_rtu.processPollingData(addressBlock);
		if (m_isFullPolling == true)
		{
			m_isFullPolling = false;
		}
	}
	    
    void STDModbusPollingWorker::CheckModbusException( const TA_Base_Bus::ModbusException & me )
	{
        switch ( me.getFailType() )
		{
		//
		// The following modbus exceptions are categorised as
		// TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR, i.e there is some
		// server specific problem with the configuration of 
		// the modbus packet
		//
		case TA_Base_Bus::ModbusException::INVALID_ADDRESS :
		case TA_Base_Bus::ModbusException::INVALID_VALUE:
		case TA_Base_Bus::ModbusException::INVALID_AND_MASK:
		case TA_Base_Bus::ModbusException::INVALID_OR_MASK:
		case TA_Base_Bus::ModbusException::BIT_ADDRESS_OUT_OF_RANGE:
		case TA_Base_Bus::ModbusException::ILLEGAL_FUNCTION:
		case TA_Base_Bus::ModbusException::ILLEGAL_DATA_ADDRESS:
		case TA_Base_Bus::ModbusException::ILLEGAL_DATA_VALUE:
		case TA_Base_Bus::ModbusException::NO_REQUEST_DATA:
			m_qualityStatus = TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR;
            break;
        
        //
		// The following modbus exceptions are categorised as
		// TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE
		//
		case TA_Base_Bus::ModbusException::ACKNOWLEDGE:
		case TA_Base_Bus::ModbusException::SLAVE_DEVICE_FAILURE:
		case TA_Base_Bus::ModbusException::SLAVE_DEVICE_BUSY:
		case TA_Base_Bus::ModbusException::MEMORY_PARITY:
		case TA_Base_Bus::ModbusException::GATEWAY_PATH_UNAVAILABLE:
		case TA_Base_Bus::ModbusException::TARGET_FAILED_TO_RESPOND:
		case TA_Base_Bus::ModbusException::REPLY_HEADER_READ_FAILED:
			m_qualityStatus = TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE;
			break;

		//
		// The following modbus exceptions are categorised as
		// TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE, i.e there is comms
		// failure, however, the last known value is available.
		// Note that the "age" of the value may be determined
		// from the timestamp
		//
		case TA_Base_Bus::ModbusException::REPLY_PACKET_TOO_SHORT:
		case TA_Base_Bus::ModbusException::REPLY_INCORRECT_SIZE:
		case TA_Base_Bus::ModbusException::PACKET_BODY_MISSING:
		case TA_Base_Bus::ModbusException::UNEXPECTED_FUNCTION_CODE:
		case TA_Base_Bus::ModbusException::INVALID_REPLY_COUNT:
		case TA_Base_Bus::ModbusException::CONNECTION_NOT_ENABLED:
		case TA_Base_Bus::ModbusException::REPLY_TIMEOUT:
		case TA_Base_Bus::ModbusException::CRC_CALC_ERROR:
		case TA_Base_Bus::ModbusException::INVALID_TRANSACTION_ID:
		case TA_Base_Bus::ModbusException::INVALID_PROTOCOL_ID:
		case TA_Base_Bus::ModbusException::INCORRECT_DEVICE_ADDRESS:
		case TA_Base_Bus::ModbusException::REPLY_PACKET_READ_ERROR:
		case TA_Base_Bus::ModbusException::CONNECT_ERROR:
		case TA_Base_Bus::ModbusException::SOCKET_WRITE_ERROR:
		case TA_Base_Bus::ModbusException::SOCKET_READ_ERROR:
		case TA_Base_Bus::ModbusException::SIZE_EXCEEDS_BUFFER:
		default:
			m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
			break;
 		}
	}

    void STDModbusPollingWorker::readHoldingRegisters(DataBlock< WORD >& addressBlock)
    {
        if (NULL == m_currentModbusConnection)
            m_currentModbusConnection = m_rtu.createModbusConnection( m_rtuServicePortNumber, m_slaveID, CONNTYPE_POLL );

		try
    	{
			// if modbus connection is established
			if ( 0 != m_currentModbusConnection )
			{
				m_currentModbusConnection->enable();

				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "*** Start Polling PLC %s, Service Port Number %s",
							 m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str());

                // some cases use ReadHoldingRegister method read data with command "03",
                // most cases use ReadInputRegister method read data with command "04"
                if ("03" == m_readFunctionCode)
                {
                    m_currentModbusConnection->readHoldingRegisters(addressBlock);
                }
                else
                {
                    m_currentModbusConnection->readInputRegisters(addressBlock);
                }
				
			
				// reset comms error counters if comms is OK
				if ( 0 < m_commsErrorCounter )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
								 "RTU %s, Service Port Number %s: Polling OK.  Resetting Comms error counter...",
								 m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str());

					m_commsErrorCounter = 0;
				}

				// set the quality status to good
				m_qualityStatus = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
			}
			
			// else if modbus connection is not yet established
			else
			{
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                             "RTU %s, Service Port Number %s: Modbus connection not established, Incrementing Comms error counter...",
                             m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str());

				// set the quality status to bad
				m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;

				++m_commsErrorCounter;
			}
    	}
    	catch ( const TA_Base_Bus::ModbusException & me )
    	{
    		//TD15792, try to reconnect only when connect_error/timeout before
    		//try to reconnect when any modbus error occur now.
    		// check for the type of exception and set the quality
    		// status accordingly
    		CheckModbusException( me );

    		// increment comms error counter
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "PLC %s, Service Port Number %s: Connection error.",
               m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str() );
			   
			++m_commsErrorCounter;

    	}
    	catch ( ... )
    	{
			// set the quality status to bad
			m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
			
    		// increment comms error counter
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "PLC %s, Service Port Number %s: Unknown exception.",
                m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str() );
				
			++m_commsErrorCounter;
    	}   

	}
    
    bool STDModbusPollingWorker::writeMultipleRegisters(DataBlock< WORD >& addressBlock)
    {
    	bool bSuccess = false;

    	try
    	{
			// if modbus connection is established
			if ( 0 != m_currentModbusConnection )
			{
				m_currentModbusConnection->enable();

				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "*** Start Polling PLC %s, Service Port Number %s",
							 m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str());

				m_currentModbusConnection->writeMultipleRegisters( addressBlock );
				
				// reset comms error counters if comms is OK
				if ( 0 < m_commsErrorCounter )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
								 "RTU %s, Service Port Number %s: Polling OK.  Resetting Comms error counter...",
								 m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str());

					m_commsErrorCounter = 0;
					
				}
				
				// set the quality status to good
				m_qualityStatus = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
				bSuccess = true;

			}
			// else if modbus connection is not yet established
			else
			{
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                             "RTU %s, Service Port Number %s: Modbus connection not established, Incrementing Comms error counter...",
                             m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str());

				// set the quality status to bad
				m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;

				++m_commsErrorCounter;
			}
    	}
    	catch ( const TA_Base_Bus::ModbusException & me )
    	{
    		//TD15792, try to reconnect only when connect_error/timeout before
    		//try to reconnect when any modbus error occur now.
    		// check for the type of exception and set the quality
    		// status accordingly
    		CheckModbusException( me );

    		// increment comms error counter
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "PLC %s, Service Port Number %s: Connection error.",
               m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str() );
			   
			++m_commsErrorCounter;

    	}
    	catch ( ... )
    	{
			// set the quality status to bad
			m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
			
    		// increment comms error counter
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "PLC %s, Service Port Number %s: Unknown exception.",
                m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str() );
				
			++m_commsErrorCounter;
    	}        
    	return bSuccess;                

    }

    bool STDModbusPollingWorker::writeMultipleCoils(DataBlock< bool >& addressBlock)
    {
    	bool bSuccess = false;
    	try
    	{
			// if modbus connection is established
			if ( 0 != m_currentModbusConnection )
			{
				m_currentModbusConnection->enable();

				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "*** Start Polling PLC %s, Service Port Number %s",
							 m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str());

				m_currentModbusConnection->writeMultipleCoils( addressBlock );
				
				// reset comms error counters if comms is OK
				if ( 0 < m_commsErrorCounter )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
								 "RTU %s, Service Port Number %s: Polling OK.  Resetting Comms error counter...",
								 m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str());

					m_commsErrorCounter = 0;
					
				}
				
				// set the quality status to good
				m_qualityStatus = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
				bSuccess = true;
			}
			// else if modbus connection is not yet established
			else
			{
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                             "RTU %s, Service Port Number %s: Modbus connection not established, Incrementing Comms error counter...",
                             m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str());

				// set the quality status to bad
				m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;

				++m_commsErrorCounter;
			}
    	}
    	catch ( const TA_Base_Bus::ModbusException & me )
    	{
    		//TD15792, try to reconnect only when connect_error/timeout before
    		//try to reconnect when any modbus error occur now.
    		// check for the type of exception and set the quality
    		// status accordingly
    		CheckModbusException( me );

    		// increment comms error counter
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "PLC %s, Service Port Number %s: Connection error.",
               m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str() );
			   
			++m_commsErrorCounter;

    	}
    	catch ( ... )
    	{
			// set the quality status to bad
			m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
			
    		// increment comms error counter
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "PLC %s, Service Port Number %s: Unknown exception.",
                m_rtuIpaddress.c_str(), m_rtuServicePortNumber.c_str() );
				
			++m_commsErrorCounter;
    	}

    	return bSuccess;                
    }	
    
    void STDModbusPollingWorker::postRequest(ModbusCommandPtr& cmd)
    {
    	if (false == isInService())
    	{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "STDModbusPollingWorker is not working now.");
	  		return;
    	}	

		if (true == m_overlapModbusRequest)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "STDModbusPollingWorker setting is overlapModbusRequest.");
			sendModbusCommand(cmd);
			return;
		}

		switch (cmd->getCommandType())
		{
			case FAST_POLLING:
				
				if (m_fastPollingQueue.isEmpty())
				{
					if (true == m_commsError)
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Modbus Connection have error, trying to check if the connection recover...");
					}
					
					m_fastPollingQueue.scheduleCommand(cmd);
				}
				break;
			case SLOW_POLLING:
				if (true == m_commsError)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Modbus Connection have error, stop adding command to SLOW_POLLING queue.");
				}
				else
				{
					m_slowPollingQueue.scheduleCommand(cmd);
				}
				break;  

			case WRITE_REQUEST_REGISTER:
			case WRITE_REQUEST_COILS:
				if (true == m_commsError)
				{
					cmd->postResult(false);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Modbus Connection have error, stop adding command to COMMAND queue.");
				}
				else
				{
					m_writeQueue.scheduleCommand(cmd);
				}
				break;                  
			default:
				break;
		}
    }
}

