/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/ISocket.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *     This file defines a TCP/IP socket class.
  */

#if !defined( __ISocket_h__ )
#define __ISocket_h__

#if !defined( WIN32 )
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
#include <winsock2.h>
#endif // !WIN32

#include <string>
#include <ctime>
#include "core/exceptions/src/TcpSocketException.h"

namespace TA_Base_Core
{

  	const unsigned long DEFAULT_CONNECT_TIMEOUT (15000);
    const unsigned long DEFAULT_CONNECTION_TEST_TIMEOUT (15000);
    const unsigned long DEFAULT_READING_TIMEOUT (15000);
    

	class ISocket
	{

	public:


		/**
  		* This method returns the underlying socket id ( or file handle )
  		* which can be used by system calls.
  		*
  		* @return int socketId - the socket id assigned to the socket
  		*
  		*/
		virtual int getSocketId() const = 0;

		/**
  		* This method returns the socket handle of the socket which can be
  		* used in system calls.
  		*
  		* @return const struct sockadd_in * - socket handle data
  		*
  		*/
		virtual  const struct sockaddr_in *getSocketHandle() const = 0;

		/**
  		* This method writes a buffer of data to the socket.  This method
  		* will block immediately if the socket is a blocking socket, and will
  		* return immediately if the socket is a non-blocking socket.
  		*
  		* @param const char * pBuffer       - address of the buffer to be written
  		* @param unsigned int pBufferLength - number of bytes in the buffer
  		*
  		* @return int - the number of bytes that were written
  		*
  		* @exception TcpSocketException     - this exception is thrown when
  		*                                     the socket address is
  		*                                     incomplete
  		*
  		*/
		virtual int write( const char * pBuffer, const int pLength ) = 0;

		/**
  		* This method reads a buffer of data from the socket.  This method
  		* will block immediately if the socket is a blocking socket, and will
  		* return immediately if the socket is a non-blocking socket.
  		*
  		* @param char * pBuffer       - address of the buffer to be
  		*                                     store the received data
  		* @param unsigned int *pActual      - number of bytes actually read
  		*
  		* @return int - zero on success, non-zero when the method failed and
  		*               the operating system error code is returned
  		*
  		* @exception TcpSocketException     - this exception is thrown when
  		*                                     Microsoft exceptions are
  		*                                     caught.  The Microsoft
  		*                                     exceptions are ignored because
  		*                                     we have insufficient
  		*                                     documentation regarding their use.
  		*
  		*/
		virtual int read ( char *pBuffer, unsigned int *pActual, const unsigned int pBufferLength ) = 0;


		//
		// Connect to the socket.
		//
    		virtual void connect () = 0;

		//
		// Determine if channel is connected to the specified service.
		//
		virtual bool isConnectedTo ( const std::string & service ) = 0;

		//
		// Determine if read required.
		//
		virtual bool isReadRequired (unsigned long timeoutMS = 0) const = 0;

		//
		// Get the name of the connected client.
		//
		virtual std::string getClientName() const = 0;

		//
		// Determine if the socket is blocking.
		//
		virtual bool isBlockingSocket() const = 0;

		//
		// Determine if the socket is still connected to the remote end point.
		//
		virtual bool stillConnected() const = 0;
	};	
}

#endif // !__ISocket_h__
