/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/ModbusTcpConnection.cpp $
  * @author  Raymond Pau
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  */

#include <sstream>
#include <iomanip>
#include <ace/Time_Value.h>
#include <ace/INET_Addr.h>
#include <ace/os_include/netinet/os_tcp.h> // ACE_5.6

#include "bus/modbus_comms/src/ModbusTcpConnection.h"
#include "bus/modbus_comms/src/ModbusException.h"

#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    const int ModbusTcpConnection::MAX_TRANSACTION_ID = 0xffff;     // maximum of 16 bits
    const int ModbusTcpConnection::MODBUS_PROTOCOL_ID = 0;
    const int ModbusTcpConnection::MBAP_HEADER_SIZE = 7;
    const int ModbusTcpConnection::ERROR_LENGTH     = 5;            // ERROR reply length.

    ModbusTcpConnection::ModbusTcpConnection( const std::string& host,
                                              const std::string& service,
                                              long socketTimeout,
                                              bool tcpNoDelay )
    :
    m_transactionID( 0 ),
    m_tcpNoDelay( tcpNoDelay ? 1 : 0 ),
    m_closed( true ),
    m_host( host ),
    m_service( service ),
    m_socketTimeout( socketTimeout / 1000,  (socketTimeout % 1000) * 1000 )
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }


    ModbusTcpConnection::~ModbusTcpConnection()
    {
        FUNCTION_ENTRY("Destructor");

        m_closed = true;

        TA_Base_Core::ThreadGuard guard( m_socketStreamLock );
		m_sockStream.close();

        FUNCTION_EXIT;
    }


    void ModbusTcpConnection::enable()
    {
        FUNCTION_ENTRY("enable");

        ModbusConnection::enable();
        
        FUNCTION_EXIT;
    }


    void ModbusTcpConnection::disable()
    {
        FUNCTION_ENTRY("disable");

        ModbusConnection::disable();
        close();

        FUNCTION_EXIT;
    }


    bool ModbusTcpConnection::isConnected()
    {
        return (false == m_closed);
    }


    void ModbusTcpConnection::close()
    {
        FUNCTION_ENTRY("close");

		m_closed = true;

        FUNCTION_EXIT;
    }


    void ModbusTcpConnection::writeRequest( const std::string& request )
    {
        FUNCTION_ENTRY("writeRequest");

        if ( request.size() == 0 )
        {
            TA_THROW( ModbusException( "No data in the request", ModbusException::NO_REQUEST_DATA ) );
        }

		// TD# 3012
		if ( false == isEnabled() )
		{
			TA_THROW( ModbusException( "The connection is not enabled for communications.", ModbusException::CONNECTION_NOT_ENABLED ) );
		}
		// end of TD# 3012

        // MBAP is an acronym for Modbus Application Protocol. The header for TCP/IP
        // communications is defined here.
        // char header[ MBAP_HEADER_SIZE ];

        // increment the transaction ID, 
        // and make sure we dont exceed 16 bits.
        ++m_transactionID;
        m_transactionID &= MAX_TRANSACTION_ID;


        // Note the packet length field includes the device address which is part of the
        // Modbus TCP header. So we add one to the request body length when filling out
        // the header.
        int length = request.size() + 1;
		/*
        header[0] = MSB( m_transactionID );
        header[1] = LSB( m_transactionID );
        header[2] = MSB( MODBUS_PROTOCOL_ID );
        header[3] = LSB( MODBUS_PROTOCOL_ID );
        header[4] = MSB( length );
        header[5] = LSB( length );
        header[6] = getDeviceAddress();
		*/
		std::string requestPacket;
		requestPacket += MSB( m_transactionID );
		requestPacket += LSB( m_transactionID );
		requestPacket += MSB( MODBUS_PROTOCOL_ID );
		requestPacket += LSB( MODBUS_PROTOCOL_ID );
		requestPacket += MSB( length );
		requestPacket += LSB( length );
		requestPacket += getDeviceAddress();

		requestPacket.append(request);
        //write( header, MBAP_HEADER_SIZE );
        write( requestPacket.c_str(), requestPacket.size() );

        FUNCTION_EXIT;
    }


    void ModbusTcpConnection::readReply( std::string& reply, int expectedSize )
    {
        FUNCTION_ENTRY("readReply");
		// TD# 3012
		if ( false == isEnabled() )
		{
			TA_THROW( ModbusException( "The connection is not enabled for communications.", ModbusException::CONNECTION_NOT_ENABLED ) );
		}
		// end of TD# 3012

        char header[ MBAP_HEADER_SIZE ];

        // Read the Header.
        int bytesRead = readModbusData( header, MBAP_HEADER_SIZE );
        if ( bytesRead != MBAP_HEADER_SIZE )
        {
            if ( bytesRead == 0 )
            {
                // If no bytes were read close the socket to force a re-connection
                // at the next comms attempt. This is needed because powering off
                // a modbus device could cause TcpSocket::read to return 0 bytes
                // with no error indication. The only error indication was that
                // no bytes were read.
                close();
            }

            std::stringstream err;
            err << "Failed to read reply header (" << bytesRead << " bytes were read)";
            TA_THROW( ModbusException( err.str(), ModbusException::REPLY_HEADER_READ_FAILED ) );
        }

        // Interpret the header contents.
        int replyTransactionID = getWord( header[0], header[1] );
        int protocolID = getWord( header[2], header[3] );

        // Note the reply size includes the device address which is part of the
        // Modbus TCP header. Hence the actual reply body size is one less than this.
        WORD replySize = getWord( header[4], header[5] ) - 1;
        int replyDevice = (unsigned char) header[6];

        // Read the Modbus packet body.
        // do this before checking the header, because if we throw
        // an exception we are left with this data in front of any valid data
        // following the next request
        reply.resize( replySize );
        bytesRead = readModbusData( const_cast<char*>( reply.data() ), replySize ); 

        // Check the header contents are as they should be.
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "reply transactionID = %d,current transID = %d", replyTransactionID, m_transactionID);
        if ( replyTransactionID != m_transactionID )
        {
			TA_THROW( ModbusException( "The request transaction ID doesn't match the reply.", ModbusException::INVALID_TRANSACTION_ID ) );
        }
        if ( protocolID != MODBUS_PROTOCOL_ID )
        {
            TA_THROW( ModbusException( "Incorrect Protocol ID in the reply packet.", ModbusException::INVALID_PROTOCOL_ID ) );
        }
        if ( replyDevice != getDeviceAddress() )
        {
            TA_THROW( ModbusException( "Reply packet has an incorrect device address.", ModbusException::INCORRECT_DEVICE_ADDRESS ) );
        }

        if ( (WORD)bytesRead != replySize )
        {
            std::stringstream err;
            err << "Failed to read the complete reply packet (Bytes read = " << bytesRead << ", expected = " << replySize << ")";
            TA_THROW( ModbusException( err.str(), ModbusException::REPLY_PACKET_READ_ERROR ) ); 
        }
        
        FUNCTION_EXIT;
    }
    

    void ModbusTcpConnection::ensureConnected()
    {
        FUNCTION_ENTRY("ensureConnected");

		if ( true == isConnected() )
		{
			return;
		}

        u_short port_number;
        std::stringstream ssPortNumber;
        ssPortNumber << m_service;
        ssPortNumber >> port_number;
        
        ACE_INET_Addr remote_sap( port_number, m_host.c_str() );
        ACE_Time_Value timeout( m_socketTimeout );

        TA_Base_Core::ThreadGuard guard( m_socketStreamLock );
        if ( m_sockStream.close() == -1 )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ACE_SOCK_Stream::close() failed before connecting to %s:%s",
                m_host.c_str(), m_service.c_str() );
        }
        
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Connecting to %s:%s...", m_host.c_str(), m_service.c_str() );

        int result = m_sockConnector.connect( m_sockStream, remote_sap, &timeout );
        
        if ( 0 != result )
        {
            if ( m_sockStream.close() == -1 )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ACE_SOCK_Stream::close() failed while connecting to %s:%s",
                    m_host.c_str(), m_service.c_str() );
            }

            std::ostringstream desc;
            desc << "Failed to connect to " << m_host << ":" << port_number << "; errno: " << errno;
            TA_THROW( TA_Base_Bus::ModbusException( desc.str(), TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Connected to %s:%s", m_host.c_str(), m_service.c_str() );
		m_closed = false;

        result = m_sockStream.set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );
        if (result != 0)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to set TCP_NODELAY to %s socket option for connection to %s:%s.",
                m_tcpNoDelay ? "TRUE" : "FALSE", m_host.c_str(), m_service.c_str() );
        }

        FUNCTION_EXIT;
    }


	int ModbusTcpConnection::write( const char* buffer, const unsigned int length )
    {
		FUNCTION_ENTRY("write");

        ensureConnected();

		hexDump( "Writing: ", buffer, length, true);

        unsigned int totalBytesWritten=0;

		while ( totalBytesWritten < length )
        {
			if ( false == isConnected() )
        	{
				TA_THROW( TA_Base_Bus::ModbusException( "Socket not connected when attempting a write.", TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
            }			

            ACE_Time_Value timeout( m_socketTimeout );
			
            TA_Base_Core::ThreadReadGuard guard( m_socketStreamLock );
            int result = m_sockStream.send_n( buffer + totalBytesWritten, length - totalBytesWritten, &timeout );
            
            if (result > 0)
			{
				// something was written update the total bytes written
				totalBytesWritten += result;
			}
			else
            {
                close();

                if (result < 0)
                {
                    int errorCode = errno;
                    if ( errorCode == ETIME )
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Timeout writing to %s:%s",
                            m_host.c_str(), m_service.c_str() );
                    }
                    else
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to write to %s:%s; Error Code: %d",
                            m_host.c_str(), m_service.c_str(), errorCode);
                    }
                }

                TA_THROW( TA_Base_Bus::ModbusException( "Failed to write to device.", TA_Base_Bus::ModbusException::SOCKET_WRITE_ERROR ) );
            }

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,  "wrote %d of %d bytes", totalBytesWritten, length);
		}

        FUNCTION_EXIT;
        return totalBytesWritten;
    }


	int ModbusTcpConnection::read( char* buffer, const unsigned int bufferLength )
    {
		FUNCTION_ENTRY("read");

        unsigned int totalBytesRead = 0;
        size_t bytesRead = 0;
        int result;
        
        while ( totalBytesRead < bufferLength )
        {
			if ( false == isConnected() )
        	{
				TA_THROW( TA_Base_Bus::ModbusException( "Socket not connected when attempting a read.", TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
            }			
            
            TA_Base_Core::ThreadReadGuard guard( m_socketStreamLock );

            ACE_Time_Value timeout( m_socketTimeout );

            if ((bufferLength > ERROR_LENGTH) && (totalBytesRead == 0))
            {
                // only read 5 bytes first to see if RTU feedback error so that no receive timeout.
                result = m_sockStream.recv_n( buffer + totalBytesRead, ERROR_LENGTH, &timeout, &bytesRead );
            }
            else
            {
                result = m_sockStream.recv_n( buffer + totalBytesRead, bufferLength - totalBytesRead, &timeout, &bytesRead );
            }

			//try to get the error reply from rtu first, if not the error reply, then need to check socket error.
			totalBytesRead += bytesRead;

            // peek into the buffer to see if an exception code has occurred.
            if ( (totalBytesRead == 5) && ((buffer[1] & 0x80)==0x80) )
            {
                char exceptionCode = buffer[2];
                TA_THROW( TA_Base_Bus::ModbusException( exceptionCode ) );
            }

            if ( result > 0 )
            {
                hexDump( "Read: ", buffer, bytesRead );
            }
            else
			{
				// Any error should force a closure of the socket
				close();

				std::stringstream ss;
				ss << "Modbus socket read error (error code = " << errno << ")";
                TA_THROW( TA_Base_Bus::ModbusException( ss.str(), TA_Base_Bus::ModbusException::SOCKET_READ_ERROR ) );
			}

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "read %d of %d bytes", totalBytesRead, bufferLength );
        }

		FUNCTION_EXIT;
        return totalBytesRead;
    }

	int ModbusTcpConnection::readModbusData( char* buffer, const unsigned int bufferLength )
    {
		FUNCTION_ENTRY("readModbusData");

        unsigned int totalBytesRead = 0;
        size_t bytesRead = 0;
        int result;
        
        while ( totalBytesRead < bufferLength )
        {
			if ( false == isConnected() )
        	{
				TA_THROW( TA_Base_Bus::ModbusException( "Socket not connected when attempting a read.", TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
            }			
            
            TA_Base_Core::ThreadReadGuard guard( m_socketStreamLock );

            ACE_Time_Value timeout( m_socketTimeout );

			result = m_sockStream.recv_n( buffer + totalBytesRead, bufferLength - totalBytesRead, &timeout, &bytesRead );

            if ( result > 0 )
            {
                hexDump( "Read: ", buffer, bytesRead );
            }
            else
			{
				// Any error should force a closure of the socket
				close();

				std::stringstream ss;
				ss << "Modbus socket read error (error code = " << errno << ")";
				TA_THROW( TA_Base_Bus::ModbusException( ss.str(), TA_Base_Bus::ModbusException::SOCKET_READ_ERROR ) );
			}

			totalBytesRead += bytesRead;

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "read %d of %d bytes", totalBytesRead, bufferLength );
        }

		FUNCTION_EXIT;
        return totalBytesRead;
    }
}
