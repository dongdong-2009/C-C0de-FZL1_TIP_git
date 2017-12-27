#ifndef IEC102_PROTOCOL_171_H
#define IEC102_PROTOCOL_171_H

#include "bus/scada/102_library/src/IEC102Protocol.h"
#include "bus/scada/102_library/src/IEC102Controller171.h"


namespace TA_Base_Bus
{
	class IEC102Protocol171 : public TA_Base_Bus::IEC102Protocol
	{
	public:
		/**
		 * Constructor
		 *
		 */
		IEC102Protocol171(IEC102Controller171& controller);
		
		/**
		 * Destructor
		 *
		 */
		virtual ~IEC102Protocol171();


		/**
		 * getRemoteMeasurement_C_YC_NA_2(171)
		 *
		 *
		 * @return true if operation success
		 *
		 * @param startAddr start address of remote measurement
		 * @param endAddr end address of remote measurement
		 *
		 */
		std::string getRemoteMeasurement_C_YC_NA_2(unsigned short startAddr, unsigned short endAddr);
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

