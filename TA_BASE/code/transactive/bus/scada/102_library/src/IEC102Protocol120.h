#ifndef IEC102_PROTOCOL_120_H
#define IEC102_PROTOCOL_120_H

#include "bus/scada/102_library/src/IEC102Protocol.h"
#include "bus/scada/102_library/src/IEC102Controller120.h"


namespace TA_Base_Bus
{
	class IEC102Protocol120 : public TA_Base_Bus::IEC102Protocol
	{
	public:
		/**
		 * Constructor
		 *
		 */
		IEC102Protocol120(IEC102Controller120& controller);
		
		/**
		 * Destructor
		 *
		 */
		virtual ~IEC102Protocol120();

		/**
		 * getPowerEnergy_C_CI_NR_2(120)
		 *
		 *
		 * @return true if operation success
		 *
		 * @param startAddr start address power energy
		 * @param endAddr end address of power energy
		 * @param startTime start time
		 * @param endTime end time
		 *
		 */
		std::string getPowerEnergy_C_CI_NR_2(unsigned short startAddr, unsigned short endAddr, time_t startTime, time_t endTime, unsigned char rad);
		void setVirtualAddress(unsigned short nAddress);
		bool checkMatchWithRequestMessage(std::string& strMessage);
		virtual void processData(std::vector<unsigned char>& buffer);
	private:

		unsigned char		m_lowVirtualAddress;
		unsigned char		m_highVirtualAddress;
		std::string	    	m_requestFrameHex;

	};
	
}

#endif

