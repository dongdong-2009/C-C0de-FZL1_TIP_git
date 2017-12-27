#include "bus/scada/102_library/src/IEC102Library.h"
#include "bus/scada/102_library/src/IEC102ControllerFactory.h"
#include "bus/scada/102_library/src/IEC102ConfigData.h"

namespace TA_Base_Bus
{
	IEC102Library::IEC102Library()
	:
	m_iec102Controller(NULL),
	m_channelManager(NULL),
	m_initSemphore(0),
	m_linkInitSuccess(false),
	m_bChannelStatus(false),
	m_running(false),
	m_observer(NULL),
	m_controllerFactory(NULL)
	{
		FUNCTION_ENTRY("IEC102Library()");
		m_channelManager = new ChannelManager(*this);
		
		initControllerFactory();
		FUNCTION_EXIT;
	}

	IEC102Library::~IEC102Library()
	{
		FUNCTION_ENTRY("~IEC102Library()");

		// remove handle to IEC102ControllerFactory
		TA_Base_Bus::IEC102ControllerFactory::removeInstance();
		m_controllerFactory = NULL;

		FUNCTION_EXIT;
	}

	void IEC102Library::handleConnectionEstablished()
	{
		if (!m_linkInitSuccess)
			m_initSemphore.post();
	}

	void IEC102Library::handleConnectionLost()
	{
		FUNCTION_ENTRY("handleConnectionLost()");
		TA_THREADGUARD( m_usingControllerLock );
		m_linkInitSuccess = false;
		if (m_iec102Controller != NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Notify controller for connection lost");
			m_iec102Controller->updateLinkControlState(CONNECTION_LOST);
		}
		FUNCTION_EXIT;
	}
	
	void IEC102Library::handleStopProcess()
	{
		FUNCTION_ENTRY("handleStopProcess()");
		TA_THREADGUARD( m_usingControllerLock );
		
		if (m_iec102Controller != NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Notify controller for stop process");
			m_iec102Controller->updateLinkControlState(STOP_PROCESS);
		}
		FUNCTION_EXIT;
	}

	void IEC102Library::handleWriteFailed()
	{
		FUNCTION_ENTRY("handleWriteFailed()");
		TA_THREADGUARD( m_usingControllerLock );
		
		if (m_iec102Controller != NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Notify controller for write failed");
			m_iec102Controller->updateLinkControlState(WRITE_FAILED);
		}
		FUNCTION_EXIT;
	}

	void IEC102Library::destroyController()
	{
		FUNCTION_ENTRY("destroyController()");
		TA_THREADGUARD( m_usingControllerLock );
		
		if (m_iec102Controller != NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Notify controller for destroy controller");
			delete m_iec102Controller;
			m_iec102Controller = NULL;
		}
		FUNCTION_EXIT;
	}
	
	void IEC102Library::setupConfiguration(const IEC102ConfigData& objConfigData )
	{
		FUNCTION_ENTRY("setupConfiguration()");
		m_objConfigData = objConfigData;
		
		m_channelManager->createChannel(objConfigData.m_address, objConfigData.m_port);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Adding Channel for %s:%s Successfully",objConfigData.m_address.c_str(), objConfigData.m_port.c_str());
		FUNCTION_EXIT;	
	}
	
	void IEC102Library::startProcess()
	{
		FUNCTION_ENTRY("startProcess()");
		m_channelManager->startProcess();
		start();
		FUNCTION_EXIT;
	}
	
	void IEC102Library::stopProcess()
	{
		FUNCTION_ENTRY("stopProcess()");
		handleStopProcess();
		m_channelManager->stopProcess();
		terminateAndWait();
		FUNCTION_EXIT;
	}

	void IEC102Library::terminate()
	{
		FUNCTION_ENTRY("terminate()");
		m_running = false;
		m_linkInitSuccess = false;
		m_initSemphore.post();
		FUNCTION_EXIT;
	}

	void IEC102Library::run()
	{
		FUNCTION_ENTRY("run()");
		m_running = true;

		while(m_running)
		{
			m_initSemphore.wait();
			if (!m_linkInitSuccess)
			{
				bool initResult = initLink();
				// Because m_initSemphore just get tcp/ip message, can't be processing when initlink failed, so we need to post to retry initLink again
                if ( initResult == false )
                {
                    m_initSemphore.post();
                }
			}
		}
		FUNCTION_EXIT;
	}

	bool IEC102Library::initLink()
	{
		FUNCTION_ENTRY("initLink()");

		m_linkInitSuccess = false; 

		TA_THREADGUARD( m_controllerLock );

		if (getChannelStatus())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus is true");
			if (m_iec102Controller == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Creating Init Controller");
				m_iec102Controller = TA_Base_Bus::IEC102ControllerFactory::getInstance().createIEC102ControllerInit(*m_channelManager,m_objConfigData);
				startFlow();
				
				m_linkInitSuccess = m_energyData.bSuccess;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Result of initLink is %d", m_linkInitSuccess);
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Controller is not NULL at Init");
				m_linkInitSuccess = false; 
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus is false");
			m_linkInitSuccess = false; 
		}
		FUNCTION_EXIT;
		return m_linkInitSuccess; 
	}	

	void IEC102Library::callLinkStatus()
	{
		FUNCTION_ENTRY("callLinkStatus()");

		TA_THREADGUARD( m_controllerLock );

		if (getChannelStatus())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus is true");
			if (m_iec102Controller == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Creating Init Controller for link status");
				m_iec102Controller = TA_Base_Bus::IEC102ControllerFactory::getInstance().createIEC102ControllerInit(*m_channelManager,m_objConfigData,false);
				startFlow();
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Finish link status refresh.");
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Controller is not NULL at Call link status");
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus is false");
		}
		FUNCTION_EXIT;
	}	
	
	FrameEnergyData IEC102Library::getPowerEnergy_C_CI_NR_2(unsigned short startAddr, unsigned short endAddr, time_t startTime, time_t endTime, unsigned char rad )
	{
		FUNCTION_ENTRY("getPowerEnergy_C_CI_NR_2()");
		clearEnergyData();

		//must make sure the input parameter is valid.
		if ((endAddr <= startAddr) || (endTime <= startTime))
		{
			m_energyData.bSuccess = false;
			return m_energyData;
		}
		
		TA_THREADGUARD( m_controllerLock );

		bool connectionStatus = getChannelStatus();
		if (connectionStatus && m_linkInitSuccess)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus and m_linkInitSuccess are true");
			if (m_iec102Controller == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Creating 120 Controller");
				m_iec102Controller = TA_Base_Bus::IEC102ControllerFactory::getInstance().createIEC102Controller120( *m_channelManager,m_objConfigData,startAddr, endAddr, startTime, endTime, rad);
				startFlow();
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Controller is not NULL at 120");
				m_energyData.bSuccess = false;
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus(%d) or m_linkInitSuccess(%d) are false", connectionStatus, m_linkInitSuccess );
			m_energyData.bSuccess = false;
		}
		FUNCTION_EXIT;
		return m_energyData; 
	}

	FrameEnergyData IEC102Library::getRemoteMeasurement_C_YC_NA_2( unsigned short startAddr, unsigned short endAddr )
	{
		FUNCTION_ENTRY("getRemoteMeasurement_C_YC_NA_2()");
		clearEnergyData();

		//must make sure the input parameter is valid.
		if (endAddr <= startAddr)
		{
			m_energyData.bSuccess = false;
			return m_energyData;
		}
		
		TA_THREADGUARD( m_controllerLock );

		bool connectionStatus = getChannelStatus();
		if (connectionStatus && m_linkInitSuccess)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus and m_linkInitSuccess are true");
			if (m_iec102Controller == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Creating 171 Controller");
				m_iec102Controller = TA_Base_Bus::IEC102ControllerFactory::getInstance().createIEC102Controller171( *m_channelManager,m_objConfigData,startAddr, endAddr);
				startFlow();
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Controller is not NULL at 171");
				FUNCTION_EXIT;
				m_energyData.bSuccess = false;
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus(%d) or m_linkInitSuccess(%d) are false", connectionStatus, m_linkInitSuccess );
			FUNCTION_EXIT;
			m_energyData.bSuccess = false;
		}
		FUNCTION_EXIT;
		return m_energyData;
	}

	FrameEnergyData IEC102Library::getRemoteMeasurement_C_YC_TA_2( unsigned short startAddr, unsigned short endAddr, time_t startTime, time_t endTime, unsigned char rad )
	{
		FUNCTION_ENTRY("getRemoteMeasurement_C_YC_TA_2()");
		clearEnergyData();

		//must make sure the input parameter is valid.
		if ((endAddr <= startAddr) || (endTime <= startTime))
		{
			m_energyData.bSuccess = false;
			return m_energyData;
		}
		
		TA_THREADGUARD( m_controllerLock );

		bool connectionStatus = getChannelStatus();
		if (connectionStatus && m_linkInitSuccess)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus and m_linkInitSuccess are true");
			if (m_iec102Controller == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Creating 172 Controller");
				m_iec102Controller = TA_Base_Bus::IEC102ControllerFactory::getInstance().createIEC102Controller172( *m_channelManager,m_objConfigData,startAddr, endAddr, startTime, endTime, rad);
				startFlow();
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Controller is not NULL at 172");
				m_energyData.bSuccess = false;
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ChannelStatus(%d) or m_linkInitSuccess(%d) are false", connectionStatus, m_linkInitSuccess );
			m_energyData.bSuccess = false;
		}
		FUNCTION_EXIT;
		return m_energyData;
	}
	
	bool IEC102Library::getChannelStatus()
	{
		m_bChannelStatus = m_channelManager->getChannelStatus();
		return m_bChannelStatus;
	}

	void IEC102Library::UpdateChannelStatus(const bool & status)
	{
		FUNCTION_ENTRY("UpdateChannelStatus()");
		processStatusForObserver(status);
		m_bChannelStatus = status;
		FUNCTION_EXIT;
	}

	void IEC102Library::processData(std::vector<unsigned char>& buffer)
	{
		FUNCTION_ENTRY("isDataValid()");
		TA_THREADGUARD( m_usingControllerLock );
		
		if (m_iec102Controller != NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Explain data of incomingBuffer");
			FUNCTION_EXIT;
			m_iec102Controller->processData(buffer);

		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unable to explain data due to no controller!!");	
			FUNCTION_EXIT;
		}
	}

	void IEC102Library::clearEnergyData()
	{
		FUNCTION_ENTRY("clearEnergyData()");
		m_energyData.bSuccess = false;
		m_energyData.PowerData.clear();
		FUNCTION_EXIT;
	}

	void IEC102Library::addEnergyData(const std::vector<PowerEnergyData>& energyData)
	{
		FUNCTION_ENTRY("addEnergyData()");
		for (unsigned int i = 0; i < energyData.size(); i++)
		{
			m_energyData.PowerData.push_back(energyData[i]);
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Added %d energy data.",energyData.size());	
		FUNCTION_EXIT;
	}

	void IEC102Library::registerIEC102Observer(IEC102Observer * observer)
	{
		FUNCTION_ENTRY("registerIEC102Observer()");
		if(observer == NULL)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Registration failed as Observer is NULL.");
		}
		m_observer = observer;
		FUNCTION_EXIT;
	}
	
	void IEC102Library::removeIEC102Observer(const IEC102Observer *observer)
	{
		FUNCTION_ENTRY("removeIEC102Observer()");
		// Kept input parameter for future use when multiple observers will be there.
		m_observer = NULL;
		FUNCTION_EXIT;
	}

	void IEC102Library::processStatusForObserver(const bool & status)
	{
		FUNCTION_ENTRY("processStatusForObserver()");	
		if( m_observer == NULL)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No observers.");
			return;
		}
		if(m_bChannelStatus != status)
		{
			//Update observer only when there is change in the status.		
			m_observer->updateChannelStatus(status);
		}
		FUNCTION_EXIT;
	}	
	
	void IEC102Library::initControllerFactory()
	{
		FUNCTION_ENTRY("initControllerFactory()");
		m_controllerFactory = &(TA_Base_Bus::IEC102ControllerFactory::getInstance());
		TA_ASSERT ( ( 0 != m_controllerFactory ), "IEC102 Controller Factory not created");
		FUNCTION_EXIT;
	}
	
	void IEC102Library::startFlow()
	{
		FUNCTION_ENTRY("startFlow()");
		bool bSuccess = true;
		//m_iec102Controller->setupConfiguration(m_objConfigData);
		bool bFlow = true;
		while(bFlow && bSuccess )
		{
			m_iec102Controller->setFlowInfo();
			bSuccess= m_iec102Controller->processFlow();

			if (bSuccess)
			{
				addEnergyData(m_iec102Controller->getEnergyData());
				m_iec102Controller->clearEnergyData();
				bFlow = m_iec102Controller->getNextFlow();
			}
		}
		destroyController();
		FUNCTION_EXIT;
		m_energyData.bSuccess = bSuccess;
	}

	ChannelManager* IEC102Library::getChannelManager()
	{
		return m_channelManager;
	}
	
}
