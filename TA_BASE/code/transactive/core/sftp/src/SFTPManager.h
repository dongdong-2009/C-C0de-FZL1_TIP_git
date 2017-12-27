#ifndef SFTPMANAGER_H
#define SFTPMANAGER_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/sftp/src/SFTPManager.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2009/12/29 14:17:48 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Presents the FTP Interface user with a simple, one command
 * method of performing FTP tasks. Directs an FTPClient instance
 * through the necessary steps. 
 *
 */

#include <string>


#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/ftp/src/FTPClient.h"

#include "curl/curl.h"

#define RPARAM_CURL_OPT_CONNECT_TIMEOUT			"CurlOptConnectTimeout"

namespace TA_Base_Core
{
	class SFTPManager
	{

	public:

		/**
		 * getInstance()
		 *
		 * SFTPManager is a singleton class, accessed via this method.
		 * This function is double locked, to provide safe
		 * multithreaded access.
		 *
		 */
		static SFTPManager& getInstance();
		static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
		static int   curl_sshkeycallback(CURL *easy, const struct curl_khkey *knownkey,  const struct curl_khkey *foundkey, enum curl_khmatch, void *clientp);


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
		 * @exception	SFTPException	SFTPException as defined in core/exceptions/src/TransactiveException.h
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
		 * @exception	SFTPException	SFTPException as defined in core/exceptions/src/TransactiveException.h
		 *
		 */
		void sendFile( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
							const std::string& srcFileLocation, const  std::string& dstFileLocation, 
							unsigned int hostPort = 21, FTPClient::FileType fileType = FTPClient::Ascii );

		/**
		 * sendFile()
		 *
		 * Connects, logs in and requests a file from the FTP server.
		 *
		 * @param	hostAddress		IP address/hostname of FTP server
		 * @param	userName		User name to log into server with
		 * @param	userPassword	User password to log into server with
		 * @param	buffer 	local message to be sent, message may come from sataic text, or file
		 * @param	dataIndex 	indicate the buffer start from which character of the message
		 * @param	length 	the length of the message to be sent
		 * @param	dstFileLocation	remote file to be stored, including path
		 * @param	hostPort		port number that FTP server is listening on (default:21)
		 * @param	fileType		type of file being retrieved (default:Image) - defined in FTPClient.h
		 *
		 * @exception	SFTPException	SFTPException as defined in core/exceptions/src/TransactiveException.h
		 *
		 */
		void sendFile( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
			 const unsigned char * buffer, const int dataIndex, const int length, const  std::string& dstFileLocation, 
							unsigned int hostPort = 21, FTPClient::FileType fileType = FTPClient::Ascii );

		void execSimpleCommand( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
							std::string& command, std::string & directoryLocation, unsigned int hostPort = 21, FTPClient::FileType fileType = FTPClient::Ascii );

		//size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp) ;
		//int my_curl_easy_perform(CURL* curl, BodyStruct* body, FILE* f);

	private:

		void setSshKeys(CURL* curl);

		void setCurlConnectionTimeout(CURL* curl);
		/**
		 * Constructor
		 *
		 * Private constructor only accessible via getInstance()
		 *
		 */
		SFTPManager();


		/**
		 * Destructor
		 *
		 */
		~SFTPManager();

		//size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp) ;

		static SFTPManager*			s_singleton;
		static TA_Base_Core::ReEntrantThreadLockable		s_singletonLock; // Protects MT access to constructor

		std::string m_randomTempFile;
	};
}


#endif // SFTPMANAGER_H
