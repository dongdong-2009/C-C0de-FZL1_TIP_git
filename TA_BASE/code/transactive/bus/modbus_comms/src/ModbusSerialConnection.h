/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/ModbusSerialConnection.h $
  * @author  A. Ivankovic
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * A serial (RS232/RS485) connection to a Modbus device.
  *
  */

#ifndef ModbusSerialConnection_h
#define ModbusSerialConnection_h

#include    "ModbusConnection.h"
#include    "SerialPort.h"
#include    "CrcCalculator.h"

namespace TA_Base_Bus
{
    class ModbusSerialConnection : public ModbusConnection
    {

    public:
        /**
          * ModbusSerialConnection
          *
          * Constructor.
          *
          * @param host The name or IP address of the Modbus device to be
          * communicated to via a TCP/IP connection.
          */
        ModbusSerialConnection();

        /**
          * ~ModbusSerialConnection
          *
          * Destructor.
          */
        virtual ~ModbusSerialConnection();

        /**
          * enable
          *
          * Enables this serial connection for Modbus device communications.
          */
        virtual void enable();

        /**
          * disable
          *
          * Disables Modbus communications via this serial connection. The serial
          * communication port is closed and any subsequent communication method
          * calls on this object will fail (until it is enabled again). The disable
          * method is useful in redundancy scenarios where a IModbus instance is
          * being used as a "hot standby" but should not take up any Modbus device
          * connection resources.
          */
        virtual void disable();

        /**
          * getSerialPort
          *
          * Gets the serial port object used by this modbus connection for
          * communications. The serial port object is deliberately exposed
          * so that communications settings can be altered as required.
          * Call the SerialPort::open method to open the port ready for
          * Modbus communications.
          */
        SerialPort& getSerialPort()
		{
			return m_serialPort;
		}

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

    private:
        ModbusSerialConnection( const ModbusSerialConnection& );
        ModbusSerialConnection& operator=( const ModbusSerialConnection& );

    private:
        static const int HEADER_SIZE;
        static const int FOOTER_SIZE;

        SerialPort m_serialPort;
        CrcCalculator m_crcCalculator;
        char m_replyBuffer[ 512 ];
    };
}

#endif // ModbusSerialConnection_h
