#ifndef FTPCLIENT_H
#define FTPCLIENT_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/ftp/src/FTPClient.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Implementation of File Transfer Protocol client. FTPClient sends
 * commands to an FTP server and processes responses.
 * FTPClient is directed by FTPManager.
 *
 */

#include <vector>
#include <string>
#include <cstring>

#include "core/ftp/src/FTPReply.h"

namespace TA_Base_Core
{
	class FTPControlConnection;
	class FTPDataConnection;

	class FTPClient 
	{

	public:

		//
		// Data Representation types supported by FTPClient
		// Used by setFileType() function.
		//
		enum FileType
		{
			Ascii,		// for text files
			Image		// for binary files
		};
			
		typedef std::vector<FTPReply*> FTPReplyList;

		/**
		 * Constructor
		 *
		 * Performs simple initialisation. No communication operations
		 * are performed.
		 *
		 */
		FTPClient();


		/**
		 * Destructor
		 *
		 */
		~FTPClient();


		/**
		 * connectToServer()
		 *
		 * Connects to the specified FTP server and receives the welcome message.
		 * Processes the server reply to confirm that it is ready for further commands.
	 	 *
		 * @param	hostAddress		IP Address/Hostname of FTP server
		 * @param	hostPort		Listening port of FTP server, defaults to FTP default (21)
		 *
		 * @exception	FTPException if a non-positive completion reply is received from the server 
		 */
		void connectToServer( const std::string& hostAddress, unsigned int hostPort = 21 );

			
		/**
		 * userLogin()
		 *
		 * Logs into the FTP server with the specified user name and, if necessary, password.
		 *
		 * @param	userName		user name to log in with
		 * @param	userPassword	user password to log in with
		 *
		 * @exception	FTPException if an unexpected reply is received from the server 
		 */
		void userLogin( const std::string& userName, const std::string& userPassword );


		/**
		 * setFileType()
		 *
		 * Sets the data representation type for the FTP session. Sends the appropriate
		 * command to the server and processes the reply for confirmation of the type change.
		 *
		 * @param	type	the desired data representation type (see enum definition above)
		 *
		 * @exception	FTPException if a non-positive completion reply is received from the server 
		 */
		void setFileType( FileType type );

		
		/**
		 * changeDirectory()
		 *
		 * Changes the server side directory. Issues the appropriate command to
		 * the server and processes the reply for confirmation of the directory change.
		 *
		 * @param	newDirectory 	the directory to change to, either absolute or relative
		 *							to the current directory.
		 *
		 * @exception	FTPException if a non-positive completion reply is received from the server 
		 */
		void changeDirectory( const std::string& newDirectory );


		/**
		 * retrieveFile()
		 *
		 * Requests the server enter passive mode (in this mode, the client initiates the data
		 * connection, contrary to normal FTP operation). After receiving and confirming the
		 * successful passive mode operation, the data connection is connected and the file
		 * retrieval command sent. The server will commence sending the file via the data connection.
		 * FTPClient waits for the server to send a reply. Upon receipt of a successful reply, the
		 * data connection waits until it's connection is closed by the server (signalling the end
		 * of the transfer). 
		 *
		 * @param	fileName		the name of the file to retrieve, relative to the current
		 *							working directory.
		 * @param	fileBuffer		a vector of bytes to store the retrieved file 
		 *
		 * @exception	FTPException if a non-positive completion reply is received from the server 
		 */
		void retrieveFile( const std::string& fileName, std::vector<unsigned char>& fileBuffer );


		/**
		 * sendFile()
		 *
		 * Requests the server enter passive mode (in this mode, the client initiates the data
		 * connection, contrary to normal FTP operation). After receiving and confirming the
		 * successful passive mode operation, the data connection is connected and the file
		 * retrieval command sent. The server will commence sending the file via the data connection.
		 * FTPClient waits for the server to send a reply. Upon receipt of a successful reply, the
		 * data connection waits until it's connection is closed by the server (signalling the end
		 * of the transfer). 
		 *
		 * @param	fileName		the name of the file to send, relative to the current
		 *							working directory.
		 * @param	fileBuffer		a vector of bytes stored the file to be sent
		 *
		 * @exception	FTPException if a non-positive completion reply is received from the server 
		 */
		void sendFile( const std::string& fileName, std::stringstream& fileBuffer );

		void execSimpleCommand( std::string& command );
		/**
		 * closeConnection()
		 *
		 * Issues the FTP QUIT command to the server, receives any replies and closes both
		 * the data and control connections. No processing is done on the reply(s) received.
		 *
		 */
		void closeConnection();


	private:

		/**
		 * sendCommand()
		 *
		 * Writes a command to the control connection, followed by the Telnet EOL
		 * sequence, as required by the File Transfer Protocol.
		 *
		 * @param	command		the command to write to the server, without the Telnet EOL
		 *
		 */
		void sendCommand( std::string& command );


		/**
		 * receiveReplies()
		 *
		 * Reads from the control connection, until the final reply is received or
		 * the timeout is reached (if specified). The Telnet EOL sequence is used
		 * to seperate received data into lines (each represented by a FTPReply
		 * object). The logic for determining whether a line is the last in the
		 * sequence, is held in the FTPReply class.
		 *
		 * @param	replies			all FTPReply objects created will be stored in this 
		 *							vector.
		 * @param	timeoutSeconds	an optional time limit to receive the final reply.
		 *							Specifying a value of 0 disables the time limit.
		 */
		void receiveReplies( FTPReplyList& replies, unsigned int timeoutSeconds );


		/**
		 * processReplyList()
		 *
		 * Given a list of replies received from the FTP server, the function 
		 * determines the overall result ('reply type' as defined by the FTP RFC)
		 * and the last reply string (used for further processing by some functions).
		 * All replies in the list are deleted prior to processing.
		 *
		 * @param	replyList		the list of replies to process
		 * @param	lastReplyType	the overall result of the replies will be written to this variable
		 * @param	lastReplyString	the string from the last reply will be written to this variable
		 *
		 * @exception	FTPException	if the overall result is of the Negative_Transient or 
		 *								Negative_Permanent type.
		 */
		void processReplyList( FTPReplyList& replyList, FTPReply::FTPReplyType& lastReplyType, std::string& lastReplyString );


		/**
		 * setPassiveMode()
		 *
		 * Requests the server enter passive mode, where the data connection is initiated
		 * by the client not the server. Formats and sends the command, receives the reply.
		 * The function parses the reply for the IP address and port number to connect to
		 * for the data connection.
		 *
		 * @param serverDataAddress		the IP address for the data connection will be written to this variable
		 * @param serverDataPort		the port for the data connection will be written to this variable
		 *
		 * @exception	FTPException if a non-positive completion reply is received from the server 
		 */
		void setPassiveMode( std::string& serverDataAddress, std::string& serverDataPort );

		
		FTPControlConnection*		m_controlConnection;
		FTPDataConnection*			m_dataConnection;
	};
}

#endif // FTPCLIENT_H
