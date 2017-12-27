/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/ISocketObserver.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Taken and modified from KRTC Signalling agent.
  * 
  * This is an observer for an observed socket.
  * It is a re-implementation of the (unreliable) TcpObservedSocket using ACE sockets.
  * It allows for proper connection timeouts, and better notifications of disconnection.
  */

#if !defined(ISOCKETOBSERVER_H)
#define ISOCKETOBSERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

namespace TA_Base_Core
{
    class ObservedSocket;

    typedef std::basic_string<unsigned char> SocketData;

    class ISocketObserver
    {
        public:

            /**
             * processReceivedData
             *
             * This method is called whenever data is received from the socket.
             * Typically the implementation of this method will determine whether
             * a complete message is stored in the received data, and remove it if
             * there is.  The remaining data is left in the buffer.
             *
             * @param data - received data
             * @param socket - the socket the data came from
             *
             */
            virtual void processReceivedData( SocketData& data,
                                              ObservedSocket* socket ) = 0;

            /**
             * writeFailed
             *
             * This method is called whenever writing to the socket fails.
             *
             * @param writeBuffer - This is all the data queued to write to the socket
             * @param socket - the socket the write failure occured on
             */
            virtual void writeFailed( const SocketData& writeBuffer,
                                      ObservedSocket* socket ) = 0;

            /**
             * connectionLost
             *
             * This method is called whenever the observed socket loses the 
             * connection to the remote socket.
             * Also @see ObservedSocket::clearWriteBuffer()
             *
             * @param socket - the socket that lost its connection
             */
            virtual void connectionLost( ObservedSocket* socket ) = 0;

            /**
             * connectionEstablished
             *
             * This method is called whenever the observed socket establishes a
             * connection to the remote socket.  It is used in conjunction with
             * the connectionLost method in order to handle socket reconnection
             * at runtime.
             *
             * @param socket - the socket that was connected
             */
            virtual void connectionEstablished( ObservedSocket* socket ) = 0;

    };

}; 

#endif // !defined(ISOCKETOBSERVER_H)
