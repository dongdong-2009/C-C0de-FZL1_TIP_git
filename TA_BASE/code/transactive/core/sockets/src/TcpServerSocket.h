/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/TcpServerSocket.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#ifndef __TcpServerSocket_h__
#define __TcpServerSocket_h__

#if !defined( WIN32 )
#include <errno.h>
#endif // !defined( WIN32 )

#include "TcpSocket.h"
#include "core/exceptions/src/TcpSocketException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include <cerrno>

namespace TA_Base_Core
{
	//
	// Control codes.
	//
	const int SERVER_BLOCKING_ACCEPT (0);
	const int SERVER_NON_BLOCKING_ACCEPT (1);

	//
	// This header file defines the object classes used to communicate between
	// the various parts of the Transaction Processor (TP) and the client.
	//
	const int NUM_PENDING_CONNECTIONS (20);  // Number of pending connections

	//*****************************************************************************
	//
	// Class: TcpServerSocket
	//
	// Purpose:
	//
	//     This class is a complimentry class to the TcpSocket class.  This class
	//     provides the server side services that a TcpSocket class connects to.
	//
	//     When a client's connection request is granted, the class creates a new
	//     TcpSocket object to represent the client that connected.  This new
	//     object must be deleted when it is no longer required, as this class
	//     cannot do it.
	//
	//
	//*****************************************************************************
	template< class S > class TcpServerSocket
	{
	private:

		//
		// Name of the service we've connected to.
		//
		std::string   serviceName;

		//
		// Socket used to do communications.
		//
		S *connectedSocket;

	public:

		//
		// Constructor/destructor.
        //
        // Set pHost to BIND_TO_ALL_INTERFACES to listen on all interfaces.
		//
		TcpServerSocket( const std::string & pHost, const std::string & pService)
    		: serviceName( pService ),
      		  connectedSocket( 0 )
    	{
        	try
        	{
            	connectedSocket = new S( pService );
	
				#if defined( WIN32 )

            	const int lvEnable = 1;
            	int lvStatus = setsockopt( connectedSocket->getSocketId(), SOL_SOCKET, SO_REUSEADDR,
                                       ( char * ) & lvEnable, sizeof( lvEnable ) );

            	lvStatus = setsockopt( connectedSocket->getSocketId(), SOL_SOCKET, SO_KEEPALIVE,
                                   ( char * ) & lvEnable, sizeof( lvEnable ) );


				#else // !defined( WIN32 )

            	const int lvEnable = 1;
            	int lvStatus = setsockopt( connectedSocket->getSocketId(), SOL_SOCKET, SO_REUSEADDR,
                                       & lvEnable, sizeof( lvEnable ) );

            	lvStatus = setsockopt( connectedSocket->getSocketId(), SOL_SOCKET, SO_KEEPALIVE,
                                   & lvEnable, sizeof( lvEnable ) );

				#endif // !defined( WIN32 )

        	}
        	catch( ... )
        	{
            	if ( 0 != connectedSocket )
            	{
                	delete connectedSocket;
                	connectedSocket = 0;
            	}

            	//
            	// Rethrow the exception.
            	//
            	throw;
        	}
    	}

		virtual ~TcpServerSocket ()
    	{
        	try
        	{
            	delete connectedSocket;
        	}
        	catch( ... )
        	{
        	}
        	connectedSocket = 0;
    	}

		//
		// Bind to a socket.
		//
    		void bind()
    	{
        	std::string   lvErrorMessage;
        	bool lvInterrupted = false;
        	int         lvStatus;

        	do
        	{
            	lvStatus = ::bind( connectedSocket->getSocketId(),
                               ( struct sockaddr * ) connectedSocket->getSocketHandle(),
                               sizeof( struct sockaddr_in ) );

            	if ( ( -1 == lvStatus) && ( EINTR == errno ) )
            	{
                	lvInterrupted = true;
            	}
        	}
        	while ( true == lvInterrupted );

        	if (0 != lvStatus)
        	{
                	TA_THROW( TA_Base_Core::TcpSocketException( "Unable to bind to socket" ) );
        	}
    	}

		//
		// Listen on the socket for incoming clients.
		//
    		int listen()
    	{
        	int lvStatus;
        	bool lvInterrupted = false;

        	do
        	{
            	lvStatus = ::listen( connectedSocket->getSocketId(), NUM_PENDING_CONNECTIONS );
            	if ( ( -1 == lvStatus ) && ( EINTR == errno ) )
            	{
                	lvInterrupted = true;
            	}
        	} while ( true == lvInterrupted );
        	return lvStatus;
    	}

		//
		// Accept a connection from a client.
		//
		S *accept( int pFlags = SERVER_BLOCKING_ACCEPT, bool sleep = true )
    	{
        	//
        	// By default the socket is a non-blocking socket, which means that
        	// we may be required to block until a connection is made.  Therefore
        	// we loop here until a connection is made IFF we are doing a blocking
        	// accept (the default).
        	//
        	// If we are doing a non-blocking accept, and the accept would have blocked
        	// then we simply return a NULL object to indicate that there was no
        	// connection pending.
        	//
        	S *lvNewSocketObj = 0;
        	do
        	{
            	int lvSocketNameSize = 0;
            	struct sockaddr_in lvNewSocketName;
            	memset( & lvNewSocketName, 0, sizeof( lvNewSocketName ) );
            	lvSocketNameSize = sizeof( lvNewSocketName );
            	int lvNewSocket = 0;
            	bool lvInterrupted = false;
            	do
            	{

					#if !defined( WIN32 )
                	lvNewSocket = ::accept( connectedSocket->getSocketId(),
                                        ( struct sockaddr * ) & lvNewSocketName,
                                        ( socklen_t * )& lvSocketNameSize );
					#else // defined( WIN32 )
                	lvNewSocket = ::accept( connectedSocket->getSocketId(),
                                        ( struct sockaddr * ) & lvNewSocketName,
                                        & lvSocketNameSize );
					#endif // defined( WIN32 )

                	if ( ( -1 == lvNewSocket ) && ( EINTR == errno ) )
                	{
                    	lvInterrupted = true;
                	}
            	} while ( true == lvInterrupted );

            	//
            	// If the accept call would have blocked but didn't and we are
            	// doing a non-blocking accept, then we exit.  Otherwise, we
            	// process any errors as normal.  If there are no errors, we
            	// create a new TcpSocket object before exitting.
            	//
            	if (0 > lvNewSocket)
            	{
                	int lvErrno = errno;
					#if !defined( WIN32 )
                	if ( ( EWOULDBLOCK == lvErrno ) || ( EAGAIN == lvErrno ) )
					#else // defined( WIN32 )
                    if ( ( WSAEWOULDBLOCK == lvErrno ) || ( EAGAIN == lvErrno ) )
					#endif // defined( WIN32 )
                    {
                        if ( SERVER_NON_BLOCKING_ACCEPT == pFlags )
                        {
                            break;
                        }
                    }
            	}
            	else
            	{
                	lvNewSocketObj = new S( lvNewSocket, static_cast< const sockaddr_in * >( & lvNewSocketName ) );

                	//
                	// Get the name of the connected client.
                	//
                	//std::string lvClient = lvNewSocketObj->getClientName();
            	}

				if (true == sleep)
				{
					TA_Base_Core::Thread::sleep( 1000 );
				}
					
        	}
        	while ( ( SERVER_BLOCKING_ACCEPT == pFlags ) && ( 0 == lvNewSocketObj ) );

        	//
        	// Done.
        	//
        	return lvNewSocketObj;
    	}

		//
		// Return the name of the service.
		//
		std::string getServiceName() const
    	{
        	return serviceName;
    	}
		int getSocketId() const
		{
			return connectedSocket->getSocketId();
		}
	};
}
#endif // !__TcpServerSocket_h__
