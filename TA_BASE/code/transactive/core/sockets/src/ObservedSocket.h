/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/ObservedSocket.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is a replacement of the old Tcp Observed Socket.
  * It is taken and modified from the KRTC Signalling agent.
  *
  * It is a re-implementation of the (unreliable) TcpObservedSocket using ACE sockets.
  * It allows for proper connection timeouts, and better notifications of disconnection.
  */

#if !defined(SIGNALLING_SOCKET_H)
#define SIGNALLING_SOCKET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "ace/Time_Value.h"

#include "core/sockets/src/ISocketObserver.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"

namespace
{
    static const unsigned short DEFAULT_SLEEP_TIME = 10;
    static const unsigned short DEFAULT_RECONNECT_INTERVAL = 500;
    static const unsigned long  DEFAULT_READ_BUFFER_SIZE = 0xFFFF;
    static const unsigned long  DEFAULT_SOCKET_TIMEOUT_MILLIS = 2000;
    static const bool           DEFAULT_TCP_NO_DELAY = true;
};

namespace TA_Base_Core
{

    class ObservedSocket : public TA_Base_Core::Thread
    {
    public:

        /**
         * Constructor taking the minimal set of configuration.
         * Will set default values:
         * Socket Timeout = DEFAULT_SOCKET_TIMEOUT_MILLIS = 2000
         * Tcp NoDelay = DEFAULT_TCP_NO_DELAY = true
         * Read Buffer Size = DEFAULT_READ_BUFFER_SIZE = 0xFFFF
         * Thread Sleep Time = DEFAULT_SLEEP_TIME = 10
         * Socket Reconnect Interval = DEFAULT_RECONNECT_INTERVAL = 500
         *
         * To override any of these, use the following syntax:
         * ObservedSocket* socket = new ObservedSocket(host, port,observer).setSocketTimeout(timeout).setThreadSleepTime(sleeptime) etc.
         * socket->start()
         *
         * OR
         *
         * ObservedSocket* socket = new ObservedSocket(host, port,observer)
         * socket->setSocketTimeout(timeout)
         * socket->setThreadSleepTime(sleeptime) etc.
         * socket->start()
         */
        ObservedSocket( const std::string& host,
                        const std::string& service,
                        ISocketObserver& observer );
                    
        ~ObservedSocket();

        /**
         * Sets the socket timeout (for connects, reads, and writes)
         *
         * @param socketTimeoutMillis the timeout value in milliseconds
         */
        ObservedSocket& setSocketTimeout( unsigned long socketTimeoutMillis );


        /**
         * Sets the TCP no delay socket option
         *
         * @param noDelay to enable/disable the option
         */
        ObservedSocket& setTcpNoDelay( bool noDelay );


        /**
         * Sets the read buffer size, the maximum data that can be read in one cycle
         *
         * @param bufferSize the buffer size to read in one operation
         */
        ObservedSocket& setReadBufferSize( unsigned long bufferSize );


        /**
         * Sets the sleep time for the socket poll thread
         *
         * @param sleepTime the time to wait between read/write attempts
         */
        ObservedSocket& setThreadSleepTime( unsigned short sleepTimeMillis );


        /**
         * Sets the interval between connection attempts
         *
         * @param reconnectIntervalMillis the time to wait between connection attempts
         */
        ObservedSocket& setReconnectInterval( unsigned short reconnectIntervalMillis );

        
        /**
         * Explicitly clear the write buffer.
         * In the event the connection is lost for example, it may not be desirable
         * to write previously buffered data on reconnect
         */
        void clearWriteBuffer();


        /**
          * sendData
          * 
          * This is meant for caller to pass the information to be 
          * send to the remote party.  This function will buffer the 
          * message into a internal buffer before sending the 
          * information out.
          * 
          * @param data the data to send
          */
        void sendData(const SocketData& data);


        /**
          * sendData
          * 
          * This is meant for caller to pass the information to be 
          * send to the remote party.  This function will buffer the 
          * message into a internal buffer before sending the 
          * information out.
          * 
          * @param data the data to send
          */
        void sendData(const std::vector<unsigned char>& data);


        /**
          * sendData
          * 
          * This is meant for caller to pass the information to be 
          * send to the remote party.  This function will buffer the 
          * message into a internal buffer before sending the 
          * information out.
          * 
          * @param data the data to send
          * @param dataSize the length of data to send
          */
        void sendData(const unsigned char* data, unsigned int dataSize);


        /**
          * run
          * 
          * run() is called by the new thread that is spawned when start() is called.
          * interface required by Thread class
          * 
          * @return void 
          * 
          * @exception <exceptions> Optional
          */
	    void run();


        /**
          * terminate
          * 
          * terminate() is called by terminateAndWait(), but it can also be called
          * interface required by the Thread class
          * 
          * @return void 
          * 
          * @exception <exceptions> Optional
          */
	    void terminate();


        /**
          * isConnected
          * 
          * Indicate to the caller if the socket connection is still valid, i.e. connected.
          * Be very careful where isConnected is call.
          * 
          * @return bool 
          * 
          * @exception <exceptions> Optional
          */
        bool isConnected() const;


    protected:
        //Uses a connector component connector_ to connect to a remote machine
        //and pass the connection into a stream component Signalling_stream_

        /**
          * connect
          * 
          * Establish a connection with the remote machine
          * Uses an ACE connector component to connect to a remote machine
          * and pass the connection into a stream component. It will perform a close
          * prior to a connect(...) attempt.
          */
        void connect();

        /**
          * close
          * 
          * Close down the connection properly.
          */
        void close();

        /**
          * sendData
          * 
          * Uses a stream component to send data to the remote host
          */
        void sendData();


        /**
          * getIncomingData
          * 
          * Checks the socket for any incoming information
          */
        void getIncomingData();

        /**
          * notifyConnectionLost
          * 
          * Notify observer when the connection is lost.
          * This will only be called once 
          */
        void notifyConnectionLost();

        /**
          * notifyConnectionEstablished
          * 
          * Notify observer when the connection is established.
          * This will only be called once 
          */
        void notifyConnectionEstablished();

        /**
          * notifyObserverOfIncomingData
          * 
          * Notify observer that there is data available.
          */
        void notifyObserverOfIncomingData();

        /**
          * notifyObserverOfWriteFailed
          * 
          * Notify observer when a sendPacket attempt has failed.
          * This will only be called once 
          */
        void notifyObserverOfWriteFailed();

    private:

        /**
          * formatErrorToString
          * 
          * Helper function that is used to decode the network status
          * 
          * @return std::string 
          */
        std::string formatErrorToString() const;


        /**
          * isConnectionLost
          * 
          * Checks the errno to see if the error was caused by a connection lost
          * 
          * @return bool 
          * @param : int state
          */
        bool isConnectionLost(int state) const;

    private:

        /**
         * Host name or IP address
         * only used once to build m_remoteAddress
         */
        std::string         m_host;

        /**
         * Port name or port number
         * only used once to build m_remoteAddress
         */
        std::string         m_service;

        /**
         * Value of the no delay socket option flag
         */
        int                 m_tcpNoDelay;

        /**
         * Holds the remote address to connect to
         */
        ACE_INET_Addr       m_remoteAddress;

        /**
          * ACE Stream class that is used to send/receive data from remote machine
          */
        ACE_SOCK_Stream     m_clientStream;

        /**
          * ACE Connector class that performs connect() to the remote host
          */
        ACE_SOCK_Connector  m_connector;

        /**
          * ACE Time object to hold socket-timeout information
          */
        ACE_Time_Value      m_socketTimeout;
    
        /**
          * The interface that is being used to communicate with the observer
          */
        ISocketObserver&    m_observer;
    
        /**
          * Internal buffer for the socket to write incoming information
          */
        unsigned char*      m_internalDataBuffer;

        /**
         * The size of m_internalDataBuffer
         */
        unsigned long       m_internalDataBufferSize;

        /**
          * Should the thread terminates/exit its run function
          */
        bool                m_toTerminate;

        /**
          * Is the socket currently connected to the remote machine
          */
        bool                m_isConnected;

        /**
          * Is the socket previously connected to the remote machine
          * Used mainly to ensure that the observers will only be called once
          */
        bool                m_isPrevConnected;

        /**
         * The sleep time for polling the socket for data
         */
        unsigned short      m_sleepTime;

        /**
         * The sleep time for socket reconnection
         */
        unsigned short      m_reconnectTime;

        /**
          * Buffer to store the data to be passed to observer
          */
        SocketData m_readBuffer;

        /**
          * Buffer to store data the observer wants to send to remote machine
          */
        SocketData m_writeBuffer;

        /**
          * Threadguards
          */
        TA_Base_Core::NonReEntrantThreadLockable m_writeBufferLock;
    };

};

#endif // !defined(SIGNALLING_SOCKET_H)
