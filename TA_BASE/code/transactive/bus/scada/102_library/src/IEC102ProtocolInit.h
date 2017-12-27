#ifndef IEC102_PROTOCOL_INIT_H
#define IEC102_PROTOCOL_INIT_H

#include "bus/scada/102_library/src/IEC102Protocol.h"
#include "bus/scada/102_library/src/IEC102ControllerInit.h"


namespace TA_Base_Bus
{
	class IEC102FlowController;
	class Channel;

	class IEC102ProtocolInit : public TA_Base_Bus::IEC102Protocol
	{
	public:
		/**
		 * Constructor
		 *
		 */
		IEC102ProtocolInit(IEC102ControllerInit& controller);
		
		/**
		 * Destructor
		 *
		 */
		virtual ~IEC102ProtocolInit();

		/**
		 * callLinkState
		 *
		 * call link state
		 *
		 * @return FrameData
		 *
		 */
		std::string callLinkState();

		/**
		 * resetLinkUnit
		 *
		 * Set remote ERTU's address
		 *
		 * @param lowAddr low byte of ERTU's address
		 * @param highAddr high byte of ERTU's address
		 *
		 * @return FrameData
		 *
		 */
		std::string resetLinkUnit();

		virtual void processData(std::vector<unsigned char>& buffer);

	
	};
	
}

#endif

