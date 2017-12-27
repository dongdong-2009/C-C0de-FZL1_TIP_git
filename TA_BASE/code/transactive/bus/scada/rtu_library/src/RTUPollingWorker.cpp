/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUPollingWorker.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2017/05/25 10:19:08 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTUPollingWorker object is responsible for polling input data from the PMS RTU
  *
  */

#include <sys/timeb.h>

#include "bus/scada/rtu_library/src/RTUPollingWorker.h"
#include "bus/scada/rtu_library/src/RTU.h"
#include "bus/scada/rtu_library/src/RTUStatusWorker.h"

#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/modbus_comms/src/IModbus.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

//TD15792, remove AvalancheHeaderDataPointTable processing codes
namespace TA_Base_Bus
{
	const int MAX_NUMBER_ERROR_RETRIES = 5;
    const int NUMBER_OF_SPARE_PORT = 4;
	
	RTUPollingWorker::RTUPollingWorker( RTU& rtu,
										unsigned long rtuLocationKey,
                                        const std::string & rtuServicePortNumber,
                                        unsigned long startIOAddress,
                                        unsigned long endIOAddress,
                                        unsigned long statusStartAddress,
                                        unsigned long statusEndAddress,
                                        unsigned long statusOffsetAddress,
                                        int scanTime,
                                        int slaveID,
										int commsErrorRetries)
	:
    m_rtu( rtu ),
    m_rtuServicePortNumber( rtuServicePortNumber ),
    m_rtuBaseServicePort( 0 ),
    m_scanTime( scanTime ),
    m_slaveID( slaveID ),
    m_commsErrorRetries( commsErrorRetries ),
    m_commsErrorCounter( 0 ),
    m_commsError( false ),
    m_qualityStatus( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON ),
	m_socketTimeoutUpdated( false ),
	m_scanTimeUpdated(false),
    m_inService( false ),
    m_threadTerminated( false ),
    m_addressBlock( startIOAddress, endIOAddress ),
    m_currentModbusConnection( NULL ),
	m_timerUtility( TA_Base_Core::SingletonTimerUtil::getInstance() )
	{
        // Keep track of the original Port Number which will be used as a base
        // when computing the next spare Port no to use to connect to the rtu.
        std::stringstream ssPortNo;
        ssPortNo << m_rtuServicePortNumber;
        ssPortNo >> m_rtuBaseServicePort;

		if ( 0 == m_commsErrorRetries )
		{
			m_commsErrorRetries = MAX_NUMBER_ERROR_RETRIES;
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "startIOAddress,endIOAddress = %d, %d", startIOAddress, endIOAddress);
	}


	RTUPollingWorker::~RTUPollingWorker()
	{
        //TD13398, should stop this thread first, then stop the timers.
		// Ensure that RTUPollingWorker thread is stopped
        setInServiceState( false );

		// Stop the timers
		m_timerUtility.stopPeriodicTimeOutClock(this);
		TA_Base_Core::SingletonTimerUtil::removeInstance();

	}


	void RTUPollingWorker::setInServiceState( bool inService )
	{
        TA_Base_Core::ThreadGuard guard( m_modbusLock );

        // prevent setting the same state
        if (m_inService == inService)
        {
            return;
        }

		m_inService = inService;

		if ( true == inService )
		{
            TA_ASSERT( NULL == m_currentModbusConnection, "m_currentModbusConnectionis not NULL in RTUPollingWorker::setInServiceState(true)" );
            m_currentModbusConnection = m_rtu.createModbusConnection( m_rtuServicePortNumber, m_slaveID, CONNTYPE_POLL );

            start();
		}
		else
		{
            terminateAndWait();

			m_rtu.destroyModbusConnection( m_currentModbusConnection );
		}
	}

    
	void RTUPollingWorker::updateCommsErrorRetries ( int retries )
	{
		// save the new retries
        m_commsErrorRetries = retries;
	}


	bool RTUPollingWorker::isInService() const
	{
		return m_inService;
	}


	void RTUPollingWorker::terminate()
	{
		// terminate polling thread
		m_threadTerminated = true;

        // signalling wont do any good if its not waiting
        // worst case is wait for the timer
        // it will terminate on the next poll cycle
		m_timerCondition.signal();
	}


	void RTUPollingWorker::run()
	{
		bool preIsCommsError = true;
		
		m_timerUtility.startPeriodicTimeOutClock( this, m_scanTime, false );

		while ( false == m_threadTerminated )
		{
			// if the worker thread is in service
			if ( true == m_inService )
			{
				//reset the scan time
				if ( true == m_scanTimeUpdated )
				{
					m_timerUtility.startPeriodicTimeOutClock(this, m_scanTime, false);

					// reset the flag
					m_scanTimeUpdated = false;
				}

				try
				{
					//recreate the modbus connection, may cause some exceptions, so put in try block.
					if ( true == m_socketTimeoutUpdated )
					{
						useSparedModbusConnection();

						// reset the flag
						m_socketTimeoutUpdated = false;
					}

					//if not connected or have retried for enough times, 
					//try to establish new modbus connection
					if (0 == m_currentModbusConnection || m_commsErrorCounter >= m_commsErrorRetries)
					{
						useSparedModbusConnection();
					}

					// if modbus connection is established
					if ( 0 != m_currentModbusConnection )
					{
						// poll the RTU for all input registers whose addresses are
						// within the range specified for this worker, using the
						// TCP Modbus Driver

						m_currentModbusConnection->enable();

                        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "*** Start Polling STE RTU %s, Service Port Number %s",
                            m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str());

						m_currentModbusConnection->readInputRegisters( m_addressBlock );

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
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RTU %s, Service Port Number %s: Connection error, Incrementing Comms error counter...",
                        m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str() );

					++m_commsErrorCounter;
				}
				catch ( ... )
				{
					// set the quality status to bad
					m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;

					// increment comms error counter
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RTU %s, Service Port Number %s: Unknown exception, Incrementing Comms error counter...",
                        m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str() );

					++m_commsErrorCounter;
				}
				
				// check the error counter and perform appropriated action
                bool preCommsError = m_commsError;
				processCommsErrorCounter();
			
				//TD15792, process data block only when connection is ok or is confirmed failed firstly
				//that is, need not process data during retrying and known comms error.
				if ((0 == m_commsErrorCounter && false == m_commsError) || (true == m_commsError && false == preCommsError))
				{
					try
					{
						processPollingData( m_addressBlock );
					}
					catch (...) 
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RTU %s, Proecessing data error.", m_rtu.getName().c_str());
					}
				}

            }   // if the worker thread is not in service

			m_timerCondition.wait();
		}

		// reset these state before next start
		m_commsError = false;
		m_commsErrorCounter = 0;

		m_timerUtility.stopPeriodicTimeOutClock(this);

		// exit thread loop --> reset the flag
		m_threadTerminated = false;
	}

	void RTUPollingWorker::timerExpired(long timerId, void* userData)
	{
		m_timerCondition.signal();
	}

	void RTUPollingWorker::processCommsErrorCounter()
	{
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


	void RTUPollingWorker::processPollingData( const TA_Base_Bus::DataBlock< WORD > & addressBlock )
	{
        // pass address block to rtu
        m_rtu.processPollingData(addressBlock);
	}

    bool RTUPollingWorker::getIsCommsOK() const
    {
        return (false == m_commsError);
    }


	void RTUPollingWorker::useSparedModbusConnection()
	{
        int nextServicePort = 0;
        std::stringstream ssServicePort;

        ssServicePort << m_rtuServicePortNumber;
        ssServicePort >> nextServicePort;
        
        ++nextServicePort;

		int sparePort = NUMBER_OF_SPARE_PORT;
        
        if ( nextServicePort > ( m_rtuBaseServicePort + sparePort ) )
        {
            nextServicePort = m_rtuBaseServicePort;
        }

        std::ostringstream ossNextServicePort;
        ossNextServicePort << nextServicePort;
        m_rtuServicePortNumber = ossNextServicePort.str();

        TA_Base_Bus::IModbus * newModbusConnection = m_rtu.createModbusConnection( m_rtuServicePortNumber, m_slaveID, CONNTYPE_POLL );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RTU %s successfully created a spared connection on port %s",
            m_rtu.getName().c_str(), m_rtuServicePortNumber.c_str() );

		// remove the current timeout socket
		m_rtu.destroyModbusConnection( m_currentModbusConnection );

		// assign new socket
		m_currentModbusConnection = newModbusConnection;
	}


	void RTUPollingWorker::updatePollTimeout( int pollTimeout )   //wenching
	{
		m_socketTimeoutUpdated = true;
	}


	void RTUPollingWorker::updateScanTimeMSecs(  int scanTimeMSecs )
	{
		if ( m_scanTime != scanTimeMSecs && scanTimeMSecs > 0 )
		{
			// save the new scan time
			m_scanTime = scanTimeMSecs;

			// set the flag to indicate so
			m_scanTimeUpdated = true;
		}
	}


    void RTUPollingWorker::CheckModbusException( const TA_Base_Bus::ModbusException & me )
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
		}
	}

    
	bool RTUPollingWorker::isThreadRunning() const
	{
		return ( THREAD_STATE_RUNNING == getCurrentState() );
	}
}





















