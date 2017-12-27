#ifndef IEC102_PROTOCOL_H
#define IEC102_PROTOCOL_H

#include "bus/scada/102_library/src/IEC102DataStruct.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <queue>
#include "boost/shared_ptr.hpp"
#include <time.h>

namespace TA_Base_Bus
{
	class IEC102FlowController;
	class Channel;

	class IEC102Protocol
	{
	public:
		/**
		 * Constructor
		 *
		 */
		IEC102Protocol(IEC102FlowController& controller);
		
		/**
		 * Destructor
		 *
		 */
		virtual ~IEC102Protocol();

		
		/**
		 * getClassIData
		 *
		 *
		 * @return FrameData
		 *
		 */
		std::string getClassIData();

		Iec102FrameInterface* createFrame(const std::vector<unsigned char> &incomingBuffer,unsigned char virtuallowAddr,unsigned char virtualHighAddr);
		void destroyFrame(Iec102FrameInterface* pFrame);
		virtual void processData(std::vector<unsigned char>& buffer){};

	protected:

		bool				m_fcb;
		bool				m_fcv;
		unsigned char		m_lowAddress;
		unsigned char		m_highAddress;
		IEC102FlowController& m_controller;

		unsigned char 	m_lastIncomingFrameControlField;
		unsigned char	m_currentAsduType;
		std::string 	m_requestFrame;
		std::vector<unsigned char> m_incomingBuffer;
	};
}

#endif

