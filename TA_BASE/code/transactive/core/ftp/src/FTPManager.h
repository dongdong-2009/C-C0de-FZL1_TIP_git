#ifndef FTPMANAGER_H
#define FTPMANAGER_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/ftp/src/FTPManager.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Presents the FTP Interface user with a simple, one command
 * method of performing FTP tasks. Directs an FTPClient instance
 * through the necessary steps. 
 *
 */

#include <string>

#include "core/ftp/src/FTPClient.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class FTPManager
	{

	public:

		/**
		 * getInstance()
		 *
		 * FTPManager is a singleton class, accessed via this method.
		 * This function is double locked, to provide safe
		 * multithreaded access.
		 *
		 */
		static FTPManager& getInstance();


		/**
		 * retrieveFile()
		 *
		 * Connects, logs in and requests a file from the FTP server.
		 *
		 * @param	hostAddress		IP address/hostname of FTP server
		 * @param	userName		User name to log into server with
		 * @param	userPassword	User password to log into server with
		 * @param	fileLocation	file to retrieve, including path relative to server's root dir
		 * @param	fileBuffer		vector of bytes to store retrieved file in
		 * @param	hostPort		port number that FTP server is listening on (default:21)
		 * @param	fileType		type of file being retrieved (default:Image) - defined in FTPClient.h
		 *
		 * @exception	FTPException	all exceptions rethrown as FTPExceptions
		 *
		 */
		void retrieveFile( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
						   const std::string& fileLocation, std::vector<unsigned char>& fileBuffer, 
						   unsigned int hostPort = 21, FTPClient::FileType fileType = FTPClient::Image );


		/**
		 * sendFile()
		 *
		 * Connects, logs in and requests a file from the FTP server.
		 *
		 * @param	hostAddress		IP address/hostname of FTP server
		 * @param	userName		User name to log into server with
		 * @param	userPassword	User password to log into server with
		 * @param	srcFileLocation	local file to be sent, including path
		 * @param	dstFileLocation	remote file to be stored, including path
		 * @param	hostPort		port number that FTP server is listening on (default:21)
		 * @param	fileType		type of file being retrieved (default:Image) - defined in FTPClient.h
		 *
		 * @exception	FTPException	all exceptions rethrown as FTPExceptions
		 *
		 */
		void sendFile( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
							const std::string& srcFileLocation, const  std::string& dstFileLocation, 
							unsigned int hostPort = 21, FTPClient::FileType fileType = FTPClient::Ascii );

		void execSimpleCommand( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
							std::string& command, std::string & directoryLocation, unsigned int hostPort = 21, FTPClient::FileType fileType = FTPClient::Ascii );

	private:

		/**
		 * Constructor
		 *
		 * Private constructor only accessible via getInstance()
		 *
		 */
		FTPManager();


		/**
		 * Destructor
		 *
		 */
		~FTPManager();


		static FTPManager*			s_singleton;
		static TA_Base_Core::ReEntrantThreadLockable		s_singletonLock; // Protects MT access to constructor
	};
}


#endif // FTPMANAGER_H
