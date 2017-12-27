/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/rtu_library/src/RTUCommandWorker.cpp $
  * @author:	HoaVu
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


#include "bus/scada/rtu_library/src/RTUCommandWorker.h"
#include "bus/scada/rtu_library/src/RTU.h"

#include "bus/modbus_comms/src/IModbus.h"
#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "bus/modbus_comms/src/SteModbusTcpConnection.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/message/src/NameValuePair.h"
#include "core/message/types/PMSAgentAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	RTUCommandWorker::RTUCommandWorker ( RTU& rtu, const std::string& strServicePortNumber)
		:
		m_rtu(rtu),
        m_rtuServicePortNumber(strServicePortNumber)
	{
	}


	RTUCommandWorker::~RTUCommandWorker()
	{
	}


	bool RTUCommandWorker::sendRTUCommand( const WORD & commandValue )
	{
		unsigned long registerAddress = 0;
		TA_Base_Bus::DataBlock < WORD > registerDataBlock;

		// initialise data block
		registerDataBlock.setStartAndLength( registerAddress, 1 );
		registerDataBlock.set( registerAddress, commandValue );

		//
		// send command now
		//
		bool result = m_rtu.processSendingData( registerDataBlock,m_rtuServicePortNumber, m_rtu.getSlaveID());

		// log an audit message
		TA_Base_Core::DescriptionParameters descriptions;
		if ( true == result )
		{
            switch( commandValue )
            {
            case 0x1000: //TD10586
                m_rtu.submitAuditMessage( descriptions, TA_Base_Core::PMSAgentAudit::CommandToSetDownloadCompletedSucceeded );
                break;
				
            case 0x2000:
                m_rtu.submitAuditMessage( descriptions, TA_Base_Core::PMSAgentAudit::CommandToSwitchToPrimarySucceeded );
                break;
                
            case 0x4000:
                m_rtu.submitAuditMessage( descriptions, TA_Base_Core::PMSAgentAudit::CommandToSwitchToStandbySucceeded );
                break;
                
            case 0x8000:
                m_rtu.submitAuditMessage( descriptions, TA_Base_Core::PMSAgentAudit::CommandToResetSucceeded );
                break;

            default:
                {
                    std::ostringstream desc;
                    desc << commandValue;
                    
                    // construct the DescriptionParameters				
                    TA_Base_Core::NameValuePair pair1( "CommandValue", desc.str() );
                    descriptions.push_back( &pair1 );

                    m_rtu.submitAuditMessage( descriptions, TA_Base_Core::PMSAgentAudit::CommandToRTUSucceeded );
                }
            }
		}
		else
		{
			switch( commandValue )
            {
            case 0x1000: //TD10586
                m_rtu.submitAuditMessage( descriptions, TA_Base_Core::PMSAgentAudit::CommandToSetDownloadCompletedFailed );
                break;
				
            case 0x2000:
                m_rtu.submitAuditMessage( descriptions, TA_Base_Core::PMSAgentAudit::CommandToSwitchToPrimaryFailed );
                break;
                
            case 0x4000:
                m_rtu.submitAuditMessage ( descriptions, TA_Base_Core::PMSAgentAudit::CommandToSwitchToStandbyFailed );
                break;
                
            case 0x8000:
                m_rtu.submitAuditMessage ( descriptions, TA_Base_Core::PMSAgentAudit::CommandToResetFailed );
                break;
                
            default:
                {
                    std::ostringstream desc;
                    desc << commandValue;
                    
                    // construct the DescriptionParameters
                    TA_Base_Core::NameValuePair pair1( "CommandValue", desc.str() );
                    descriptions.push_back( &pair1 );
                    
                    m_rtu.submitAuditMessage( descriptions, TA_Base_Core::PMSAgentAudit::CommandToRTUFailed );
                }
            }
		}

		return result;
	}


	bool RTUCommandWorker::resetRTU()
	{
		// bit order: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
		//	   value: 1  0  0  0  0  0  0 0 0 0 0 0 0 0 0 0 = 0x8000

		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x8000 );
	}


	bool RTUCommandWorker::switchPrimaryToStandby()
	{
		// bit order: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
		//	   value: 0  1  0  0  0  0  0 0 0 0 0 0 0 0 0 0 = 0x4000

		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x4000 );
	}


	bool RTUCommandWorker::switchStandbyToPrimary()
	{
		// bit order: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
		//	   value: 0  0  1  0  0  0  0 0 0 0 0 0 0 0 0 0 = 0x2000

		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x2000 );
	}


	bool RTUCommandWorker::setDownloadRequired()
	{
		// bit order: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
		//	   value: 0  0  0  0  1  0  0 0 0 0 0 0 0 0 0 0 = 0x0800

		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0800 );
	}


	//TD10586
	bool RTUCommandWorker::setDownloadCompleted()
	{
		// bit order: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
		//	   value: 0  0  0  1  0  0  0 0 0 0 0 0 0 0 0 0 = 0x1000

		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x1000 );
	}


	bool RTUCommandWorker::enablePOWER1Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0200 );
	}


	bool RTUCommandWorker::enablePOWER2Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0201 );
	}


	bool RTUCommandWorker::enablePOWER3Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0202 );
	}


	bool RTUCommandWorker::enableCLOCKPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0203 );
	}


	bool RTUCommandWorker::enableWILDPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0204 );
	}


	bool RTUCommandWorker::enableECSPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0205 );
	}


	bool RTUCommandWorker::enableFPSPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0206 );
	}


	bool RTUCommandWorker::enableTWPPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0207 );
	}


	bool RTUCommandWorker::enablePSD1Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0208 );
	}


	bool RTUCommandWorker::enablePSD2Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0209 );
	}


	bool RTUCommandWorker::enablePSD3Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x020A );
	}


	bool RTUCommandWorker::enablePSD4Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x020B );
	}


	bool RTUCommandWorker::enablePLC1Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x020C );
	}


	bool RTUCommandWorker::enablePLC2Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x020D );
	}


	bool RTUCommandWorker::enablePLC3Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x020E );
	}


	bool RTUCommandWorker::enablePLC4Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x020F );
	}


	bool RTUCommandWorker::enableReserved1Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0210 );
	}


	bool RTUCommandWorker::enableReserved2Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0211 );
	}


	bool RTUCommandWorker::enableReserved3Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0212 );
	}


	bool RTUCommandWorker::enableReserved4Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0213 );
	}


	bool RTUCommandWorker::inhibitPOWER1Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0100 );
	}


	bool RTUCommandWorker::inhibitPOWER2Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0101 );
	}


	bool RTUCommandWorker::inhibitPOWER3Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0102 );
	}


	bool RTUCommandWorker::inhibitCLOCKPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0103 );
	}


	bool RTUCommandWorker::inhibitWILDPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0104 );
	}


	bool RTUCommandWorker::inhibitECSPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0105 );
	}


	bool RTUCommandWorker::inhibitFPSPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0106 );
	}


	bool RTUCommandWorker::inhibitTWPPolling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0107 );
	}


	bool RTUCommandWorker::inhibitPSD1Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0108 );
	}


	bool RTUCommandWorker::inhibitPSD2Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0109 );
	}


	bool RTUCommandWorker::inhibitPSD3Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x010A );
	}


	bool RTUCommandWorker::inhibitPSD4Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x010B );
	}


	bool RTUCommandWorker::inhibitPLC1Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x010C );
	}


	bool RTUCommandWorker::inhibitPLC2Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x010D );
	}


	bool RTUCommandWorker::inhibitPLC3Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x010E );
	}


	bool RTUCommandWorker::inhibitPLC4Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x010F );
	}


	bool RTUCommandWorker::inhibitReserved1Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0110 );
	}


	bool RTUCommandWorker::inhibitReserved2Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0111 );
	}


	bool RTUCommandWorker::inhibitReserved3Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0112 );
	}


	bool RTUCommandWorker::inhibitReserved4Polling()
	{
		// send the command using TCP Modbus Driver
		return sendRTUCommand ( 0x0113 );
	}
}
