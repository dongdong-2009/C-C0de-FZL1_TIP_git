#include "bus/scada/102_library/src/IEC102ControllerInit.h"
#include "bus/scada/102_library/src/IEC102ProtocolInit.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	IEC102ControllerInit::IEC102ControllerInit(ChannelManager& channelManager,IEC102ConfigData& objConfigData,bool isInitLink)
	:IEC102FlowController(channelManager,objConfigData),
	m_protocolInit(NULL)
	{
		FUNCTION_ENTRY("IEC102ControllerInit()");
		m_protocolInit = new IEC102ProtocolInit(*this);
		m_102Protocol = m_protocolInit;
		m_resetLink = false;
		m_callLink = false;
		m_isInitLink = isInitLink;
		FUNCTION_EXIT;
	}
	
	IEC102ControllerInit::~IEC102ControllerInit()
	{
		FUNCTION_ENTRY("~IEC102ControllerInit()");
		if (m_102Protocol != NULL)
		{
			delete m_102Protocol;
			m_102Protocol = NULL;
		}
		FUNCTION_EXIT;
	}
	
	bool IEC102ControllerInit::processFlow()
	{
		FUNCTION_ENTRY("processFlow()");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set remoteAddress");

		bool bFlowSuccess = false;
		while (m_bFlowRunning)
		{
			switch (m_nextStep)
			{
				case STEP_START:
				case STEP_INIT_LINK:
					callLinkState();
					break;
				case STEP_RESET_LINK:
					resetLinkUnit();
					break;

				case STEP_CALL_I_DATA:
					getClassIData();
					break;

				case STEP_ABORT:
					m_bFlowRunning = false;
					break;

				case STEP_FINISH:
					bFlowSuccess = true;
					m_bFlowRunning = false;
					break;

				default:
					break;
			}
		}
	
		FUNCTION_EXIT;
		return bFlowSuccess;
	}
		
	// set the informatioin for the next flow 
	void IEC102ControllerInit::setFlowInfo()
	{
		FUNCTION_ENTRY("setFlowInfo()");
		FUNCTION_EXIT;
	}

	// Check whehter the next flow exist.
	bool IEC102ControllerInit::getNextFlow()
	{
		FUNCTION_ENTRY("getNextFlow()");
		FUNCTION_EXIT;
		return false;
	}
	void IEC102ControllerInit::callLinkState()
	{
		FUNCTION_ENTRY("callLinkState()");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get frame data for callLinkState");
		std::string frameData = m_protocolInit->callLinkState();
		ControlStateObserver* pObserver = new ControlStateObserver(m_messageResponseTime);
		pObserver->addObservedState(E_M_ACTIVE_I_DATA_RESPONSE_LINK_STATE);		// 10 2B 01 00 0C 16
		pObserver->addObservedState(E_M_RESPONSE_LINK_STATE);					// 10 0B 01 00 0C 16

		pObserver->addObservedState(E_M_0E5H_CON);			// E5
		pObserver->addObservedState(E_M_LINK_BUSY);			// 10 01 01 00 0C 16
		pObserver->addObservedState(E_M_BUFFER_FULL);		// 10 0B 01 00 0C 16

		pObserver->addObservedState(CONNECTION_LOST);	// Connection Lost
		pObserver->addObservedState(DATA_INVALID);		// Invalid Data
		pObserver->addObservedState(STOP_PROCESS);		// Stop Process
		pObserver->addObservedState(WRITE_FAILED);		// Write Fail

		setStateObserver(pObserver);

		EFlowControlState replyState;
		if (sendAndWait(frameData,replyState))
		{
			switch (replyState)
			{
				case E_M_ACTIVE_I_DATA_RESPONSE_LINK_STATE:
					if (m_isInitLink)
					{
						m_callLink = true;
						m_nextStep = STEP_CALL_I_DATA;
					}
					else
					{
						m_nextStep = STEP_FINISH;
					}
					break;
				case E_M_RESPONSE_LINK_STATE:
					if (m_isInitLink)
					{
						if (m_resetLink)
						{
							m_nextStep = STEP_FINISH;
						}
						else
						{
							m_nextStep = STEP_RESET_LINK;
						}
					}
					else
					{
						m_nextStep = STEP_FINISH;
					}
					break;
				case E_M_0E5H_CON:
				case E_M_LINK_BUSY:
				case E_M_BUFFER_FULL:
					TA_Base_Core::Thread::sleep( 2000 );
					m_nextStep = STEP_INIT_LINK;
					break;
				default:
					m_nextStep = STEP_ABORT;
					break;
			}			
		}
		else
		{
			m_nextStep = STEP_ABORT;
		}
	
		removeStateObserver();
		FUNCTION_EXIT;

	}

	void IEC102ControllerInit::resetLinkUnit()
	{
		FUNCTION_ENTRY("resetLinkUnit()");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get frame data for resetLinkUnit");
		std::string frameData = m_protocolInit->resetLinkUnit();
		ControlStateObserver* pObserver = new ControlStateObserver(m_messageResponseTime);
		pObserver->addObservedState(E_M_ACTIVE_I_DATA_RESPONSE_RESET_LINK);		// 10 2B 01 00 0C 16
		pObserver->addObservedState(E_M_RESPONSE_RESET_LINK);					// 10 0B 01 00 0C 16

		pObserver->addObservedState(E_M_0E5H_CON);			// E5
		pObserver->addObservedState(E_M_LINK_BUSY);			// 10 01 01 00 0C 16
		pObserver->addObservedState(E_M_BUFFER_FULL);		// 10 0B 01 00 0C 16

		pObserver->addObservedState(CONNECTION_LOST);	// Connection Lost
		pObserver->addObservedState(DATA_INVALID);		// Invalid Data
		pObserver->addObservedState(STOP_PROCESS);		// Stop Process
		pObserver->addObservedState(WRITE_FAILED);		// Write Fail
		pObserver->addObservedState(E_M_UNKNOWN_CONTROL_STATE);		// Unknownn control state

		setStateObserver(pObserver);

		EFlowControlState replyState;
		if (sendAndWait(frameData,replyState))
		{
			switch (replyState)
			{
				case E_M_ACTIVE_I_DATA_RESPONSE_RESET_LINK:
					m_resetLink = true;
					m_nextStep = STEP_CALL_I_DATA;
					break;

				case E_M_RESPONSE_RESET_LINK:
					m_resetLink = true;
					m_nextStep = STEP_INIT_LINK;
					break;
				case E_M_0E5H_CON:
					m_nextStep = STEP_INIT_LINK;
					break;
				default:
					m_nextStep = STEP_ABORT;
					break;
			}			
		}
		else
		{
			m_nextStep = STEP_ABORT;
		}
		
		removeStateObserver();
		FUNCTION_EXIT;

	}		
		

	void IEC102ControllerInit::getClassIData()
	{
		FUNCTION_ENTRY("getClassIData()");
		std::string frameData = m_protocolInit->getClassIData();
		ControlStateObserver* pObserver = new ControlStateObserver(m_messageResponseTime);
		pObserver->addObservedState(E_M_RESPONSE_DATA_COT4);		// E5
		pObserver->addObservedState(E_M_0E5H_CON);		// E5
		pObserver->addObservedState(CONNECTION_LOST);	// Connection Lost
		pObserver->addObservedState(DATA_INVALID);		// Invalid Data
		pObserver->addObservedState(STOP_PROCESS);		// Stop Process
		pObserver->addObservedState(WRITE_FAILED);		// Write Fail
		pObserver->addObservedState(E_M_INIT_COMPLETED);    // init complete
		setStateObserver(pObserver);

		EFlowControlState replyState;
		if (sendAndWait(frameData,replyState))
		{
			switch (replyState)
			{
				case E_M_RESPONSE_DATA_COT4:
					if (m_callLink)
					{
						if (!m_resetLink)
							m_nextStep = STEP_RESET_LINK;
						else
							m_nextStep = STEP_INIT_LINK;
					}
					else
					{
						m_nextStep = STEP_INIT_LINK;
					}
					break;
				case E_M_0E5H_CON:
					m_nextStep = STEP_CALL_I_DATA;
					break;
				case E_M_INIT_COMPLETED:
					m_nextStep = STEP_INIT_LINK;
					break;
				default:
					m_nextStep = STEP_ABORT;
					break;
			}			
		}
		else
		{
			m_nextStep = STEP_ABORT;
		}

		removeStateObserver();
		FUNCTION_EXIT;
	}

	
}

