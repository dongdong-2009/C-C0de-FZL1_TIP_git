/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/ftp/src/FTPDataConnection.cpp $
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
#include <algorithm>
#include <iterator>

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TcpSocketException.h"

#include "core/ftp/src/FTPDataConnection.h"
#include "core/ftp/src/FTPException.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::FTPException;
using TA_Base_Core::TcpSocketException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	FTPDataConnection::FTPDataConnection()
	: m_socket( NULL ), m_dataBuffer( NULL ), m_connected( false )
	{
		// nothing yet
	}


	FTPDataConnection::~FTPDataConnection()
	{
		if ( m_socket != NULL )
		{
			close();
		}
	}


	void FTPDataConnection::connect( const std::string& serverAddress, const std::string& serverPort )
	{
		try
		{
			if ( m_socket != NULL )
			{
				// The socket is already connected - close and delete before connecting 
				// with a new socket
				close();
			}

			// create new socket
			m_socket = new TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket>( serverAddress, serverPort );
			// register as observer so that FTPDataConnection gets callback when incoming
			// data is detected.
			m_socket->addObserver( *this );
			// connect new socket to FTP server
		//	m_socket->connect();
			// start socket thread - FTPDataConnection will get callbacks when connection
			// is established/lost or if incoming data is detected
			m_socket->start();
		}
		catch( TcpSocketException& tex )
		{
			// Ignore 'Not Connected' exceptions
			if ( strstr( tex.what(), "Not Connected" ) == NULL )
			{
				throw;
			}
		}
	}


	void FTPDataConnection::close()
	{
		if ( m_socket != NULL )
		{
			// close connection
			m_socket->close();
			// kill thread
			m_socket->terminateAndWait();
			// delete socket
			delete m_socket;
			m_socket = NULL;
		}
	}
	

	void FTPDataConnection::setDataBuffer( std::vector<unsigned char>& buffer )
	{
		// Protect access to the buffer against multithreaded access
		ThreadGuard guard( m_bufferLock ); 
		m_dataBuffer = &buffer;
	}


	void FTPDataConnection::waitForTransferToComplete()
	{
		TA_ASSERT( m_socket != NULL, "Socket is not initialised" );

		// Block until connection is dropped

		while( m_connected )
		{
			TA_Base_Core::Thread::sleep( 10 );
		}
	}


    void FTPDataConnection::processReceivedData( std::vector<unsigned char>& pData, TA_Base_Core::ISocket* pSocket )
	{
		if ( m_dataBuffer != NULL )
		{
			// Protect access to the buffer against multithreaded access
			ThreadGuard guard( m_bufferLock );
			// Reserve enough extra space for the new data
			m_dataBuffer->reserve( m_dataBuffer->size() + pData.size() );
			// Append new data to the current buffer
			std::copy( pData.begin(), pData.end(), std::back_inserter( *m_dataBuffer ) );
			// Clear the socket's buffer
			pData.clear();
		} 
	}


    void FTPDataConnection::writeData(std::stringstream& fileStream)
	{
		if ( m_socket != NULL )
		{
			m_socket->write(fileStream.str().c_str(), fileStream.str().length());
		} 
	}


    void FTPDataConnection::writeFailed()
	{
		// No writes implemented - not applicable
	}


    bool FTPDataConnection::connectionLost()
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Data connection closed" );
		m_connected = false;
		return false;
	}


    void FTPDataConnection::connectionEstablished()
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Data connection established" );
		m_connected = true;
	}
}


