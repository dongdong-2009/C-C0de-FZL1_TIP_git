#include "bus/scada/102_library/src/Channel.h"
#include "bus/scada/102_library/src/IEC102Protocol.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DataConversion.h"
#include "bus/scada/102_library/src/IEC102FlowController.h"
#include <iomanip>
#include <sstream>

namespace TA_Base_Bus
{
	IEC102Protocol::IEC102Protocol(IEC102FlowController& controller)
		:
	m_fcb(false),		
	m_fcv(true),
	m_lowAddress(0),
	m_highAddress(0),
	m_controller(controller),
	m_lastIncomingFrameControlField(0),
	m_currentAsduType(0)
	{
		m_lowAddress = m_controller.getLowRemoteAddress();
		m_highAddress = m_controller.getHighRemoteAddress();
	
		FUNCTION_ENTRY("IEC102Protocol()");
		FUNCTION_EXIT;
	}

	IEC102Protocol::~IEC102Protocol()
	{
		FUNCTION_ENTRY("~IEC102Protocol()");
		FUNCTION_EXIT;
	}

	std::string IEC102Protocol::getClassIData()
	{
		FUNCTION_ENTRY("getClassIData");
		m_fcv = true;
		m_fcb = (!m_fcb);
		//std::string frameData = Iec102FixedLenFrameOld::create_Frame_C_PL1_NA_2(m_lowAddress, m_highAddress, m_fcb, m_fcv);
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
		Iec102FixedLenFrame fixFrame(m_lowAddress,m_highAddress,fcb,fcv,FUN_CODE_10);
		std::string frameData = fixFrame.getRawData();
		FUNCTION_EXIT;
		return frameData;	
	}

	Iec102FrameInterface* IEC102Protocol::createFrame(const std::vector<unsigned char> &incomingBuffer,unsigned char virtuallowAddr,unsigned char virtualHighAddr )
	{
		FUNCTION_ENTRY("createFrame");
		Iec102FrameInterface* pFrame = NULL;
		unsigned int dataSize = incomingBuffer.size();

		if (FRAME_SINGLE_FORMAT_SIZE == dataSize )
		{
			pFrame = new Iec102SingleFrame(incomingBuffer);
		}
		else if (FRAME_FIX_FROMAT_SIZE == dataSize)
		{
			pFrame = new Iec102FixedLenFrame(incomingBuffer,m_lowAddress,m_highAddress);
		}
		else
		{
			pFrame = new Iec102VariableFrame(incomingBuffer,IEC102_MONITOR_DIRECTION,m_lowAddress,m_highAddress,virtuallowAddr,virtualHighAddr);
		}
		FUNCTION_EXIT;
		return pFrame;	
	}

	void IEC102Protocol::destroyFrame(Iec102FrameInterface* pFrame)
	{
		FUNCTION_ENTRY("destroyFrame");

		if (NULL != pFrame)
		{
			delete pFrame;
		}
		FUNCTION_EXIT;
	}

}

