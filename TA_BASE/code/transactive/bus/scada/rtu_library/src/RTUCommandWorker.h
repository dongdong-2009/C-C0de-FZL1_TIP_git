/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUCommandWorker.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2017/05/25 10:19:08 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *	The RTUCommandWorker object is responsible for sending commands for controlling
  * the status of the RTU
  *
  */


#ifndef RTUCOMMANDWORKER_H
#define RTUCOMMANDWORKER_H

#include <string>

#include "bus/scada/rtu_library/src/StationSystemOutputWriter.h"

#include "bus/modbus_comms/src/DataBlock.h"

#include "core/message/src/NameValuePair.h"
#include "core/sockets/src/SocketOptions.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"

namespace TA_Base_Bus
{
    class IModbus;
}

namespace TA_Base_Core
{
    class AuditMessageSender;
    class MessageType;
}

namespace TA_Base_Bus
{
    // forward declaration
    class RTU;

	class RTUCommandWorker
	{
	// operations
	public:

		RTUCommandWorker ( RTU& rtu, const std::string& strServicePortNumber);
			
		virtual ~RTUCommandWorker();

		//
		// operations to command RTU
		//

		bool resetRTU();
		bool switchPrimaryToStandby();
		bool switchStandbyToPrimary();
		bool setDownloadRequired();
		bool setDownloadCompleted(); //TD10586

		bool enablePOWER1Polling();
		bool enablePOWER2Polling();
		bool enablePOWER3Polling();
		bool enableCLOCKPolling();
		bool enableWILDPolling();
		bool enableECSPolling();
		bool enableFPSPolling();
		bool enableTWPPolling();
		bool enablePSD1Polling();
		bool enablePSD2Polling();
		bool enablePSD3Polling();
		bool enablePSD4Polling();
		bool enablePLC1Polling();
		bool enablePLC2Polling();
		bool enablePLC3Polling();
		bool enablePLC4Polling();
		bool enableReserved1Polling();
		bool enableReserved2Polling();
		bool enableReserved3Polling();
		bool enableReserved4Polling();

		bool inhibitPOWER1Polling();
		bool inhibitPOWER2Polling();
		bool inhibitPOWER3Polling();
		bool inhibitCLOCKPolling();
		bool inhibitWILDPolling();
		bool inhibitECSPolling();
		bool inhibitFPSPolling();
		bool inhibitTWPPolling();
		bool inhibitPSD1Polling();
		bool inhibitPSD2Polling();
		bool inhibitPSD3Polling();
		bool inhibitPSD4Polling();
		bool inhibitPLC1Polling();
		bool inhibitPLC2Polling();
		bool inhibitPLC3Polling();
		bool inhibitPLC4Polling();
		bool inhibitReserved1Polling();
		bool inhibitReserved2Polling();
		bool inhibitReserved3Polling();
		bool inhibitReserved4Polling();

	// operations
	private:

		bool sendRTUCommand( const WORD & commandValue );

        // disabled constructors
		RTUCommandWorker();
		RTUCommandWorker ( const RTUCommandWorker & obj );
		RTUCommandWorker & operator= ( const RTUCommandWorker & rhs );

	// attributes
	private:
        TA_Base_Core::WriteReadThreadLockable m_modbusLock;

        RTU& m_rtu;
        std::string  m_rtuServicePortNumber;
    };
}

#endif
