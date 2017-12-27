/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/SteRTU.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #5 $
  *
  * Last modification:	$DateTime: 2017/06/02 12:08:57 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The STERTU object manages a set of DataPoints that have been configured for it.
  *
  */

#include <sstream>

#include "bus/scada/rtu_library/src/SteRTU.h"
#include "bus/scada/rtu_library/src/SteRTUManager.h"
#include "bus/scada/rtu_library/src/RTUCommandWorker.h"
#include "bus/scada/rtu_library/src/RTUPollingWorker.h"
#include "bus/scada/rtu_library/src/SteRTUStatusWorker.h"
#include "bus/scada/rtu_library/src/SteRTUDpProcessor.h"

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "bus/modbus_comms/src/SteModbusTcpConnection.h"
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
    SteRTU::SteRTU (SteRTUManager& rtuManager, TA_Base_Core::RTUEntityDataPtr rtuEntityData)
	    :
		RTU(rtuManager, rtuEntityData),
		m_rtuPolling(NULL),
		m_isUploadingConfigFile(false),
		m_rtuCommand(NULL),
		m_rtuDownloadWorker(*this)
	{
        // create RTUWorker to look after the RTU commands if addresses are valid
		if (false == getAreAddressesValid(m_rtuEntityData->getRTUCommandTableStartAddress(), 
										  m_rtuEntityData->getRTUCommandTableEndAddress()))
        {
            std::ostringstream desc;
            desc << "STE RTU " << m_name << " Command table address invalid;"
                << " rtuCommandTableStartAddress: " << m_rtuEntityData->getRTUCommandTableStartAddress() 
                << " rtuCommandTableEndAddress: " << m_rtuEntityData->getRTUCommandTableEndAddress();

			TA_THROW(GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()));        
        }

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating SteRTU [%s]...", getName().c_str());
		
    }

	SteRTU::~SteRTU()
	{
		if (NULL != m_rtuPolling)
		{
			delete m_rtuPolling;
		}

		// clean up the rtu command worker if available
		delete m_rtuCommand;
		m_rtuCommand = NULL;

	}


    TA_Base_Bus::IModbus * SteRTU::createModbusConnection(const std::string & rtuServicePortNumber, 
    														int slaveID, 
    														ModbusConnectionType connectionType)
    {
		TA_ASSERT( connectionType == CONNTYPE_COMMAND || 
			connectionType == CONNTYPE_POLL, "Modbus connection type is invalid in SteRTU::createModbusConnection" );

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

		modbusConnection = new TA_Base_Bus::SteModbusTcpConnection(m_address,
			rtuServicePortNumber,
			timeout,
			m_tcpNoDelay);

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "SteRTU::createModbusConnection - Timeout=%d",timeout);

		modbusConnection->setDeviceAddress(slaveID);
		modbusConnection->disable();

		return modbusConnection;
    }	

	unsigned long SteRTU::adjustAddress(unsigned long registerAddress)
	{
		return registerAddress;
	}
	
    void SteRTU::createRTUWorkers()
    {
        // if polling thread already exist, must be called more than once
        TA_ASSERT(NULL == m_rtuPolling, "createRTUWorkers can be only called once");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating polling worker for SteRTU [%s]...", getName().c_str());
        // create rtu polling worker
        m_rtuPolling = new RTUPollingWorker (*this,
								            m_rtuEntityData->getLocation(),
								            m_rtuEntityData->getRTUPollingTableServiceNumber(),
								            m_rtuEntityData->getRTUPollingTableStartAddress(),
								            m_rtuEntityData->getRTUPollingTableEndAddress(),
								            m_rtuEntityData->getRTUStatusTableStartAddress(),
								            m_rtuEntityData->getRTUStatusTableEndAddress(),
								            m_rtuEntityData->getRTUStatusOffsetAddress(),
								            m_rtuEntityData->getScanTimeMSecs(),
								            m_slaveID,
								            m_commsErrorRetries);
        
        TA_ASSERT(NULL == m_rtuStatus, "createRTUWorkers can be only called once");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating status worker for SteRTU [%s]...", getName().c_str());
        // create rtu status worker to process RTU Status data
        // NOTE: RTU Status table range is 0-32 words, but the DataPoint entities
        // associated with the RTU status info is within [0 + statusOffsetAddress, 32 + statusOffsetAddress]
        m_rtuStatus = new SteRTUStatusWorker(m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUStatusTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUStatusTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_slaveID);
        
        TA_ASSERT(NULL == m_rtuCommand, "createRTUWorkers can be only called once");
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating output writer for SteRTU [%s] internal datapoints processing...", getName().c_str());
        //create output writer for command worker
        StationSystemOutputWriter* writer = new StationSystemOutputWriter(*this, 
            m_name,
            m_rtuEntityData->getRTUCommandTableServiceNumber(),
            m_rtuEntityData->getRTUCommandTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUCommandTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_slaveID,
			"0F",
			m_rtuEntityData->getRTUStatusOffsetAddress());
        
        m_outputWriters.push_back(writer);
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating command worker for SteRTU [%s]...", getName().c_str());
        // create output writer
        m_rtuCommand = new RTUCommandWorker (*this, m_rtuEntityData->getRTUCommandTableServiceNumber());

        TA_ASSERT(NULL == m_rtuInternalDpProcessor, "createRTUWorkers can be only called once");

        // create station system worker for RTU status datapoint processing
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating station system worker for SteRTU [%s] internal datapoints processing...", getName().c_str());
        std::vector<std::pair<unsigned long, unsigned long> > statusBlockAddress;

		std::pair<unsigned long, unsigned long> statusAddressPair;
		statusAddressPair.first = m_rtuEntityData->getRTUStatusTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress();
		statusAddressPair.second = m_rtuEntityData->getRTUStatusTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress();
		statusBlockAddress.push_back(statusAddressPair);
        
        m_rtuInternalDpProcessor = new SteRTUDpProcessor(
            m_rtuManager.getStationName(),
            m_name,
            m_rtuEntityData->getRTUPollingTableStartAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUPollingTableEndAddress() + m_rtuEntityData->getRTUStatusOffsetAddress(),
            m_rtuEntityData->getRTUStatusOffsetAddress(),
            statusBlockAddress,
            m_slaveID,
            0,
            m_analogueDpUpdateInterval
            );
    }
	
    // called by rtu polling thread and forward to rtu manager
    void SteRTU::processPollingData(const TA_Base_Bus::DataBlock< WORD > & addressBlock)
    {
        // process rtu status
		if (NULL == m_rtuPolling)
		{
			return;
		}

		m_rtuStatus->processRTUDataBlock(addressBlock, m_rtuPolling->getIsCommsOK());
		
        SteRTUStatusWorker* rtuStatus = dynamic_cast<SteRTUStatusWorker*> (m_rtuStatus);

        // check if download configfile is required
        if (rtuStatus->getIsDownloadRequired())
        {
            uploadConfigFile();
        }
        
        // process rtu internal datapoints
		if (m_rtuPolling->getIsCommsOK())
		{
			m_qualityStatus = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
		}
		else if (TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != m_qualityStatus)
		{
			m_qualityStatus = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
		}

        m_rtuInternalDpProcessor->processDataBlock(addressBlock, m_qualityStatus);

        // notify holder to process polling datablock
        m_rtuManager.processRTUDataBlock(*this, addressBlock);
    }

	//TD10586
	bool SteRTU::processWriteRequest(TA_Base_Bus::DataPointWriteRequest & newWrite)
	{
		bool isProcessed = false;

		for (StationSystemOutputWriters::iterator itr = m_outputWriters.begin();
			isProcessed == false && itr != m_outputWriters.end();
			itr++)
		{
			// if station system slaveId equil to rtu's slaveId, then should process write request
			// else only process write request when this rtu is primary
			if (getIsPrimaryRTU() || (*itr)->getSlaveID() == m_slaveID)
			{
				isProcessed = (*itr)->processWriteRequest(newWrite);
			}
		}

		return isProcessed;
	}

	void SteRTU::startProcessing()
	{
		TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in SteRTU::startProcessing");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Start the polling thread of SteRTU %s, IP address %s",
			m_name.c_str(), m_address.c_str());

		// only proceed if the RTU is in service
		if (false == m_inService)
		{
			m_pollingStarted = false;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Cannot start polling for status since SteRTU %s, IP address %s is not IN SERVICE.",
				m_name.c_str(), m_address.c_str());
			return;
		}

		// enable modbus connection
		m_rtuPolling->setInServiceState(true);

		m_pollingStarted = true;		
	}


	void SteRTU::stopProcessing()
	{
		TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in SteRTU::stopProcessing");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Terminate the polling thread of SteRTU %s, IP address %s",
			m_name.c_str(), m_address.c_str());

		m_rtuPolling->setInServiceState(false);

		m_pollingStarted = false;		
    }

	void SteRTU::updateInServiceParameter(bool newState)
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


	void SteRTU::updateScanTimeMSecs(int scanTimeMSecs)
	{
        TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updateScanTimeMSecs");

        m_rtuPolling->updateScanTimeMSecs(scanTimeMSecs);
	}


	void SteRTU::updateCommsErrorRetries(int retries)
	{
        TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updateCommsErrorRetries");

        if (m_commsErrorRetries != retries)
        {
            m_commsErrorRetries = retries;
            m_rtuPolling->updateCommsErrorRetries(retries);
        }
	}

	void SteRTU::updatePollTimeout( int pollTimeout )
	{
		TA_ASSERT( NULL != m_rtuPolling, "m_rtuPolling is NULL in RTU::updatePollTimeout" );
        
        if ( m_pollTimeout != pollTimeout )
        {
            m_pollTimeout = pollTimeout;
        }
	}
	
	bool SteRTU::getIsCommsWithStationSystemsValid (int statuswordnumber,int slaveID) const
	{
		SteRTUStatusWorker* rtuStatus = dynamic_cast<SteRTUStatusWorker*> (m_rtuStatus);

		if (0 != statuswordnumber)
		{
			return rtuStatus->getIsCommsWithStatusWordNumber(statuswordnumber);
		}
		else
		{
			return rtuStatus->getIsCommsWithStationSystemsValid(slaveID);
		}
	}
	
	bool SteRTU::getIsCommsOK() const
	{
		TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in SteRTU::getIsCommsOK");
 		SteRTUStatusWorker* rtuStatus = dynamic_cast<SteRTUStatusWorker*> (m_rtuStatus);
 		return rtuStatus->getIsCommsOK();
	}
	
	bool SteRTU::getIsRTUSwitching() const
	{
        TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in SteRTU::getIsRTUSwitching");
 		SteRTUStatusWorker* rtuStatus = dynamic_cast<SteRTUStatusWorker*> (m_rtuStatus);
       return rtuStatus->getIsRTUSwitching();
	}

	bool SteRTU::getIsPrimaryRTU() const
	{
		TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in SteRTU::getIsPrimaryRTU");
 		SteRTUStatusWorker* rtuStatus = dynamic_cast<SteRTUStatusWorker*> (m_rtuStatus);
		return rtuStatus->getIsPrimaryRTU();
	}

	//TD10586
	bool SteRTU::getIsDownloadRequired() const
	{
        TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in SteRTU::getIsDownloadRequired");
 		SteRTUStatusWorker* rtuStatus = dynamic_cast<SteRTUStatusWorker*> (m_rtuStatus);
        return rtuStatus->getIsDownloadRequired();
	}
	

	int SteRTU::getSWCWeightage() const
	{
        TA_ASSERT(NULL != m_rtuStatus, "m_rtuStatus is NULL in SteRTU::getSWCWeightage");
 		SteRTUStatusWorker* rtuStatus = dynamic_cast<SteRTUStatusWorker*> (m_rtuStatus);
        return rtuStatus->getSWCWeightage();
	}


	bool SteRTU::resetRTU()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::resetRTU");
        return m_rtuCommand->resetRTU();
	}


	bool SteRTU::switchPrimaryToStandby()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::switchPrimaryToStandby");
        return m_rtuCommand->switchPrimaryToStandby();
	}


	bool SteRTU::switchStandbyToPrimary()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::switchStandbyToPrimary");
        return m_rtuCommand->switchStandbyToPrimary();
	}


	bool SteRTU::setDownloadRequired()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::setDownloadRequired");
        return m_rtuCommand->setDownloadRequired();
	}

	//TD10586
	bool SteRTU::setDownloadCompleted(bool isSucceeded)
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::setDownloadCompleted");

		if (isSucceeded == true)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Downloading config file succeeded, to send download completed and reset SteRTU.");
			m_rtuCommand->setDownloadCompleted();

			//if reseting rtu very soon after download completed, RTU will lost the config file, don't know why...
			//So wait max 5 second for rtu getting ready before reset.
			for (int i = 10; i > 0; -- i)
			{
				TA_Base_Core::Thread::sleep(500);
				
				if (getIsDownloadRequired() == false)
				{
					break;
				}
			}
			
			m_isUploadingConfigFile = false;

			return m_rtuCommand->resetRTU();
        }
		
		m_isUploadingConfigFile = false;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Downloading config file failed, need to check if the source file exists and rtu ftp server can be connected.");
		return isSucceeded;
	}

	//TD10586
	bool SteRTU::uploadConfigFile()
	{
		if (m_isUploadingConfigFile == true)
		{
			//if download is in processing, do nothing
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Download is in progress...");
			return true;
		}

		//to indicate download is in processing
		m_isUploadingConfigFile = true;
		
		//to start download process.
		m_rtuDownloadWorker.startDownload();

		return true;
	}
	
	bool SteRTU::enablePOWER1Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePOWER1Polling");
        return m_rtuCommand->enablePOWER1Polling();
	}


	bool SteRTU::enablePOWER2Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePOWER2Polling");
        return m_rtuCommand->enablePOWER2Polling();
	}


	bool SteRTU::enablePOWER3Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePOWER3Polling");
        return m_rtuCommand->enablePOWER3Polling();
	}


	bool SteRTU::enableCLOCKPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enableCLOCKPolling");
        return m_rtuCommand->enableCLOCKPolling();
	}


	bool SteRTU::enableWILDPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enableWILDPolling");
        return m_rtuCommand->enableWILDPolling();
	}


	bool SteRTU::enableECSPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enableECSPolling");
        return m_rtuCommand->enableECSPolling();
	}


	bool SteRTU::enableFPSPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enableFPSPolling");
        return m_rtuCommand->enableFPSPolling();
	}


	bool SteRTU::enableTWPPolling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enableTWPPolling");
        return m_rtuCommand->enableTWPPolling();
    }


	bool SteRTU::enablePSD1Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePSD1Polling");
        return m_rtuCommand->enablePSD1Polling();
	}
		

	bool SteRTU::enablePSD2Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePSD2Polling");
        return m_rtuCommand->enablePSD2Polling();
	}


	bool SteRTU::enablePLC1Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePLC1Polling");
        return m_rtuCommand->enablePLC1Polling();
	}


	bool SteRTU::enablePLC2Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePLC2Polling");
        return m_rtuCommand->enablePLC2Polling();
	}


	bool SteRTU::enablePLC3Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePLC3Polling");
        return m_rtuCommand->enablePLC3Polling();
	}


	bool SteRTU::enablePLC4Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::enablePLC4Polling");
        return m_rtuCommand->enablePLC4Polling();
	}


	bool SteRTU::inhibitPOWER1Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPOWER1Polling");
		return m_rtuCommand->inhibitPOWER1Polling();
	}


	bool SteRTU::inhibitPOWER2Polling()
	{
        TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPOWER2Polling");
        return m_rtuCommand->inhibitPOWER2Polling();
	}


	bool SteRTU::inhibitPOWER3Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPOWER3Polling");
        return m_rtuCommand->inhibitPOWER3Polling();
	}


	bool SteRTU::inhibitCLOCKPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitCLOCKPolling");
        return m_rtuCommand->inhibitCLOCKPolling();
	}


	bool SteRTU::inhibitWILDPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitWILDPolling");
        return m_rtuCommand->inhibitWILDPolling();
	}


	bool SteRTU::inhibitECSPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitECSPolling");
        return m_rtuCommand->inhibitECSPolling();
	}


	bool SteRTU::inhibitFPSPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitFPSPolling");
        return m_rtuCommand->inhibitFPSPolling();
	}


	bool SteRTU::inhibitTWPPolling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitTWPPolling");
        return m_rtuCommand->inhibitTWPPolling();
	}


	bool SteRTU::inhibitPSD1Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPSD1Polling");
        return m_rtuCommand->inhibitPSD1Polling();
	}


	bool SteRTU::inhibitPSD2Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPSD2Polling");
        return m_rtuCommand->inhibitPSD2Polling();
	}


	bool SteRTU::inhibitPLC1Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPLC1Polling");
        return m_rtuCommand->inhibitPLC1Polling();
	}


	bool SteRTU::inhibitPLC2Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPLC2Polling");
        return m_rtuCommand->inhibitPLC2Polling();
	}


	bool SteRTU::inhibitPLC3Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPLC3Polling");
        return m_rtuCommand->inhibitPLC3Polling();
    }


	bool SteRTU::inhibitPLC4Polling()
	{
		TA_ASSERT(NULL != m_rtuCommand, "m_rtuCommand is NULL in SteRTU::inhibitPLC4Polling");
        return m_rtuCommand->inhibitPLC4Polling();
	}

	unsigned int SteRTU::getRTUFtpPort()
	{
		return m_rtuEntityData->getRTUFtpPort();
	}

	std::string SteRTU::getRTUFtpUsername()
	{
		return m_rtuEntityData->getRTUFtpUsername();
	}

	std::string SteRTU::getRTUFtpPassword()
	{
		return m_rtuEntityData->getRTUFtpPassword();
	}

	std::string SteRTU::getRTUConfigFilePath()
	{
		return m_rtuEntityData->getRTUConfigFilePath();
	}

	std::string SteRTU::getPMSConfigFilePath()
	{
		return m_rtuEntityData->getPMSConfigFilePath();
	}
}
