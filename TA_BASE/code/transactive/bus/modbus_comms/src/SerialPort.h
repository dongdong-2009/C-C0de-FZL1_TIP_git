/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/SerialPort.h $
  * @author  A. Ivankovic
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * A serial (RS232/RS485) connection to a Modbus device.
  *
  */

#ifndef SerialPort_h
#define SerialPort_h


#ifndef WIN32
#include    <termios.h>

#endif

namespace TA_Base_Bus
{
    typedef enum
    {
        BITS_ERROR = -1,
        BITS_8 = 8,
        BITS_7 = 7
    } DataBits;

    typedef enum
    {
        PARITY_ERROR = -1,
        NO_PARITY = 0,
        ODD_PARITY = 1,
        EVEN_PARITY = 2
    } Parity;

    typedef enum
    {
        STOPBITS_ERROR = 0,
        STOPBITS_1 = 1,
        STOPBITS_2 = 2
    } StopBits;

    class SerialPort
    {

    public:
        /**
          * SerialPort
          *
          * Constructor.
          */
        SerialPort( );

        /**
          * ~SerialPort
          *
          * Destructor.
          */
        virtual ~SerialPort();

        /**
          * open
          *
          * Opens the specified serial port for communications.
          *
          * @param portName     The name of the communication port to open.
          * @exception SerialPortException
          */
        void open( const char* portName );

        /**
          * close
          * 
          * Closes the communication port.
          */
        void close();

        /**
          * write
          *
          * Writes a block of data to the serial port.
          *
          * @return             The number of bytes actually written.
          * @param data         The data buffer to be written.
          * @param dataSize     The number of bytes of data to write.
          * @exception          SerialPortException
          */
        int write( const char* data, int dataSize );

        /**
          * write
          *
          * Writes a block of data to the serial port.
          *
          * @return             The number of bytes actually written.
          * @param data         The data buffer to be written.
          * @exception          SerialPortException
          */
        int write( const std::string data )
        {
            return write( data.c_str(), data.size() );
        }

        /**
          * write
          *
          * Writes a single byte to the serial port.
          *
          * @param data         The data byte to be written.
          * @exception          SerialPortException
          */
        void write( char data )     
        {
            write( &data, 1 ); 
        }

        /**
          * read
          * 
          * Reads a single character from the serial port.
          *
          * @return             The number of bytes actually read. A zero value
          *                     indicates that the read timed out.
          * @param data         The character read from the serial port. This is
          *                     only altered if the method returns a 1.
          * @exception          SerialPortException
          */
        int read( char& data );

        /**
          * read
          * 
          * Reads a block of data from the serial port.
          *
          * @return             The number of bytes actually read.
          * @param data         The data block read from the serial port.
          * @param maxDataSize  The maximum number of bytes that should be
          *                     read in this method call.
          * @exception          SerialPortException
          */
        int read( char* buffer, int maxDataSize );

        /**
          * getReadTimeOut
          *
          * The time out in milliseconds to wait for a data read to succeed.
          **/
        int getReadTimeOut() const;

        /**
          * setReadTimeOut
          * 
          * Sets the read time out in milliseconds.
          */
        void setReadTimeOut( int milliTimeOut );

        /**
          * getBaudRate
          *
          * The serial port baud rate as an integer.
          **/
        int getBaudRate() const
		{
			return m_baudRate;
		}

        /**
          * setBaudRate
          *
          * Sets the serial port baud rate.
          **/
        void setBaudRate( int baudRate );

        /**
          * getDataBits
          *
          * The number of data bits per byte communicated.
          **/
        DataBits getDataBits() const
		{
			return m_dataBits;
		}

        /**
          * setDataBits
          *
          * Sets the number of data bits per byte communicated.
          **/
        void setDataBits( DataBits dataBits );

        /**
          * getParity
          *
          * The data parity used for serial port communications.
          **/
        Parity getParity() const
		{
			return m_parity;
		}

        /**
          * setParity
          *
          * Sets the data parity used for serial port communications.
          **/
        void setParity( Parity parity );

        /**
          * getStopBits
          *
          * The number of stop bits per byte used in serial port 
          * communications.
          **/
        StopBits getStopBits() const
		{
			return m_stopBits;
		}

        /**
          * setParity
          *
          * Sets the number of stop bits per byte used in serial port 
          * communications.
          **/
        void setStopBits( StopBits stopBits );

        /**
          * setModemControl
          *
          * Sets whether modem control functionality is applied to the
          * communication port.
          **/
        void setModemControl( bool modemControl );

    private:
        SerialPort( const SerialPort& );
        SerialPort& operator=( const SerialPort& );

        /**
          * setSpeed
          *
          * Sets the baud rate using the speed_t type.
          *
          * @exception TransactiveException Thrown if an unsupported baud
          * rate is specified.
          */
        void setSpeed( speed_t, int baudRate );

        /**
          * updateSettings
          *
          * Updates the ioctl settings for the communication port.
          *
          */
        void updateSettings();

    private:
        int m_fd;
        int m_baudRate;
        DataBits m_dataBits;
        Parity m_parity;
        StopBits m_stopBits;
        static const int INVALID_DESCRIPTOR;



		#ifndef WIN32

        struct termios m_termIO;

		#endif
    };
}

#endif // SerialPort_h
