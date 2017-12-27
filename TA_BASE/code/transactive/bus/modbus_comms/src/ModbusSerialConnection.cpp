/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/ModbusSerialConnection.cpp $
  * @author  A. Ivankovic
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  */

#include    <iostream>
#include    <sstream>

#include    "core/utilities/src/DebugUtil.h"
#include    "core/utilities/src/TAAssert.h"

#include    "ModbusSerialConnection.h"
#include    "ModbusException.h"

using namespace std;

namespace TA_Base_Bus
{
    const int ModbusSerialConnection::HEADER_SIZE = 1;
    const int ModbusSerialConnection::FOOTER_SIZE = 2;


    ModbusSerialConnection::ModbusSerialConnection()
    {            
    }


    ModbusSerialConnection::~ModbusSerialConnection()
    {
    }


    void ModbusSerialConnection::enable()
    {
        ModbusConnection::enable();
    }


    void ModbusSerialConnection::disable()
    {
        ModbusConnection::disable();
    }


    void ModbusSerialConnection::writeRequest( const string& request )
    {
        TA_VERIFY( isEnabled(), "The connection is not enabled for communications" );

        string requestPacket;

        requestPacket += (char) getDeviceAddress();
        requestPacket.append( request );
        
        // Calculate the CRC for request and write it to the serial port.
        m_crcCalculator.clear();
        m_crcCalculator.updateCrc( requestPacket );

        requestPacket += LSB( m_crcCalculator.getCrc() );
        requestPacket += MSB( m_crcCalculator.getCrc() );

        m_serialPort.write( requestPacket );
    }


    void ModbusSerialConnection::readReply( string& reply, int expectedSize )
    {
        TA_VERIFY( isEnabled(), "The connection is not enabled for communications" );

        // Note that the Modbus serial protocol doesn't have any sizing
        // information in the protocol. It relies on an inter-character
        // timeout to expire to determine the end of a packet. So the
        // following read requests more than the maximum characters
        // possible. It will time out to indicate the end of the reply
        // packet.
        int bytesRead = m_serialPort.read( m_replyBuffer, expectedSize );
        int replyAddress = m_replyBuffer[0];

        if ( replyAddress != getDeviceAddress() )
        {
            stringstream err;
            err << "The reply's device address " << replyAddress 
                << " does not match the expected address of "
                << getDeviceAddress() << ".";
                TA_THROW( ModbusException( err.str(), ModbusException::INVALID_ADDRESS ) );
        }

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
            stringstream err;
            err << "The packet CRC ("
                << hex << packetCrc
                << " doesn't match the calculated CRC "
                << hex << m_crcCalculator.getCrc() << ".";

                TA_THROW( ModbusException( err.str(), ModbusException::CRC_CALC_ERROR ) );
        }
    }
}
