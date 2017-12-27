/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/SerialPort.cpp $
  * @author  A. Ivankovic
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  */

#include    <sstream>
#include    <fcntl.h>
#include    <errno.h>
#include    <sys/types.h>
#include    <sys/stat.h>


#ifndef WIN32

#include    <unistd.h>

#include    <sys/ioctl.h>

#endif


#include    "core/utilities/src/TAAssert.h"

#include    "SerialPort.h"
#include    "SerialPortException.h"
#include    "ModbusException.h"

using namespace std;

namespace TA_Base_Bus
{
    const int SerialPort::INVALID_DESCRIPTOR = -1;


    SerialPort::SerialPort()
        :   m_fd( INVALID_DESCRIPTOR )
    {
        memset( &m_termIO, sizeof(m_termIO), 0 );

        m_termIO.c_iflag = IGNBRK;
        m_termIO.c_oflag = 0;
        m_termIO.c_cflag = CREAD | CLOCAL;
        m_termIO.c_lflag = 0;
        m_termIO.c_cc[ VINTR ] = 0;
        m_termIO.c_cc[ VQUIT ] = 0;
        m_termIO.c_cc[ VERASE ] = 0;
        m_termIO.c_cc[ VKILL ] = 0;
        m_termIO.c_cc[ VMIN ] = 0;            // Implies VTIME is a read time out.
        m_termIO.c_cc[ VTIME ] = 20;          // Default of 2.0 seconds for the read time out.
        m_termIO.c_cc[ VEOL2 ] = 0;
        m_termIO.c_cc[ VSUSP ] = 0;
        m_termIO.c_cc[ VSTART ] = 0;
        m_termIO.c_cc[ VSTOP ] = 0;
        m_termIO.c_cc[ VDISCARD ] = 0;
        m_termIO.c_cc[ VWERASE ] = 0;
        m_termIO.c_cc[ VLNEXT ] = 0;
        m_termIO.c_cc[ VREPRINT ] = 0;

        #ifdef linux
        m_termIO.c_cc[ VEOL ] = 0;
        m_termIO.c_cc[ VEOF ] = 0;
        m_termIO.c_cc[ VSWTC ] = 0;
        #else
        m_termIO.c_cc[ VSWTCH ] = 0;
        m_termIO.c_cc[ VDSUSP ] = 0;
        #endif

        // Set the defaults.
        setBaudRate( 9600 );
        setDataBits( BITS_8 );
        setParity( NO_PARITY );
        setStopBits( STOPBITS_1 );            
    }


    SerialPort::~SerialPort()
    {
        close();
    }


    void SerialPort::open( const char* name )
    {
        TA_ASSERT( name != 0, "port name must be specified" );
        TA_ASSERT( m_fd == INVALID_DESCRIPTOR, "The communication port is already open" );

        m_fd = ::open( name, O_RDWR );

        if ( m_fd >= 0 )
        {
            updateSettings();
        }
        else
        {
            m_fd = INVALID_DESCRIPTOR;
            stringstream err;
            err << "Error " << errno << " opening the " << name << " port.";
            throw SerialPortException( err.str() );
        }
    }


    void SerialPort::close()
    {
        if ( m_fd != INVALID_DESCRIPTOR )
        {
            ::close( m_fd );
            m_fd = INVALID_DESCRIPTOR;
        }
    }


    int SerialPort::write( const char* data, int dataSize )
    {
        TA_ASSERT( data != 0, "Data buffer is null" );
        TA_ASSERT( dataSize > 0, "Data size is not positive" );
        TA_ASSERT( m_fd != INVALID_DESCRIPTOR, "The communication port is not open" );

        int rc = ::write( m_fd, data, dataSize );

        if ( rc < 0 )
        {
            stringstream err;
            err << "Error " << errno << " occurred trying to write to the serial port.";
            throw SerialPortException( err.str() );
        }

        return rc;
    }


    int SerialPort::read( char& data )
    {
        TA_ASSERT( m_fd != INVALID_DESCRIPTOR, "The communication port is not open" );

        int rc = ::read( m_fd, &data, 1 );

        if ( rc < 0 )
        {
            stringstream err;
            err << "Error " << errno << " occurred trying to read from the serial port.";
            throw SerialPortException( err.str() );
        }

        return rc;
    }


    int SerialPort::read( char* data, int maxDataSize )
    {
        TA_ASSERT( data != 0, "Data buffer is null" );
        TA_ASSERT( maxDataSize > 0, "Data size is not positive" );
        TA_ASSERT( m_fd != INVALID_DESCRIPTOR, "The communication port is not open" );

        int i;

        for( i=0; i<maxDataSize; i++ )
        {
            if ( read( data[i] ) == 0 )
                break;
        }

        // Return the number of characters read.
        return i;
    }


    void SerialPort::setBaudRate( int rate )
    {
        int oldBaudRate = m_baudRate;
        m_baudRate = rate;

        switch ( rate )
        {
            case 50:
                return setSpeed( B50, rate );
            case 75:
                return setSpeed( B110, rate );
            case 110:
                return setSpeed( B110, rate );
            case 150:
                return setSpeed( B150, rate );
            case 200:
                return setSpeed( B200, rate );
            case 300:
                return setSpeed( B300, rate );
            case 600:
                return setSpeed( B600, rate );
            case 1200:
                return setSpeed( B1200, rate );
            case 2400:
                return setSpeed( B2400, rate );
            case 4800:
                return setSpeed( B4800, rate );
            case 9600:
                return setSpeed( B9600, rate );
            case 19200:
                return setSpeed( B19200, rate );
            case 38400:
                return setSpeed( B38400, rate );
            default:
                m_baudRate = oldBaudRate;
                throw SerialPortException( "Unsupported baud rate" );
				break;
        };
    }


    void SerialPort::setDataBits( DataBits bits )
    {
        // Clear the bit size flags.
        unsigned short cflags = m_termIO.c_cflag & ( ~CSIZE );

        switch ( bits )
        {
            case BITS_7:
                cflags |= CS7;        break;
            case BITS_8:
                cflags |= CS8;        break;
            default:
                throw SerialPortException( "Unsupported data bits setting" );
				break;
        };

        m_termIO.c_cflag = cflags;
        m_dataBits = bits;

        updateSettings();
    }


    void SerialPort::setParity( Parity newParity )
    {
        // Clear the parity flags.
        // PARENB set implies parity checking enabled.
        // PARODD set implies odd parity.
        // PARODD reset implies even parity.

        unsigned short cflags = m_termIO.c_cflag & ~( PARENB | PARODD );

        switch ( newParity )
        {
            case ODD_PARITY:
                cflags |= PARENB | PARODD;
                break;

            case NO_PARITY:
                cflags |= 0;            
                break;

            case EVEN_PARITY:
                cflags |= PARENB;            
                break;

            default:
                throw SerialPortException( "Unsupported parity setting" );
				break;
        };

        m_termIO.c_cflag = cflags;
        m_parity = newParity;

        updateSettings();
    }


    void SerialPort::setStopBits( StopBits bits )
    {
        // Clear the stop bits flag.
        unsigned short cflags = m_termIO.c_cflag & (~CSTOPB);

        switch ( bits )
        {
            case STOPBITS_1:
                cflags |= 0;        break;
            case STOPBITS_2:
                cflags |= CSTOPB;    break;
            default:
                throw SerialPortException( "Unsupported stop bits setting" );
				break;
        };

        m_termIO.c_cflag = cflags;
        m_stopBits = bits;

        updateSettings();
    }


    int SerialPort::getReadTimeOut() const
    {
        // The VTIME element is in tenths of a second, so multiply it by 100
        // to return it as a millisecond value.
        return m_termIO.c_cc[ VTIME ] * 100;
    }


    void SerialPort::setReadTimeOut( int milliTimeOut )
    {
        // The VTIME element is in tenths of a second, so we divide the milliseconds
        // by 100 to set it. The maximum value for c_cc[] is 255 as it is an
        // unsigned char, so some range checking is also needed.

        static const int MAX_MILLI_TIME_OUT = 255 * 100;

        if ( milliTimeOut > MAX_MILLI_TIME_OUT )
            milliTimeOut = MAX_MILLI_TIME_OUT;

        m_termIO.c_cc[ VTIME ] = milliTimeOut / 100;
        updateSettings();
    }


    void SerialPort::setSpeed( speed_t speed, int rate )
    {
        if ( cfsetispeed( &m_termIO, speed ) != 0 )
        {
            stringstream err;
            err << "Setting the input speed to " << rate << " (setting "
                << speed << ") failed.";
            throw SerialPortException( err.str() );
        }

        if ( cfsetospeed( &m_termIO, speed ) != 0 )
        {
            stringstream err;
            err << "Setting the output speed to " << rate << " (setting "
                << speed << ") failed.";
            throw SerialPortException( err.str() );
        }

        updateSettings();
    }


    void SerialPort::setModemControl( bool modemControl )
    {
        if ( modemControl )
        {
            m_termIO.c_cflag &= (~CLOCAL);
            m_termIO.c_cflag |= HUPCL;
        }
        else
        {
            m_termIO.c_cflag |= CLOCAL;
            m_termIO.c_cflag &= (~HUPCL);
        }

        updateSettings();
    }


    void SerialPort::updateSettings()
    {
        if ( m_fd != INVALID_DESCRIPTOR &&
             tcsetattr( m_fd, TCSADRAIN, &m_termIO ) < 0 )
        {
            stringstream err;
            err << "Error " << errno << " attempting to update IO settings.";
            throw SerialPortException( err.str() );
        }
    }
}
