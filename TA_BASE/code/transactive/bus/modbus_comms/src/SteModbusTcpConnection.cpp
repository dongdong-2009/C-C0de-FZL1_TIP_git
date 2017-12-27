/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/SteModbusTcpConnection.cpp $
  * @author  Raymond Pau
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  */

#include <sstream>

#include "bus/modbus_comms/src/SteModbusTcpConnection.h"
#include "bus/modbus_comms/src/ModbusException.h"

#include "core/utilities/src/DebugUtil.h"

#define THROW_NOT_SUPPORTED_EXCEPTION() \
TA_THROW( TA_Base_Bus::ModbusException( "The functionality requested is not supported by the STE RTU.", TA_Base_Bus::ModbusException::ILLEGAL_FUNCTION ) )

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    const int SteModbusTcpConnection::HEADER_SIZE = 1;
    const int SteModbusTcpConnection::FOOTER_SIZE = 2;

    SteModbusTcpConnection::SteModbusTcpConnection( const std::string& host,
													const std::string& service,
													long socketTimeout,
                                                    bool tcpNoDelay )
	:
    ModbusTcpConnection( host, service, socketTimeout, tcpNoDelay )
    {
        FUNCTION_ENTRY("Constructor");

        FUNCTION_EXIT;
    }


    SteModbusTcpConnection::~SteModbusTcpConnection()
    {
        FUNCTION_ENTRY("Destructor");

        FUNCTION_EXIT;
    }


    void SteModbusTcpConnection::writeRequest( const std::string& request )
    {
        FUNCTION_ENTRY("writeRequest");

		// TD# 3012
		if ( false == isEnabled() )
		{
			TA_THROW( ModbusException( "The connection is not enabled for communications.", ModbusException::CONNECTION_NOT_ENABLED ) );
		}
		// end of TD# 3012

		std::string requestPacket;
		requestPacket += (char) getDeviceAddress();
		requestPacket.append( request );

		// Calculate the CRC for request and write it to the serial port.
		m_crcCalculator.clear();
		m_crcCalculator.updateCrc( requestPacket );

		requestPacket += LSB( m_crcCalculator.getCrc() );
		requestPacket += MSB( m_crcCalculator.getCrc() );

		write( requestPacket.c_str(), requestPacket.size() );

        FUNCTION_EXIT;
    }


    void SteModbusTcpConnection::readReply( std::string& reply, int expectedSize )
    {
        FUNCTION_ENTRY("readReply");

		// TD# 3012
		if ( false == isEnabled() )
		{
			TA_THROW( ModbusException( "The connection is not enabled for communications.", ModbusException::CONNECTION_NOT_ENABLED ) );
		}
		// end of TD# 3012

		if ( expectedSize >= (int) sizeof(m_replyBuffer) )
		{
			TA_THROW( ModbusException( "The expected reply size exceeds internal buffer size allowances",
                ModbusException::SIZE_EXCEEDS_BUFFER ) );
		}

		unsigned int bytesRead = 0;

		// Add an allowance for the device address and the CRC.
		expectedSize += sizeof(char) + sizeof(WORD);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Expecting %d bytes in the reply.", expectedSize);

		bytesRead = read( m_replyBuffer, expectedSize );

		if ( 0 == bytesRead )
		{
			TA_THROW( ModbusException( "Reply timed out", ModbusException::REPLY_TIMEOUT ) );
		}
		else if ( bytesRead <= HEADER_SIZE + FOOTER_SIZE )
		{
			TA_THROW( ModbusException( "Reply packet too short.", ModbusException::REPLY_PACKET_TOO_SHORT ) );
		}

		reply.assign( m_replyBuffer + HEADER_SIZE, bytesRead - HEADER_SIZE - FOOTER_SIZE );

		// Calculate the expected CRC. Note the CRC calculation covers the address (in the header)
		// and the data.
		int startCrc = bytesRead - FOOTER_SIZE;
		m_crcCalculator.clear();
		m_crcCalculator.updateCrc( m_replyBuffer, startCrc );

		unsigned int loCrc = m_replyBuffer[ startCrc ] & 0xff;
		unsigned int hiCrc = m_replyBuffer[ startCrc + 1 ] & 0xff;
		unsigned int packetCrc = hiCrc << 8 | loCrc;

		if ( packetCrc != m_crcCalculator.getCrc() )
		{
            hexDump( "Reply with CRC error: ", m_replyBuffer, bytesRead, true );

			std::stringstream desc;
			desc << std::hex << "CRC Mistmatch; Expected: 0x"
				<< m_crcCalculator.getCrc() << " Received: 0x" << packetCrc << ".";
            
            TA_THROW( ModbusException( desc.str(), ModbusException::CRC_CALC_ERROR ) );
		}

        FUNCTION_EXIT;
    }


    void SteModbusTcpConnection::writeHoldingRegister( int address, WORD value )
    {
        THROW_NOT_SUPPORTED_EXCEPTION();
    }


    void SteModbusTcpConnection::writeHoldingRegister( int address, WORD andMask, WORD orMask )
    {
        THROW_NOT_SUPPORTED_EXCEPTION();
    }


    void SteModbusTcpConnection::writeHoldingRegisterBit( int address, int bit, bool value )
    {
        THROW_NOT_SUPPORTED_EXCEPTION();
    }
}
