/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTU.cpp $
  * @author:	Liu Yong
  * @version:	$Revision: #5 $
  *
  * Last modification:	$DateTime: 2017/06/02 12:08:57 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTU object manages a set of DataPoints that have been configured for it.
  *
  */

#include <sstream>

#include "bus/scada/rtu_library/src/RTU.h"
#include "bus/scada/rtu_library/src/BaseRTUManager.h"
#include "bus/scada/rtu_library/src/RTUCommandWorker.h"
#include "bus/scada/rtu_library/src/RTUStatusWorker.h"
#include "bus/scada/rtu_library/src/ModbusDpProcessor.h"

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "bus/modbus_comms/src/SteModbusTcpConnection.h"
#include "bus/modbus_comms/src/DuplexModbusTcpConnection.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PMSAgentAlarms_MessageTypes.h"
#include "core/message/types/PMSAgentAudit_MessageTypes.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

//TD15792, remove AvalancheHeaderDataPointTable processing codes
namespace TA_Base_Bus
{
    RTU::RTU (BaseRTUManager& rtuManager, TA_Base_Core::RTUEntityDataPtr rtuEntityData)
	    :
        m_rtuManager(rtuManager),
        m_rtuEntityData(rtuEntityData),
	    m_name(rtuEntityData->getName()),
	    m_address(rtuEntityData->getAddress()),
        m_primaryKey(rtuEntityData->getKey()),
        m_typeKey(rtuEntityData->getTypeKey()),
        m_locationKey(rtuEntityData->getLocation()),
        m_subsystemKey(rtuEntityData->getSubsystem()),
        m_inService(rtuEntityData->getInServiceState()),
        m_isInControlMode(false),
        m_pollingStarted(false),
        m_commsErrorAlarmGenerated(false),
        m_tcpNoDelay(rtuEntityData->getTcpNoDelay()),
        m_slaveID(rtuEntityData->getSlaveID()),
	    m_commsErrorRetries(rtuEntityData->getCommsErrorRetries()),
		m_pollTimeout( rtuEntityData->getPollTimeout() ),
		m_commandTimeout( rtuEntityData->getCommandTimeout() ),
        m_analogueDpUpdateInterval(0),
		m_qualityStatus(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED),
	    m_auditMessageSender(NULL),
	    m_rtuStatus(NULL),
	    m_rtuInternalDpProcessor(NULL)
	{
		m_outputWriters.clear();

		// create RTUWorker to look after the RTU Polling table if addresses are valid
		if (false == getAreAddressesValid(m_rtuEntityData->getRTUPollingTableStartAddress(), m_rtuEntityData->getRTUPollingTableEndAddress()))
        {
            std::ostringstream desc;
            desc << "RTU " << m_name << " polling table address invalid;"
                << " rtuPollingTableStartAddress: " << m_rtuEntityData->getRTUPollingTableStartAddress() 
                << " rtuPollingTableEndAddress: " << m_rtuEntityData->getRTUPollingTableEndAddress();

            TA_THROW(GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()));        
        }

        m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::PMSAgentAudit::Context);
    }

	RTU::~RTU()
	{        
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
 
		// clean up other output workers
		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
			itr != m_outputWriters.end();
			++itr)
		{
			delete (*itr);
		}
		m_outputWriters.clear();
		
		if (NULL != m_auditMessageSender)
		{
			delete m_auditMessageSender;
			m_auditMessageSender = NULL;
		}

		if (NULL != m_rtuStatus)
		{
			delete m_rtuStatus;
			m_rtuStatus = NULL;			
		}

		if (NULL != m_rtuInternalDpProcessor)
		{
			delete m_rtuInternalDpProcessor;
			m_rtuInternalDpProcessor = NULL;			
		}

    }


	const std::string & RTU::getName() const
	{
		return m_name;
	}


	const std::string & RTU::getAddress() const
	{
		return m_address;
	}


	unsigned long RTU::getPKey() const
	{
		return m_primaryKey;
	}


	unsigned long RTU::getTypeKey() const
	{
		return m_typeKey;
	}


	unsigned long RTU::getLocationKey() const
	{
		return m_locationKey;
	}


	unsigned long RTU::getSubsystemKey() const
	{
		return m_subsystemKey;
	}

    
    int RTU::getSlaveID() const
    {
        return m_slaveID;
    }

	bool RTU::isInService() const
	{
		return m_inService;
	}

	bool RTU::getIsInControlMode()
	{
		return m_isInControlMode;
	}

	int RTU::getMaxCommsRetries() const
	{
		return m_commsErrorRetries;
	}
	
	void RTU::updateCommsAlarm( bool raiseAlarm, 
								const std::string & servicePortNumber, 
								const std::string & additionalComment)
    {
        TA_THREADGUARD(m_alarmGenereatedLock);

        // if the current comms status is bad then raise a comms alarm and log
        // the a new event
        if (true == raiseAlarm)
        {
            // if no alarm and event have been generated
            if (false == m_commsErrorAlarmGenerated)
            {
                // construct the TA_Base_Core::DescriptionParameters list
                TA_Base_Core::DescriptionParameters descriptionParameters;
                TA_Base_Core::NameValuePair pair1("RTUName", m_name);
                TA_Base_Core::NameValuePair pair2("ServicePortNumber", servicePortNumber);
                TA_Base_Core::NameValuePair pair3("ModbusException", additionalComment);
                descriptionParameters.push_back(&pair1);
                descriptionParameters.push_back(&pair2);
                descriptionParameters.push_back(&pair3);

                try
                {
                    //submit an unique RTU comms alarm
                    TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::PMSAgentAlarms::RTUCommsError,
                                                m_primaryKey,				// entity key
												m_typeKey,					// entity type key
                                                descriptionParameters,
                                                m_name,						// entity name
                                                m_locationKey,				// location key
                                                m_subsystemKey,				// subystem key
												m_rtuManager.getAgentAssetName(),
												TA_Base_Core::AlarmConstants::defaultTime);
                }
                catch (...)
                {
                }
                
                // set up the flags
                m_commsErrorAlarmGenerated = true;
            }
        }
        else
        {
            //
            // Else if the current RTU state is good then close the comms alarms
            // and log a new event
			//
			// if alarm and event have been generated
			if (true == m_commsErrorAlarmGenerated)
			{
				// close the unique RTU Comms alarm
				TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::PMSAgentAlarms::RTUCommsError,
                                                     m_primaryKey,
                                                     m_locationKey);
                
                // set up the flags
				m_commsErrorAlarmGenerated = false;
			}
		}
    }


    void RTU::submitAuditMessage(const TA_Base_Core::DescriptionParameters & description,
                                 const TA_Base_Core::MessageType & messageType)
    {
        TA_ASSERT(NULL != m_auditMessageSender, "m_auditMessageSender is NULL in RTU::submitAuditMessage");
        
        TA_Base_Core::DescriptionParameters descriptionParameters(description);
        TA_Base_Core::NameValuePair nvpRTUName("RTUName", m_name);
        descriptionParameters.push_back (&nvpRTUName);

        try
		{
            m_auditMessageSender->sendAuditMessage(messageType,				// message type
                                                    m_primaryKey,				// entity key
												    descriptionParameters,		// description
												    "",							// details
												    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),	// session ID
                                                    "",							// alarm ID
                                                    "",							// incidentkey
                                                    "");						// parent event ID
		}
        catch(const TA_Base_Core::TransactiveException& te)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
        }
		catch (...)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Caught unknown exception while sending audit message");
		}
    }

	RTU_STATUS RTU::getRTUStatus()
	{
		if (m_rtuStatus->getIsCommsOK())
		{
			if (m_rtuStatus->getIsRTUSwitching())
			{
				return RTU_SWITCHING;
			}
			else
			{
				if (m_rtuStatus->getIsPrimaryRTU())
				{
					return RTU_PRIMARY;
				}
				else
				{
					return RTU_STANDBY;
				}
			}
		}
		else
		{
			return RTU_COMMS_DOWN;
		}
	}

	void RTU::destroyModbusConnection(TA_Base_Bus::IModbus * & modbusConnection)
	{
		TA_ASSERT(NULL != modbusConnection, "modbusConnection == NULL in RTU::destroyModbusConnection");

		modbusConnection->disable();

		delete modbusConnection;
		modbusConnection = NULL;
	}

	bool RTU::addDataPoint(TA_Base_Bus::DataPoint * dp)
	{
		TA_ASSERT(NULL != dp, "dp is NULL in RTU::addDataPoint");

		// only hold output datapoints
		bool isAdded = false;
		if (dp->isWriteable())
		{
			for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
				isAdded == false && itr != m_outputWriters.end();
				++itr)
			{
				isAdded =(*itr)->addDataPoint(dp);
			}
		}
		else
		{
			if (NULL != m_rtuInternalDpProcessor)
			{
				isAdded = m_rtuInternalDpProcessor->addDataPoint(dp);
			}
		}

		return isAdded;
	}


	void RTU::removeDataPoint (TA_Base_Bus::DataPoint * dp)
	{
		TA_ASSERT(NULL != dp, "dp is NULL in RTU::removeDataPoint");

		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
			itr != m_outputWriters.end();
			++itr)
		{
			(*itr)->removeDataPoint(dp);
		}
	}

	void RTU::setToControlMode()
	{
        // set the flag
		m_isInControlMode = true;

		// if the RTU is IN_SERVICE
		if (true == m_inService)
		{
			startProcessing();
		}
	}


	void RTU::setToMonitorMode()
	{
        // set the flag
		m_isInControlMode = false;

		stopProcessing();
	}

	bool RTU::getAreAddressesValid (unsigned long startAddress, unsigned long endAddress)
	{
		if (((0 == startAddress) && (0 == endAddress)) ||
			 (0 > startAddress) || (0 > endAddress) ||
			 (startAddress > endAddress))
		{
			return false;
		}

		return true;
	}

	void RTU::createOutputWriter(const std::string & name,
								const std::string & outputServiceNumber,
								unsigned long outputStartAddress,
								unsigned long outputEndAddress,
								int slaveID,
								const std::string digitalWriteFunCode)
	{
		// create RTUWorker to look after the RTU output table if addresses are valid
		if (true == getAreAddressesValid(outputStartAddress, outputEndAddress))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating output writer [%s] for RTU [%s]", name.c_str(), getName().c_str());

			StationSystemOutputWriter* writer = new StationSystemOutputWriter(*this, 
																			name,
																			outputServiceNumber,
																			outputStartAddress,
																			outputEndAddress,
																			slaveID,
																			digitalWriteFunCode);

			m_outputWriters.push_back(writer);
		}
	}
	
    void RTU::updateRTUConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "RTU [%s] received a config update event", getName().c_str());
        
        switch (updateEvent.getModifications())
        {
        // only if the RTU has been updated in the database
        case TA_Base_Core::Update:
            {
                // get the list of changed parameter from the config update
                const std::vector< std::string > * changes = &(updateEvent.getChangedParams());
                
                if ((0 != changes) && (false == changes->empty()))
                {
                    try
                    {
                        // force rtu entity data to be reloaded
                        m_rtuEntityData->invalidate();
                        
                        for (unsigned long i = 0; i < changes->size(); i++)
                        {
                            if (TA_Base_Core::RTUEntityData::INSERVICE == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating InService state of %s", getName().c_str());
                                updateInServiceParameter (m_rtuEntityData->getInServiceState());
                            }
                            else if (TA_Base_Core::RTUEntityData::SCANTIME == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating ScanTime of %s", getName().c_str());
                                updateScanTimeMSecs (m_rtuEntityData->getScanTimeMSecs());
                            }
                            else if (TA_Base_Core::RTUEntityData::COMMS_ERROR_RETRIES == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating CommsErrorRetries of %s", getName().c_str());
                                updateCommsErrorRetries (m_rtuEntityData->getCommsErrorRetries());
                            }
                            else if (TA_Base_Core::RTUEntityData::COMMAND_TIMEOUT == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating command timeout of %s", getName().c_str());
                                updateCommandTimeout(m_rtuEntityData->getCommandTimeout());
                            }
							else if (TA_Base_Core::RTUEntityData::POLL_TIMEOUT == (*changes)[i])
                            {
								LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating poll timeout of %s", getName().c_str());
								updatePollTimeout(m_rtuEntityData->getPollTimeout());
                            }
                        }
                    }
                    catch (...)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught when process rtu entity data");
                    }
                }
                
                break;
            }
            
        // don't care in other case
        default:
            {
                break;
            }
        }
    }
        
	void RTU::updateInServiceParameter(bool newState)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for InService parameter...");

		// save the new state
		m_inService = newState;

		// if the RTU has been in control mode
		if (true == m_isInControlMode && true == m_inService)
		{
			startProcessing();
        }
		else
		{
			stopProcessing();
		}
	}


	void RTU::updateScanTimeMSecs(int scanTimeMSecs)
	{
        //TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updateScanTimeMSecs");

        //m_rtuPolling->updateScanTimeMSecs(scanTimeMSecs);
	}


	void RTU::updateCommsErrorRetries(int retries)
	{
        //TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updateCommsErrorRetries");

        // if (m_commsErrorRetries != retries)
        // {
        //     m_commsErrorRetries = retries;
        //     m_rtuPolling->updateCommsErrorRetries(retries);
        // }
	}

	void RTU::updatePollTimeout( int pollTimeout )
	{
		//TA_ASSERT( NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updatePollTimeout" );
        
        // if ( m_pollTimeout != pollTimeout )
        // {
        //     m_pollTimeout = pollTimeout;
        // }
	}
	
	//TD16169: update analogue datapoint update interval
	void RTU::updateAnalogueDpUpdateInterval(int analogueDpUpdateInterval)
	{
		m_analogueDpUpdateInterval = analogueDpUpdateInterval;

		if (NULL != m_rtuInternalDpProcessor)
		{
			m_rtuInternalDpProcessor->updateAnalogueDpUpdateInterval(analogueDpUpdateInterval);
		}
	}

	void RTU::updateCommandTimeout( int commandTimeout )
	{
		if ( m_commandTimeout != commandTimeout )
		{
			m_commandTimeout = commandTimeout;
		}
	}

	bool RTU::processSendingData( TA_Base_Bus::DataBlock < WORD > & commandValues, 
										std::string& rtuServicePortNumber,
										int slaveID )
	{
		bool raiseAlarm = false;
		bool operationStatus = true;
		std::string additionalAlarmComment;

        TA_Base_Bus::IModbus* modbusConnection = createModbusConnection( rtuServicePortNumber, slaveID, CONNTYPE_COMMAND );

		
		// if modbus connection is available
		if ( NULL != modbusConnection )
		{
		    try
		    {
		        // enable the Modbus Connection for writting command
                modbusConnection->enable();

				// write the command
				modbusConnection->writeMultipleRegisters ( commandValues );
            }
		    catch ( const TA_Base_Bus::ModbusException & me )
		    {
                additionalAlarmComment = me.what();

			    // set the flag to indicate the operation has failed
			    raiseAlarm = true;
			    operationStatus = false;
		    }
		    catch ( ... )
		    {
                // set the flag to indicate the operation has failed
			    raiseAlarm = true;
			    operationStatus = false;
		    }

            // disable the Modbus Connection
            // modbusConnection->disable();
			destroyModbusConnection( modbusConnection );
			modbusConnection = NULL;
        }
        else
        {
            // set the flag to indicate the operation has failed
            additionalAlarmComment = "TCP Modbus connection is not available";
            raiseAlarm = true;
			operationStatus = false;
		}

        // raise/close comms alarm if neccessary
		updateCommsAlarm( raiseAlarm, rtuServicePortNumber, additionalAlarmComment );

		return operationStatus;
	}

	bool RTU::processSendingData( TA_Base_Bus::DataBlock < bool > & commandValues, 
										std::string& rtuServicePortNumber,
										int slaveID )
	{
		bool raiseAlarm = false;
		bool operationStatus = true;
		std::string additionalAlarmComment;

        TA_Base_Bus::IModbus* modbusConnection = createModbusConnection( rtuServicePortNumber, slaveID, CONNTYPE_COMMAND );
		
		// if modbus connection is available
		if ( NULL != modbusConnection )
		{
		    try
		    {
		        // enable the Modbus Connection for writting command
                modbusConnection->enable();

				// write the command
				modbusConnection->writeMultipleCoils( commandValues );
            }
		    catch ( const TA_Base_Bus::ModbusException & me )
		    {
                additionalAlarmComment = me.what();

			    // set the flag to indicate the operation has failed
			    raiseAlarm = true;
			    operationStatus = false;
		    }
		    catch ( ... )
		    {
                // set the flag to indicate the operation has failed
			    raiseAlarm = true;
			    operationStatus = false;
		    }

            // disable the Modbus Connection
            // modbusConnection->disable();
			destroyModbusConnection( modbusConnection );
			modbusConnection = NULL;
        }
        else
        {
            // set the flag to indicate the operation has failed
            additionalAlarmComment = "TCP Modbus connection is not available";
            raiseAlarm = true;
			operationStatus = false;
		}

        // raise/close comms alarm if neccessary
		updateCommsAlarm( raiseAlarm, rtuServicePortNumber, additionalAlarmComment );

		return operationStatus;
	}

}
