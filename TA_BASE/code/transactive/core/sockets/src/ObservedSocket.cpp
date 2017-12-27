/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/ObservedSocket.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is the socket class that will manage the connection 
  * to the RTC server.  This class works with a observer pattern, 
  * running on its own thread.  It will notify the observer when  
  * the connection is lost or established as well as when there
  * is incoming data.
  */

#include "core/sockets/src/ObservedSocket.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "ace/OS.h"
#include "ace/os_include/netinet/os_tcp.h"
#include <sstream>

namespace TA_Base_Core
{

ObservedSocket::ObservedSocket( const std::string& host,
                                const std::string& service,
                                ISocketObserver& observer )
                                : m_host( host ),
                                  m_service( service ),
                                  m_tcpNoDelay( DEFAULT_TCP_NO_DELAY ? 1 : 0 ),
                                  m_observer( observer ),
                                  m_internalDataBuffer( NULL ),
                                  m_internalDataBufferSize(DEFAULT_READ_BUFFER_SIZE),
                                  m_toTerminate( false ),
                                  m_isConnected( false ),
                                  m_isPrevConnected( false ),
                                  m_sleepTime( DEFAULT_SLEEP_TIME ),
                                  m_reconnectTime( DEFAULT_RECONNECT_INTERVAL )

{
    u_short portNumber(0);
    std::stringstream ssPortNumber;
    ssPortNumber << m_service;
    ssPortNumber >> portNumber;
    
    if (portNumber != 0)
    {
        // port number given
        m_remoteAddress.set( portNumber, m_host.c_str() );
    }
    else
    {
        // named port given
        m_remoteAddress.set( m_service.c_str(), m_host.c_str() );
    }

    m_socketTimeout.msec(static_cast<long>(DEFAULT_SOCKET_TIMEOUT_MILLIS));

    m_internalDataBuffer = new unsigned char[m_internalDataBufferSize];
}


ObservedSocket::~ObservedSocket()
{
    delete[] m_internalDataBuffer;
}



ObservedSocket& ObservedSocket::setSocketTimeout( unsigned long socketTimeoutMillis )
{
    m_socketTimeout.msec(static_cast<long>(socketTimeoutMillis));
    
    return *this;
}


ObservedSocket& ObservedSocket::setTcpNoDelay( bool noDelay )
{
    m_tcpNoDelay = (noDelay ? 1 : 0);
    
    return *this;
}


ObservedSocket& ObservedSocket::setReadBufferSize( unsigned long bufferSize )
{
    delete[] m_internalDataBuffer;
    m_internalDataBufferSize = bufferSize;
    m_internalDataBuffer = new unsigned char[m_internalDataBufferSize];

    return *this;
}


ObservedSocket& ObservedSocket::setThreadSleepTime( unsigned short sleepTimeMillis )
{
    m_sleepTime = sleepTimeMillis;

    return *this;
}


ObservedSocket& ObservedSocket::setReconnectInterval( unsigned short reconnectIntervalMillis )
{
    m_reconnectTime = reconnectIntervalMillis;

    return *this;
}


void ObservedSocket::clearWriteBuffer()
{
    TA_Base_Core::ThreadGuard guard(m_writeBufferLock);
    
    // now the stl documentation says this is valid
    // but as always VC++ thinks it knows better...
    // stupid microsoft
    //m_writeBuffer.clear();

    m_writeBuffer.erase( m_writeBuffer.begin(), m_writeBuffer.end() );
}


void ObservedSocket::run()
{
    connect();

    while ( m_toTerminate == false )
    {
        if ( true == m_isConnected )
        {
            // Check if there are things to write 
            // If there is, send out to server first
            {
                TA_Base_Core::ThreadGuard guard(m_writeBufferLock);
                if(m_writeBuffer.size() != 0)
                {
                    sendData();
                }
            }

            // Check if there are information from remote party
            getIncomingData();
            if(m_isConnected == true && m_readBuffer.size() != 0)  // Socket is still connected
            { 
                notifyObserverOfIncomingData();
            }
        }
        else // try to connect until success
        {
            connect();
        }

        if ( true == m_isConnected )
        {
            // fast poll - get data quicker
            Thread::sleep(m_sleepTime);
        }
        else
        {
            // slower poll, give it time between attempts
            Thread::sleep(m_reconnectTime);
        }
    }
}


void ObservedSocket::terminate()
{
    m_toTerminate = true;
    close();
}


bool ObservedSocket::isConnected() const
{
    return m_isConnected;
}


void ObservedSocket::connect()
{
    if ( true == m_isConnected )
    {
        return;
    }

    close();
   
    int status = m_connector.connect (m_clientStream, m_remoteAddress, &m_socketTimeout);
    int error = errno;

    if ( status == -1 )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                    "Failed to connect to %s:%d. Error[%d][%s]", 
                    m_remoteAddress.get_host_name(), m_remoteAddress.get_port_number(),
                    error, ACE_OS::strerror(error) );

        m_isConnected = false;
        if(m_isPrevConnected != m_isConnected)
        {
            notifyConnectionLost();
            m_isPrevConnected = m_isConnected;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Failed to connect to %s:%d. Error[%s]", 
            m_remoteAddress.get_host_name(), m_remoteAddress.get_port_number(), formatErrorToString().c_str() );


    }
    else
    {
        m_isConnected = true;

        m_clientStream.enable(ACE_NONBLOCK);

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Connected to %s:%d.", m_remoteAddress.get_host_name(), m_remoteAddress.get_port_number() );

        status = m_clientStream.set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );
        if (status != 0)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to set TCP_NODELAY to %s socket option for connection to %s:%s.",
                m_tcpNoDelay ? "TRUE" : "FALSE", m_host.c_str(), m_service.c_str() );
        }

        if(m_isPrevConnected != m_isConnected)
        {
            notifyConnectionEstablished();
            m_isPrevConnected = m_isConnected;
        }
    }
}


void ObservedSocket::close()
{
    if (m_clientStream.close () == -1)
    {
        int error = errno;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Error closing socket connection to %s:%s, Error:[%d][%s]",
                    m_host.c_str(), m_service.c_str(), error, ACE_OS::strerror(error));
    }
}


void ObservedSocket::sendData()
{
    if(m_isConnected == true)
    {
        int status = m_clientStream.send_n(m_writeBuffer.c_str(), m_writeBuffer.size(), &m_socketTimeout);
        if(status <= 0 /*== -1*/)
        {
            int error = errno;
            if(isConnectionLost(error) == true || error == ETIME)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                            "Socket %s:%s send_n error: Native Error Code:[%d][%s]",
                            m_host.c_str(), m_service.c_str(), error, ACE_OS::strerror(error));
                m_isPrevConnected = m_isConnected;
                m_isConnected = false;
                notifyObserverOfWriteFailed();
            }
        }
        
        // again, microsoft STL is crap
        //m_writeBuffer.clear(); // Clear the buffer only if write succeeded
        m_writeBuffer.erase( m_writeBuffer.begin(), m_writeBuffer.end() );
    }
}

void ObservedSocket::getIncomingData()
{
    if(m_isConnected == true) // already connected
    {
        size_t byteCount = 0;
        int status = 0;
        size_t bytes_transferred = 0;

        // must be called before every operation - it seems it doesnt stick
        m_clientStream.enable(ACE_NONBLOCK);

        // also no timeout on read operations - otherwise they block for the timeout period
        status = m_clientStream.recv(m_internalDataBuffer, m_internalDataBufferSize);

        if (status > 0)
        {
            bytes_transferred = status;

            m_readBuffer.append(m_internalDataBuffer, bytes_transferred);

            // debugging
            if(bytes_transferred > 0)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                            "Socket %s:%s incoming %d bytes of data",
                            m_host.c_str(), m_service.c_str(), bytes_transferred);
            }
        }

        m_isPrevConnected = m_isConnected;
        if(status < 0) // error
        {
            int error = errno;
            m_isConnected = !isConnectionLost(error);

            // only log on error
            if (false == m_isConnected)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Socket %s:%s status = %d, Native Error Code:[%d][%s], isConnected:%s", 
                    m_host.c_str(), m_service.c_str(), status, error,
                    ACE_OS::strerror(error), (m_isConnected)?"True":"False");
            }
        }
        else if (0 == status) // graceful remote close
        {
            m_isConnected = false;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Socket %s:%s closed normally",
                        m_host.c_str(), m_service.c_str() );
        }

		//notify observer the lost of connection if the status changes to be disconnected
		if (false == m_isConnected)
		{
			notifyConnectionLost();
			m_isPrevConnected = m_isConnected;
		}
    }
}

void ObservedSocket::sendData(const SocketData& data)
{
    TA_Base_Core::ThreadGuard guard(m_writeBufferLock);

    m_writeBuffer.append(data);
}


void ObservedSocket::sendData(const std::vector<unsigned char>& data)
{
    TA_Base_Core::ThreadGuard guard(m_writeBufferLock);

    m_writeBuffer.append(data.begin(), data.end());
}


void ObservedSocket::sendData(const unsigned char* data, unsigned int dataSize)
{
    TA_Base_Core::ThreadGuard guard(m_writeBufferLock);

    m_writeBuffer.append(data, dataSize);
}


void ObservedSocket::notifyConnectionLost()
{
    m_observer.connectionLost( this );
}


void ObservedSocket::notifyConnectionEstablished()
{
    m_observer.connectionEstablished( this );
}


void ObservedSocket::notifyObserverOfIncomingData()
{
    m_observer.processReceivedData( m_readBuffer, this );
}


void ObservedSocket::notifyObserverOfWriteFailed()
{
    m_observer.writeFailed( m_writeBuffer, this );
}


bool ObservedSocket::isConnectionLost(int state) const
{
    switch(state)
    {
        case ECONNABORTED:
        case ECONNREFUSED:
        case ECONNRESET:
        case EHOSTDOWN:
        case EHOSTUNREACH:
        case ENETDOWN:
        case ENETUNREACH:
        case ENOTCONN:
        case ETIMEDOUT:
            return true;
    }
    return false;
}


std::string ObservedSocket::formatErrorToString() const
{
    std::string codeString;
    switch(errno)
    {
        case ETIME: 
        {
            codeString = "ETIME";
            break;
        }

        case EWOULDBLOCK:
        {
            codeString = "EWOULDBLOCK"; 
            break;
        }
        case EINPROGRESS: 
        {
            codeString = "EINPROGRESS"; 
            break;
        }
        case EALREADY:
        {
            codeString = "EALREADY"; 
            break;
        }
        case ENOTSOCK: 
        {
            codeString = "ENOTSOCK"; 
            break;
        }
        case EDESTADDRREQ: 
        {
            codeString = "EDESTADDRREQ"; 
            break;
        }
        case EMSGSIZE: 
        {
            codeString = "EMSGSIZE"; 
            break;
        }
        case EPROTOTYPE: 
        {
            codeString = "EPROTOTYPE"; 
            break;
        }
        case ENOPROTOOPT: 
        {
            codeString = "ENOPROTOOPT"; 
            break;
        }
        case EPROTONOSUPPORT: 
        {
            codeString = "EPROTONOSUPPORT"; 
            break;
        }
        case ESOCKTNOSUPPORT: 
        {
            codeString = "ESOCKTNOSUPPORT"; 
            break;
        }
        case EOPNOTSUPP: 
        {
            codeString = "EOPNOTSUPP"; 
            break;
        }
        case EPFNOSUPPORT: 
        {
            codeString = "EPFNOSUPPORT"; 
            break;
        }
        case EAFNOSUPPORT:
        {
            codeString = "EAFNOSUPPORT"; 
            break;
        }
        case EADDRINUSE:
        {
            codeString = "EADDRINUSE"; 
            break;
        }
        case EADDRNOTAVAIL:
        {
            codeString = "EADDRNOTAVAIL"; 
            break;
        }
        case ENETDOWN:
        {
            codeString = "ENETDOWN"; 
            break;
        }
        case ENETUNREACH:
        {
            codeString = "ENETUNREACH"; 
            break;
        }
        case ENETRESET:
        {
            codeString = "ENETRESET"; 
            break;
        }
        case ECONNABORTED:
        {
            codeString = "ECONNABORTED"; 
            break;
        }
        case ECONNRESET:
        {
            codeString = "ECONNRESET"; 
            break;
        }
        case ENOBUFS:
        {
            codeString = "ENOBUFS"; 
            break;
        }
        case EISCONN:
        {
            codeString = "EISCONN"; 
            break;
        }
        case ENOTCONN:
        {
            codeString = "ENOTCONN"; 
            break;
        }
        case ESHUTDOWN:
        {
            codeString = "ESHUTDOWN"; 
            break;
        }
        case ETOOMANYREFS:
        {
            codeString = "ETOOMANYREFS"; 
            break;
        }
        case ETIMEDOUT:
        {
            codeString = "ETIMEDOUT"; 
            break;
        }
        case ECONNREFUSED:
        {
            codeString = "ECONNREFUSED"; 
            break;
        }
        case ELOOP:
        {
            codeString = "ELOOP"; 
            break;
        }
        case EHOSTDOWN:
        {
            codeString = "EHOSTDOWN"; 
            break;
        }
        case EHOSTUNREACH:
        {
            codeString = "EHOSTUNREACH"; 
            break;
        }
        /*
        case EPROCLIM:
        {
            codeString = "EPROCLIM"; 
            break;
        }
        */
        case EUSERS:
        {
            codeString = "EUSERS"; 
            break;
        }
        case EDQUOT:
        {
            codeString = "EDQUOT"; 
            break;
        }
        case ESTALE:
        {
            codeString = "ESTALE"; 
            break;
        }
        case EREMOTE:
        {
            codeString = "EREMOTE"; 
            break;
        }
        default:
        {
            codeString = "UN-RECOGNIZED ERROR %d";
            break;
        }
    }
    return codeString;
}

}

