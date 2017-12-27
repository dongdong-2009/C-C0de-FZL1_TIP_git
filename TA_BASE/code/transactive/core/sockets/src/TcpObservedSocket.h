/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/sockets/src/TcpObservedSocket.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  */

#if !defined( __TcpObservedSocket_h__ )
#define __TcpObservedSocket_h__

#include <list>
#include <algorithm>

#if !defined( WIN32 )
#include <errno.h>
#endif // !defined( WIN32 )



#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "ITcpSocketObserver.h"
#include "core/exceptions/src/TcpSocketException.h"


/*
This template class provides a thread that observes when
data can be retrieved from a socket and notifies interested
observers that there is data available.
*/

namespace TA_Base_Core
{

    template < class SocketClass > class TcpObservedSocket : public TA_Base_Core::Thread
	{

    public:


		/**
 		* This constructor creates the socket and the thread that manages it.
 		* If the construction of the thread or the socket fails, then a
 		* TcpSocketException is thrown.
 		*/
		TcpObservedSocket( const std::string & pHost, 
                           const std::string & pService,
                  		   unsigned long pMaxWriteBufferLength = 0xffffffff, 
                           unsigned long pNonBlockingConnectTimout = DEFAULT_CONNECT_TIMEOUT,
                           unsigned long connectionTestTimeout = DEFAULT_CONNECTION_TEST_TIMEOUT, 
                           unsigned long readingTimeout = DEFAULT_READING_TIMEOUT )
    		: Thread(),
      		  m_connectedSocket( NULL ),
      		  m_shouldBeRunning( true ),
			  m_isFirstFail(true),
      		  m_isConnectedServerSocket( false ),
      		  m_maxWriteBufferLength( pMaxWriteBufferLength ),
              m_nonBlockingConnectTimeout( pNonBlockingConnectTimout ),
              m_connectionTestTimeout (connectionTestTimeout),
              m_readingTimeout(readingTimeout),
			  m_shouldClearWriteBuffer(false)
		{
			m_Host = pHost;
			m_Service = pService;
      		m_connectedSocket = new SocketClass( m_Host, 
                                                 m_Service, 
                                                 true, /* blocking */ 
                                                 m_nonBlockingConnectTimeout,
                                                 m_connectionTestTimeout,
                                                 m_readingTimeout );

    		std::string lvName = pHost;
    		lvName.append( "_" );
    		lvName.append( pService );
		}
		/**
 		* This constructor creates the socket and the thread that manages it.
 		* If the construction of the thread or the socket fails, then a
 		* TcpSocketException is thrown.
 		*/

		TcpObservedSocket(const std::string & pService,
                  		   unsigned long pMaxWriteBufferLength = 0xffffffff )
    		: Thread(),
      		  m_connectedSocket( NULL ),
      		  m_shouldBeRunning( true ),
			  m_isFirstFail(true),
      		  m_isConnectedServerSocket( true ), // don't want attempt connection to try and create a new one a host name not provided.
      		  m_maxWriteBufferLength( pMaxWriteBufferLength ),
              m_Service(pService),
			  m_shouldClearWriteBuffer(false)
		{
            m_connectedSocket = new SocketClass( m_Service.c_str() );

		}

		/**
 		* This constructor is used by the TcpServerSocket class in order to
 		* create client sockets that can be observed by the server.  This is
 		* different from the normal construction method because when a server
 		* accepts a connection from a client, the socket is already connected.
 		*
 		* @param const int pSocket            - the accepted socket ID
 		* @param const sockaddr_in * pAddress - the actual socket address
 		*
 		* @throws TcpSocketException
 		*/
		TcpObservedSocket( const int pSocket, const sockaddr_in * pAddress )
    		: Thread(),
      		  m_connectedSocket( NULL ),
              m_shouldBeRunning( true ),
			  m_isFirstFail(true),
      		  m_isConnectedServerSocket( true ),
      		  m_maxWriteBufferLength( 0xffffffff ),
			  m_shouldClearWriteBuffer(false)
    	{
      		m_connectedSocket = new SocketClass( pSocket, pAddress );
    	}

		/**
 		* The destructor simply cleans things up.
 		*/
		~TcpObservedSocket()
		{
    		m_shouldBeRunning = false;
				if ( ( Thread::getCurrentState () != Thread::THREAD_STATE_NEW ) &&
					 ( Thread::getCurrentState () != Thread::THREAD_STATE_UNKNOWN )) 
			{
    			Thread::terminateAndWait();
			}
			delete m_connectedSocket;
			m_connectedSocket = NULL;
		}

		/**
 		* Connect the socket to the remote end point.  If an exception is
 		* thrown then let the caller deal with it.
 		*/
		void connect()
		{
    		TA_Base_Core::ThreadGuard lvLock( m_connectedSocketLock );
        	m_connectedSocket->connect();

    		//
    		// Now that we've connected notify all the observers.
    		//
    		for( std::list< ITcpSocketObserver * >::iterator lvItr = observers.begin();
         		observers.end() != lvItr;
         		lvItr++ )
        	{
            	( * lvItr )->connectionEstablished();
        	}
		}

		/**
 		* Close the socket.  Calling this method results in the socket being
 		* closed and the observing thread to be terminated.
 		*
 		* @returns nothing
 		*
 		*/
		void close()
		{
   			 terminate();
		}

		// BCF: terminate() implements the pure virtual method from Thread and
		// closes the connection
		//
		void terminate()
		{
   			 //
   			 // Setting the value of m_shouldBeRunning causes not only the
   			 // thread to stop executing and be destroyed, but it also
   			 // results in the socket being closed.  These two behaviours
   			 // are side effects of the termination of the run() method
   			 // which is why there is no need to explicitly close the
   			 // socket.
   			 //
   			 m_shouldBeRunning = false;
		}

	    /**
   		    * This method adds an observer to the existing list of observers.  It
 		* ensures that the observer is not added twice by ignoring duplicates.
 		*
 		* @param ifTcpSocketObserver & pObserver - the observer to add
 		*
 		* @returns nothing
 		*
 		*/
		void addObserver(ITcpSocketObserver & pObserver)
		{
    		//
    		// Locate an existing observer.
    		//
    		const std::list< ITcpSocketObserver * >::iterator lvExists
        		= std::find( observers.begin(), observers.end(), & pObserver );

    		//
    		// If it doesn't exist then add it.
    		//
    		if ( observers.end() == lvExists )
        	{
            	observers.push_back( & pObserver );
        	}
		}

		/**
 		* This method removes an observer from the list of observers.  If the
 		* observer does not already exist in the list, it is simply ignored.
 		*
 		* @param ifTcpSocketObserver & pObserver - the observer to remove
 		*
 		* @returns nothing
 		*
 		*/
		void removeObserver(ITcpSocketObserver & pObserver)
		{
    		//
    		// Locate an existing observer.
    		//
    		std::list< ITcpSocketObserver * >::iterator lvExists
       		 = std::find( observers.begin(), observers.end(), & pObserver );

    		//
    		// Remove it if it exists.
    		//
    		if ( observers.end() != lvExists )
        	{
            	observers.remove( & pObserver );
        	}
		}

		/**
 		* setMaxWriteBufferLength
 		*
 		* This method sets the maximum write buffer length.
 		*
 		* @parameter unsigned int pMaxWriteBufferLength
 		*
 		* @returns previous value
 		*
 		* @throws nothing
 		*/
		unsigned int setMaxWriteBufferLength( unsigned int pMaxWriteBufferLength )
    	{
       		unsigned int lvOldSize = m_maxWriteBufferLength;
        	m_maxWriteBufferLength = pMaxWriteBufferLength;
        	return lvOldSize;
    	}

		/**
 		* This method appends the specifed data to be written to the end of
 		* the write buffer.  Once the data has been added to the buffer it is
 		* automatically written out.
 		*
 		* This method is thread safe.
 		*
 		* @param const char * pBuffer - data to be written
 		* @param const unsigned int pLength - length of the data
 		*
 		* @returns nothing
 		*
 		*/
		void write( const char * pBuffer, const unsigned int pLength )
    	{
            TA_Base_Core::ThreadGuard lvLock( writeBufferLock );
        	for( unsigned int i = 0; i < pLength; i++ )
        	{
            	writeBuffer.push_back( (unsigned char)pBuffer[i] );
        	}

        	//
        	// If the buffer gets too large we call the writeFailed()
        	// method and let the observers deal with it.  If an exception
        	// is thrown by an observer, then terminate the connection.
        	//
        	if( m_maxWriteBufferLength < writeBuffer.size() )
        	{
           	 	try
            	{
                	std::list< ITcpSocketObserver * >::iterator lvItr;
                	for( lvItr = observers.begin(); lvItr != observers.end(); lvItr++ )
                	{
                    	( * lvItr )->writeFailed();
                	}
            	}
            	catch( ... )
            	{
                	terminate();
            	}
        	}
    	}

		/* TD 10658
		 * allows option to clear write buffer if write fails
		 */
		void setClearWriteBufferOnFailedWrite(bool shouldClearWriteBuffer)
		{
		 	m_shouldClearWriteBuffer = true;
		}
		
		/**
 		* This method reads the contents of the read buffer and returns it in
 		* a second buffer.  The method can empty the contents of the read
 		* buffer.  This method automatically removes the read contents from the
 		* read buffer.
 		*
 		* This method is thread safe.
 		*
 		* @param char * pBuffer                    - buffer to receive data
 		* @param unsigned int *pActualSize         - actual size of the data read
 		* @param const unsigned int pMaximumLength - maximum length of the buffer
 		* @param const bool pEmptyBuffer = true    - remove read data from buffer
 		*
 		* @returns nothing
 		*
 		* @exception TcpSocketException            - either the buffer address or
 		*                                            the address of ActualSize is NULL
 		*/
		void read( char * pBuffer, unsigned int *pActualSize, const unsigned int pMaximumLength,
           		   const bool pEmptyBuffer = true )
		{
    		if ( ( 0 == pBuffer ) || ( 0 == pActualSize ) )
        	{
               		 TA_THROW(TA_Base_Core::TcpSocketException( "Invalid address" ));
        	}

        		TA_Base_Core::ThreadGuard lvLock( readBufferLock );

    		//
    		// Determine the number of bytes to be removed from the read buffer.
    		//
    		unsigned int lvLength = 0;
    		if ( pMaximumLength > readBuffer.size() )
        	{
            	lvLength = readBuffer.size();
        	}
    		else
       		{
            	lvLength = pMaximumLength;
        	}

    		//
    		// Do the copy. Erase the bytes if necessary.
    		//
			std::vector<unsigned char>::iterator p = readBuffer.begin();
			for( unsigned int i = 0; i < lvLength; i++ )
			{
    			pBuffer[i] = *p;
    			if( true == pEmptyBuffer )
    			{
       				readBuffer.erase(p);
    			}
    			else
    			{
       				 p++;
    			}
			}
   			*pActualSize = lvLength;
		}

		/**
 		* Get the number of registered observers.
 		*
 		*/
		int getNumberOfObservers() const
		{
    		return observers.size();
		}

		/**
 		* Determine if the socket is still connected.
 		*
 		* @returns true if the socket is connected
 		* @returns false if the socket is not connected
 		*/
		bool stillConnected() const
		{
    		TA_Base_Core::ThreadGuard lvLock( m_connectedSocketLock );
        	return m_connectedSocket->stillConnected();
		}

		/**
 		* This helper method simply returns the socket ID of the connected
 		* socket.  It is primarily used by the TcpServerSocket class whenever
 		* an observed socket is created by a server application.
 		*
 		* @returns int - the socket ID
 		*/
		inline int getSocketId() const
    	{
       		TA_Base_Core::ThreadGuard lvLock( m_connectedSocketLock );
        	return m_connectedSocket->getSocketId();
    	}

		/**
 		* This helper method simply returns the socket handle of the connected
 		* socket.  It is primarily used by the TcpServerSocket class whenever
 		* an observed socket is created by a server application.
 		*
 		* @returns const struct sockaddr_in * - socket handle
 		*/
		inline const struct sockaddr_in *getSocketHandle() const
    	{
        	TA_Base_Core::ThreadGuard lvLock( m_connectedSocketLock );
        	return m_connectedSocket->getSocketHandle();
    	}

		/**
 		* This helper method simply returns the name of the client socket.
 		* It is primarily used by the TcpServerSocket class whenever an
 		* observed socket is created by a server application.
 		*
 		* @returns std::string - name of the clien socket.
 		*/
		inline std::string getClientName() const
   		{
        	TA_Base_Core::ThreadGuard lvLock( m_connectedSocketLock );
        	return m_connectedSocket->getClientName();
    	}

		/*
 		* BCF:
 		* Implement the pure virtual method from Thread. This method blocks
 		* indefinitely. Don't call it directly - use Thread::start() instead
 		*/
		virtual void run()
    	{
            int const RECEIVE_BUFFER_SIZE = 1048576;

            // DJC: boost::scoped_array would be better but it would mean
            //      modification for the include path for all projects including
            //      this file.
            //      It should be allocated dynamically because otherwise it will
            //      allocate this on the stack (and may cause a stack overflow).
            char* lvTemporaryBuffer = NULL;

       		try
        	{

           		//
           	 	// The socket only attempts to connect to the remote socket when
            	// the thread is running.  Prior to that, the socket is
            	// unconnected.
            	//
            	attemptConnection();
                lvTemporaryBuffer = new char[ RECEIVE_BUFFER_SIZE ];
				memset(lvTemporaryBuffer, 0, RECEIVE_BUFFER_SIZE);

            	while( true == m_shouldBeRunning )
            	{
               		 //
               		 // Handle the possibility that the socket has been closed,
               		 // and notify the sockets observers.  The observers are
               		 // expected to want the socket to be reconnected
               		 // automatically, but if one of them says "no" then the
               		 // observed socket closes also and exits.
               		 //
               		 if( false == m_connectedSocket->stillConnected() )
               		 {
						LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
								TA_Base_Core::DebugUtil::DebugDebug, 
								"Connection to socket lost, calling close()" );
               					m_connectedSocket->close();

                   		LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
							  TA_Base_Core::DebugUtil::DebugWarn, 
							  "Connection to socket lost, notifying observers" );

               		    for( std::list< ITcpSocketObserver * >::iterator lvItr = observers.begin();
                         	  lvItr != observers.end(); lvItr++ )
                    	{
							if( false == ( * lvItr )->connectionLost() )
							{
									LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
											  TA_Base_Core::DebugUtil::DebugDebug, 
										  "Called connectionLost() on observer, returned false. Setting m_shouldBeRunning to false" );
								m_shouldBeRunning = false;
							}
                    	}

                    	if ( false == m_shouldBeRunning )
                    	{
                        	break;
                    	}

                    	//
                    	// Since the observers have decided to reconnect, we will
                    	// do so.
                    	//
                    	attemptConnection();
                	}

                	//
                	// Handle reads.
                	//
                	if ( m_connectedSocket->isReadRequired() )
                	{
						LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
							  TA_Base_Core::DebugUtil::DebugDebug, 
						  "Read is Required on socket %d", m_connectedSocket->getSocketId() );

                       		TA_Base_Core::ThreadGuard lvLock( readBufferLock );

                    	//
                    	// Read from the socket.
                    	//
                    	lvTemporaryBuffer[ 0 ] = '\0';
                    	unsigned int lvNumRead = 0;
                    	try
                    	{
                       		 m_connectedSocket->read( lvTemporaryBuffer, & lvNumRead, RECEIVE_BUFFER_SIZE );
                       		 if ( 0 != lvNumRead )
                       		 {
                       		     for( unsigned int i = 0; i < lvNumRead; i++ )
                       		     {
                       		         readBuffer.push_back( static_cast<unsigned char>(lvTemporaryBuffer[i]) );
                       		     }
                       		 }

                       		 //
                       		 // Notify all observers.
                       		 //
                       		 if ( true != readBuffer.empty() )
                       		 {
								LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
									TA_Base_Core::DebugUtil::DebugDebug, 
									"calling processReceivedData on observer of socket %d", m_connectedSocket->getSocketId() );

                       		     std::list< ITcpSocketObserver * >::iterator lvItr;
                       		     for( lvItr = observers.begin(); ( lvItr != observers.end() ) &&
                                     ( true != readBuffer.empty() ); lvItr++ )
                            	 {
                                	 ( * lvItr )->processReceivedData( readBuffer, m_connectedSocket );
                            	 }
                        	 }
                    	 }
                         catch( const TA_Base_Core::TcpSocketException & lvEx )
                    	 {
                        	 if( std::string( "Connection closed unexpectedly" ) == std::string( lvEx.what() ) )
                        	 {
								LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
								  				TA_Base_Core::DebugUtil::DebugWarn, 
                            				"Connection to socket lost, notifying observers" );

                            	 std::list< ITcpSocketObserver * >::iterator lvItr;
                            	 for( lvItr = observers.begin(); ( lvItr != observers.end() ) && ( true != readBuffer.empty() ); lvItr++ )
								 {
									 if( false == ( * lvItr )->connectionLost() )
									 {
											LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
													  TA_Base_Core::DebugUtil::DebugDebug, 
												  "Called connectionLost() on observer, returned false. Setting m_shouldBeRunning to false" );
											m_shouldBeRunning = false;
									 }
								 }

                     			 if ( false == m_shouldBeRunning )
								 {
                        		 	break;
								 }
                   			
                    			 attemptConnection();
                             }
                    	}
                	}

                	//
                	// If the buffer is not empty, keep passing it to all observers,
                	//
                	if ( true != readBuffer.empty() )
                	{
							LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
							  TA_Base_Core::DebugUtil::DebugDebug, 
						  "Calling processReceivedData() again on observers of socket %d", m_connectedSocket->getSocketId() );

                   		 std::list< ITcpSocketObserver * >::iterator lvItr;
                   		 for( lvItr = observers.begin(); ( lvItr != observers.end() ) &&
                             ( true != readBuffer.empty() ); lvItr++ )
                    	 {
                        	 ( * lvItr )->processReceivedData( readBuffer, m_connectedSocket );
                        	 if ( true == readBuffer.empty() )
                        	 {
                            	 break;
                        	 }
                    	 }
                	 }

                	 //
                	 // Handle writes.  The conditions that writes occur are:
                	 //
                	 // 1. There is data to be written
                	 // 2. The socket write buffer is not full
                	 //
                	 // If an error occurs, all the observers are notified and
                	 // the socket is closed.
                	 //
                	 try
                	 {
							TA_Base_Core::ThreadGuard guard( writeBufferLock );
                    	 if ( true != writeBuffer.empty() )
                    	 {
                   		    LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
								TA_Base_Core::DebugUtil::DebugDebug, 
								"Attempting to write %d bytes", writeBuffer.size() );

                        	int lvNumWritten =
                           		 m_connectedSocket->write( reinterpret_cast<char*>(&(writeBuffer[0])), writeBuffer.size() );

                   		    LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
								TA_Base_Core::DebugUtil::DebugDebug, 
								"Wrote %d bytes to socket", lvNumWritten );

                        	if ( 0 > lvNumWritten )
                        	{
								#if !defined( WIN32 )
                            	if ( EWOULDBLOCK != errno )
								#else // defined( WIN32 )
                                if ( WSAEWOULDBLOCK != WSAGetLastError() )
								#endif // defined( WIN32 )
                                {
									
					    				LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
								  			  TA_Base_Core::DebugUtil::DebugError, 

									#ifdef WIN32
							  			  "\nWrite to socker ERROR: %d\n", WSAGetLastError() );
									#else
							  			  "\nWrite to socker ERROR: %d\n", errno );
									#endif

                                        TA_THROW( TA_Base_Core::TcpSocketException( "Error writing to socket" ) );

                                }
                         	}
                         	else
                         	{
                            	//
                             	// Remove the data that was written from
                            	// the write buffer.
                            	//
									LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
										TA_Base_Core::DebugUtil::DebugDebug, 
									"Removing %d bytes from the write buffer. Buffer size=%d", lvNumWritten, writeBuffer.size() );

								// Just in case ...
								if ( lvNumWritten > static_cast< int >( writeBuffer.size() ) )
								{
									lvNumWritten = writeBuffer.size();
								}

                            	writeBuffer.erase( writeBuffer.begin(), writeBuffer.begin() + lvNumWritten );

                            	if( !writeBuffer.empty() )
                            	{
									char lvtmpMsg[64] = {0};
									sprintf( lvtmpMsg, "%d characters left in write buffer", writeBuffer.size() );
                   		     			LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
								  			TA_Base_Core::DebugUtil::DebugDebug, "%s", lvtmpMsg );

                            	}
                        	}
                    	}
                	}
                	catch( ... )
                	{
							LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
							  TA_Base_Core::DebugUtil::DebugDebug, 
						  "An exception was thrown while writing, calling WriteFailed on observers." );

                   		 std::list< ITcpSocketObserver * >::iterator lvItr;
                   		 for( lvItr = observers.begin(); lvItr != observers.end(); lvItr++ )
                    	 {
                        	( * lvItr )->writeFailed();
                       	 }
                	}
                	Thread::sleep(10);
            	}

        	}

        	//
        	// An unexpected exception has been thrown, so we terminate the
        	// thread.
        	//
        	catch( ... )
        	{
					LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
						TA_Base_Core::DebugUtil::DebugDebug, 
					"Unexpected exception on socket %d, closing socket", m_connectedSocket->getSocketId() );
        	}

			if ( lvTemporaryBuffer != NULL )
			{
				delete[] lvTemporaryBuffer;
				lvTemporaryBuffer = NULL;
			}

        	m_connectedSocket->close();
    	}

		private:

		//
		// Disable the copy constructor.
		//
		TcpObservedSocket( const TcpObservedSocket & theTcpObservedSocket );

		//
		// The read buffer.
		//
		std::vector<unsigned char> readBuffer;

		//
		// The read lock used to protect the read buffer.
		//
		TA_Base_Core::ReEntrantThreadLockable readBufferLock;

		//
		// The write buffer.
		//
		std::vector<unsigned char> writeBuffer;

		//
		// The wite lock on used to protect the write buffer.
		//
    	TA_Base_Core::ReEntrantThreadLockable writeBufferLock;

		//
		// The socket used to connect to.
		//
		SocketClass* m_connectedSocket;

		//
		// The socket lock used to protect the creation of the socket.
		//
    	mutable TA_Base_Core::ReEntrantThreadLockable m_connectedSocketLock;

		//
		// This attribtue stores the list of observers that must be called
		// whenever data arrives on the socket.
		//
		std::list< ITcpSocketObserver * > observers;

		//
		// Flag indicating that the thread should be running.
		//
		bool m_shouldBeRunning;
		bool m_isFirstFail;

		std::string 	m_Host;
		std::string     m_Service;

		//
		// Flag indicating that the socket is a server socket meaning that it
		// has already been connected by the TcpServerSocket::accept() method.
		//
		bool m_isConnectedServerSocket;

		unsigned long m_maxWriteBufferLength;

        unsigned long   m_nonBlockingConnectTimeout;
        unsigned long   m_connectionTestTimeout;
        unsigned long   m_readingTimeout;
		bool			m_shouldClearWriteBuffer;
		//
		// This method simply attempts to connect to the remote socket.
		//
		void attemptConnection()
		{
			if (m_isConnectedServerSocket)
			{
				// Don't reconnect
				return;
			}

 			if(m_shouldClearWriteBuffer)
			{
				TA_Base_Core::ThreadGuard lvLock( writeBufferLock );
				writeBuffer.clear();
			}
        	
 			LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
					  TA_Base_Core::DebugUtil::DebugDebug, 
				  "attemptConnection" );

        	bool lvConnected    = false;
			bool isLossNotified = false;

        	while( ( false == lvConnected ) && ( true == m_shouldBeRunning ) )
        	{
    			try
    			{
					LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
						  TA_Base_Core::DebugUtil::DebugDebug, 
							  "Attempting to connect.." );

					// Create a new socket - Note, only done for non server sockets where we know the host and port
					if (!m_isFirstFail)
					{
						TA_Base_Core::ThreadGuard lvLock( m_connectedSocketLock );
        	
						delete m_connectedSocket;
      					m_connectedSocket = new SocketClass( m_Host, 
                                                             m_Service, 
                                                             true, /* blocking */ 
                                                             m_nonBlockingConnectTimeout,
                                                             m_connectionTestTimeout,
                                                             m_readingTimeout );
					}

                    m_isFirstFail = false; // All subsequent attempts will re-create socket object.
            	
                    connect();
            		lvConnected = true;
        			
    			}
        		catch(TA_Base_Core::TcpSocketException& e)
    			{
					LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
						  TA_Base_Core::DebugUtil::DebugWarn, 
					  "TcpSocketException caught: %s", e.what() );

					if (!isLossNotified)
					{
						isLossNotified = true;
								   
						for( std::list< ITcpSocketObserver * >::iterator lvItr = observers.begin();lvItr != observers.end(); lvItr++ )
						{
							if( false == ( * lvItr )->connectionLost() )
							{
								LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
									  TA_Base_Core::DebugUtil::DebugDebug, 
									  "Called connectionLost() on observer, returned false. Setting m_shouldBeRunning to false" );
									 m_shouldBeRunning = false;
							}
						}

						if ( false == m_shouldBeRunning )
						{
							break;
						}   
					}

					sleep(1000);
    			}
			}
		}
	};
}

#endif // !defined( __TcpObservedSocket_h__ )

