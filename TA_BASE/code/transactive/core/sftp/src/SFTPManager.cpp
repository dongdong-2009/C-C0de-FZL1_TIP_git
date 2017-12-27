/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/ftp/src/FTPManager.cpp $
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

#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
//#include "curl/curl.h"
#include "core/sftp/src/SFTPManager.h"
#include "core/sftp/src/SFTPException.h"
#include "core/utilities/src/RunParams.h"

#ifndef _WIN32
	#include <pwd.h>
	#include <unistd.h>
#else	
	#include "../cots/VisualCpp/Include/direct.h"
	#include <Windows.h>
	#include <tchar.h>
#endif

#ifdef MAX_PATH
	#define MAX_TEMPFILE_PATH MAX_PATH
#else
	#define MAX_TEMPFILE_PATH 1024
#endif

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::TransactiveException;


namespace TA_Base_Core
{
	SFTPManager* SFTPManager::s_singleton = NULL;
	TA_Base_Core::ReEntrantThreadLockable	SFTPManager::s_singletonLock;
	
	
	struct WriteThis {
		const char *readptr;
		int sizeleft;
	};

	/**
	 * read_callback
	 *
	 * if use CURLOPT_READFUNCTION macro in curl_easy_setopt to read file/text/message in sendFile(), we need this callback function.
	 * however, we didn't use it right now.
	 *
	 * @param	ptr		system define
	 * @param size	system define
	 * @param nmemb	system define
	 * @param userp	user data, use CURLOPT_READDATA macro to set it
	 */
	static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
	{
		struct WriteThis *pooh = (struct WriteThis *)userp;

		if(size*nmemb < 1)
		{
			return 0;
		}

		if(pooh->sizeleft)
		{
			*(char *)ptr = pooh->readptr[0];	// copy one single byte 
			pooh->readptr++;						// advance pointer 
			pooh->sizeleft--;						    // less data left
			return 1;										// we return 1 byte at a time! 
		}

		return 0;											// no more data left to deliver 
	}

	/**
	 * createRandomFileInCurrentDir
	 *
	 * create random file in current directory, use current time and 8 random numbers(letter and digit) to name a file.
	 *
	 * @return	std::string - the random file name, include the path.
	 *
	 */
	std::string createRandomFileInCurrentDir()
	{
		time_t now_time = time(NULL);
		char time_buf[32];
		strftime(time_buf,sizeof(time_buf),"%Y%m%d%H%M%S",localtime(&now_time));
		std::string time_str(time_buf);


		const int RANDOM_LEN = 62; // 26 + 26 + 10
		char g_arrCharElem[RANDOM_LEN] = {'0','1', '2','3','4','5','6','7','8','9',
			'A', 'B', 'C','D', 'E', 'F','G', 'H', 'I','J', 'K', 'L','M', 'N', 'O','P', 'Q', 'R','S', 'T', 'U','V', 'W', 'X','Y', 'Z',
			'a', 'b', 'c','d', 'e', 'f','g', 'h', 'i','j', 'k', 'l','m', 'n', 'o','p', 'q', 'r','s', 't', 'u','v', 'w', 'x','y', 'z'};
		int randomBufLen = 8;
		char* random_buf = new char[randomBufLen + 1];
		random_buf[randomBufLen] = '\0';
		srand((unsigned)time(0));
		int iRand = 0;
		for (int i = 0; i < randomBufLen; ++i)
		{
			iRand = rand() % RANDOM_LEN;            // iRand = 0 - 61
			random_buf[i] = g_arrCharElem[iRand];
		}
		std::string randomStr(random_buf);


		const int CURRENTPATH_LEN = 256;
		char currentDir_buf[CURRENTPATH_LEN];
		getcwd(currentDir_buf, CURRENTPATH_LEN);
		std::string currentDirStr(currentDir_buf);


		std::string fileStr = currentDirStr + '\\' + time_str + "_" + randomStr + ".tmp";

		return fileStr;
	}

	/**
	 * createRandomTempFileWithAPI
	 *
	 * create random file in OS temp directory, use Win32 API 'GetTempPath/GetTempFileName' or UNIX API 'tmpnam' to create random temp file.
	 *
	 * @return	std::string - the random file name, include the path.
	 *
	 */
	std::string createRandomTempFileWithAPI()
	{
		char outputPath[MAX_TEMPFILE_PATH];

		#ifdef _WIN32
			int pathStrrlen = GetTempPath(MAX_TEMPFILE_PATH, outputPath);
		#endif

		char outputFileNameBuf[MAX_TEMPFILE_PATH];

		time_t now_time = time(NULL);
		char time_buf[32];
		strftime(time_buf,sizeof(time_buf),"%S%M%H%d%m%Y",localtime(&now_time));
		std::string time_str(time_buf);
		//printf("time_str: %s\n\n\n",time_str.c_str());

		#ifdef _WIN32
			GetTempFileNameA(outputPath,_TEXT(time_str.c_str()),0,outputFileNameBuf);
			//printf("WIN32: Tempfile path and name: %s\n\n\n",outputFileNameBuf);
		#else 
			strcpy(outputFileNameBuf,tmpnam(outputPath));
			//printf("UNIX: Tempfile path and name: %s\n\n\n",outputFileNameBuf);
		#endif

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Tempfile path and name: %s",outputFileNameBuf);

		std::string outputTempFileNameStr = outputFileNameBuf ;

		return outputTempFileNameStr;
	}

	/**
	 * deleteTempFile
	 *
	 * delete the specified file, use Win32 API 'DeleteFile' or c function 'remove' to create random temp file.
	 *
	 * @param tempFileName	the specified file, include full path.
	 *
	 * @return	int - 0 for deleted successful, others for not.
	 *
	 */
	int deleteTempFile(std::string &tempFileName)
	{
#ifndef _WIN32
		if( remove( tempFileName.c_str() ) == -1 )
		{
			//printf("UNIX: Could not delete temp file\n\n\n");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not delete temp file: %s", tempFileName.c_str() );
			return -1;
		}
		else
		{
			//printf("UNIX: Already delete temp file\n\n\n");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Deleted: %s", tempFileName.c_str() );
		}
#else
		if (DeleteFile(TEXT(tempFileName.c_str())) == 0) 
		{ 
			// It always be here. why? 
			DWORD err = GetLastError(); 
			if (err == ERROR_SHARING_VIOLATION) 
			{ 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ERROR_SHARING_VIOLATION.");
			} 
			else 
			{ 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Other error: %d.",err);
			}
			return -1;
		}
#endif
		return 0;
	}

	SFTPManager& SFTPManager::getInstance()
	{
    	if ( s_singleton == NULL )
        {
        	// Double locking to ensure that there is only one
            ThreadGuard guard( s_singletonLock );

            if ( s_singleton == NULL )
            {
           	 	s_singleton = new SFTPManager();
            }
        }
        return *s_singleton;
	}

	size_t SFTPManager::write_data(void *buffer, size_t size, size_t nmemb, void *userp)
	{
		 std::vector<unsigned char>* th=static_cast< std::vector<unsigned char>*>(userp);
		size_t added=size*nmemb;		

		unsigned char* pChar =reinterpret_cast<unsigned char *>( buffer);
		for(size_t i=0; i < added ; ++i)
		{
			th->push_back( *(pChar + i));
		}
		return added;
	}

	int   SFTPManager::curl_sshkeycallback(CURL *easy, const struct curl_khkey *knownkey,  const struct curl_khkey *foundkey, enum curl_khmatch, void *clientp)
	{
		return CURLKHSTAT_FINE_ADD_TO_FILE;
	};

	void SFTPManager::retrieveFile( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
					   			   const std::string& fileLocation, std::vector<unsigned char>& fileBuffer, unsigned int hostPort,
								   FTPClient::FileType fileType ) 
	{
	

		// Parse fileName for directory and file name
		std::string fileName, fileDirectory;
		std::string::size_type slashPos = fileLocation.find_last_of( "/" );
		if ( slashPos != std::string::npos )
		{
			//
			// There is directory information present as well - seperate from the
			// actual file name
			//
			fileDirectory.assign( fileLocation.substr( 0, slashPos ) );
			fileName.assign( fileLocation.substr( slashPos + 1, std::string::npos ) );
		}
		else
		{
			fileName.assign( fileLocation );
		}
		std::string remoteURL;
		std::stringstream ss ;
		ss << hostPort;
		std::string port = ss.str();
		remoteURL = "sftp://" + hostAddress +  ":" + port +  "/" + fileDirectory + "/" + fileName;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "remoteURL: %s", remoteURL.c_str());
		CURL*  curl = curl_easy_init();

		if(curl)
		{
			
				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
				//curl_easy_setopt(curl,CURLOPT_SSH_AUTH_TYPES,CURLSSH_AUTH_PASSWORD);
				curl_easy_setopt(curl,CURLOPT_SSH_AUTH_TYPES,CURLSSH_AUTH_PUBLICKEY );
				this->setSshKeys(curl);

				curl_easy_setopt(curl,CURLOPT_URL, remoteURL.c_str());

				setCurlConnectionTimeout(curl);

				std::stringstream ssUserPwd; 
				ssUserPwd << userName << ":" << userPassword; 
				curl_easy_setopt(curl,CURLOPT_USERPWD, ssUserPwd.str().c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

				std::vector<unsigned char>* pBuffer = &fileBuffer;
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, pBuffer);		

				CURLcode res; 

				res = curl_easy_perform(curl);
				if(res !=0 )
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "curl error code %d", res);
				}				 
				curl_easy_cleanup(curl);		
		}
		else
		{			
			TA_THROW( SFTPException( "Failed to initialize curl!" ) );
		}
	}

	void SFTPManager::setCurlConnectionTimeout(CURL* curl)
	{
		// Add a parameter to control the connection time out of sftp.
		// Get that value from runtime parameter, and give it a default value 5 secondsif the parameter is not there.
		std::string connectTimeoutS = TA_Base_Core::RunParams::getInstance().get(RPARAM_CURL_OPT_CONNECT_TIMEOUT);
		long connectTimeoutL = 5L;
		if ( !connectTimeoutS.empty() )
		{
			connectTimeoutL = strtol(connectTimeoutS.c_str(), '\0', 16);
		}
		curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,connectTimeoutL);
	}

	void SFTPManager::sendFile( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
				const std::string& srcFileLocation, const  std::string& dstFileLocation, 
				unsigned int hostPort, FTPClient::FileType fileType )
	{
		std::string remoteURL;

		// Parse fileName for directory and file name
		std::string fileName, fileDirectory;
		std::string::size_type slashPos = dstFileLocation.find_last_of( "/" );
		if ( slashPos != std::string::npos )
		{
			//
			// There is directory information present as well - seperate from the
			// actual file name
			//
			fileDirectory.assign( dstFileLocation.substr( 0, slashPos ) );
			fileName.assign( dstFileLocation.substr( slashPos + 1, std::string::npos ) );
		}
		else
		{
			fileName.assign( dstFileLocation );
		}
		std::stringstream ss ;
		ss << hostPort;
		std::string port = ss.str();
		remoteURL = "sftp://" + hostAddress +  ":" + port +  "/" + fileDirectory + "/" + fileName;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "remoteURL: %s", remoteURL.c_str());
		CURL*  curl = curl_easy_init();

		if(curl)
		{
		

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "source file: %s", srcFileLocation.c_str());
			FILE  * hd_src = fopen(srcFileLocation.c_str(), "rb");
			char error_buf[CURL_ERROR_SIZE] = {'\0'};
			if(hd_src == NULL)
			{
				if ( EOF == fclose(hd_src))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "open file fail, and close the file handle fail.");
				}
				curl_easy_cleanup(curl);
				
				TA_THROW(SFTPException( "Invalid source file!" ) );
			}
			else
			{	
				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

				setCurlConnectionTimeout(curl);

				//curl_easy_setopt(curl,CURLOPT_SSH_AUTH_TYPES,CURLSSH_AUTH_PASSWORD);
				curl_easy_setopt(curl,CURLOPT_SSH_AUTH_TYPES,CURLSSH_AUTH_PUBLICKEY );
				this->setSshKeys(curl);

				curl_easy_setopt(curl,CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);
				curl_easy_setopt(curl,CURLOPT_URL, remoteURL.c_str());
				
				std::stringstream ssUserPwd; 
				ssUserPwd << userName << ":" << userPassword; 
				curl_easy_setopt(curl,CURLOPT_USERPWD, ssUserPwd.str().c_str());				
				curl_easy_setopt(curl,CURLOPT_READDATA,hd_src);
				
				struct stat file_info;
				curl_off_t fsize;
				stat(srcFileLocation.c_str(),&file_info);
				fsize = (curl_off_t) file_info.st_size;

				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
					(curl_off_t)fsize);

				 CURLcode res; 

				 res = curl_easy_perform(curl);
				 if(res !=0 )
				 {
					 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "curl error code %d", res);
					 std::string errmsg(error_buf);

					 if ( EOF == fclose(hd_src))
					 {
						 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "open file success, perform upload file fail, and close the file handle fail.");
					 }
					 curl_easy_cleanup(curl);
					 TA_THROW( SFTPException( "ERROR connecting to sftp: "+errmsg ) );
				 }				 
				 curl_easy_cleanup(curl);
			}
			fclose(hd_src);
		}
		else
		{			
			TA_THROW( SFTPException( "Failed to initialize curl!" ) );
		}
	}

	void SFTPManager::sendFile(const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
		const unsigned char* buffer, const int dataIndex, const int length, const std::string& dstFileLocation,
		unsigned int hostPort, FTPClient::FileType fileType)
	{
		// create a temp file to store the text/message.
		m_randomTempFile = createRandomTempFileWithAPI();

		int hd_src_length = sizeof(char) * length;
		unsigned char *hd_src_data = (unsigned char*)malloc(hd_src_length + 1);
		memcpy( hd_src_data, buffer + dataIndex, length );
		hd_src_data[hd_src_length] = '\0';

		std::ofstream out(m_randomTempFile.c_str()); 
		out<<hd_src_data;
		out.close();

		// call origin sendFile interface using the temp file as the source file.
		sendFile(hostAddress,userName,userPassword,m_randomTempFile,dstFileLocation,hostPort);

		// after use the temp file, need to delete it.
		if( 0 == deleteTempFile(m_randomTempFile))
		{
			//printf("Delete File after send file\n\n\n");
			m_randomTempFile.empty();
			m_randomTempFile = "";
		}

		delete hd_src_data;
		hd_src_data = NULL;
	}


	SFTPManager::SFTPManager()
	{
		//m_randomTempFile = "";
		// Nothing yet	
		 curl_global_init(CURL_GLOBAL_ALL);
	}


	SFTPManager::~SFTPManager()
	{
		if ( m_randomTempFile != "")
		{
			//printf("Delete File in Destructor\n\n\n");
			#ifndef _WIN32
				if( remove( m_randomTempFile.c_str() ) == -1 )
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not delete temp file: %s", m_randomTempFile.c_str() );
				else
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Deleted: %s", m_randomTempFile.c_str() );
			#else
				if (DeleteFile(TEXT(m_randomTempFile.c_str())) == 0) 
				{ 
					// It always be here. why? 
					DWORD err = GetLastError(); 
					if (err == ERROR_SHARING_VIOLATION) 
					{ 
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ERROR_SHARING_VIOLATION.");
					} 
					else 
					{ 
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Other error: %d.",err);
					} 
				}
			#endif
		}
		// Nothing yet
		 curl_global_cleanup();
	}

	void SFTPManager::setSshKeys(CURL* curl)
	{

		std::string pubKey("/.ssh/id_rsa.pub");
		std::string pvtKey("/.ssh/id_rsa");
		std::string knowHosts("/.ssh/know_hosts");

		static std::string home = ""; 

#ifdef _WIN32
		char* profile = getenv("USERPROFILE");
		if(profile != NULL)
		{
			home = profile;
		}
		else
		{
			 char* szHomePath = getenv("HOMEPATH");
			 char* szHomeDrive = getenv("HOMEDRIVE");
			 if(szHomePath != NULL &&  szHomeDrive != NULL)
			 {
				 std::string path(szHomePath);
				 std::string drive(szHomeDrive);
				home = drive + path; 
			 }
		}
#else
		if(home == "")
		{
			const char *h = getenv("HOME");
			if (h != 0)
				home = h;
		}
		// Try using passwd entry
		if (home == "")
		{
			struct passwd *pw = 0;

			const char *user = getenv("USER");
			if (user == 0)
				user = getenv("LOGNAME");

			if (user != 0)
				pw = getpwnam(user);
			if (pw == 0)
				pw = getpwuid(getuid());
			if (pw != 0)
				home = pw->pw_dir;
		}

		// Revert to current dir
		if (home == "")
			home = ".";

#endif
		
		pubKey= home + pubKey;
		pvtKey = home + pvtKey;
		knowHosts = home + knowHosts;

		curl_easy_setopt(curl,CURLOPT_SSH_PUBLIC_KEYFILE,pubKey.c_str());
		curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE,pvtKey.c_str());
		curl_easy_setopt(curl, CURLOPT_SSH_KNOWNHOSTS, knowHosts.c_str());
		curl_easy_setopt(curl,CURLOPT_SSH_KEYFUNCTION,curl_sshkeycallback);
	}
	void SFTPManager::execSimpleCommand( const std::string& hostAddress, const std::string& userName, const std::string& userPassword,
		std::string& command, std::string & directoryLocation, unsigned int hostPort, FTPClient::FileType fileType )
	{
	
		std::string remoteURL;

		std::stringstream ss ;
		ss << hostPort;
		std::string port = ss.str();
		remoteURL = "sftp://" + hostAddress +  ":" + port +  "/" + directoryLocation ;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "remoteURL: %s", remoteURL.c_str());
		CURL*  curl = curl_easy_init();

		if(curl)
		{
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			//curl_easy_setopt(curl,CURLOPT_SSH_AUTH_TYPES,CURLSSH_AUTH_PASSWORD);
			curl_easy_setopt(curl,CURLOPT_SSH_AUTH_TYPES,CURLSSH_AUTH_PUBLICKEY );
			this->setSshKeys(curl);			
			curl_easy_setopt(curl,CURLOPT_NOBODY,true);
			curl_easy_setopt(curl,CURLOPT_URL,remoteURL.c_str());

			setCurlConnectionTimeout(curl);
			

			std::stringstream ssUserPwd; 
			ssUserPwd << userName << ":" << userPassword; 
			curl_easy_setopt(curl,CURLOPT_USERPWD, ssUserPwd.str().c_str());			

			struct curl_slist *slist = 0;			// post request
			slist = curl_slist_append(slist,command.c_str());

			curl_easy_setopt(curl,CURLOPT_POSTQUOTE,slist);

			CURLcode res = curl_easy_perform(curl);
			if(res !=0 )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "curl error code %d", res);
			}				 
			curl_slist_free_all(slist);
			curl_easy_cleanup(curl);
		}
		else
		{
			TA_THROW( SFTPException( "Failed to initialize curl!" ) );
		}
	}
}
