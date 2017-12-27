#include "bus/scada/102_library/src/IEC102Controller120.h"
#include "bus/scada/102_library/src/IEC102Protocol120.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	IEC102Controller120::IEC102Controller120(ChannelManager& channelManager,IEC102ConfigData& objConfigData,unsigned short startAddr,unsigned short endAddr, time_t startTime, time_t endTime, unsigned char rad)
	:IEC102FlowController(channelManager,objConfigData),
	 m_protocol120(NULL)
	{
		FUNCTION_ENTRY("IEC102Controller120()");
		m_protocol120 = new IEC102Protocol120(*this);
		m_102Protocol = m_protocol120;
		m_startTime   = startTime;
		m_endTime 	  = endTime;
		m_startAddr   = startAddr;
		m_endAddr 	  = endAddr;
		m_rad 		  = rad;
		m_currentStartAddr = 0;
		m_currentEndAddr   = 0;
		m_bNextRoundFlow   = false;
		FUNCTION_EXIT;
	}
	
	IEC102Controller120::~IEC102Controller120()
	{
		FUNCTION_ENTRY("~IEC102Controller120()");
		if (m_102Protocol != NULL)
		{
			delete m_102Protocol;
			m_102Protocol = NULL;
		}
		FUNCTION_EXIT;
	}


	bool IEC102Controller120::processFlow()
	{
		FUNCTION_ENTRY("processFlow()");
		m_bFlowRunning = true;
		m_nextStep = STEP_START;

		bool bFlowSuccess = false;
		while (m_bFlowRunning)
		{
			switch (m_nextStep)
			{
				case STEP_START:
				case STEP_REQ_120:
					call120Data();
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
	void IEC102Controller120::setFlowInfo()
	{
		FUNCTION_ENTRY("setFlowInfo()");
		unsigned short nExpectedEnd;

		if (0 == m_currentStartAddr)
		{
			m_currentStartAddr = m_startAddr;
		}
		else
		{
			m_currentStartAddr = m_currentEndAddr + 1;
		}

		//Got the expected end adddress
		nExpectedEnd = ((m_currentStartAddr / VIRTUAL_ADDRESS_TRANSIT_VALUE) + 1) * VIRTUAL_ADDRESS_TRANSIT_VALUE ;
		if (nExpectedEnd >= m_endAddr )
		{
			m_currentEndAddr = m_endAddr;
			m_bNextRoundFlow = false;
		}
		else
		{
			m_currentEndAddr = nExpectedEnd;
			m_bNextRoundFlow = true;
		}
		unsigned short currentStartAddr = m_currentStartAddr % VIRTUAL_ADDRESS_TRANSIT_VALUE;		
		unsigned short currentEndAddr = m_currentEndAddr % VIRTUAL_ADDRESS_TRANSIT_VALUE;
		unsigned short virtualAddr = (m_currentStartAddr / VIRTUAL_ADDRESS_TRANSIT_VALUE) + 1;
		
		if (currentEndAddr == 0)
		{
			currentEndAddr = VIRTUAL_ADDRESS_TRANSIT_VALUE;
		}
		
		m_protocol120->setVirtualAddress(virtualAddr);		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_currentStartAddr=%d,m_currentEndAddr=%d,CurrentStartAddr=%d,CurrentEndAddr=%d,VirtualAddr=%d,HasNextFlow=%s",m_currentStartAddr,m_currentEndAddr,currentStartAddr,currentEndAddr,virtualAddr,m_bNextRoundFlow?"Yes":"No");
		
		FUNCTION_EXIT;
	}

	// Check whehter the next flow exist.
	bool IEC102Controller120::getNextFlow()
	{
		FUNCTION_ENTRY("getNextFlow()");
		FUNCTION_EXIT;
		return m_bNextRoundFlow;
	}

	void IEC102Controller120::getClassIData()
	{
		FUNCTION_ENTRY("getClassIData()");
		std::string frameData = m_protocol120->getClassIData();
		ControlStateObserver* pObserver = new ControlStateObserver(m_messageResponseTime);
		pObserver->addObservedState(E_M_0E5H_CON);				// E5
		pObserver->addObservedState(E_M_RESPONSE_DATA_COT7);	// Confirmation image frame
		pObserver->addObservedState(E_M_RESPONSE_DATA_COT5);	// Information of requested Class I data
		pObserver->addObservedState(E_M_RESPONSE_DATA_COTA);	// Completed image frame
		pObserver->addObservedState(E_M_RESPONSE_DATA_COTD);	// No requested data
		pObserver->addObservedState(CONNECTION_LOST);			// Connection Lost
		pObserver->addObservedState(DATA_INVALID);				// Invalid Data
		pObserver->addObservedState(STOP_PROCESS);				// Stop Process
		pObserver->addObservedState(WRITE_FAILED);				// Write Fail
		pObserver->addObservedState(E_M_LINK_BUSY);			// 10 01 01 00 0C 16
		pObserver->addObservedState(E_M_BUFFER_FULL);		// 10 0B 01 00 0C 16
		pObserver->addObservedState(E_M_UNKNOWN_CONTROL_STATE);		// Unknownn control state

		setStateObserver(pObserver);

		EFlowControlState replyState;
		if (sendAndWait(frameData,replyState))
		{
			
			switch (replyState)
			{
				case E_M_RESPONSE_DATA_COT5:
				case E_M_RESPONSE_DATA_COT7:
				case E_M_0E5H_CON:
						m_nextStep = STEP_CALL_I_DATA;
						break;

				case E_M_RESPONSE_DATA_COTA:
				case E_M_RESPONSE_DATA_COTD:
					m_nextStep = STEP_FINISH;
	
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
	void IEC102Controller120::call120Data()
	{
		FUNCTION_ENTRY("getClassIData()");
		unsigned short currentStartAddr = m_currentStartAddr % VIRTUAL_ADDRESS_TRANSIT_VALUE;		
		unsigned short currentEndAddr = m_currentEndAddr % VIRTUAL_ADDRESS_TRANSIT_VALUE;

		if (currentEndAddr == 0)
		{
			currentEndAddr = VIRTUAL_ADDRESS_TRANSIT_VALUE;
		}
		
		std::string frameData = m_protocol120->getPowerEnergy_C_CI_NR_2(currentStartAddr, currentEndAddr, m_startTime, m_endTime, m_rad);
		ControlStateObserver* pObserver = new ControlStateObserver(m_messageResponseTime);
		pObserver->addObservedState(E_M_0E5H_CON);				// E5
		pObserver->addObservedState(E_M_RESPONSE_DATA_COTD);	// No requested data
		pObserver->addObservedState(CONNECTION_LOST);			// Connection Lost
		pObserver->addObservedState(DATA_INVALID);				// Invalid Data
		pObserver->addObservedState(STOP_PROCESS);				// Stop Process
		pObserver->addObservedState(WRITE_FAILED);				// Write Fail

		setStateObserver(pObserver);


		EFlowControlState replyState;
		if (sendAndWait(frameData,replyState))
		{		
			switch (replyState)
			{
				case E_M_0E5H_CON:
					m_nextStep = STEP_CALL_I_DATA;
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

