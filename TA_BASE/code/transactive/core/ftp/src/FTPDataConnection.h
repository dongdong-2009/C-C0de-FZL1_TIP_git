#ifndef FTPDATACONNECTION_H
#define FTPDATACONNECTION_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/ftp/src/FTPDataConnection.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Class representing the FTP data connection, by which
 * the actual file transfer between client and server takes 
 * place.
 *
 */

#include <vector>

#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class FTPDataConnection : public TA_Base_Core::ITcpSocketObserver
	{

	public:

		/**
		 * Constructor
		 *
		 */
		FTPDataConnection();


		/**
		 * Destructor
		 *
		 * If the connection is still open, it is closed.
		 *
		 */
		virtual ~FTPDataConnection();


		/**
		 * connect()
		 *
		 * Creates and connects the socket member variable to the address and port 
		 * specified. Registers with the new socket as an observer (in order to receive
		 * callbacks). If the socket member is already initialised, it is closed and
		 * deleted before commencing with the creation of a new socket and connection.
		 *
		 * @param	serverAddress		the IP address/hostname to connect to
		 * @param	serverPort			the port number to connect to
		 *
		 */
		void connect( const std::string& serverAddress, const std::string& serverPort );


		/**
		 * close()
		 *
		 * If the socket member variable is initialised, it is closed and deleted.
		 *
		 */
		void close();


		/**
		 * setDataBuffer()
		 *
		 * Sets the current buffer. All received data will be appended to this buffer.
		 *
		 * @param	buffer		the new data buffer to append to
		 *
		 */
		void setDataBuffer( std::vector<unsigned char>& buffer );

	
		/**
		 * waitForTransferToComplete()
		 *
		 * Blocks until the connection has been closed (by the FTP server). 
		 * This signals the completion of the file transfer.
		 *
		 */
		void waitForTransferToComplete();



        /**
         * processReceivedData()
         *
		 * Inherited from ITcpSocketObserver. When incoming data is detected by the
		 * socket thread, this callback is executed. Incoming data is appended to the
		 * current buffer (if set) and the socket's buffer is cleared.
		 *
		 * @param	pData		the sockets read buffer, containing the received data
		 * @param	pSocket		the socket that the data was received from 
		 *
         */
        virtual void processReceivedData( std::vector<unsigned char>& pData, TA_Base_Core::ISocket* pSocket );

		virtual void writeData(std::stringstream& fileStream);
		
        /**
         * writeFailed()
         *
		 * Inherited from ITcpSocketObserver. Not applicable as no write operations 
		 * are implemented for FTPDataConnection.
		 *
         */
        virtual void writeFailed();


        /**
         * connectionLost()
         *
		 * Inherited from ITcpSocketObserver. This callback is executed by the socket
		 * thread when a connection loss is detected. This may be the result of 
		 * network error or the end of the file transfer.
		 *
		 * @return	whether or not to attempt re-connection (always false for FTPDataConnection)
		 *
         */
        virtual bool connectionLost();


        /**
         * connectionEstablished()
         *
		 * Inherited from ITcpSocketObserver. This callback is executed by the socket
		 * thread when connection establishment is detected.
		 *
         */
        virtual void connectionEstablished();


	private:

		TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket>*		m_socket;
			std::vector<unsigned char>*						m_dataBuffer;
			TA_Base_Core::ReEntrantThreadLockable				m_bufferLock;
			bool											m_connected;

	};
}


#endif // FTPCONTROLCONNECTION_H
