/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PScadaAgent/src/PScadaUnit.cpp $
  * @author:	HoaVu
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

#include <sstream>

#include "app/scada/PScadaAgent/src/PScadaUnit.h"
#include "app/scada/PScadaAgent/src/PScadaManager.h"

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
namespace TA_IRS_App
{
	bool PScadaUnit::m_stopProcessing = false;
	
    PScadaUnit::PScadaUnit (PScadaManager& pscadaManager, TA_Base_Core::RTUEntityDataPtr pscadaEntityData)
	    :
        m_pscadaManager(pscadaManager),
        m_pscadaEntityData(pscadaEntityData),
	    m_name(pscadaEntityData->getName()),
	    m_address(pscadaEntityData->getAddress()),
        m_primaryKey(pscadaEntityData->getKey()),
        m_typeKey(pscadaEntityData->getTypeKey()),
        m_locationKey(pscadaEntityData->getLocation()),
        m_subsystemKey(pscadaEntityData->getSubsystem()),
//        m_inService(pscadaEntityData->getInServiceState()),
		m_inService(true),
        m_isInControlMode(false),
        m_pollingStarted(false),
        m_commsErrorAlarmGenerated(false),
        //m_isStandardTcpModbusDevice(false),
        //m_tcpNoDelay(pscadaEntityData->getTcpNoDelay()),
        //m_slaveID(pscadaEntityData->getSlaveID()),
		m_slaveID(1),
	    //m_commsErrorRetries(pscadaEntityData->getCommsErrorRetries()),
		m_commsErrorRetries(3),
		//m_pollTimeout( pscadaEntityData->getPollTimeout() ),
		//m_commandTimeout( pscadaEntityData->getCommandTimeout() ),
        //m_analogueDpUpdateInterval(0),
		m_qualityStatus(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED),
	    //TD18093,jianghp, m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),
	    m_auditMessageSender(NULL),
		m_pscadaCommonAddr(1),
		m_104ProtocolInterface(NULL),
		m_isAllDataTransfered(false),
		m_isKwhDataTransfered(false),
		m_is104Started(false)
	{
		FUNCTION_ENTRY("PScadaUnit::PScadaUnit");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Start to new PScadaUnit");
		m_pollAllDataTimeInSec = pscadaEntityData->getPollAllDataTimeInSec();
		m_pollKwhDataTimeInSec = pscadaEntityData->getPollKwhDataTimeInSec();
		m_dataLostDurableTimeInsec = pscadaEntityData->getDataLostDurableTimeInSec();
		m_updateLostPeriodInSec	= pscadaEntityData->getUpdateLostPeriodInSec();
		m_pscadaEventHandler = new PScadaEventHandler();
		try
		{
			IEC104_API::init();
			m_104ProtocolInterface =  IEC104_API::getInstance();

			if (NULL != m_104ProtocolInterface)
			{
				initializePScadaUnit();

				//m_104ProtocolInterface->setLocalAddr("178.15.10.11");
				m_104ProtocolInterface->addPScadaLink(1,pscadaEntityData->getAddress().c_str());
				addPScadaLink(1,pscadaEntityData->getAddress());

				std::string secondAddress = pscadaEntityData->getSecondAddress();
				if(""!=secondAddress)
				{
					m_104ProtocolInterface->addPScadaLink(1,secondAddress.c_str());
					addPScadaLink(1,secondAddress);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "add second address %s to pscadalink",secondAddress.c_str());
				}


				//m_104ProtocolInterface->addPScadaLink(1,pscadaEntityData->getSecondAddress());
				m_104ProtocolInterface->setLogCallBackFunc(TA_IRS_App::PScadaUnit::logRecord);

				m_104ProtocolInterface->registerHandler(m_pscadaEventHandler);
				//m_104ProtocolInterface->setWorkMode(0);
				//m_104ProtocolInterface->Start();
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Get IEC104 Library Instance failed");

			}

			m_pscadaEventHandler->setPScadaUnit(this);
			m_pscadaEventHandler->setMsgTimer(&m_pscadaMsgTimer);
			m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::PMSAgentAudit::Context);
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "New PScadaUnit failed.");

		}
		FUNCTION_EXIT("PScadaUnit::PScadaUnit");

    }

    void PScadaUnit::createPScadaWorkers()
    {
		FUNCTION_ENTRY("createPScadaWorkers");
		FUNCTION_EXIT("createPScadaWorkers");
        
    }

	void PScadaUnit::initializePScadaUnit()
	{
		FUNCTION_ENTRY("initializePScadaUnit");
		if (m_104ProtocolInterface != NULL)
		{
			m_104ProtocolInterface->setT0();
			m_104ProtocolInterface->setT1();
			m_104ProtocolInterface->setT2();
			m_104ProtocolInterface->setT3();
			m_104ProtocolInterface->setRtuAddr(1);
			m_104ProtocolInterface->setK();
			m_104ProtocolInterface->setW();
			m_104ProtocolInterface->setCotLen();
			m_104ProtocolInterface->setAsduAddrLen();
			m_104ProtocolInterface->setInfoAddrLen();
			m_104ProtocolInterface->setSockReadRetries(3);

			int t4 = this->getPollAllDataTimeInSec();
			if(t4 > 0)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "pscadaunit set t4 to [%d]",t4);			
				m_104ProtocolInterface->setT4(t4);
			}
			else
			{
				m_104ProtocolInterface->setT4();
			}
		}
		FUNCTION_EXIT("initializePScadaUnit");
	}

	PScadaUnit::~PScadaUnit()
	{
		FUNCTION_ENTRY("PScadaUnit::~PScadaUnit");
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
		//IEC104_API::cleanUp();

		if (NULL != m_104ProtocolInterface)
		{
			m_104ProtocolInterface->stop();	
			m_104ProtocolInterface = NULL;
		}
        
		if (m_auditMessageSender != NULL)
		{
			delete m_auditMessageSender;
			m_auditMessageSender = NULL;
		}
		FUNCTION_EXIT("PScadaUnit::~PScadaUnit");
   }


	const std::string & PScadaUnit::getName() const
	{
		FUNCTION_ENTRY("getName");
		FUNCTION_EXIT("getName");
		return m_name;
	}


	const std::string & PScadaUnit::getAddress() const
	{
		FUNCTION_ENTRY("getAddress");
		FUNCTION_EXIT("getAddress");
		return m_address;
	}


	unsigned long PScadaUnit::getPKey() const
	{
		FUNCTION_ENTRY("getPKey");
		FUNCTION_EXIT("getPKey");
		return m_primaryKey;
	}


	unsigned long PScadaUnit::getTypeKey() const
	{
		FUNCTION_ENTRY("getTypeKey");
		FUNCTION_EXIT("getTypeKey");
		return m_typeKey;
	}


	unsigned long PScadaUnit::getLocationKey() const
	{
		FUNCTION_ENTRY("getLocationKey");
		FUNCTION_EXIT("getLocationKey");
		return m_locationKey;
	}


	unsigned long PScadaUnit::getSubsystemKey() const
	{
		FUNCTION_ENTRY("getSubsystemKey");
		FUNCTION_EXIT("getSubsystemKey");
		return m_subsystemKey;
	}

    
    int PScadaUnit::getSlaveID() const
    {
		FUNCTION_ENTRY("getSlaveID");
		FUNCTION_EXIT("getSlaveID");
       return m_slaveID;
    }

	int PScadaUnit::getPollAllDataTimeInSec()
	{
		FUNCTION_ENTRY("getPollAllDataTimeInSec");
		FUNCTION_EXIT("getPollAllDataTimeInSec");
       	return m_pollAllDataTimeInSec;	
	}

		
	int PScadaUnit::getPollKwhDataTimeInSec()
	{
		FUNCTION_ENTRY("getPollKwhDataTimeInSec");
		FUNCTION_EXIT("getPollKwhDataTimeInSec");
       	return m_pollKwhDataTimeInSec;
	}

	int PScadaUnit::getDataLostDurableTimeInsec()
	{
		FUNCTION_ENTRY("getDataLostDurableTimeInsec");
		FUNCTION_EXIT("getDataLostDurableTimeInsec");
		return m_dataLostDurableTimeInsec;
	}
	
	int PScadaUnit::getUpdateLostPeriodInSec()
	{
		FUNCTION_ENTRY("getUpdateLostPeriodInSec");
		FUNCTION_EXIT("getUpdateLostPeriodInSec");
		return m_updateLostPeriodInSec;
	}

    void PScadaUnit::setAllDataTransfered(bool isFinished)
    {
    	FUNCTION_ENTRY("setAllDataTransfered");
    	m_isAllDataTransfered= isFinished;
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "set allDataTransfered = %d",isFinished);
		FUNCTION_EXIT("setAllDataTransfered");
    }

	bool PScadaUnit::isAllDataTransfered()
	{
		FUNCTION_ENTRY("isAllDataTransfered");
		FUNCTION_EXIT("isAllDataTransfered");
    	return m_isAllDataTransfered;
	}

	void PScadaUnit::setKwhDataTransfered(bool isFinished)
	{
		FUNCTION_ENTRY("setKwhDataTransfered");
    	m_isKwhDataTransfered= isFinished;
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "set KwhDataTransfered = %d",isFinished);
		FUNCTION_EXIT("setKwhDataTransfered");
	}

	bool PScadaUnit::isKwhDataTransfered()
	{
		FUNCTION_ENTRY("isKwhDataTransfered");
		FUNCTION_EXIT("isKwhDataTransfered");
		return m_isKwhDataTransfered;
	}
	void PScadaUnit::updateCommsAlarm(bool raiseAlarm, const std::string & servicePortNumber, const std::string & additionalComment)
    {
		FUNCTION_ENTRY("updateCommsAlarm");
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
                    //submit an unique PSCADA comms alarm
                    TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::PMSAgentAlarms::RTUCommsError,
                                                m_primaryKey,				// entity key
												m_typeKey,					// entity type key
                                                descriptionParameters,
                                                m_name,						// entity name
                                                m_locationKey,				// location key
                                                m_subsystemKey,				// subystem key
												m_pscadaManager.getAgentAssetName(),
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
            // Else if the current PScadaUnit state is good then close the comms alarms
            // and log a new event
			//
			// if alarm and event have been generated
			if (true == m_commsErrorAlarmGenerated)
			{
				// close the unique PScadaUnit Comms alarm
				TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Base_Core::PMSAgentAlarms::RTUCommsError,
                                                     m_primaryKey,
                                                     m_locationKey);
                
                // set up the flags
				m_commsErrorAlarmGenerated = false;
			}
		}
		FUNCTION_EXIT("updateCommsAlarm");
   }


    void PScadaUnit::submitAuditMessage(const TA_Base_Core::DescriptionParameters & description,
                                  const TA_Base_Core::MessageType & messageType)
    {
		FUNCTION_ENTRY("submitAuditMessage");
        TA_ASSERT(NULL != m_auditMessageSender, "m_auditMessageSender is NULL in PScadaUnit::submitAuditMessage");
        
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
		FUNCTION_EXIT("submitAuditMessage");
    }


    bool PScadaUnit::isInService() const
	{
		FUNCTION_ENTRY("isInService");
		FUNCTION_EXIT("isInService");
		return m_inService;
	}


	void PScadaUnit::start()
	{
		FUNCTION_ENTRY("start");
		FUNCTION_EXIT("start");
	}


	void PScadaUnit::stop()
	{
		FUNCTION_ENTRY("stop");

		m_stopProcessing = true;
		
		if (NULL != m_104ProtocolInterface)
		{
			m_104ProtocolInterface->setWorkMode(0);
		}
		
		if (NULL != m_pscadaEventHandler)
		{
			m_pscadaEventHandler->stop();
		}

		FUNCTION_EXIT("stop");
	}

	bool PScadaUnit::isValid()
	{
		FUNCTION_ENTRY("isValid");
		FUNCTION_EXIT("isValid");
		return true;
	}


	void PScadaUnit::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("update");
		FUNCTION_EXIT("update");
		// to be implemented
	}


	void PScadaUnit::remove()
	{
		FUNCTION_ENTRY("remove");
		FUNCTION_EXIT("remove");
		// do nothing.  Destructor will clean up the data point
	}

	void PScadaUnit::setToControlMode()
	{
		FUNCTION_ENTRY("setToControlMode");
       // set the flag
		m_isInControlMode = true;
		if (m_104ProtocolInterface != NULL)
		{
			m_104ProtocolInterface->setWorkMode(1);
			if(false == m_is104Started)
			{
				m_104ProtocolInterface->Start();
				m_is104Started = true;
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "IEC104 has set to control mode");
		}

		m_pscadaEventHandler->setOperationMode(true);
		// if the PScadaUnit is IN_SERVICE
		if (true == m_inService)
		{
			//startProcessing();
		}
		FUNCTION_EXIT("setToControlMode");
	}


	void PScadaUnit::setToMonitorMode()
	{
		FUNCTION_ENTRY("setToMonitorMode");
        // set the flag
		m_isInControlMode = false;
		if (m_104ProtocolInterface != NULL)
		{
			m_104ProtocolInterface->setWorkMode(0);
			if(false == m_is104Started)
			{
				m_104ProtocolInterface->Start();
				m_is104Started = true;
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "IEC104 has set to Monitor mode");
		}

		m_pscadaEventHandler->setOperationMode(false);
		//stopProcessing();
		FUNCTION_EXIT("setToMonitorMode");
	}


	bool PScadaUnit::getIsInControlMode()
	{
		FUNCTION_ENTRY("getIsInControlMode");
		FUNCTION_EXIT("getIsInControlMode");
		return m_isInControlMode;
	}


	bool PScadaUnit::addDataPoint(TA_Base_Bus::DataPoint * dp)
	{
		FUNCTION_ENTRY("addDataPoint");
        TA_ASSERT(NULL != dp, "dp is NULL in PScadaUnit::addDataPoint");

        // only hold output datapoints
        bool isAdded = false;
        if (dp->isWriteable())
        {
			m_OutputDataPoints[ dp->getEntityKey() ] = dp;
			m_pscadaEventHandler->addDataPoint(dp);
			isAdded = true;
		}
        else
        {
			isAdded = m_pscadaEventHandler->addDataPoint(dp);
        }
		FUNCTION_EXIT("addDataPoint");

        return isAdded;
	}


	void PScadaUnit::removeDataPoint (TA_Base_Bus::DataPoint * dp)
	{
		FUNCTION_ENTRY("removeDataPoint");
		TA_ASSERT(NULL != dp, "dp is NULL in PScadaUnit::removeDataPoint");

		PScadaDataPointMap::iterator itr = m_OutputDataPoints.find( dp->getEntityKey() );

		if ( itr != m_OutputDataPoints.end() )
		{
			m_OutputDataPoints.erase( itr );
			return;
		}

		m_pscadaEventHandler->removeDataPoint(dp);
		FUNCTION_EXIT("removeDataPoint");
	}

	//TD10586
	bool PScadaUnit::processWriteRequest(TA_Base_Bus::DataPointWriteRequest & newWrite)
	{
		FUNCTION_ENTRY("processWriteRequest");

		// get the data point from the new request
		TA_Base_Bus::DataPoint * dp = newWrite.getDataPoint();

		TA_ASSERT( 0 != dp, "PScadaUnit::processWriteRequest: newWrite contains a NULL datapoint." );

		PScadaDataPointMap::iterator itr = m_OutputDataPoints.find( dp->getEntityKey() );

		if ( itr == m_OutputDataPoints.end() )
		{
			return false;
		}


		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "%s, : start writing process for %s",
			getName().c_str(),
			dp->getDataPointName().c_str() );

		if ( dp->getIsDigitalDataPoint() )
		{
			TA_Base_Bus::EControlOperation controlOperation = newWrite.getValue().getControlOperation();
			bool bOperation = newWrite.getValue().getBoolean();
			unsigned long  nPointAddr = dp->getDataPointRegisterAddress();
			
			b_ui8 iOperation = 0;

			if (bOperation)
				iOperation = 1;
			else
				iOperation = 0;

		
			switch(controlOperation)
			{
			case TA_Base_Bus::CONTROL_SELECT:
				{
					m_104ProtocolInterface->singleCommandSelect(m_pscadaCommonAddr,nPointAddr,iOperation);
					m_pscadaMsgTimer.addMessage(dp,TA_Base_Bus::selectCmdConfirmTimeout);
				}
				break;

			case TA_Base_Bus::CONTROL_EXECUTE:
				{
					m_pscadaMsgTimer.removeMessage(dp,TA_Base_Bus::operateCmdTimeout);
					m_104ProtocolInterface->singleCommandExec(m_pscadaCommonAddr,nPointAddr,iOperation);
					m_pscadaMsgTimer.addMessage(dp,TA_Base_Bus::execCmdConfirmTimeout);

					dp->SetAbeExpressionValue(bOperation);
					dp->setSession(newWrite.getSessionID());

				}
				break;

			case TA_Base_Bus::CONTROL_CANCEL:
				{
					m_pscadaMsgTimer.removeMessage(dp,TA_Base_Bus::operateCmdTimeout);
					m_104ProtocolInterface->singleCommandUndo(m_pscadaCommonAddr,nPointAddr,iOperation);
					m_pscadaMsgTimer.addMessage(dp,TA_Base_Bus::cancelCmdConfirmTimeout);
				}
				break;
			
			default:
				break;
			}

			//writeDigitalDataPoint( newWrite );
		}
		FUNCTION_EXIT("processWriteRequest");

		return true;

	}

    void PScadaUnit::updatePScadaUnitConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
    {
		FUNCTION_ENTRY("updatePScadaUnitConfiguration");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PScadaUnit [%s] received a config update event", getName().c_str());
        
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
                        m_pscadaEntityData->invalidate();
                        
                        for (unsigned long i = 0; i < changes->size(); i++)
                        {
                            if (TA_Base_Core::RTUEntityData::INSERVICE == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating InService state of %s", getName().c_str());
                                updateInServiceParameter (m_pscadaEntityData->getInServiceState());
                            }
                            else if (TA_Base_Core::RTUEntityData::SCANTIME == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating ScanTime of %s", getName().c_str());
                                updateScanTimeMSecs (m_pscadaEntityData->getScanTimeMSecs());
                            }
                            else if (TA_Base_Core::RTUEntityData::COMMS_ERROR_RETRIES == (*changes)[i])
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Updating CommsErrorRetries of %s", getName().c_str());
                                updateCommsErrorRetries (m_pscadaEntityData->getCommsErrorRetries());
                            }
                        }
                    }
                    catch (...)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught when process PScadaUnit entity data");
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
		FUNCTION_EXIT("updatePScadaUnitConfiguration");
   }
        
	void PScadaUnit::updateInServiceParameter(bool newState)
	{
		FUNCTION_ENTRY("updateInServiceParameter");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for InService parameter...");

		// save the new state
		m_inService = newState;

		// if the PScadaUnit has been in control mode
		if (true == m_isInControlMode && true == m_inService)
		{
			//startProcessing();
        }
		else
		{
			//stopProcessing();
		}
		FUNCTION_EXIT("updateInServiceParameter");
	}


	void PScadaUnit::updateScanTimeMSecs(int scanTimeMSecs)
	{
		FUNCTION_ENTRY("updateScanTimeMSecs");
		FUNCTION_EXIT("updateScanTimeMSecs");
	}


	void PScadaUnit::updateCommsErrorRetries(int retries)
	{
        //TA_ASSERT(NULL != m_rtuPolling, "m_rtuPolling is NULL in PScadaUnit::updateCommsErrorRetries");
		FUNCTION_ENTRY("updateCommsErrorRetries");

        if (m_commsErrorRetries != retries)
        {
            m_commsErrorRetries = retries;
        }
		FUNCTION_EXIT("updateCommsErrorRetries");
	}

	int PScadaUnit::getMaxCommsRetries() const
	{
		FUNCTION_ENTRY("getMaxCommsRetries");
		FUNCTION_EXIT("getMaxCommsRetries");
		return m_commsErrorRetries;
	}

	//Using for IEC 104 Protocol
	void PScadaUnit::addLink( TA_Base_Core::RTUEntityDataPtr pscadaEntityData )
	{
		FUNCTION_ENTRY("addLink");
		PScadaLinkStatus pscadaLinkStatus;
		pscadaLinkStatus.nGroupID = 2;
		
		pscadaLinkStatus.bLinkState = false;
		pscadaLinkStatus.bMasterConnection = false;
		
		std::string strIpAddress = pscadaEntityData->getAddress();
		if( 0 <= strIpAddress.size() )
		{
			m_104ProtocolInterface->addPScadaLink(2,strIpAddress);
			pscadaLinkStatus.strIPAddress = strIpAddress;
			m_vLinkStatus.push_back(pscadaLinkStatus);
		}
		strIpAddress = pscadaEntityData->getSecondAddress();
		if(0 <= strIpAddress.size())
		{
			m_104ProtocolInterface->addPScadaLink(2,strIpAddress);
			pscadaLinkStatus.strIPAddress = strIpAddress;
			m_vLinkStatus.push_back(pscadaLinkStatus);
		}
		FUNCTION_EXIT("addLink");
	}

	void PScadaUnit::logRecord(int level,std::string &str)
	{
		if ( m_stopProcessing ) return;

		std::string strLog;
		strLog = str;
		switch(level)
		{
			case LOG_LEVEL_DEBUG:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, strLog.c_str());
				break;
			case LOG_LEVEL_INFO:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, strLog.c_str());
				break;
			case LOG_LEVEL_WARNING:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, strLog.c_str());
				break;
			case LOG_LEVEL_ERROR:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, strLog.c_str());
				break;
			default:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, strLog.c_str());
				break;
		}
	}

	void PScadaUnit::addPScadaLink(int nGroupID, std::string strIPAddress)
	{
		PScadaLinkStatus pscadaLinkStatus;
		
		pscadaLinkStatus.nGroupID = nGroupID;
		pscadaLinkStatus.strIPAddress = strIPAddress;
		pscadaLinkStatus.bLinkState = false;
		pscadaLinkStatus.bMasterConnection = false;

		m_vLinkStatus.push_back(pscadaLinkStatus);
	}

	void PScadaUnit::setPScadaLinkConnectStatus(int nGroupID, std::string strIPAddress,bool bLinkState)
	{
		std::vector<PScadaLinkStatus>::iterator linkIter;
		for (linkIter = m_vLinkStatus.begin();linkIter != m_vLinkStatus.end(); ++linkIter)
		{
			if ((*linkIter).nGroupID == nGroupID && (*linkIter).strIPAddress == strIPAddress)
			{
				(*linkIter).bLinkState = bLinkState;
			}
		}
	}

	void PScadaUnit::setPScadaLinkModeStatus(int nGroupID, std::string strIPAddress, bool bMasterLink)
	{
		std::vector<PScadaLinkStatus>::iterator linkIter;
		for (linkIter = m_vLinkStatus.begin();linkIter != m_vLinkStatus.end(); ++linkIter)
		{
			if ((*linkIter).nGroupID == nGroupID && (*linkIter).strIPAddress == strIPAddress)
			{
				(*linkIter).bMasterConnection = bMasterLink;
			}
		}
	}

	bool PScadaUnit::isMasterPScadaLink(int nGroupID, std::string& strIPAddress)
	{
		std::vector<PScadaLinkStatus>::iterator linkIter;
		for (linkIter = m_vLinkStatus.begin();linkIter != m_vLinkStatus.end(); ++linkIter)
		{
			if ((*linkIter).nGroupID == nGroupID && (*linkIter).strIPAddress == strIPAddress)
			{
				return (*linkIter).bMasterConnection;
			}
		}
		return false;
	}

	//Using for IEC 104 Protocol
	bool PScadaUnit::isLinkConnected()
	{
		bool bLinkConnected = false;
		std::vector<PScadaLinkStatus>::iterator linkIter;

		for (linkIter = m_vLinkStatus.begin();linkIter != m_vLinkStatus.end(); ++linkIter)
		{
			if ((*linkIter).bLinkState )
			{
				bLinkConnected = true;
				break;
			}
		}

		return bLinkConnected;
	}

	bool  PScadaUnit::processPollKwhData()
	{
		FUNCTION_ENTRY("processPollData");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "start polling KWH data");
		if(false == this->isLinkConnected())
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to poll KWH data, due to connect failed");
			return false;
		}
		if(false == m_isAllDataTransfered || false == m_isKwhDataTransfered)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to poll KWH data, due to call all data or kwh not finished");
			return false;
		}
		int ret = m_104ProtocolInterface->interrogateCounter(m_pscadaCommonAddr);
		if(ret < 0 )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed to poll KWH data");
			return false;
		}
		return true;

		FUNCTION_EXIT("processPollData");
	}
	
	void PScadaUnit::resetLink()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "HB--PScadaUnit reset current link and reconnect");
		m_104ProtocolInterface->resetLink();
	}
}
