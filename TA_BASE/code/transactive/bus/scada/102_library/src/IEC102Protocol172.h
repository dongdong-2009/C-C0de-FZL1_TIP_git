#ifndef IEC102_PROTOCOL_172_H
#define IEC102_PROTOCOL_172_H

#include "bus/scada/102_library/src/IEC102Protocol.h"
#include "bus/scada/102_library/src/IEC102Controller172.h"

namespace TA_Base_Bus
{
	class IEC102Protocol172 : public TA_Base_Bus::IEC102Protocol
	{
	public:
		/**
		 * Constructor
		 *
		 */
		IEC102Protocol172(IEC102Controller172& controller);
		
		/**
		 * Destructor
		 *
		 */
		virtual ~IEC102Protocol172();

		/**
		 * getRemoteMeasurement_C_YC_TA_2(172)
		 *
		 *
		 * @return true if operation success
		 *
		 * @param startAddr start address of remote measurement
		 * @param endAddr end address of remote measurement
		 * @param startTime start time
		 * @param endTime end time
		 *
		 */
		std::string getRemoteMeasurement_C_YC_TA_2(unsigned short startAddr, unsigned short endAddr, time_t startTime, time_t endTime, unsigned char rad);
		void setVirtualAddress(unsigned short nAddress);
		bool checkMatchWithRequestMessage(std::string& strMessage);
		virtual void processData(std::vector<unsigned char>& buffer);
		
	private:
		unsigned char	m_lowVirtualAddress;
		unsigned char	m_highVirtualAddress;
		std::string	    m_requestFrameHex;

	};
	
}

#endif

