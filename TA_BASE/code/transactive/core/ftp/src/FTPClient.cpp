/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/ftp/src/FTPClient.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by:  $Author: Ouyang $
 * 
 * Implementation of FTP client. FTPClient sends
 * commands to an FTP server and processes responses.
 * FTPClient is directed by FTPManager.
 *
 */
#include <iostream>
#include <cstring>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"
#include "core/exceptions/src/TcpSocketException.h"

#include "core/ftp/src/FTPClient.h"
#include "core/ftp/src/FTPException.h"
#include "core/ftp/src/FTPConstants.h"
#include "core/ftp/src/FTPControlConnection.h"
#include "core/ftp/src/FTPDataConnection.h"
#include "core/ftp/src/FTPReply.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::FTPException;
using TA_Base_Core::TcpSocketException;

namespace TA_Base_Core
{
	FTPClient::FTPClient()
	: m_controlConnection( NULL ), m_dataConnection( NULL )
	{
		m_dataConnection = new FTPDataConnection();
	}



	FTPClient::~FTPClient()
	{
		if ( m_controlConnection != NULL )
		{
			delete m_controlConnection;
			m_controlConnection = NULL;
		}

		if ( m_dataConnection != NULL )
		{
			delete m_dataConnection;
			m_dataConnection = NULL;
		}
	}


	void FTPClient::connectToServer( const std::string& hostAddress, unsigned int hostPort )
	{
		// Initialise the control connection
		char portAsChar[12] = {0};
		sprintf( portAsChar, "%d", hostPort );
		std::string portAsString( portAsChar );
		m_controlConnection = new FTPControlConnection( hostAddress, portAsString );

		try
		{
			// Make the control connection to the server
			m_controlConnection->connect();
		}
		catch ( const TA_Base_Core::TcpSocketException& se )
		{
			TA_THROW( FTPException( se.what() ) );
		}
		
		// Receive the welcome messsage
		FTPReplyList connectReplies;
		receiveReplies( connectReplies, 30 );
		
		// Process the reply and confirm that the server is ready 
		FTPReply::FTPReplyType connectReplyType;
		std::string connectReplyString;
		processReplyList( connectReplies, connectReplyType, connectReplyString );
		if ( connectReplyType != FTPReply::Positive_Completion )
		{
			std::string exceptionString( "Unexpected reply received upon connection: " );
			exceptionString.append( connectReplyString );
			TA_THROW( FTPException( exceptionString.c_str() ) );
		}
	}


	void FTPClient::userLogin( const std::string& userName, const std::string& userPassword )
	{
		// Format user name command
		std::string userLoginCommand( FTP_COMMAND_USERNAME );
		userLoginCommand.append( " " );	
		userLoginCommand.append( userName );	

		// Send command to server
		sendCommand( userLoginCommand );

		// Receive replies and process
 		FTPReplyList userLoginReplies;
 		receiveReplies( userLoginReplies, 30 );
		FTPReply::FTPReplyType userReply;
		std::string userReplyString;
		processReplyList( userLoginReplies, userReply, userReplyString );

		if ( userReply == FTPReply::Positive_Completion )
		{
			//
			// No password required for this user
			//
			return;
		}
		else if ( userReply == FTPReply::Positive_Intermediate )
		{
			//
			// Password validation required
			//

			// Format password command
			std::string userPasswordCommand( FTP_COMMAND_PASSWORD );
			userPasswordCommand.append( " " );	
			userPasswordCommand.append( userPassword );	

			// Send command and receive reply
			sendCommand( userPasswordCommand );
 			userLoginReplies.clear();
 			receiveReplies( userLoginReplies, 30 );

			// Process reply for positive completion
			processReplyList( userLoginReplies, userReply, userReplyString );
			if ( userReply != FTPReply::Positive_Completion )
			{
				std::string exceptionString( "Unexpected reply received after user password command: " );
				exceptionString.append( userReplyString );
				TA_THROW( FTPException( exceptionString.c_str() ) );
			}
		}
		else
		{
			std::string exceptionString( "Unexpected reply received after user name command: " );
			exceptionString.append( userReplyString );
			TA_THROW( FTPException( exceptionString.c_str() ) );
		}
	}

	
	void FTPClient::setFileType( FileType type )
	{
		// Format command
		std::string setFileTypeCommand( FTP_COMMAND_CHANGETYPE );
		setFileTypeCommand.append( " " );
		setFileTypeCommand.append( type == Ascii ? "A" : "I" );

		sendCommand( setFileTypeCommand );

		FTPReplyList setFileTypeReplies;
		receiveReplies( setFileTypeReplies, 30 );

		// Process reply for positive completion
		FTPReply::FTPReplyType setFileTypeReplyType;
		std::string setFileTypeReplyString;
		processReplyList( setFileTypeReplies, setFileTypeReplyType, setFileTypeReplyString );

		if ( setFileTypeReplyType != FTPReply::Positive_Completion )
		{
			std::string exceptionString( "Unexpected reply received after set file type command: " );
			exceptionString.append( setFileTypeReplyString );
			TA_THROW( FTPException( exceptionString.c_str() ) );
		}
	}

	
	void FTPClient::changeDirectory( const std::string& newDirectory )
	{
		std::string changeDirCommand( FTP_COMMAND_CHANGEDIR );
		changeDirCommand.append( " " );
		changeDirCommand.append( newDirectory );

		sendCommand( changeDirCommand );

		FTPReplyList changeDirReplies;
		receiveReplies( changeDirReplies, 30 );

		FTPReply::FTPReplyType changeDirReplyType;
		std::string changeDirReplyString;
		processReplyList( changeDirReplies, changeDirReplyType, changeDirReplyString );

		if ( changeDirReplyType != FTPReply::Positive_Completion )
		{
			std::string exceptionString( "Unexpected reply received after change directory command: " );
			exceptionString.append( changeDirReplyString );
			TA_THROW( FTPException( exceptionString.c_str() ) );
		}
	}


	void FTPClient::retrieveFile( const std::string& fileName, std::vector<unsigned char>& fileBuffer  )
	{
		TA_ASSERT( m_dataConnection != NULL, "NULL Data Connection pointer" );

		// Instruct server to enter passive mode - the server replies with an address and port
		// to connect to
		std::string serverDataAddress, serverDataPort;
		setPassiveMode( serverDataAddress, serverDataPort );

		// Sets the data connection to use the supplied buffer to store the file
		m_dataConnection->setDataBuffer( fileBuffer );
		// Connect the data connection using the info supplied by the server
		m_dataConnection->connect( serverDataAddress, serverDataPort );

		// Format file retrieval command
		std::string retrieveFileCommand( FTP_COMMAND_RETRIEVEFILE );
		retrieveFileCommand.append( " " );
		retrieveFileCommand.append( fileName );

		sendCommand( retrieveFileCommand );
		time_t startTime = time(NULL);

		// receive reply (no timeout as the file transfer may take a long time)
		FTPReplyList retrieveFileReplies;
		receiveReplies( retrieveFileReplies, 0 );

		// process reply for positive completion
        FTPReply::FTPReplyType retrieveFileReplyType;
        std::string retrieveFileReplyString;
        processReplyList( retrieveFileReplies, retrieveFileReplyType, retrieveFileReplyString );

        if ( retrieveFileReplyType != FTPReply::Positive_Completion )
        {
        	std::string exceptionString( "Unexpected reply received after retrieve file command: " );
            exceptionString.append( retrieveFileReplyString );
            TA_THROW( FTPException( exceptionString.c_str() ) );
        }

		// the reply on the control connection does not necessarily mean that the data connection
		// has received all of the data. wait until the server closes the data connection
		m_dataConnection->waitForTransferToComplete();

		// Log the time taken for the transfer
		time_t endTime = time(NULL);
		if ( endTime > startTime )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				 "%d bytes received in %d seconds (%d bytes/sec)", fileBuffer.size(), 
				 endTime - startTime, fileBuffer.size() / (endTime-startTime) );
		}
		else
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				 "%d bytes received in < 1 second", fileBuffer.size() ) ;
		}
	}

	void FTPClient::sendFile( const std::string& fileName, std::stringstream& fileStream )
	{
		// Instruct server to enter passive mode - the server replies with an address and port
		// to connect to
		std::string serverDataAddress, serverDataPort;
		setPassiveMode( serverDataAddress, serverDataPort );
		
		// Format file retrieval command
		std::string sendFileCommand( FTP_COMMAND_SENDFILE );
		sendFileCommand.append( " " );
		sendFileCommand.append( fileName );
		
		sendCommand( sendFileCommand );

		time_t startTime = time(NULL);

		//To use blocking transfer mode, so need not to use m_dataConnection member to transfer data
		TcpSocket dataConnection(serverDataAddress, serverDataPort);
		
		dataConnection.connect();
		dataConnection.write(fileStream.str().c_str(), fileStream.str().length());
		dataConnection.close();
		
		// receive reply (no timeout as the file transfer may take a long time)
		FTPReplyList replies;
		receiveReplies( replies, 0 );
		
		// process reply for positive completion
        FTPReply::FTPReplyType replyType;
        std::string replyString;
        processReplyList( replies, replyType, replyString );
		
        if ( replyType != FTPReply::Positive_Completion )
        {
			std::string exceptionString( "Unexpected reply received after send file command: " );
            exceptionString.append( replyString );
            TA_THROW( FTPException( exceptionString.c_str() ) );
        }

		// Log the time taken for the transfer
		time_t endTime = time(NULL);
		if ( endTime > startTime )
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				"%d bytes sent in %d seconds (%d bytes/sec)", fileStream.str().length(), 
				endTime - startTime, fileStream.str().length() / (endTime-startTime) );
		}
		else
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				"%d bytes received in < 1 second", fileStream.str().length() ) ;
		}
	}
	
	void FTPClient::closeConnection()
	{
		try
		{
			std::string logoutCommand( FTP_COMMAND_LOGOUT );

			sendCommand( logoutCommand );

			FTPReplyList logoutReplies;
			receiveReplies( logoutReplies, 30 );
			// no need to process replies
		}
		catch( TcpSocketException& )
		{
			// The server may have closed the control connection while
			// we are attempting a read operation. Ignore exceptions
			// generated by the sockets subsystem
		}
		m_controlConnection->close();
		m_dataConnection->close();
	}


	void FTPClient::setPassiveMode( std::string& serverDataAddress, std::string& serverDataPort )
	{
		// Format command and send
		std::string passiveModeCommand( FTP_COMMAND_PASSIVEMODE );

		sendCommand( passiveModeCommand );

		// Receive reply and process for positive completion
		FTPReplyList passiveModeReplies;
		receiveReplies( passiveModeReplies, 30 );

		FTPReply::FTPReplyType passiveModeReplyType;
		std::string passiveModeReplyString;
		processReplyList( passiveModeReplies, passiveModeReplyType, passiveModeReplyString );

		if ( passiveModeReplyType != FTPReply::Positive_Completion )
		{
			std::string exceptionString( "Unexpected reply received after passive mode command: " );
			exceptionString.append( passiveModeReplyString );
			TA_THROW( FTPException( exceptionString.c_str() ) );
		}

		//
		// Passive mode successful. Parse reply for the address and port number that the data connection
		// should be made to.
		// 
		// The format is (h1,h2,h3,h4,p1,p2) where h1-4 are the address and p1-2 are the port
		//
		std::string::size_type leftBracketPos = passiveModeReplyString.find_last_of( "(" );
		if ( leftBracketPos == std::string::npos )
		{
			TA_THROW( FTPException( "Unable to parse passive mode reply - no left bracket found" ) );
		}
		std::string::size_type rightBracketPos = passiveModeReplyString.find_first_of( ")", leftBracketPos );
		if ( rightBracketPos == std::string::npos )
		{
			TA_THROW( FTPException( "Unable to parse passive mode reply - no right bracket found" ) );
		}

		// Seperate the bracketed string into the six integer values
		int serverAddress1, serverAddress2, serverAddress3, serverAddress4, serverPort1, serverPort2;

		sscanf( passiveModeReplyString.substr( leftBracketPos, rightBracketPos ).c_str(),
				"(%d,%d,%d,%d,%d,%d)", &serverAddress1, &serverAddress2, &serverAddress3, &serverAddress4, 
				&serverPort1, &serverPort2 );

		// Form the IP address to connect to, using the first four integers
		char passiveAddress[24] = {0};
		sprintf( passiveAddress, "%d.%d.%d.%d", serverAddress1, serverAddress2, serverAddress3, serverAddress4 );
		serverDataAddress.assign( passiveAddress );

		// Form the port to connect to, by shifting the first integer left by 8 bits and adding the second integer
		unsigned int passivePort = ( serverPort1 << 8 ) + serverPort2;
		char portAsChar[12] = {0};
		sprintf( portAsChar, "%d", passivePort );
		serverDataPort.assign( portAsChar );
	}
			


	void FTPClient::sendCommand( std::string& command )
	{
		TA_ASSERT( command.length() > 0, "Empty command passed to sendCommand()" );
		TA_ASSERT( m_controlConnection != NULL, "Control connection is uninitialised" );

		if ( ! m_controlConnection->stillConnected() )
		{
			TA_THROW( FTPException( "sendCommand(): Control connection lost" ) );
		}

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Sent: %s", command.c_str() );

		// Write command to control connection
		m_controlConnection->write( command.c_str(), command.length() );
		// Write command end sequence to control connection
		m_controlConnection->write( TELNET_EOL_SEQUENCE, strlen( TELNET_EOL_SEQUENCE ) );
	}


	void FTPClient::receiveReplies( FTPReplyList& replies, unsigned int timeoutSeconds )
	{
		TA_ASSERT( m_controlConnection != NULL, "Control connection is uninitialised" );

		char receiveBuffer[FTP_RECV_BUFFERSIZE];
		int bytesReceivedSoFar = 0;

		// Calculate time limit (if applicable)
		time_t timeLimit;
		if ( timeoutSeconds > 0 )
		{
			timeLimit = time( NULL ) + timeoutSeconds;
		}
				
		//
		// Loop until final reply received or time limit reached (if applicable)
		//
		while ( timeoutSeconds == 0 || time( NULL ) < timeLimit ) 
		{
			unsigned int bytesReceived = 0;

			// Attempt to read data from control connection
			m_controlConnection->read( receiveBuffer + bytesReceivedSoFar, 	&bytesReceived, FTP_RECV_BUFFERSIZE - bytesReceivedSoFar );
			
			bytesReceivedSoFar += bytesReceived;

			// Anything in the buffer to process ?
			if ( bytesReceivedSoFar > 0 )
			{
				// Locate the Telnet End Of Line sequence
				char* eolPosition = strstr( receiveBuffer, TELNET_EOL_SEQUENCE );
				if ( eolPosition > 0 )
				{
					// Found Telnet EOL - create a new FTPReply object and add to the list
					FTPReply* newReply = new FTPReply( receiveBuffer );
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Received: %s", newReply->logReply().c_str() );
					replies.push_back( newReply );	

					// Is this the final reply ?
					if ( newReply->isLastReply() )
					{
						return;
					}
	
					// This pointer marks the end of the reply
					char* endOfReply = eolPosition + strlen( TELNET_EOL_SEQUENCE );

					// Is there more data remaining after the reply ?
					if ( ( receiveBuffer + bytesReceivedSoFar ) > endOfReply )
					{
						//
						// Yes - move the remaining data to the front of the buffer and move
						// the bytesReceivedSoFar marker so any more received data is appended
						// to the remainders 
						//
						unsigned int remainingBytes = ( receiveBuffer + bytesReceivedSoFar ) - endOfReply;
						memmove( receiveBuffer, endOfReply, remainingBytes );
						bytesReceivedSoFar = remainingBytes;
					}
					else
					{
						// 
						// No - clear buffer and move marker to beginning of buffer
						//
						memset( receiveBuffer, 0, FTP_RECV_BUFFERSIZE );
						bytesReceivedSoFar = 0;
					}
				}
			}

			// Sleep a little while and retry the receive
			TA_Base_Core::Thread::sleep( 5 );
		}
			
		// Timeout reached without receiving final reply
		TA_THROW( FTPException( "Timed out waiting for response" ) );
	}


	void FTPClient::processReplyList( FTPReplyList& replies, FTPReply::FTPReplyType& lastReplyType, std::string& lastReplyString )
	{
		FTPReplyList::iterator replyIter = replies.begin();
		for( ; replyIter != replies.end(); ++replyIter )
		{
			FTPReply* thisReply = *replyIter;
			lastReplyType = thisReply->getType();
			lastReplyString.assign( thisReply->logReply() );

			delete thisReply;
		}

		if ( lastReplyType == FTPReply::Negative_Transient || lastReplyType == FTPReply::Negative_Permanent )
		{
			TA_THROW( FTPException( lastReplyString ) );
		}
	}

	void FTPClient::execSimpleCommand( std::string& command )
	{
		sendCommand( command );

		FTPReplyList replies;
		receiveReplies( replies, 30 );

		FTPReply::FTPReplyType replyType;
		std::string replyString;
		processReplyList( replies, replyType, replyString );

		if ( replyType != FTPReply::Positive_Completion )
		{
			std::string exceptionString( "Unexpected reply received after sending delete command: " );
			exceptionString.append( command );
			TA_THROW( FTPException( exceptionString.c_str() ) );
		}
	}

}
