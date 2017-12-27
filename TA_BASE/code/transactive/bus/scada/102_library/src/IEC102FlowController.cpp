#include "bus/scada/102_library/src/IEC102FlowController.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/102_library/src/IEC102ConfigData.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	IEC102FlowController::~IEC102FlowController()
	{
		FUNCTION_ENTRY("~IEC102FlowController");
		FUNCTION_EXIT;
	}
	
	IEC102FlowController::IEC102FlowController(ChannelManager& channelManager,IEC102ConfigData& objConfigData)
		:m_channelManager(channelManager),
		 m_sendRetryTime(objConfigData.m_sendRetryTime),
		 m_messageResponseTime(objConfigData.m_messageResponseTime), // For simulation
		 m_remoteAddress(objConfigData.m_remoteAddress),
		 m_nextStep(STEP_START),
		 m_bFlowRunning(true),
		 m_102Protocol(NULL),
		 m_stateObserver(NULL)
	{
		FUNCTION_ENTRY("IEC102FlowController");
		m_lowRemoteAddress = static_cast<unsigned char>(m_remoteAddress & 0x00FF);
		m_highRemoteAddress = static_cast<unsigned char>((m_remoteAddress & 0xFF00) >> 8);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"m_sendRetryTime(%d),m_messageResponseTime(%d), m_remoteAddress(%d)", 
			m_sendRetryTime, m_messageResponseTime, m_remoteAddress); 
		
		FUNCTION_EXIT;				
	}
	
	void IEC102FlowController::setupConfiguration(const IEC102ConfigData& objConfigData )
	{
		FUNCTION_ENTRY("setupConfiguration()");
		m_sendRetryTime = objConfigData.m_sendRetryTime;
		m_messageResponseTime = objConfigData.m_messageResponseTime; // For simulation
		m_remoteAddress = objConfigData.m_remoteAddress; // For simulation
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_sendRetryTime(%d),m_messageResponseTime(%d), m_remoteAddress(%d)", m_sendRetryTime, m_messageResponseTime, m_remoteAddress); 
		FUNCTION_EXIT;	
	}

	unsigned char IEC102FlowController::getHighRemoteAddress()
	{
		return m_highRemoteAddress;
	}

	unsigned char IEC102FlowController::getLowRemoteAddress()
	{
		return m_lowRemoteAddress;
	}

	void IEC102FlowController::addEnergyData(const std::vector<PowerEnergyData>& energyData)
	{
		FUNCTION_ENTRY("addEnergyData()");
		for (unsigned int i = 0; i < energyData.size(); i++)
		{
			m_energyData.push_back(energyData[i]);
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Add %d energy data.",energyData.size());	
		FUNCTION_EXIT;
	}

	void IEC102FlowController::processData(std::vector<unsigned char>& buffer)
	{
		FUNCTION_ENTRY("processData()");
		m_102Protocol->processData(buffer);
		FUNCTION_EXIT;		
	}

	void IEC102FlowController::updateFlowControlState(EFlowControlState newState)
	{
		FUNCTION_ENTRY("processData()");
		TA_Base_Core::ThreadGuard guard(m_stateObserverLock);
		if (NULL != m_stateObserver)
		{
			m_stateObserver->notifyStateChange(newState);
		}
		FUNCTION_EXIT;		
	}

	void IEC102FlowController::updateLinkControlState(EFlowControlState newState)
	{
		FUNCTION_ENTRY("updateLinkControlState()");
		TA_Base_Core::ThreadGuard guard(m_stateObserverLock);
		if (NULL != m_stateObserver)
		{
			m_stateObserver->notifyStateChange(newState);
		}
		
		FUNCTION_EXIT;
	}
	
	bool IEC102FlowController::sendAndWait(std::string& sendData,EFlowControlState& replyState)
	{
		FUNCTION_ENTRY("sendAndWait()");
		bool bSendSuccess = false;
		int retryTime = m_sendRetryTime;
		while (!bSendSuccess && retryTime > 0 )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start sending,bSendSuccess = %s,retryTime = %d",bSendSuccess?"True":"False",m_sendRetryTime - retryTime);
			m_channelManager.writeDataToChannel(sendData);
			bool getReply = m_stateObserver->waitForReply();							//observer shall wait for reply until timeout, getReply=0 after timeout

			//If the state observer get the response within the timeout, will process the response.
			//If timeout, means can not receive the data, will retry according to the configurated retry times 
			if (getReply)
			{
				replyState = m_stateObserver->getNotifiedState();
				std::string strDesc = m_stateObserver->getControlStateDescription(replyState);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Got NotifiedState %d(%s)",replyState,strDesc.c_str());

				if (WRITE_FAILED != replyState)
				{
					bSendSuccess = true;
				}
			}

			retryTime--;
		}
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Finish sending,bSendSuccess = %s,retryTime = %d",bSendSuccess?"True":"False",retryTime);
		FUNCTION_EXIT;
		return bSendSuccess;
	}

	void IEC102FlowController::setStateObserver(ControlStateObserver* pObserver)
	{
		FUNCTION_ENTRY("setStateObserver()");
		TA_Base_Core::ThreadGuard guard(m_stateObserverLock);
		m_stateObserver = pObserver;
		FUNCTION_EXIT;
	}
	
	void IEC102FlowController::removeStateObserver()
	{
		FUNCTION_ENTRY("removeStateObserver()");
		ControlStateObserver* pObserver = m_stateObserver;
		{
			TA_Base_Core::ThreadGuard guard(m_stateObserverLock);
			m_stateObserver = NULL;
		}	
		
		delete pObserver;	
		FUNCTION_EXIT;
	}
	
}
