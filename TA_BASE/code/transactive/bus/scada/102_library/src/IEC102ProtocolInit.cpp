#include "bus/scada/102_library/src/IEC102ProtocolInit.h"


namespace TA_Base_Bus
{
	IEC102ProtocolInit::IEC102ProtocolInit(IEC102ControllerInit& controller)
		:IEC102Protocol(controller)
	{
		FUNCTION_ENTRY("IEC102ProtocolInit()");
		FUNCTION_EXIT;
	}

	IEC102ProtocolInit::~IEC102ProtocolInit()
	{
		FUNCTION_ENTRY("~IEC102ProtocolInit()");
		FUNCTION_EXIT;
	}

	std::string IEC102ProtocolInit::callLinkState()
	{
		FUNCTION_ENTRY("callLinkState()");
		//std::string frameData = Iec102FixedLenFrameOld::create_Frame_C_RLK_NA_2(m_lowAddress, m_highAddress);
		Iec102FixedLenFrame fixFrame(m_lowAddress,m_highAddress,IEC102_FCB_FALSE,IEC102_FCV_FALSE,FUN_CODE_9);
		std::string frameData = fixFrame.getRawData();
		FUNCTION_EXIT;
		return frameData;
	}

	std::string IEC102ProtocolInit::resetLinkUnit()
	{
		FUNCTION_ENTRY("resetLinkUnit()");
		//std::string frameData = Iec102FixedLenFrameOld::create_Frame_C_RCU_NA_2(m_lowAddress, m_highAddress);
		Iec102FixedLenFrame fixFrame(m_lowAddress,m_highAddress,IEC102_FCB_FALSE,IEC102_FCV_FALSE,FUN_CODE_0);
		std::string frameData = fixFrame.getRawData();
		FUNCTION_EXIT;		
		return frameData;
	}

	void IEC102ProtocolInit::processData(std::vector<unsigned char> &buffer)
	{
		FUNCTION_ENTRY("processData()");

		//For Init Protocol, set the virtual address to 0x00.
		Iec102FrameInterface* pFrame = createFrame(buffer,0x00,0x00);

		EFlowControlState flowCongtrolState =  pFrame->getFlowControlState();
		m_controller.updateFlowControlState(flowCongtrolState);

		FUNCTION_EXIT;
	}	

}

