/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/ModbusTcpConnection.h $
  * @author  Raymond Pau
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * A TCP/IP connection to a Modbus device.
  *
  * Note: In failure scenarios calls to the IModbus method implementations of this
  * class will block the calling thread for the TCP/IP connection time out period.
  * If this is an issue for the application, the Modbus communication threads need
  * to be isolated from the application code that needs to be responsive even
  * under failure scenarios.
  *
  */

#ifndef ModbusTcpConnection_h
#define ModbusTcpConnection_h

#include <string>
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Time_Value.h"

#include "bus/modbus_comms/src/ModbusConnection.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"

namespace TA_Base_Bus
{
    class ModbusTcpConnection : public ModbusConnection
    {

    public:
        /**
          * ModbusTcpConnection
          *
          * Constructor.
          *
          * @param host The name or IP address of the Modbus device to be
          * communicated to via a TCP/IP connection.
          */
        ModbusTcpConnection( const std::string& host,
                             const std::string& service,
                             long socketTimeout = 1000,
                             bool tcpNoDelay = false );

        /**
          * ~ModbusTcpConnection
          *
          * Destructor.
          */
        virtual ~ModbusTcpConnection();

        /**
          * enable
          *
          * Enables this object for Modbus device communications. This object
          * will subsequently initiate a connection to the actual modbus device
          * as required to respond to communication method calls.
          */
        virtual void enable();

        /**
          * disable
          *
          * Disables Modbus communications via the object. The current connection
          * to the Modbus device is disconnected. Any subsequent communication method
          * calls on this object will fail (until it is enabled again). The disable
          * method is useful in redundancy scenarios where a IModbus instance is
          * being used as a "hot standby" but should not take up any Modbus device
          * connection resources.
          */
        virtual void disable();

		/**
          * isConnected
          *
          * Checks the device connection and returns true if the device is connected
		  *
          * @return			True if the device is connected, false otherwise
          */
		virtual bool isConnected();

		int getTransactionID()
		{
			return  m_transactionID;
		}

    protected:
        /**
          * close
          *
          * Closes the socket connection if open and destroys the socket object.
          * This method does not change the enable status.
          */
        void close();

        /**
          * writeRequest
          *
          * Writes Modbus request data to the communication connection.
          *
          * @param request      The data buffer defining the request.
          */
        virtual void writeRequest( const std::string& request );

        /**
          * readReply
          *
          * Reads the reply to a Modbus request.
          *
          * @param reply        A buffer to store the reply data in. Note that the reply
          *                     data does not include any transport layer specific header
          *                     or footer data.
		  * @param expectedSize The expected number of bytes in the reply packet assuming
		  *                     the reply isn't an exception/error response.
          */
        virtual void readReply( std::string& reply, int expectedSize );


        /**
          * ensureConnected
          *
          * Connects to the Modbus device if a connection is not current.
          *
          * @exception TcpSocketException Thrown if a TCP/IP connection problem is
          * encountered.
          */
        void ensureConnected();

        /**
          * write
          *
          * Wraps the TcpSocket write function with some exception
          * processing code.
          *
          * @param buffer   The buffer to be written to the socket.
          * @param length   The length of the buffer to be written.
          * @return         The number of characters actually written.
          */
		int write( const char* buffer, const unsigned int length );

        /**
          * read
          *
          * Wraps the TcpSocket read function with some exception
          * processing code.
          *
          * @param buffer       The buffer to be read from the socket.
          * @param bytesRead    The actual number of bytes read.
          * @param length       The maximum allowed length of the read buffer returned.
          * @return             The number of characters actually read.
          */
		int read( char* buffer, const unsigned int bufferLength );

    private:
		/** this function is similar to read,
		**  but can use to read standard modbus data;
		*/
		int readModbusData( char* buffer, const unsigned int bufferLength );
        ModbusTcpConnection( const ModbusTcpConnection& );
        ModbusTcpConnection& operator=( const ModbusTcpConnection& );

    protected:
        int                                     m_transactionID;
        int                                     m_tcpNoDelay;
		volatile bool                           m_closed;
        std::string                             m_host;
        std::string                             m_service;
        ACE_Time_Value                          m_socketTimeout;
        ACE_SOCK_Connector                      m_sockConnector;
        ACE_SOCK_Stream                         m_sockStream;
        TA_Base_Core::ReadWriteThreadLockable   m_socketStreamLock;

        /** The transaction ID in the modbus header is sent as
          * 16 bits, this is used to make sure we dont have a 
          * transaction ID greater than this.
          */
        static const int MAX_TRANSACTION_ID;
        static const int MODBUS_PROTOCOL_ID;
        static const int MBAP_HEADER_SIZE;
        static const int ERROR_LENGTH;
     };
}

#endif // ModbusTcpConnection_h
