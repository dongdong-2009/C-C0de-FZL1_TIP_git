/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/SteModbusTcpConnection.h $
  * @author  Raymond Pau
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * A TCP/IP connection to an STE RTU device using a variation of the Modbus
  * protocol specific to that device. The variation is that the implementation
  * is the equivalent of the Modbus serial protocol running over a TCP/IP
  * socket. The _official_ Modbus over TCP/IP includes an additional header
  * packet and excludes CRC checks.
  *
  * Note: In failure scenarios calls to the IModbus method implementations of this
  * class will block the calling thread for the TCP/IP connection time out period.
  * If this is an issue for the application, the Modbus communication threads need
  * to be isolated from the application code that needs to be responsive even
  * under failure scenarios.
  *
  */

#ifndef SteModbusTcpConnection_h
#define SteModbusTcpConnection_h

#include "bus/modbus_comms/src/CrcCalculator.h"
#include "bus/modbus_comms/src/ModbusTcpConnection.h"

namespace TA_Base_Bus
{
	class SingletonTimerUtil;

    class SteModbusTcpConnection : public ModbusTcpConnection
    {

    public:
        /**
          * SteModbusTcpConnection
          *
          * Constructor.
          *
          * @param host The name or IP address of the STE RTU device to be
          * communicated to via a TCP/IP connection.
          */
        SteModbusTcpConnection( const std::string& host,
                                const std::string& service,
                                long socketTimeout = 1000,
                                bool tcpNoDelay = false );


        /**
          * ~SteModbusTcpConnection
          *
          * Destructor.
          */
        virtual ~SteModbusTcpConnection();

        /**
          * writeHoldingRegister
          *
          * Writes a word to the specified holding register. The STE RTU does not
		  * support this functionality.
          *
          * @param address  The address of the register to be written to.
          * @param value    The value to write.
          * @exception      A ModbusException indicating that the functionality is not
		  *					supported is always thrown by this method implementation.
          */
        virtual void writeHoldingRegister( int address, WORD value );

        /**
          * writeHoldingRegister
          *
          * Writes to the specified holding register but also applies a combination
          * of an AND mask and an OR mask during the write. The algorithm used
          * is as follows:
          * Result = (Current Contents AND And_Mask) OR (Or_Mask AND And_Mask)
		  *
		  * The STE RTU does not support this functionality.
          *
          * @param address  The address of the register to be written to.
          * @param andMask  The AND mask to use.
          * @param mask     The OR mask to use.
          * @exception      A ModbusException indicating that the functionality is not
		  *					supported is always thrown by this method implementation.
          */
        virtual void writeHoldingRegister( int address, WORD andMask, WORD orMask );

        /**
          * writeHoldingRegister
          *
          * Writes to a specific bit within a holding register. The bit address
          * ranges from 1 through to 16 inclusive. The STE RTU does not support
		  * this functionality.
		  *
          * @param address  The address of the register to be written to.
          * @param bit      The bit number to set.
          * @param value    The value to set the bit to (true=1, false=0).
          * @exception      A ModbusException indicating that the functionality is not
		  *					supported is always thrown by this method implementation.
          */
        virtual void writeHoldingRegisterBit( int address, int bit, bool value );

    protected:

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
          * createSocket
          *
          * Function called by ensureConnected to create a TcpSocket.
          * 
          * Allows for subclasses to create and return a socket other than that
          * normally created, being a blocking TcpSocket
          *
          * @param host     The name of the host to connect to
          * @param service  The service (ie. port) to connect to
          * @return         The new'd TcpSocket
          */
        //virtual TA_Base_Core::TcpSocket * createSocket( const std::string& host, const std::string& service);

    private:
        SteModbusTcpConnection( const SteModbusTcpConnection& );
        SteModbusTcpConnection& operator=( const SteModbusTcpConnection& );

	private:
        static const int HEADER_SIZE;
        static const int FOOTER_SIZE;

		CrcCalculator m_crcCalculator;
        //char m_replyBuffer[ 2048 ];
		char m_replyBuffer[ 16 * 1024 ];
    };
}

#endif // SteModbusTcpConnection_h
