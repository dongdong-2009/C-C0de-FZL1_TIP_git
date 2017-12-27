#include "bus/scada/102_library/src/IEC102Protocol172.h"


namespace TA_Base_Bus
{
	IEC102Protocol172::IEC102Protocol172(IEC102Controller172& controller)
		:IEC102Protocol(controller),
		 m_lowVirtualAddress(0),
		 m_highVirtualAddress(0)

	{
	
		FUNCTION_ENTRY("IEC102Protocol172()");
		FUNCTION_EXIT;
	}

	IEC102Protocol172::~IEC102Protocol172()
	{
		FUNCTION_ENTRY("~IEC102Protocol172()");
		FUNCTION_EXIT;
	}

	void IEC102Protocol172::setVirtualAddress(unsigned short nAddress)
	{
		m_highVirtualAddress = static_cast<unsigned char>((nAddress & 0xFF00) >> 8);
		m_lowVirtualAddress  = static_cast<unsigned char>(nAddress & 0x00FF);
	}
	

	/*************************************************************************
	send command to read remote measurement info within specified period and address (172)( bytes). 
	This command is defined in YuBang document.
	*************************************************************************/
	
	std::string IEC102Protocol172::getRemoteMeasurement_C_YC_TA_2( unsigned short startAddr, unsigned short endAddr, time_t startTime, time_t endTime, unsigned char rad )
	{
		FUNCTION_ENTRY("getPowerEnergy_C_CI_NR_2()");
		m_fcv = true;
		m_fcb = (!m_fcb);
		E_IEC102_FCV fcv = IEC102_FCV_TRUE;
		E_IEC102_FCB fcb;
	
		if (m_fcb)
		{
			fcb = IEC102_FCB_TRUE;
		}
		else
		{
			fcb = IEC102_FCB_FALSE;
		}
		
		unsigned char ucStartAddr = static_cast<unsigned char>(startAddr);
		unsigned char ucEndAddr = static_cast<unsigned char>(endAddr);
		Iec102VariableFrame varFrame(fcb,fcv,
		TA_Base_Bus::FUN_CODE_3,
		TA_Base_Bus::C_YC_TA_2,
		m_lowAddress,m_highAddress,m_lowVirtualAddress,m_highVirtualAddress,rad,
		ucStartAddr,ucEndAddr,startTime,endTime);
	
		m_requestFrameHex = varFrame.getRawData();
	
		FUNCTION_EXIT;
		return m_requestFrameHex;
	}

	bool IEC102Protocol172::checkMatchWithRequestMessage(std::string& strMessage)
	{
		std::string strRequest = m_requestFrameHex;
		strRequest[4] = 0x00;
		strRequest[9] = 0x00;
		strRequest[25] = 0x00;

		std::string strTempMessage = strMessage;
		strTempMessage[4] = 0x00;
		strTempMessage[9] = 0x00;
		strTempMessage[25] = 0x00;

		if (strRequest.compare(strTempMessage) == 0)
		{
			return true;
		}
		else
		{
			std::string strRequestHex = TA_Base_Bus::HexHelper::hexToStr(strRequest);
			std::string strRecvHex = TA_Base_Bus::HexHelper::hexToStr(strTempMessage);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Frame Data Mismatch\n\tRequest Frame Data:\n\t%s\n\tConfirm Request Frame Data:\n\t%s", strRequestHex.c_str(), strRecvHex.c_str());

			
			return false;
		}
	}

	void IEC102Protocol172::processData(std::vector<unsigned char> &buffer)
	{
		FUNCTION_ENTRY("processData()");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s","Protocol 172 start to process data.");
		
		Iec102FrameInterface* pFrame = createFrame(buffer,m_lowVirtualAddress,m_highVirtualAddress);
		EFlowControlState flowCongtrolState =  pFrame->getFlowControlState();
		
		if (pFrame->isFrameValid())
		{	
			Iec102VariableFrame* pVarFrame = dynamic_cast<Iec102VariableFrame*>(pFrame);
			if (NULL != pVarFrame )
			{
				
				if (E_M_RESPONSE_DATA_COT7 == flowCongtrolState ||
					E_M_RESPONSE_DATA_COTA == flowCongtrolState)
				{
					std::string strRecvData = pVarFrame->getRawData();
					bool bCheckMatch =  checkMatchWithRequestMessage(strRecvData);
					if (!bCheckMatch)
					{
						flowCongtrolState = DATA_INVALID;
					}
				}
				else
				{
					m_controller.addEnergyData(pVarFrame->getEnergyData());
				}
			}
		}
		
		m_controller.updateFlowControlState(flowCongtrolState);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s","Protocol 172 end to process data.");
		FUNCTION_EXIT;
	}	
	
}

